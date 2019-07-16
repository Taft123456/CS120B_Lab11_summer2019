#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1; 
unsigned long _avr_timer_cntcurr = 0; 

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void TimerOn() {
	TCCR1B 	= 0x0B;	
	OCR1A 	= 125;	
	TIMSK1 	= 0x02; 
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;	
}

void TimerOff() {
	TCCR1B 	= 0x00; 
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect)
{
	_avr_timer_cntcurr--; 			
	if (_avr_timer_cntcurr == 0) { 	
		TimerISR(); 				
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

unsigned char SetBit(unsigned char pin, unsigned char number, unsigned char bin_value)
{
	return (bin_value ? pin | (0x01 << number) : pin & ~(0x01 << number));
}

unsigned char GetBit(unsigned char port, unsigned char number)
{
	return ( port & (0x01 << number) );
}

unsigned long int findGCD(unsigned long int a, unsigned long int b)
{
	unsigned long int c;
	while(1){
		c = a % b;
		if( c == 0 ) { return b; }
		a = b;
		b = c;
	}
	return 0;
}

typedef struct _task{
	signed 	 char state; 		
	unsigned long period; 		
	unsigned long elapsedTime; 	
	int (*TickFct)(int); 		
} task;

unsigned char GetKeypadKey() {
    PORTC = 0xEF; 
    asm("nop"); 
    if (GetBit(PINC,0)==0) 
	{ return('1'); }
    if (GetBit(PINC,1)==0) 
	{ return('4'); }
    if (GetBit(PINC,2)==0) 
	{ return('7'); }
    if (GetBit(PINC,3)==0) 
	{ return('*'); }

    PORTC = 0xDF; 
    asm("nop"); 
    if (GetBit(PINC,0)==0) 
	{ return('2'); }
    if (GetBit(PINC,1)==0) 
	{ return('5'); }
    if (GetBit(PINC,2)==0) 
	{ return('8'); }
    if (GetBit(PINC,3)==0) 
	{ return('0'); }

    PORTC = 0xBF; 
    asm("nop"); 
    if (GetBit(PINC,0)==0) 
	{ return('3'); }
    if (GetBit(PINC,1)==0) 
	{ return('6'); }
    if (GetBit(PINC,2)==0) 
	{ return('9'); }
    if (GetBit(PINC,3)==0) 
	{ return('#'); }

    PORTC = 0x7F; 
    asm("nop");
    if (GetBit(PINC,0)==0) 
	{ return('A'); }
    if (GetBit(PINC,1)==0) 
	{ return('B'); }
    if (GetBit(PINC,2)==0) 
	{ return('C'); }
    if (GetBit(PINC,3)==0) 
	{ return('D'); }

    return('\0');
}

enum States {PRESS};
	
int Tick(int state){
    static unsigned char press;
    press = GetKeypadKey();
    switch(state) {
        case PRESS:    
            switch (press) {
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
			break;
			default:
				state = PRESS;
				break;
    }
    return state;    
}

int main(void)
{
    DDRA = 0x00; 
    DDRB = 0xFF; 
    DDRC = 0xF0; 
    DDRD = 0xFF; 
	PORTA = 0x00;
	PORTB = 0x00;
	PORTC = 0x0F;
	PORTD = 0x00;
    
    unsigned long keypad_time = 10;
    static task task1;
    task *tasks[] = {&task1};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    
    task1.state = -1;
    task1.period = keypad_time;
    task1.elapsedTime = keypad_time; 
    task1.TickFct = &Tick; 
  
    TimerSet(5);
    TimerOn();

    unsigned short i;    
    while(1){
       for (i = 0; i < numTasks; i++) {
           if (tasks[i]->elapsedTime == tasks[i]->period) {
               tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
               tasks[i]->elapsedTime = 0;
           }
           tasks[i]->elapsedTime += 1;
       }
       while(!TimerFlag);
       TimerFlag = 0;
    }
    return 0;
}


