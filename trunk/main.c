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
#define RXD	BIT2     	// RXD on P1.2
volatile long tempRaw;
volatile unsigned int i;

void FaultRoutine(void);
void ConfigWDT(void);
void ConfigClocks(void);
void ConfigLEDs(void);
void ConfigADC10(void);
void ConfigTimerA2(void);

void main(void)
{
  ConfigWDT();
  ConfigClocks();
  ConfigLEDs();
  ConfigADC10();
  ConfigTimerA2();
  ConfigUART();
  _BIS_SR(GIE);
  
  Print_UART("This is gaimande\n\r");
  while(1)
  {
   P1OUT |= BIT0;
   for (i = 0; i < 100; i++);
   P1OUT &= ~BIT0;
   for (i = 0; i < 5000; i++);
   i = 0;
   if((P1IN & BIT5))
    P2OUT |= BIT0;
   else 
     P2OUT &= ~BIT0;
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
  BCSCTL3 |= LFXT1S_2;                      // LFXT1 = VLO
  IFG1 &= ~OFIFG;                           // Clear OSCFault flag
  BCSCTL2 |= SELM_0 + DIVM_3 + DIVS_3;      // MCLK = DCO/8, SMCLK = DCO/8
 }

void FaultRoutine(void)
 {
   P1OUT = BIT0;                            // P1.0 on (red LED)
   while(1); 			                    // TRAP
 }

void ConfigLEDs(void)
 {
  P1SEL = TXD + RXD;						// P1.1 & 2 TA0, rest GPIO
  P1SEL2 = TXD + RXD;						// P1.1 & 2 TA0, rest GPIO

  P1DIR = ~(BIT3 + BIT5 + RXD);					// P1.3 input, other outputs
  P1REN |= BIT5;                                                // P1.5 pull-up resistor enable
  P1IE |= BIT5;                                                 // Enable P1.5 interrupt
  P1IES &= ~BIT5;                                               // Low to High transition
  P1IFG &= ~BIT5;                                               //Clear interrupt Flag
  P1OUT = 0;              					// clear output pins
  P2SEL = 0; 					// P2.6 and 7 GPIO
  P2DIR |= BIT0 ;	  					// P1.6 and 7 outputs
  P2OUT = 0;		  				        // clear output pins
 }

void ConfigADC10(void)
 {
  ADC10CTL1 = INCH_10 + ADC10DIV_0;        // Temp Sensor ADC10CLK
 }

void ConfigTimerA2(void)
  {
   CCTL0 = CCIE;
   CCR0 = 12000;
   TACTL = TASSEL_1 + MC_2;
  }

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
  ADC10CTL0 = SREF_1 + ADC10SHT_3 + REFON + ADC10ON;
  __delay_cycles(5);                         // Wait for ADC Ref to settle
  ADC10CTL0 |= ENC + ADC10SC;               // Sampling and conversion start
  P1OUT |= BIT6; 			                // P1.6 on (green LED)
  __delay_cycles(100);
  ADC10CTL0 &= ~ENC;				   		// Disable ADC conversion
  ADC10CTL0 &= ~(REFON + ADC10ON);        	// Ref and ADC10 off
  tempRaw = ADC10MEM;						// Read conversion value
  P1OUT &= ~BIT6; 				                // green LED off
  CCR0 +=12000;								// add 1 second to the timer  
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{    
	Send_Char(UCA0RXBUF);
}

#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{    
	P2OUT |= BIT0;
        P1IFG &= ~BIT5;         //Clear interrupt Flag
}