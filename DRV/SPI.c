
#include "SPI.h"
#include  <msp430f5659.h>
#include "timer_drv.h"
#include "defines.h"

void SPI_Init()
{
      // Init Port 1
      ENC_CSn_DIR |=  ENC_CSn;
      
      // Enable SPI Mode
      SPI_Enable();

      UCA1CTL1 = UCSWRST;                     // SMCLK
      UCA1CTL0 |= UCCKPH + UCMSB +  UCMST + UCSYNC;    // 3-pin, 8-bit SPI master
      UCA1CTL1 |= UCSSEL__SMCLK;
      UCA1BR0 = 0x00;
      UCA1BR1 = 0x00; 
      UCA1CTL1 &= ~UCSWRST;
      UCA1IFG &= ~UCRXIFG;

      SPI_Disable();
}
void SPI_Enable()
{
    P2OUT |= BIT7;  
    P2REN |= BIT7;                         
    
    ENC_SI_SEL |= ENC_SI;
    ENC_SI_DIR |= ENC_SI;  
    
    ENC_SO_DIR &= ~ENC_SO;
    ENC_SO_SEL |= ENC_SO;
    
    ENC_SCLK_DIR |= ENC_SCLK;
    ENC_SCLK_SEL |= ENC_SCLK;
    
    ENC_CSn_DIR |=  ENC_CSn;
    ENC_CSn_OUT &=  ~ENC_CSn;
}

void SPI_Disable()
{
    ENC_CSn_DIR |=  ENC_CSn;
    ENC_CSn_OUT |=  ENC_CSn;
}

void SPI_WriteByte(UINT8 byte)
{
   while (!(UCA1IFG&UCTXIFG));           // USCI_A0 TX buffer ready?
   UCA1TXBUF = byte;
}

void SPI_WriteBytes(UINT8* buf, WORD cnt)
{

    for(WORD idx=0; idx<cnt;idx++)
    {
        while (!(UCA1IFG&UCTXIFG));           // USCI_A0 TX buffer ready?
        UCA1TXBUF = buf[idx];
    }
}

VOID SPI_ReadByte(UINT8 *byte)
{
   while ( !(UCA1IFG&UCTXIFG));         // USCI_A0 TX buffer ready?
    UCA1TXBUF = 0xFF;
    __delay_cycles(20);
    *byte = UCA1RXBUF;    

}


VOID SPI_ReadBytes(UINT8 *buff, WORD count)
{
    UINT16 idx;
    for(idx=0; idx<count; idx++) 
    {
        buff[idx] = 0;
        SPI_ReadByte( &buff[idx] );
    }
}

