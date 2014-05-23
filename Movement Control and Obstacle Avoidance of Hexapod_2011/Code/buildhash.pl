#!/usr/bin/perl
#authors: Krishna Teja, Nikhil, srinivas
#use Switch;
$ip=" ";  
$ip=(@ARGV)[0];
$filename=$ip;
$modulename=(@ARGV)[2];
#print "$modulename\n";
unless(-e $filename)
{
   print "file doesnot exitst";
   return;
}
open FH,$filename or die $!;
@inputfile_contents=<FH>;
$length_inputfile=@inputfile_contents;
$i=0;
$key=0;
$value=0;
while( $i < $length_inputfile )       # code for getting the input and output signals used in the esterel program
{
  $e15 = (@inputfile_contents)[$i];
  if ( $e15 =~ /<BuiltinTypeSymbol id="(.*)">/ )
  {
	$key = $1;
	$i++;
	$e16=(@inputfile_contents)[$i];
	if ( $e16 =~ /<S>(.*)<\/S>/ )
	{
		$value=$1;
		$symboltable{$key}=$value;
        }
  }
  $i++;
}
#my %symboltable=(
#	4   => boolean,
#	5   => integer,
#	6   => float,
#	7   => double,
#	8   => string,
#	10  => false,
#	11  => 0,
#	12  => ture,
#	13  => 1,
#);
#print %symboltable;
#exit 0;

$i=0;
$key=0;
$value=0;
$type="null";
while( $i < $length_inputfile )       # code for getting the input and output signals used in the esterel program
{
  $e12 = (@inputfile_contents)[$i];
  if ( $e12 =~ /<SignalSymbol id="(.*)">/ )
  {
    #print "$1 ----";
    $key = $1;
    $i++;
    $e13=(@inputfile_contents)[$i];
    if ( $e13 =~ /<S>(.*)<\/S>/ )
    {
      #print "$1 \n";
      $value=$1;
      $i=$i+1;
      $e1311=(@inputfile_contents)[$i];		
      if($e1311 =~ /<Ref id="(.*)"\/>/ )
      {
	$type=$symboltable{$1};
	#print "\n$type\n";
      }
      $i=$i+2;
      $e131=(@inputfile_contents)[$i];
      if($e131 =~ /<Int>(.*)<\/Int>/ )
      {
	if( $1 =~ /0/ )
	{
	      $signals{$key}=$value;
	      @input=(@input,$value);
	      if($type !~ /null/)
            	     { @input_signature=(@input_signature,$value."(".$type.")"); }
	      else
		     { @input_signature=(@input_signature,$value);}	
 	}
	elsif ( $1 =~ /1/ )
	{
	      $signals{$key}=$value;
	      @output=(@output,$value);
	      if($type !~ /null/)
            	    {  @output_signature=(@output_signature,$value."(".$type.")");}
	      else
		    {  @output_signature=(@output_signature,$value);}		 	
	}
	elsif ( $1 =~ /2/ )
	{
	      $signals{$key}=$value;
	      @inputoutput=(@inputoutput,$value);
	      if($type !~ /null/)
            	    {  @inputoutput_signature=(@inputoutput_signature,$value."(".$type.")");}
	      else
		    {  @inputoutput_signature=(@inputoutput_signature,$value);}		 	
	
        }
	elsif ( $1 =~/3/ )
	{
	      $signals{$key}=$value;
	      @sensor=(@sensor,$value);
	      if($type !~ /null/)
            	    {  @sensor_signature=(@sensor_signature,$value."(".$type.")");}
	      else
		    {  @sensor_signature=(@sensor_signature,$value);}	
	} 
      }		
    }
  }
  $i++;
  $type="null";
}

#removing duplicates from output signals  ---- duplicates occur when your program has multiple modules
my %hash   = map { $_ => 1 } @output_signature;
@output_signature = keys %hash;

#print %signals;
#print "\n";
#print "input signals:\n";
#print "@input\n";
#print "input signals with signatures:\n";
#print "@input_signature\n";
#print "output signals:\n";
#print "@output\n";
#print "output signals with signatures:\n";
#print "@output_signature\n";
#print "inputoutput signals:\n";
#print "@inputoutput\n";
#print "inputoutput signals with signatures:\n";
#print "@inputoutput_signature\n";
#print "sensors:\n";
#print "@sensor\n";
#print "sensors with types:\n";
#print "@sensor_signature\n";
#exit 0; # to be removed


$j=0;
while( $j < $length_inputfile )   #Code for getting the signals on which awaits are called. (ex. await, abort..when, do..watching, etc.)
{
  $e14 = (@inputfile_contents)[$j];
  if ( $e14 =~ /<LoadSignalExpression id="(.*)">/ )
  {
    $j++;
    $j++; 	
    $e15=(@inputfile_contents)[$j];
    if ( $e15 =~ /<Ref id="(.*)"\/>/ )
    {
      #print "$1 \n";
      @awaits_id=(@awaits_id,$1);
    }
  }
  $j++;
}
#print "awaits_ids\n";
#print "@awaits_id\n";
@awaits_without_dups = grep { !$count{$_}++ } @awaits_id; # removing duplicates for awaits
#print "@awaits_without_dups\n";
for $e16 (@awaits_without_dups)
{
  @await=(@await,$signals{$e16});
}
@await = grep { !$count{$_}++ } @await; # removing duplicates for awaits
#print "awaits:\n";
#print "@await\n";

#these are the set of valid input signals for firebird
@input_keywords = ("BUMPSWITCH_ANY","BUMPSWITCH_1","BUMPSWITCH_2","BUMPSWITCH_3","BUMPSWITCH_4","BUMPSWITCH_5",
		   "FRONT_IR_VALUE(integer)","FRONT_LEFT_IR_VALUE(integer)","FRONT_RIGHT_IR_VALUE(integer)","REAR_RIGHT_IR_VALUE(integer)","REAR_LEFT_IR_VALUE(integer)","LEFT_IR_VALUE(integer)","RIGHT_IR_VALUE(integer)","REAR_IR_VALUE(integer)","BATTERY_VOLTAGE(integer)","RECEIVE(integer)",
                   "FRONT_SHARP_VALUE(integer)","FRONT_LEFT_SHARP_VALUE(integer)","FRONT_RIGHT_SHARP_VALUE(integer)","LEFT_SHARP_VALUE(integer)","RIGHT_SHARP_VALUE(integer)","FIR_LESS_THRE","RIR_LESS_THRE","LIR_LESS_THRE");

#there are the set of valid output signals for firebird
@output_keywords = ("BUZZER_ON","BUZZER_OFF","MOTOR_1A(integer)","MOTOR_2A(integer)","MOTOR_3A(integer)","MOTOR_4A(integer)","MOTOR_5A(integer)","MOTOR_6A(integer)","MOTOR_1B(integer)","MOTOR_2B(integer)","MOTOR_3B(integer)","MOTOR_4B(integer)","MOTOR_5B(integer)","MOTOR_6B(integer)","MOTOR_1C(integer)","MOTOR_2C(integer)","MOTOR_3C(integer)","MOTOR_4C(integer)","MOTOR_5C(integer)","MOTOR_6C(integer)",
		    "MOVE_FWD","MOVE_REV","ROTATE_RIGHT(integer)","ROTATE_LEFT(integer)","LCD_INIT","LCD_SET_4_BIT","PRINT_SENSOR(integer,integer,integer)","LCD_CLEAR","LCD_DISPLAY_1(string)","LCD_DISPLAY_2(string)","DELAY(integer)","SERVO_CALIBARATION",
                    "LCD_DISPLAY_INT_1(integer)","LCD_DISPLAY_INT_2(integer)","SEND(integer)","SET_FIR_THRESHOLD(integer)","SET_LIR_THRESHOLD(integer)","SET_RIR_THRESHOLD(integer)");

#these are the set of valid sesors for firebird
@sensor_keywords = ("FRONT_IR_VALUE(integer)","LEFT_IR_VALUE(integer)","RIGHT_IR_VALUE(integer)","BATTERY_VOLTAGE(integer)");

#cheking whether the used input signals are valid or not
$flag_input=0;
$global_flag=0;
for $l1 (@input_signature)
{
 $flag_input=0;
 for $m1 (@input_keywords)
 {
   if ( $l1 eq $m1)
   {
     $flag_input=1;
   }
 }
 if ( $flag_input == 0 )
 {
   print "$l1 : Not a valid keyword\n";
   $global_flag=1;
 }
}

#cheking whether the used output signals are valid or not
$flag_output=0;
for $l2 (@output_signature)
{
 $flag_output=0;
 for $m2 (@output_keywords)
 {
   if ( $l2 eq $m2)
   {
     $flag_output=1;
   }
 }
 if ( $flag_output == 0 )
 {
   print "$l2 : Not a valid keyword\n";
   $global_flag=1;
 }
}

#cheking whether the used sensors are valid or not
$flag_sensor=0;
for $l2 (@sensor_signature)
{
 $flag_sensor=0;
 for $m2 (@sensor_keywords)
 {
   if ( $l2 eq $m2)
   {
     $flag_sensor=1;
   }
 }
 if ( $flag_sensor == 0 )
 {
   print "$l2 : Not a valid keyword\n";
   $global_flag=1;
 }
}

if ( $global_flag == 1 )
{
  exit 0;
}

#code for the main() function to be added to the esterel generated C code

$ofile=(@ARGV)[1];
open OF,">$ofile" or die $!;
print OF "/****************************** Fire Bird Specific part ***************************/\n";
#print OF "#include \"functions.h\"\n";
print OF "static int  IR_THRESHHOLD[3] = {50, 50, 50};\n";
$length_output=@output_signature;
if( $length_output !=0 )
{
 for $elem (@output_signature)
 {
	if ( $elem eq "BUZZER_ON" )
	{
	  print OF "$modulename\_O_BUZZER_ON(void)\n";
 	  print OF "{\n";
	  print OF "	BUZZER_ON();\n";
	  print OF "}\n";	
	}
	if ( $elem eq "SERVO_CALIBARATION" )
	{
	  print OF "$modulename\_O_SERVO_CALIBARATION(void)\n";
 	  print OF "{\n";
	  print OF "	servo_calibration();\n";
	  print OF "}\n";	
	}
	if ( $elem eq "BUZZER_OFF" )
	{
	  print OF "$modulename\_O_BUZZER_OFF(void)\n";
 	  print OF "{\n";
	  print OF "	BUZZER_OFF();\n";
	  print OF "}\n";	
	}
	if ( $elem eq "DELAY(integer)" )
	{
	  print OF "$modulename\_O_DELAY(int val)\n";
 	  print OF "{\n";
	  print OF " if(val==100)\n";
	  print OF "	delay_100ms();\n";
	  print OF " else if(val==250)\n";
	  print OF "	delay_250ms();\n";
	  print OF " else if(val==500)\n";
	  print OF "	delay_500ms();\n";
	  print OF " else if(val==1000)\n";
	  print OF "	delay_1s();\n";
	  print OF " else\n";
	  print OF "	delay_100ms();\n";
	  print OF "}\n";	
	}
	if ( $elem eq "MOTOR_1A(integer)" )
	{
	  print OF "$modulename\_O_MOTOR_1A(int val)\n";
 	  print OF "{\n";
	  print OF "	angle_1A (val);\n";
	  print OF "}\n";	
	}
	if ( $elem eq "MOTOR_2A(integer)" )
	{
	  print OF "$modulename\_O_MOTOR_2A(int val)\n";
 	  print OF "{\n";
	  print OF "	angle_2A (val);\n";
	  print OF "}\n";	
	}
	if ( $elem eq "MOTOR_3A(integer)" )
	{
	  print OF "$modulename\_O_MOTOR_3A(int val)\n";
 	  print OF "{\n";
	  print OF "	angle_3A (val);\n";
	  print OF "}\n";	
	}
	if ( $elem eq "MOTOR_4A(integer)" )
	{
	  print OF "$modulename\_O_MOTOR_4A(int val)\n";
 	  print OF "{\n";
	  print OF "	angle_4A (val);\n";
	  print OF "}\n";	
	}
	if ( $elem eq "MOTOR_5A(integer)" )
	{
	  print OF "$modulename\_O_MOTOR_5A(int val)\n";
 	  print OF "{\n";
	  print OF "	angle_5A (val);\n";
	  print OF "}\n";	
	}
	if ( $elem eq "MOTOR_6A(integer)" )
	{
	  print OF "$modulename\_O_MOTOR_6A(int val)\n";
 	  print OF "{\n";
	  print OF "	angle_6A (val);\n";
	  print OF "}\n";	
	}
	if ( $elem eq "MOTOR_1B(integer)" )
	{
	  print OF "$modulename\_O_MOTOR_1B(int val)\n";
 	  print OF "{\n";
	  print OF "	angle_1B (val);\n";
	  print OF "}\n";	
	}
	if ( $elem eq "MOTOR_2B(integer)" )
	{
	  print OF "$modulename\_O_MOTOR_2B(int val)\n";
 	  print OF "{\n";
	  print OF "	angle_2B (val);\n";
	  print OF "}\n";	
	}
	if ( $elem eq "MOTOR_3B(integer)" )
	{
	  print OF "$modulename\_O_MOTOR_3B(int val)\n";
 	  print OF "{\n";
	  print OF "	angle_3B (val);\n";
	  print OF "}\n";	
	}
	if ( $elem eq "MOTOR_4B(integer)" )
	{
	  print OF "$modulename\_O_MOTOR_4B(int val)\n";
 	  print OF "{\n";
	  print OF "	angle_4B (val);\n";
	  print OF "}\n";	
	}
	if ( $elem eq "MOTOR_5B(integer)" )
	{
	  print OF "$modulename\_O_MOTOR_5B(int val)\n";
 	  print OF "{\n";
	  print OF "	angle_5B (val);\n";
	  print OF "}\n";	
	}
	if ( $elem eq "MOTOR_6B(integer)" )
	{
	  print OF "$modulename\_O_MOTOR_6B(int val)\n";
 	  print OF "{\n";
	  print OF "	angle_6B (val);\n";
	  print OF "}\n";	
	}
	if ( $elem eq "MOTOR_1C(integer)" )
	{
	  print OF "$modulename\_O_MOTOR_1C(int val)\n";
 	  print OF "{\n";
	  print OF "	angle_1C (val);\n";
	  print OF "}\n";	
	}
	if ( $elem eq "MOTOR_2C(integer)" )
	{
	  print OF "$modulename\_O_MOTOR_2C(int val)\n";
 	  print OF "{\n";
	  print OF "	angle_2C (val);\n";
	  print OF "}\n";	
	}
	if ( $elem eq "MOTOR_3C(integer)" )
	{
	  print OF "$modulename\_O_MOTOR_3C(int val)\n";
 	  print OF "{\n";
	  print OF "	angle_3C (val);\n";
	  print OF "}\n";	
	}
	if ( $elem eq "MOTOR_4C(integer)" )
	{
	  print OF "$modulename\_O_MOTOR_4C(int val)\n";
 	  print OF "{\n";
	  print OF "	angle_4C (val);\n";
	  print OF "}\n";	
	}
	if ( $elem eq "MOTOR_5C(integer)" )
	{
	  print OF "$modulename\_O_MOTOR_5C(int val)\n";
 	  print OF "{\n";
	  print OF "	angle_5C (val);\n";
	  print OF "}\n";	
	}
	if ( $elem eq "MOTOR_6C(integer)" )
	{
	  print OF "$modulename\_O_MOTOR_6C(int val)\n";
 	  print OF "{\n";
	  print OF "	angle_6C (val);\n";
	  print OF "}\n";	
	}
####
	if ( $elem eq "MOVE_FWD" )
	{
	  print OF "$modulename\_O_MOVE_FWD(void)\n";
 	  print OF "{\n";
	  print OF "	forward();\n";
	  print OF "}\n";	
	}
	if ( $elem eq "MOVE_REV" )
	{
	  print OF " $modulename\_O_MOVE_REV(void)\n";
 	  print OF "{\n";
	  print OF "	back();\n";
	  print OF "}\n";	
	}
	if ( $elem eq "ROTATE_LEFT(integer)" )
	{
	  print OF " $modulename\_O_ROTATE_LEFT(int val)\n";
 	  print OF "{\n";
 	  print OF " int i;\n";
 	  print OF " for(i=0;i<val;i++)\n";
	  print OF "	robot_rotate_anticlockwise_7_5();\n";
	  print OF "}\n";	
	}
	if ( $elem eq "ROTATE_RIGHT(integer)" )
	{
	  print OF "$modulename\_O_ROTATE_RIGHT(int val)\n";
 	  print OF "{\n";
 	  print OF " int i;\n";
 	  print OF " for(i=0;i<val;i++)\n";
	  print OF "	robot_rotate_clockwise_7_5();\n";
	  print OF "}\n";	
	}
	#if ( $elem eq "MOVE_STOP" )
	#{
	 # print OF "$modulename\_O_MOVE_STOP(void)\n";
 	 # print OF "{\n";
	 # print OF "	MOVE_STOP();\n";
	 # print OF "}\n";	
	#}
	if ( $elem eq "LCD_INIT" )
	{
	  print OF "$modulename\_O_LCD_INIT(void)\n";
 	  print OF "{\n";
	  print OF "	lcd_init();\n";
	  print OF "}\n";	
	}
	if ( $elem eq "LCD_SET_4_BIT" )
	{
	  print OF "$modulename\_O_LCD_SET_4_BIT(void)\n";
 	  print OF "{\n";
	  print OF "	lcd_set_4bit();\n";
	  print OF "}\n";	
	}
	if ( $elem eq "PRINT_SENSOR(integer,integer,integer)" )
	{
	  print OF "$modulename\_O_PRINT_SENSOR(int val1,int val2,int val3)\n";
 	  print OF "{\n";
	  print OF "	print_sensor(val1,val2,val3);\n";
	  print OF "}\n";	
	}
	
	if ( $elem eq "LCD_CLEAR" )
	{
	  print OF "$modulename\_O_LCD_CLEAR(void)\n";
 	  print OF "{\n";
	  print OF "	LCD_CLEAR();\n";
	  print OF "}\n";	
	}
	if ( $elem eq "LCD_DISPLAY_1(string)" )
	{
	  print OF "$modulename\_O_LCD_DISPLAY_1(char val[])\n";
 	  print OF "{\n";
	  print OF "	lcd_string(val);\n";
	  print OF "}\n";	
	}
	if ( $elem eq "LCD_DISPLAY_2(string)" )
	{
	  print OF "$modulename\_O_LCD_DISPLAY_2(char val[])\n";
 	  print OF "{\n";
	  print OF "    lcd_cursor(2,1);\n";
	  print OF "	lcd_string(val);\n";
	  print OF "}\n";	
	}
	if ( $elem eq "LCD_DISPLAY_INT_1(integer)" )
	{
	  print OF " $modulename\_O_LCD_DISPLAY_INT_1(int val)\n";
 	  print OF "{\n";
	  print OF "	LCD_DISPLAY_INT_1(val);\n";
	  print OF "}\n";	
	}
	if ( $elem eq "LCD_DISPLAY_INT_2(integer)" )
	{
	  print OF " $modulename\_O_LCD_DISPLAY_INT_2(int val)\n";
 	  print OF "{\n";
	  print OF "	LCD_DISPLAY_INT_2(val);\n";
	  print OF "}\n";	
	}
	if ( $elem eq "SEND(integer)" )
	{
	  print OF "$modulename\_O_SEND(int val)\n";
 	  print OF "{\n";
	  print OF "	send_integer(val);\n";
	  print OF "}\n";	
	}
	if ( $elem eq "SET_FIR_THRESHOLD(integer)" )
	{
	  print OF "$modulename\_O_SET_FIR_THRESHOLD(int val)\n";
 	  print OF "{\n";
	  print OF "	IR_THRESHHOLD[0] = val;\n";
	  print OF "}\n";	
	}
	if ( $elem eq "SET_LIR_THRESHOLD(integer)" )
	{
	  print OF "$modulename\_O_SET_LIR_THRESHOLD(int val)\n";
 	  print OF "{\n";
	  print OF "	IR_THRESHHOLD[1] = val;\n";
	  print OF "}\n";	
	}
	if ( $elem eq "SET_RIR_THRESHOLD(integer)" )
	{
	  print OF "$modulename\_O_SET_RIR_THRESHOLD(int val)\n";
 	  print OF "{\n";
	  print OF "	IR_THRESHHOLD[2] = val;\n";
	  print OF "}\n";	
	}
 }
}

$length_sensor=@sensor_signature;
if( $length_sensor !=0 )
{
 for $ele (@sensor_signature)
 {
	if ( $ele eq "BATTERY_VOLTAGE(integer)" )
	{
	   print OF "int $modulename\_S_BATTERY_VOLTAGE(void)\n";
	   print OF "{\n";
	   print OF "	int val;\n";
	   print OF "	val=battery_voltage_calculation();\n";
	   print OF "        return val;\n";
	   print OF "}\n";
	}
	if ( $ele eq "FRONT_IR_VALUE(integer)" )
	{
	   print OF "int $modulename\_S_FRONT_IR_VALUE(void)\n";
	   print OF "{\n";
	   print OF "	int val;\n";
	   print OF "	val=front_dist_mm();\n";
	   print OF "        return val;\n";
	   print OF "}\n";
	}
	if ( $ele eq "FRONT_LEFT_IR_VALUE(integer)" )
	{
	   print OF "int $modulename\_S_FRONT_LEFT_IR_VALUE(void)\n";
	   print OF "{\n";
	   print OF "	int val;\n";
	   print OF "	val=front_left_dist_mm();\n";
	   print OF "        return val;\n";
	   print OF "}\n";
	}
	if ( $ele eq "FRONT_RIGHT_IR_VALUE(integer)" )
	{
	   print OF "int $modulename\_S_FRONT_RIGHT_IR_VALUE(void)\n";
	   print OF "{\n";
	   print OF "	int val;\n";
	   print OF "	val=front_right_dist_mm();\n";
	   print OF "        return val;\n";
	   print OF "}\n";
	}
	if ( $ele eq "REAR_IR_VALUE(integer)" )
	{
	   print OF "int $modulename\_S_REAR_IR_VALUE(void)\n";
	   print OF "{\n";
	   print OF "	int val;\n";
	   print OF "	val=rear_dist_mm();\n";
	   print OF "        return val;\n";
	   print OF "}\n";
	}
	if ( $ele eq "REAR_LEFT_IR_VALUE(integer)" )
	{
	   print OF "int $modulename\_S_REAR_LEFT_IR_VALUE(void)\n";
	   print OF "{\n";
	   print OF "	int val;\n";
	   print OF "	val=rear_left_dist_mm();\n";
	   print OF "        return val;\n";
	   print OF "}\n";
	}
	if ( $ele eq "REAR_RIGHT_IR_VALUE(integer)" )
	{
	   print OF "int $modulename\_S_REAR_RIGHT_IR_VALUE(void)\n";
	   print OF "{\n";
	   print OF "	int val;\n";
	   print OF "	val=rear_right_dist_mm();\n";
	   print OF "        return val;\n";
	   print OF "}\n";
	}
	if ( $ele eq "LEFT_IR_VALUE(integer)" )
	{
	   print OF "int $modulename\_S_LEFT_IR_VALUE(void)\n";
	   print OF "{\n";
	   print OF "	int val;\n";
	   print OF "	val=left_dist_mm();\n";
	   print OF "        return val;\n";
	   print OF "}\n";
	}
	if ( $ele eq "RIGHT_IR_VALUE(integer)" )
	{
	   print OF "int $modulename\_S_RIGHT_IR_VALUE(void)\n";
	   print OF "{\n";
	   print OF "	int val;\n";
	   print OF "	val=right_dist_mm();\n";
	   print OF "        return val;\n";
	   print OF "}\n";
	}
	if ( $ele eq "FRONT_SHARP_VALUE(integer)" )
	{
	   print OF "int $modulename\_S_FRONT_SHARP_VALUE(void)\n";
	   print OF "{\n";
	   print OF "	int val;\n";
	   print OF "	val=front_s_dist_mm();\n";
	   print OF "        return val;\n";
	   print OF "}\n";
	}
	if ( $ele eq "FRONT_LEFT_SHARP_VALUE(integer)" )
	{
	   print OF "int $modulename\_S_FRONT_LEFT_SHARP_VALUE(void)\n";
	   print OF "{\n";
	   print OF "	int val;\n";
	   print OF "	val=front_left_s_dist_mm();\n";
	   print OF "        return val;\n";
	   print OF "}\n";
	}
	if ( $ele eq "FRONT_RIGHT_SHARP_VALUE(integer)" )
	{
	   print OF "int $modulename\_S_FRONT_RIGHT_SHARP_VALUE(void)\n";
	   print OF "{\n";
	   print OF "	int val;\n";
	   print OF "	val=front_right_s_dist_mm();\n";
	   print OF "        return val;\n";
	   print OF "}\n";
	}
	if ( $ele eq "LEFT_SHARP_VALUE(integer)" )
	{
	   print OF "int $modulename\_S_LEFT_SHARP_VALUE(void)\n";
	   print OF "{\n";
	   print OF "	int val;\n";
	   print OF "	val=left_s_dist_mm();\n";
	   print OF "        return val;\n";
	   print OF "}\n";
	}
	if ( $ele eq "RIGHT_SHARP_VALUE(integer)" )
	{
	   print OF "int $modulename\_S_RIGHT_SHARP_VALUE(void)\n";
	   print OF "{\n";
	   print OF "	int val;\n";
	   print OF "	val=right_s_dist_mm();\n";
	   print OF "        return val;\n";
	   print OF "}\n";
	}
 }
}


print OF "\n/****************************** Main function ***************************/\n";
print OF "void main()\n";
print OF "{\n";
print OF " init_devices();\n";
print OF " servo_calibration();\n";
print OF " servo_on();\n";
print OF " delay_1s();\n";
print OF " delay_1s();\n";
print OF " delay_1s();\n";
print OF " lcd_init();\n";
print OF "lcd_set_4bit();\n";
print OF " $modulename\_reset();\n";
print OF " $modulename();\n";
$length_await=@await;
if ( $length_await != 0 )
{
  print OF " while(1)\n";
  print OF " {\n";
  for $e133 (@await)
  {
	#print $e133."\n";
	if ( $e133 eq  "BUMPSWITCH_ANY" )
  	{
	   print OF "    if(BUMPANY == 1)  { $modulename\_I_BUMPSWITCH_ANY(); }\n";
	}
	if ( $e133 eq  "BUMPSWITCH_1" )
  	{
	   print OF "    if(BUMP1 == 1)  { $modulename\_I_BUMPSWITCH_1(); }\n";
	}
	if ( $e133 eq  "BUMPSWITCH_2" )
  	{
	   print OF "    if(BUMP2 == 1)  { $modulename\_I_BUMPSWITCH_2(); }\n";
	}
	if ( $e133 eq  "BUMPSWITCH_3" )
  	{
	   print OF "    if(BUMP3 == 1)  { $modulename\_I_BUMPSWITCH_3(); }\n";
	}
	if ( $e133 eq  "BUMPSWITCH_4" )
  	{
	   print OF "    if(BUMP4 == 1)  { $modulename\_I_BUMPSWITCH_4(); }\n";
	}
	if ( $e133 eq  "BUMPSWITCH_5" )
  	{
	   print OF "    if(BUMP5 == 1)  { $modulename\_I_BUMPSWITCH_5(); }\n";
	}
	if ( $e133 eq "FRONT_IR_VALUE" )
	{
	   print OF "    $modulename\_I_FRONT_IR_VALUE(FRONT_IR);\n";
	}
	if ( $e133 eq "LEFT_IR_VALUE" )
	{
	   print OF "    $modulename\_I_LEFT_IR_VALUE(LEFT_IR);\n";
	}
	if ( $e133 eq "RIGHT_IR_VALUE" )
	{
	   print OF "    $modulename\_I_RIGHT_IR_VALUE(RIGHT_IR);\n";
	}
	if ( $e133 eq "FRONT_SHARP_VALUE" )
	{
	   print OF "    $modulename\_I_FRONT_SHARP_VALUE(FRONT_SHARP);\n";
	}
	if ( $e133 eq "LEFT_SHARP_VALUE" )
	{
	   print OF "    $modulename\_I_LEFT_SHARP_VALUE(LEFT_SHARP);\n";
	}
	if ( $e133 eq "RIGHT_SHARP_VALUE" )
	{
	   print OF "    $modulename\_I_RIGHT_SHARP_VALUE(RIGHT_SHARP);\n";
	}
	if ( $e133 eq "BATTERY_VOLTAGE" )
	{
	   print OF "    $modulename\_I_BATTERY_VOLTAGE(BATTERY_VOLTAGE);\n";
	}
	if ( $e133 eq  "RECEIVE" )
  	{
	   print OF "    if(DATA_RECEIVED == 1)\n"; #$modulename\_I_BUMPSWITCH_4(); }\n";
	   print OF "    {\n";
	   print OF "	   int val;\n";
	   print OF "	   val=receive_integer();\n";
	   print OF "      $modulename\_I_RECEIVE(val);\n";
	   print OF "    }\n";
	}
	if ( $e133 eq "FIR_LESS_THRE" )
	{
           print OF "    if(FRONT_IR < IR_THRESHHOLD[0])\n";
	   print OF "    {\n";
	   print OF "    	$modulename\_I_FIR_LESS_THRE();\n";
	   print OF "    }\n";
	}
	if ( $e133 eq "LIR_LESS_THRE" )
	{
           print OF "    if(LEFT_IR < IR_THRESHHOLD[1])\n";
	   print OF "    {\n";
	   print OF "    	$modulename\_I_LIR_LESS_THRE();\n";
	   print OF "    }\n";
	}
	if ( $e133 eq "RIR_LESS_THRE" )
	{
           print OF "    if(RIGHT_IR < IR_THRESHHOLD[0])\n";
	   print OF "    {\n";
	   print OF "    	$modulename\_I_RIR_LESS_THRE();\n";
	   print OF "    }\n";
	}
  }
  print OF "    $modulename();\n";
  print OF " }\n";
}
else
{ 
  print OF " $modulename();\n";
}
print OF "}\n";

close FH;
close OF;

