/*******************************************************************************
    Filename: hal_enc.h
***************************************************************************/

#ifndef HAL_ENC_H
#define HAL_ENC_H

#define ENC_SPI_CS       BIT0
#define ENC_SPI_CLK      BIT1
#define ENC_SPI_SIMO     BIT2
#define ENC_SPI_SOMI     BIT3

/*-------------------------------------------------------------
 *                  Function Prototypes
 * ------------------------------------------------------------*/
void halEncInit(void);
void halEncWriteByte(unsigned char data);
unsigned char halEncReadByte(void);


void EncSPI_WriteOpcode(unsigned char opcode);
void EncSPI_WriteRegister08(unsigned char regAddr, unsigned char data);
void EncSPI_WriteRegister16(unsigned char regAddr, unsigned short data);
void EncSPI_WriteRegisterUnbanked(unsigned char regAddr, unsigned short data);
void EncSPI_WriteRegisterN(unsigned char opcode, unsigned char *data, unsigned short len);

unsigned char EncSPI_ReadRegister08(unsigned char regAddr);
unsigned short EncSPI_ReadRegister16(unsigned char regAddr);
unsigned long EncSPI_ReadRegisterUnbanked(unsigned char regAddr);
void EncSPI_ReadRegisterN(unsigned char opcode, unsigned char *data, unsigned short len);

#endif
