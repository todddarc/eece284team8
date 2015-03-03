
#include <stdlib.h>
#include <stdio.h>
#include <p89lpc9351.h>
#include <math.h>
#include "pwmControl.h"
#include "lcdControl.h"

#define XTAL 7373000L
#define BAUD 115200L

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
#define STOP_TURN 3

//wheelbase in inches
#define WHEELBASE 5
//max speed in inches/second
#define SPEED_CONVERSION 10

int perpThreshLow = 20;
int perpThreshMed = 100;
int perpThreshHigh = 500;
//Base (straight line) movement speed. Max motor speed = 1
double baseSpeed = .8;
//Gain for differential steering
double p = .2;

int[2] leftTankValue;
int[2] rightTankValue;
int[2] perpTankValue;

//vars for turning
int turnDirection;
int perpCount = 0;

bool kill = false;


void init()
{
	
}

void int main(int argc, char const *argv[])
{
	LCDprintln("Init...");
	initLCD();
	initPWM();
	init();
	LCDprintln("Done");
	while(!kill) {
		LCDprintln("GOTTA GO FAST!");
		runTrack();
	}
	LCDprintln("Robot is Kill");
	return 0;
}
//turn radius given in inches
void executeTurn( int turnDirection, double speed, double radius)
{	
	if (radius != 0 && speed > 1.0/(1+WHEELBASE/(2*radius))) {
		speed = 1.0/(1+WHEELBASE/(2*radius));
	}

	double speed_in = radius == 0 ? -speed : speed*(1-WHEELBASE/(2*radius));
	double speed_out = radius == 0 ? speed : speed*(1+WHEELBASE/(2*radius));

	double speed_l = 0;
	double speed_r = 0; 

	switch(turnDirection) {

		case NO_TURN: 
			speed_l = speed;
			speed_r = speed;
			break;
		case LEFT_TURN: 
			speed_l = speed_in;
			speed_r = speed_out;
			break;
		case RIGHT_TURN: 
			speed_l = speed_out;
			speed_r = speed_in;
			break;
		case STOP_TURN:
			kill = true;
			return;
	}

	setPWM(speed_l, speed_r);
	waitms(SPEED_CONVERSION*1000*0.5*M_PI/speed);
}	

void setTurn(int perpCount) { 
	switch(perpCount) {
		case 2: 
			turnDirection = LEFT_TURN;
			break;
		case 3: 
			turnDirection = RIGHT_TURN;
			break;
		case 4: turnDirection = STOP_TURN;
	}	
}

void runTrack ()
{
	//TODO: read leftSignal and rightSignal
	readTank();
	if (perpTankValue(1) == 2 && perpTankValue(0) == 3) {
		perpCount++;
	} else if (perpTankValue(1) == 2 && perpTankValue(0) ==1 && perpCount !=0) {
		setTurn(perpCount);
		executeTurn(turnDirection, baseSpeed, 3.0);
		perpCount = 0;
	} else if (perpTankValue(1) == 1 && perpTankValue(0) == 2 && perpCount == 1) {
		//1 is invalid number for turn signal: 
		//must be a wire crossing the track, not a signal from signal box
		perpCount = 0;
	}
		
		setPWM(baseSpeed - (leftTankValue(1)- rightTankValue(1))*p/1024, baseSpeed + (leftTankValue(1)- rightTankValue(1))*p/1024);


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
	else if(value>perpThreshMed)
	{
		newValue=2;
	}
	else if(value>perpThreshLow)
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