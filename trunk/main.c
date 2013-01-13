//***********************************************************
// Lab5.c  Temperature sampling example using ADC10
//         Clocking: MCLK and SMCLK = 1.1MHz DCO/8
//					 ACLK = 12kHz VLO
//
// SFB 1/2012
//***********************************************************

#include <msp430g2553.h>
#include "uart_simple.h"
#ifndef TIMER0_A1_VECTOR
#define TIMER0_A1_VECTOR    TIMERA1_VECTOR
#define TIMER0_A0_VECTOR    TIMERA0_VECTOR
#endif

#define TXD     BIT1		// TXD on P1.1
#define RXD		BIT2     	// RXD on P1.2

volatile unsigned long t1, t2, tempRaw;
unsigned char flag_timeout, flag_gas;

void FaultRoutine(void);
void ConfigWDT(void);
void ConfigClocks(void);
void ConfigLEDs(void);
void ConfigADC10(void);
void ConfigTimerA2(void);
void Warning(void);

void main(void)
{
	
	ConfigWDT();
	ConfigClocks();
	ConfigLEDs();
	ConfigADC10();
	ConfigTimerA2();
	ConfigUART();
	Print_UART("This is gaimande\n\r");
	standby:	
	_BIS_SR(LPM3_bits + GIE);
	while(1)
	{		
		if (flag_timeout == 1)
		{
			// Buzzer TIME mode
			P1OUT |= BIT4; 			                // Light_Run ON
			
			P2OUT |= BIT0;							// Buzzer is ON
			__delay_cycles(10000);					// Delay 80ms, value = (time in second) * (DC0/8)
			P2OUT &= ~BIT0;							// Buzzer is OFF
			__delay_cycles(10000);					// Delay 80ms
			
			P2OUT |= BIT0;							// Buzzer is ON
			__delay_cycles(10000);					// Delay 80ms, value = (time in second) * (DC0/8)
			P2OUT &= ~BIT0;							// Buzzer is OFF
			__delay_cycles(10000);					// Delay 80ms
			
			P2OUT |= BIT0;							// Buzzer is ON
			__delay_cycles(10000);					// Delay 80ms, value = (time in second) * (DC0/8)
			P2OUT &= ~BIT0;							// Buzzer is OFF
			__delay_cycles(10000);					// Delay 80ms
			
			P2OUT |= BIT0;							// Buzzer is ON
			__delay_cycles(10000);					// Delay 80ms, value = (time in second) * (DC0/8)
			P2OUT &= ~BIT0;							// Buzzer is OFF
			__delay_cycles(10000);					// Delay 80ms
			
			P2OUT |= BIT0;							// Buzzer is ON
			__delay_cycles(10000);					// Delay 80ms, value = (time in second) * (DC0/8)
			P2OUT &= ~BIT0;							// Buzzer is OFF
			__delay_cycles(30000);					// Delay 80ms
			
			P2OUT |= BIT0;							// Buzzer is ON
			__delay_cycles(8000);					// Delay 80ms, value = (time in second) * (DC0/8)
			P2OUT &= ~BIT0;							// Buzzer is OFF
			__delay_cycles(10000);					// Delay 80ms
			
			P2OUT |= BIT0;							// Buzzer is ON
			__delay_cycles(30000);					// Delay 80ms, value = (time in second) * (DC0/8)
			P2OUT &= ~BIT0;							// Buzzer is OFF
			__delay_cycles(100000);					// Delay 80ms
			
		}
		else if (flag_gas == 1)
		{
			P2IE &= ~BIT3; 							// Start INT disable
			if(P1IN & BIT5)							// Keep warning til Gas dose not appear
			{
				// Buzzer GAS_OUT mode
				P1OUT |= BIT3;						// light_gas ON
				P2OUT |= BIT0;						// Buzzer is ON
				__delay_cycles(7000);				// Delay 80ms, value = (time in second) * (DC0/8)
				P1OUT &= ~BIT3;						// light_gas OFF
				P2OUT &= ~BIT0;						// Buzzer is OFF
				__delay_cycles(7000);				// Delay 80ms
				P1OUT |= BIT3;							// light_gas ON
				P2OUT |= BIT0;						// Buzzer is ON
				__delay_cycles(100000);
				P1OUT &= ~BIT3;						// light_gas OFF
				P2OUT &= ~BIT0;						// Buzzer is OFF
				__delay_cycles(7000);				// Delay 80ms
			}
			else
			{
				// Buzzer OVER mode
				P1OUT |= BIT3;						// light_gas ON
				P2OUT |= BIT0;						// Buzzer is ON
				__delay_cycles(20000);				// Delay 80ms, value = (time in second) * (DC0/8)
				P2OUT &= ~BIT0;						// Buzzer is OFF
				__delay_cycles(4000);				// Delay 80ms
				P2OUT |= BIT0;						// Buzzer is ON
				__delay_cycles(20000);				// Delay 80ms, value = (time in second) * (DC0/8)				
				P2OUT &= ~BIT0;						// Buzzer is OFF
				__delay_cycles(200000);				// Delay 80ms
			}
		}
		else
			goto standby;		
	}		
}

void ConfigWDT(void)
{
	WDTCTL = WDTPW + WDTHOLD;                 	// Stop watchdog timer
}

void ConfigClocks(void)
{
	if (CALBC1_1MHZ ==0xFF || CALDCO_1MHZ == 0xFF)
	FaultRoutine();		                    // If calibration data is erased
											// run FaultRoutine()
	BCSCTL1 = CALBC1_1MHZ; 					// Set range
	DCOCTL = CALDCO_1MHZ;  					// Set DCO step + modulation
	BCSCTL3 |= LFXT1S_2;                    // LFXT1 = VLO
	IFG1 &= ~OFIFG;                         // Clear OSCFault flag
	BCSCTL2 |= SELM_0 + DIVM_3 + DIVS_3;    // MCLK = DCO/8, SMCLK = DCO/8
}

void FaultRoutine(void)
{
	P1OUT = BIT0;                           // P1.0 on (red LED)
	while(1); 			                    // TRAP
}

void ConfigLEDs(void)
{
	P1SEL = TXD + RXD;						// P1.1 & 2 TA0, rest GPIO
	P1SEL2 = TXD + RXD;						// P1.1 & 2 TA0, rest GPIO

	P1DIR = ~(BIT0 + BIT5 + RXD);			// ADC pin, MQ2 pin adn RXD pin as inputs, other outputs
	P1REN |= BIT0 + BIT5;                   // Pull-up resistor enable

	P1IE |= BIT5;                           // Enable GAS interrupt
	P1IES &= ~BIT5;                         // Low to High transition

	P1IFG = 0;
	P2IFG = 0;
	
	P2SEL = 0; 								// XIN and XOUT pins as GPIOs
	P2DIR = ~(BIT3 + BIT4 + BIT5);		  	// Run, Stop and Fao buttons as inputs, other outputs
	P1REN |= BIT3 + BIT4 + BIT5;            // Pull-up resistor enable

	P2IE |= BIT3 + BIT4 ;          			// Enable Buttons interrupt
	P2IES |= (BIT3 + BIT4 + BIT5);        	// High to Low transition
	P2IFG &= ~(BIT3 + BIT4 + BIT5);     	// Clear interrupt Flags

	P1OUT = 0;              				// clear output pins  
	P2OUT = 0;		  				        // clear output pins
}

void ConfigADC10(void)
{
	ADC10CTL1 = INCH_0 + ADC10DIV_0;        // INCH_0 selects the analog chanel A0
											// ADC10DIV_0 selects divide-by-1 as the ADC10 clock 
}

void ConfigTimerA2(void)
{
	CCTL0 &= ~CCIE;							// Disable capture/compare mode
	CCR0 = 6000;							// Select the ACLK (VLO) and sets the operation for up mode
	TACTL = TASSEL_1 + MC_1;				// CCR0 = 12000 x (time in second)
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
	// Read ADC value
	ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10ON;	// SREF_0: selects the range from Vss to Vcc
												// ADC10SHT_3: maximum sample-and-hold time
												// ADC10ON: turns on the ADC10 peripheral
	ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
	ADC10CTL0 &= ~ENC;				   		// Disable ADC conversion
	ADC10CTL0 &= ~ADC10ON;		        	// ADC10 off
	if (ADC10MEM > 617)
		tempRaw = 3;
	else if (ADC10MEM > 542)
		tempRaw = 1200;	
	else if (ADC10MEM > 484)
		tempRaw = 1800;
	else if (ADC10MEM > 326)
		tempRaw = 2400;
	else if (ADC10MEM > 177)
		tempRaw = 3000;	
	else
		tempRaw = 3600;	

	if (t1 == tempRaw)		
	{
		t2--;
		if(t2 == 0)
		{
			_BIC_SR_IRQ(LPM3_bits);			// LPM off
			flag_timeout = 1;
			CCTL0 &= ~CCIE;					// Disable capture/compare mode
		}
	}
	else
	{
		// BEEP
		P2OUT |= BIT0;						// Buzzer is ON
		__delay_cycles(10000);				// Delay 80ms, value = (time in second) * (DC0/8)
		P2OUT &= ~BIT0;						// Buzzer is OFF
		
		t1 = tempRaw;
		t2 = t1;
	}	
	
	P1OUT ^= BIT6; 				            // Green LED off
	P1OUT ^= BIT4; 				      		// Light_Run blink
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{    
	Send_Char(UCA0RXBUF);
}

// GAS ISR
#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{    
	flag_gas = 1;
	flag_timeout = 0;						// Warning while buzzer in TIMEOUT mode
	P1IFG &= ~BIT5;         				// Clear interrupt Flag for next warn	
	CCTL0 &= ~CCIE;						// Disable capture/compare mode
	_BIC_SR_IRQ(LPM3_bits);					// LPM off
}

// Buttons ISR
#pragma vector=PORT2_VECTOR
__interrupt void PORT2_ISR(void)
{    
	
	if ((P2IFG&BIT3) == BIT3)				// RUN button is pressed
	{
		P1OUT |= BIT4;						// light_run ON
		t1 = 0;
		CCTL0 = CCIE;						// Enable capture/compare mode
		P2IFG &= ~BIT3;         			// Clear interrupt Flag for next warning
	}
	else if ((P2IFG&BIT4) == BIT4)			// STOP button is pressed
	{
		// BEEP
		P2OUT |= BIT0;						// Buzzer is ON
		__delay_cycles(10000);				// Delay 80ms, value = (time in second) * (DC0/8)
		P2OUT &= ~BIT0;						// Buzzer is OFF
		
		P1OUT &= ~(BIT3+BIT4);				// light_run and light_gas OFF
		P2OUT &= ~BIT0;						// Buzzer is OFF
		flag_gas = 0;
		flag_timeout = 0;
		CCTL0 &= ~CCIE;						// Disable capture/compare mode
		P2IE |= BIT3; 						// Start INT enable
		P2IFG &= ~BIT4;         			// Clear interrupt Flag for next warning
	}	
	
}