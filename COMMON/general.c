#include "defines.h"
#include "general.h"
#include "client.h"

static BYTE currState = RF_STATE;

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

BYTE GetRFState(BYTE slot)
{
    currState = RF_STATE;
    if( (slot % PC_COM_RES_STATE)  == 1 )
      currState = PC_COM_RES_STATE;

    return currState;
}

static BYTE GetBeaconSlot(WORD StarId)
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

VOID Reset(WORD Counter)
{
    if( Counter == 0 ) WDTCTL=0;
}
