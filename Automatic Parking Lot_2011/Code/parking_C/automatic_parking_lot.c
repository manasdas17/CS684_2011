#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>

/*//////////////////////////////////////////////////////////////////////// */
//initialization of ports

void init_devices()
{
 cli();
 motion_pin_config();
 adc_pin_config ();
 init_adc();
 timer5_init()
 uart0_init(); //Initailize UART1 for serial communiaction
 init_buzzer();
 left_encoder_pin_config(); //left encoder pin config
 right_encoder_pin_config(); //right encoder pin config	
 left_position_encoder_interrupt_init();
 right_position_encoder_interrupt_init();
 sei();
}


void motion_pin_config (void)
{
 DDRA = DDRA | 0x0F;
 PORTA = PORTA & 0xF0;
 DDRL = DDRL | 0x18;   //Setting PL3 and PL4 pins as output for PWM generation
 PORTL = PORTL | 0x18; //PL3 and PL4 pins are for velocity control using PWM.
}

void adc_pin_config (void)
{
 DDRF = 0x00; 
 PORTF = 0x00;
 DDRK = 0x00;
 PORTK = 0x00;
}

void init_adc()
{
 ADCSRA = 0x00;
 ADCSRB = 0x00;		//MUX5 = 0
 ADMUX = 0x20;		//Vref=5V external --- ADLAR=1 --- MUX4:0 = 0000
 ACSR = 0x80;
 ADCSRA = 0x86;		//ADEN=1 --- ADIE=1 --- ADPS2:0 = 1 1 0
}

void timer5_init()
{
	TCCR5B = 0x00;	//Stop
	TCNT5H = 0xFF;	//Counter higher 8-bit value to which OCR5xH value is compared with
	TCNT5L = 0x01;	//Counter lower 8-bit value to which OCR5xH value is compared with
	OCR5AH = 0x00;	//Output compare register high value for Left Motor
	OCR5AL = 0xFF;	//Output compare register low value for Left Motor
	OCR5BH = 0x00;	//Output compare register high value for Right Motor
	OCR5BL = 0xFF;	//Output compare register low value for Right Motor
	OCR5CH = 0x00;	//Output compare register high value for Motor C1
	OCR5CL = 0xFF;	//Output compare register low value for Motor C1
	TCCR5A = 0xA9;	/*{COM5A1=1, COM5A0=0; COM5B1=1, COM5B0=0; COM5C1=1 COM5C0=0}
 					  For Overriding normal port functionalit to OCRnA outputs.
				  	  {WGM51=0, WGM50=1} Along With WGM52 in TCCR5B for Selecting FAST PWM 8-bit Mode*/
	
	TCCR5B = 0x0B;	//WGM12=1; CS12=0, CS11=1, CS10=1 (Prescaler=64)
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

void init_buzzer()
{
 DDRC = DDRC | 0x08;				//Setting PORTC pin3 as outpt
 PORTC = 0x00;				//Initialize Values of PORTC to 0.
}

//Function to configure INT4 (PORTE 4) pin as input for the left position encoder
unsigned long int ShaftCountLeft = 0; //to keep track of left position encoder 
unsigned long int ShaftCountRight = 0; //to keep track of right position encoder
unsigned int Degrees; //to accept angle in degrees for turning

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

////////////////////////////////////////////////

//Function used for setting motor's direction
void motion_set (unsigned char Direction)
{
 unsigned char PortARestore = 0;

 Direction &= 0x0F; 			// removing upper nibbel as it is not needed
 PortARestore = PORTA; 			// reading the PORTA's original status
 PortARestore &= 0xF0; 			// setting lower direction nibbel to 0
 PortARestore |= Direction; 	// adding lower nibbel for direction command and restoring the PORTA status
 PORTA = PortARestore; 			// setting the command to the port

}
void MOVE_FWD(void) //both wheels forward
{
  motion_set(0x06);
}

void MOVE_REV(void) //both wheels backward
{
  motion_set(0x09);
}

void MOVE_LEFT(void) //Left wheel backward, Right wheel forward
{
  motion_set(0x05);
}

void MOVE_RIGHT(void) //Left wheel forward, Right wheel backward
{
  motion_set(0x0A);
}

void MOVE_SOFT_LEFT (void) //Left wheel stationary, Right wheel forward
{
 motion_set(0x04);
}

void MOVE_SOFT_RIGHT (void) //Left wheel forward, Right wheel is stationary
{
 motion_set(0x02);
}

void MOVE_INPLACE_LEFT(void) //Left wheel backward, right wheel stationary
{
 motion_set(0x01);
}

void MOVE_INPLACE_RIGHT (void) //Left wheel stationary, Right wheel backward
{
 motion_set(0x08);
}

void STOP (void)
{
  motion_set(0x00);
}

/////////////////////////
//function for ADC conversion

//Function For ADC Conversion
unsigned char ADC_Conversion(unsigned char Ch) 
{
	unsigned char a;
	if(Ch>7)
	{
		ADCSRB = 0x08;
	}
	Ch = Ch & 0x07;  			
	ADMUX= 0x20| Ch;	   		
	ADCSRA = ADCSRA | 0x40;		//Set start conversion bit
	while((ADCSRA&0x10)==0);	//Wait for conversion to complete
	a=ADCH;
	ADCSRA = ADCSRA|0x10;
	ADCSRB = 0x00;
	return a;
}

///////////////////////////////////////////////////////////////////////////
// Function for robot velocity control
// Make sure that you pass only unsigned char value.



void MOTOR_LEFT_SPEED(int val)
{
    if ( val > 254 )   //limiting the max velocity
	   val = 255;	
    OCR5AL = (unsigned char) val;
}

void MOTOR_RIGHT_SPEED(int val)
{
  if ( val > 254 )     //limiting the max velocity
	  val = 255;
  OCR5BL = (unsigned char) val;
}



void velocity1(int a, int b)
{
 MOTOR_LEFT_SPEED(a);
 MOTOR_RIGHT_SPEED(b);

}
////////////////////////////////////////////////////////////////////////////////

//Function for BUZZER
void buzzer_delay()
{
 unsigned int i;
 for(i=0; i<10;i++)
 {
   _delay_us(100);
 }
}

void BUZZER_ON()
{
 int i=0; 
 unsigned char port_C_copy;
 port_C_copy = PORTC;
 port_C_copy = port_C_copy | 0x08;
 PORTC = port_C_copy;	
 buzzer_delay();
 port_C_copy = PORTC;
 port_C_copy = port_C_copy & 0xF7;
 PORTC = port_C_copy;	
}


void BUZZER_OFF()
{

PORTC = 0x00;			//Clear pin3 to turn the "buzzer OFF"
buzzer_delay();			//delay

}

//////////////////////////////////////////////////////////////////////////////////

//Function for Interrupt Service Routine


//ISR for right position encoder
ISR(INT5_vect)  
{
 ShaftCountRight++;  //increment right shaft position count
}


//ISR for left position encoder
ISR(INT4_vect)
{
 ShaftCountLeft++;  //increment left shaft position count
}

//ISR for uart
SIGNAL(SIG_USART0_RECV) 		
{
 data = UDR0; 				//making copy of data from UDR0 in 'data' variable 
 UDR0 = data; 				//echo data back to PC
 if(data == 0x30) 
 {
  unpark_cmd=1; //active signal for unparking
 }
 if(data == 0x31) 
 {             //signal for park at junction 1 in left slot
  junction=1;
  side=1;
  park_cmd=1;
 }
 if(data == 0x32) 
 {             //signal for park at junction 1 in right slot
  junction=1;
  side=2;
  park_cmd=1;
 }
 if(data == 0x33) 
 {             //signal for park at junction 2 in left slot
  junction=2;
  side=1;
  park_cmd=1;
 }
 if(data == 0x34) 
 {             //signal for park at junction 2 in right slot
  junction=2;
  side=2;
  park_cmd=1;
 }
 if(data == 0x35) 
 {             //signal for park at junction 3 in left slot
  junction=3;
  side=1;
  park_cmd=1;
 }
 if(data == 0x36) 
 {             //signal for park at junction 3 in right slot
  junction=3;
  side=2;
  park_cmd=1;
 }
}

/////////////////////////////////////////////////////////////////////////////
//Function for rotatin bot a specified angle or move specified linear distance

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
 STOP(); //Stop action
}

//Function for specified linear distance
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
 STOP(); //Stop robot
}
//////////////////////////////////////////////////////////////////////////////////////

//Function for turning robot to 90degree left/right or for fixed distance forward/reverse motion
void TURN_LEFT() 
{                              //turning left 90 degree keeping left wheel stationary
 _delay_ms(500);
 MOTOR_LEFT_SPEED(150);
 MOTOR_RIGHT_SPEED(150);
 MOVE_SOFT_LEFT(); //Turn left
 angle_rotate(180); // 2*angle
}

void TURN_RIGHT()
{                              //turning left 90 degree keeping right wheel stationary
 _delay_ms(500);
 MOTOR_LEFT_SPEED(150);
 MOTOR_RIGHT_SPEED(150);
 MOVE_SOFT_RIGHT(); //Turn right
 angle_rotate(135); //2*angle
}

void LEFT_BACK()
{                              //turning left 90 degree keeping right wheel stationary
 _delay_ms(500);
 MOTOR_LEFT_SPEED(150);
 MOTOR_RIGHT_SPEED(150);
 MOVE_SOFT_LEFT(); //Turn right
 angle_rotate(180); //2*angle
}

void RIGHT_BACK()
{                              //turning left 90 degree keeping left wheel stationary
 _delay_ms(500);
 MOTOR_LEFT_SPEED(130);
 MOTOR_RIGHT_SPEED(130);
 MOVE_SOFT_RIGHT(); //Turn right
 angle_rotate(180); //2*angle
}

void forward_mm(unsigned int DistanceInMM)
{
 MOVE_FWD();
 velocity1(150,150);
 linear_distance_mm(DistanceInMM);
}
void reverse_mm_l(unsigned int DistanceInMM)
{
 MOVE_REV();
 velocity1(150,150);
 linear_distance_mm(DistanceInMM);
}

////////////////////////////////////////////////////////////////////////////////////

/////////Function for Parking and unparking////////////////////////////////////////

//Function for white line follwing till parking junction
void white_forward(int cnt)
{
 int count=0;
 int detected=0;
 while(count<cnt)
 {
  Left_white_line = ADC_Conversion(3);	         //Getting data of Left WL Sensor
  Center_white_line = ADC_Conversion(2);	//Getting data of Center WL Sensor
  Right_white_line = ADC_Conversion(1);  	//Getting data of Right WL Sensor

  flag=0;
  if(Left_white_line<0x28 && Center_white_line<0x28 && Right_white_line<0x28)
  {
   flag=1;
   MOVE_FWD();
   velocity1(100,100);
   detected=1;
  }
  if((Left_white_line>0x28) && (flag==0))
  {
   flag=1;
   MOVE_FWD();
   velocity1(130,50);
   if(detected==1)
   {
    count++;
    detected=0;
   }
  }
  if((Right_white_line>0x28) && (flag==0))
  {
   flag=1;
   MOVE_FWD();
   velocity1(50,130);
   if(detected==1)
   {
    count++;
    detected=0;
   }
  }
  if(Center_white_line>0x28 && Left_white_line>0x28 && Right_white_line>0x28)
  {
   MOVE_FWD();
   velocity1(0,0);
  }
 }
 MOVE_FWD();
 velocity1(0,0);
}

//Function for left parking after reaching specified junction
void leftpark()
{
 TURN_LEFT();
 velocity1(130,130);
 forward_mm(150);
}

//Function for left parking after reaching specified junction
void rightpark()
{
 TURN_RIGHT();
 velocity1(130,130);
 forward_mm(150);
}


///Function for parking
void park()
{
 white_forward(junction);
 if(side==1) //to park in left side
 {
  leftpark();
 }
 if(side==2) //to park in right side
 {
  rightpark();
 }
}

///Function for unparking
void unpark()
{
 if(side==1) //when bot is parked in left side
 {	
  reverse_mm_l(280);
  LEFT_BACK();
 }
 if(side==2) //when bot is parked in right side
 {
  reverse_mm_r(280);
  RIGHT_BACK();
 }
 else  //when bot is not parked
 {
  MOVE_FWD();
  velocity1(0,0);
 }
 white_forward(junction);
}


//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////MAIN//////////////////////////////////////////////////

int main(void)
{
 init_devices();
 park_cmd=0;
 unpark_cmd=0;
 junction=0;
 side=0;
 while(1)
 {
  if(park_cmd==0 || unpark_cmd==0)
  {
   MOVE_FWD();
   velocity1(0,0);
  }
  if(park_cmd==1)
  {
   park();
   park_cmd=0;
  }
  if(unpark_cmd==1)
  {
   unpark();
   unpark_cmd=0;
  }
 }
}








