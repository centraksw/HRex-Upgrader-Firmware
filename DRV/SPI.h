#ifndef __SPI_H
#define __SPI_H


#include "defines.h"
#include "hal_enc.h"

#define ENC_CSn                  ENC_SPI_CS
#define ENC_CS                   ENC_SPI_CS
#define ENC_SI                   ENC_SPI_SIMO
#define ENC_SO                   ENC_SPI_SOMI
#define ENC_SCLK                 ENC_SPI_CLK
#define ENC_CLK                  ENC_SPI_CLK

#define ENC_CSn_IN               P8IN
#define ENC_CS_IN                P8IN
#define ENC_SCLK_IN              P8IN
#define ENC_CLK_IN               P8IN
#define ENC_SI_IN                P8IN
#define ENC_SO_IN                P8IN

#define ENC_CSn_OUT              P8OUT
#define ENC_CS_OUT               P8OUT
#define ENC_SCLK_OUT             P8OUT
#define ENC_CLK_OUT              P8OUT
#define ENC_SI_OUT               P8OUT
#define ENC_SO_OUT               P8OUT

#define ENC_CSn_DIR              P8DIR
#define ENC_CS_DIR               P8DIR
#define ENC_SCLK_DIR             P8DIR
#define ENC_CLK_DIR              P8DIR
#define ENC_SI_DIR               P8DIR
#define ENC_SO_DIR               P8DIR

#define ENC_CSn_SEL              P8SEL
#define ENC_CS_SEL               P8SEL
#define ENC_SCLK_SEL             P8SEL
#define ENC_CLK_SEL              P8SEL
#define ENC_SI_SEL               P8SEL
#define ENC_SO_SEL               P8SEL

void SPI_Enable();
void SPI_Disable();
void SPI_Init();
void SPI_WriteByte(UINT8 byte);
void SPI_WriteBytes(UINT8* buf, WORD cnt);
void SPI_ReadBytes(UINT8 *buff, WORD count);
void SPI_ReadByte(UINT8 *byte);
void Timer_Delay(UINT16 time);
#endif