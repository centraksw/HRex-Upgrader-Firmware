#ifndef __TIMER_DRV_H
#define __TIMER_DRV_H

#include "defines.h"

#define Micro_Sleep()               _BIS_SR(LPM3_bits + GIE)
#define Micro_Wakeup()              _BIC_SR_IRQ(LPM3_bits)

    #define Micro_Sleep_LPM1()          _BIS_SR(LPM1_bits + GIE)
    #define Micro_Wakeup_LPM1()         _BIC_SR_IRQ(LPM1_bits)


VOID Micro_Init();

VOID Timer_Init();

VOID Timer_Delay(WORD time);

VOID CCR_Delay(WORD time);

VOID Init_SlotTimer();

VOID Stop_SlotTimer();

VOID Sleep_SlotTimer(WORD time);

VOID DELAY_US(DWORD delay);

VOID DELAY_MS(WORD delay);

VOID InitSlotOffset();

DWORD GetSlotOffsetInUS();

BYTE GetSlotOffsetInMilliSecs();

#endif // __TIMER_DRV_H
