/*	Author: gyama009
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #11 Exercise #3
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
#include "io.h"

//-----Shared Variables--------------------------
unsigned char keypad;
//----End Shared Variables-----------------------

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
					keypad = 0xFF;
					break;
				case '1':
					keypad = '1';
					break;
				case '2':
					keypad = '2';
					break;
				case '3':
					keypad = '3';
					break;
				case '4':
					keypad = '4';
					break;
				case '5':
					keypad = '5';
					break;
				case '6':
					keypad = '6';
					break;
				case '7':
					keypad = '7';
					break;
				case '8':
					keypad = '8';
					break;
				case '9':
					keypad = '9';
					break;
				case 'A':
					keypad = 'A';
					break;
				case 'B':
					keypad = 'B';
					break;
				case 'C':
					keypad = 'C';
					break;
				case 'D':
					keypad = 'D';
					break;
				case '*':
					keypad = 'E';
					break;
				case '0':
					keypad = '0';
					break;
				case '#':
					keypad = '#';
					break;
				default:
					keypad = 0x1B;
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

// Enumeration of states.
enum LCD_States {LCD_wait, LCD_process};

int LCDSMTick(int state){
	switch(state){ // State machine transitions
		case LCD_wait:
			if(keypad == 0xFF){
				state = LCD_wait;
			}
			else if(keypad != 0xFF){
				LCD_Cursor(1);
				LCD_WriteData(keypad);
				state = LCD_process;
			}
			break;
		case LCD_process:
			if(keypad == 0xFF){
				state = LCD_wait;
			}
			else if (keypad != 0xFF){
				state = LCD_process;
			}
			break;
		default:
			state = LCD_wait;
			break;
	}
	switch(state){ // State machine actions
		case LCD_wait:
			break;
		case LCD_process:
			break;
	}
	return state;
}

// Implement schedulter code from PES.
int main() {
	DDRC = 0xF0; PORTC = 0x0F; // Keypad Inputs
	DDRB = 0xFF; PORTB = 0x00; // LCD
	DDRD = 0xFF; PORTD = 0x00; // LCD

	LCD_init();
	LCD_ClearScreen();
	LCD_Cursor(1);

	// Declare an array of tasks
	static task task1, task2;
	task *tasks[] = {&task1, &task2};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;

	// Task 1 (everything)
	task1.state = start; // Task initial state.
	task1.period = 10; // Task Period
	task1.elapsedTime = task1.period; // Task current elapsed time.
	task1.TickFct = &pressSMTick; // Function pointer for the tick.

	task2.state = start; // Task initial state.
	task2.period = 10; // Task Period
	task2.elapsedTime = task2.period; // Task current elapsed time.
	task2.TickFct = &LCDSMTick; // Function pointer for the tick.

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
