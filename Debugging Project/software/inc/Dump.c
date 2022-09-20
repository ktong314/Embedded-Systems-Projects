/**
 * @file Dump.c
 * @author your name (you@domain.com), Jonathan Valvano, Matthew Yu
 *    <TA NAME and LAB SECTION # HERE>
 * @brief 
 *    A set of debugging functions that capture data for later inspection.
 *    There are two primary methods:
 *       - DebugCapture will record data and time.
 *       - JitterMeasure will measure real time jitter.
 * @version 0.2
 * @date 2022-09-01 <REPLACE WITH DATE OF LAST REVISION>
 *
 * @copyright Copyright (c) 2022
 */

/** File includes. */
#include <stdint.h>
#include "../inc/Dump.h"
#include "../inc/tm4c123gh6pm.h"

// Global variables
uint32_t DumpTimeBuffer[DUMPBUFSIZE];
uint32_t DumpDataBuf[DUMPBUFSIZE];
uint32_t DumpNum;

uint32_t JitterBuffer[DUMPBUFSIZE - 1];
uint32_t JitterNum;
uint32_t previousCall;

void Timer1_Init(void) {
    volatile uint32_t delay;
    SYSCTL_RCGCTIMER_R    |= 0x02;                // 0) activate TIMER1
    delay                  = SYSCTL_RCGCTIMER_R;  // allow time to finish activating
    TIMER1_CTL_R           = 0x00000000;          // 1) disable TIMER1A during setup
    TIMER1_CFG_R           = 0x00000000;          // 2) configure for 32-bit mode
    TIMER1_TAMR_R          = 0x00000002;          // 3) configure for periodic mode, default down-count settings
    TIMER1_TAILR_R         = 0xFFFFFFFF;          // 4) reload value
    TIMER1_TAPR_R          = 0;                   // 5) bus clock resolution
    TIMER1_CTL_R           = 0x00000001;          // 10) enable TIMER1A
}

void DumpInit(void){
    /* TODO (EE445L Lab 2): complete this. */
	Timer1_Init();
	DumpNum = 0;
}

void DumpCapture(uint32_t data){
    /* TODO (EE445L Lab 2): complete this. */
    // Use TIMER1_TAR_R as current time
	if(DumpNum < DUMPBUFSIZE){
		DumpTimeBuffer[DumpNum] =  TIMER1_TAR_R;
		DumpDataBuf[DumpNum] = data;
		
		DumpNum++;
		
	} 
	
	
}

uint32_t DumpCount(void){ 
    /* TODO (EE445L Lab 2): complete this. */
	
    return DumpNum;
}

uint32_t* DumpData(void){ 
    return DumpDataBuf;
}

uint32_t* DumpTime(void){ 
    return DumpTimeBuffer;
}

void JitterInit(void){
    /* TODO (EE445L Lab 2): complete this. */
	previousCall = 0;
	JitterNum = 0;
}

void JitterMeasure(void){
    /* TODO (EE445L Lab 2): complete this. */
	if(JitterNum < DUMPBUFSIZE - 1){
		uint32_t currentTime = 0xFFFFFFFF - TIMER1_TAR_R;
		JitterBuffer[JitterNum] = currentTime - previousCall;
		previousCall = currentTime;
		JitterNum++;
	}
	
}

uint32_t JitterGet(void){
    /* TODO (EE445L Lab 2): complete this. */ 
	uint32_t max = 0;
	uint32_t min = UINT32_MAX;
	for(int i = 1; i < DUMPBUFSIZE - 2; i++){
		if (JitterBuffer[i] > max){
			max = JitterBuffer[i];
		}
		if (JitterBuffer[i] < min){
			min = JitterBuffer[i];
		}
	}
	return max - min;
}




