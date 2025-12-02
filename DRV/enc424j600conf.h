/******************************************
 * Title        : Microchip ENCX24J600 Ethernet Interface Driver
 * Author       : Jiri Melnikov
 * Created      : 29.03.2010
 * Version      : 0.2
 * Target MCU   : Atmel AVR series
 *
 * Description  : * This driver provides initialization and transmit/receive
 *                  functions for the Microchip ENCX24J600 100Mb Ethernet
 *                  Controller and PHY.
 *                * As addition, userspace access and hardware checksum
 *                  functions are available.
 *                * Only supported interface is SPI, no PSP interface available
 *                  by now.
 *                * No security functions are supported by now.
 *
 *                * This driver is inspired by ENC28J60 driver from Pascal
 *                  Stang (2005).
 *
 *                * Some lines of code are rewritten from Microchip's TCP/IP
 *                  stack.
 *
 * ****************************************/


#ifndef ENC424J600CONF_H
#define ENC424J600CONF_H

// ENC424J600 SPI port
#define ENC424J600_SPI_PORT		SPI_PORT
#define ENC424J600_SPI_DDR		SPI_DDR
#define ENC424J600_SPI_SCK		SPI_SCK
#define ENC424J600_SPI_MOSI		SPI_MOSI
#define ENC424J600_SPI_MISO		SPI_MISO
#define ENC424J600_SPI_SS		SPI_SS
// ENC424J600 control port
#define ENC424J600_CONTROL_PORT CONTROL_PORT
#define ENC424J600_CONTROL_DDR  CONTROL_DDR
#define ENC424J600_CONTROL_CS	CONTROL_CS

// ENC424J600 config
#define RAMSIZE         		(0x6000)
#define TXSTART                 (0x0000)        // Transmit buffer start
#define USSTART                 (0x0600)        // User space buffer start (to disable user space set to 0x6000 - should be an even memory address)
#define USEND                   (0x15FF)        // User space buffer end (to disable user space set to 0x6001 - should be an odd memory address)
#define RXSTART                 (0x1600)	// Recieve buffer start (should be an even memory address)




#endif
