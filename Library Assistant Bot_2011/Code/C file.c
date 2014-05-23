/********************************************************************************
 Written by: Aditya Sharma, NEX Robotics Pvt. Ltd.
 Edited by: Sachitanand Malewar NEX Robotics Pvt. Ltd.
 
 AVR Studio Version 4.17, Build 666

 Date: 13th January 2010
 
 This experiment demonstrates use of position encoders.

 Concepts covered: External Interrupts, Position control
 
 Microcontroller pins used:
 PORTA3 to PORTA0: Robot direction control
 PL3, PL4: Robot velocity control. Currently set to 1 as PWM is not used
 PE4 (INT4): External interrupt for left motor position encoder
 PE5 (INT5): External interrupt for the right position encoder

 Note:
 
 1. Make sure that in the configuration options following settings are
     done for proper operation of the code

     Microcontroller: atmega2560
     Frequency: 11059200
     Optimization: -O0 (For more information read section: Selecting proper optimization options
                        below figure 4.22 in the hardware manual)
    It is observed that external interrupts does not work with the optimization level -Os

 2. Sudden change in the motor direction produces current surge up to 1.6Amps.
     Battery can give current up to 3Amps. Auxiliary supply can give current up to 1.5Amp at peak load
    When robot is on Battery power we can do sudden change motion direction as battery can supply current up to 3Amps
    Motors are stopped for 0.5 seconds between consecutive motion commands to keep current surge below 1.5Amp.
    Stop motors for at least 0.3 seconds before reversing its direction if it is running at full speed.
 
 3. In case of robot is turning with one of the wheels stationary then give twice
     the angle of required angle of turn. For more details refer to section 3.7 of the hardware manual.
*********************************************************************************/

/********************************************************************************

   Copyright (c) 2010, NEX Robotics Pvt. Ltd.                       -*- c -*-
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

********************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned long int ShaftCountLeft = 0; //to keep track of left position encoder
unsigned long int ShaftCountRight = 0; //to keep track of right position encoder
unsigned int Degrees; //to accept angle in degrees for turning

unsigned char data; //to store received data from UDR1
unsigned int a=0;
void buzzer_pin_config (void)
{
 DDRC = DDRC | 0x08;		//Setting PORTC 3 as outpt
 PORTC = PORTC & 0xF7;		//Setting PORTC 3 logic low to turnoff buzzer
}


//Function to initialize ports


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

//Function To Initialize UART2
// desired baud rate:9600
// actual baud rate:9600 (error 0.0%)
// char size: 8 bit
// parity: Disabled
void uart0_init(void)
{
 UCSR0B = 0x00; //disable while setting baud rate
 UCSR0A = 0x00;
 UCSR0C = 0x06;
 UBRR0L = 0x47; //set baud rate lo
 UBRR0H = 0x00; //set baud rate hi
 UCSR0B = 0x98;
}




//Function To Initialize all The Devices

//#####################################################################


//Function to configure ports to enable robot's motion
void motion_pin_config (void)
{
 DDRA = DDRA | 0x0F;
 PORTA = PORTA & 0xF0;
 DDRL = DDRL | 0x18;   //Setting PL3 and PL4 pins as output for PWM generation
 PORTL = PORTL | 0x18; //PL3 and PL4 pins are for velocity control using PWM.
}

//Function to configure INT4 (PORTE 4) pin as input for the left position encoder
void left_encoder_pin_config (void)
{
 DDRE  = DDRE & 0xEF;  //Set the direction of the PORTE 4 pin as input
 PORTE = PORTE | 0x10; //Enable internal pullup for PORTE 4 pin
}

//Function to configure INT5 (PORTE 5) pin as input for the right position encoder
void right_encoder_pin_config (void)
{
 DDRE  = DDRE & 0xDF;  //Set the direction of the PORTE 4 pin as input
 PORTE = PORTE | 0x20; //Enable internal pullup for PORTE 4 pin
}

//Function to initialize ports
void port_init()
{
 motion_pin_config(); //robot motion pins config
 left_encoder_pin_config(); //left encoder pin config
 right_encoder_pin_config(); //right encoder pin config    
 buzzer_pin_config();
  servo1_pin_config(); //Configure PORTB 5 pin for servo motor 1 operation
 servo2_pin_config(); //Configure PORTB 6 pin for servo motor 2 operation
 servo3_pin_config(); //Configure PORTB 7 pin for servo motor 3 operation  

}

void left_position_encoder_interrupt_init (void) //Interrupt 4 enable
{
 cli(); //Clears the global interrupt
 EICRB = EICRB | 0x02; // INT4 is set to trigger with falling edge
 EIMSK = EIMSK | 0x10; // Enable Interrupt INT4 for left position encoder
 sei();   // Enables the global interrupt
}

void right_position_encoder_interrupt_init (void) //Interrupt 5 enable
{
 cli(); //Clears the global interrupt
 EICRB = EICRB | 0x08; // INT5 is set to trigger with falling edge
 EIMSK = EIMSK | 0x20; // Enable Interrupt INT5 for right position encoder
 sei();   // Enables the global interrupt
}

//ISR for right position encoder
ISR(INT5_vect)  
{
 ShaftCountRight++;  //increment right shaft position count
}


//SR for left position encoder
ISR(INT4_vect)
{
 ShaftCountLeft++;  //increment left shaft position count
}


//Function used for setting motor's direction
void motion_set (unsigned char Direction)
{
 unsigned char PortARestore = 0;

 Direction &= 0x0F;         // removing upper nibbel for the protection
 PortARestore = PORTA;         // reading the PORTA original status
 PortARestore &= 0xF0;         // making lower direction nibbel to 0
 PortARestore |= Direction; // adding lower nibbel for forward command and restoring the PORTA status
 PORTA = PortARestore;         // executing the command
}

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


//Function used for turning robot by specified degrees
void angle_rotate(unsigned int Degrees)
{
 float ReqdShaftCount = 0;
 unsigned long int ReqdShaftCountInt = 0;

 ReqdShaftCount = (float) Degrees/ 4.090; // division by resolution to get shaft count
 ReqdShaftCountInt = (unsigned int) ReqdShaftCount;
 ShaftCountRight = 0;
 ShaftCountLeft = 0;

 while (1)
 {
  if((ShaftCountRight >= ReqdShaftCountInt) | (ShaftCountLeft >= ReqdShaftCountInt))
  break;
 }
 stop(); //Stop action
}

//Function used for moving robot forward by specified distance

void linear_distance_mm(unsigned int DistanceInMM)
{
 float ReqdShaftCount = 0;
 unsigned long int ReqdShaftCountInt = 0;

 ReqdShaftCount = DistanceInMM / 5.338; // division by resolution to get shaft count
 ReqdShaftCountInt = (unsigned long int) ReqdShaftCount;
 
 ShaftCountRight = 0;
 while(1)
 {
  if(ShaftCountRight > ReqdShaftCountInt)
  {
      break;
  }
 }
 stop(); //Stop action
}

void forward_mm(unsigned int DistanceInMM)
{
 forward();
 linear_distance_mm(DistanceInMM);
}

void back_mm(unsigned int DistanceInMM)
{
 back();
 linear_distance_mm(DistanceInMM);
}

void left_degrees(unsigned int Degrees)
{
// 88 pulses for 360 degrees rotation 4.090 degrees per count
 left(); //Turn left
 angle_rotate(Degrees);
}



void right_degrees(unsigned int Degrees)
{
// 88 pulses for 360 degrees rotation 4.090 degrees per count
 right(); //Turn right
 angle_rotate(Degrees);
}


void soft_left_degrees(unsigned int Degrees)
{
 // 176 pulses for 360 degrees rotation 2.045 degrees per count
 soft_left(); //Turn soft left
 Degrees=Degrees*2;
 angle_rotate(Degrees);
}

void soft_right_degrees(unsigned int Degrees)
{
 // 176 pulses for 360 degrees rotation 2.045 degrees per count
 soft_right();  //Turn soft right
 Degrees=Degrees*2;
 angle_rotate(Degrees);
}

void soft_left_2_degrees(unsigned int Degrees)
{
 // 176 pulses for 360 degrees rotation 2.045 degrees per count
 soft_left_2(); //Turn reverse soft left
 Degrees=Degrees*2;
 angle_rotate(Degrees);
}

void soft_right_2_degrees(unsigned int Degrees)
{
 // 176 pulses for 360 degrees rotation 2.045 degrees per count
 soft_right_2();  //Turn reverse soft right
 Degrees=Degrees*2;
 angle_rotate(Degrees);
}

//Function to initialize all the devices
void init_devices()
{
 cli(); //Clears the global interrupt
 port_init();  //Initializes all the ports
uart0_init(); //Initailize UART1 for serial communiaction
 timer1_init();
 left_position_encoder_interrupt_init();
 right_position_encoder_interrupt_init();
 sei();   // Enables the global interrupt
}
////////////////////////////////////#########################//////////////////////////////////////
void servo1_pin_config (void)
{
 DDRB  = DDRB | 0x20;  //making PORTB 5 pin output
 PORTB = PORTB | 0x20; //setting PORTB 5 pin to logic 1
}

//Configure PORTB 6 pin for servo motor 2 operation
void servo2_pin_config (void)
{
 DDRB  = DDRB | 0x40;  //making PORTB 6 pin output
 PORTB = PORTB | 0x40; //setting PORTB 6 pin to logic 1
}

//Configure PORTB 7 pin for servo motor 3 operation
void servo3_pin_config (void)
{
 DDRB  = DDRB | 0x80;  //making PORTB 7 pin output
 PORTB = PORTB | 0x80; //setting PORTB 7 pin to logic 1
}

//Initialize the ports
/*void port_init(void)
{
 servo1_pin_config(); //Configure PORTB 5 pin for servo motor 1 operation
 servo2_pin_config(); //Configure PORTB 6 pin for servo motor 2 operation
 servo3_pin_config(); //Configure PORTB 7 pin for servo motor 3 operation  
}
*/
//TIMER1 initialization in 10 bit fast PWM mode  
//prescale:256
// WGM: 7) PWM 10bit fast, TOP=0x03FF
// actual value: 42.187Hz
void timer1_init(void)
{
 TCCR1B = 0x00; //stop
 TCNT1H = 0xFC; //Counter high value to which OCR1xH value is to be compared with
 TCNT1L = 0x01;    //Counter low value to which OCR1xH value is to be compared with
 OCR1AH = 0x03;    //Output compare eegister high value for servo 1
 OCR1AL = 0xFF;    //Output Compare Register low Value For servo 1
 OCR1BH = 0x03;    //Output compare eegister high value for servo 2
 OCR1BL = 0xFF;    //Output Compare Register low Value For servo 2
 OCR1CH = 0x03;    ///Output compare eegister high value for servo 3
 OCR1CL = 0xFF;    //Output Compare Register low Value For servo 3
 ICR1H  = 0x03;    
 ICR1L  = 0xFF;
 TCCR1A = 0xAB; /*{COM1A1=1, COM1A0=0; COM1B1=1, COM1B0=0; COM1C1=1 COM1C0=0}
                     For Overriding normal port functionalit to OCRnA outputs.
                  {WGM11=1, WGM10=1} Along With WGM12 in TCCR1B for Selecting FAST PWM Mode*/
 TCCR1C = 0x00;
 TCCR1B = 0x0C; //WGM12=1; CS12=1, CS11=0, CS10=0 (Prescaler=256)
}


//Function to initialize all the peripherals
/*void init_devices(void)
{
 cli(); //disable all interrupts
 port_init();
 timer1_init();
 sei(); //re-enable interrupts
}
*/

//Function to rotate Servo 1 by a specified angle in the multiples of 2.25 degrees
void servo_1(unsigned char degrees)  
{
 float PositionPanServo = 0;
 PositionPanServo = ((float)degrees / 2.25) + 21.0;
 OCR1AH = 0x00;
 OCR1AL = (unsigned char) PositionPanServo;
}


//Function to rotate Servo 2 by a specified angle in the multiples of 2.25 degrees
void servo_2(unsigned char degrees)
{
 float PositionTiltServo = 0;
 PositionTiltServo = ((float)degrees / 2.25) + 21.0;
 OCR1BH = 0x00;
 OCR1BL = (unsigned char) PositionTiltServo;
}

//Function to rotate Servo 3 by a specified angle in the multiples of 2.25 degrees
void servo_3(unsigned char degrees)
{
 float PositionTiltServo = 0;
 PositionTiltServo = ((float)degrees / 2.25) + 21.0;
 OCR1CH = 0x00;
 OCR1CL = (unsigned char) PositionTiltServo;
}

//servo_free functions unlocks the servo motors from the any angle
//and make them free by giving 100% duty cycle at the PWM. This function can be used to
//reduce the power consumption of the motor if it is holding load against the gravity.

void servo_1_free (void) //makes servo 1 free rotating
{
 OCR1AH = 0x03;
 OCR1AL = 0xFF; //Servo 1 off
}

void servo_2_free (void) //makes servo 2 free rotating
{
 OCR1BH = 0x03;
 OCR1BL = 0xFF; //Servo 2 off
}

void servo_3_free (void) //makes servo 3 free rotating
{
 OCR1CH = 0x03;
 OCR1CL = 0xFF; //Servo 3 off
}

////////////////////////////////////#########################/////////////////////////////////////

//Main Function
void fun1()
{
    servo_1(150);
        _delay_ms(1000);
        forward_mm(100); //Moves robot forward 100mm
        stop();
        _delay_ms(1000);            
        
        servo_1(37);
        _delay_ms(1000);
         servo_1_free();
        
        back_mm(100);   //Moves robot backward 100mm
        stop();            
        _delay_ms(1000);
        
        right_degrees(76); //Rotate robot left by 90 degrees
        stop();
        _delay_ms(1000);

        forward_mm(400); //Moves robot forward 100mm
        stop();
        _delay_ms(1000);            

        left_degrees(76); //Rotate robot left by 90 degrees
        stop();
        _delay_ms(500);

    forward_mm(200); //Moves robot forward 100mm
        stop();
        _delay_ms(1000);            
        
        servo_1(150);
        _delay_ms(1000);
         servo_1_free();
        
        back_mm(200);   //Moves robot backward 100mm
        stop();            
        _delay_ms(1000);
    
        left_degrees(78); //Rotate robot left by 90 degrees
        stop();
        _delay_ms(500);
        
            forward_mm(400); //Moves robot forward 100mm
        stop();
        _delay_ms(1000);            

        right_degrees(77); //Rotate robot left by 90 degrees
        stop();
        _delay_ms(1000);

        servo_1(0);
        _delay_ms(1000);
         servo_1_free();

		 
        back_mm(35);   //Moves robot backward 100mm
        stop();            
        _delay_ms(1000);
    
/*		 */
}
void fun2()
{
    servo_1(150);
        _delay_ms(1000);
        forward_mm(100); //Moves robot forward 100mm
        stop();
        _delay_ms(1000);            
        
        servo_1(37);
        _delay_ms(1000);
         servo_1_free();
        
        back_mm(100);   //Moves robot backward 100mm
        stop();            
        _delay_ms(1000);
        
        right_degrees(77); //Rotate robot left by 90 degrees
        stop();
        _delay_ms(1000);

        forward_mm(600); //Moves robot forward 100mm
        stop();
        _delay_ms(1000);            

        left_degrees(76); //Rotate robot left by 90 degrees
        stop();
        _delay_ms(500);

    forward_mm(200); //Moves robot forward 100mm
        stop();
        _delay_ms(1000);            
        
        servo_1(150);
        _delay_ms(1000);
         servo_1_free();
        
        back_mm(200);   //Moves robot backward 100mm
        stop();            
        _delay_ms(1000);

        servo_1(0);
        _delay_ms(1000);
         servo_1_free();

    
        left_degrees(78); //Rotate robot left by 90 degrees
        stop();
        _delay_ms(500);
        
            forward_mm(600); //Moves robot forward 100mm
        stop();
        _delay_ms(1000);            

        right_degrees(79); //Rotate robot left by 90 degrees
        stop();
        _delay_ms(1000);


		 
        back_mm(40);   //Moves robot backward 100mm
        stop();            
        _delay_ms(1000);
    
/*		 */
}
void fun3()
{
    servo_1(150);
        _delay_ms(1000);
        forward_mm(100); //Moves robot forward 100mm
        stop();
        _delay_ms(1000);            
        
        servo_1(37);
        _delay_ms(1000);
         servo_1_free();
        
        back_mm(100);   //Moves robot backward 100mm
        stop();            
        _delay_ms(1000);
        
        right_degrees(77); //Rotate robot left by 90 degrees
        stop();
        _delay_ms(1000);

        forward_mm(800); //Moves robot forward 100mm
        stop();
        _delay_ms(1000);            

        left_degrees(77); //Rotate robot left by 90 degrees
        stop();
        _delay_ms(500);

    forward_mm(200); //Moves robot forward 100mm
        stop();
        _delay_ms(1000);            
        
        servo_1(150);
        _delay_ms(1000);
         servo_1_free();
        
        back_mm(200);   //Moves robot backward 100mm
        stop();            
        _delay_ms(1000);
    
	        servo_1(0);
        _delay_ms(1000);
         servo_1_free();

        left_degrees(76); //Rotate robot left by 90 degrees
        stop();
        _delay_ms(500);
        
            forward_mm(800); //Moves robot forward 100mm
        stop();
        _delay_ms(1000);            

        right_degrees(76); //Rotate robot left by 90 degrees
        stop();
        _delay_ms(1000);



		 
        back_mm(50);   //Moves robot backward 100mm
        stop();            
        _delay_ms(1000);
    
/*		 */
}
SIGNAL(SIG_USART0_RECV) 		// ISR for receive complete interrupt
{
	data = UDR0; 				//making copy of data from UDR0 in 'data' variable 

	UDR0 = data; 				//echo data back to PC
a=data;
/*		if(data == 0x31) //ASCII value of 1
		{
		servo_1(50);
			//fun1();  //forward
		}

		if(data == 0x32) //ASCII value of 2
		{
		servo_1(0);
		//	fun2(); //back
		}

		if(data == 0x33) //ASCII value of 3
		{
		forward_mm(100); //Moves robot forward 100mm
        stop();
        _delay_ms(1000);            

			//fun3();  //left
		}
		if(data == 0x34) //ASCII value of 3
		{
			stop();  //left
		}

*/
}


int main(void)
{
    init_devices();

//fun1();
//fun2();     
//fun3();

while(1){
		if(a == 0x31) //ASCII value of 1
		{
	//	servo_1(50);
	
			fun1();  //forward
				a=0;
		}

		if(a == 0x32) //ASCII value of 2
		{
	//	servo_1(0);
	//	
			fun2(); //back
			a=0;
		}

		if(a == 0x33) //ASCII value of 3
		{
		//forward_mm(70); //Moves robot forward 100mm
        //stop();
        //_delay_ms(1000);            

			fun3();  //left
			a=0;
		}
		if(a == 0x34) //ASCII value of 3
		{servo_1(50);
			stop();  //left
		a=0;
		}

}
}

