/********************************************************************************
Title: dsPIC controller main file

Authors: Arjun Joshi
	 	 Prashant Bhut
	 	 Manish Patidar
		 
Version: 1.0

********************************************************************************/
/********************************************************************************

   Copyright (c) 2011, ERTS Lab IIT Bombay erts@cse.iitb.ac.in            
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

// dsPIC Header Files to be included
#include <stdio.h>
#include <math.h>
#include <dsp.h>
#include <p33Fxxxx.h>
#include <libpic30.h>

// Function Protoype declarations
void uart_config();						// Configures the UART module
void uart_port_config();				// Configures the Port for UART on dsPIC
void adc_config();						// Configures the ADC module
void initdma();							// Configures the DMA module for ADC data to be filled in dma buffer
void bpfilter();						// BandPass filter to filter out the noise

// Macro definitions
#define NUMSAMP			128				// total buffer space allocated to DMA
#define NUMTAPS			31				// number of coefficients for filter total 31
#define BLK_LEN			32				// 32 samples per each analog input

// Global definitions
// Filter coefficients. Obtained from MATLAB simulations								
float filt_coeff[] = {-0.0000,-0.0336,-0.0465,-0.0156,0.0264,0.0318,0.0065,0.0051,0.0471,0.0694,0.0000,-0.1281,
   					  -0.1798,-0.0658,0.1341,0.2333,0.1341,-0.0658,-0.1798,-0.1281,0.0000,0.0694,0.0471,0.0051,
    				   0.0065,0.0318,0.0264,-0.0156,-0.0465,-0.0336,-0.0000};
							
int BufferA[NUMSAMP] __attribute__((space(dma)));		// DMA buffer

// Input Buffer to store the sensor value converted by ADC
int ch0in[BLK_LEN],ch1in[BLK_LEN],ch2in[BLK_LEN],ch3in[BLK_LEN];

// Output buffers to store the digital data after filtering 			
float ch0out[BLK_LEN],ch1out[BLK_LEN],ch2out[BLK_LEN],ch3out[BLK_LEN];

float max_val = 26;					// Threshold value for gunshot detection. Value above this is detected as gunshot
									// Obtained by calibration for various sensors
									 
int dma_data_avail;					// 0 - all channel data not available in DMA
									// 1 - all channel data available in DMA 
int dma_num = 0;					// num of dma interrupt before data is available for filtering


/* Oscillator Configuration */
	
_FOSCSEL(FNOSC_PRI);								// 	Primary HS Oscillator
_FOSC(FCKSM_CSDCMD & OSCIOFNC_OFF  & POSCMD_HS); 	// 	Clock Switching is diabled and Fail Safe Clock Monitor is disabled
													// 	OSC2 Pin Function: OSC2 is Clock Output
													// 	Primary Oscillator Mode: Enabled
_FWDT(FWDTEN_OFF);              					// 	Watchdog Timer Enabled/disabled by user software					
_FICD(JTAGEN_OFF & ICS_PGD1);
_FPOR(ALTI2C_OFF);

/*******************************************************
This function configures the ADC module. 
Called from the main() function.
Value of each field of ADC registers is set here.
ADC is configured to sample 4 channel simultaneously
*******************************************************/
void adc_config()
{
	AD1CON1bits.ADSIDL  = 1;		// stop in Idle mode
	AD1CON1bits.ADDMABM = 1;		// Fill DMA buffers in order of conversion
	AD1CON1bits.FORM    = 0;		// Data Output Format: Integer
	AD1CON1bits.SSRC    = 7;		// Sample Clock Source: Internal clock starts conversion
	AD1CON1bits.ASAM    = 1;		// ADC Sample Control: Sampling begins immediately after conversion
	AD1CON1bits.AD12B   = 0;		// 10-bit 4 channel ADC operation
	AD1CON1bits.SIMSAM  = 1;		// Simultaneous sampling 4 channels
	AD1CON2bits.VCFG    = 0;		// AVdd and AVss are used for reference voltage
	AD1CON2bits.CSCNA   = 0;		// Do not scan inputs
	AD1CON2bits.CHPS    = 2;		// Converts CH0, CH1, CH2, CH3
	AD1CON2bits.SMPI    = 15;		// Increments DMA buffer address/interrupt after every 16 sample/conversion
	AD1CON2bits.BUFM    = 0;		// Buffers starts filling at 0x0
	AD1CON2bits.ALTS    = 0;		// Alternate selection disabled
	
	AD1CON3bits.ADRC    = 0;		// ADC Clock is derived from Systems Clock
	AD1CON3bits.ADCS    = 3;		// ADC Conversion Clock Tad=Tcy*(ADCS+1)= 1/(6Mhz); Tad = 4Tcy
	AD1CON3bits.SAMC    = 2;		// ADC Sampling time =  2*Tad
									// ADC Conversion Time for 10-bit Tc=12*Tab
									// Total sampling rate = 2Tad + 4*12Tad -- since converting 4 channels simultaneously
									// Sampling freq =  30 Khz
	
	AD1CON4bits.DMABL = 4;			// Allocating 16 word buffer for each analog input 	

	// Necessary to give positive input and negative input for Ch1,2,3 only.
	AD1CHS123bits.CH123SA = 0;		// Ch1 positive input for AN0, Ch2 positive input in AN1, Ch3 +ve input is AN2
	AD1CHS123bits.CH123NA = 3;		// Ch1 negative input is AN9,Ch2 negative input is AN10,Ch3 negative input is AN11 
 
	AD1CHS0bits.CH0SA = 12;			// Channel 0 postive input is AN12

	//AD1CSSL: A/D Input Scan Selection Register
	AD1CSSL = 0;			// Skip all channel scan
	
	//AD1PCFGH/AD1PCFGL: Port Configuration Register
	AD1PCFGL=0xFFFF;				// setting all pins as digital
	AD1PCFGLbits.PCFG0  = 0;		// AN0 as Analog Input for CH1 Pin 2
	AD1PCFGLbits.PCFG1  = 0;		// AN1 as Analog Input for CH2 Pin 3
	AD1PCFGLbits.PCFG2  = 0;		// AN2 as Analog Input for CH3 Pin 4
	AD1PCFGLbits.PCFG9  = 0;		// AN9 as Analog -ve Input as gnd for CH1 Pin 26
	AD1PCFGLbits.PCFG10 = 0;		// AN10 as Analog -ve Input as gnd for CH2 Pin 25
	AD1PCFGLbits.PCFG11 = 0;		// AN11 as Analog -ve Input as gnd for CH3 Pin 24
	AD1PCFGLbits.PCFG12 = 0;		// AN12 as Analog Input Pin 23 for CH0

 
    IFS0bits.AD1IF = 0;				// Clear the A/D interrupt flag bit
  	IEC0bits.AD1IE = 0;				// Disable A/D interrupt 
    AD1CON1bits.ADON = 1;			// Turn on the A/D converter
	
}	

/*******************************************************
This function configures the dma module. 
Called from the main() function.
Value of each field of DMA registers is set here.
ADC interrupts the DMA to store sensor data in the 
DMA buffers allocated. 
*******************************************************/
void initdma(void)
{
	DMA0CONbits.SIZE  = 0; 					// Word transfer
	DMA0CONbits.DIR   = 0; 					// ADC to DMA RAM
	DMA0CONbits.HALF  = 0;					// Initiate interrupt after all data moved
	DMA0CONbits.NULLW = 0; 					// Normal operation
	DMA0CONbits.AMODE = 0; 					// Configure DMA for Register indirect with post increment
	DMA0CONbits.MODE  = 2; 					// Configure DMA for Continuous Ping-Pong mode
	
	DMA0PAD 		  = (int)&ADC1BUF0;		// Address of ADC result data given to DMA.
	DMA0CNT 		  = 63;   				// Total DMA transfers to be 64
	DMA0REQ           = 13;					// Selecting ADC as the module to interrupt DMA engine
	DMA0STA 		  = __builtin_dmaoffset(BufferA);	
											// Allocating memory in DMA RAM
	IFS0bits.DMA0IF   = 0; 					// Clear the DMA interrupt flag bit
	IEC0bits.DMA0IE   = 1; 					// Set the DMA interrupt enable bit
	DMA0CONbits.CHEN  = 1; 					// DMA Channel Enable
}

/********************************************************************
This is the DMA interrupt service routine. 
The max number of buffers that can be allocated in dsPIC 
to 1 analog input is 16. So we after every 16 samples for
each of 4 channel is collected this function is called.
Since we require 32 samples for filtering, after every 2 interrupts
the samples are used for filtering.
********************************************************************/
void __attribute__((interrupt, no_auto_psv)) _DMA0Interrupt(void)
{
	
	int i = 0;

	/* Storing the sensor values in user buffers for filtering */
	for(i=0; i < 16; i++)
	{
		ch0in[i + 16*dma_num] = BufferA[4*i];
		ch1in[i + 16*dma_num] = BufferA[4*i + 1];	
		ch2in[i + 16*dma_num] = BufferA[4*i + 2];
		ch3in[i + 16*dma_num] = BufferA[4*i + 3];
	}
	
	/* After every 2 interrupts Data is available for filtering */
	if(dma_num == 0)
		dma_num = 1;
	else
	{
		dma_num = 0;
		dma_data_avail = 1;
	}
	
	IFS0bits.DMA0IF = 0; // Clearing the interrupt flag
}

/*******************************************************
This function configures the uart port 
Called from the main() function.
*******************************************************/
void uart_port_config()
{
	RPOR4bits.RP9R = 0x03;	  						// 	U1TX mapped to RP9 pin, Pin 18
} 

/*******************************************************
This function filters the sensor data.
Called from the main() function.
After filtering the output data is sent to FireBird
where the triangulation is done.
*******************************************************/
void bpfilter()
{
	// Variable definitions
	
	int i,j;
	float chout0 = 0;
	float chout1 = 0;
	float chout2 = 0;
	float chout3 = 0;

	// BandPass filter implemented for each channel
	// Circular convolution is implemented.
	for (i = 0; i < BLK_LEN-1; i++)
	{
		ch0out[i] = 0;
		for(j = 0; j < NUMTAPS; j++)
		{
			if( i-j < 0)
				ch0out[i] += ((ch0in[i-j + BLK_LEN-1] * filt_coeff[j])/NUMTAPS);
			else
				ch0out[i] += ((ch0in[i-j] * filt_coeff[j])/NUMTAPS);
		}
	}

	for (i = 0; i < BLK_LEN-1; i++)
	{
		ch1out[i] = 0;
		for(j = 0; j < NUMTAPS; j++)
		{
			if( i-j < 0)
				ch1out[i] += ((ch1in[i-j + BLK_LEN-1] * filt_coeff[j])/NUMTAPS);
			else
				ch1out[i] += ((ch1in[i-j] * filt_coeff[j])/NUMTAPS);
		}
	}

	for (i = 0; i < BLK_LEN-1; i++)
	{
		ch2out[i] = 0;
		for(j = 0; j < NUMTAPS; j++)
		{
			if( i-j < 0)
				ch2out[i] += ((ch2in[i-j + BLK_LEN-1] * filt_coeff[j])/NUMTAPS);
			else
				ch2out[i] += ((ch2in[i-j] * filt_coeff[j])/NUMTAPS);
		}
	}

	for (i = 0; i < BLK_LEN-1; i++)
	{
		ch3out[i] = 0;
		for(j = 0; j < NUMTAPS; j++)
		{
			if( i-j < 0)
				ch3out[i] += ((ch3in[i-j + BLK_LEN-1] * filt_coeff[j])/NUMTAPS);
			else
				ch3out[i] += ((ch3in[i-j] * filt_coeff[j])/NUMTAPS);
		}
	}

	// Squaring and Calculating the average value of the samples
	for(i = 0; i < BLK_LEN-1; i++)
	{
		chout0 += ch0out[i] * ch0out[i];
		chout1 += ch1out[i] * ch1out[i];
		chout2 += ch2out[i] * ch2out[i];
		chout3 += ch3out[i] * ch3out[i];		
	}

	chout0 = chout0/(BLK_LEN-1);
	chout1 = chout1/(BLK_LEN-1);
	chout2 = chout2/(BLK_LEN-1);
	chout3 = chout3/(BLK_LEN-1);

	// This commented code was used for calibrating the sensors value.
	// Find the max value for each sensor individually and calculate a common threshold.
	// All values can be sent via UART to PC where they are displayed.
/*
	if(max_val_ch0 < chout0)
		max_val_ch0 = chout0;
	
	if(max_val_ch1 < chout1)
		max_val_ch1 = chout1;
	
	if(max_val_ch2 < chout2)
		max_val_ch2 = chout2;

	if(max_val_ch3 < chout3)
		max_val_ch3 = chout3;
*/
	
	// If any of the channel data becomes greater than the threshold then send the data
	// to ATMEGA for traingulation.			
	if(chout0 > (max_val * 0.3) || chout1 > (max_val * 0.3) || chout2 > (max_val * 0.3) || chout3 > (max_val * 0.3))
	{
		while(!U1STAbits.TRMT);					// Wait till transmission completes	for 1 character
		U1TXREG = (char)chout0;					// transmitting channel 0 data to Firebird
		while(!U1STAbits.TRMT);					// Wait till transmission completes	for 1 character
		U1TXREG = (char)chout1;					// transmitting channel 1 data to Firebird
		while(!U1STAbits.TRMT);					// Wait till transmission completes	for 1 character
		U1TXREG = (char)chout2;					// transmitting channel 2 data to Firebird
		while(!U1STAbits.TRMT);					// Wait till transmission completes	for 1 character
		U1TXREG = (char)chout3;					// transmitting channel 3 data to Firebird	
	}
}		

/*************************************************************
This function configures the uart module of dsPIC
Called from the main() function.
Each UART register is individually configured here
Baud rate is 9600. 8 data bits No parity
There are multiple UARTs in dsPIC. Any one can be configured.
**************************************************************/
void uart_config()
{
	int i;
	
	// configure U1MODE
	
	U1MODEbits.UARTEN  = 0;						// Bit15 TX, RX DISABLED, ENABLE at end of func
	U1MODEbits.USIDL   = 0;						// Bit13 Continue in Idle
	U1MODEbits.IREN    = 0;						// Bit12 No IR translation
	U1MODEbits.RTSMD   = 0;						// Bit11 Simplex Mode
	U1MODEbits.UEN     = 0;						// Bits8,9 TX,RX enabled, CTS,RTS not
	U1MODEbits.WAKE    = 0;						// Bit7 No Wake up )
	U1MODEbits.LPBACK  = 0;						// Bit6 No Loop Back
	U1MODEbits.ABAUD   = 0;						// Bit5 No Autobaud (would require sending '55')
	U1MODEbits.URXINV  = 0;						// Bit4 IdleState = 1  (for dsPIC)
	U1MODEbits.BRGH    = 0;						// Bit3 16 clocks per bit period
	U1MODEbits.PDSEL   = 0;						// Bits1,2 8bit, No Parity
	U1MODEbits.STSEL   = 0;						// Bit0 One Stop Bit
	
	// Load a value into Baud Rate Generator.  Example is for 9600.
	U1BRG 				= 38;	// 12Mhz osc, 9600 Baud

	// Load all values in for U1STA SFR
	U1STAbits.UTXISEL1 = 0;						// Bit15 Interrupt when Char is transferred
	U1STAbits.UTXINV   = 0;						// Bit14 N/A, IRDA config
	U1STAbits.UTXISEL0 = 0;						// Bit13 Other half of Bit15
	U1STAbits.UTXBRK   = 0;						// Bit11 Disabled
	U1STAbits.UTXEN    = 0;						// Bit10 TX pins controlled by periph
	U1STAbits.UTXBF    = 0;						// Bit9 *Read Only Bit*
	U1STAbits.TRMT     = 0;						// Bit8 *Read Only bit*
	U1STAbits.URXISEL  = 0;						// Bits6,7 Int. on character recieved
	U1STAbits.ADDEN    = 0;						// Bit5 Address Detect Disabled
	U1STAbits.RIDLE    = 0;						// Bit4 *Read Only Bit*
	U1STAbits.PERR     = 0;						// Bit3 *Read Only Bit*
	U1STAbits.FERR     = 0;						// Bit2 *Read Only Bit*
	U1STAbits.OERR     = 0;						// Bit1 *Read Only Bit*
	U1STAbits.URXDA    = 0;						// Bit0 *Read Only Bit*

	IPC2 = 0x4000;								// Priority for Receive Interrupt
	IPC3 = 0x0004;								// Priority for Transmit Interrupt

	IFS0bits.U1TXIF = 0;						// Clear the Transmit Interrupt Flag
	IEC0bits.U1TXIE = 0;						// Disable Transmit Interrupts
	IFS0bits.U1RXIF = 0;						// Clear the Recieve Interrupt Flag
	IEC0bits.U1RXIE = 0;						// Disable Recieve Interrupts

	U1MODEbits.UARTEN = 1;						// turn the peripheral on
	U1STAbits.UTXEN = 1;						// Enable transimission
	
	/* wait at least 104 usec (1/9600) before sending first char */
	for(i = 0; i < 50; i++)
	{
		Nop();
	}

	CNPU1 = 0x0000;								// Pullups on change notification pins disabled
	CNPU2 = 0x0000;								// Pullups on change notification pins disabled
}

/*******************************************************
 main() function.
 Calls the configuration functions of dsPCI modules.
*******************************************************/
int main()
{
	adc_config();							// Calling adc configuration function
	uart_config();							// Calling uart configuration function
	uart_port_config();						// Calling uart port configuration function
	initdma();								// Calling dma configuration function
			
	dma_data_avail = 0;						// No data available for filtering. Set in dma interrupt routine.
	while(1)
	{
		// if dma data available for filtering call the filtering function from main.
		// reset the flag after filtering is completed.
		if(dma_data_avail == 1)
		{
			bpfilter();
			dma_data_avail = 0;
		}
	}	
	return 0;
}

