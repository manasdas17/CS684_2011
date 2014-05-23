
/* 	Microcontroller: atmega16
 	Frequency: 7372800Hz
 	Optimization: -O0 (For more information read section: Selecting proper 
	              optimization options below figure 4.22 in the hardware manual)


********************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.c"
//#include <time.h>
#include <stdlib.h>
#undef RAND_MAX
#define		IR_THRESHOLD	40
#define		VELOCITY	40
#define 	ID 		2

unsigned long int ShaftCountLeft = 0; //to keep track of left position encoder 
unsigned long int ShaftCountRight = 0; //to keep track of right position encoder

unsigned char Front_IR_Sensor=0;
unsigned char Left_IR_Sensor=0;
unsigned char Right_IR_Sensor=0;	
int R=0;//R
int T=0;//theta (should not be unsigned)

//Function to configure LCD port
void lcd_port_config (void)
{
 DDRC = DDRC | 0xF7;    //all the LCD pin's direction set as output
 PORTC = PORTC & 0x80;  // all the LCD pins are set to logic 0 except PORTC 7
}

//Function to initialize Buzzer 
void buzzer_pin_config (void)
{
 DDRC = DDRC | 0x08;	//Setting PORTC 3 as outpt
 PORTC = PORTC & 0xF7;	//Setting PORTC 3 logic low to turnoff buzzer
}


void timer1_init(void)
{
 TCCR1B = 0x00; //stop
 TCNT1H = 0xFF; //setup
 TCNT1L = 0x01;
 OCR1AH = 0x00;
 OCR1AL = 0xFF;
 OCR1BH = 0x00;
 OCR1BL = 0xFF;
 ICR1H  = 0x00;
 ICR1L  = 0xFF;
 TCCR1A = 0xA1;
 TCCR1B = 0x0D; //start Timer
}


//ADC pin configuration
void adc_pin_config (void)
{
 DDRA = 0x00;   //set PORTF direction as input
 PORTA = 0x00;  //set PORTF pins floating
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
 ADCSRA = ADCSRA | 0x40;	//Set start conversion bit
 while((ADCSRA&0x10)==0);	//Wait for ADC conversion to complete
 a=ADCH;
 ADCSRA = ADCSRA|0x10;      //clear ADIF (ADC Interrupt Flag) by writing 1 to it
 return a;
}

unsigned char read_lir(void) // reads left IR proximity sensor 
{
    return ADC_Conversion(0);
}

unsigned char read_fir(void) // reads front IR proximity sensor 
{
    return ADC_Conversion(1);
}

unsigned char read_rir(void) // reads right IR proximity sensor 
{
    return ADC_Conversion(2);
}

//Function for velocity control
void velocity (unsigned char left_motor, unsigned char right_motor)
{
 OCR1AH = 0x00;
 OCR1AL = left_motor; 
 OCR1BH = 0x00;
 OCR1BL = right_motor;
}



//Function to configure ports to enable robot's motion
void motion_pin_config (void) 
{
 DDRB = DDRB | 0x0F;   //set direction of the PORTB3 to PORTB0 pins as output
 PORTB = PORTB & 0xF0; // set initial value of the PORTB3 to PORTB0 pins to logic 0
 DDRD = DDRD | 0x30;   //Setting PD4 and PD5 pins as output for PWM generation
 PORTD = PORTD | 0x30; //PD4 and PD5 pins are for velocity control using PWM
}

//Function to configure INT1 (PORTD 3) pin as input for the left position encoder
void left_encoder_pin_config (void)
{
 DDRD  = DDRD & 0xF7;  //Set the direction of the PORTD 3 pin as input
 PORTD = PORTD | 0x08; //Enable internal pull-up for PORTD 3 pin
}

//Function to configure INT0 (PORTD 2) pin as input for the right position encoder
void right_encoder_pin_config (void)
{
 DDRD  = DDRD & 0xFB;  //Set the direction of the PORTD 2 pin as input
 PORTD = PORTD | 0x04; //Enable internal pull-up for PORTD 2 pin
}

//Function to initialize ports
void port_init()
{
 motion_pin_config();          //robot motion pins config
 left_encoder_pin_config();    //left encoder pin config
 right_encoder_pin_config();   //right encoder pin config	
 lcd_port_config();		//lcd
 adc_pin_config();		//adc
 buzzer_pin_config(); 		//buzzer
}

void left_position_encoder_interrupt_init (void) //Interrupt 1 enable
{
 cli(); //Clears the global interrupt
 MCUCR = MCUCR | 0x08; // INT1 is set to trigger with falling edge
 GICR = GICR | 0x80;   // Enable Interrupt INT1 for left position encoder
 sei(); // Enables the global interrupt 
}

void right_position_encoder_interrupt_init (void) //Interrupt 0 enable
{
 cli(); //Clears the global interrupt
 MCUCR = MCUCR | 0x02; // INT0 is set to trigger with falling edge
 GICR = GICR | 0x40;   // Enable Interrupt INT5 for right position encoder
 sei(); // Enables the global interrupt 
}

//ISR for right position encoder
ISR(INT1_vect)  
{
 ShaftCountRight++;  //increment right shaft position count
}

//ISR for left position encoder
ISR(INT0_vect)
{
 ShaftCountLeft++;  //increment left shaft position count  	
}

//Function used for setting motor's direction
void motion_set (unsigned char Direction)
{
 unsigned char PortBRestore = 0;

 Direction &= 0x0F; 		// removing upper nibbel for the protection
 PortBRestore = PORTB; 		// reading the PORT original status
 PortBRestore &= 0xF0; 		// making lower direction nibbel to 0
 PortBRestore |= Direction; // adding lower nibbel for forward command and restoring the PORTB status
 PORTB = PortBRestore; 		// executing the command
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


//REPLACEMENT

void shaftcount_rotate(int ReqdShaftCountInt)
{
    ShaftCountRight = 0; 
    ShaftCountLeft = 0; 
    
    while (1)
    {		
		if((ShaftCountRight >= ReqdShaftCountInt) | (ShaftCountLeft >= ReqdShaftCountInt))
	    	break;
    }
    stop(); //Stop robot
}



// rotates robot left by given ,ReqdShaftCountInt, shaft counts
void left_rotate(int ReqdShaftCountInt)
{
    left();
    shaftcount_rotate(ReqdShaftCountInt);
}



// rotates robot right by given,ReqdShaftCountInt, shaft counts
void right_rotate(int ReqdShaftCountInt)
{
    right();
    shaftcount_rotate(ReqdShaftCountInt);
}



//Function to initialize all the devices
void init_devices()
{
 cli(); //Clears the global interrupt
 port_init();  //Initializes all the ports
 left_position_encoder_interrupt_init();
 right_position_encoder_interrupt_init();
 timer1_init();
 adc_init();
 //USARTInit(67);
 uart0_init();
 sei();   // Enables the global interrupt 
}


void USARTInit(uint16_t ubrr_value)
{
/*
Baud rate = 9600
Data bits =8
Parity = None
Stop bits = 1
Flow Control = None
Transmission Mode = Asynchronous
USART Reciever = ON
USART Transmitter = ON
U2X = 0
UBRR value = 51

ATMega16 Calibrated External RC Oscilator Frequency = 16 MHz

*/




   UBRRL = ubrr_value;
   UBRRH = (ubrr_value>>8);
   
   //UCSRC|=(1<<URSEL);
   UCSRC|=(1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);


   UCSRB|=(1<<RXEN)|(1<<TXEN);


}


unsigned char USARTReadChar()
{


   while(!(UCSRA & (1<<RXC)));



   return UDR;
}

void USARTWriteChar(char data)
{

   while(!(UCSRA & (1<<UDRE)))
   ;
 

   UDR=data;
}


void USARTWriteString(char *str)
{

while(*str!='\0')
{
  USARTWriteChar(*str);
str++;

}

}


void uart0_init(void)
{
 UCSRB = 0x00; //disable while setting baud rate
 UCSRA = 0x00;
 UCSRC = 0x86;
 UBRRL = 0x2F; //set baud rate lo  //67 is for 16MHz 9600 baudrate
 UBRRH = 0x00; //set baud rate hi
 UCSRB = 0x98; 
}


//Main Function

int main(void)
{
	int wd;
	char buffer[25];
	int flag=0,dir,send_T=0;
	init_devices();
	lcd_set_4bit();
	lcd_init();
	
 	srand(read_lir());
	
	velocity(VELOCITY,VELOCITY);
	
	while(1)
	{
	   
		//reading IR sensors
	    	Front_IR_Sensor=read_fir();
	    	Left_IR_Sensor=read_lir();
	    	Right_IR_Sensor=read_rir();
	    


		//check obstacle		
		if(Front_IR_Sensor<IR_THRESHOLD || Left_IR_Sensor<IR_THRESHOLD || Right_IR_Sensor<IR_THRESHOLD )
		// obstacle detected
		{
		    	
			
			
			if(flag==0)
			{
				// the data sending part is flag controlled to provide one time sending of data while turning multiple times in front of a obstacle to get rid of it 
				R=ShaftCountLeft;// reading distance moved
			    	//send position information(R,T) T stands for theta
			 	sprintf(buffer,"$%03d,%04d,%03d#",ID,R,send_T); // prepare packet , id is 3 digit,
			 										  
			 	USARTWriteString(buffer); //transfer packet ,contains shaft counts moved in forward since previous packet has been sent 
			
			 
			 	//$id,R,T#
				dir=rand()%2;  //selects a direction to rotate
				flag=1;		//direction selected
				send_T=0;	//resets the send_T value to be used in summing up multiple turns
				
			}

			if(dir==0 && flag==1)		//if selected direction is left
			{
				T=rand()%14+1;		//selects shaft counts to turn which in turn decides the angle to turn
				left_rotate(T);		//rotates left by T shaft counts
			}
			else if(dir==1 && flag==1)	//if selected direction is right
			{
				T=rand()%14+1;		//selects shaft counts to turn which in turn decides the angle to turn
				right_rotate(T);	//rotates right by T shaft counts
			}

			send_T=send_T+T;		//total angle,i.e, shaft counts turned in a given position trying to get rid of obstacle




		 
		   
		   
		}
		// no obstacle
		else
		{			
		//velocity(VL,VR);
		if(d==1)
			send_T=-send_T;		//if left,make angle negative...it represents total shaft counts turned when detected obstacle and 0 when in forward motion
		if(ShaftCountLeft >= 6)		//send packet on each 6 shaft counts in forward motion
		{
			sprintf(buffer,"$%03d,%04d,%03d#",ID,6,send_T); // prepare packet , id is 3 digit, 
		 										 
		 	USARTWriteString(buffer); //transfer packet
			send_T=0;		  // set send_T=0 as it is in forward motion
			ShaftCountLeft=0;
			ShaftCountRight=0;//reset distance travelled information after 
		} 
		   forward();			 //move forward
		}
    
	 
	}

}

