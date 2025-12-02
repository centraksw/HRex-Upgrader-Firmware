#include "defines.h"
#include <string.h>
#include "flash.h"
#include "timer_drv.h"

#define SEGB            0x1800
#define SEGA            0x1880
#define SEGMENT_SIZE    0x80    


#define OFFLINE_TEMP_SEGB            0xFA00
#define OFFLINE_TEMP_SEGA            0xF800

/****************************************
**
** Flash_Read
**
*****************************************/
void Flash_Read(void *data, DWORD Addr, BYTE nBytes)
{
    memcpy((char *)data, (char *)Addr, nBytes);
    
}

/****************************************
**
** Clear the Segment(128 bytes)
**
*****************************************/
void Flash_Clear(char *Addr)
{
    FCTL3 = FWKEY;
    FCTL1 = FWKEY + ERASE;
    
    *Addr = 0;
     while(FCTL3 & BUSY);

    FCTL1 = FWKEY;
    FCTL3 = FWKEY + LOCK;
}

/****************************************
**
** Write to the Particular Address
**
*****************************************/
void Flash_Rewrite(void *data, DWORD Addr, BYTE nBytes)
{
    char* AddrB = (char*) SEGB;
    char* AddrA = (char*) SEGA;
    WORD offset = 0;
    
    Flash_Clear(AddrA);

    FCTL3 = FWKEY;		// Unlock
    FCTL1 = FWKEY + WRT;	// Writing
    
    offset = (Addr - SEGB);
    
    memcpy((char *)AddrA, (char *)AddrB, offset);
    memcpy((char *)AddrA + offset, (char *)data, nBytes);
    offset += nBytes;
    memcpy((char *)AddrA + offset, (char *)AddrB + offset, SEGMENT_SIZE - offset );

    FCTL1 = FWKEY;    		// Stop Writing
    
    Flash_Clear(AddrB);    
    
    FCTL3 = FWKEY;		// Unlock
    FCTL1 = FWKEY + WRT;	// Writing
    
    memcpy((char *)AddrB, (char *)AddrA, 0x80);
    
    FCTL1 = FWKEY;    		// Stop Writing
    FCTL3 = FWKEY + LOCK;	// Lock , Not ready for Next Word Write
}

/****************************************
**
** Write to the Particular Address
**
*****************************************/
void Flash_Write(void *data, DWORD Addr, WORD nBytes)
{
    FCTL3 = FWKEY;		// Unlock
    FCTL1 = FWKEY + WRT;	// Writing

    memcpy((char *)Addr, (char *)data, nBytes);
    while(FCTL3 & BUSY);
    
    FCTL1 = FWKEY;    		// Stop Writing
    FCTL3 = FWKEY + LOCK;	// Lock , Not ready for Next Word Write
}