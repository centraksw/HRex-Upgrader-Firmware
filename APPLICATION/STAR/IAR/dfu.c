#include <string.h>
#include "defines.h"
#include "general.h"
#include "dfu.h"
#include "flash.h"
#include "client.h"
#include "uip.h"
#include "timer_drv.h"

#define PART1_BASE_ADDR                               0xD000
#define SEG0                                          0xFE00
#define SEG_SIZE                                      0x200l

void Read_INTVEC()
{
    DWORD Addr = 0xFFE0;
    BYTE buf[32];
    Flash_Read(buf,Addr, 32);
    _DINT();
    Flash_Clear((char*)0xFC00);
    Flash_Write(buf, 0xFC00 , 32);
    _EINT();
}

void DFU_Task(void)
{
   DWORD idx;
   Read_INTVEC();
   //Dectect current part

   Flash_Erase_Part();

   Init_FirmwareUpgrade();

   for(idx=0; idx<500000; idx++)
   {
       blnInMainLoop = TRUE;
       blnWDTCheck = FALSE;

        do_uip_process();

        if ( !GetUpdateStatus() ) break;
   }

   //If not complete, then Reset
    WDTCTL = 0;
}

void Flash_Write_INTVEC(BYTE* intvec_data, WORD len)
{
    char *seg0 = (char *)SEG0;

    _DINT();

    Flash_Clear(seg0);

    Flash_Write(intvec_data, 0xFFE0, MAX_UPGRADE_SEGMENT_SIZE);

    WDTCTL = 0;
}

void Flash_Erase_Part()
{
    DWORD seg_addr;
    BYTE idx, MaxSegments;
    char *seg;
    MaxSegments = MAX_SEGMENTS;
    seg_addr = (DWORD)PART1_BASE_ADDR;
    MaxSegments = MAX_SEGMENTS - 1;

    for(idx=0; idx<MaxSegments; idx++)
    {
      if((idx != 22) && (idx != 23))
      {
        seg = (char*)seg_addr;

        //Disable All interrupts
        _DINT();

        //Clear Lock
        FCTL3 = FWKEY;

        //Enable segment erase
        FCTL1 = FWKEY + ERASE;

        while(FCTL3 & BUSY);

        //Perform Erase
        *seg = 0;
        while(FCTL3 & BUSY);

        //Disable Erase
        FCTL1 = FWKEY;

        //set LOCK
        FCTL3 = FWKEY + LOCK;

        while(FCTL3 & BUSY);

        //Enable Interrupts
        _EINT();

        seg_addr += SEG_SIZE;
      }
      else
        seg_addr += SEG_SIZE;
    }
}

void Flash_Write_Segment(BYTE seg_no, BYTE* seg_data, WORD len)
{
    DWORD seg_addr;

    seg_addr = ((DWORD)PART1_BASE_ADDR + (DWORD)(seg_no*SEG_SIZE));

    // Segment no 22 and 23 are Temp interrupt vector and main Interrupt vector segments.
    // We should not write the actual firmware in these segments.
    if(seg_no > 21)
        seg_addr += (2 * SEG_SIZE);

    //Disable All interrupts
    _DINT();

    Flash_Write(seg_data, seg_addr, 512);

    //Enable Interrupts
    _EINT();
}
