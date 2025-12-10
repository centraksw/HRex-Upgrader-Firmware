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

#include "enc424j600.h"
#include "enc424j600conf.h"
#include "defines.h"
#include "timer_drv.h"
#include "hal_enc.h"

#define	_delay_us(x)	CCR_Delay(x*20)

// Binary constant identifiers for ReadMemoryWindow() and WriteMemoryWindow()
// functions
#define UDA_WINDOW		(0x1)
#define GP_WINDOW		(0x2)
#define RX_WINDOW		(0x4)

// Promiscuous mode, uncomment if you want to receive all packets, even those which are not for you
//#define PROMISCUOUS_MODE

static uint8_t currentBank;
static uint16_t nextPacketPointer;

// Static functions
static void enc424j600SendSystemReset(void);

static bool enc424j600MACIsTxReady(void);
static void enc424j600MACFlush(void);
static uint16_t enc424j600ChecksumCalculation(uint16_t position, uint16_t length, uint16_t seed);

static void enc424j600WriteMemoryWindow(uint8_t window, uint8_t *data, uint16_t length);
static void enc424j600ReadMemoryWindow(uint8_t window, uint8_t *data, uint16_t length);

static uint16_t enc424j600ReadReg(uint16_t address);
static void enc424j600WriteReg(uint16_t address, uint16_t data);
static uint16_t enc424j600ReadPHYReg(uint8_t address);
static void enc424j600WritePHYReg(uint8_t address, uint16_t Data);
static void enc424j600ReadN(uint8_t op, uint8_t* data, uint16_t dataLen);
static void enc424j600WriteN(uint8_t op, uint8_t* data, uint16_t dataLen);
static void enc424j600BFSReg(uint16_t address, uint16_t bitMask);
static void enc424j600BFCReg(uint16_t address, uint16_t bitMask);


/********************************************************************
* INITIALIZATION
* ******************************************************************/
void enc424j600Init(void) {

    //Set default bank
    currentBank = 0;

    // Perform a reliable reset
    enc424j600SendSystemReset();

    // Clear the PKTCNT field in ESTAT by setting the PKTDEC bit
    // (ECON1<8>) enough times for the count to reach zero.
    while((enc424j600ReadStatus() & 0xFF) != 0)
        enc424j600BFSReg(ECON1, ECON1_PKTDEC);

    // Initialize RX tracking variables and other control state flags
    nextPacketPointer = RXSTART;

    // Set up TX/RX/UDA buffer addresses
    enc424j600WriteReg(ETXST, TXSTART);
    enc424j600WriteReg(ERXST, RXSTART);
    enc424j600WriteReg(ERXTAIL, RAMSIZE - 2);
    enc424j600WriteReg(EUDAST, USSTART);
    enc424j600WriteReg(EUDAND, USEND);

    // If promiscuous mode is set, than allow accept all packets
#ifdef PROMISCUOUS_MODE
    enc424j600WriteReg(ERXFCON, (ERXFCON_CRCEN | ERXFCON_RUNTEN | ERXFCON_UCEN | ERXFCON_NOTMEEN | ERXFCON_MCEN | ERXFCON_BCEN));
#endif

    // Set PHY Auto-negotiation to support 10BaseT Half duplex,
    // 10BaseT Full duplex, 100BaseTX Half Duplex, 100BaseTX Full Duplex,
    // and symmetric PAUSE capability
    enc424j600WritePHYReg(PHANA, PHANA_ADPAUS0 | PHANA_AD10FD | PHANA_AD10 | PHANA_AD100FD | PHANA_AD100 | PHANA_ADIEEE0);

    enc424j600PowerSaveDisable();
}

/********************************************************************
* PACKET TRANSMISSION
* ******************************************************************/

bool enc424j600isPacketReceiveReady()
{
    if (!(enc424j600ReadReg(EIR) & EIR_PKTIF)) {
        return FALSE;
    }

    return TRUE;
}

/**
* Recieves packet
* */
uint16_t enc424j600PacketReceive(uint16_t len, uint8_t* packet) {
    uint16_t newRXTail;
    RXSTATUS statusVector;
    uint16_t recvLen;

    if (!(enc424j600ReadReg(EIR) & EIR_PKTIF)) {
        return 0;
    }

    // Set the RX Read Pointer to the beginning of the next unprocessed packet
    enc424j600WriteReg(ERXRDPT, nextPacketPointer);

    enc424j600ReadMemoryWindow(RX_WINDOW, (uint8_t*) & nextPacketPointer, sizeof (nextPacketPointer));

    enc424j600ReadMemoryWindow(RX_WINDOW, (uint8_t*) & statusVector, sizeof (statusVector));
    recvLen = (statusVector.bits.ByteCount <= len+4) ? statusVector.bits.ByteCount-4 : 0;
    if(recvLen > len)
        recvLen = len;
    enc424j600ReadMemoryWindow(RX_WINDOW, packet, recvLen);

    newRXTail = nextPacketPointer - 2;
    //Special situation if nextPacketPointer is exactly RXSTART
    if (nextPacketPointer == RXSTART)
        newRXTail = RAMSIZE - 2;

    //Packet decrement
    enc424j600BFSReg(ECON1, ECON1_PKTDEC);

    //Write new RX tail
    enc424j600WriteReg(ERXTAIL, newRXTail);

    return recvLen;
}

/**
* Sends packet
* */
void enc424j600PacketSend(uint16_t len, uint8_t* packet) {

    // Set the Window Write Pointer to the beginning of the transmit buffer
    enc424j600WriteReg(EGPWRPT, TXSTART);

    enc424j600WriteMemoryWindow(GP_WINDOW, packet, len);

    enc424j600WriteReg(ETXLEN, len);

    enc424j600MACFlush();

    //Wait until the transmission completes
    while(!enc424j600MACIsTxReady());
}

/**
* Reads MAC address of device
* */
void enc424j600ReadMacAddr(uint8_t * macAddr) {
    // Get MAC adress
    uint16_t regValue;
    regValue = enc424j600ReadReg(MAADR1);
    *macAddr++ = ((uint8_t*) & regValue)[0];
    *macAddr++ = ((uint8_t*) & regValue)[1];
    regValue = enc424j600ReadReg(MAADR2);
    *macAddr++ = ((uint8_t*) & regValue)[0];
    *macAddr++ = ((uint8_t*) & regValue)[1];
    regValue = enc424j600ReadReg(MAADR3);
    *macAddr++ = ((uint8_t*) & regValue)[0];
    *macAddr++ = ((uint8_t*) & regValue)[1];
}

/**
* Sets MAC address of device
* */
void enc424j600SetMacAddr(uint8_t * macAddr) {
    uint16_t regValue;
    ((uint8_t*) & regValue)[0] = *macAddr++;
    ((uint8_t*) & regValue)[1] = *macAddr++;
    enc424j600WriteReg(MAADR1, regValue);
    ((uint8_t*) & regValue)[0] = *macAddr++;
    ((uint8_t*) & regValue)[1] = *macAddr++;
    enc424j600WriteReg(MAADR2, regValue);
    ((uint8_t*) & regValue)[0] = *macAddr++;
    ((uint8_t*) & regValue)[1] = *macAddr++;
    enc424j600WriteReg(MAADR3, regValue);
}

/**
* Enables powersave mode
* */
void enc424j600PowerSaveEnable(void) {
    uint16_t state;

    //Turn off modular exponentiation and AES engine
    enc424j600BFCReg(EIR, EIR_CRYPTEN);
    //Turn off packet reception
    enc424j600BFCReg(ECON1, ECON1_RXEN);
    //Wait for any in-progress receptions to complete
    while (enc424j600ReadReg(ESTAT) & ESTAT_RXBUSY) {
        _delay_us(100);
    }
    //Wait for any current transmisions to complete
    while (enc424j600ReadReg(ECON1) & ECON1_TXRTS) {
        _delay_us(100);
    }
    //Power-down PHY
    state = enc424j600ReadPHYReg(PHCON1);
    enc424j600WritePHYReg(PHCON1, state | PHCON1_PSLEEP);
    //Power-down eth interface
    enc424j600BFCReg(ECON2, ECON2_ETHEN);
    enc424j600BFCReg(ECON2, ECON2_STRCH);
}

/**
* Disables powersave mode
* */
void enc424j600PowerSaveDisable(void) {
    uint16_t state;
    //Wake-up eth interface
    enc424j600BFSReg(ECON2, ECON2_ETHEN);
    enc424j600BFSReg(ECON2, ECON2_STRCH);
    //Wake-up PHY
    state = enc424j600ReadPHYReg(PHCON1);
    enc424j600WritePHYReg(PHCON1, state & ~PHCON1_PSLEEP);
    //Turn on packet reception
    enc424j600BFSReg(ECON1, ECON1_RXEN);
}

/**
* Is link connected?
* @return <bool>
*/
bool enc424j600IsLinked(void) {
    return (enc424j600ReadReg(ESTAT) & ESTAT_PHYLNK) != 0u;
}

/**
* Saves data to userspace defined by USSTART & USEND
* @return bool (true if saved, FALSE if there is no space)
* */
bool enc424j600SaveToUserSpace(uint16_t position, uint8_t* data, uint16_t len) {
    if ((USSTART + position + len) > USEND) return FALSE;
    enc424j600WriteReg(EUDAWRPT, USSTART + position);
    enc424j600WriteMemoryWindow(UDA_WINDOW, data, len);
    return TRUE;
}

/**
* Loads data from userspace defined by USSTART & USEND
* @return bool (true if area is in userspace, FALSE if asked area is out of userspace)
* */
bool enc424j600ReadFromUserSpace(uint16_t position, uint8_t* data, uint16_t len) {
    if ((USSTART + position + len) > USEND) return FALSE;
    enc424j600WriteReg(EUDARDPT, USSTART + position);
    enc424j600ReadMemoryWindow(UDA_WINDOW, data, len);
    return TRUE;
}

/********************************************************************
* UTILS
* ******************************************************************/

static void enc424j600SendSystemReset(void) {
    uint16_t readData;
    uint16_t writeData = 0xAAAA;

    // Perform a reset via the SPI/PSP interface
    do {
        // Set and clear a few bits that clears themselves upon reset.
        // If EUDAST cannot be written to and your code gets stuck in this
        // loop, you have a hardware problem of some sort (SPI or PMP not
        // initialized correctly, I/O pins aren't connected or are
        // shorted to something, power isn't available, etc.)
        do {
            enc424j600WriteReg(EUDAST, 0x1234);
            _delay_us(100);
        } while (enc424j600ReadReg(EUDAST) != 0x1234);
        // Issue a reset and wait for it to complete
        enc424j600BFSReg(ECON2, ECON2_ETHRST);

        currentBank = 0;
        while ((enc424j600ReadReg(ESTAT) & (ESTAT_CLKRDY | ESTAT_RSTDONE | ESTAT_PHYRDY)) != (ESTAT_CLKRDY | ESTAT_RSTDONE | ESTAT_PHYRDY));
        _delay_us(300);
        // Check to see if the reset operation was successful by
        // checking if EUDAST went back to its reset default.  This test
        // should always pass, but certain special conditions might make
        // this test fail, such as a PSP pin shorted to logic high.
    } while (enc424j600ReadReg(EUDAST) != 0x0000u);

    EncSPI_WriteRegister16(WCR | (MAADR1 & 0x1F), writeData);
    readData = EncSPI_ReadRegister16(RCR | (MAADR1 & 0x1F));
    // Really ensure reset is done and give some time for power to be stable
    _delay_us(1000);
}

/**
* Is transmission active?
* @return <bool>
*/
static bool enc424j600MACIsTxReady(void) {
    return !(enc424j600ReadReg(ECON1) & ECON1_TXRTS);
}

static void enc424j600MACFlush(void) {
    uint16_t w;

    // Check to see if the duplex status has changed.  This can
    // change if the user unplugs the cable and plugs it into a
    // different node.  Auto-negotiation will automatically set
    // the duplex in the PHY, but we must also update the MAC
    // inter-packet gap timing and duplex state to match.
    if (enc424j600ReadReg(EIR) & EIR_LINKIF) {
        enc424j600BFCReg(EIR, EIR_LINKIF);

        // Update MAC duplex settings to match PHY duplex setting
        w = enc424j600ReadReg(MACON2);
        if (enc424j600ReadReg(ESTAT) & ESTAT_PHYDPX) {
            // Switching to full duplex
            enc424j600WriteReg(MABBIPG, 0x15);
            w |= MACON2_FULDPX;
        } else {
            // Switching to half duplex
            enc424j600WriteReg(MABBIPG, 0x12);
            w &= ~MACON2_FULDPX;
        }
        w |= (MACON2_NOBKOFF | MACON2_BPEN);
        enc424j600WriteReg(MACON2, w);
    }

    // Start the transmission, but only if we are linked.
    if (enc424j600ReadReg(ESTAT) & ESTAT_PHYLNK)
        enc424j600BFSReg(ECON1, ECON1_TXRTS);
}

/**
* Calculates IP checksum value
*
* */
static uint16_t enc424j600ChecksumCalculation(uint16_t position, uint16_t length, uint16_t seed) {
    // Wait until module is idle
    while (enc424j600ReadReg(ECON1) & ECON1_DMAST) {
    }
    // Clear DMACPY to prevent a copy operation
    enc424j600BFCReg(ECON1, ECON1_DMACPY);
    // Clear DMANOCS to select a checksum operation
    enc424j600BFCReg(ECON1, ECON1_DMANOCS);
    // Clear DMACSSD to use the default seed of 0000h
    enc424j600BFCReg(ECON1, ECON1_DMACSSD);
    // Set EDMAST to source address
    enc424j600WriteReg(EDMAST, position);
    // Set EDMALEN to length
    enc424j600WriteReg(EDMALEN, length);
    //If we have a seed, now it's time
    if (seed) {
        enc424j600BFSReg(ECON1, ECON1_DMACSSD);
        enc424j600WriteReg(EDMACS, seed);
    }
    // Initiate operation
    enc424j600BFSReg(ECON1, ECON1_DMAST);
    // Wait until done
    while (enc424j600ReadReg(ECON1) & ECON1_DMAST) {
    }
    return enc424j600ReadReg(EDMACS);
}

/********************************************************************
* READERS AND WRITERS
* ******************************************************************/

static void enc424j600WriteMemoryWindow(uint8_t window, uint8_t *data, uint16_t length) {
    uint8_t op = WBMUDA;

    if (window & GP_WINDOW)
        op = WBMGP;
    if (window & RX_WINDOW)
        op = WBMRX;

    enc424j600WriteN(op, data, length);
}

static void enc424j600ReadMemoryWindow(uint8_t window, uint8_t *data, uint16_t length) {
    uint8_t op;

    if (length == 0u)
        return;

    op = RBMUDA;

    if (window & GP_WINDOW)
        op = RBMGP;
    if (window & RX_WINDOW)
        op = RBMRX;

    enc424j600ReadN(op, data, length);
}

/**
* Reads from address
* @variable <uint16_t> address - register address
* @return <uint16_t> data - data in register
*/
static uint16_t enc424j600ReadReg(uint16_t address) {
    uint16_t returnValue;
    uint8_t bank;

    // See if we need to change register banks
    bank = ((uint8_t) address) & 0xE0;
    //If address is banked, we will use banked access
    if (bank <= (0x3u << 5)) {
        if (bank != currentBank) {
            if (bank == (0x0u << 5))
                EncSPI_WriteOpcode(B0SEL);
            else if (bank == (0x1u << 5))
                EncSPI_WriteOpcode(B1SEL);
            else if (bank == (0x2u << 5))
                EncSPI_WriteOpcode(B2SEL);
            else if (bank == (0x3u << 5))
                EncSPI_WriteOpcode(B3SEL);

            currentBank = bank;
        }
        returnValue = EncSPI_ReadRegister16(RCR | (address & 0x1F));
    } else {
        returnValue = EncSPI_ReadRegisterUnbanked(address);
    }

    return returnValue;
}

/**
* Writes to register
* @variable <uint16_t> address - register address
* @variable <uint16_t> data - data to register
*/
static void enc424j600WriteReg(uint16_t address, uint16_t data) {
    uint8_t bank;

    // See if we need to change register banks
    bank = ((uint8_t) address) & 0xE0;
    //If address is banked, we will use banked access
    if (bank <= (0x3u << 5)) {
        if (bank != currentBank) {
            if (bank == (0x0u << 5))
                EncSPI_WriteOpcode(B0SEL);
            else if (bank == (0x1u << 5))
                EncSPI_WriteOpcode(B1SEL);
            else if (bank == (0x2u << 5))
                EncSPI_WriteOpcode(B2SEL);
            else if (bank == (0x3u << 5))
                EncSPI_WriteOpcode(B3SEL);

            currentBank = bank;
        }
        EncSPI_WriteRegister16(WCR | (address & 0x1F), data);
    } else {
        EncSPI_WriteRegisterUnbanked(address, data);
    }

}

static uint16_t enc424j600ReadPHYReg(uint8_t address) {
    uint16_t returnValue;

    // Set the right address and start the register read operation
    enc424j600WriteReg(MIREGADR, 0x0100 | address);
    enc424j600WriteReg(MICMD, MICMD_MIIRD);

    // Loop to wait until the PHY register has been read through the MII
    // This requires 25.6us
    while (enc424j600ReadReg(MISTAT) & MISTAT_BUSY);

    // Stop reading
    enc424j600WriteReg(MICMD, 0x0000);

    // Obtain results and return
    returnValue = enc424j600ReadReg(MIRD);

    return returnValue;
}

static void enc424j600WritePHYReg(uint8_t address, uint16_t Data) {
    // Write the register address
    enc424j600WriteReg(MIREGADR, 0x0100 | address);

    // Write the data
    enc424j600WriteReg(MIWR, Data);

    // Wait until the PHY register has been written
    while (enc424j600ReadReg(MISTAT) & MISTAT_BUSY);
}

static void enc424j600ReadN(uint8_t op, uint8_t* data, uint16_t dataLen) {
    EncSPI_ReadRegisterN(op, data, dataLen);
}

static void enc424j600WriteN(uint8_t op, uint8_t* data, uint16_t dataLen) {
    EncSPI_WriteRegisterN(op, data, dataLen);
}

static void enc424j600BFSReg(uint16_t address, uint16_t bitMask) {
    uint8_t bank;

    // See if we need to change register banks
    bank = ((uint8_t) address) & 0xE0;
    if (bank != currentBank) {
        if (bank == (0x0u << 5))
            EncSPI_WriteOpcode(B0SEL);
        else if (bank == (0x1u << 5))
            EncSPI_WriteOpcode(B1SEL);
        else if (bank == (0x2u << 5))
            EncSPI_WriteOpcode(B2SEL);
        else if (bank == (0x3u << 5))
            EncSPI_WriteOpcode(B3SEL);

        currentBank = bank;
    }

    EncSPI_WriteRegister16(BFS | (address & 0x1F), bitMask);
}

static void enc424j600BFCReg(uint16_t address, uint16_t bitMask) {
    uint8_t bank;

    // See if we need to change register banks
    bank = ((uint8_t) address) & 0xE0;
    if (bank != currentBank) {
        if (bank == (0x0u << 5))
            EncSPI_WriteOpcode(B0SEL);
        else if (bank == (0x1u << 5))
            EncSPI_WriteOpcode(B1SEL);
        else if (bank == (0x2u << 5))
            EncSPI_WriteOpcode(B2SEL);
        else if (bank == (0x3u << 5))
            EncSPI_WriteOpcode(B3SEL);

        currentBank = bank;
    }

    EncSPI_WriteRegister16(BFC | (address & 0x1F), bitMask);
}

bool enc424j600isRecvInterrupt()
{
    return (enc424j600ReadReg(EIR) & EIR_PKTIF);
}

uint16_t enc424j600ReadStatus()
{
    return enc424j600ReadReg(ESTAT);
}
