#include "defines.h"
#include "client.h"
#include "uip.h"
#include "general.h"
#include "dhcpc.h"
#include "timer_drv.h"
#include "dfu.h"
#include<flash.h>
#include <string.h>
#include <stdlib.h>

TCP_APP_STATE state[2];

#define PAGE_RESPONSE_HEADER        6
#define BTN_DATA_LEN                10

#define NW_HDR_PROFILE_PULL_REQUEST       11
#define NW_HDR_PROFILE_PULL_RESPONSE      12
#define NW_HDR_LOCATION_DATA              13
#define NW_HDR_PC_COM_RESPONSE 		  14

static WORD TotalDatabufsize=0;
static WORD DevDataOffset=0;

BYTE data[UIP_TCP_APP_STATE_SIZE];
BYTE recv_data[UIP_TCP_APP_STATE_SIZE];

WORD tcpConnectionFail = 0;

struct uip_udp_conn *udp_conn;
static BOOL blnUDPSent;
static BOOL isUDPDone = FALSE;

extern struct uip_eth_addr uip_ethaddr;
static BYTE strUdpResp[12];
static BYTE udp_buf[81];

WORD receivedDataLen = 0;
WORD totalDataLen = 0;

static DWORD ServerTime=0;
DWORD g_timer=0;

static char handle_broadcast(void);
static void sendTagData();

extern BOOL blnResetFlag;

extern volatile BYTE curSlot;

static BYTE* locData = (BYTE*) (data + DATA_OFFSET);
FIRMWARE_UPGRADE firmware_upgrade;
BCAST_DATA bcast_data;
char tmp_macid[6];

BYTE segment_index, total_segment;
BOOL upgrade_status;
DWORD chksum, src_chksum, len;

static BYTE PageResSlot, PageResRecvicedSlot;
static BYTE PageResOffset, PageResReceivedOffset;

extern BYTE TimeRequestOffset;
extern BYTE TimeRequestSlot;

extern WORD AllowedOffset;
extern BYTE Retry;
extern DWORD RoundTripOffset;
extern DWORD g_ServerTime;
extern DWORD g_TotalOffset;
extern DWORD g_Delay;

static DWORD g_RespOffset;

#define NW_HDR_PAGE_RESPONSE           13
#define NW_HDR_STAR_COMMAND            7
#define NW_HDR_TAGSPIDER_UPGRADE       9


VOID InitCounters()
{
    tcpConnectionFail = 0;
}

VOID HandleStarAndBroadcastCommand(WORD cmd)
{
    BYTE Command = (cmd >> 12) & 0xF;
    
    switch( Command )
    {
    case CMD_STAR_RESET:
        blnResetFlag = TRUE;
        break;

    case CMD_STAR_FIRMWARE_UPGRADE:
        blnStarUpgradeProcess = FALSE;
        if( !firmware_upgrade.blnUpgradeStarted )
        {
            blnStarUpgradeProcess = TRUE;
            //DFU_Task();
        }
        break;
    }
}

WORD GetTCPFailCount()
{
    return tcpConnectionFail;
}


void HandlePagingResponse()
{
    BYTE Count = 0, len =0, Header  = 0;
    WORD TotalLen = 0, StarCmd = 0;
    WORD size = 0; 
    
    memcpy(&TotalLen, recv_data, 2);
    Header = recv_data[4];
    
    if(Header == NW_HDR_PC_COM_RESPONSE)
    {
        memcpy(&size, recv_data+2, 2);
      	memcpy(&StarCmd, recv_data+5, 2);

        if(StarCmd > 0 )
          HandleStarAndBroadcastCommand(StarCmd);
    }        
        
}

BOOL GetUpdateStatus()
{
    return upgrade_status;
}

BOOL Init_FirmwareUpgrade(void)
{
    struct uip_conn *conn;
    static u16_t addr[2];
    BYTE* ip = bcast_data.ServerIP;

    upgrade_status = TRUE;

    firmware_upgrade.segment_index = 0;

    uip_ipaddr(addr, ip[0], ip[1], ip[2], ip[3]);
    conn = uip_connect(&addr, htons(10006));
    if( conn == NULL )
    {
        upgrade_status = FALSE;
        return FALSE;
    }

    conn->appstate = NULL;

    return TRUE;
}

void send_firmware_request()
{
    data[0] = segment_index;

    data[1] = 1;

    memcpy(data+2,&bcast_data.StarId,2);

    data[4] = VERSION;

    uip_send(data , 5);
}

void tcp_appcall_firmware_upgrade()
{
    WORD idx;
    if(uip_connected())
    {
        send_firmware_request();
        return;
    }

    if( uip_rexmit() )
    {
        send_firmware_request();
        return;
    }

    if( uip_newdata() )
    {
        len = ((WORD*)uip_appdata)[0];
        if( uip_datalen() >= len )
        {
            memcpy(data, uip_appdata, uip_datalen());
            total_segment = data[3];

            if(total_segment == 0 || total_segment > MAX_SEGMENTS)
            {
               upgrade_status = FALSE;
               return;
            }

            if( data[2] == segment_index )
            {
                chksum = 0;
                for(idx=0; idx<512; idx++)
                {
                    chksum += data[idx+4];
                }

                memcpy(&src_chksum, data + (len-4), 4);
                if( chksum == src_chksum)
                {
                    //Check is last segment
                    if(segment_index == (total_segment-1) )
                      Flash_Write_INTVEC(&data[4], 512);
                    else
                      Flash_Write_Segment(segment_index, &data[4], 512);

                    ++segment_index;
                }
            }
            if( segment_index <  total_segment)
                send_firmware_request();
            return;
        }
    }

    if( uip_aborted() || uip_timedout() )
    {
        upgrade_status = FALSE;
    }
}

void tcp_appcall(void)
{
    TCP_APP_STATE* tcp_state = (TCP_APP_STATE*) uip_conn->appstate;

    if( tcp_state == NULL )
    {
        tcp_appcall_firmware_upgrade();
        return;
    }

    if(uip_connected())
    {
        tcpConnectionFail = 0;
        sendTagData();
        return;
    }

    if( uip_poll() )
    {
        if( ++tcp_state->count >= 50 )
        {
            uip_close();
            return;
        }
    }

    if(uip_newdata())
    {
        if( uip_datalen() > 0 )
        {
            if( totalDataLen == 0 )
            {
                totalDataLen = ((WORD*)uip_appdata)[0];
            }
            memcpy(recv_data + receivedDataLen, uip_appdata, uip_datalen());
            receivedDataLen += uip_datalen();

            if( receivedDataLen >= totalDataLen )
            {
                HandlePagingResponse();
            }
        }
        return;
    }

    if( uip_acked() )
    {
        tcp_state->bytesSent += tcp_state->lastSentLen;
        sendTagData();
        return;
    }

    if ( uip_rexmit() )
    {
        sendTagData();
        return;
    }
    
    if(uip_closed())
    {
        return;
    }

    if( uip_aborted() || uip_timedout() )
    {
        ++tcpConnectionFail;
    }
}

//LTC Location Data packet
BOOL PrepareLocationDataPacket(WORD StarId, NETWORK_DATA* nd)
{
    WORD pktLen, offset;
    BYTE header;

    pktLen = DevDataOffset + PKT_HEADER_SIZE + RESERVED_OFFSET;

    nd->data = (BYTE*) &locData[0];
    nd->data -= DATA_OFFSET;

    /*Clear tcp Received Info*/
    totalDataLen = 0;
    receivedDataLen = 0;

    offset = 0;
    memcpy(nd->data+offset, &pktLen, 2);
    offset = 2;

    header = NW_HDR_LOCATION_DATA;
    nd->data[offset++] = header;

    nd->data[offset++] = 1;
    
    nd->data[offset++] = 1;
    
    nd->data[offset++] = VERSION & 0x7F;

    nd->data[offset++] = 0;

    offset += DevDataOffset;

    offset += 10; //Reserved

    nd->data[offset] = CheckSum(nd->data+2, offset-2);
    offset++;

    nd->dataLen = offset;

    TotalDatabufsize -= DevDataOffset;
    DevDataOffset = 0;    

    return TRUE;
}

BOOL PreparePageResponse(WORD StarId, BYTE rssi, WORD AssociatedStarId, DWORD delay, NETWORK_DATA* nd)
{
    WORD offset = 0, pktSize;
    WORD totalLen = 67;
    u16_t addr[2];
    BYTE type,header;
    BYTE* buf = data + RESPONSE_REQUEST_OFFSET;
    nd->data = data + RESPONSE_REQUEST_OFFSET;

    /*Clear Page Response Received Info*/
    totalDataLen = 0;
    receivedDataLen = 0;

    offset = 2;
    header = NW_HDR_PAGE_RESPONSE;
    header |= (0x1) << 6;           //To identify the 3.0 Star in PCServer.
    header |= (0x1) << 7;           //To Identify 2.2 PCServer in PCServer
    buf[offset++] = header;
    memcpy(buf+offset, &totalLen, 2);
    offset += 2;
    memcpy(buf+offset, &StarId, 2);
    offset += 2;
    buf[offset++] = rssi;

    memcpy(buf+offset, &AssociatedStarId, 2);
    offset += 2;
    
    uip_gethostaddr(addr);
    memcpy(buf+offset, addr, 4);
    offset+=4;
    memcpy(buf+offset, &delay, 4);
    offset+=4;
    memcpy(buf+offset, &bcast_data.MACId, 6);
    offset+=6;
    type = 6;
    type |= (0x1) << 6;              // To identify the 3.0 star in PCServer
    type |= (0x1) << 7;               // To identify the 2.2 Star in PCServer.
    buf[offset++] = type;
    buf[offset++] = VERSION;
    buf[offset++] = ((firmware_upgrade.blnUpgradeRequestSent & 0x1) << 7) | (REGULAR_STAR & 0xF);
   
    memcpy(buf+offset, &g_timer, 4);
    offset += 4;


    buf[31] = curSlot;
        
    memcpy(buf+32, &g_ServerTime, 4);
    
    buf[36] = Retry;
    
    memcpy(buf+37, &AllowedOffset, 2);
    
    memcpy(buf+39, &g_TotalOffset, 4);
    
    memcpy(buf+43, &RoundTripOffset, 4);
    
    memcpy(buf+47, &g_Delay, 4);
    
    buf[51] = 0;
    buf[52] = 0;
    
    buf[53] = 0;
    buf[54] = 0;
    
    buf[55] = PageResSlot;
    buf[56] = PageResOffset;
    
    buf[57] = PageResRecvicedSlot;
    buf[58] = PageResReceivedOffset;
    
    buf[59] = TimeRequestSlot;
    buf[60] = TimeRequestOffset;
    
    buf[61] = 0;
    buf[62] = 0;
    buf[63] = 0;
    
    memcpy(buf+64, &g_RespOffset, 4);
    offset = 68;

    
    buf[offset] = CheckSum((BYTE*) (buf+2), offset-2);
    offset++;
        
    if( firmware_upgrade.blnUpgradeRequestSent )
    {   
        buf[offset++] = NW_HDR_TAGSPIDER_UPGRADE;
        pktSize = 5 + 3;
        memcpy(buf+offset, &pktSize, 2);
        offset += 2;
        memcpy(buf+offset,&StarId,2);
        offset +=2;
        
        if(firmware_upgrade.segment_index == firmware_upgrade.total_segment) 
            firmware_upgrade.segment_index = 0; 

        buf[offset++] = firmware_upgrade.segment_index;
        buf[offset++] = firmware_upgrade.FirmwarePart | ((firmware_upgrade.deviceType & 0xF) << 1);
        
        buf[offset++] = CheckSum((BYTE*) (buf + totalLen + 2), pktSize-1);        
        totalLen += pktSize;
    }
    
    memcpy(buf, &totalLen, 2);
    
    nd->dataLen = offset;
    
    PageResSlot = 0;
    PageResOffset =0;
    
    PageResRecvicedSlot = 0;
    PageResReceivedOffset = 0;
    
    TimeRequestSlot = 0;
    TimeRequestOffset = 0;
    
    return TRUE;
}

BOOL Init_Connection(BYTE* data, WORD len)
{
    struct uip_conn *conn;
    static u16_t addr[2];
    BYTE* IPAddr = NULL;
    WORD Port = 0;

    if( len == 0) return FALSE;

    state[0].dataLength = len;
    state[0].lastSentLen = 0;
    state[0].bytesSent = 0;
    state[0].count = 0;
    state[0].data = data;

    PageResSlot = curSlot;
    PageResOffset = GetSlotOffsetInMilliSecs();
    g_RespOffset = GetSlotOffsetInUS();
    IPAddr = bcast_data.ServerIP;
    Port = bcast_data.UdpPort;
    
    uip_ipaddr(addr, IPAddr[0], IPAddr[1], IPAddr[2], IPAddr[3]);
    conn = uip_connect(&addr, htons(Port));
    if( conn == NULL )
    {
        tcpConnectionFail += 1;
        return FALSE;
    }

    conn->appstate = (u8_t*) &state[0];

    return TRUE;
}

static void sendTagData()
{
    TCP_APP_STATE* tcp_state;
    WORD avail_uip_len;
    avail_uip_len = uip_mss();

    tcp_state = (TCP_APP_STATE*) uip_conn->appstate;

    tcp_state->lastSentLen = tcp_state->dataLength-tcp_state->bytesSent;
    tcp_state->lastSentLen = tcp_state->lastSentLen > avail_uip_len ? avail_uip_len : tcp_state->lastSentLen;
    if( tcp_state->lastSentLen <= 0 )
        return;
    uip_send(tcp_state->data + tcp_state->bytesSent, tcp_state->lastSentLen);
}


BYTE PrepareDebugInfo()
{
    BYTE len, offset;

    offset = 4;
    memcpy(udp_buf + offset, &uip_ethaddr, 6);
    offset += 6;
    switch(udp_buf[0])
    {
    case DBG_PROFILE:
        len = sizeof(bcast_data);
        memcpy(udp_buf+offset, &bcast_data, len);
        offset += len;
        break;
    }
    return offset;
}

static char handle_broadcast(void)
{
    BYTE len;
    BYTE state = uip_udp_conn->state;
    char* data;

    if( uip_poll() )
    {
        data = (char *)uip_appdata;
        switch(state)
        {
        case UDP_TIME_REQUEST:
            if( !blnUDPSent )
            {
                strcpy(data, "TIME_SYNC_REQUEST_V3");
                uip_send(uip_appdata, 20);
                blnUDPSent = TRUE;
            }
            break;

        case UDP_DISCOVERY:
            if( !blnUDPSent )
            {
                strcpy(data, "LTC_SERVER_DISCOVERY");
                memcpy(data+20, &uip_ethaddr, 6);
                data[26] = VERSION;
                uip_send(uip_appdata, 27);
                blnUDPSent = TRUE;
            }
            break;

        case UDP_DEBUG:
            if( !blnUDPSent )
            {
                len = PrepareDebugInfo();
                memcpy(data, udp_buf, len);
                uip_send(uip_appdata, len);
                blnUDPSent = TRUE;
            }
            break;
        }
    }

    if ( uip_newdata() )
    {
        switch(state)
        {
        case UDP_TIME_REQUEST:
            memcpy(&ServerTime,  (char*)uip_appdata, 4);
            memcpy(&g_timer,  (char*)uip_appdata+4, 4);
            isUDPDone = TRUE;
            Close_UDP_Connection();
            break;

        case UDP_DISCOVERY:
            memcpy(&tmp_macid, (char*)uip_appdata+4, 6);
            if ( memcmp((char*)&tmp_macid, (char*)&uip_ethaddr, 6) == 0 )
            {
                memcpy(&bcast_data, (char*)uip_appdata, sizeof(bcast_data));

                isUDPDone = TRUE;
                tcpConnectionFail = 0;
                Close_UDP_Connection();
            }
            break;

        case UDP_DEBUG:
            memcpy(&strUdpResp,  (char*)uip_appdata, 7);
            if ( memcmp(strUdpResp, (char*)&uip_ethaddr, 6) == 0 )
            {
                //if ( strUdpResp[6] > 0 )
                    //HandleStarAndBroadcastCommand(strUdpResp[6]);

                isUDPDone = TRUE;
                Close_UDP_Connection();
            }
            break;
        }
    }
    return 0;
}

void Init_Server_Discovery(BYTE* IP)
{
    uip_ipaddr_t addr;

    blnUDPSent = FALSE;

    isUDPDone = FALSE;
    if( IP == NULL )
        uip_ipaddr(addr, 255, 255, 255, 255);
    else
        uip_ipaddr(addr, IP[0], IP[1], IP[2], IP[3]);

    //If the old udp connection object is not properly closed, then close it here
    Close_UDP_Connection();

    udp_conn = uip_udp_new(&addr, HTONS(BROADCAST_SERVER_PORT));
    if( udp_conn == NULL ) return;
    uip_udp_bind(udp_conn, HTONS(BROADCAST_CLIENT_PORT));
    udp_conn->state = UDP_DISCOVERY;
}
#if 0
void Init_Time_Request()
{
    uip_ipaddr_t addr;
    uip_ipaddr(addr, bcast_data.TimeServerIP[0], bcast_data.TimeServerIP[1], bcast_data.TimeServerIP[2], bcast_data.TimeServerIP[3]);

    g_timer = 0;
    isUDPDone = FALSE;
    blnUDPSent = FALSE;

    //If the old udp connection object is not properly closed, then close it here
    Close_UDP_Connection();

    udp_conn = uip_udp_new(&addr, HTONS(TIME_SERVER_PORT));
    if( udp_conn == NULL ) return;
    uip_udp_bind(udp_conn, HTONS(TIME_CLIENT_PORT));
    udp_conn->state = UDP_TIME_REQUEST;
}
#endif
BOOL CheckIsUDP_ProcessCompleted()
{
    return isUDPDone;
}

DWORD GetServerTime()
{
    return ServerTime; 
}

void Init_Debug(BYTE* IP, BYTE dbg_code, BYTE dbg_data, WORD StarId)
{
    uip_ipaddr_t addr;

    if( dbg_code == 0 ) return;

    blnUDPSent = FALSE;

    udp_buf[0] = dbg_code;
    udp_buf[1] = dbg_data;
    memcpy(udp_buf+2, &StarId, 2);
    
    if( IP == NULL )
        uip_ipaddr(addr, 255, 255, 255, 255);
    else
        uip_ipaddr(addr, IP[0], IP[1], IP[2], IP[3]);

    isUDPDone = FALSE;

    //If the old udp connection object is not properly closed, then close it here
    Close_UDP_Connection();

    udp_conn = uip_udp_new(&addr, HTONS(DEBUG_SERVER_PORT));
    if( udp_conn == NULL ) return;
    uip_udp_bind(udp_conn, HTONS(DEBUG_CLIENT_PORT));
    udp_conn->state = UDP_DEBUG;
}

void Close_UDP_Connection()
{
    if( udp_conn )
    {
        uip_udp_remove(udp_conn);
        udp_conn = NULL;
    }
}

void udp_appcall(void)
{
    if( uip_udp_conn->state == UDP_DHCP )
        handle_dhcp();
    else
        handle_broadcast();
}
