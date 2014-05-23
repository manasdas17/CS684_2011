/********************************************************************************
Project Name: Fire Fighter

Team No.: 8

Team Members:
	Jeet Patani (10305001)
	Kaustubh Keskar (10305909)
	Anjali Singhal (10305919)

 Platform: FIREBIRD V
 Last Modification: 7th Nov 2011
*********************************************************************************/

/********************************************************************************

   Copyright (c) 2011, ERTS Lab IIT Bombay erts@cse.iitb.ac.in               -*- c -*-
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

********************************************************************************/


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>

unsigned char ADC_Conversion(unsigned char);
unsigned char ADC_Value;
unsigned int BATT_Voltage;

#define RS 0
#define RW 1
#define EN 2
#define lcd_port PORTC
#define EXTINGUISH_VAL 50

#define sbit(reg,bit)	reg |= (1<<bit)			// Macro defined for Setting a bit of any register.
#define cbit(reg,bit)	reg &= ~(1<<bit)		// Macro defined for Clearing a bit of any register.

#define NODE_ID '4'					// Node Id for particular SPARK
#define TOTAL_NODES 4				// Total Nodes (SPARKS) in the topology
#define TOPOLOGY '5'				// Variable to define topology on the nodes

int totalData = 0;
unsigned char firstByte = 0xFF;
int onFire = 0;						//Total nodes on fire
unsigned char topology[TOTAL_NODES][TOTAL_NODES];//={{'2','2'},{'3','3'}};
int topoUpdate = 0;
int rtopo = 0;
int ctopo = 0;
char string[10];


//ADC pin configuration
void adc_pin_config (void)
{
	DDRA = 0x00;  //set PORTA direction as input
	PORTA = 0x00; //set PORTA pins floating
}


void motion_pin_config (void)
{
	DDRB = DDRB | 0x0F;   //set direction of the PORTB3 to PORTB0 pins as output
	PORTB = PORTB & 0xF0; // set initial value of the PORTB3 to PORTB0 pins to logic 0
	DDRD = DDRD | 0x30;   //Setting PD4 and PD5 pins as output for PWM generation
	PORTD = PORTD | 0x30; //PD4 and PD5 pins are for velocity control using PWM
}


void buzzer_pin_config (void)
{
	DDRC = DDRC | 0x08;		//Setting PORTC 3 as outpt
	PORTC = PORTC & 0xF7;		//Setting PORTC 3 logic low to turnoff buzzer
}


//Function to Initialize ADC
void adc_init()
{
	ADCSRA = 0x00;
	ADMUX = 0x20;		//Vref=5V external --- ADLAR=1 --- MUX4:0 = 0000
	ACSR = 0x80;
	ADCSRA = 0x86;		//ADEN=1 --- ADIE=1 --- ADPS2:0 = 1 1 0
}


//This Function accepts the Channel Number and returns the corresponding Analog Value 
unsigned char ADC_Conversion(unsigned char Ch)
{
	unsigned char a;
	Ch = Ch & 0x07;  			
	ADMUX= 0x20| Ch;	   		
	ADCSRA = ADCSRA | 0x40;		//Set start conversion bit
	while((ADCSRA&0x10)==0);	    //Wait for ADC conversion to complete
	a=ADCH;
	ADCSRA = ADCSRA|0x10;          //clear ADIF (ADC Interrupt Flag) by writing 1 to it
	return a;
}


// This Function prints the Analog Value Of Corresponding Channel No. at required Row
// and Coloumn Location. 
void print_sensor(char row, char coloumn,unsigned char channel)
{
	ADC_Value = ADC_Conversion(channel);
	lcd_print(row, coloumn, ADC_Value, 3);
}


//Function to initialize ports
void port_init()
{
	motion_pin_config();
	buzzer_pin_config();
	lcd_port_config();
	//lcd_port_config();
	adc_pin_config();
}


//UART0 initialisation
// desired baud rate: 9600
// actual: baud rate:9600 (0.0%)
// char size: 8 bit
// parity: Disabled
void uart0_init(void)
{
	UCSRB = 0x00; //disable while setting baud rate
	UCSRA = 0x00;
	UCSRC = 0x86;
	UBRRL = 0x2F; //set baud rate lo  //67 is for 16MHz 9600 baudrate
	UBRRH = 0x00; //set baud rate hi
	UCSRB = 0x98; 
}


//Function used for setting motor's direction
void motion_set (unsigned char Direction)
{
	unsigned char PortBRestore = 0;

	Direction &= 0x0F; 			// removing upper nibbel as it is not needed
	PortBRestore = PORTB; 			// reading the PORTB's original status
	PortBRestore &= 0xF0; 			// setting lower direction nibbel to 0
	PortBRestore |= Direction; 	// adding lower nibbel for direction command and restoring the PORTB status
	PORTB = PortBRestore; 			// setting the command to the port
}


void forward (void)         //both wheels forward
{
	motion_set(0x06);
}


void backward (void)        //both wheels backward
{
	motion_set(0x09);
}


void left (void)            //Left wheel backward, Right wheel forward
{
	motion_set(0x05);
}


void right (void)           //Left wheel forward, Right wheel backward
{   
	motion_set(0x0A);
}


void stop (void)            //hard stop
{
	motion_set(0x00);
}


void buzzer_on (void)
{
	unsigned char port_restore = 0;
	port_restore = PINC;
	port_restore = port_restore | 0x08;
	PORTC = port_restore;
}


void buzzer_off (void)
{
	unsigned char port_restore = 0;
	port_restore = PINC;
	port_restore = port_restore & 0xF7;
	PORTC = port_restore;
}


void init_devices (void)
{
	cli();         //Clears the global interrupts
	port_init();
	uart0_init();
	adc_init();
	sei();         //Enables the global interrupts
}


// ZigBee Communication
SIGNAL(SIG_UART_RECV) 
{
	unsigned char receive_data=0;  					// used to save Receiving data
	int i=0,r,c;
	receive_data = UDR;								// store the received byte
		
	if(totalData == 0){								//receive 1st byte
		totalData++;
		firstByte = receive_data;
		if(firstByte == '5'){						//set topology of nodes
			topoUpdate = 1;
			lcd_cursor(1,1);
			lcd_string("TOPOLOGY");
		}
		else{
		//	lcd_cursor(1,1);		
		//	lcd_string("               ");
		}
	}
	else if(totalData == 1){								// receive 2nd byte
		if(firstByte == TOPOLOGY || topoUpdate == 1){		//setting topology
			topoUpdate = 1;
			topology[rtopo][ctopo] = receive_data;			//add the neighbours of this node
			ctopo++;

			if(ctopo == TOTAL_NODES){
				ctopo = 0;
				rtopo++;
				if(rtopo == TOTAL_NODES){
					rtopo = 0;
					topoUpdate = 0;
					totalData = 0;
				}
			}

			lcd_cursor(1,1);
  			lcd_string("          ");
			lcd_cursor(1,1);
			string[0]=topology[0][0];
			string[1]=topology[0][1];
			string[2]=topology[1][0];
			string[3]=topology[1][1];
			string[4]=totalData + 48;
			string[5]=topoUpdate + 48;
			string[6]=onFire + 48;
			string[7]='\0';
			lcd_string(string);
		}
		else if(firstByte == '2' && onFire == 0){			//if firstbyte is 2 i.e it received a fire spread message & it is not on fire
			firstByte = 0xFF;
			totalData=0;
			if(topology[NODE_ID - 49][receive_data - 49] == '1'){	//check if the spread message is from one of its neighbours then set fire
					fire();
			}
		}
		else{								
			totalData=0;
		}
	}		
}


//put fire on this node
void fire(void)
{
	onFire = 1;

	_delay_ms((((int) NODE_ID) - 48)*25);		//back off delay to avoid collision
	UDR = '1';									//send fire start message to firebird
	_delay_ms(10);
	UDR = NODE_ID;								//send the node id on which fire has started to firebird

	lcd_cursor(1,1);
	lcd_string("               ");
	lcd_cursor(1,1);
	lcd_string("FIRE!");						//display fire on lcd
}


// Stop the fire
void end_fire(void)
{
	onFire = 0;

	_delay_ms((((int) NODE_ID) - 48)*25);		//back off delay to avoid collision
	UDR = '3';									//fire extinguished message sent to firebird
	_delay_ms(10);
	UDR = NODE_ID;								//send the node id on which fire is extinguished

	lcd_cursor(1,1);
	lcd_string("               ");
	lcd_cursor(1,1);
	lcd_string("NO FIRE!! :-)   ");				//display no fire on lcd
}


//Main Function
int main()
{
	init_devices();
	lcd_set_4bit();
	lcd_init();
	int time_count = 0;
	int fire_level=0;
	//lcd_cursor(1,5);
	//lcd_string(UDR);

	while(1) {
		if(onFire == 1 && fire_level == 0){			//indicating fire just started on the bot
			fire_level = 1;
		}
		else if(fire_level == 1){					//increase fire on the bot after some delay
			time_count++;
			if(time_count > 30){
				fire_level++;
				time_count = 0;
				lcd_cursor(1,1);
  				lcd_string("FIRE!FIRE!");
			}
		}
		else if(fire_level == 2){					//maximum fire on the bot and spread fire to neighbouring bots
			time_count++;
			if(time_count > 30){
				fire_level++;
				time_count = 0;
				lcd_cursor(1,1);
				lcd_string("FIRE!FIRE!FIRE!");
				_delay_ms((((int) NODE_ID) - 48)*25);		//back off delay 
				UDR = '2';									//send fire spread message
				_delay_ms(10);
				UDR = NODE_ID;								//send the node id to whom neighbours are to be set on fire
			}
		}

		ADC_Value = ADC_Conversion(1);							//read value of IR sensor

		if(ADC_Value <= EXTINGUISH_VAL && onFire == 1){			//if firebird is in the range and it is on fire then stop fire
			fire_level = 0;
			end_fire();
		}

		_delay_ms(100);
	}
}
