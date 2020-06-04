/*	Author: gyama009
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #11 Exercise #2
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
#include "io.h"
#include "scheduler.h"

// Enumeration of states.
enum process_States {process_wait, process_process};

int processSMTick(int state){
	// Local Variables
	unsigned char cnt;
	unsigned char arr[53] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'C', 'S', '1', '2', '0', 'B', ' ', 'i', 's', ' ', 'L', 'e', 'g', 'e', 'n', 'd', '.', '.', '.', ' ', 'w', 'a', 'i', 't', ' ', 'f', 'o', 'r', ' ', 'i', 't', ' ', 'D', 'A', 'R', 'Y', '!'};
	switch(state){ // State machine transitions
		case process_wait:
			cnt = 0;
			state = process_process;
			break;
		case process_process:
			cnt += 1;
			if(cnt > 53){
				cnt = 0;
			}
			state = process_process;
			break;
		default:
			state = process_process;
			break;
	}
	switch(state){ // State machine actions
		case process_wait:
			break;
		case process_process:
			for(unsigned char i = 0; i <= 15; i++){
				LCD_Cursor(i+1);
				LCD_WriteData(arr[((cnt + i) < 53) ? (cnt + i): (cnt + i -53)]);
			}
			break;
	}
	return state; 
}



// Implement schedulter code from PES.
int main() {
	DDRD = 0xFF; PORTD = 0x00;
	DDRB = 0xFF; PORTB = 0x00;

	LCD_init();
	LCD_ClearScreen();
	LCD_Cursor(0);

	// Declare an array of tasks
	static task task1;
	task *tasks[] = {&task1};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;

	// Task 1 (everything)
	task1.state = start; // Task initial state.
	task1.period = 250; // Task Period
	task1.elapsedTime = task1.period; // Task current elapsed time.
	task1.TickFct = &processSMTick; // Function pointer for the tick.

	// Set the timer and turn it on
	TimerSet(250);
	TimerOn();

	unsigned short i; // Scheduler for-loop iterator
	while (1) {
		for(i=0; i < numTasks;i++){ // Scheduler code
			if(tasks[i]->elapsedTime == tasks[i]->period){ // Task is ready to tick
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state); // Set next state
				tasks[i]->elapsedTime = 0; // Reset the elapsed time for next tick.
			}
			tasks[i]->elapsedTime += 250;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
	return 0; // Error: Program should not exit!
}
