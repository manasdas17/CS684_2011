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


	
#define		THRESHOLD		200       // set the pots such that all three sensor 
                                      // calibrated to show its min value on LCD. 
                                      // i.e on LCD Sensor values are betwn 0 To 25
									  // on white line  
#define		VELOCITY_MAX	50       
#define		VELOCITY_MIN	35
#define 	VELOCITY_LOW	0



/* THESE DELAYS ARE CALCULATED BY TRIAL AND ERROR AND MAY CHANGE FROM ROBOT TO ROBOT */
#define 	LEFT_TURN_DELAY 1500
#define 	RIGHT_TURN_DELAY 1500
#define 	REVERSE_TURN_DELAY 3000

#define 	FORWARD_DELAY_TURN 500

#define 	EXTRA_DELAY  500

unsigned char ADC_Conversion(unsigned char);



#include "uart.h"
#include "spark.h"

void turn_left()
{
    forward();
	_delay_ms(FORWARD_DELAY_TURN);
	left();
	_delay_ms(LEFT_TURN_DELAY);
}


void turn_right()
{
	forward();
	_delay_ms(FORWARD_DELAY_TURN);	
	right();
	_delay_ms(RIGHT_TURN_DELAY);
}


void turn_reverse()
{
	right();
	_delay_ms(REVERSE_TURN_DELAY);
}


void turn_reverse_sharp()
{
	right();
	_delay_ms(REVERSE_TURN_DELAY+EXTRA_DELAY);
}


void follow_white_line()
{
	unsigned char Left_white_line = 0;
	unsigned char Center_white_line = 0;
	unsigned char Right_white_line = 0;
	
	Left_white_line = ADC_Conversion(3);	//Getting data of Left WL Sensor
	Center_white_line = ADC_Conversion(4);	//Getting data of Center WL Sensor
	Right_white_line = ADC_Conversion(5);	//Getting data of Right WL Sensor
 
	int flag=0;

	if(Center_white_line > THRESHOLD)               // Is middle Whiteline is within threshold limit
	{                                             
		flag=1;
		forward();     
	}

	if((Left_white_line > THRESHOLD) && (flag==0))  // Is left Whiteline is not within threshold limit
	{                                             
		flag=1;
		left();		
	}

	if((Right_white_line > THRESHOLD) && (flag==0)) // Is right Whiteline is not within threshold limit
	{
		flag=1;
		right();		
	}

	if(Center_white_line < THRESHOLD && Left_white_line < THRESHOLD && Right_white_line < THRESHOLD && (flag == 0))
	                                // if all Whiteline sensor are not within threshold limit    
	{
		flag=1;
		hard_stop();
	} 
}

int my_flag=0;

int is_turn()
{
	unsigned char Left_white_line = 0;
	unsigned char Center_white_line = 0;
	unsigned char Right_white_line = 0;
	
	Left_white_line = ADC_Conversion(3);	//Getting data of Left WL Sensor
	Center_white_line = ADC_Conversion(4);	//Getting data of Center WL Sensor
	Right_white_line = ADC_Conversion(5);	//Getting data of Right WL Sensor
	
	 
	
	if(Left_white_line > THRESHOLD && Center_white_line > THRESHOLD 
	    && Right_white_line > THRESHOLD && my_flag==0)
	{
		my_flag=1;
		return 1;
	}
	
	if(!(Left_white_line > THRESHOLD && Center_white_line > THRESHOLD 
	    && Right_white_line > THRESHOLD))
		my_flag=0;
	
	return 0;
}


/*
	from: 0-lift
		  1-4 : room no
    similarly for 'to'		  
*/	
void move_to_room(int /*from*/ source, int /*to*/ destination)
{
	int turns=0;
	
	velocity(30,30);
	
	switch(source) // currently implemented only for "from=lift"
	{
		case 0: 
				switch(destination)
				{
					case 1: 
							while(turns < 3) // untill 1st turn
							{
							    forward();
								follow_white_line();
								if(is_turn()) turns++;
							}
							
							turn_left();
							
							while(!is_turn())
							{
								forward();
								follow_white_line();
							}
							hard_stop();
							break;

					case 2:

							while(turns < 3) // untill 1st turn
							{
							    forward();
								follow_white_line();
								if(is_turn()) turns++;
							}
							
							turn_right();
							
							while(!is_turn())              
							{
								forward();
								follow_white_line();
							}
							hard_stop();
							break;


					case 3:

							while(turns < 2) // untill 1st turn
							{
							    forward();
								follow_white_line();
								if(is_turn()) turns++;
							}
							
							turn_left();
							
							while(!is_turn())              
							{
								forward();
								follow_white_line();
							}
							hard_stop();
							break;


					case 4:

							while(turns < 2) // untill 1st turn
							{
							    forward();
								follow_white_line();
								if(is_turn()) turns++;
							}
							
							turn_right();
							
							while(!is_turn())              
							{
								forward();
								follow_white_line();
							}
							hard_stop();
							break;

					default:   	break;
			       
				}
				break;

		case 1:
				switch(destination)
				{
					case 0: 
						 	turn_reverse();
							while(!is_turn())
							{
								forward();
								follow_white_line();
							}
							turn_right();
							while(turns <2)
							{
								forward();
								follow_white_line();
								if(is_turn()) turns++;
							}
							hard_stop();
							break;
							
					case 2:
						 	turn_reverse();
							while(turns < 2)
							{
								forward();
								follow_white_line();
								if(is_turn()) turns++;
							}

							hard_stop();
							break;

					case 3:		

						 	turn_reverse();
							while(turns < 1)
							{
								forward();
								follow_white_line();
								if(is_turn()) turns++;
							}

							turn_right();

                                                        while(!is_turn())
							{
								forward();
								follow_white_line();								
							}

							turn_right();

							while(!is_turn())
							{
								forward();
								follow_white_line();
							}

							hard_stop();
							break;
			
					case 4:		

						 	turn_reverse();
							while(turns < 1)
							{
								forward();
								follow_white_line();
								if(is_turn()) turns++;
							}

							turn_right();

                                                        while(!is_turn())
							{
								forward();
								follow_white_line();								
							}

							turn_left();

							while(!is_turn())
							{
								forward();
								follow_white_line();
							}

							hard_stop();
							break;
			
					default: 	break;				
			
				}
				break;

		case 2:
				switch(destination)
				{
					case 0:
							
						 	turn_reverse();
							while(!is_turn())
							{
								forward();
								follow_white_line();
							}
							turn_right();
							while(turns <2)
							{
								forward();
								follow_white_line();
								if(is_turn()) turns++;
							}
							hard_stop();
							break;
							
					case 1:		
							turn_reverse();
							while(turns<2)
							{
								forward();
								follow_white_line();
							}	

							hard_stop();
							break;

					case 3:

							turn_reverse();
							while(!is_turn())
							{
								forward();
								follow_white_line();
							}	

							turn_left();

							while(!is_turn())
							{
								forward();
								follow_white_line();
							}

							turn_right();


							while(!is_turn())
							{
								forward();
								follow_white_line();
							}

							hard_stop();

							break;

					case 4:

							turn_reverse();
							while(!is_turn())
							{
								forward();
								follow_white_line();
							}	

							turn_left();

							while(!is_turn())
							{
								forward();
								follow_white_line();
							}

							turn_left();


							while(!is_turn())
							{
								forward();
								follow_white_line();
							}

							hard_stop();

							break;

				}
				break;

		case 4:
				switch(destination)
				{
					case 0:
							
						 	turn_reverse();							
							while(!is_turn())
							{
								forward();
								follow_white_line();								
							}
							turn_left();
							while(!is_turn())
							{
								forward();
								follow_white_line();								
							}
							hard_stop();
							break;
					case 1:
							turn_reverse();

							while(!is_turn())
							{
								forward();
								follow_white_line();
							}

							turn_right();

							
							while(!is_turn())
							{
								forward();
								follow_white_line();
							}

							turn_left();


							while(!is_turn())
							{
								forward();
								follow_white_line();
							}

							hard_stop();

							break;

					case 2:

							turn_reverse();

							while(!is_turn())
							{
								forward();
								follow_white_line();
							}

							turn_right();

							
							while(!is_turn())
							{
								forward();
								follow_white_line();
							}

							turn_right();


							while(!is_turn())
							{
								forward();
								follow_white_line();
							}

							hard_stop();
							break;

					case 3:
							turn_reverse();

							while(turns < 2)
							{
								forward();
								follow_white_line();
								if(is_turn()) turns++;
							}

							hard_stop();
							break;

					default:	break;



				}
				break;
				
		case 3:
				switch(destination)
				{
					case 0:
							
						 	turn_reverse();							
							while(!is_turn())
							{
								forward();
								follow_white_line();								
							}
							turn_right();
							while(!is_turn())
							{
								forward();
								follow_white_line();								
							}
							hard_stop();
							break;

					case 1:
							turn_reverse();

							while(!is_turn())
							{
								forward();
								follow_white_line();
							}

							turn_left();

							
							while(!is_turn())
							{
								forward();
								follow_white_line();
							}

							turn_left();


							while(!is_turn())
							{
								forward();
								follow_white_line();
							}

							hard_stop();

							break;

					case 2:

							turn_reverse();

							while(!is_turn())
							{
								forward();
								follow_white_line();
							}

							turn_left();

							
							while(!is_turn())
							{
								forward();
								follow_white_line();
							}

							turn_right();


							while(!is_turn())
							{
								forward();
								follow_white_line();
							}

							hard_stop();
							break;

					case 4:
							turn_reverse();

							while(turns < 2)
							{
								forward();
								follow_white_line();
								if(is_turn()) turns++;
							}

							hard_stop();
							break;

					default:	break;



				}
				break;


				
	}
}



void parking_to_lift()
{
	int turns=0;
	while(turns < 2) // robot is standing on a turn
	{
	    forward();
	    follow_white_line();
	    if(is_turn()) turns++;
	}

	hard_stop();	
}



void parking_to_ground_floor()
{
	int turns=0;
	while(turns < 3) // robot is standing on a turn
	{
	    forward();
	    follow_white_line();
	    if(is_turn()) turns++;
	}							

	hard_stop();		
}



void move_out_of_lift()
{
	int turns=0;
	while(turns < 2) // robot is standing on a turn
	{
	    forward();
	    follow_white_line();
	    if(is_turn()) turns++;
	}					

	hard_stop();		
}




void lift_to_parking()
{
	int turns=0;
	while(turns < 2) // robot is standing on a turn
	{
	    forward();
	    follow_white_line();
	    if(is_turn()) turns++;
	}
							
	turn_reverse_sharp();	
	hard_stop();
}





void ground_floor_to_parking()
{
	int turns =0;

	while(turns < 4) // robot is standing on a turn
	{
	    forward();
	    follow_white_line();
	    if(is_turn()) turns++;
	}
	
	turn_reverse_sharp();
	hard_stop();	
}


void ground_floor_to_lift()
{
	int turns =0;

	while(turns < 4) // robot is standing on a turn
	{
	    forward();
	    follow_white_line();
	    if(is_turn()) turns++;
	}

	turn_reverse_sharp();

	parking_to_lift();
	
	hard_stop();			
}


void move_into_lift()
{
	int turns =0;

	while(turns < 3) // robot is standing on a turn
	{
	    forward();
	    follow_white_line();
	    if(is_turn()) turns++;
	}
	
	hard_stop();
}


//Main Function
int main(void)
{


 init_devices();
 
 unsigned char src,dest;

	
 velocity(30,30);

 int flag = 0;
 while(1)
 {
    flag=0;
	my_flag=0;
	
	 do
	 {
		 src = get_data();
	 }while(src==0xff);

	/* 80-parking->lift
	 * 81-lift->parking
	 * 82-parking->ground_floor
	 * 83-move_out_of_lift (move out of list to 0th position for 1st floor)
	 * 84-ground_floor to parking
	 * 85-ground_floor to lift
	 * */
	 switch(src) // special codes for navigation 
	 {
		 case 0x80:	parking_to_lift();
					flag=1;
					break;
				//continue; // continue the loop

		 case 0x81:	lift_to_parking();
					flag=1;
					break;
				//continue;

		 case 0x82: 	parking_to_ground_floor();
						flag=1;
						break;
				//continue;

		 case 0x83:	move_out_of_lift();
					flag=1;
					break;
				//continue;

		 case 0x84:	ground_floor_to_parking();
					flag=1;
					break;
				//continue;

		 case 0x85:	ground_floor_to_lift();
					flag=1;
					break;
				//continue;
				
		 case 0x86:	move_into_lift();
					flag=1;
					break;		

		 default:	break;	
	 }

	if(flag==1)
	{	
	   send_data(0x21);	   
	   continue;
	}
	
	 do
	 {
		 dest =  get_data();
	 }while(dest==0xff);

	 move_to_room((int)(src-0x30),(int)(dest-0x30));

	 send_data(0x20); //code for saying the robot has reched the destination

 }

 
}
