
#include <stdlib.h>
#include <stdio.h>
#include <p89lpc9351.h>
#include <math.h>
#include "pwmControl.h"
#include "lcdControl.h"

#define XTAL 7373000L
#define BAUD 115200L

//tank pins
#define L_TANK P0_0
#define R_TANK P0_1
#define PERP_TANK P0_2

// Other non-lcd pins
#define MOTOR_L P0_5
#define MOTOR_R P0_4


void int main(int argc, char const *argv[])
{
	InitADC();
	while(true) {
		LCDPrintln("L:%x R:%x P:%x",AD1DAT0,AD1DAT1,AD1DAT2);
		waitms(100);
	}
	return 0;
}

void Wait50us (void)
{
	_asm
    mov R0, #82
	L0: djnz R0, L0 ; 2 machine cycles-> 2*0.27126us*92=50us
    _endasm;
}

void waitms (unsigned int ms)
{
	unsigned int j;
	unsigned char k;
	for(j=0; j<ms; j++)
		for (k=0; k<20; k++) Wait50us();
}

void readTank(void)
{
	leftTankValue(0) =leftTankValue(1); 
	leftTankValue(1) = AD1DAT0;

	rightTankValue(0) = rightTankValue(1);
	rightTankValue(1) = AD1DAT1;

	perpTankValue(0) = perpTankValue(1);
	perpTankValue(1) = threshParser(AD1DAT2);
}

void InitADC(void)
{
	// Set adc1 channel pins as input only 
	P0M1 |= (P0M1_4 | P0M1_3 | P0M1_2 | P0M1_1);
	P0M2 &= ~(P0M1_4 | P0M1_3 | P0M1_2 | P0M1_1);

	BURST1=1; //Autoscan continuos conversion mode
	ADMODB = CLK0; //ADC1 clock is 7.3728MHz/2
	ADINS  = (ADI13|ADI12|ADI11|ADI10); // Select the four channels for conversion
	ADCON1 = (ENADC1|ADCS10); //Enable the converter and start immediately
	while((ADCI1&ADCON1)==0); //Wait for first conversion to complete
}
