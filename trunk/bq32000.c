#include <msp430g2553.h>
#define SDA     BIT7		// SDA on P1.7
#define SCL		BIT6     	// SCL on P1.6

unsigned char Timer_Temperture[16];
unsigned char Timer_from_PC[7];

void ConfigI2C()
{
	 P1DIR |= SCL;                         
	 P1DIR &= ~SDA; 
	 
	 do
	 {
		P1OUT &= ~SCL;
		__no_operation(); 
		P1OUT |= SCL;
		__no_operation(); 
	 }
	 while(P1IN&SDA == 0);
	 
	 P1SEL |= SCL + SDA;					// SDA and SCL is true
	 P1SEL2 |= SCL + SDA;					// SDA and SCL is true
	 
	 UCB0CTL0 |= UCMST + UCMODE_3 + UCSYNC;	// Master mode select, I2C mode, synchronous mode enable
	 UCB0CTL1 |= UCSSEL_2 + UCSWRST;		// USCI clock from SMCLK and Enable SW reset
	 
	 UCB0BR0 = 10;							// value = SMCLK / 100khz 
	 UCB0BR1 = 0;							// value = SMCLK / 100khz 
	 
	 UCB0I2CSA = 0x68;						// Set slave address
	 UCB0I2CIE |= UCNACKIE; 				// Cho phep ngat acnk recever
	 UCB0CTL1 &= ~UCSWRST;       			// Clear SW reset, resume operation	
}
//==============================================================
unsigned char Read_RTC(unsigned char address)
{
	while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent	
	
	UCB0CTL1 |= UCTR + UCTXSTT;             // I2C TX, start condition       
	
	while (!(IFG2&UCB0TXIFG));     
	UCB0TXBUF = address;					// Address to read

	while (!(IFG2&UCB0TXIFG));  

	UCB0CTL1 &= ~UCTR;                      // I2C RX 
	UCB0CTL1 |= UCTXSTT;                    // I2C start condition	
	IFG2 &= ~UCB0TXIFG;                     // Clear USCI_B0 TX int flag
	
	while (UCB0CTL1 & UCTXSTT);             // Loop until I2C STT is sent
    UCB0CTL1 |= UCTXSTP;                    // I2C stop condition after 1st TX
	
	return(UCB0RXBUF);
}

//=========================================================================
void Wirte_RTC(unsigned char *time_data)
{

	while (UCB0CTL1 & UCTXSTP);             // Loop until I2C STT is sent
	UCB0CTL1 |= UCTR + UCTXSTT;             // I2C TX, start condition

	while (!(IFG2&UCB0TXIFG)); 
	UCB0TXBUF = 0x00; 						// adress

	for(unsigned char i = 0; i < 7; i++)
	{
		while (!(IFG2&UCB0TXIFG)); 
		UCB0TXBUF = time_data[i];
	}	
	
	while (!(IFG2&UCB0TXIFG));	
	UCB0CTL1 |= UCTXSTP;                    // I2C stop condition after 1st TX
	IFG2 &= ~UCB0TXIFG;                     // Clear USCI_B0 TX int flag 
}	
//==========================================================================

