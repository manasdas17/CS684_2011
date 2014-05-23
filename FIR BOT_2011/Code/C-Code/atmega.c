/********************************************************************************
Title: Firebird main file

Authors: Arjun Joshi
	 	 Prashant Bhut
	 	 Manish Patidar
		 
Version: 1.0

********************************************************************************/
/********************************************************************************

   Copyright (c) 2011, ERTS Lab IIT Bombay erts@cse.iitb.ac.in            
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.

   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

   * Source code can be used for academic purpose. 
	 For commercial use permission form the author needs to be taken.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. 

  Software released under Creative Commence cc by-nc-sa licence.
  For legal information refer to: 
  http://creativecommons.org/licenses/by-nc-sa/3.0/legalcode

*********************************************************************************/

// Header files to be included
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

// Global definitions 
#define RS 0
#define RW 1
#define EN 2
#define lcd_port PORTC

#define sbit(reg,bit)	reg |= (1<<bit)			// Macro defined for Setting a bit of any register.
#define cbit(reg,bit)	reg &= ~(1<<bit)		// Macro defined for Clearing a bit of any register.

// Function prototype declarations
void port_init();
void lcd_port_config();
void lcd_reset();
void lcd_init();
void lcd_set_4bit();
void uart1_init();
void uart0_init();
void lcd_wr_command(unsigned char);
void lcd_wr_char(char);
void lcd_string(char*);

unsigned int temp;
unsigned int unit;
unsigned int tens;
unsigned int hundred;
unsigned int thousand;
unsigned int million;

// Variables for sound triangulation
float data[4];						// variable to hold data received from UART
int counter = 0;					// flag to indicated data from all 4 channel 
									// received from dsPIC

// Function to configure pins for motion configuration
void motion_pin_config (void)
{
 	DDRA = DDRA | 0x0F;
 	PORTA = PORTA & 0xF0;
 	DDRL = DDRL | 0x18;   			//Setting PL3 and PL4 pins as output for PWM generation
 	PORTL = PORTL | 0x18; 			//PL3 and PL4 pins are for velocity control using PWM.
}

//Function to configure LCD port
void lcd_port_config (void)
{
 	DDRC = DDRC | 0xF7; 			// all the LCD pin's direction set as output
	PORTC = PORTC & 0x80; 			// all the LCD pins are set to logic 0 except PORTC 7
}

//Function to Initialize PORTS
void port_init()
{
	lcd_port_config();				// calling lcd port config 
	motion_pin_config();			// calling config function for motion control
}

//Function used for setting motor's direction
void motion_set (unsigned char Direction)
{
 	unsigned char PortARestore = 0;

 	Direction &= 0x0F; 				// removing upper nibbel as it is not needed
 	PortARestore = PORTA; 			// reading the PORTA's original status
	 PortARestore &= 0xF0; 			// setting lower direction nibbel to 0
 	PortARestore |= Direction; 		// adding lower nibbel for direction command and
									// restoring the PORTA status
 	PORTA = PortARestore; 			// setting the command to the port
}

// Various functions for motor functioning
void forward (void) //both wheels forward
{
  motion_set(0x06);
}

void back (void) //both wheels backward
{
  motion_set(0x09);
}

void left (void) //Left wheel backward, Right wheel forward
{
  motion_set(0x05);
}

void right (void) //Left wheel forward, Right wheel backward
{
  motion_set(0x0A);
}

void soft_left (void) //Left wheel stationary, Right wheel forward
{
 motion_set(0x04);
}

void soft_right (void) //Left wheel forward, Right wheel is stationary
{
 motion_set(0x02);
}

void soft_left_2 (void) //Left wheel backward, right wheel stationary
{
 motion_set(0x01);
}

void soft_right_2 (void) //Left wheel stationary, Right wheel backward
{
 motion_set(0x08);
}

void stop (void)
{
  motion_set(0x00);
}

//Function to Reset LCD
void lcd_set_4bit()
{
	_delay_ms(1);

	cbit(lcd_port,RS);				//RS=0 --- Command Input
	cbit(lcd_port,RW);				//RW=0 --- Writing to LCD
	lcd_port = 0x30;				//Sending 3
	sbit(lcd_port,EN);				//Set Enable Pin
	_delay_ms(5);					//Delay
	cbit(lcd_port,EN);				//Clear Enable Pin

	_delay_ms(1);

	cbit(lcd_port,RS);				//RS=0 --- Command Input
	cbit(lcd_port,RW);				//RW=0 --- Writing to LCD
	lcd_port = 0x30;				//Sending 3
	sbit(lcd_port,EN);				//Set Enable Pin
	_delay_ms(5);					//Delay
	cbit(lcd_port,EN);				//Clear Enable Pin

	_delay_ms(1);

	cbit(lcd_port,RS);				//RS=0 --- Command Input
	cbit(lcd_port,RW);				//RW=0 --- Writing to LCD
	lcd_port = 0x30;				//Sending 3
	sbit(lcd_port,EN);				//Set Enable Pin
	_delay_ms(5);					//Delay
	cbit(lcd_port,EN);				//Clear Enable Pin

	_delay_ms(1);

	cbit(lcd_port,RS);				//RS=0 --- Command Input
	cbit(lcd_port,RW);				//RW=0 --- Writing to LCD
	lcd_port = 0x20;				//Sending 2 to initialise LCD 4-bit mode
	sbit(lcd_port,EN);				//Set Enable Pin
	_delay_ms(5);					//Delay
	cbit(lcd_port,EN);				//Clear Enable Pin

	
}

//Function to Initialize LCD
void lcd_init()
{
	_delay_ms(1);

	lcd_wr_command(0x28);			//LCD 4-bit mode and 2 lines.
	lcd_wr_command(0x01);
	lcd_wr_command(0x06);
	lcd_wr_command(0x0E);
	lcd_wr_command(0x80);
		
}

//Function To Initialize UART1 to communicate with dsPIC
// baud rate:9600
// char size: 8 bit
// parity: Disabled
void uart1_init(void)
{
 	UCSR1B = 0x00; 					//disable RX and TX while setting baud rate
 	UCSR1A = 0x00;					//Clearing RX and TX complete flag, baud rate divisor is 16
	UCSR1C = 0x06;					//8 bit data, no parity
 	UBRR1H = 0x00; 					//set baud rate hi
	UBRR1L = 0x5F; 					//set baud rate lo
 	UCSR1B = 0x98;					//RX interrupt enabled, TX interrupt disabled
									//RX and TX enabled
}

//Function To Initialize UART0 for zigbee communication with PC
// baud rate:9600
// char size: 8 bit
// parity: Disabled
void uart0_init(void)
{
 	UCSR0B = 0x00; 					//disable RX and TX while setting baud rate
 	UCSR0A = 0x00;					//Clearing RX and TX complete flag, baud rate divisor is 16
	UCSR0C = 0x06;					//8 bit data, no parity
 	UBRR0H = 0x00; 					//set baud rate hi
	UBRR0L = 0x5F; 					//set baud rate lo
 	UCSR0B = 0x18;					//RX and TX interrupt disabled
									//RX and TX enabled
}

SIGNAL(SIG_USART1_RECV) 			// ISR for receive complete interrupt for UART1
{
	data[counter++] = UDR1;			// filtered data from dsPIC stored in user buffer

	// Once 4 channel data recieved UART 1 RX interrupt disabled
	if(counter == 4)
		UCSR1B = 0x18;				// RX interrupt disabled when gunshot data received
	//lcd_wr_char(data[counter]);	// using lcd function for debugging purposes
}

//Function to Write Command on LCD
void lcd_wr_command(unsigned char cmd)
{
	unsigned char temp;
	temp = cmd;
	temp = temp & 0xF0;
	lcd_port &= 0x0F;
	lcd_port |= temp;
	cbit(lcd_port,RS);
	cbit(lcd_port,RW);
	sbit(lcd_port,EN);
	_delay_ms(5);
	cbit(lcd_port,EN);
	
	cmd = cmd & 0x0F;
	cmd = cmd<<4;
	lcd_port &= 0x0F;
	lcd_port |= cmd;
	cbit(lcd_port,RS);
	cbit(lcd_port,RW);
	sbit(lcd_port,EN);
	_delay_ms(5);
	cbit(lcd_port,EN);
}

//Function to Write Data on LCD
void lcd_wr_char(char letter)
{
	char temp;
	temp = letter;
	temp = (temp & 0xF0);
	lcd_port &= 0x0F;
	lcd_port |= temp;
	sbit(lcd_port,RS);
	cbit(lcd_port,RW);
	sbit(lcd_port,EN);
	_delay_ms(5);
	cbit(lcd_port,EN);

	letter = letter & 0x0F;
	letter = letter<<4;
	lcd_port &= 0x0F;
	lcd_port |= letter;
	sbit(lcd_port,RS);
	cbit(lcd_port,RW);
	sbit(lcd_port,EN);
	_delay_ms(5);
	cbit(lcd_port,EN);
}


//Function to bring cursor at home position
void lcd_home()
{
	lcd_wr_command(0x80);
}


//Function to Print String on LCD
void lcd_string(char *str)
{
	while(*str != '\0')
	{
		lcd_wr_char(*str);
		str++;
	}
}

//Position the LCD cursor at "row", "column".

void lcd_cursor (char row, char column)
{
	switch (row) {
		case 1: lcd_wr_command (0x80 + column - 1); break;
		case 2: lcd_wr_command (0xc0 + column - 1); break;
		case 3: lcd_wr_command (0x94 + column - 1); break;
		case 4: lcd_wr_command (0xd4 + column - 1); break;
		default: break;
	}
}

//Function To Print Any input value upto the desired digit on LCD
void lcd_print (char row, char coloumn, unsigned int value, int digits)
{
	unsigned char flag=0;
	if(row==0||coloumn==0)
	{
		lcd_home();
	}
	else
	{
		lcd_cursor(row,coloumn);
	}
	if(digits==5 || flag==1)
	{
		million=value/10000+48;
		lcd_wr_char(million);
		flag=1;
	}
	if(digits==4 || flag==1)
	{
		temp = value/1000;
		thousand = temp%10 + 48;
		lcd_wr_char(thousand);
		flag=1;
	}
	if(digits==3 || flag==1)
	{
		temp = value/100;
		hundred = temp%10 + 48;
		lcd_wr_char(hundred);
		flag=1;
	}
	if(digits==2 || flag==1)
	{
		temp = value/10;
		tens = temp%10 + 48;
		lcd_wr_char(tens);
		flag=1;
	}
	if(digits==1 || flag==1)
	{
		unit = value%10 + 48;
		lcd_wr_char(unit);
	}
	if(digits>5)
	{
		lcd_wr_char('E');
	}
	
}

void init_devices (void)
{
 	SREG = 0x00; 					// Clears the global interrupts
	port_init();					// Initializes all the ports
	SREG = 0x80;   					// Enables the global interrupts
}		

//Main Function
int main(void)
{
	int channel;					// sensor whose value is max
	float max_value = 0;			// max value of sensors
	int i;
	float S1,S2;					// The nearest sensors value to be used for triangulation
	float angle;					// Angle to move the FireBird
	float offset;					
	int delay;						// delay before fire bird is stopped for taking the image

	init_devices();					// init all port for devices
	lcd_set_4bit();					// set LCD in 4 bit mode
	lcd_init();						// init LCD module
	uart1_init();					// init uart module for RS232 with dsPIC
	uart0_init();					// init uart module for Zigbee
	

//	lcd_string("LCD Check");
	while(1)
	{	
		// If 4 channel data is received then this block is executed
		if(counter == 4)
		{
			// Calculating the max value among the data received
			for(i = 0; i < 4; i++)
			{
				if(data[i] > max_value)
				{
					max_value = data[i];
					channel = i;
				}
			}

			// resetting the max value to 0 for next data
			max_value = 0;
			
			// Calculating the nearest 2 sensor value for triangulation
			// Sensors are in square grid pattern arranged in increasing order in 
			// clockwise manner. i.e. sensor 0 -> sensor 1 -> sensor 2 -> sensor 3

			if(channel+1 > 3)
				S1 = data[channel - 3];
			else
				S1 = data[channel + 1];
		
			if(channel-1 < 0)
				S2 = data[channel + 3];
			else
				S2 = data[channel - 1];
			
			// Calculating the angle to move for firebird
			angle = (((S1 - S2)/(S1 + S2)) + channel) * 90;
		
			// Move right if angle < 180
			// Move left if angle > 180 by subtracting 360 from it.
			// So firebird bird moves 180 degree right and left covering the 360 degree
			if(angle > 180)
				angle = angle - 360;

			// Calculating the delay to stop the motor
			// for 90 deg fire bird takes about 565 ms. Calculated from observing firebird motion
			delay = (angle * 565)/90;
			
			// If angle is negative delay will be calculated negative
			// So for the  'if' block firebird moves left and then right to move into
			// original position
			// for the 'else' block firebird moves right first and then left to move 
			// to original position			
			if(delay < 0)
			{
				delay *= -1;
				left();					// moving firebird to left for 'delay' ms
				_delay_ms(delay);
				stop();					// stopping the firebird
				UDR0 = 'C';				// zigbee command to capture the image to MATLAB
				_delay_ms(2000);
				right();				// moving firebird to right	
				_delay_ms(delay);	
				stop();
			}
			else
			{	
				right();				// moving firebird to right for 'delay' ms
				_delay_ms(delay);
				stop();					// stopping the firebird
				UDR0 = 'C';				// zigbee command to capture the image to MATLAB
				_delay_ms(2000);
				left();					// moving firebird to left
				_delay_ms(delay);
				stop();
			}

			counter = 0;   				// ready to accept new data from dsPIC
			UCSR1B = 0x98;				// enabling the UART recv interrupt to accept new data					
		}
									
	}
}
