/*
 * main.c
 *
 *  Created on: 24.09.2013
 *      Author: einball
 */



#include "main.h"

#include "config_AIC23.h"
#include "config_MCBSP.h"

int main(void) {

	/* Initialize the CSL and the CPLD */
	CSL_init();
	DSK6713_init();
	DSK6713_LED_init();

	/* Turn on one LED so we can see it executed at least the main function */
	DSK6713_LED_on(0);

	/* Initialize the DIP switches to be able to read them */
	DSK6713_DIP_init();


	/* Configure the codec according to the definitions in config_AIC23.c
	 * via the McBSP0 interface
	 */
	conf_AIC23();

	/* Configure the McBSP to transfer the data from and to the codec */
	conf_MCBSP();

	/* Start the MCBSP */
	start_MCBSP();

	/* Configure EDMA */
	conf_EDMA();

	/* Time to initialize the buffer and zerofill it */
	for(i = 0; i < 10; i++) FIFO_I[i] = 0;
	for(i = 0; i < 10; i++) FIFO_Q[i] = 0;


	/* Config Interrupts */
	IRQ_enable(IRQ_EVT_EDMAINT);
	IRQ_map(IRQ_EVT_EDMAINT, 8);
	IRQ_globalEnable();

	/* Enable the EDMA channels */
	EDMA_enableChannel(hEDMATrx);


	/******************************************************/
	/* We should be done here by now. The McBSP generates an
	 * Interrupt (called "event" in this case) each time
	 * there's a new word ready to be written or ready to
	 * be transferred from the serial port to the
	 * input buffer. We use it for the golden wire config
	 * and will disable the input when we throw in the
	 * QPSK modulation algorithm as it is not needed by then.
	 */
	/******************************************************/

	/* End main - RTOS takes over */
}



/*********************************************************/
/*
 * Blink_LED() is the first test function I implemented
 * to determine if the RTOS works properly and within
 * defined parameters. It remains here for continued
 * monitoring in case I break something in the process!
 * It is scheduled as a periodic function with 100ms delay
 * in the RTOS config file and takes no parameters at all!
 */
/*********************************************************/

void PER_Blink_LED(){
	DSK6713_LED_toggle(0);
	DSK6713_LED_toggle(1);

}

/*********************************************************/
/*
 * This function examines the switch positions and determines
 * where to get the data from. It takes data in 8 bit chunks,
 * splits them up in 2 bit chunks and returns them.
 */
/*********************************************************/
uint8_t HLP_getData(){
	uint8_t outVal = 0x00;

	/* This is commented out for testing. The various input methods still have to be implemented .. */
/*	uint8_t newSwitchState = DSK6713_DIP_get(1)<<1 | DSK6713_DIP_get(0);

	switch(dataSource){
	case SRC_ZERO:
		break;
	case SRC_RAND:
		break;
	case SRC_ALT:
		break;
	case SRC_TEXT:
		break;
	case SRC_GEL:
		break;
	}
*/
	bytePos +=1;
	if(bytePos > 40) bytePos = 0;
	return text[bytePos];

//	bytePos +=1;
//	bytePos = bytePos % 4;
//	if(FIFO_I[0] == 1) return 0x01;
//	return 0x00;
	//return bytePos;
}


/*********************************************************/
/*
 * This function advances the FIFO.
 */
/*********************************************************/
void HLP_UpdateFIFO(){
	uint8_t bitInput;

	//The function getData() provides abstraction from the data source.
	//It returns two bits in a char ... See what I did there? ;)
	bitInput = HLP_getData();
	bitInput = bitInput & 0x03;

	//Shift
	for(i = 9; i > 0; i--) FIFO_I[i] = FIFO_I[i-1];
	for(i = 9; i > 0; i--) FIFO_Q[i] = FIFO_Q[i-1];

	//Fill up
	if(bitInput == 0x00){
		FIFO_I[0] = 1;
		FIFO_Q[0] = 0;
	}
	else if(bitInput == 0x01){
		FIFO_I[0] = 0;
		FIFO_Q[0] = 1;
	}
	else if(bitInput == 0x02){
		FIFO_I[0] = 0;
		FIFO_Q[0] = -1;
	}
	else{
		FIFO_I[0] = -1;
		FIFO_Q[0] = 0;
	}
	//Done

}

/*********************************************************/
/* This interrupt is called when the EDMA has finished
 * copying all the data from and to the in- and output
 * buffers. The interrupt should decide what happens next,
 * i.e. posts a software interrupt that does the actual
 * processing of the data copied.
 */
/*********************************************************/


void HWI_handleEDMAInterrupt(){

		if(EDMA_intTest(tccTrxPing)) {
			EDMA_intClear(tccTrxPing);
			SWI_post(&procPing);
		}
		else if(EDMA_intTest(tccTrxPong)) {
			EDMA_intClear(tccTrxPong);
			SWI_post(&procPong);
		}


}


/*********************************************************/
/* These are software interrupts that get called whenever
 * the EDMA has finished a transfer and new data that is
 * yet to be processed recides in the buffer. These interrupts
 * have the highest priority the RTOS can assign (apart from
 * any pending hardware interrupts). They cannot be interrupted
 * and under all circumstances have to finish before the next
 * EDMA interrupt fires and destroys the data consistency.
 */
/*********************************************************/

void SWI_processPing(){
	int i,j;

	/*********************************************************/
	/* Advance the FIFO and insert a new symbol as everytime
	 * this function is called a symbol has been transmitted.
	 */
	/*********************************************************/
	HLP_UpdateFIFO();

	/*********************************************************/
	/* We have twenty-four samples in a symbol. Therefore we
	 * have to go through all of them. The outer loop takes
	 * care of that.
	 */
	/*********************************************************/
	for(i = 0; i < SAMPLES_PER_SYMBOL; i++){

		/*********************************************************/
		/* For each sample we need to set the initial array
		 * value to zero. That is because we have a filter kernel
		 * that spans over ten symbols which in turn means that we
		 * have ten filter responses per sample. The inner loop
		 * sums all responses up and stores them.
		 */
		/*********************************************************/
		iPulse[i] = 0;
		qPulse[i] = 0;


		for(j = 0; j < HAMM_WIN_SYM_LENG; j++){

			iPulse[i] = iPulse[i] + FIFO_I[j] * RCResp[j * SAMPLES_PER_SYMBOL + i];
			qPulse[i] = qPulse[i] + FIFO_Q[j] * RCResp[j * SAMPLES_PER_SYMBOL + i];

		} /* End for-loop over all responses */

	} /* End for-loop over all samples */


	/*********************************************************/
	/* Now we need to mix the IQ baseband signal we just
	 * generated to the final frequency. At 48kHz sampling
	 * rate we have four points supporting the 12kHz sinewave
	 * corresponding to the following multiplication factors.
	 * This allows us to multiply the baseband values in a smart
	 * way and copy them to the output buffer. We only take the
	 * real part and therefore we have to subtract the Quadrature
	 * channel from the Inphase channel.
	 * We have a stereo codec but only a mono plug. If we were to
	 * use only one channel, the plug would short circuit
	 * and the output would be useless. Therefore we copy the
	 * left channel to the right channel (or vice versa) to
	 * solve that problem.
	 */
	/*********************************************************/
	for(i = 0; i < SAMPLES_PER_SYMBOL; i = i + 4){

		oBufPing[2*i + 0]	= (+1) * iPulse[i + 0];		/*	I * cos(0)      - Q * sin(0)      => (+1) * I -   0  * Q 	*/
		oBufPing[2*i + 1]	= (+1) * iPulse[i + 0];
		oBufPing[2*i + 2]	= (-1) * qPulse[i + 1];		/* 	I * cos(PI/2)   - Q * sin(PI/2)   =>   0  * I - (+1) * Q	*/
		oBufPing[2*i + 3]	= (-1) * qPulse[i + 1];
		oBufPing[2*i + 4]	= (-1) * iPulse[i + 2];		/*	I * cos(PI)     - Q * sin(PI)     => (-1) * I -   0  * Q 	*/
		oBufPing[2*i + 5]	= (-1) * iPulse[i + 2];
		oBufPing[2*i + 6] 	= (+1) * qPulse[i + 3];	 	/*	I * cos(3*PI/2) - Q * sin(3*PI/2) =>   0  * I - (-1) * Q 	*/
		oBufPing[2*i + 7] 	= (+1) * qPulse[i + 3];

	} /* End for-loop to mix to the carrier frequency */

}

/* See SWI_processPing() for explanations. Same principles apply here */
void SWI_processPong(){
	int i,j;

	HLP_UpdateFIFO();

	for(i = 0; i < SAMPLES_PER_SYMBOL; i++){

		iPulse[i] = 0;
		qPulse[i] = 0;

		for(j = 0; j < HAMM_WIN_SYM_LENG; j++){

			iPulse[i] = iPulse[i] + FIFO_I[j] * RCResp[j * SAMPLES_PER_SYMBOL + i];
			qPulse[i] = qPulse[i] + FIFO_Q[j] * RCResp[j * SAMPLES_PER_SYMBOL + i];

		}

	}

	for(i = 0; i < SAMPLES_PER_SYMBOL; i = i + 4){

		oBufPong[2*i + 0]	= (+1) * iPulse[i + 0];
		oBufPong[2*i + 1]	= (+1) * iPulse[i + 0];
		oBufPong[2*i + 2]	= (-1) * qPulse[i + 1];
		oBufPong[2*i + 3]	= (-1) * qPulse[i + 1];
		oBufPong[2*i + 4]	= (-1) * iPulse[i + 2];
		oBufPong[2*i + 5]	= (-1) * iPulse[i + 2];
		oBufPong[2*i + 6] 	= (+1) * qPulse[i + 3];
		oBufPong[2*i + 7] 	= (+1) * qPulse[i + 3];

	}
}


void conf_EDMA(){
	/*
	 * Open EDMA channels to the REVT1 and XEVT1 interrupts
	 * issued from the MCBSP periphery. The channels still
	 * need to be configured properly with a parameterset
	 * and another parameterset that fills the pong Buffer.
	 */
	hEDMATrx = EDMA_open(EDMA_CHA_XEVT1, EDMA_OPEN_RESET);


	/*
	 * Aquire EDMA tables that hold the reload 	tables for both,
	 * transmit and receive side bufferswitch.
	 */
	hEDMATrxPing = EDMA_allocTable(-1);
	hEDMATrxPong = EDMA_allocTable(-1);


	/*
	 * Set the EDMA source address. On the receive side we use
	 * the MCBSP source address and save it in the EDMA config
	 * handle. For the transmit side we will have a fixed
	 * destination but a variable source address, so we need to
	 * set the destination to a fixed address!
	 */
	conf_EDMA_oBuf.dst = MCBSP_getXmtAddr(hMcBsp);



	/*
	 * The next step is to find free Transfer complete codes,
	 * save them to a variable so the EDMA hardware interrupt
	 * is able to distinguish between the RX and TX complete
	 * code. We *could* assign those as static codes, but it
	 * is better coding practice to make it variable to allow
	 * future expansion of the code.
	 */
	tccTrxPing = EDMA_intAlloc(-1);
	conf_EDMA_oBuf.opt |= EDMA_FMK(OPT, TCC, tccTrxPing);


	/*
	 * Configure the transmit and receive channel to write in the
	 * ping buffers. The function copies the struct into the
	 * EDMA parameter RAM. This allows us to reuse the configs
	 * to set up the reload configuration for the pong buffers.
	 */
	EDMA_config(hEDMATrx, &conf_EDMA_oBuf);
	EDMA_config(hEDMATrxPing, &conf_EDMA_oBuf);


	/*
	 * Now we are ready to configure the reload parametersets
	 * that will write to the pong buffers after the ping
	 * buffer is filled with data.
	 */


	/*
	 * First we need to set the destination and source of the
	 * corresponding pong buffers for both sides.
	 */
	conf_EDMA_oBuf.src = (uint32_t)oBufPong;

	/*
	 * We also need a transfer complete code for the second
	 * configuration set to show that we indeed have finished
	 * the transfer to the pong buffers and switch to ping.
	 * To overwrite the ones written before we need to
	 * overwrite the whole 32 OPT bits
	 */
	tccTrxPong = EDMA_intAlloc(-1);
	conf_EDMA_oBuf.opt =      	EDMA_FMKS(OPT, PRI, HIGH)       |
								EDMA_FMKS(OPT, ESIZE, 32BIT)    |
								EDMA_FMKS(OPT, 2DS, NO)         |
								EDMA_FMKS(OPT, SUM, INC)        |
								EDMA_FMKS(OPT, 2DD, NO)         |
								EDMA_FMKS(OPT, DUM, NONE)       |
								EDMA_FMKS(OPT, TCINT, YES)      |
								EDMA_FMKS(OPT, TCC, OF(0))      |
								EDMA_FMKS(OPT, LINK, YES)       |
								EDMA_FMKS(OPT, FS, NO);

	conf_EDMA_oBuf.opt |=		EDMA_FMK(OPT, TCC, tccTrxPong);


	/*
	 * Now we can write the pong buffer configs to the parameter RAM
	 * tables we aquired before by using EDMA_config()..
	 */
	EDMA_config(hEDMATrxPong, &conf_EDMA_oBuf);


	/*
	 * We now need to link the transfers so the EDMA fires an interrupt
	 * whenever we finished a job and immediately starts to
	 * fill the other buffer (ping -> pong -> ping).
	 */
	EDMA_link(hEDMATrx, hEDMATrxPing);
	EDMA_link(hEDMATrxPing, hEDMATrxPong);
	EDMA_link(hEDMATrxPong, hEDMATrxPing);



	/*
	 * Now we take precautions and clear all
	 * the interrupt sources so no interrupt
	 * can fire because of some wiggling bits
	 * caused by an unstable supply.
	 */
	EDMA_intClear(tccTrxPing);
	EDMA_intClear(tccTrxPong);


	/*
	 * Lets enable the interrupts, but we aren't
	 * done yet: There's still the global interrupt
	 * switch to be toggled by enable_INT() after
	 * we started the EDMA transfers.
	 */
	EDMA_intEnable(tccTrxPing);
	EDMA_intEnable(tccTrxPong);
}

