/* Author: whe024
 * Partner(s) Name: Wentao He
 * Lab Section: A21
 * Assignment: Lab #11  Exercise #3
 * Exercise Description: [optional - include for your own benefit]
 *
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */
#include <avr/io.h>
#include "bit.h"
#include "timer.h"
#include "scheduler.h"
#include "keypad.h"
#include "io.h"
#include "io.c"
//#include <stdio.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

unsigned char led0_output = 0x00;
unsigned char led1_output = 0x00;
unsigned char pause = 0;

enum pauseButtonSM_States {pauseButton_wait, pauseButton_press, pauseButton_release};

int pauseButtonSMTick(int state)
{
  unsigned char press = GetKeypadKey();
  switch(state){
    case pauseButton_wait:
    	state = (press == '\0') ? pauseButton_wait : pauseButton_press;
    	break;
    case pauseButton_press:
    	state = pauseButton_release;
    	break;
    case pauseButton_release:
    	state = (press == '\0') ? pauseButton_wait : pauseButton_release;
    	break;
    default:
    	state = pauseButton_wait;
    	break;
  }
  switch(state){
    case pauseButton_wait:
    	break;
    case pauseButton_press:
    	switch(press){
      		case '\0':
			PORTB = 0x1F;
			break;
      		case '1': 
			LCD_ClearScreen();
        		LCD_Cursor(1);
			LCD_WriteData('1');
			break;
      		case '2':
        		LCD_ClearScreen(); 
        		LCD_Cursor(1);
        		LCD_WriteData('2');
        		break;
        	case '3':
        		LCD_ClearScreen(); 
        		LCD_Cursor(1);
        		LCD_WriteData('3');
        		break;
      		case '4':
        		LCD_ClearScreen(); 
        		LCD_Cursor(1);
        		LCD_WriteData('4');
        		break;
      		case '5':
        		LCD_ClearScreen(); 
        		LCD_Cursor(1);
        		LCD_WriteData('5');
        		break;
      		case '6':
        		LCD_ClearScreen(); 
        		LCD_Cursor(1);
        		LCD_WriteData('6');
        		break;
        	case '7':
        		LCD_ClearScreen(); 
        		LCD_Cursor(1);
        		LCD_WriteData('7');
        		break;
      		case '8':
        		LCD_ClearScreen(); 
        		LCD_Cursor(1);
        		LCD_WriteData('8');
        		break;
      		case '9':
        		LCD_ClearScreen(); 
        		LCD_Cursor(1);
        		LCD_WriteData('9');
        		break;
      		case 'A':
        		LCD_ClearScreen(); 
        		LCD_Cursor(1);
        		LCD_WriteData('A');
        		break;
      		case 'B':
        		LCD_ClearScreen(); 
        		LCD_Cursor(1);
        		LCD_WriteData('B');
        		break;
      		case 'C':
        		LCD_ClearScreen(); 
        		LCD_Cursor(1);
        		LCD_WriteData('C');
        		break;
      		case 'D':
        		LCD_ClearScreen(); 
        		LCD_Cursor(1);
        		LCD_WriteData('D');
        		break;
      		case '*':
        		LCD_ClearScreen(); 
        		LCD_Cursor(1);
        		LCD_WriteData('*');
        		break;
      		case '#':
        		LCD_ClearScreen(); 
        		LCD_Cursor(1);
        		LCD_WriteData('#');
        		break;
      		case '0':
        		LCD_ClearScreen(); 
        		LCD_Cursor(1);
        		LCD_WriteData('0');
        		break;
		default:
			PORTB = 0x1B;
			break;
    	}
    	break;
    case pauseButton_release:
    	break;
    default:
    	break;
  }
  return state;
 }

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0xF0;
    DDRC = 0xFF;
    DDRD = 0xFF;
    PORTA = 0x0F;
    PORTC = 0x00;
    PORTD = 0x00;
    /* Insert your solution below */
    LCD_init();

    static task task1;
    task *tasks[] = {&task1};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);    
    
    task1.state = pauseButton_wait;
    task1.period = 1;
    task1.elapsedTime = task1.period;
    task1.TickFct = &pauseButtonSMTick;

    unsigned long int GCD = 1;
    TimerSet(1);    
    TimerOn();

    unsigned short i;
    while (1) {
       for ( i = 0; i < numTasks; i++ ) {
          if ( tasks[i]->elapsedTime == tasks[i]->period ) {
             tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
             tasks[i]->elapsedTime = 0;
          }
          tasks[i]->elapsedTime += GCD;
       }
       while(!TimerFlag);
       TimerFlag = 0;
    }
    return 1;
}
