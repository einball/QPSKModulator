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
#define BUF_SIZE	241 //DATASTREAM_SIZE*SAMPLES_PER_SYMBOL*HAMM_WIN_SYM_LENG/2

/*Variables to add to watch expressions */
int g = 0;
int processping = 0, processpong = 0;
//int32_t arr[8] = {0x00000000, 0x00000000, 0x7fffffff, 0x7fffffff, 0x00000000,0x00000000, 0x80000000, 0x80000000 };
//int32_t arr[4] = {0,2147483647,0,-2147483647};
int32_t arr[BUF_SIZE]={ 0 ,  4000 ,  16342 ,  37439 ,  67541 ,  106697 ,  154706 ,  211062 ,  274896 ,  344920 ,  419376 ,  496002 ,  572012 ,  644102 ,  708487 ,  760970 ,  797052 ,  812073 ,  801400 ,  760637 ,  685871 ,  573928 ,  422647 ,  231133 ,  0 , -268431 , -569986 , -898566 , -1.24616e+06 , -1.60296e+06 , -1.95757e+06 , -2.29736e+06 , -2.60889e+06 , -2.87851e+06 , -3.09294e+06 , -3.24008e+06 , -3.30975e+06 , -3.29451e+06 , -3.19047e+06 , -2.99802e+06 , -2.72248e+06 , -2.37463e+06 , -1.97103e+06 , -1.5341e+06 , -1.09198e+06 , -678083 , -330283 , -89849 ,  0 , -104166 , -443964 , -1.05693e+06 , -1.97409e+06 , -3.21739e+06 , -4.79718e+06 , -6.70969e+06 , -8.93477e+06 , -1.14339e+07 , -1.41487e+07 , -1.69998e+07 , -1.98866e+07 , -2.26872e+07 , -2.52604e+07 , -2.74469e+07 , -2.90727e+07 , -2.99531e+07 , -2.9897e+07 , -2.8713e+07 , -2.62158e+07 , -2.22328e+07 , -1.66122e+07 , -9.2303e+06 ,  0 ,  1.11218e+07 ,  2.41266e+07 ,  3.89476e+07 ,  5.54536e+07 ,  7.34444e+07 ,  9.26477e+07 ,  1.12717e+08 ,  1.33231e+08 ,  1.53698e+08 ,  1.73557e+08 ,  1.92185e+08 ,  2.08907e+08 ,  2.23004e+08 ,  2.33726e+08 ,  2.40307e+08 ,  2.4198e+08 ,  2.37993e+08 ,  2.27627e+08 ,  2.10215e+08 ,  1.85162e+08 ,  1.51957e+08 ,  1.10195e+08 ,  5.95929e+07 ,  0 , -6.85877e+07 , -1.46019e+08 , -2.31981e+08 , -3.25996e+08 , -4.27423e+08 , -5.35459e+08 , -6.4915e+08 , -7.67397e+08 , -8.88976e+08 , -1.01255e+09 , -1.1367e+09 , -1.25992e+09 , -1.3807e+09 , -1.49746e+09 , -1.60869e+09 , -1.71288e+09 , -1.80861e+09 , -1.89453e+09 , -1.96945e+09 , -2.03228e+09 , -2.08212e+09 , -2.11826e+09 , -2.14015e+09 , -2.14748e+09 , -2.14015e+09 , -2.11826e+09 , -2.08212e+09 , -2.03228e+09 , -1.96945e+09 , -1.89453e+09 , -1.80861e+09 , -1.71288e+09 , -1.60869e+09 , -1.49746e+09 , -1.3807e+09 , -1.25992e+09 , -1.1367e+09 , -1.01255e+09 , -8.88976e+08 , -7.67397e+08 , -6.4915e+08 , -5.35459e+08 , -4.27423e+08 , -3.25996e+08 , -2.31981e+08 , -1.46019e+08 , -6.85877e+07 ,  0 ,  5.95929e+07 ,  1.10195e+08 ,  1.51957e+08 ,  1.85162e+08 ,  2.10215e+08 ,  2.27627e+08 ,  2.37993e+08 ,  2.4198e+08 ,  2.40307e+08 ,  2.33726e+08 ,  2.23004e+08 ,  2.08907e+08 ,  1.92185e+08 ,  1.73557e+08 ,  1.53698e+08 ,  1.33231e+08 ,  1.12717e+08 ,  9.26477e+07 ,  7.34444e+07 ,  5.54536e+07 ,  3.89476e+07 ,  2.41266e+07 ,  1.11218e+07 ,  0 , -9.2303e+06 , -1.66122e+07 , -2.22328e+07 , -2.62158e+07 , -2.8713e+07 , -2.9897e+07 , -2.99531e+07 , -2.90727e+07 , -2.74469e+07 , -2.52604e+07 , -2.26872e+07 , -1.98866e+07 , -1.69998e+07 , -1.41487e+07 , -1.14339e+07 , -8.93477e+06 , -6.70969e+06 , -4.79718e+06 , -3.21739e+06 , -1.97409e+06 , -1.05693e+06 , -443964 , -104166 ,  0 , -89849 , -330283 , -678083 , -1.09198e+06 , -1.5341e+06 , -1.97103e+06 , -2.37463e+06 , -2.72248e+06 , -2.99802e+06 , -3.19047e+06 , -3.29451e+06 , -3.30975e+06 , -3.24008e+06 , -3.09294e+06 , -2.87851e+06 , -2.60889e+06 , -2.29736e+06 , -1.95757e+06 , -1.60296e+06 , -1.24616e+06 , -898566 , -569986 , -268431 ,  0 ,  231133 ,  422647 ,  573928 ,  685871 ,  760637 ,  801400 ,  812073 ,  797052 ,  760970 ,  708487 ,  644102 ,  572012 ,  496002 ,  419376 ,  344920 ,  274896 ,  211062 ,  154706 ,  106697 ,  67541 ,  37439 ,  16342 ,  4000 ,  0 };




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

uint32_t oBufPing[BUF_SIZE*2], oBufPong[BUF_SIZE*2];

int TrxPingDone=0;
int TrxPongDone=0;

/* The config needs further commenting on what I have done */
EDMA_Config conf_EDMA_oBuf = {
    EDMA_FMKS(OPT, PRI, HIGH)          |  // Distribute to the two Queues
    EDMA_FMKS(OPT, ESIZE, 32BIT)       |  // Element size
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
    EDMA_FMK (CNT, ELECNT, BUF_SIZE*2),          // Anzahl Elemente
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
