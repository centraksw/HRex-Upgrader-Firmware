#include "defines.h"
#include "general.h"
#include "client.h"

#define RSSI_DBM_OFFSET     78



BYTE CheckSum(const BYTE* data, WORD len)
{
    WORD idx;
    BYTE checksum = 0;

    for( idx=0; idx<len; idx++ )
    {
        checksum += data[idx];
    }
    return checksum;
}

BOOL VerifyCheckSum(const BYTE* data, WORD len)
{
    WORD idx;
    BYTE checksum = 0;

    len -= 1;
    for( idx=0; idx<len; idx++ )
    {
        checksum += data[idx];
    }
    return( checksum == data[len] );
}
BYTE currState = RF_STATE;
BYTE GetRFState(BYTE slot)
{
#if 0
    BYTE tmpslot = (slot % NUM_SLOTS_PER_PC_COM) + 1;

    if( tmpslot == (NUM_SLOTS_PER_PC_COM/2))
        return IR_STATE;
    else if(tmpslot == NUM_SLOTS_PER_PC_COM)
        return PC_COM_REQ_STATE;
    else if(tmpslot == 1)
        return PC_COM_RES_STATE;
#endif
    currState = RF_STATE;
    if( (slot % PC_COM_RES_STATE)  == 1 )
      currState = PC_COM_RES_STATE;

    return currState;
}

float CalculateRSSI(BYTE data)
{
    float rssi;
    if( data >= 128 )
        rssi = ( data - 256 ) / 2.0f - RSSI_DBM_OFFSET;
    else
        rssi = data / 2.0f - RSSI_DBM_OFFSET;
    return rssi;
}

BYTE GetBeaconSlot(WORD StarId)
{
    BYTE slot = StarId % 9;
    if(slot == 0) slot = 9;
    if(slot >= 5) slot +=1;
    return slot;
}

BYTE GetSpecialBeaconSlot(WORD StarId)
{
    BYTE slot = GetBeaconSlot(StarId);
    BYTE s1 = StarId / 9;

    if( slot == 10 && s1 > 0)
        s1-=1;
    slot = (slot + s1*12) % MAX_SLOTS;

    return slot;
}

BYTE GetTxPower(BYTE retry)
{
    switch(retry)
    {
    case 2: return PLUS_1_8_DBM;
    case 3: return PLUS_5_1_DBM;
    default: return MINUS_1_0_DBM;
    }
}

VOID Reset(WORD Counter)
{
    if( Counter == 0 ) WDTCTL=0;
}

BOOL GetBit(WORD pos, BYTE *data)
{
    BOOL blnRetVal = FALSE;
    
    BYTE byte = pos / 8;
    BYTE bit = pos - (byte * 8);
    
    blnRetVal = ((data[byte] & (1 << bit)) > 0);
    
    return blnRetVal;
}

VOID SetBit(WORD pos, BYTE *data)
{
    if( pos > MAX_UPGRADE_SLOTS ) return;
    
    BYTE byte = pos / 8;
    BYTE bit = pos % 8;  
       
    data[byte] |= (1 << bit);
}

BYTE GetIRSlot(BYTE BeaconCnt, BYTE Slot, BYTE IRProfile)
{
    // 1.5secs - Slots : 5, 11, 17, 23, 29, 35, 41, 47
    // 3secs   - Slots : 5, 17, 29, 41
    BYTE IRSlot;
    
    if( IRProfile == IR_PROFILE_1_5_SECS )
    {
        IRSlot = Slot / 6; 
        IRSlot += (BeaconCnt * 8);
    }
    else
    {
        IRSlot = Slot / 12;
        IRSlot += (BeaconCnt * 4);
    }

    return IRSlot;
}


WORD GetUpgradedPacketsCount(BYTE* data)
{
    WORD cnt = 0;
    BYTE bit, idx;

    bit = 0x80;
    while (bit)
    {
        for(idx=0; idx<MAX_UPGRADE_BYTE_SIZE; idx++)
        {
            if( data[idx] & bit )
                cnt++;
        }
        bit >>= 1;
    }
    return cnt;
}

BOOL CheckIsUpgradeCompleted(BYTE* data, WORD TotalSegments)
{
    WORD idx;
    BYTE bytes = TotalSegments / 8;
       
    for(idx=0; idx<bytes; idx++)
    {
        if( data[idx] != 0xFF ) 
            return FALSE;
    }
    return TRUE;
}

WORD GetUpgradeSlot(WORD StarId)
{
    return (StarId % MAX_TIME_SLOT);
}

BYTE GetPagingDelay(BYTE Profile)
{
    switch(Profile)
    {
    case 0: return PAGE_DELAY_12_5_SEC;
    case 1: return PAGE_DELAY_9_5_SEC;
    case 2: return PAGE_DELAY_6_5_SEC;
    }
    return PAGE_DELAY_9_5_SEC;
}
