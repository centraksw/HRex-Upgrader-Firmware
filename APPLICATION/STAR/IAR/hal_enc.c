/**
 * @file  hal_enc.c
 *
***************************************************************************/
//#include  <msp430x16x.h>
#include "msp430f5659.h"
#include "timer_drv.h"
#include "hal_enc.h"
#include "SPI.h"
#define	_delay_us(x)	CCR_Delay(x*20)
//#define	_delay_us(x)	__delay_cycles(x*20)
#define CLK_DELAY       10


/**********************************************************************//**
 * @brief  Initializes the ports and serial communication peripherals to
 *         communicate with the enc.
 *
 * @param  none
 *
 * @return none
 **************************************************************************/
void halEncInit(void)
{
  // Set up pins used by peripheral unit
  //ENC_PORT_DIR |= ENC_SPI_SIMO + ENC_SPI_CLK + ENC_SPI_CS;
  //ENC_PORT_DIR &= ~ENC_SPI_SOMI;
  
  SPI_Init();  
}

/**********************************************************************//**
 * @brief  Writes the data to the radio register at address.
 *
 * @param  address     Radio register address to which to write the SPI data
 *
 * @param  data        Radio register setting to be written
 *
 * @return receiveChar Dummy receive character for SPI communication
 **************************************************************************/
void halEncWriteByte(unsigned char data)
{
    SPI_Enable();
    SPI_WriteByte( data );
    SPI_Disable();
  
}

void EncSPI_WriteOpcode(unsigned char opcode)
{
    SPI_Enable();
    SPI_WriteByte( opcode );
    SPI_Disable();
}

void EncSPI_WriteRegister08(unsigned char regAddr, unsigned char data)
{
    SPI_Enable();
    SPI_WriteByte( regAddr );
    SPI_WriteByte( data );
    SPI_Disable();
}

void EncSPI_WriteRegister16(unsigned char regAddr, unsigned short data)
{
  unsigned char lsb, msb;
  lsb = (data & 0xFF);
  msb = ((data & 0xFF00)>>8);
 
  SPI_Enable();
  SPI_WriteByte( regAddr );
  SPI_WriteByte( lsb );
  SPI_WriteByte( msb );
  SPI_Disable();
}

void EncSPI_WriteRegisterUnbanked(unsigned char regAddr, unsigned short data)
{
    unsigned char lsb, msb, cmd;
    cmd = 0x22;
    lsb = (data & 0xFF);
    msb = ((data & 0xFF00)>>8);

    SPI_Enable();
    SPI_WriteByte( cmd );
    SPI_WriteByte( regAddr );
    SPI_WriteByte( lsb );
    SPI_WriteByte( msb );
    SPI_Disable();
}

void EncSPI_WriteRegisterN(unsigned char opcode, unsigned char *datas, unsigned short len)
{
    SPI_Enable();
    SPI_WriteByte( opcode );
    SPI_WriteBytes( datas, len);
    SPI_Disable();
}

unsigned char EncSPI_ReadRegister08(unsigned char regAddr)
{
    unsigned char data = 0;
    SPI_Enable();
    SPI_WriteByte( regAddr );
    SPI_ReadByte( &data );
    SPI_Disable();
    return data;
}

unsigned short EncSPI_ReadRegister16(unsigned char regAddr)
{
    unsigned char lsb=0, msb=0;
    unsigned short data = 0;
    SPI_Enable();
    SPI_WriteByte( regAddr );
    SPI_ReadByte( &lsb );
    SPI_ReadByte( &msb );
    SPI_Disable();
    data = (unsigned short)(msb << 8) + (unsigned short)lsb;
    return data;
}

unsigned long EncSPI_ReadRegisterUnbanked(unsigned char regAddr)
{
    unsigned char lsb, msb;
    unsigned long data = 0;
    unsigned char cmd = 0x20;

    SPI_Enable();
    SPI_WriteByte( cmd );
    SPI_WriteByte( regAddr );
    SPI_ReadByte( &lsb );
    SPI_ReadByte( &msb );
    SPI_Disable();

    data = (unsigned short)(msb << 8) | (unsigned short)lsb;

    return data;
}

void EncSPI_ReadRegisterN(unsigned char opcode, unsigned char *datas, unsigned short len)
{
  SPI_Enable();
  SPI_WriteByte( opcode );
  SPI_ReadBytes( datas, len);
  SPI_Disable();
}

/**********************************************************************//**
 * @brief  Reads a byte from the enc.
 *
 * @param  Nothing
 *
 * @return receiveChar byte that is read from the enc
 **************************************************************************/
unsigned char halEncReadByte(void)
{
   unsigned char data = 0;
  
   SPI_Enable();
   SPI_ReadByte(&data);
   SPI_Disable();

   return data;
}
