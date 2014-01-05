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

	/* Config Interrupts */
	IRQ_enable(IRQ_EVT_EDMAINT);
	IRQ_map(IRQ_EVT_EDMAINT, 8);
	IRQ_globalEnable();

	/* Enable the EDMA channels */
	EDMA_enableChannel(hEDMARcv);
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
/* This interrupt is called when the EDMA has finished
 * copying all the data from and to the in- and output
 * buffers. The interrupt should decide what happens next,
 * i.e. posts a software interrupt that does the actual
 * processing of the data copied.
 */
/*********************************************************/

void HWI_handleEDMAInterrupt(){
	g++; //How often did the interrupt fire?

	/* Debug */
	int TrxPingIntF = EDMA_intTest(tccTrxPing);
	int TrxPongIntF = EDMA_intTest(tccTrxPong);
	/* Debug end */
	if(EDMA_intTest(tccTrxPing)) {
		EDMA_intClear(tccTrxPing);
		TrxPingDone=1;
	}
	else if(EDMA_intTest(tccTrxPong)) {
		EDMA_intClear(tccTrxPong);
		TrxPongDone=1;
	}

	if(TrxPingDone) {
		TrxPingDone=0;
		// SW-Interrupt
		SWI_post(&procPing);
	}
	else if(TrxPongDone) {
		TrxPongDone=0;
		// SW-Interrupt
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
	int c;
	for(c = 0; c < 16;c++){


	}

	processping++;
	int i;
	for(i = 0; i < BUF_SIZE; ++i){
		oBufPing[i] = 0x00000000 | arr[i%4] | arr[i%4]<<16;
	}

}

void SWI_processPong(){
	processpong++;
	int i;
	for(i = 0; i < BUF_SIZE; ++i){
		oBufPong[i] = 0x00000000 | arr[i%4] | arr[i%4]<<16;
	}
}

/***********************************************************/



void conf_EDMA(){
	/*
	 * Open EDMA channels to the REVT1 and XEVT1 interrupts
	 * issued from the MCBSP periphery. The channels still
	 * need to be configured properly with a parameterset
	 * and another parameterset that fills the pong Buffer.
	 */
	hEDMARcv = EDMA_open(EDMA_CHA_REVT1, EDMA_OPEN_RESET);
	hEDMATrx = EDMA_open(EDMA_CHA_XEVT1, EDMA_OPEN_RESET);


	/*
	 * Aquire EDMA tables that hold the reload 	tables for both,
	 * transmit and receive side bufferswitch.
	 */
	hEDMARcvPing = EDMA_allocTable(-1);
	hEDMATrxPing = EDMA_allocTable(-1);
	hEDMARcvPong = EDMA_allocTable(-1);
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
								EDMA_FMKS(OPT, ESIZE, 16BIT)    |
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

