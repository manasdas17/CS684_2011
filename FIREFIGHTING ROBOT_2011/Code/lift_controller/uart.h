void uart0_init(void)
{
 UCSRB = 0x00; //disable while setting baud rate
 UCSRA = 0x00;
 UCSRC = 0x86;
 UBRRL = 0x2F; //set baud rate lo  //67 is for 16MHz 9600 baudrate
 UBRRH = 0x00; //set baud rate hi
 UCSRB = 0x98; 
}

unsigned char received_data=0xff;   // used to save Receiving data

SIGNAL(SIG_UART_RECV) 
{
 

 received_data = UDR;			// send back the echo + 1
 
 UDR = received_data+1;           // Echo the received data plus 1 
}


unsigned char get_data()
{
   unsigned char temp = received_data;
   received_data = 0xff;
   return temp;
}

void send_data(unsigned char data)
{
   UDR = data;
}

