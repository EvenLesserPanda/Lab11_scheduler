/*	Author: gyama009
 *  Partner(s) Name: 
 *	Lab Section: 022
 *	Assignment: Lab #11 Exercise #4
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
unsigned char power;
unsigned char cnt1;
unsigned char cnt2;
unsigned char player;
unsigned char message;
//----End Shared Variables-----------------------

// Enumeration of states.
enum power_States {power_offrelease, power_onpress, power_onrelease, power_offpress};

int powerSMTick(int state){
	// Local Variables
	switch(state){ // State machine transitions
		case power_offrelease:
			if((~PINA & 0xFF) == 0x01){
				power = 0x01;
				state = power_onpress;
			}
			else if((~PINA & 0xFF) == 0x00){
				state = power_offrelease;
			}
			break;
		case power_onpress:
			if((~PINA & 0xFF) == 0x01){
				state = power_onpress;
			}
			else if((~PINA & 0xFF) == 0x00){
				state = power_onrelease;
			}
			break;
		case power_onrelease:
			if((~PINA & 0xFF) == 0x01){
				power = 0x00;
				state = power_offpress;
			}
			else if((~PINA & 0xFF) == 0x00){
				//state = power_onrelease;
				if(message != 0x01){
					state = power_onrelease;
				}
				else if(message == 0x01){
					message = 0x00;
					state = power_offrelease;
				}
			}
			break;
		case power_offpress:
			if((~PINA & 0xFF) == 0x01){
				state = power_offpress;
			}
			else if((~PINA & 0xFF) == 0x00){
				state = power_offrelease;
			}
			break;
		default:
			state = power_offrelease;
			break;
	}
	switch(state){ // State machine actions
		case power_offrelease:
			break;
		case power_onpress:
			break;
		case power_onrelease:
			break;
		case power_offpress:
			break;
	}
	return state;
}

// Enumeration of States
enum player_States {player_wait, player_action};

int playerSMTick(int state){
	switch(state){ // State machine transitions
		case player_wait:
			if(power == 0x01){
				player = 2;
				state = player_action;
			}
			else if(power == 0x00){
				state = player_wait;
			}
			break;
		case player_action:
			if(power == 0x00){
				state = player_wait;
			}
			else if(power == 0x01){
				if((~PINA & 0xFF) == 0x02){
					LCD_Cursor(2);
					player = 2;
				}
				else if((~PINA & 0xFF) == 0x04){
					LCD_Cursor(18);
					player = 18;
				}
				state = player_action;
			}
			break;
		default:
			state = player_wait;
			break;
	}
	switch(state){ // State machine actions
		case player_wait:
			break;
		case player_action:
			LCD_Cursor(player);
			break;
	}
	return state;
}

enum enemy_States {enemy_init, enemy_wait, enemy_process, enemy_remove};

int enemySMTick(int state){
	switch(state){
		case enemy_init:
			cnt1 = 9;
			cnt2 = 29;
			state = enemy_wait;	
			break;
		case enemy_wait:
			if(power == 0x00){
				state = enemy_wait;
			}
			else if(power == 0x01){
				LCD_ClearScreen();
				state = enemy_process;
			}
			break;
		case enemy_process:
			if(power == 0x00){
				state = enemy_wait;
			}
			else if(power == 0x01){
				state = enemy_remove;
			}
			break;
		case enemy_remove:
			if(power == 0x00){
				state = enemy_wait;
			}
			else if(power == 0x01){
				cnt1 = (cnt1 == 1) ? 16 : (cnt1 - 1);
                                cnt2 = (cnt2 == 17) ? 32 : (cnt2 - 1);
				state = enemy_process;
			}
			break;
		default:
			state = enemy_init;
			break;
	}
	switch(state){
		case enemy_init:
			LCD_Cursor(9);
			LCD_WriteData('#');
			LCD_Cursor(29);
			LCD_WriteData('#');
			LCD_Cursor(2);
			break;
		case enemy_wait:
			break;
		case enemy_process:
			LCD_Cursor(cnt1);
			LCD_WriteData('#');
			LCD_Cursor(0);
			LCD_Cursor(cnt2);
			LCD_WriteData('#');
			LCD_Cursor(0);
			break;
		case enemy_remove:
			LCD_Cursor(cnt1);
			LCD_WriteData(' ');
			LCD_Cursor(0);
			LCD_Cursor(cnt2);
			LCD_WriteData(' ');
			LCD_Cursor(0);
			break;
	}
	return state;
}

// Enumeration of states.
enum message_States{message_wait, message_process, message_message};

int messageSMTick(int state){
	switch(state){ // State machine transitions
		case message_wait:
			if(power == 0x00){
				state = message_wait;
			}
			else if(power == 0x01){
				state = message_process;
			}
			break;
		case message_process:
			if(power == 0x00){
				state = message_wait;
			}
			else if((power == 0x01) && ((player != cnt1) && (player != cnt2))){
				state = message_process;
			}
			else if((power == 0x01) && ((player == cnt1) || (player == cnt2))){
				state = message_message;
			}
			break;
		case message_message:
			state = message_wait;
			break;
		default:
			state = message_wait;
			break;
	}
	switch(state){ // State machine actions
		case message_wait:
			break;
		case message_process:
			break;
		case message_message:
			LCD_ClearScreen();
                        LCD_DisplayString(1, "   Hurray for   Embedded Systems");
			player = 2;
			cnt1 = 9;
			cnt2 = 29;
			message = 0x01;
                        power = 0x00;
			break;
	}
	return state;
}




// Implement schedulter code from PES.
int main() {
	DDRA = 0x00; PORTA = 0xFF; // Button Inputs
	DDRB = 0xFF; PORTB = 0x00; // LCD
	DDRD = 0xFF; PORTD = 0x00; // LCD

	LCD_init();
	LCD_ClearScreen();
	LCD_Cursor(2);

	// Declare an array of tasks
	static task task1, task2, task3, task4;
	task *tasks[] = {&task1, &task2, &task3, &task4};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;

	// Task 1
	task1.state = start; // Task initial state.
	task1.period = 10; // Task Period
	task1.elapsedTime = task1.period; // Task current elapsed time.
	task1.TickFct = &powerSMTick; // Function pointer for the tick.
	
	// Task 2
	task2.state = start; // Task initial state.
	task2.period = 10; // Task Period
	task2.elapsedTime = task2.period; // Task current elapsed time.
	task2.TickFct = &playerSMTick; // Function pointer for the tick.

	// Task 3
	task3.state = start; // Task initial state.
	task3.period = 100; // Task Period
	task3.elapsedTime = task3.period; // Task current elapsed time.
	task3.TickFct = &enemySMTick; // Function pointer for the tick.

	// Task 4
	task4.state = start; // Task initial state.
	task4.period = 100; // Task Period
	task4.elapsedTime = task4.period; // Task current elapsed time.
	task4.TickFct = &messageSMTick; // Function pointer for the tick.

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
