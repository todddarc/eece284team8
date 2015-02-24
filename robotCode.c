
#include <stdlib.h>
#include <stdio.h>
#include <p89lpc9351.h>
#include <math.h>
#include "pwmControl.h"
#include "lcdControl.h"

#define XTAL 7373000L
#define BAUD 115200L

// Make sure these definitions match your wiring
#define LCD_RS P3_0
#define LCD_RW P1_6
#define LCD_E  P1_7
#define LCD_D7 P2_7
#define LCD_D6 P2_6
#define LCD_D5 P2_5
#define LCD_D4 P2_4
#define LCD_D3 P2_3
#define LCD_D2 P2_2
#define LCD_D1 P2_1
#define LCD_D0 P2_0
#define CHARS_PER_LINE 16

// Other non-lcd pins
#define MOTOR_L P0_5
#define MOTOR_R P0_4

//Tank circuit inputs
#define L_TANK
#define R_TANK
#define PERP_TANK

//Turning directions
#define NO_TURN 0
#define RIGHT_TURN 1
#define LEFT_TURN 2

//Minimum signal strength from tank input (assuming 1024 bin)
int thresh = 20;

int perpThreshLow = 20;
int perpThreshMed = 100;
int perpThreshHigh = 500;
//Base (straight line) movement speed. Max motor speed = 1
double baseSpeed = .8;
//Gain for differential steering
double p = .2

int[2] leftTankValue;
int[2] rightTankValue;
int[2] perpTankValue;
//vars for turning
int turnDirection
int perpCount = 0;
bool ready = true;


void init()
{
	pulse_width = 1000/pwm_frequency;
	cycles = 1000/(read_frequency * pulse_width);
}

void int main(int argc, char const *argv[])
{
	LCDprintln("Init...");
	initLCD();
	initPWM();
	init();
	LCDprintln("Done");
	while(1) {
		runTrack();
	}
	return 0;
}
void exicuteTurn( int turnDirection)
{
	if(turnDirection==LEFT_TURN)
	{
		//TODO:set motor speed for left turn
	}
	else if(turnDirection==RIGHT_TURN)
	{
		//TODO:set motor speed for right turn
	}
}

void setTurn(int perpCount) { 
	if (perpCount == 2)
	{
		//set turn direection to left
		turnDirection = LEFT_TURN;
	} 
	else if (perpCount == 3) 
	{
		//set turn direction to right
		turnDirection = RIGHT_TURN;
	}
}

void runTrack ()
{
	//TODO: read leftSignal and rightSignal
	readTank();
	if (perpTankValue > perpThreshMed) 
	{
		if (perpTankValue > perpThreshHigh && ready) 
		{
			perpCount++;
			ready = false;
		} 
		else 
		{
			ready = true;
		}
	} 
		ready = false;
		setTurn(perpCount);
	
	if (leftTankValue < thresh && rightTankValue < thresh) {
		runMotorSpeed(0,0);
	} else if (1) {
		
		(baseSpeed - (leftSignal-rightSignal)*p/1024, baseSpeed + (leftSignal-rightSignal)*p/1024);
	}

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

int threshParser(int value)
{
	int newValue;
	if(value>perpThreshHigh)
	{
		newValue=3;
	}
	else if(value<perpThreshHigh && value>perpThreshMed)
	{
		newValue=2;
	}
	else if(value<perpThreshMed && value>perpThreshLow)
	{
		newValue=1;
	}
	else
	{
		newValue=0;
	}
	return newValue;
}
void readTank(void)
{
	leftTankValue(0) =leftTankValue(1); 
	leftTankValue(1) = L_TANK;

	rightTankValue(0) = rightTankValue(1);
	rightTankValue(1) = R_TANK;

	perpTankValue(0) = perpTankValue(1);
	perpTankValue(1) = threshParser(PERP_TANK);
}