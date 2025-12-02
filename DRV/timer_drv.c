#include "timer_drv.h"
#include <stdlib.h>
#include "general.h"

extern BYTE curSlot;
extern BOOL blnInMainLoop;
extern BOOL blnWDTCheck;

VOID Micro_Init()
{
    // Stop watchdog timer
    WDTCTL = WDTPW + WDTHOLD;
}

VOID Timer_Init()
{      
    while(BAKCTL & LOCKBAK)                   // Unlock XT1 pins for operation
    BAKCTL &= ~(LOCKBAK);                
    UCSCTL0 = 0x00;                           // Set lowest possible DCOx, MODx
    UCSCTL1 = DCORSEL_5;                    // Select suitable range
    
    UCSCTL2 = 121 + FLLD_1;         // Set DCO Multiplier
    UCSCTL3 = SELREF_0;
    
    do
    {
       UCSCTL7 &= ~(DCOFFG + XT1LFOFFG + XT1HFOFFG + XT2OFFG);
       SFRIFG1 &= ~OFIFG;
    }while (SFRIFG1&OFIFG); 
    
    UCSCTL4 = SELA__XT1CLK + SELS__DCOCLK + SELM__DCOCLK;
    
    TA1CTL = TASSEL_2 + MC_2 + TAIE + ID1 + ID0;

}

// Each tick is 30.51 us
// Micro will be in LPM1 mode.
// LPM3 micro wakeup execuited when the chip is in LPM1.
// Min Delay: 30.51 us 
// Max Delay: 1.999472.85 secs
VOID CCR_Delay(WORD time)
{
    if( time <= 0 ) return;
    
    TA1CCR0 = TA1R + time;
    TA1CCTL0 |= CCIE;                          // CCR0 interrupt enabled
    Micro_Sleep_LPM1();
    _NOP();
    //Micro_Sleep();
    
    TA1CCTL0 = 0; 
}

// This is a service function, which calls the CCR_Delay
// We should give delay value in micro seconds.
// this function divides the delay value  by 30.52 and send it to CCR_Delay
VOID DELAY_US(DWORD delay)
{
    delay &= 0xFFFFF;
    if( delay <= 0 ) return;
        
    delay *= 100;
    CCR_Delay(delay / 3052);
}


// This is a service function, which calls the CCR_Delay
// We should give delay value in milli seconds.
VOID DELAY_MS(WORD delay)
{
    DELAY_US(delay * 1000l);
}

// This is a common delay function.
// Micro will be active during this delay time.
// In Tag: Minimum delay: 244us  : 1 * 8 * 30.51  : 1 Tick = 30.51 us
// In Star: Minimum delay: 32us : 1 * 32 : 1 Tick = 1 us
VOID Timer_Delay(WORD time)
{
    // Timer source 1MHz - 1 Tick (1 * 32) us
    WORD nTemp = TA1R + ( time * 32 );
    if( nTemp >= 0xFFFA ) nTemp += 10;

     if(nTemp > TA1R) 
    {
        while(TA1R < nTemp);
    }
    else 
    {
         while(TA1R > 10 && TA1R < 0xFFFE); 
        while( (TA1R == 0xFFFE) || (TA1R == 0xFFFF) || (TA1R == 0x0) || (TA1R < nTemp) );
    }
}

// Initialize WDT
// WDT will be triggered once in every 250ms.
VOID Init_SlotTimer()
{
    WDTCTL = WDT_ADLY_250;
    TBR = 0;
    TBCTL = TBSSEL_1 + MC_2;                    // ACLK, up mode


    SFRIE1 |= WDTIE; 

 
    _EINT();
}

// Stop WDT 
VOID Stop_SlotTimer()
{
    TBCTL = 0;
    SFRIE1 &= (~WDTIE);
    WDTCTL = WDTPW + WDTHOLD; 
}

// Initialize the slot offset counter. 
// which is used to measure how much time elapsed after last WDT trigger.
VOID InitSlotOffset()
{
    TBR = 0;
}

DWORD GetSlotOffsetInUS()
{
    DWORD time;
    time = TBR * 3051l;
    time /= 100;
    return time; 
}


BYTE GetSlotOffsetInMilliSecs()
{
    DWORD offset;
    offset = GetSlotOffsetInUS();
    offset /= 1000;
    return offset;
}

// This will put the micro to LPM3 mode sleep for the specified amount of time.
// Sleep time should be in multiples of 250 ms.
VOID Sleep_SlotTimer(WORD time)
{
    WORD idx;
    if( time == 0 ) return;
    Init_SlotTimer();
    for( idx=1; idx<=time; idx++ )
    {
        Micro_Sleep();
    }
    Stop_SlotTimer();
}

VOID TM_Init50MS()
{
    TA0CCTL0 = CCIE;
    TA0CTL = TACLR + TASSEL_1 + MC_2;

    TA0CCR0 = 1638;
}

// Watchdog Timer interrupt service routine
#pragma vector = WDT_VECTOR
__interrupt void watchdog_timer(void)
{
    DBG_TOGGLE_TIMER_PIN();

     //Init 50 Ms clock
    TM_Init50MS();
    
    //We should not wakeup micro if it is in LPM1 mode sleep.
    Micro_Wakeup();

    // Increment slot counter and check it exits max slots.
    // If it exits max slot, then reset curslot to initial value : 1
    ++curSlot;
    if( curSlot > MAX_SLOTS )
    {
        curSlot = 1; 
        if( blnInMainLoop && blnWDTCheck )
            WDTCTL = 0;
        blnWDTCheck = TRUE;
    }
    TBR = 0;
}
// Timer A0 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
{   
    Micro_Wakeup();
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
   TA0CCR0 += 1638;
}

