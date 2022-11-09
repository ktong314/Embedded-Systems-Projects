//this is the c file that contains code for the encoder
#include "lib/encoder/encoder.h"

//call this in initialization
void encoder_init(void){ 
	Timer1A_Init(&encoder, encoding_period, 4);
	enc_count=-1;
}

//encoder
void encoder(void){
	static int32_t vals[8];
	if (enc_count==-1){
		vals[0] = get_fifo(2);
		if (vals[0] != -1){
			for (int j=1; j<8; j++){
				vals[j] = get_fifo(2);
			}
			dac_period = PERIOD300; //start bit
			enc_count++;
			return;
		}
		else {
			return;
		}
	}
	if (enc_count>=0 && enc_count<=7){
		if (vals[enc_count]==0){
			dac_period=PERIOD300;
		}
		else if (vals[enc_count]==1){
			dac_period=PERIOD1000;
		}
		else {
			dac_period=PERIOD1000;
		}
		enc_count++;
		return;
	}
	
	if (enc_count==8){
		uint32_t sum;
		for (int j=0; j<8; j++){
			sum+=vals[j];
		}
		if (sum % 2){
			dac_period = PERIOD300;
		}
		else {
			dac_period = PERIOD1000;
		}
		enc_count++;
		return;
	}
	if (enc_count==9){
		dac_period = PERIOD1000;
		enc_count=-1;
		return;
	}
}

