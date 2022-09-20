/**
 * @file Lab2.c
 * @author your name (you@domain.com), Jonathan Valvano, Matthew Yu
 *    <TA NAME and LAB SECTION # HERE>
 * @brief
 *    Possible main program to test Lab 2.
 *    Feel free to edit this to match your specifications.
 *
 *    For this lab, the student must implement the following functions defined 
 *    in inc/Dump.h: 
 *        - DumpInit
 *        - DumpCapture
 *        - DumpCount
 *        - DumpData
 *        - DumpTime
 *        - JitterInit
 *        - JitterMeasure
 *        - JitterGet
 *     Feel free to modify the functions in this file as necessary.
 * 
 * @version 0.2
 * @date 2022-09-14 <REPLACE WITH DATE OF LAST REVISION>
 *
 * @copyright Copyright (c) 2022
 * @note Pinout:
 *    Backlight (pin 10) connected to +3.3 V
 *    MISO (pin 9) unconnected
 *    SCK (pin 8) connected to PA2 (SSI0Clk)
 *    MOSI (pin 7) connected to PA5 (SSI0Tx)
 *    TFT_CS (pin 6) connected to PA3 (SSI0Fss)
 *    CARD_CS (pin 5) unconnected
 *    Data/Command (pin 4) connected to PA6 (GPIO)
 *    RESET (pin 3) connected to PA7 (GPIO)
 *    VCC (pin 2) connected to +3.3 V
 *    Gnd (pin 1) connected to ground
 * 
 *    Center of 10k-ohm potentiometer connected to PE2/AIN1
 *    Bottom of 10k-ohm potentiometer connected to ground
 *    Top of 10k-ohm potentiometer connected to +3.3V 
 */

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2021

 Copyright 2022 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

/** File includes. */
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "./inc/ADCSWTrigger.h"
#include "./inc/tm4c123gh6pm.h"
#include "./inc/PLL.h"
#include "./inc/Timer0A.h"
#include "./inc/Timer2A.h"
#include "./inc/ST7735.h"
#include "./inc/CortexM.h"
#include "./inc/LaunchPad.h"
#include "./inc/Dump.h"
#include "./inc/TExaS.h"


/** MMAP Pin definitions. */
#define PF2   (*((volatile uint32_t *)0x40025010))
#define PF3   (*((volatile uint32_t *)0x40025020))
#define PF4   (*((volatile uint32_t *)0x40025040))

/** Other defines */
#define REALTIMEFREQ 1024
#define REALTIMEPERIOD (80000000 / REALTIMEFREQ)
#define ADCSAMPFREQ 125
#define ADCSAMPPERIOD (80000000 / ADCSAMPFREQ)
#define FIXED 100

/** Function declarations. */

/**
 * @brief realTimeTask measures the jitter at a frequency of 1024 Hz
 *        runs from Timer2A ISR
 */
void realTimeTask(void);

/** @brief pause waits for a button press and release. */
void pause(void);

/**
 * @brief realTimeSampling gets a value from the ADC and dumps it into a buffer.
 *        frequency of 125 Hz runs from Timer0A ISR.
 */
void realTimeSampling(void);

/**
 * @brief standardDeviation returns the standard deviation of a buffer of
 *        uint32_t values.
 * 
 * @param buffer The buffer to get the STD from.
 * @param size The size of the buffer.
 * @return uint32_t STD.
 */
uint32_t standardDeviation(uint32_t buffer[], uint32_t size);

/**
 * @brief printStandardDeviation outputs the standard deviation to the ST7735
 * 
 * @param sigma 
 */
void printStandardDeviation(uint32_t sigma);

/** Global variables. */
volatile uint32_t realTimeCount;
uint32_t min, max, jitter;
uint32_t histogram[64];     // probability mass function
uint32_t center;
uint32_t sigma;             // units 1/FIXED
uint32_t jitterVariable;    // global variable to calculate jitter

/** Main functions. */
/**
 * @brief main0 runs the TExaS oscilloscope
 *        analog scope on PD3, PD2, PE2 or PB5 using ADC1
 */
int main0(void) {
    DisableInterrupts();
    TExaS_Init(SCOPE);  // connect analog input to PD3
    LaunchPad_Init();
    uint32_t count = 0;
    EnableInterrupts();
    while(1) {
        ++count;
        if(count > 10000000) {
            PF1 ^= 0x02;  // toggle slowly
            count = 0;
        }
    }
}

/** @brief main1: study jitter with a real logic analyzer */
int main1(void) {
    int line = 1;
    DisableInterrupts();
    jitterVariable = 0;
    
    PLL_Init(Bus80MHz);  // 80 MHz
    ST7735_InitR(INITR_REDTAB);
    ST7735_SetTextColor(ST7735_YELLOW);
    
    DumpInit();
    LaunchPad_Init();
    Timer2A_Init(&realTimeTask, REALTIMEPERIOD, 2);  // PF3 toggle at about 1kHz
    
    ST7735_FillScreen(0);                            // set screen to black
    ST7735_SetCursor(0, 0);
    ST7735_OutString("Lab 2 Jitter, 12.5ns");
    while (1) {
        DisableInterrupts();
        JitterInit();  // analyze 1 kHz real time task
        realTimeCount = 0;
        EnableInterrupts();
        while (realTimeCount < 3000) {       // 3 second measurement
            PF1 ^= 0x02;                     // toggles when running in main
            jitterVariable = (jitterVariable * 12345678) / 1234567;  // the divide instruction causes jitter
        }
        ST7735_SetCursor(0, line);
        ST7735_OutString("Jitter = ");
        ST7735_SetCursor(9, line);
        ST7735_OutUDec(JitterGet());
        line = (line + 1) % 15;
    }
}

/**
 * @brief main2: study jitter with TExaS
 *        run TExaSdisplay on the PC in logic analyzer mode
 */
int main2(void) {
    int line = 1;
    DisableInterrupts();
    jitterVariable = 0;

    TExaS_Init(LOGICANALYZERF);
    ST7735_InitR(INITR_REDTAB);
    ST7735_SetTextColor(ST7735_YELLOW);

    DumpInit();
    LaunchPad_Init();
    Timer2A_Init(&realTimeTask, REALTIMEPERIOD, 2);  // PF3 toggle at about 1kHz

    ST7735_FillScreen(0);                            // set screen to black
    ST7735_SetCursor(0, 0);
    ST7735_OutString("Lab 2 Jitter, 12.5ns");

    while (1) {
        DisableInterrupts();
        JitterInit();  // analyze 1 kHz real time task
        realTimeCount = 0;
        EnableInterrupts();

        while (realTimeCount < 3000) {       // 3 second measurement
            PF1 ^= 0x02;                     // toggles when running in main
            jitterVariable = (jitterVariable * 12345678) / 1234567;  // the divide instruction causes jitter
        }

        ST7735_SetCursor(0, line);
        ST7735_OutString("Jitter = ");
        ST7735_SetCursor(9, line);
        ST7735_OutUDec(JitterGet());
        line = (line + 1) % 15;
    }
}

/**
 * @brief main3: study jitter (central limit theorem) with real logic analyzer
 */
int main(void) {
    uint32_t i, d, data, time, sac;
    // main3, study jitter, CLT with real logic analyzer
    uint32_t* dataBuf;
    uint32_t* timeBuf;

    PLL_Init(Bus80MHz);  // 80 MHz
    ST7735_InitR(INITR_REDTAB);
    ST7735_SetTextColor(ST7735_YELLOW);

    DumpInit();
    dataBuf = DumpData();
    timeBuf = DumpTime();

    LaunchPad_Init();
    Timer2A_Init(&realTimeTask, REALTIMEPERIOD, 2);     // PF3 toggle at about 1kHz
    ADC0_InitSWTriggerSeq3(9);                          // Feel free to use any analog channel
    Timer0A_Init(&realTimeSampling, ADCSAMPPERIOD, 1);  // set up Timer0A for 100 Hz interrupts
    sac = 0;
		
    ST7735_OutString("Lab 2 PMF, SAC=");
    ST7735_OutUDec(sac);
    while (1) {
        DisableInterrupts();
        ADC0_SAC_R = sac;
        DumpInit();
        JitterInit();  // analyze 1 kHz real time task
        realTimeCount = 0;
        EnableInterrupts();

        while (DumpCount() < DUMPBUFSIZE) {
            PF1 ^= 0x02;       // Heartbeat
        }                      // wait for buffers to fill
        
        ST7735_FillScreen(0);  // set screen to black
        ST7735_SetCursor(0, 0);
        ST7735_OutString("Lab 2 PMF, SAC=");
        ST7735_OutUDec(sac);

        for (i = 0; i < 64; ++i) {
            histogram[i] = 0;  // clear
        }

        center = dataBuf[0];
        for (i = 0; i < DUMPBUFSIZE; ++i) {
            data = dataBuf[i];
            if (data < center - 32) {
                ++histogram[0];
            }
            else if (data >= center + 32) {
                ++histogram[63];
            }
            else {
                d = data - center + 32;
                ++histogram[d];
            }
        }

        min = 0xFFFFFFFF;
        max = 0;
        for (i = 1; i < DUMPBUFSIZE; ++i) {
            time = timeBuf[i] - timeBuf[i - 1];  // elapsed
            if (time < min) min = time;
            if (time > max) max = time;
        }

        jitter = max - min;
        sigma = standardDeviation(dataBuf, DUMPBUFSIZE);
        ST7735_SetCursor(0, 1);
        ST7735_OutString("ADC Jitter = ");
        ST7735_OutUDec(jitter);
        ST7735_SetCursor(0, 2);
        ST7735_OutString("PF3 Jitter = ");
        ST7735_OutUDec(JitterGet());
        ST7735_PlotClear(0, DUMPBUFSIZE / 2);

        for (i = 0; i < 63; ++i) {
            if (histogram[i] >= DUMPBUFSIZE / 2) {
                histogram[i] = (DUMPBUFSIZE / 2) - 1;
            }

            ST7735_PlotBar(histogram[i]);
            ST7735_PlotNext();
            ST7735_PlotBar(histogram[i]);
            ST7735_PlotNext();
        }

        printStandardDeviation(sigma);
        pause();

        if (sac < 6) {
            ++sac;
        }
        else {
            sac = 0;
        }
    }
}

/**
 * @brief main4: study jitter (central limit theorem) with TExaS
 *        run TExaSdisplay on the PC in logic analyzer mode
 */
int main4(void) {
    uint32_t i, d, data, time, sac;
    uint32_t* dataBuf;
    uint32_t* timeBuf;

    TExaS_Init(LOGICANALYZERF);
    ST7735_InitR(INITR_REDTAB);
    ST7735_SetTextColor(ST7735_YELLOW);

    DumpInit();
    dataBuf = DumpData();
    timeBuf = DumpTime();

    LaunchPad_Init();
    Timer2A_Init(&realTimeTask, REALTIMEPERIOD, 2);     // PF3 toggle at about 1kHz
    ADC0_InitSWTriggerSeq3(1);                          // Feel free to use any analog channel
    Timer0A_Init(&realTimeSampling, ADCSAMPPERIOD, 1);  // set up Timer0A for 100 Hz interrupts

    sac = 0;
    ST7735_OutString("Lab 2 PMF, SAC=");
    ST7735_OutUDec(sac);

    while (1) {
        DisableInterrupts();
        ADC0_SAC_R = sac;
        DumpInit();
        JitterInit();  // analyze 1 kHz real time task
        realTimeCount = 0;
        EnableInterrupts();
        
        while (DumpCount() < DUMPBUFSIZE) {
            PF1 ^= 0x02;       // Heartbeat
        }                      // wait for buffers to fill

        ST7735_FillScreen(0);  // set screen to black
        ST7735_SetCursor(0, 0);
        ST7735_OutString("Lab 2 PMF, SAC=");
        ST7735_OutUDec(sac);

        for (i = 0; i < 64; ++i) {
            histogram[i] = 0;  // clear
        }

        center = dataBuf[0];
        for (i = 0; i < DUMPBUFSIZE; ++i) {
            data = dataBuf[i];
            if (data < center - 32) {
                ++histogram[0];
            }
            else if (data >= center + 32) {
                ++histogram[63];
            }
            else {
                d = data - center + 32;
                ++histogram[d];
            }
        }

        min = 0xFFFFFFFF;
        max = 0;
        for (i = 1; i < DUMPBUFSIZE; ++i) {
            time = timeBuf[i] - timeBuf[i - 1];  // elapsed
            if (time < min) {
                min = time;
            }
            if (time > max) {
                max = time;
            }
        }

        jitter = max - min;
        sigma = standardDeviation(dataBuf, DUMPBUFSIZE);
        ST7735_SetCursor(0, 1);
        ST7735_OutString("ADC Jitter = ");
        ST7735_OutUDec(jitter);
        ST7735_SetCursor(0, 2);
        ST7735_OutString("PF3 Jitter = ");
        ST7735_OutUDec(JitterGet());
        ST7735_PlotClear(0, DUMPBUFSIZE / 2);

        for (i = 0; i < 63; ++i) {
            if (histogram[i] >= DUMPBUFSIZE / 2) {
                histogram[i] = (DUMPBUFSIZE / 2) - 1;
            }

            ST7735_PlotBar(histogram[i]);
            ST7735_PlotNext();
            ST7735_PlotBar(histogram[i]);
            ST7735_PlotNext();
        }

        printStandardDeviation(sigma);
        pause();

        if (sac < 6) {
            sac++;
        }
        else {
            sac = 0;
        }
    }
}

/** Function Implementations. */
void realTimeTask(void) {
    PF3 ^= 0x08;
    PF3 ^= 0x08;
    ++realTimeCount;
    JitterMeasure();
    PF3 ^= 0x08;
}

void pause(void) {
    LaunchPad_WaitForTouch();
    LaunchPad_WaitForRelease();
}

void realTimeSampling(void) {
    uint32_t ADCvalue;
    PF2 ^= 0x04;  // profile
    PF2 ^= 0x04;  // profile
    ADCvalue = ADC0_InSeq3();
    DumpCapture(ADCvalue);
    PF2 ^= 0x04;  // profile
}

uint32_t standardDeviation(uint32_t buffer[], uint32_t size) {
    int32_t sum = 0;
    for (int i = 0; i < size; ++i) {
        sum = sum + FIXED * buffer[i];
    }
    int32_t ave = sum / size;

    sum = 0;
    for (int i = 0; i < size; ++i) {
        sum = sum + (FIXED * buffer[i] - ave) * (FIXED * buffer[i] - ave);
    }
    return sqrt(sum / (size - 1));  // units 1/FIXED
}

void printStandardDeviation(uint32_t sigma) {
    ST7735_SetCursor(0, 3);
    ST7735_OutString("s = ");
    ST7735_OutUDec(sigma / FIXED);
    ST7735_OutChar('.');

    sigma = sigma % FIXED;
    uint32_t n = FIXED / 10;
    while (n) {
        ST7735_OutUDec(sigma / n);
        sigma = sigma % n;
        n = n / 10;
    }
}
