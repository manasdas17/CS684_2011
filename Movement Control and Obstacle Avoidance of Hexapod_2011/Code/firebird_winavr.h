/****** authors: Krishna Teja V, Naga Srinivas, Nikhil ***********/

#include <iom2560v.h>
#include <macros.h>
#include <math.h>


unsigned int delay_count = 0;
unsigned char arm_number = 0;
unsigned char angle_upper_byte = 0; 
unsigned char angle_lower_byte = 0;
unsigned char degree = 0;
unsigned char angle_upper_byte_1A, angle_lower_byte_1A; angle_upper_byte_1B, angle_lower_byte_1B; angle_upper_byte_1C, angle_lower_byte_1C;
unsigned char angle_upper_byte_2A, angle_lower_byte_2A; angle_upper_byte_2B, angle_lower_byte_2B; angle_upper_byte_2C, angle_lower_byte_2C;
unsigned char angle_upper_byte_3A, angle_lower_byte_3A; angle_upper_byte_3B, angle_lower_byte_3B; angle_upper_byte_3C, angle_lower_byte_3C;
unsigned char angle_upper_byte_4A, angle_lower_byte_4A; angle_upper_byte_4B, angle_lower_byte_4B; angle_upper_byte_4C, angle_lower_byte_4C;
unsigned char angle_upper_byte_5A, angle_lower_byte_5A; angle_upper_byte_5B, angle_lower_byte_5B; angle_upper_byte_5C, angle_lower_byte_5C;
unsigned char angle_upper_byte_6A, angle_lower_byte_6A; angle_upper_byte_6B, angle_lower_byte_6B; angle_upper_byte_6C, angle_lower_byte_6C;
//variables relatd to the serial commuication or remote control
unsigned int serial_communication_interval = 0;
unsigned char serial_communication_channel_number  = 0;
unsigned char remote_data [8]; //see remote channel allocation in the notes above
unsigned char motion_busy = 0; //sets to one if any of the motion function is called.

/*//////////////////////////////////////////////////////////////////////// */

// motion functions


void motion_pin_config (void) 
{
 DDRA = DDRA | 0x0F;
 PORTA = PORTA & 0xF0;
 DDRL = DDRL | 0x18;   //Setting PL3 and PL4 pins as output for PWM generation
 PORTL = PORTL | 0x18; //PL3 and PL4 pins are for velocity control using PWM.
}

//Function to initialize ports

void init_devices(void)
{
 //stop errant interrupts until set up
 CLI(); //disable all interrupts
 XMCRA = 0x00; //external memory
 XMCRB = 0x00; //external memory
 //cli(); //Clears the global interrupts
	adc_init();
	timer5_init();
	//sei(); 
 port_init();
 timer1_init();
 uart0_init();

 MCUCR  = 0x00;
 EICRA  = 0x00; //pin change int edge 0:3
 EICRB  = 0x00; //pin change int edge 4:7
 PCICR  = 0x00; //pin change int enable
 PCMSK0 = 0x00; //pin change mask
 PCMSK1 = 0x00; //pin change mask
 PCMSK2 = 0x00; //pin change mask
 EIMSK  = 0x00;
 TIMSK0 = 0x00; //timer0 interrupt sources
 TIMSK1 = 0x0F; //timer1 interrupt sources
 TIMSK2 = 0x00; //timer2 interrupt sources
 TIMSK3 = 0x00; //timer3 interrupt sources
 TIMSK4 = 0x00; //timer4 interrupt sources
 TIMSK5 = 0x00; //timer5 interrupt sources
 
 SEI(); //re-enable interrupts
 //all peripherals are now initialized
}


/*init_lcd();
lcd_init();
lcd_set_4bit();
init_buzzer();*/

//Function used for setting motor's direction
/*void motion_set (unsigned char Direction)
{
 unsigned char PortARestore = 0;

 Direction &= 0x0F; 			// removing upper nibbel as it is not needed
 PortARestore = PORTA; 			// reading the PORTA's original status
 PortARestore &= 0xF0; 			// setting lower direction nibbel to 0
 PortARestore |= Direction; 	// adding lower nibbel for direction command and restoring the PORTA status
 PORTA = PortARestore; 			// setting the command to the port

}*/


/*#define MOVE_FWD forward()
#define MOVE_REV back()
#define MOVE_LEFT left()
#define MOVE_RIGHT right()
#define MOVE_SOFT_RIGHT soft_right()
#define MOVE_SOFT_LEFT soft_left()
#define MOVE_INPLACE_RIGHT inplace_right()
#define MOVE_INPLACE_LEFT inplace_left()*/


void servo_on (void)
{ PORTB = PORTB | 0x01; }

void servo_off (void)
{ PORTB = PORTB & 0xFE; }

void reset_SP_LEFT (void)
{ PORTL = PORTL & 0xFD; }

void set_SP_LEFT (void)
{ PORTL = PORTL | 0x02; }

void reset_1A (void)
{ PORTD = PORTD & 0xDF; }

void set_1A (void)
{ PORTD = PORTD | 0x20; }

void reset_1B (void)
{ PORTL = PORTL & 0xFB; }

void set_1B (void)
{ PORTL = PORTL | 0x04; }

void reset_1C (void)
{ PORTG = PORTG & 0xFD; }

void set_1C (void)
{ PORTG = PORTG | 0x02; }

void reset_2A (void)
{ PORTL = PORTL & 0xBF; }

void set_2A (void)
{ PORTL = PORTL | 0x40; }

void reset_2B (void)
{ PORTD = PORTD & 0xEF; }

void set_2B (void)
{ PORTD = PORTD | 0x10; }

void reset_2C (void)
{ PORTL = PORTL & 0x7F; }

void set_2C (void)
{ PORTL = PORTL | 0x80; }

void reset_3A (void)
{ PORTH = PORTH & 0xBF; }

void set_3A (void)
{ PORTH = PORTH | 0x40; }

void reset_3B (void)
{ PORTH = PORTH & 0xEF; }

void set_3B (void)
{ PORTH = PORTH | 0x10; }

void reset_3C (void)
{ PORTH = PORTH & 0xDF; }

void set_3C (void)
{ PORTH = PORTH | 0x20; }


void reset_SP_RIGHT (void)
{ PORTJ = PORTJ & 0x7F; }

void set_SP_RIGHT (void)
{ PORTJ = PORTJ | 0x80; }

void reset_4A (void)
{ PORTG = PORTG & 0xFE; }

void set_4A (void)
{ PORTG = PORTG | 0x01; }

void reset_4B (void)
{ PORTD = PORTD & 0x7F; }

void set_4B (void)
{ PORTD = PORTD | 0x80; }

void reset_4C (void)
{ PORTD = PORTD & 0xBF; }

void set_4C (void)
{ PORTD = PORTD | 0x40; }

void reset_5A (void)
{ PORTJ = PORTJ & 0xDF; }

void set_5A (void)
{ PORTJ = PORTJ | 0x20; }

void reset_5B (void)
{ PORTJ = PORTJ & 0xBF; }

void set_5B (void)
{ PORTJ = PORTJ | 0x40; }

void reset_5C (void)
{ PORTJ = PORTJ & 0xEF; }

void set_5C (void)
{ PORTJ = PORTJ | 0x10; }

void reset_6A (void)
{ PORTJ = PORTJ & 0xF7; }

void set_6A (void)
{ PORTJ = PORTJ | 0x08; }

void reset_6B (void)
{ PORTJ = PORTJ & 0xFD; }

void set_6B (void)
{ PORTJ = PORTJ | 0x02; }

void reset_6C (void)
{ PORTJ = PORTJ & 0xFB; }

void set_6C (void)
{ PORTJ = PORTJ | 0x04; }

/*********************angles*********************/
void angle_value_calculation (void)
{
 unsigned int angle_value = 0;
 unsigned int temp = 0;
 if (degree > 180)
 degree = 180; // limiting the scope of the servo rotation
 
 angle_value = 0xBF39 + (64 * (unsigned char) degree); //actual constant is 64.4
 
 angle_lower_byte = (unsigned char) angle_value; //separating the lower byte
 
 temp = angle_value >> 8;
 angle_upper_byte = (unsigned char) temp; //separating the upper byte
}

// all the functions are self descriptive
void angle_1A (unsigned char angle) // eg. moves motor 1a by the angle specified
{
 degree = angle;
 angle_value_calculation();
 angle_upper_byte_1A = angle_upper_byte;
 angle_lower_byte_1A = angle_lower_byte;
}

void angle_1B (unsigned char angle)
{
 degree = angle;
 angle_value_calculation();
 angle_upper_byte_1B = angle_upper_byte;
 angle_lower_byte_1B = angle_lower_byte;
}

void angle_1C (unsigned char angle)
{
 degree = angle;
 angle_value_calculation();
 angle_upper_byte_1C = angle_upper_byte;
 angle_lower_byte_1C = angle_lower_byte;
}

void angle_2A (unsigned char angle)
{
 degree = angle;
 angle_value_calculation();
 angle_upper_byte_2A = angle_upper_byte;
 angle_lower_byte_2A = angle_lower_byte;
}

void angle_2B (unsigned char angle)
{
 degree = angle;
 angle_value_calculation();
 angle_upper_byte_2B = angle_upper_byte;
 angle_lower_byte_2B = angle_lower_byte;
}

void angle_2C (unsigned char angle)
{
 degree = angle;
 angle_value_calculation();
 angle_upper_byte_2C = angle_upper_byte;
 angle_lower_byte_2C = angle_lower_byte;
}

void angle_3A (unsigned char angle)
{
 degree = angle;
 angle_value_calculation();
 angle_upper_byte_3A = angle_upper_byte;
 angle_lower_byte_3A = angle_lower_byte;
}

void angle_3B (unsigned char angle)
{
 degree = angle;
 angle_value_calculation();
 angle_upper_byte_3B = angle_upper_byte;
 angle_lower_byte_3B = angle_lower_byte;
}

void angle_3C (unsigned char angle)
{
 degree = angle;
 angle_value_calculation();
 angle_upper_byte_3C = angle_upper_byte;
 angle_lower_byte_3C = angle_lower_byte;
}

void angle_4A (unsigned char angle)
{
 degree = angle;
 angle_value_calculation();
 angle_upper_byte_4A = angle_upper_byte;
 angle_lower_byte_4A = angle_lower_byte;
}

void angle_4B (unsigned char angle)
{
 degree = angle;
 angle_value_calculation();
 angle_upper_byte_4B = angle_upper_byte;
 angle_lower_byte_4B = angle_lower_byte;
}

void angle_4C (unsigned char angle)
{
 degree = angle;
 angle_value_calculation();
 angle_upper_byte_4C = angle_upper_byte;
 angle_lower_byte_4C = angle_lower_byte;
}

void angle_5A (unsigned char angle)
{
 degree = angle;
 angle_value_calculation();
 angle_upper_byte_5A = angle_upper_byte;
 angle_lower_byte_5A = angle_lower_byte;
}

void angle_5B (unsigned char angle)
{
 degree = angle;
 angle_value_calculation();
 angle_upper_byte_5B = angle_upper_byte;
 angle_lower_byte_5B = angle_lower_byte;
}

void angle_5C (unsigned char angle)
{
 degree = angle;
 angle_value_calculation();
 angle_upper_byte_5C = angle_upper_byte;
 angle_lower_byte_5C = angle_lower_byte;
}

void angle_6A (unsigned char angle)
{
 degree = angle;
 angle_value_calculation();
 angle_upper_byte_6A = angle_upper_byte;
 angle_lower_byte_6A = angle_lower_byte;
}

void angle_6B (unsigned char angle)
{
 degree = angle;
 angle_value_calculation();
 angle_upper_byte_6B = angle_upper_byte;
 angle_lower_byte_6B = angle_lower_byte;
}

void angle_6C (unsigned char angle)
{
 degree = angle;
 angle_value_calculation();
 angle_upper_byte_6C = angle_upper_byte;
 angle_lower_byte_6C = angle_lower_byte;
}

/***********************servo motor calibaration********************/
void servo_calibration (void)
{
angle_1A(90); angle_1B(90); angle_1C(90); angle_2A(90); angle_2B(90); angle_2C(90);
angle_3A(90); angle_3B(90); angle_3C(90); angle_4A(90); angle_4B(90); angle_4C(90);
angle_5A(90); angle_5B(90); angle_5C(90); angle_6A(90); angle_6B(90); angle_6C(90);
}

void even_A_servo_90 (void)
{ angle_2A(90); angle_4A(90); angle_6A(90); }

void even_A_servo_65 (void) //clockwise
{ angle_2A(65); angle_4A(65); angle_6A(65); }

void even_A_servo_115 (void)//anticlockwise_
{ angle_2A(115); angle_4A(115); angle_6A(115); }

void even_BC_servo_90 (void)
{ angle_2B(90); angle_2C(90); angle_4B(90); angle_4C(90); angle_6B(90); angle_6C(90); }

void even_BC_servo_up_45 (void)
{ angle_2B(45); angle_2C(90); angle_4B(135); angle_4C(90); angle_6B(135); angle_6C(90); }

void even_BC_servo_down_45 (void)
{ angle_2B(135); angle_2C(135); angle_4B(45); angle_4C(45); angle_6B(45); angle_6C(45); }

void odd_A_servo_90 (void)
{ angle_1A(90); angle_3A(90); angle_5A(90); }

void odd_A_servo_65 (void)//clockwise
{ angle_1A(65); angle_3A(65); angle_5A(65); }

void odd_A_servo_115 (void) //anticlockwise
{ angle_1A(115); angle_3A(115); angle_5A(115); }

void odd_BC_servo_90 (void)
{ angle_1B(90); angle_1C(90); angle_3B(90); angle_3C(90); angle_5B(90); angle_5C(90); }

void odd_BC_servo_up_45 (void)
{ angle_1B(45); angle_1C(90); angle_3B(45); angle_3C(90); angle_5B(135); angle_5C(90); }

void odd_BC_servo_down_45 (void)
{ angle_1B(135); angle_1C(135); angle_3B(135); angle_3C(135); angle_5B(45); angle_5C(45); }
/********************************motions*************************/
void robot_rotate_clockwise_7_5 (void) //code to rotate hexapod clockwise
{
 motion_busy = 1; 
 servo_calibration(); delay_250ms(); 
 even_A_servo_65(); even_BC_servo_up_45(); delay_250ms(); 
 even_BC_servo_90(); delay_250ms(); 
 odd_A_servo_65(); odd_BC_servo_up_45(); delay_250ms(); 
 odd_BC_servo_90(); delay_250ms(); 
 even_A_servo_90(); odd_A_servo_90();
 motion_busy = 0; 
}

void robot_rotate_anticlockwise_7_5 (void)//code to rotate hexapod anti clockwise
{
 motion_busy = 1; 
 servo_calibration(); delay_250ms(); 
 even_A_servo_115(); even_BC_servo_up_45(); delay_250ms(); 
 even_BC_servo_90(); delay_250ms(); 
 odd_A_servo_115(); odd_BC_servo_up_45(); delay_250ms(); 
 odd_BC_servo_90(); delay_250ms(); 
 even_A_servo_90(); odd_A_servo_90();
 motion_busy = 0; 
}

void forward (void)// code to move hexapod forward with 2 legs at a time
 {
 motion_busy = 1; 
 servo_calibration();
 delay_250ms();
 
 //legs 2 and 4 move forward
 angle_2B(45); angle_2C(45); angle_4B(135); angle_4C(135); //front pair BC servo up 45
 delay_250ms();
 angle_2A(45); angle_4A(135); //front pair move forward while legs are up
 delay_250ms();
 angle_2B(90); angle_2C(90); angle_4B(90); angle_4C(90); // put down legs
 delay_250ms();
 
 //legs 1 and 6 move forward
 
 angle_1B(45); angle_1C(45); angle_6B(135); angle_6C(135); //middel pair BC servo up 45
 delay_250ms();
 angle_1A(45); angle_6A(135); //middel pair move back while legs are up
 delay_250ms();
 angle_1B(90); angle_1C(90); angle_6B(90); angle_6C(90); // put down legs
 delay_250ms();
 
 //legs 3 and 5 move forward
 angle_3B(45); angle_3C(45); angle_5B(135); angle_5C(135); //middel pair BC servo up 45
 delay_250ms();
 angle_3A(45); angle_5A(135); //middel pair move forward while legs are up
 delay_250ms();
 angle_3B(90); angle_3C(90); angle_5B(90); angle_5C(90); // put down legs
 delay_250ms(); 
 
 servo_calibration(); //push towards forward
 motion_busy = 0; 
 }

 
 void back (void)
 {
 motion_busy = 1; 
 servo_calibration();
 delay_250ms(); 
 
 //legs 2 and 4 move back
 angle_2B(45); angle_2C(45); angle_4B(135); angle_4C(135); //front pair BC servo up 45
 delay_250ms(); 
 angle_2A(135); angle_4A(45); //front pair move forward while legs are up
 delay_250ms(); 
 angle_2B(90); angle_2C(90); angle_4B(90); angle_4C(90); // put down legs
 delay_250ms(); 
 
 //legs 1 and 6 move back
 
 angle_1B(45); angle_1C(45); angle_6B(135); angle_6C(135); //middel pair BC servo up 45
 delay_250ms(); 
 angle_1A(135); angle_6A(45); //middel pair move back while legs are up
 delay_250ms(); 
 angle_1B(90); angle_1C(90); angle_6B(90); angle_6C(90); // put down legs
 delay_250ms(); 
 
 //legs 3 and 5 move back
 angle_3B(45); angle_3C(45); angle_5B(135); angle_5C(135); //middel pair BC servo up 45
 delay_250ms(); 
 angle_3A(135); angle_5A(45); //middel pair move forward while legs are up
 delay_250ms(); 
 angle_3B(90); angle_3C(90); angle_5B(90); angle_5C(90); // put down legs
 delay_250ms();  
 
 servo_calibration(); //push towards back
 motion_busy = 0; 
 }

void robot_standup (void) //warning: do not call this function while robot is standing or servos will get damaged
{
motion_busy = 1; 
angle_1B(1); angle_1C(110); angle_2B(1); angle_2C(110); angle_3B(1); angle_3C(110);
angle_4B(179); angle_4C(70); angle_5B(179); angle_5C(70); angle_6B(179); angle_6C(70);
delay_1s();
delay_1s();
servo_calibration();
motion_busy = 0; 
}
/*****************************/

// Timer 5 initialised in PWM mode for velocity control
// Prescale:64
// PWM 8bit fast, TOP=0x00FF
// Timer Frequency:674.988Hz
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

// Function for robot velocity control
// Make sure that you pass only unsigned char value.
//TIMER1 initialize - prescale:1
// WGM: 0) Normal, TOP=0xFFFF
// desired value: 400Hz
// actual value: 400.007Hz (0.0%)
void timer1_init(void)
{
 TCCR1B = 0x00; //stop
 TCNT1H = 0x94; //setup
 TCNT1L = 0x01;
 OCR1AH = 0x6B;
 OCR1AL = 0xFF;
 OCR1BH = 0x6B;
 OCR1BL = 0xFF;
 OCR1CH = 0x00;
 OCR1CL = 0x00;
 ICR1H  = 0x6B;
 ICR1L  = 0xFF;
 TCCR1A = 0x00;
 TCCR1C = 0x00;
 TCCR1B = 0x01; //start Timer
}

#pragma interrupt_handler timer1_compa_isr:iv_TIM1_COMPA
void timer1_compa_isr(void)
{
 //compare occured TCNT1=OCR1A
 if (arm_number == 0) { reset_1A(); }
 if (arm_number == 1) { reset_2A(); }
 if (arm_number == 2) { reset_3A(); }
 if (arm_number == 3) { reset_4A(); }
 if (arm_number == 4) { reset_5A(); }
 if (arm_number == 5) { reset_6A(); }
}

#pragma interrupt_handler timer1_compb_isr:iv_TIM1_COMPB
void timer1_compb_isr(void)
{
 //compare occured TCNT1=OCR1B
 if (arm_number == 0) { reset_1B(); }
 if (arm_number == 1) { reset_2B(); }
 if (arm_number == 2) { reset_3B(); }
 if (arm_number == 3) { reset_4B(); }
 if (arm_number == 4) { reset_5B(); }
 if (arm_number == 5) { reset_6B(); }
}

#pragma interrupt_handler timer1_compc_isr:iv_TIM1_COMPC
void timer1_compc_isr(void)
{
 //compare occured TCNT1=OCR1c
 if (arm_number == 0) { reset_1C(); }
 if (arm_number == 1) { reset_2C(); }
 if (arm_number == 2) { reset_3C(); }
 if (arm_number == 3) { reset_4C(); }
 if (arm_number == 4) { reset_5C(); }
 if (arm_number == 5) { reset_6C(); }
}

#pragma interrupt_handler timer1_ovf_isr:iv_TIM1_OVF
void timer1_ovf_isr(void)
{
 //TIMER1 has overflowed
 TCNT1H = 0x94; //reload counter high value
 TCNT1L = 0x01; //reload counter low value
 delay_count++;
 arm_number ++;
 if (arm_number>7)
 {
  	arm_number = 0;
 }
   
 if (arm_number == 0)
 {
  set_1A(); 
  set_1B(); 
  set_1C();
  OCR1AH = angle_upper_byte_1A;
  OCR1AL = angle_lower_byte_1A;
  OCR1BH = angle_upper_byte_1B;
  OCR1BL = angle_lower_byte_1B;
  OCR1CH = angle_upper_byte_1C;
  OCR1CL = angle_lower_byte_1C;
 }
 
 if (arm_number == 1)
 {
  set_2A(); 
  set_2B(); 
  set_2C();
  OCR1AH = angle_upper_byte_2A;
  OCR1AL = angle_lower_byte_2A;
  OCR1BH = angle_upper_byte_2B;
  OCR1BL = angle_lower_byte_2B;
  OCR1CH = angle_upper_byte_2C;
  OCR1CL = angle_lower_byte_2C;
 }
 
 if (arm_number == 2)
 {
  set_3A(); 
  set_3B(); 
  set_3C();
  OCR1AH = angle_upper_byte_3A;
  OCR1AL = angle_lower_byte_3A;
  OCR1BH = angle_upper_byte_3B;
  OCR1BL = angle_lower_byte_3B;
  OCR1CH = angle_upper_byte_3C;
  OCR1CL = angle_lower_byte_3C;
 }
 
 if (arm_number == 3)
 {
  set_4A(); 
  set_4B(); 
  set_4C();
  OCR1AH = angle_upper_byte_4A;
  OCR1AL = angle_lower_byte_4A;
  OCR1BH = angle_upper_byte_4B;
  OCR1BL = angle_lower_byte_4B;
  OCR1CH = angle_upper_byte_4C;
  OCR1CL = angle_lower_byte_4C;
 }
 
 if (arm_number == 4)
 {
  set_5A(); 
  set_5B(); 
  set_5C();
  OCR1AH = angle_upper_byte_5A;
  OCR1AL = angle_lower_byte_5A;
  OCR1BH = angle_upper_byte_5B;
  OCR1BL = angle_lower_byte_5B;
  OCR1CH = angle_upper_byte_5C;
  OCR1CL = angle_lower_byte_5C;
 }
 
 if (arm_number == 5)
 {
  set_6A(); 
  set_6B(); 
  set_6C();
  OCR1AH = angle_upper_byte_6A;
  OCR1AL = angle_lower_byte_6A;
  OCR1BH = angle_upper_byte_6B;
  OCR1BL = angle_lower_byte_6B;
  OCR1CH = angle_upper_byte_6C;
  OCR1CL = angle_lower_byte_6C;
 }
 //serial communication related code
 serial_communication_interval ++;
 if (serial_communication_interval > 40) //gives time interval of 40*2.5ms = 100ms
 {
  serial_communication_interval = 0;
  serial_communication_channel_number++;
  if(serial_communication_channel_number > 4) //set maximum number of command (can be set up to 7)
  {
  serial_communication_channel_number  = 3; //set first command to begin with (can be set up to 1)
  }
  UDR0 = serial_communication_channel_number;
 }
}



/******************UART**************/
//UART0 initialize
// desired baud rate: 9600
// actual: baud rate:9600 (0.0%)
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

#pragma interrupt_handler uart0_rx_isr:iv_USART0_RXC
void uart0_rx_isr(void)
{
 //uart has received a character in UDR
 remote_data[serial_communication_channel_number] = UDR0;  
}






#define FRONT_IR front_dist_mm()
#define FRONT_LEFT_IR front_left_dist_mm()
#define FRONT_RIGHT_IR front_right_dist_mm()
#define REAR_IR rear_dist_mm()
#define REAR_LEFT_IR rear_left_dist_mm()
#define REAR_RIGHT_IR rear_right_dist_mm()
#define LEFT_IR left_dist_mm()
#define RIGHT_IR right_dist_mm()
#define FRONT_SHARP front_s_dist_mm()
#define FRONT_LEFT_SHARP front_left_s_dist_mm()
#define FRONT_RIGHT_SHARP front_right_s_dist_mm()
#define LEFT_SHARP left_s_dist_mm()
#define RIGHT_SHARP right_s_dist_mm()



#define FCPU 11059200ul //defined here to make sure that program works properly

void port_init();
void timer5_init();

unsigned char ADC_Conversion(unsigned char);
unsigned char ADC_Value;
unsigned char flag = 0;


//ADC pin configuration
void adc_pin_config (void)
{
 DDRF = 0x00; 
 PORTF = 0x00;
 DDRK = 0x00;
 PORTK = 0x00;
}

void lcd_port_config (void)
{
 DDRC = DDRC | 0xF7; //all the LCD pin's direction set as output
 PORTC = PORTC & 0x80; // all the LCD pins are set to logic 0 except PORTC 7
}

//Function to Initialize PORTS
void port_init(void)
{
 PORTA = 0x00;
 DDRA  = 0xFF;
 PORTB = 0x00;
 DDRB  = 0x01;
 PORTC = 0x00; //m103 output only
 DDRC  = 0xFF;
 PORTD = 0x00;
 DDRD  = 0xF0;
 PORTE = 0x00;
 DDRE  = 0x00;
 PORTF = 0x00;
 DDRF  = 0x00;
 PORTG = 0x00;
 DDRG  = 0x03;
 PORTH = 0x00;
 DDRH  = 0x70;
 PORTJ = 0x00;
 DDRJ  = 0xFE; //0xFE original
 PORTK = 0x00;
 DDRK  = 0x00;
 PORTL = 0x00;
 DDRL  = 0xC7;
 lcd_port_config();
 //init_lcd();
	adc_pin_config();
	buzzer_pin_config();
}





void adc_init()
{
	ADCSRA = 0x00;
	ADCSRB = 0x00;		//MUX5 = 0
	ADMUX = 0x20;		//Vref=5V external --- ADLAR=1 --- MUX4:0 = 0000
	ACSR = 0x80;
	ADCSRA = 0x86;		//ADEN=1 --- ADIE=1 --- ADPS2:0 = 1 1 0
}

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
	ADCSRA = ADCSRA|0x10; //clear ADIF (ADC Interrupt Flag) by writing 1 to it
	ADCSRB = 0x00;
	return a;
}




/***********************************************************************************
 This Function calculates the actual distance in millimeters(mm) from the input
 analog value of Sharp Sensor. 
 ***********************************************************************************/

/***** Function to Initialize PORTS *****/
void init_ir()
{
	DDRF = 0x00; 
	PORTF = 0x00;
	DDRA = 0x0F;
	PORTA = 0x00;
	DDRL = 0x18;
	PORTL = 0x18;
	DDRC = 0xFF;
	PORTC = 0x00;
	DDRJ = 0x00;
	PORTJ = 0xFF;
}


unsigned int Sharp_estimation(unsigned char adc_reading)
{
	float distance;
	unsigned int distanceInt;
	distance = (int)(10.00*(2799.6*(1.00/(pow(adc_reading,1.1546)))));
	distanceInt = (int)distance;
	if(distanceInt>800)
	{
		distanceInt=800;
	}
	return distanceInt;
}


int front_dist_mm(void)  // returns front IR sensor value similarly others are all self descriptive
{
  //float front_distance;
  int front_distance_mm;
  unsigned char analog_val;
  analog_val = ADC_Conversion(6);
 

 front_distance_mm=Sharp_estimation(analog_val);
  return front_distance_mm; 	

  //return 100;
}
int front_left_dist_mm(void) 
{
  //float front_distance;
  int front_distance_mm;
  unsigned char analog_val;
  analog_val = ADC_Conversion(5);
 

 front_distance_mm=Sharp_estimation(analog_val);
  return front_distance_mm; 	

  //return 100;
}
int left_dist_mm(void) 
{
  //float front_distance;
  int front_distance_mm;
  unsigned char analog_val;
  analog_val = ADC_Conversion(4);
 

 front_distance_mm=Sharp_estimation(analog_val);
  return front_distance_mm; 	

  //return 100;
}
int right_dist_mm(void) 
{
  //float front_distance;
  int front_distance_mm;
  unsigned char analog_val;
  analog_val = ADC_Conversion(8);
 

 front_distance_mm=Sharp_estimation(analog_val);
  return front_distance_mm; 	

  //return 100;
}
int front_right_dist_mm(void) 
{
  //float front_distance;
  int front_distance_mm;
  unsigned char analog_val;
  analog_val = ADC_Conversion(7);
 

 front_distance_mm=Sharp_estimation(analog_val);
  return front_distance_mm; 	

  //return 100;
}
int rear_dist_mm(void) 
{
  //float front_distance;
  int front_distance_mm;
  unsigned char analog_val;
  analog_val = ADC_Conversion(7);
 

 front_distance_mm=Sharp_estimation(analog_val);
  return front_distance_mm; 	

  //return 100;
}
int rear_right_dist_mm(void) 
{
  //float front_distance;
  int front_distance_mm;
  unsigned char analog_val;
  analog_val = ADC_Conversion(7);
 

 front_distance_mm=Sharp_estimation(analog_val);
  return front_distance_mm; 	

  //return 100;
}
int rear_left_dist_mm(void) 
{
  //float front_distance;
  int front_distance_mm;
  unsigned char analog_val;
  analog_val = ADC_Conversion(7);
 

 front_distance_mm=Sharp_estimation(analog_val);
  return front_distance_mm; 	

  //return 100;
}
int front_s_dist_mm(void) // returns front sharp sensor value similarly others are all self descriptive
{
  //float front_distance;
  int front_distance_mm;
  unsigned char analog_val;
  analog_val = ADC_Conversion(11);
 

 front_distance_mm=Sharp_estimation(analog_val);
  return front_distance_mm; 	

  //return 100;
}
int left_s_dist_mm(void) 
{
  //float front_distance;
  int front_distance_mm;
  unsigned char analog_val;
  analog_val = ADC_Conversion(9);
 

 front_distance_mm=Sharp_estimation(analog_val);
  return front_distance_mm; 	

  //return 100;
}
int front_left_s_dist_mm(void) 
{
  //float front_distance;
  int front_distance_mm;
  unsigned char analog_val;
  analog_val = ADC_Conversion(10);
 

 front_distance_mm=Sharp_estimation(analog_val);
  return front_distance_mm; 	

  //return 100;
}
int front_right_s_dist_mm(void) 
{
  //float front_distance;
  int front_distance_mm;
  unsigned char analog_val;
  analog_val = ADC_Conversion(12);
 

 front_distance_mm=Sharp_estimation(analog_val);
  return front_distance_mm; 	

  //return 100;
}
int right_s_dist_mm(void) 
{
  //float front_distance;
  int front_distance_mm;
  unsigned char analog_val;
  analog_val = ADC_Conversion(13);
 

 front_distance_mm=Sharp_estimation(analog_val);
  return front_distance_mm; 	

  //return 100;
}






/*     ////////////////////////////////////////////////////////////////////////////////////////////////////////////   */

// lcd functions


#define FCPU 11059200ul
#define RS 0
#define RW 1
#define EN 2
#define lcd_port PORTC

#define sbit(reg,bit)	reg |= (1<<bit)
#define cbit(reg,bit)	reg &= ~(1<<bit)

void init_ports();
void lcd_reset_4bit();
void lcd_init();
void lcd_wr_command(unsigned char);
void lcd_wr_char(char);
void lcd_home();
void lcd_cursor(char, char);
void lcd_print(char, char, unsigned int, int);
void lcd_string(char*);
void print_sensor(char, char,unsigned char);
unsigned int temp;
unsigned int unit;
unsigned int tens;
unsigned int hundred;
unsigned int thousand;
unsigned int million;

int i;
void print_sensor(char row, char coloumn,unsigned char channel)
{
	
	ADC_Value = ADC_Conversion(channel);
	lcd_print(row, coloumn, ADC_Value, 3);
}


void init_lcd()
{
	DDRF = 0x00; 
	PORTF = 0x00;
	DDRA = 0x0F;
	PORTA = 0x00;
	DDRL = 0x18;
	PORTL = 0x18;
	DDRC = 0xFF;
	PORTC = 0x00;
	DDRJ = 0x00;
	PORTJ = 0xFF;
}




/*****Function to Reset LCD*****/
void lcd_set_4bit()
{
	delay_100ms();

	cbit(lcd_port,RS);				//RS=0 --- Command Input
	cbit(lcd_port,RW);				//RW=0 --- Writing to LCD
	lcd_port = 0x30;				//Sending 3
	sbit(lcd_port,EN);				//Set Enable Pin
	delay_100ms();					//Delay
	cbit(lcd_port,EN);				//Clear Enable Pin

	delay_100ms();

	cbit(lcd_port,RS);				//RS=0 --- Command Input
	cbit(lcd_port,RW);				//RW=0 --- Writing to LCD
	lcd_port = 0x30;				//Sending 3
	sbit(lcd_port,EN);				//Set Enable Pin
	delay_100ms();					//Delay
	cbit(lcd_port,EN);				//Clear Enable Pin

	delay_100ms();

	cbit(lcd_port,RS);				//RS=0 --- Command Input
	cbit(lcd_port,RW);				//RW=0 --- Writing to LCD
	lcd_port = 0x30;				//Sending 3
	sbit(lcd_port,EN);				//Set Enable Pin
	delay_100ms();					//Delay
	cbit(lcd_port,EN);				//Clear Enable Pin

	delay_100ms();

	cbit(lcd_port,RS);				//RS=0 --- Command Input
	cbit(lcd_port,RW);				//RW=0 --- Writing to LCD
	lcd_port = 0x20;				//Sending 2 to initialise LCD 4-bit mode
	sbit(lcd_port,EN);				//Set Enable Pin
	delay_100ms();					//Delay
	cbit(lcd_port,EN);				//Clear Enable Pin

	
}

/*****Function to Initialize LCD*****/
void lcd_init()
{
	delay_100ms();

	lcd_wr_command(0x28);			//LCD 4-bit mode and 2 lines.
	lcd_wr_command(0x01);
	lcd_wr_command(0x06);
	lcd_wr_command(0x0E);
	lcd_wr_command(0x80);
		
}

	 
/*****Function to Write Command on LCD*****/
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
	delay_100ms();
	cbit(lcd_port,EN);
	
	cmd = cmd & 0x0F;
	cmd = cmd<<4;
	lcd_port &= 0x0F;
	lcd_port |= cmd;
	cbit(lcd_port,RS);
	cbit(lcd_port,RW);
	sbit(lcd_port,EN);
	delay_100ms();
	cbit(lcd_port,EN);
}

/*****Function to Write Data on LCD*****/
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
	delay_100ms();
	cbit(lcd_port,EN);

	letter = letter & 0x0F;
	letter = letter<<4;
	lcd_port &= 0x0F;
	lcd_port |= letter;
	sbit(lcd_port,RS);
	cbit(lcd_port,RW);
	sbit(lcd_port,EN);
	delay_100ms();
	cbit(lcd_port,EN);
}


void lcd_home()
{
	lcd_wr_command(0x80);
}


/*****Function to Print String on LCD*****/
void lcd_string(char *str)
{
	while(*str != '\0')
	{
		lcd_wr_char(*str);
		str++;
	}
}

/*** Position the LCD cursor at "row", "column". ***/

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

/***** Function To Print Any input value upto the desired digit on LCD *****/
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


void LCD_DISPLAY_INT_1(int val)
{


 lcd_print(1,1,val,3);

}

/* reverse: reverse string s in place */
void reverse(char s[])
{
    int c, i, j;
    for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}
void itoa1(int n, char s[])
{
    int i, sign;
    if ((sign = n) < 0) /* record sign */
         n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0'; /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
         s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}


/*void LCD_DISPLAY_INT_1(int val)
{
unsigned char porta_restore,i;
char display_data_line1[16];
itoa1(val,display_data_line1);
//lcd_init();
//clear_screen();
lcd_cursor(1,1);
lcd_set_4bit();
//go_to_next_line();
for (i = 0; i<=16; i++)
 	 {

	 lcd_wr_char(display_data_line1[i]);
	
 	 }

}*/
	

void LCD_DISPLAY_INT_2(int val)
{	


lcd_print(2,1,val,4);

}


/*****************delay*************************/

void delay_100ms(void)
{
 delay_count = 0;
 while(delay_count < 40);
}

void delay_250ms(void)
{
 delay_count = 0;
 while(delay_count < 100);
}

void delay_500ms(void)
{
 delay_count = 0;
 while(delay_count < 200);
}

void delay_1s(void)
{
 delay_count = 0;
 while(delay_count < 400);
}

/*  ////////////////////////////////////////////    */

// BUZZER Functions
void buzzer_pin_config (void)
{
 DDRC = DDRC | 0x08;		//Setting PORTC 3 as outpt
 PORTC = PORTC & 0xF7;		//Setting PORTC 3 logic low to turnoff buzzer
}

/*****Function to Initialize PORTS*****/
void init_buzzer()
{
	DDRC = DDRC | 0x08;				//Setting PORTC pin3 as outpt
	PORTC = 0x00;				//Initialize Values of PORTC to 0.
}

/***** Buzzer Delay Function*****/
void buzzer_delay()
{
	unsigned int i;
	for(i=0; i<10;i++)
	{
		delay_100ms();
	}
}

void BUZZER_ON()
{

int i=0; 
   unsigned char port_C_copy;
   port_C_copy = PORTC;
   port_C_copy = port_C_copy | 0x08;
   PORTC = port_C_copy;	
   //for(i=0;i<10;i++)
	//tick_timer();
	buzzer_delay();
   port_C_copy = PORTC;
   port_C_copy = port_C_copy & 0xF7;
   PORTC = port_C_copy;	



//PORTC = 0x08;			//Set pin3 to turn the "buzzer ON"
//buzzer_delay();			//delay


}


void BUZZER_OFF()
{

PORTC = 0x00;			//Clear pin3 to turn the "buzzer OFF"
buzzer_delay();			//delay

}










