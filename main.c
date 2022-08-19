/********************************************************
Title			:Looping Louie PLUS
Description		:A ATiny85-20 PU program to alter the rotation speed of the table toy "Looping Louie"
Author			:Aimo Leffers

*********************************************************/
//Defines
#define F_CPU 8000000 // CPU Frequency

// PWM Settings (Timer0) to define the rotation speed
#define SPEED_0 0				// 0%
#define SPEED_1 51				// 20%
#define SPEED_2 64				// 25%
#define SPEED_3 77				// 30%
#define SPEED_4 89				// 35%
#define SPEED_5 204				// 80%
#define STANDARD_MODUS 1		// Standard operating mode
#define MAX_MODUS_RND_VAL 8		// possible steps 1 to 8
#define MAX_DURATION_RND_VAL 3		// possible steps 1 to 3

//Includes
#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>				// IO-Function Include
#include <avr/interrupt.h>
#include <stdbool.h>

//Function prototypes
void start_pwm(void);									// Initiate the PWM-Function of Timer0
int choose_mode(int cur_mode);							// Function to choose a new mode by random chance
bool mode1(bool new_mode);								// Mode 1: Starts slowly and gets faster over time
bool mode2(bool new_mode);								// Mode 2: Starts fast and gets slower over time
bool mode3(bool new_mode, int seconds, int duration);		// Mode 3: Stops for two seconds
bool mode4(bool new_mode, int seconds, int duration);		// Mode 4: constant speed, SPEED 1
bool mode5(bool new_mode, int seconds, int duration);		// Mode 5: constant speed, SPEED 2
bool mode6(bool new_mode, int seconds, int duration);		// Mode 6: constant speed, SPEED 3
bool mode7(bool new_mode, int seconds, int duration);		// Mode 7: constant speed, SPEED 4
bool mode8(bool new_mode, int seconds, int duration);		// Mode 8: constant speed, SPEED 5

//Global Vars
volatile unsigned int overflow_Counter = 0;
volatile unsigned int duty_cycle = 0;					// 0 (=0%) to 255 (=100%)
volatile bool t0_OVF_FLAG = false;

/********************************************************/
// main function
int main(void){

	// Variables
	unsigned int seconds = 0;
	unsigned int cur_mode = STANDARD_MODUS;
	bool new_mode = true;
	int duration = 0;
	
	// define the output ports
	DDRB |= (1 << DDB0);		// PB0 as output (PWM output of Timer 0)
	start_pwm();				// Start PWM of Timer0
	duty_cycle = SPEED_1;		// Defines the initial speed
	srand(45);					// Seed for the random number generator
	sei();						// enable global interrupts
	
	// main loop
	while(1) {
		// #### count seconds
		if (overflow_Counter >= 490) {
			// One count equals rouhly 2,04ms. therefor: 490 * 2,04ms = 999,6ms ~ 1s
			overflow_Counter = 0;
			if (seconds < 255) {
				seconds++;
			}
			else {
				// prevent overflow
				seconds = 0;
			}
		}
		// run the current operating mode
		switch(cur_mode) {
			case 1:
			new_mode = mode1(new_mode);
			break;
			case 2:
			new_mode = mode2(new_mode);
			break;
			case 3:
			new_mode = mode3(new_mode, seconds, duration);
			break;
			case 4:
			new_mode = mode4(new_mode, seconds, duration);
			break;
			case 5:
			new_mode = mode5(new_mode, seconds, duration);
			break;
			case 6:
			new_mode = mode6(new_mode, seconds, duration);
			break;
			case 7:
			new_mode = mode7(new_mode, seconds, duration);
			break;
			case 8:
			new_mode = mode8(new_mode, seconds, duration);
			break;
			default:
			cur_mode = 0;
			break;
			
		}
		// select a new mode (but not the same as before)
		if (new_mode) {
			cur_mode = choose_mode(cur_mode);
			seconds = 0; // reset time measurement for each new operating mode
			duration = rand() / (RAND_MAX / MAX_DURATION_RND_VAL + 1);
		}
		// reset the Timer0 overflow flag
		t0_OVF_FLAG = false;
	}
	return 0;
}

int choose_mode(int cur_mode) {
	int new_mode = cur_mode; 
	
	// choose a new mode at random and prevent that the same mode as before is chosen again
	while (new_mode == cur_mode) {
		new_mode = rand() / (RAND_MAX / MAX_MODUS_RND_VAL + 1);
	}
	return new_mode;
}

// Mode 1: Starts slowly and gets faster over time
bool mode1(bool new_mode) {
	if (new_mode) {
		new_mode = false;
		duty_cycle = SPEED_1;
	}
	if (t0_OVF_FLAG && !(new_mode)) {
		if ((overflow_Counter % 4) == 0){
			duty_cycle++;
		}
	}
	if (duty_cycle > SPEED_5){
		new_mode = true;
	}
	return new_mode;
}

// Mode 2: Starts fast and gets slower over time
bool mode2(bool new_mode)
{
	if (new_mode){
		new_mode = false;
		duty_cycle = SPEED_5;
	}
	if (t0_OVF_FLAG && !(new_mode))	{
		if ((overflow_Counter % 4) == 0){
			duty_cycle--;
		}
	}
	if (duty_cycle <= SPEED_1){
		new_mode = true;
	}
	return new_mode;
}

// Mode 3: Stops for two seconds
bool mode3(bool new_mode, int seconds, int duration){
	if (new_mode) {
		new_mode = false;
		duty_cycle = SPEED_0;
		DDRB &= ~(1 << DDB0);		// PB0 disabled
	}
	if (seconds >= duration){
		new_mode = true;
		DDRB |= (1 << DDB0);		// PB0 as output (PWM output of Timer0)
	}
	return new_mode;
}

// Mode 4: constant speed, SPEED 1
bool mode4(bool new_mode, int seconds, int duration) {
	if (new_mode) {
		// Start with Mode 3 to overcome the starting torque
		new_mode = false;
		duty_cycle = SPEED_3;
	}
	// Switch after one second to the actual speed
	if (seconds >= 1) {
		duty_cycle = SPEED_1;
	}
	if (seconds >= duration) {
		new_mode = true;
	}
	return new_mode;
}

// Mode 5: constant speed, SPEED 2
bool mode5(bool new_mode, int seconds, int duration) {
	// Start with Mode 3 to overcome the starting torque
	if (new_mode) {
		new_mode = false;
		duty_cycle = SPEED_3;
	}
	// Switch after one second to the actual speed
	if (seconds >= 1) 	{
		duty_cycle = SPEED_2;
	}
	if (seconds >= duration) {
		new_mode = true;
	}
	return new_mode;
}

// Mode 6: constant speed, SPEED 3
bool mode6(bool new_mode, int seconds, int duration) {
	if (new_mode) {
		new_mode = false;
		duty_cycle = SPEED_3;
	}
	if (seconds >= duration) {
		new_mode = true;
	}
	return new_mode;
}

// Mode 7: constant speed, SPEED 4
bool mode7(bool new_mode, int seconds, int duration) {
	if (new_mode) {
		new_mode = false;
		duty_cycle = SPEED_4;
	}
	if (seconds >= duration) {
		new_mode = true;
	}
	return new_mode;
}

// Mode 8: constant speed, SPEED 5
bool mode8(bool new_mode, int seconds, int duration) {
	if (new_mode) {
		new_mode = false;
		duty_cycle = SPEED_5;
	}
	if (seconds >= duration) {
		new_mode = true;
	}
	return new_mode;
}


void start_pwm() {
	TCCR0A |= (1<<WGM01) | (1<<WGM00);	// Enable Fast-PWM Mode @ Port PB0
	TCCR0A |= (1 << COM0A1);			// non-inverting mode; Set at Bottom, Clear on Match
	TCCR0B |= (1<<CS01) | (1<<CS00);	// Set Prescaler to 256 -> 31,25kHz
	TIMSK |= (1<<TOIE0);				// Enable Timer Overflow Interrupt
}

// interrupt service routine
ISR (TIMER0_OVF_vect) {
	OCR0A = duty_cycle;
	overflow_Counter++;
	t0_OVF_FLAG = true;
}