/*
 * config_MCBSP.c
 *
 *  Created on: 20.10.2013
 *      Author: einball
 */

#include "config_MCBSP.h"


MCBSP_Config cfgDataMCBSP = {

		/* McBSP Control Register */
        MCBSP_FMKS(SPCR, FREE, NO)              |	// No Freerunning - The Serial Interface is not clocked during emulation halt
        MCBSP_FMKS(SPCR, SOFT, YES)		        |	// SOFT is on, so we complete transmissions after halting during emulations #TODO
        MCBSP_FMKS(SPCR, FRST, YES)             |	// Framesync is on
        MCBSP_FMKS(SPCR, GRST, YES)             |	// Reset off, so the Samplerate generator runs
        MCBSP_FMKS(SPCR, XINTM, XRDY)           |
        MCBSP_FMKS(SPCR, XSYNCERR, NO)          |	// No framesync supervision on transmitter side
        MCBSP_FMKS(SPCR, XRST, YES)             |	// Enable transmitter to run
        MCBSP_FMKS(SPCR, DLB, OFF)              |	// Loopback  inactive
        MCBSP_FMKS(SPCR, RJUST, RZF)            |	// Right align and zero fill
        MCBSP_FMKS(SPCR, CLKSTP, DISABLE)       |	// Clock starts without delay on falling edge (see also: PCR-Register) #TODO: in AIC23 config "NODELAY" is used - which one is right?
        MCBSP_FMKS(SPCR, DXENA, OFF)            |	// Do not use DX- Enabler
        MCBSP_FMKS(SPCR, RINTM, RRDY)           |	// Receiver interrupt will  be triggered by "RRDY-Bit"
        MCBSP_FMKS(SPCR, RSYNCERR, NO)          |	// No synchronisation supervision on receive side
        MCBSP_FMKS(SPCR, RRST, YES),				// Enable the receiver to run


		/* Receive Control Register */
        MCBSP_FMKS(RCR, RPHASE, SINGLE)         |	// Only one phase per frame
        MCBSP_FMKS(RCR, RFRLEN2, DEFAULT)       |	// Framelength in phase 2 is not relevant
        MCBSP_FMKS(RCR, RWDLEN2, DEFAULT)       |	// Symbollength in Phase 2 is not relevant
        MCBSP_FMKS(RCR, RCOMPAND, MSB)          |	// No data compansion (MSB first)
        MCBSP_FMKS(RCR, RFIG, NO)               |	// Discard RX register content if framesync pulse occurs after the first bit
        MCBSP_FMKS(RCR, RDATDLY, 0BIT)          |	// Do not delay the data
        MCBSP_FMKS(RCR, RFRLEN1, OF(1))         |	// Phase 1 consists of 1 symbol in 1 frame
        MCBSP_FMKS(RCR, RWDLEN1, 16BIT)         |	//
        MCBSP_FMKS(RCR, RWDREVRS, DISABLE),			// 32-bit Reversal is not used here


		/* Send-Control Register */
        MCBSP_FMKS(XCR, XPHASE, SINGLE)         |	//
        MCBSP_FMKS(XCR, XFRLEN2, DEFAULT)       |	// Framelength in Phase2 not relevant as we only send 16bit and not 32bit
        MCBSP_FMKS(XCR, XWDLEN2, DEFAULT)       |	// Symbollength in Phase 2 also not relevant
        MCBSP_FMKS(XCR, XCOMPAND, MSB)          |	// No data compansion (MSB first)
        MCBSP_FMKS(XCR, XFIG, NO)               |	// Restart transmission if framesync pulse occurs after the first bit
        MCBSP_FMKS(XCR, XDATDLY, 0BIT)          |	// Do not delay the data
        MCBSP_FMKS(XCR, XFRLEN1, OF(1))         |	// Phase 1 consists of 1 symbol in 1 frame
        MCBSP_FMKS(XCR, XWDLEN1, 16BIT)         |	// Symbollength in Phase 1 is 16bit
        MCBSP_FMKS(XCR, XWDREVRS, DISABLE),			// 32-bit Reversal is not used here


		/* Sample Rate Generator Register */
        MCBSP_FMKS(SRGR, GSYNC, DEFAULT)        |	// Settings are not relevant as
        MCBSP_FMKS(SRGR, CLKSP, DEFAULT)        |	// McBSP1 is running as slave
        MCBSP_FMKS(SRGR, CLKSM, DEFAULT)        |	// and is fed with an external
        MCBSP_FMKS(SRGR, FSGM, DEFAULT)/*Not used*/         |	// clock.
        MCBSP_FMKS(SRGR, FPER, DEFAULT)         |	// --
        MCBSP_FMKS(SRGR, FWID, DEFAULT)         |	// --
        MCBSP_FMKS(SRGR, CLKGDV, DEFAULT),			// --


		/* Multichannel */
        MCBSP_MCR_DEFAULT,				// Multichannel is disabled
        MCBSP_RCER_DEFAULT,				// same
        MCBSP_XCER_DEFAULT,				// same


		/* Pinout Control Register */
        MCBSP_FMKS(PCR, XIOEN, SP)              |	// Pin is used for the serial interface and not as GPIO
        MCBSP_FMKS(PCR, RIOEN, SP)              |	// Pin is used for the serial interface and not as GPIO
        MCBSP_FMKS(PCR, FSXM, EXTERNAL)         |	// Framesync- signal for the transmitter is fed externally
        MCBSP_FMKS(PCR, FSRM, EXTERNAL)         |	// Framesync- signal for the receiver is fed externally
        MCBSP_FMKS(PCR, CLKXM, INPUT)           |	// Clock for transmitter is fed externally
        MCBSP_FMKS(PCR, CLKRM, INPUT)           |	// Clock for receiver is fes externally
        MCBSP_FMKS(PCR, CLKSSTAT, DEFAULT)      |	// Pins are not used as GPIO and therefore this is not relevant for us
        MCBSP_FMKS(PCR, DXSTAT, DEFAULT)        |	// Pins are not used as GPIO and therefore this is not relevant for us
        MCBSP_FMKS(PCR, FSXP, ACTIVEHIGH)       |	// Framesync on transmit side is "active high"
        MCBSP_FMKS(PCR, FSRP, ACTIVEHIGH)       |	// Framesync on receive side is "active high"
        MCBSP_FMKS(PCR, CLKXP, FALLING)         |	// Data is sent on the falling edge
        MCBSP_FMKS(PCR, CLKRP, RISING)				// Data is received on the rising edge
};

/* Configure McBSP1 - Data interface */
void conf_MCBSP(){
	hMcBsp = MCBSP_open(MCBSP_DEV1, MCBSP_OPEN_RESET);
    MCBSP_config(hMcBsp, &cfgDataMCBSP);
}

void start_MCBSP(){
    MCBSP_start(hMcBsp, MCBSP_XMIT_START | MCBSP_RCV_START, 0xffffffff);
    MCBSP_write(hMcBsp, 0x0); /* Send only once to get things going */
}
