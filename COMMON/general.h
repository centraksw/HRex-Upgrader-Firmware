#ifndef __GENERAL_H
#define __GENERAL_H

typedef struct _Frequency {
  BYTE Freq2;
  BYTE Freq1;
  BYTE Freq0;
} FREQUENCY;

BOOL VerifyCheckSum(const BYTE* data, WORD len);
BYTE CheckSum(const BYTE* data, WORD len);

BYTE GetRFState(BYTE slot);
float CalculateRSSI(BYTE data);
BYTE GetBeaconSlot(WORD StarId);
BYTE GetSpecialBeaconSlot(WORD StarId);
BYTE GetTxPower(BYTE retry);
BYTE GetEquivalentTxPower(BYTE idx);
VOID Reset(WORD counter);
BOOL GetBit(WORD pos, BYTE *data);
VOID SetBit(WORD pos, BYTE *data);
BYTE GetIRSlot(BYTE BeaconCnt, BYTE Slot, BYTE IRProfile);

WORD GetUpgradeSlot(WORD StarId);
BYTE GetPagingDelay(BYTE Profile);
#endif

