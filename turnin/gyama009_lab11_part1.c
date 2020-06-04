/*	Author: gyama009
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #11 Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#include "timer.h"
#include "keypad.h"
#include "bit.h"
#include "scheduler.h"

// Enumeration of states.
enum press_States {press_wait, press_press};

int pressSMTick(int state){
	// Local Variables
	unsigned char tmp;
	switch(state){
		case press_wait:
			tmp = GetKeypadKey();
			switch(tmp){
				case '\0':
					PORTB = 0x1F;
					break;
				case '1':
					PORTB = 0x01;
					break;
				case '2':
					PORTB = 0x02;
					break;
				case '3':
					PORTB = 0x03;
					break;
				case '4':
					PORTB = 0x04;
					break;
				case '5':
					PORTB = 0x05;
					break;
				case '6':
					PORTB = 0x06;
					break;
				case '7':
					PORTB = 0x07;
					break;
				case '8':
					PORTB = 0x08;
					break;
				case '9':
					PORTB = 0x09;
					break;
				case 'A':
					PORTB = 0x0A;
					break;
				case 'B':
					PORTB = 0x0B;
					break;
				case 'C':
					PORTB = 0x0C;
					break;
				case 'D':
					PORTB = 0x0D;
					break;
				case '*':
					PORTB = 0x0E;
					break;
				case '0':
					PORTB = 0x00;
					break;
				case '#':
					PORTB = 0x0F;
					break;
				default:
					PORTB = 0x1B;
					break;
			}
			state = press_wait;
			break;
		default:
			state = press_wait;
	}
	switch(state){
		case press_wait:
			break;
	}
	return state; 
}



// Implement schedulter code from PES.
int main() {
	DDRC = 0xF0; PORTC = 0x0F; // Keypad Inputs
	DDRB = 0xFF; PORTB = 0x00; // LED Outputs

	// Declare an array of tasks
	static task task1;
	task *tasks[] = {&task1};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;

	// Task 1 (everything)
	task1.state = start; // Task initial state.
	task1.period = 10; // Task Period
	task1.elapsedTime = task1.period; // Task current elapsed time.
	task1.TickFct = &pressSMTick; // Function pointer for the tick.

	// Set the timer and turn it on
	TimerSet(10);
	TimerOn();

	unsigned short i; // Scheduler for-loop iterator
	while (1) {
		for(i=0; i < numTasks;i++){ // Scheduler code
			if(tasks[i]->elapsedTime == tasks[i]->period){ // Task is ready to tick
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state); // Set next state
				tasks[i]->elapsedTime = 0; // Reset the elapsed time for next tick.
			}
			tasks[i]->elapsedTime += 10;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
	return 0; // Error: Program should not exit!
}
