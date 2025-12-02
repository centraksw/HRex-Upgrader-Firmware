#ifndef _CLIENT_H
#define _CLIENT_H

#include "defines.h"

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

#if 0
typedef struct _BCastData
{
    WORD StarId;  
    WORD PagingPort;
    WORD DataPort1;
    WORD DataPort2;
   
    BYTE MACId[6];
    BYTE TimeServerIP[4];

    BYTE PagingServerIP[4];
    BYTE DataServerIP1[4];
    BYTE DataServerIP2[4];
    
    BYTE StaticIP[4];
    BYTE SubNet[4];
    BYTE Gateway[4];
    
    BYTE TxPower;
    BYTE EnableDHCP;    
    
    BYTE Frequency;
    BYTE StarType; 
    BYTE NumBeaconFails;
    INT8 BeaconRssiThreshold;
    
    BYTE BCastCnt;

    BYTE InitialDelay;
    
    BYTE SaveSettings;
    BYTE randomPort;
    BYTE FirmwarePart;
    BYTE blnWifiIn900MHz;
    BYTE Reserved[10];
}BCAST_DATA;
#endif

#pragma pack(1)
typedef struct _BCastData
{
    BYTE StarId;
    WORD UdpPort;
    BYTE SystemIdentity;
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

void tcp_appcall(void);

BOOL PreparePageResponse(WORD StarId, BYTE rssi, WORD AssociatedStarId, DWORD delay, NETWORK_DATA* nd);
BOOL  Init_Connection(BYTE* data, WORD len);

void udp_appcall(void);

#ifndef UIP_APPCALL
#define UIP_APPCALL         tcp_appcall
#endif

#define UIP_UDP_APPCALL     udp_appcall

#define BROADCAST_SERVER_PORT   10001
#define SERVER_PORT             10002
#define DEBUG_SERVER_PORT       10004
#define DEBUG_CLIENT_PORT       6129

#define TIME_SERVER_PORT        4747
#define TIME_CLIENT_PORT        4848
#define BROADCAST_CLIENT_PORT   7071

//#define PKT_HEADER_SIZE         7
//#define DATA_OFFSET             8  


#define PKT_HEADER_SIZE         4
#define DATA_OFFSET             7
#define RESERVED_OFFSET         10

extern BCAST_DATA bcast_data;

VOID HandleStarAndBroadcastCommand(WORD cmd);

void Init_Server_Discovery(BYTE* IP);
void Init_Time_Request();
void Init_Debug(BYTE* IP, BYTE dbg_code, BYTE dbg_data, WORD StarId);

void Close_UDP_Connection();
BOOL CheckIsUDP_ProcessCompleted();
DWORD GetServerTime();
BOOL GetUpdateStatus();
BOOL PrepareLocationDataPacket(WORD StarId, NETWORK_DATA* nd);
#endif
