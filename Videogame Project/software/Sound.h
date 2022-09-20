// Sound.h
// Last Modified: 5/2/2021 
// Ethan Litchauer and Kevin Tong
// Runs on TM4C123 or LM4F120
// Prototypes for basic functions to play sounds from the
typedef enum {Drop,Clear,Rocket,Fire} soundeffect;
void playsound(soundeffect);
