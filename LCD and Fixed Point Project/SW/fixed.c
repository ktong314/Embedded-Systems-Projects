/**
 * @file fixed.c
 * @author Kevin Tong (ktong314@utexas.edu), Jonathan Valvano, Matthew Yu
 *    <Jared McArthur and LAB SECTION #17710>
 * @brief 
 *    Possible implementation file for Lab 1.
 *    Feel free to edit this to match your specifications.
 * @version 1.1.0
 * @date 2022-09-02
 * 
 * @copyright Copyright (c) 2022
 */

/** File includes. */
#include "fixed.h"
#include "inc/ST7735.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/** Function implementation. */
void ST7735_sDecOut3(int32_t n) {
	char output[8] = "";
  if(n > 99999 || n < -99999) {
		if(n > 0){
			ST7735_OutString(" **.***");
		}
		else{
			ST7735_OutString("-**.***");
		}
		return;
	}
	if(n < 0) {
		strcat(output, "-");
	}
	else {
		output[0] = 32;
	}
	
	int bDec = abs(n / 1000);
	int aDec = abs(n % 1000);
	char beforeDec[3] ;
	beforeDec[0] = (bDec/10) + '0';
	beforeDec[1] = (bDec%10) + '0';
	beforeDec[2] = 0x00;
	if(beforeDec[0] == 48){
		beforeDec[0] = ' ';
		if(n < 0) {
			beforeDec[0] = '-';
			output[0] = ' ';
		}
	}
	char afterDec[4];
	afterDec[0] = (aDec/100) + '0';
	afterDec[1] = ((aDec%100)/10) + '0';
	afterDec[2] = (aDec%10) + '0';
	afterDec[3] = 0x00;
	strcat(output, beforeDec);
	strcat(output, ".");
	strcat(output, afterDec);
	output[7] = 0x00;
	ST7735_OutString(output);
	
}

void ST7735_uBinOut5(uint32_t n) {
	char output[7] = "";
	output[6] = 0x00;
  if(n >= 32000) {
		ST7735_OutString("***.**");
		return;
	}
		//another way to round
		// flip steps
		//n = n +16;
		//n = (n * 100) >> 5;
		
		//multiply by 1000 to save the digits when dividing
		//n * 1000/32 = n * 125/4
	n = (n * 125) >> 2; //convert from binary to decimal 
	if(n % 10 >= 5){ // rounding
		n+=10;
	}
	int bDec = n / 1000;
	int aDec = n % 1000;
	char beforeDec[4] ;
	beforeDec[0] = (bDec/100) + '0';
	beforeDec[1] = ((bDec%100)/10) + '0';
	beforeDec[2] = (bDec%10) + '0';
	beforeDec[3] = 0x00;
	int x = 0;
	while(beforeDec[x] == 48 && beforeDec[x + 1] !=0x00){
		beforeDec[x] = ' ';
		x++;
	}
	char afterDec[3];
	afterDec[0] = (aDec/100) + '0';
	afterDec[1] = ((aDec%100)/10) + '0';
	afterDec[2] = 0x00;
	strcat(output, beforeDec);
	strcat(output, ".");
	strcat(output, afterDec);
	ST7735_OutString(output);
}
