#ifndef __GENERAL_H
#define __GENERAL_H

BYTE CheckSum(const BYTE* data, WORD len);

BYTE GetRFState(BYTE slot);
BYTE GetSpecialBeaconSlot(WORD StarId);
VOID Reset(WORD counter);

#endif

