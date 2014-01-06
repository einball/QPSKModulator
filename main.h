/*
 * main.h
 *
 *  Created on: 20.10.2013
 *      Author: einball
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <csl.h>
#include <dsk6713.h>
#include <dsk6713_led.h>
#include <dsk6713_dip.h>
#include <std.h>
#include <stdint.h>
#include <swi.h>
#include <csl_edma.h>
#include <csl_mcbsp.h>
#include "LED_blinkcfg.h"

/*
 * datastream is 16 long. That means we have 8 IQ samples
 * and therefore we have at 48kHz output rate (24SPS)
 * 8*24 = 192 Samples for 8 Symbols. But as we need to
 * make our Hamming Window 10 symbols wide, we will need
 * to multiply that by 10 and therefore we get 1920 Samples
 * for 8 symbols.
 *
 */

#define DATASTREAM_SIZE 16
#define SAMPLES_PER_SYMBOL 24
#define HAMM_WIN_SYM_LENG 10
#define BUF_SIZE	DATASTREAM_SIZE*SAMPLES_PER_SYMBOL*HAMM_WIN_SYM_LENG/2

/*Variables to add to watch expressions */
int g = 0;
int processping = 0, processpong = 0;
uint16_t arr[4] = {0x0000,0x7fff, 0x0000, 0x8000 };

uint8_t dataStream[DATASTREAM_SIZE] = {0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00};

/* Important stuff */
MCBSP_Handle hMcBsp;

EDMA_Handle hEDMARcv;
EDMA_Handle hEDMATrx;
EDMA_Handle hEDMARcvPing;
EDMA_Handle hEDMARcvPong;
EDMA_Handle hEDMATrxPing;
EDMA_Handle hEDMATrxPong;

volatile int tccRcvPing, tccRcvPong, tccTrxPing, tccTrxPong;

uint32_t oBufPing[BUF_SIZE], oBufPong[BUF_SIZE];

int TrxPingDone=0;
int TrxPongDone=0;

/* The config needs further commenting on what I have done */
EDMA_Config conf_EDMA_oBuf = {
    EDMA_FMKS(OPT, PRI, HIGH)          |  // Distribute to the two Queues
    EDMA_FMKS(OPT, ESIZE, 16BIT)       |  // Element size
    EDMA_FMKS(OPT, 2DS, NO)            |  // No 2D-Transfer
    EDMA_FMKS(OPT, SUM, INC)           |  // Quell-update mode -> FEST (McBSP)!!!
    EDMA_FMKS(OPT, 2DD, NO)            |  // 2kein 2D-Transfer
    EDMA_FMKS(OPT, DUM, NONE)          |  // Ziel-update mode
    EDMA_FMKS(OPT, TCINT, YES)         |  // EDMA interrupt erzeugen?
    EDMA_FMKS(OPT, TCC, OF(0))         |  // Transfer complete code (TCC)
    EDMA_FMKS(OPT, LINK, YES)          |  // Link Parameter nutzen?
    EDMA_FMKS(OPT, FS, NO),               // Frame Sync nutzen?
    (uint32_t)&oBufPing,           // Quell-Adresse
    EDMA_FMK (CNT, FRMCNT, NULL)       |  // Anzahl Frames
    EDMA_FMK (CNT, ELECNT, BUF_SIZE),          // Anzahl Elemente
    EDMA_FMKS(DST, DST, OF(0)),      	  // Ziel-Adresse
    EDMA_FMKS(IDX, FRMIDX, DEFAULT)    |  // Frame index Wert
    EDMA_FMKS(IDX, ELEIDX, DEFAULT),      // Element index Wert
    EDMA_FMK (RLD, ELERLD, NULL)       |  // Reload Element
    EDMA_FMK (RLD, LINK, NULL)            // Reload Link
};

uint32_t ui32SwitchState = 0x00000000;

/* Output Signal to be fed to the DAC */
uint16_t ui16Output[BUF_SIZE];

/* Declaration of function prototypes */
void Blink_LED();
void SWI_processPing();
void SWI_processPong();
void HWI_handleEDMAInterrupt();
void CalcOutput();
void HLP_ReadSwitches();
void conf_EDMA();


#endif /* MAIN_H_ */
