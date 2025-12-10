#ifndef _CLIENT_H
#define _CLIENT_H

#include "defines.h"
#define UIP_UDP_APPCALL                                 udp_appcall

#define BROADCAST_SERVER_PORT                           10001
#define SERVER_PORT                                     10002
#define DEBUG_SERVER_PORT                               10004
#define TCP_UPGRADE_PORT                                10006
#define DEBUG_CLIENT_PORT                               6129
#define BROADCAST_CLIENT_PORT                           7071

#define PKT_HEADER_SIZE                                 4
#define DATA_OFFSET                                     7
#define RESERVED_OFFSET                                 10

#ifndef UIP_APPCALL
#define UIP_APPCALL         tcp_appcall
#endif

typedef struct _TCP_STATE
{
    WORD dataLength;
    WORD lastSentLen;
    WORD bytesSent;
    WORD count;
    BYTE* data;
}TCP_APP_STATE;

typedef struct _Network_Data
{
    BYTE* data;
    WORD dataLen;
}NETWORK_DATA;

#pragma pack(1)
typedef struct _BCastData
{
    BYTE StarId;
    WORD UdpPort;
    BYTE SystemId;
    BYTE MACId[6];
    BYTE ServerIP[4];
    BYTE IPMode;
    BYTE StaticIP[4];
    BYTE SubNet[4];
    BYTE Gateway[4];
    BYTE Frequency;
    BYTE InitialDelay;
    BYTE RandomPort;
    BYTE SaveSettings;

    BYTE Reserved[33];
}BCAST_DATA;

#pragma pack(4)
typedef struct _FIRMWARE_UPGRADE
{
    BYTE UpgradeDataBuf[UPGRADE_BUF_SIZE];
    BYTE segment_index;
    BYTE total_segment;
    BYTE FirmwarePart;
    BYTE Reserved;
    BYTE upgradeCycleCount;
    BYTE deviceType;
    BOOL blnUpgradeRequestSent;
    BOOL blnUpgradeStarted;
    WORD segIndex32byte;
    WORD TotSeg32b;
    WORD SegNo32b;
    WORD bufIndex;
    BYTE bln5xUpgrade;
}FIRMWARE_UPGRADE;

//Disable Warnings
#pragma diag_suppress = Pe068
#pragma diag_suppress = Pe061

extern struct uip_eth_addr uip_ethaddr;
extern BCAST_DATA bcast_data;
extern BOOL blnResetFlag;
extern BYTE curSlot;

void udp_appcall(void);
void tcp_appcall(void);
BOOL  Init_Connection(BYTE* data, WORD len);
BOOL PrepareLocationDataPacket(WORD StarId, NETWORK_DATA* nd);
VOID HandleStarAndBroadcastCommand(WORD cmd);
void Init_Server_Discovery(BYTE* IP);
void Init_Debug(BYTE* IP, BYTE dbg_code, BYTE dbg_data, WORD StarId);
void Close_UDP_Connection();
BOOL CheckIsUDP_ProcessCompleted();
BOOL GetUpdateStatus();
BOOL Init_FirmwareUpgrade(void);

#endif
