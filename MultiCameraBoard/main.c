/*
 * MultiCameraBoard.c
 *
 * Created: 27.06.2020 18:49:36
 * Author : kpy17
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>

#define BAUD 9600UL
#define UBR0_Value ((F_CPU/(16*BAUD)) -1)

void uart_init()
{
	UBRR0H = (UBR0_Value >> 8); //Baudrate register High
	UBRR0L = UBR0_Value;  //and low
	
	UCSR0B = (1<<TXEN0) | (1<<RXEN0); //usart control and status B register, receive complete: RX and TX enable
	UCSR0C = (1<<UCSZ00) | (1<<UCSZ01); //set to 8bit (C-register UCSZ settings)
}

char uart_getnew()
{
	while(!(UCSR0A & (1<<RXC0))) //wait until register is ready
	;
	return UDR0; //returns register with character
}


void uart_readnew(char *calledstring)
{
	DDRD |= (1<<5);
	char ch;
	int cntr = 0;
	
	while (1)
	{
		ch = uart_getnew();
			
		if (ch == 13) //if new line
		{
			calledstring[cntr] = 0; //set to 0 -> is called later to check
			return;
		}
		else
		{
			calledstring[cntr] = ch;
			cntr++;
		}
	}
}


void uart_outputchar(char ch)
{
	while(!(UCSR0A &(1<<UDRE0))) //check if register is ready to send something
	;
	UDR0 = ch;
}

//output function
void uart_output(char *calledstring, size_t l_puffer)
{
	for (int i=0; i<l_puffer; i++)
	{
		if(calledstring[i] != 0)
		{
			uart_outputchar(calledstring[i]);
		}
		else
		{
			break;
		}
	}
}

//set all output pins to high
void outpins_all_high_init()
{
	PORTC |= (1<<0);
	PORTC |= (1<<1);
	PORTC |= (1<<2);
	PORTC |= (1<<3);
	PORTC |= (1<<4);
	PORTC |= (1<<5);
	PORTD |= (1<<2);
	PORTD |= (1<<3);
	PORTD |= (1<<4);
	PORTD |= (1<<5);
	PORTD |= (1<<6);
	PORTD |= (1<<7);
}


int main(void)
{
	//initialize output pins
	DDRD = 0b11111100; //Port D Data register, ones are outputs PORTD2-PORTD7
	DDRC = 0b00111111; //Port C register PORTC0-PORTC5 as outputs
	_delay_ms(50);
	outpins_all_high_init();
	
	char mode[2];
	char pos[3];
	int slotNum;
	int modeNum;
	
	char INPUT[255];
	uart_init();

    /* Replace with your application code */
    while (1) 
    {
		_delay_ms(100);
		uart_readnew(INPUT);
		
		mode[0] = INPUT[1];
		mode[1] = '\0';
		pos[0] = INPUT[2];
		pos[1] = INPUT[3];
		pos[2] = '\0';
		slotNum = atoi(pos);
		modeNum = atoi(mode);
		
		if ( (modeNum == 0) & (slotNum == 0)) //000
		{
			outpins_all_high_init();
			uart_output(INPUT,255);
		}
		else if((modeNum == 1) & ~(slotNum == 0))
		{
			
			if ((slotNum>6) & (slotNum<13)) //switch pin on PORT D bank, PORTD2-PORTD7
			{
				slotNum= slotNum -5;
				PORTD |= (1<<slotNum); //switch slotNum to LOW leave rest unchanged
				uart_output(INPUT,255);
			}
			else if ((slotNum<7) & (slotNum>0)) //switch pin on PORT C bank, PORTC0-PORTC5
			{
				slotNum= slotNum -1;
				PORTC |= (1<<slotNum); //switch slotNum to LOW leave rest unchanged
				uart_output(INPUT,255);
			}
			else
			{
				char *msg = "invalid input";
				uart_output(msg,13);
			}
		}
		else if( (modeNum == 0) & ~(slotNum == 0))
		{
			
			if ((slotNum>6) & (slotNum<13)) //switch pin on PORT D bank, PORTD2-PORTD7
			{
				slotNum= slotNum -5;
				PORTD &= ~(1<<slotNum); //switch slotNum to LOW leave rest unchanged
				uart_output(INPUT,255);
			}
			else if ((slotNum<7) & (slotNum>0)) //switch pin on PORT C bank, PORTC0-PORTC5
			{
				slotNum= slotNum -1;
				PORTC &= ~(1<<slotNum); //switch slotNum to LOW leave rest unchanged
				uart_output(INPUT,255);
			}
			else
			{
				char *msg = "invalid input";
				uart_output(msg,13);
			}
		}
		else
		{
			char *msg = "invalid input";
			uart_output(msg,13);
		}
			
		
		//uart_output(INPUT,255);
		uart_output("\r\n",2); //carriage return and newline 
		
		/*
		if (slotNum>7)
		{
			char *msg = "invalid input\r\n";
			uart_output(msg,15);
			PORTD &= ~(1<<2);
		}
		
		uart_output(INPUT,255);
		uart_output("\r\n",2);
		_delay_ms(1000);
		uart_output(mode,2);
		uart_output("\r\n",2);
		_delay_ms(500);
		uart_output(pos,3);
		uart_output("\r\n",2);
		_delay_ms(500);*/
	
	
    }
}

