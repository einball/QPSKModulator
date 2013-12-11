<<<<<<< HEAD
/*
 * config_AIC23.c
 *
 *  Created on: 20.10.2013
 *      Author: einball
 */

#include "config_AIC23.h"

/***************************************************/
/* AIC23 configuration from CCS online help        */
/* Data Converter/Devices/TLV320AIC23              */
/***************************************************/
unsigned short myAIC23_registers[10] = { \
	    0x0015, /* Reg 0       Left line input channel volume control */  \
	    0x0015, /* Reg 1       Right line input channel volume control */ \
	    0x00ff, /* Reg 2       Left channel headphone volume control, f9 for 0db, ff for +6db */   \
	    0x00ff, /* Reg 3       Right channel headphone volume control */  \
	    0x0014, /* Reg 4       Analog audio path control */ /* Bypass (d4) (without: D2) on to test if parametrized right */              \
	    0x0006, /* Reg 5       Digital audio path control */              \
	    0x0000, /* Reg 6       Power down control */                      \
	    0x0043, /* Reg 7       Digital audio interface format */          \
	    0x0000, /* Reg 8       Sample rate control */                     \
	    0x0001  /* Reg 9       Digital interface activation */            \
	};


//McBSP0 configuration structure
MCBSP_Config mcbspforAIC23Cfg = {
        /* McBSP Control Register */
		MCBSP_FMKS(SPCR, FREE, NO)              |	// Disable Freerunning mode
        MCBSP_FMKS(SPCR, SOFT, NO)              |	// Clock immediately stops at emulation halt
        MCBSP_FMKS(SPCR, FRST, YES)             |	// FrameSync on because it is the CS line on the Codec
        MCBSP_FMKS(SPCR, GRST, YES)             |	// Enable the samplerate generator
        MCBSP_FMKS(SPCR, XINTM, XRDY)           |	// Sender Interrupt will betriggered by "XRDY-Bit"
        MCBSP_FMKS(SPCR, XSYNCERR, NO)          |	// No synchronisation supervision on transmit side
        MCBSP_FMKS(SPCR, XRST, YES)             |	// Enable transmitter to run
        MCBSP_FMKS(SPCR, DLB, OFF)              |	// Loopback  inactive
        MCBSP_FMKS(SPCR, RJUST, RZF)            |	// Right align and zero fill
        MCBSP_FMKS(SPCR, CLKSTP, NODELAY)       |	// Clock starts without delay on falling edge (see also: PCR-Register)
        MCBSP_FMKS(SPCR, DXENA, OFF)            |	// Do not use DX- Enabler
        MCBSP_FMKS(SPCR, RINTM, RRDY)           |	// Receiver interrupt will  be triggered by "RRDY-Bit"
        MCBSP_FMKS(SPCR, RSYNCERR, NO)          |	// No synchronisation supervision on receive side
        MCBSP_FMKS(SPCR, RRST, YES),				// Enable the receiver to run


		/* Receive Control Register */
        MCBSP_FMKS(RCR, RPHASE, DEFAULT)        |	// Only one phase per frame
        MCBSP_FMKS(RCR, RFRLEN2, DEFAULT)       |	// Framelength in phase 2 is not relevant
        MCBSP_FMKS(RCR, RWDLEN2, DEFAULT)       |	// Symbollength in Phase 2 is not relevant
        MCBSP_FMKS(RCR, RCOMPAND, DEFAULT)      |	// No data compansion (MSB first)
        MCBSP_FMKS(RCR, RFIG, DEFAULT)          |	// Discard RX register content if framesync pulse occurs after the first bit
        MCBSP_FMKS(RCR, RDATDLY, DEFAULT)       |	// Do not delay the data
        MCBSP_FMKS(RCR, RFRLEN1, DEFAULT)       |	// Framelength in Phase 1 not relevant as we do not need to reveive anything
        MCBSP_FMKS(RCR, RWDLEN1, DEFAULT)       |	// Symbollength in Phase 1 is not relevant
        MCBSP_FMKS(RCR, RWDREVRS, DEFAULT),			// 32-bit Reversal is not used here


		/* Sende-Control Register */
        MCBSP_FMKS(XCR, XPHASE, SINGLE)         |	// Only one phase per frame
        MCBSP_FMKS(XCR, XFRLEN2, OF(0))         |	// Framelength in Phase2 not relevant as we only send 16bit and not 32bit
        MCBSP_FMKS(XCR, XWDLEN2, 8BIT)          |	// Symbollength in Phase 2 also not relevant
        MCBSP_FMKS(XCR, XCOMPAND, MSB)          |	// No data compansion (MSB first)
        MCBSP_FMKS(XCR, XFIG, NO)               |	// Restart transmission if framesync pulse occurs after the first bit
        MCBSP_FMKS(XCR, XDATDLY, 1BIT)          |	// Issue 1 bit delay on the data
        MCBSP_FMKS(XCR, XFRLEN1, OF(0))         |	// Phase 1 consists of 1 symbol in 1 frame
        MCBSP_FMKS(XCR, XWDLEN1, 16BIT)         |	// Symbollength in Phase 1 is 16bit
        MCBSP_FMKS(XCR, XWDREVRS, DISABLE),			// 32-bit Reversal is not used here


		/* Sample Rate Generator Register */
        MCBSP_FMKS(SRGR, GSYNC, FREE)           |	// Samplerate-Clock runs free
        MCBSP_FMKS(SRGR, CLKSP, RISING)         |	// Not relevant here because the internal clock is used
        MCBSP_FMKS(SRGR, CLKSM, INTERNAL)       |	// Samplerate-Clock will be derived from the CPU clock
        MCBSP_FMKS(SRGR, FSGM, DXR2XSR)         |	// Framesync on every DXR to XSR copy (FPER and FWID have no more effect)
        MCBSP_FMKS(SRGR, FPER, OF(0))           |	// see above
        MCBSP_FMKS(SRGR, FWID, OF(19))          |	// see above
        MCBSP_FMKS(SRGR, CLKGDV, OF(99)),			// Divider for CPU clock


		/* Multichannel */
        MCBSP_MCR_DEFAULT,							// Multichannel is disabled
        MCBSP_RCER_DEFAULT,							// same
        MCBSP_XCER_DEFAULT,							// same


		/* Pinout Control Register */
        MCBSP_FMKS(PCR, XIOEN, SP)              |	// Pin is used for the serial interface and not as GPIO
        MCBSP_FMKS(PCR, RIOEN, SP)              |	// Pin is used for the serial interface and not as GPIO
        MCBSP_FMKS(PCR, FSXM, INTERNAL)         |	// Signal is derived from samplerate generator
        MCBSP_FMKS(PCR, FSRM, EXTERNAL)         |	// We don't care about the receiver since it is not used
        MCBSP_FMKS(PCR, CLKXM, OUTPUT)          |	// McBSP0 is the clock source and therefore the master
        MCBSP_FMKS(PCR, CLKRM, INPUT)           |	// nNot relevant since we only want to transmit
        MCBSP_FMKS(PCR, CLKSSTAT, DEFAULT)      |	// Pins are not used as GPIO and therefore this is not relevant for us
        MCBSP_FMKS(PCR, DXSTAT, DEFAULT)        |	// Pins are not used as GPIO and therefore this is not relevant for us
        MCBSP_FMKS(PCR, FSXP, ACTIVELOW)        |	// Framesync is "active low"
        MCBSP_FMKS(PCR, FSRP, DEFAULT)          |	// Not relevant since we are only transmitting
        MCBSP_FMKS(PCR, CLKXP, FALLING)         |	// Data is sent on the falling edge
        MCBSP_FMKS(PCR, CLKRP, DEFAULT)				// Not relevant since we are only transmitting
};

void conf_AIC23(void)
{
	/* Handle for control channel */
	MCBSP_Handle hMcbsp0;
	unsigned short i;

	hMcbsp0 = MCBSP_open(MCBSP_DEV0, MCBSP_OPEN_RESET);
    MCBSP_config(hMcbsp0, &mcbspforAIC23Cfg);
    MCBSP_start(hMcbsp0, MCBSP_XMIT_START | MCBSP_SRGR_START | MCBSP_SRGR_FRAMESYNC, 220);

	/* Now configure all registers */
	/* A reset at the beginning is always good, so we set reg 15 to 0 */
    setReg_AIC23(hMcbsp0,15,0);

    /* power-down registers first */
    setReg_AIC23(hMcbsp0,6,myAIC23_registers[6]);

    /* Now the rest */
    for (i = 0; i < 6; i++)
      setReg_AIC23(hMcbsp0,i,myAIC23_registers[i]);
    for (i = 7; i < 10; i++)
      setReg_AIC23(hMcbsp0,i,myAIC23_registers[i]);

	 /* Done here, but don't close the port because we loose the clock to the Codec then!*/
}





void setReg_AIC23(MCBSP_Handle hMcbsp,unsigned short regnum, unsigned short regval)
{
	/* Programming it that way so in B[15:9] you find the register number
	   and in B[8:0] the data that has to be written */

    /* Mask regval to 9 bits as we only need 9 bits of data */
    regval &= 0x1ff;

	/* Mask regnum to 7 bit for security */
	regnum &= 0x07f;

    /* Wait */
    while (!MCBSP_xrdy(hMcbsp));

    /* Write */
    MCBSP_write(hMcbsp, (regnum<<9) | regval);
}
=======
/*
 * config_AIC23.c
 *
 *  Created on: 20.10.2013
 *      Author: einball
 */

#include "config_AIC23.h"

/***************************************************/
/* AIC23 configuration from CCS online help        */
/* Data Converter/Devices/TLV320AIC23              */
/***************************************************/
unsigned short myAIC23_registers[10] = { \
	    0x0015, /* Reg 0       Left line input channel volume control */  \
	    0x0015, /* Reg 1       Right line input channel volume control */ \
	    0x00ff, /* Reg 2       Left channel headphone volume control, f9 for 0db, ff for +6db */   \
	    0x00ff, /* Reg 3       Right channel headphone volume control */  \
	    0x0014, /* Reg 4       Analog audio path control */ /* Bypass (d4) (without: D2) on to test if parametrized right */              \
	    0x0006, /* Reg 5       Digital audio path control */              \
	    0x0000, /* Reg 6       Power down control */                      \
	    0x0043, /* Reg 7       Digital audio interface format */          \
	    0x0000, /* Reg 8       Sample rate control */                     \
	    0x0001  /* Reg 9       Digital interface activation */            \
	};


//McBSP0 configuration structure
MCBSP_Config mcbspforAIC23Cfg = {
        /* McBSP Control Register */
		MCBSP_FMKS(SPCR, FREE, NO)              |	// Disable Freerunning mode
        MCBSP_FMKS(SPCR, SOFT, NO)              |	// Clock immediately stops at emulation halt
        MCBSP_FMKS(SPCR, FRST, YES)             |	// FrameSync on because it is the CS line on the Codec
        MCBSP_FMKS(SPCR, GRST, YES)             |	// Enable the samplerate generator
        MCBSP_FMKS(SPCR, XINTM, XRDY)           |	// Sender Interrupt will betriggered by "XRDY-Bit"
        MCBSP_FMKS(SPCR, XSYNCERR, NO)          |	// No synchronisation supervision on transmit side
        MCBSP_FMKS(SPCR, XRST, YES)             |	// Enable transmitter to run
        MCBSP_FMKS(SPCR, DLB, OFF)              |	// Loopback  inactive
        MCBSP_FMKS(SPCR, RJUST, RZF)            |	// Right align and zero fill
        MCBSP_FMKS(SPCR, CLKSTP, NODELAY)       |	// Clock starts without delay on falling edge (see also: PCR-Register)
        MCBSP_FMKS(SPCR, DXENA, OFF)            |	// Do not use DX- Enabler
        MCBSP_FMKS(SPCR, RINTM, RRDY)           |	// Receiver interrupt will  be triggered by "RRDY-Bit"
        MCBSP_FMKS(SPCR, RSYNCERR, NO)          |	// No synchronisation supervision on receive side
        MCBSP_FMKS(SPCR, RRST, YES),				// Enable the receiver to run


		/* Receive Control Register */
        MCBSP_FMKS(RCR, RPHASE, DEFAULT)        |	// Only one phase per frame
        MCBSP_FMKS(RCR, RFRLEN2, DEFAULT)       |	// Framelength in phase 2 is not relevant
        MCBSP_FMKS(RCR, RWDLEN2, DEFAULT)       |	// Symbollength in Phase 2 is not relevant
        MCBSP_FMKS(RCR, RCOMPAND, DEFAULT)      |	// No data compansion (MSB first)
        MCBSP_FMKS(RCR, RFIG, DEFAULT)          |	// Discard RX register content if framesync pulse occurs after the first bit
        MCBSP_FMKS(RCR, RDATDLY, DEFAULT)       |	// Do not delay the data
        MCBSP_FMKS(RCR, RFRLEN1, DEFAULT)       |	// Framelength in Phase 1 not relevant as we do not need to reveive anything
        MCBSP_FMKS(RCR, RWDLEN1, DEFAULT)       |	// Symbollength in Phase 1 is not relevant
        MCBSP_FMKS(RCR, RWDREVRS, DEFAULT),			// 32-bit Reversal is not used here


		/* Sende-Control Register */
        MCBSP_FMKS(XCR, XPHASE, SINGLE)         |	// Only one phase per frame
        MCBSP_FMKS(XCR, XFRLEN2, OF(0))         |	// Framelength in Phase2 not relevant as we only send 16bit and not 32bit
        MCBSP_FMKS(XCR, XWDLEN2, 8BIT)          |	// Symbollength in Phase 2 also not relevant
        MCBSP_FMKS(XCR, XCOMPAND, MSB)          |	// No data compansion (MSB first)
        MCBSP_FMKS(XCR, XFIG, NO)               |	// Restart transmission if framesync pulse occurs after the first bit
        MCBSP_FMKS(XCR, XDATDLY, 1BIT)          |	// Issue 1 bit delay on the data
        MCBSP_FMKS(XCR, XFRLEN1, OF(0))         |	// Phase 1 consists of 1 symbol in 1 frame
        MCBSP_FMKS(XCR, XWDLEN1, 16BIT)         |	// Symbollength in Phase 1 is 16bit
        MCBSP_FMKS(XCR, XWDREVRS, DISABLE),			// 32-bit Reversal is not used here


		/* Sample Rate Generator Register */
        MCBSP_FMKS(SRGR, GSYNC, FREE)           |	// Samplerate-Clock runs free
        MCBSP_FMKS(SRGR, CLKSP, RISING)         |	// Not relevant here because the internal clock is used
        MCBSP_FMKS(SRGR, CLKSM, INTERNAL)       |	// Samplerate-Clock will be derived from the CPU clock
        MCBSP_FMKS(SRGR, FSGM, DXR2XSR)         |	// Framesync on every DXR to XSR copy (FPER and FWID have no more effect)
        MCBSP_FMKS(SRGR, FPER, OF(0))           |	// see above
        MCBSP_FMKS(SRGR, FWID, OF(19))          |	// see above
        MCBSP_FMKS(SRGR, CLKGDV, OF(99)),			// Divider for CPU clock


		/* Multichannel */
        MCBSP_MCR_DEFAULT,							// Multichannel is disabled
        MCBSP_RCER_DEFAULT,							// same
        MCBSP_XCER_DEFAULT,							// same


		/* Pinout Control Register */
        MCBSP_FMKS(PCR, XIOEN, SP)              |	// Pin is used for the serial interface and not as GPIO
        MCBSP_FMKS(PCR, RIOEN, SP)              |	// Pin is used for the serial interface and not as GPIO
        MCBSP_FMKS(PCR, FSXM, INTERNAL)         |	// Signal is derived from samplerate generator
        MCBSP_FMKS(PCR, FSRM, EXTERNAL)         |	// We don't care about the receiver since it is not used
        MCBSP_FMKS(PCR, CLKXM, OUTPUT)          |	// McBSP0 is the clock source and therefore the master
        MCBSP_FMKS(PCR, CLKRM, INPUT)           |	// nNot relevant since we only want to transmit
        MCBSP_FMKS(PCR, CLKSSTAT, DEFAULT)      |	// Pins are not used as GPIO and therefore this is not relevant for us
        MCBSP_FMKS(PCR, DXSTAT, DEFAULT)        |	// Pins are not used as GPIO and therefore this is not relevant for us
        MCBSP_FMKS(PCR, FSXP, ACTIVELOW)        |	// Framesync is "active low"
        MCBSP_FMKS(PCR, FSRP, DEFAULT)          |	// Not relevant since we are only transmitting
        MCBSP_FMKS(PCR, CLKXP, FALLING)         |	// Data is sent on the falling edge
        MCBSP_FMKS(PCR, CLKRP, DEFAULT)				// Not relevant since we are only transmitting
};

void conf_AIC23(void)
{
	/* Handle for control channel */
	MCBSP_Handle hMcbsp0;
	unsigned short i;

	hMcbsp0 = MCBSP_open(MCBSP_DEV0, MCBSP_OPEN_RESET);
    MCBSP_config(hMcbsp0, &mcbspforAIC23Cfg);
    MCBSP_start(hMcbsp0, MCBSP_XMIT_START | MCBSP_SRGR_START | MCBSP_SRGR_FRAMESYNC, 220);

	/* Now configure all registers */
	/* A reset at the beginning is always good, so we set reg 15 to 0 */
    setReg_AIC23(hMcbsp0,15,0);

    /* power-down registers first */
    setReg_AIC23(hMcbsp0,6,myAIC23_registers[6]);

    /* Now the rest */
    for (i = 0; i < 6; i++)
      setReg_AIC23(hMcbsp0,i,myAIC23_registers[i]);
    for (i = 7; i < 10; i++)
      setReg_AIC23(hMcbsp0,i,myAIC23_registers[i]);

	 /* Done here, but don't close the port because we loose the clock to the Codec then!*/
}





void setReg_AIC23(MCBSP_Handle hMcbsp,unsigned short regnum, unsigned short regval)
{
	/* Programming it that way so in B[15:9] you find the register number
	   and in B[8:0] the data that has to be written */

    /* Mask regval to 9 bits as we only need 9 bits of data */
    regval &= 0x1ff;

	/* Mask regnum to 7 bit for security */
	regnum &= 0x07f;

    /* Wait */
    while (!MCBSP_xrdy(hMcbsp));

    /* Write */
    MCBSP_write(hMcbsp, (regnum<<9) | regval);
}
>>>>>>> ab23cd8a9350fd4523f9f55789349701ba5af898
