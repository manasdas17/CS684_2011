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
#include <stdlib.h>


// Total no. of nodes [Change according to your topology]
#define TOTAL_NODES 4
#define N 2
// Distance (in mm) between 2 vertical white lines in a grid
#define X_GRID 500
// Distance (in mm) between 2 horizontal white lines in a grid
#define Y_GRID 560

unsigned char data; //to store received data from UDR0
int byteCount = 0;
unsigned char prevByte, nextByte;
int onFire = 0;
int totalFire = 0;
int onfirenodes[TOTAL_NODES];
int topology[2][2]={{1,2},{3,4}};
int curPos = 1;
enum direction {U,D,L,R};
int curDir = R;


// Function to send an SMS
// Inputs: 
//	number: mobile number to which SMS will be sent
//	text: text (body) of an SMS
void send_sms(char *number, char *text)
{
	char msg1[60] = "AT\r\n";
	char msg2[60] = "ATE0\r\n";
	char msg3[60] = "AT+CMGF=1\r\n";
	char msg4[60] = "AT+CMGS=\"";
	char msg5[60] = "\"\r\n";
	int i;

	UCSR1B = UCSR1B | 0x08;

	for(i=0;i<strlen(msg1);i++) {
		UDR1=msg1[i];
		_delay_ms(10);
	}

	for(i=0;i<strlen(msg2);i++) {
		UDR1=msg2[i];
		_delay_ms(10);
	}
	
	for(i=0;i<strlen(msg3);i++) {
		UDR1=msg3[i];
		_delay_ms(10);
	}
	
	for(i=0;i<strlen(msg4);i++) {
		UDR1=msg4[i];
		_delay_ms(10);
	}

	for(i=0;i<strlen(number);i++) {
		UDR1=number[i];
		_delay_ms(10);
	}

	for(i=0;i<strlen(msg5);i++) {
		UDR1=msg5[i];
		_delay_ms(10);
	}

	for(i=0;i<strlen(text);i++) {
		UDR1=text[i];
		_delay_ms(10);
	}

	UDR1=0x1A;
	_delay_ms(10);
	UCSR1B = UCSR1B & 0xF7;
}


void buzzer_pin_config (void)
{
	DDRC = DDRC | 0x08;		//Setting PORTC 3 as outpt
	PORTC = PORTC & 0xF7;		//Setting PORTC 3 logic low to turnoff buzzer
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


//Function To Initialize UART0
// desired baud rate:9600
// actual baud rate:9600 (error 0.0%)
// char size: 8 bit
// parity: Disabled
// ZigBee communication between PC, FireBird & Spark
void uart0_init(void)
{
	UCSR0B = 0x00; //disable while setting baud rate
	UCSR0A = 0x00;
	UCSR0C = 0x06;
	UBRR0L = 0x47; //set baud rate lo
	UBRR0H = 0x00; //set baud rate hi
	UCSR0B = 0x98;
}


// Serial Communication between FireBird & GSM module (attached to PC)
// to send an SMS
void uart1_init(void)
{
	UCSR1B = 0x00; //disable while setting baud rate
	UCSR1A = 0x00;
	UCSR1C = 0x06;
	UBRR1L = 0x47; //set baud rate lo
	UBRR1H = 0x00; //set baud rate hi
	UCSR1B = 0x98;
}


// ZigBee Communication interrupt
// ISR for receive complete interrupt
SIGNAL(SIG_USART0_RECV)
{
	data = UDR0; 				//making copy of data from UDR0 in 'data' variable 

	int i,j;

	// First byte
	// Store it in prevByte for further processing (i.e. when second byte is recvd)
	if(byteCount == 0) {
		prevByte = data;
		byteCount++;
	}
	else {
		// Second byte
		if(byteCount == 1) {
			nextByte = data;
			
			if(prevByte == '1') {							// '1' means fire has started
				if(totalFire == 0){							// if this is the first node to report fire
					onFire = 1;
					send_sms("9320699707","Fire!!!");
				}
				if(onfirenodes[nextByte-49] == 0){			// if node is not in queue
					totalFire++;
					onfirenodes[nextByte-49] = 1;
				}
			}
			else if(prevByte == '3') {						// '3' means fire is extinguished
				onfirenodes[nextByte-49] = 0;
				totalFire--;
				if(totalFire == 0){
					onFire=0;
				}
			}
			byteCount=0;
		}

		byteCount = 0;
		prevByte = 0xFF;
		
	}
}


//Function to initialize ports
void port_init()
{
	//motion_pin_config();
	port_init_PosCon();
	buzzer_pin_config();
	lcd_port_config();
	adc_pin_config();
}


// Function to initialize all the devices
void init_devices()
{
	cli(); 			//Clears the global interrupts

	port_init();	//Initializes all the ports
	uart0_init();	//Initailize UART1 for serial communiaction
	uart1_init();
	adc_init();
	timer5_init();
	left_position_encoder_interrupt_init();
	right_position_encoder_interrupt_init();

	sei();			//Enables the global interrupts
}


// Given the current location, this function returns
// nearest node (bot) which has caught fire
int next_node(int cur)
{
	int min_dist, dist, i, j, k, cur_row, cur_col, nxt_node;

	min_dist = TOTAL_NODES + TOTAL_NODES;

	// Find out own position
	for(i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			if(topology[i][j] == cur){
				cur_row = i;
				cur_col = j;
		  	}
		}
	}

	// Find the destination node to move to
	for(k = 0; k < TOTAL_NODES; k++){
		for(i = 0; i < N; i++){
			for(j = 0; j < N; j++){
				if(onfirenodes[k] == 1 && topology[i][j] == (k+1)){
					dist = abs(i-cur_row) + abs(j-cur_col);
					//printf("%d -- > %d\n",onfirenodes[k],dist);
					if(dist < min_dist){
						min_dist = dist;
						nxt_node = k+1;
					}
				}
			}
		}
	}

	return nxt_node;
}


// Given source & destination position, this functions allows
// FireBird to move from source to the destination 
void move_grid(int src, int dest)
{
	int srcr,srcc,destr,destc;
  	int i,j;

//	UDR0 = dest+48;

	// Find row, column index of source & destination nodes
	for(i = 0; i < N; i++){
    	for(j = 0; j < N; j++){
      		if(src == topology[i][j]){
				srcr = i;
				srcc = j;
			//printf("Source Index : %d,%d\n",i,j);
      		}

      		if(dest == topology[i][j]){
				destr = i;
				destc = j;
			//	printf("Destination Index : %d, %d\n",i,j);
      		}
    	}
  	}

	// Row movement
  	if(srcr < destr){
    	while(srcr < destr){
	      	srcr++;
			if(curDir == U){
				left_90();
				_delay_ms(100);
				left_90();
			}
			else if (curDir == L ){
				left_90();
			}
			else if (curDir == R ){
				right_90();
			}
			curDir = D;
			forward_mm(X_GRID);
			//lcd_cursor(1,1);
	      	//lcd_string("Move Down!");
			//_delay_ms(1000);
    	}
  	}
  	else if(srcr > destr){
    	while(srcr > destr){
      		srcr--;
			if(curDir == D){
				left_90();
				_delay_ms(100);
				left_90();
			}
			else if (curDir == R ){
				left_90();
			}
			else if (curDir == L ){
				right_90();
			}
			curDir = U;
			forward_mm(X_GRID);
      	//	lcd_cursor(1,1);
      	//	lcd_string("Move Up!");
		//	_delay_ms(1000);
    	}
  	}

	// Column movement
  	if(srcc < destc){
    	while(srcc < destc){
      		srcc++;
			if(curDir == L){
				left_90();
				_delay_ms(100);
				left_90();
			}
			else if (curDir == D ){
				left_90();
			}
			else if (curDir == U ){
				right_90();
			}
			curDir = R;
			forward_mm(Y_GRID);
		//	lcd_cursor(1,1);
      	//	lcd_string("Move Right!");
		//	_delay_ms(1000);
      		//printf("Move Right!\n");
    	}
  	}
  	else if(srcc > destc){
    	while(srcc > destc){
      		srcc--;
			if(curDir == R){
				left_90();
				_delay_ms(100);
				left_90();
			}
			else if (curDir == U ){
				left_90();
			}
			else if (curDir == D ){
				right_90();
			}
			curDir = L;
			forward_mm(Y_GRID);
		//	lcd_cursor(1,1);
      	//	lcd_string("Move Left!");
		//	_delay_ms(1000);
      		//printf("Move Left!\n");
    	} 
  	}

	if(curDir == L){
		left_90();
		_delay_ms(100);
		left_90();
	}
	else if (curDir == D ){
		left_90();
	}
	else if (curDir == U ){
		right_90();
	}

	curDir = R;
	forward_mm((Y_GRID/2));
	_delay_ms(1000);
	back_mm(Y_GRID/2);
}


//Main Function
int main(void)
{
	int i;
	int dest;
	char temp[10];

	init_devices();
	lcd_set_4bit();
	lcd_init();

	for(i = 0; i < TOTAL_NODES; i++)
		onfirenodes[i]=0;

	while(1){
	
	/*	// For debugging
		for(i = 0;i < TOTAL_NODES; i++){
			temp[i] = onfirenodes[i] + 48;
		}
		
		temp[i] = totalFire + 48;
		temp[i+1]='\0';
		lcd_cursor(1,1);
		lcd_string(temp);
	*/
		if(totalFire > 0){
			dest = next_node(curPos);
			move_grid(curPos,dest);
			curPos = dest;
		}
	}
}
