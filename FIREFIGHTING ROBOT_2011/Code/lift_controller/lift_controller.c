/********************************************************************************
  
 1. Make sure that in the configuration options following settings are 
 	done for proper operation of the code

 	Microcontroller: atmega16
 	Frequency: 7372800 
	
	Optimization: -O0 (For more information read section: Selecting proper optimization
	              options below figure 4.22 in the hardware manual)

 
*********************************************************************************/


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>



	
#define		THRESHOLD		200       
#define		VELOCITY_MAX	50       
#define		VELOCITY_MIN	35
#define 	VELOCITY_LOW	0



/* THESE DELAYS ARE CALCULATED BY TRIAL AND ERROR AND MAY CHANGE FROM ROBOT TO ROBOT */
#define 	LEFT_TURN_DELAY 1500
#define 	RIGHT_TURN_DELAY 1500
#define 	REVERSE_TURN_DELAY 3000

#define 	FORWARD_DELAY_TURN 500

unsigned char ADC_Conversion(unsigned char);



#include "uart.h"
#include "spark.h"


void left_clockwise() // lift comes down
{
	motion_set(0x05); // left backwards
}


void left_anti_clockwise() // lift goes up
{
	motion_set(0x0A);
}

//Main Function
int main(void)
{


 init_devices();
 
 unsigned char src,dest;

 
 velocity(100,0);
 
 
 
 while(1)
 {
	 do
	 {
		 src = get_data();
	 }while(src==0xff);
		
     if(src==0x32)
	 {
		 left_clockwise();
		 _delay_ms(100); // 5 sec dealy
	 }
	 else if(src == 0x38)
	 {
		 left_anti_clockwise();
		 _delay_ms(100);
	 }
	 hard_stop();
	 //send_data(0x72); //code for saying the robot has reched the commanded floor
 }

 
}
