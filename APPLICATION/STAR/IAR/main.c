#undef HTONS

#include "main.h"
#include "timer_drv.h"
#include "enc424j600.h"
#include "network.h"
#include "uip.h"
#include "uip_arp.h"
#include "client.h"
#include "flash.h"
#include "general.h"
#include <string.h>
#include "SPI.h"
#include "dfu.h"

#define ADDR_STAR_MAC                                   0x1800
#define ADDR_STAR_SETTINGS                              0x1820
#define TCP_TIMER                                       3000
#define BUF                                             ((struct uip_eth_hdr *)&uip_buf[0])
#define RT_CLOCK_SECOND                                 3

static BOOL blnPageResSent;
static BCAST_DATA star_settings;
static WORD dhcpCounter;
static BYTE MacAddr[6];
static WORD OverflowCnt;
BOOL blnResetFlag;
static BYTE SendBeaconSlot;

BOOL blnInMainLoop;
BOOL blnWDTCheck;
BYTE curSlot;

static NETWORK_DATA nd;
static WORD arptimer;
static WORD start;
static WORD current;
static unsigned short ticks;

BOOL blnStarUpgradeProcess;

static BOOL Call_UDPProcess(WORD cnt);

WORD rt_ticks(void)
{
    return ticks;
}

WORD Timer_Ticks(void)
{
    return ticks;
}

int __low_level_init(void)
{
    /* Insert your low-level initializations here */

    /*==================================*/
    /* Choose if segment initialization */
    /* should be done or not.		*/
    /* Return: 0 to omit seg_init	*/
    /*	       1 to run seg_init	*/
    /*==================================*/
    WDTCTL = WDTPW + WDTHOLD;
    return (1);
}

VOID DBG_Send(BYTE* IP, BYTE dbg_code, BYTE dbg_data, WORD StarId)
{
    BYTE retry;
    BOOL blnRes;

    for(retry=0; retry<2; retry++)
    {
        //Initialize UDP Connection to send debug thro Unicast
        Init_Debug(IP, dbg_code, dbg_data, StarId);

        //Run the uip_process to send debug info
        blnRes = Call_UDPProcess(200);

        //Close the Debug Info UDP Connection
        Close_UDP_Connection();

        //if sent successfully, then exit the loop
        if( blnRes )
            break;
    }
}

VOID SendDebugInfo(BYTE dbg_code, BYTE dbg_data, WORD StarId)
{
    if( bcast_data.ServerIP[0] != 0 )
        DBG_Send(bcast_data.ServerIP, dbg_code, dbg_data, StarId);
    else
        DBG_Send(NULL, dbg_code, dbg_data, StarId);
}

VOID ResetStar(BYTE Reason, WORD StarId)
{
    //Send debug information before reset the star
    SendDebugInfo(DBG_RESET, Reason, StarId);

    //Reset Star
    WDTCTL = 0;
}

VOID DHCP_Process()
{
    DWORD idx;
    BYTE retry = 0;
    BYTE status;

    while( 1 )
    {
        // Initialize the uIP TCP/IP stack.
        uip_init(MacAddr);

        for(idx=0; idx<2000; idx++) do_uip_process();

        //Initialize DHCP Process.
        dhcpc_init(MacAddr, 6);

        for(idx=0; idx<100000; idx++)
        {
            //Run the uip_process to send dhcp request and get response.
            do_uip_process();

            //Break if we got IP Address
            status = Get_Dhcp_Status();
            if ( status == 1)
                return;

            // DCHP Status DHCPNAK
            if( status == 6 )
                break;

            // wait 50 ms after every 1000 tries.
            if( (idx%1000) == 0 )
                Timer_Delay(1562);
        }

        SendDebugInfo(DBG_DHCP, ++retry, 0);

        // Wait 10 sec for sending dhcp discovery request once again
        Sleep_SlotTimer(40);

        if( retry > MAX_DHCP_RETRY )
            ResetStar(RESET_NO_DHCP, 0);
    }
}

void DHCP_ReInit()
{
    //Check this is the time to send DHCP reinit.
    if( bcast_data.IPMode && (dhcpCounter >= DHCP_REINIT_COUNTER) )
    {
        //if so, send DHDP reinit request.
        dhcpc_reinit();

        dhcpCounter = 0;
    }
}

VOID SetIPAddress(BCAST_DATA *settings)
{
    //Check the IPAddress type either DHCP or  Static
    if ( settings->IPMode )
    {
        // Get Local IP from DHCP server, if already not received from DHCP
        if( !Get_Dhcp_Status() )
            DHCP_Process();
    }
    else
    {
        // Set Static IP to STAR
        uip_sethostaddr(settings->StaticIP);

        //Set Subnet Address.
        uip_setnetmask(settings->SubNet);

        //Set Default Gateway.
        uip_setdraddr(settings->Gateway);
    }
}

BOOL doServerDiscovery(BYTE* IP)
{
    BYTE idx, retry;
    BOOL blnRes = FALSE;

    for(retry=0; retry<3; retry++)
    {
        //Initialize the Server Discovery UDP Request.
        Init_Server_Discovery(IP);

        for( idx=0; idx<30; idx++ )
        {
            //Break if got the Discovery response.
            blnRes = Call_UDPProcess(1000);
            if( blnRes )
                break;

            // wait 50 ms after every try.
            Timer_Delay(1562);
        }

        //close the Server Discovery UDP Request.
        Close_UDP_Connection();

        //Break if got the Discovery response.
        if( blnRes )
            break;
    }
    return blnRes;
}

VOID GetStarProfileFromServer()
{
    BYTE retry = 0;
    u16_t ipaddr[2];

    //Set local ip address to zero
    uip_ipaddr(ipaddr, 0,0,0,0);
    uip_sethostaddr(ipaddr);

    while( 1 )
    {
        //Do the Server discovery three times in broadcast
        if( doServerDiscovery(NULL) )
            break;

        //Check the settings is stored in flash or not
        if( star_settings.SaveSettings == 1)
        {
            //Set local IP Address before send the unicast server discovery.
            SetIPAddress(&star_settings);

            //Do the Server discovery three times in unicast
            if( doServerDiscovery(star_settings.ServerIP) )
                break;
        }

        //If no response,  Wait 10 sec for sending Discovery request once again
        Sleep_SlotTimer(40);

        retry += 1;

        //Send debug information to Server
        SendDebugInfo(DBG_DISCOVERY, retry, 0);

        if( retry >= MAX_DISCOVERY_RETRY )
            ResetStar(RESET_NO_DISCOVERY_RESP, 0);
    }
}

VOID DelayMoreThan64ms(DWORD delay)
{
    while( delay )
    {
        if( delay >= 2000 )
        {
            Timer_Delay(2000);
            delay -= 2000;
        }
        else
        {
            Timer_Delay(delay);
            delay = 0;
        }
    }
}

/*--------------------------------------------------------------------------------*/
VOID main()
{
    WORD idx;
    WORD StarId;
    blnInMainLoop = FALSE;
    blnWDTCheck = FALSE;
    BOOL blnResetFlags = FALSE;

    //Initialize the Debug Out pins.
    DBG_INIT_BS();
    DBG_INIT_BS1();

    // Initialize the Micro controller.
    Micro_Init();

    // Initialize the Timer.
    Timer_Init();

    //Initialize the uip timer.
    start = rt_ticks();
    arptimer = 0;

#ifdef STAR_ID
    MacAddr[0] = 0x00;
    MacAddr[1] = 0xBD;
    MacAddr[2] = 0x3B;
    MacAddr[3] = 0x33;
    MacAddr[4] = 0x05;
    MacAddr[5] = 0x00 + STAR_ID;
    Flash_Write(MacAddr, ADDR_STAR_MAC, sizeof(MacAddr) );
#endif

    //Read MacId from Flash
    Flash_Read(MacAddr, ADDR_STAR_MAC, sizeof(MacAddr) );

    halEncInit();

    SPI_Enable();

    enc424j600Init();

    TA1CCR1 = TA1R + TCP_TIMER;
    //Initialize the CCR for uip.
    TA1CCTL1 = CCIE;

    RF_RESET1_DIR |= RF_RESET1_PIN;
    RF_RESET1_OUT |= RF_RESET1_PIN;

    RF_RESET2_DIR |= RF_RESET2_PIN;
    RF_RESET2_OUT |= RF_RESET2_PIN;

    //Initialize uip arp.
    uip_arp_init();

    enc424j600SetMacAddr(MacAddr);

    // Initialize the uIP TCP/IP stack.
    uip_init(MacAddr);

    // Get Star Profile from PCServer
    Flash_Read(&star_settings, ADDR_STAR_SETTINGS, sizeof(star_settings));

    // To avoid delay in first trasmission.
    for(idx=0; idx<2000; idx++)
        do_uip_process();

    //Get Star Profile from PCServer thro Server discovery.
    GetStarProfileFromServer();

    //Store the discovery result in flash if save Settings flag is enabled.
    if( memcmp(&star_settings, &bcast_data, sizeof(star_settings)) != 0)
    {
        //Store Discovery settings in Flash.
        Flash_Rewrite(&bcast_data, ADDR_STAR_SETTINGS, sizeof(bcast_data));
    }

    SetLastPort(bcast_data.RandomPort);

    Sleep_SlotTimer((WORD)bcast_data.InitialDelay * 4);

    //Set the local IP Address
    SetIPAddress(&bcast_data);

    for(idx=0; idx<2000; idx++)
        do_uip_process();

    Call_UDPProcess(2000);

    StarId = bcast_data.StarId;

    SendBeaconSlot = GetSpecialBeaconSlot(StarId);

    curSlot = 1;

    dhcpCounter = 0;

    //Send Star Profile to Debug Server.
    SendDebugInfo(DBG_PROFILE, 0, StarId);

    blnResetFlag = FALSE;

    Init_SlotTimer();
    blnStarUpgradeProcess = FALSE;
    while(1)
    {
        blnInMainLoop = TRUE;
        blnWDTCheck = FALSE;

        if( blnStarUpgradeProcess )
        {
            blnStarUpgradeProcess = FALSE;
            DFU_Task();
        }

        //RF State Machine
        switch( GetRFState(curSlot) )
        {
        case RF_STATE:
            blnResetFlags = TRUE;
            blnPageResSent = FALSE;
            break;

        case PC_COM_REQ_STATE:
            //Reset the Response Request sent variables.
            blnPageResSent = FALSE;

            //Send debug information before reset the star
            if( blnResetFlag ) ResetStar(RESET_BY_COMMAND, StarId);
            break;

        case PC_COM_RES_STATE:
            //If Page Response is sent, then call the uip process only.
            if( blnPageResSent )
            {
                //Run the uip process to send the Page Response request and receive response
                do_uip_process();
                break;
            }

            PrepareLocationDataPacket(StarId, &nd);

            //Sleep based on BeaconSlot before send page response request
            //Delay Range 8ms to 80ms.
            DelayMoreThan64ms( (SendBeaconSlot%12) * 250 );

            //Create TCP Connection to send Page Response Request.
            Init_Connection(nd.data, nd.dataLen);

            //Set Page Response Request is sent
            blnPageResSent = TRUE;

            break;
        }

        if( curSlot == MAX_SLOTS && blnResetFlags )
        {
            //Reset state variables.
            blnResetFlags = FALSE;
            dhcpCounter++;
        }
    }
}

void do_uip_process()
{
    BYTE i;

    uip_len = enc424j600_poll();

    if(uip_len > 0)
    {
        if(BUF->type == htons(UIP_ETHTYPE_IP))
        {
            uip_arp_ipin();
            uip_input();
            if(uip_len > 0)
            {
                uip_arp_out();
                enc424j600_send();
            }
        }
        else if(BUF->type == htons(UIP_ETHTYPE_ARP))
        {
            uip_arp_arpin();
            if(uip_len > 0)
            {
                enc424j600_send();
            }
        }
    }
    else
    {
        current = rt_ticks();
        if((u16_t)(current - start) >= (u16_t)RT_CLOCK_SECOND / 2)
        {
            start = current;
            for(i = 0; i < UIP_CONNS; i++)
            {
                uip_periodic(i);
                if(uip_len > 0)
                {
                    uip_arp_out();
                    enc424j600_send();
                }
            }

#if UIP_UDP
            for(i = 0; i < UIP_UDP_CONNS; i++)
            {
                uip_udp_periodic(i);
                /* If the above function invocation resulted in data that
                should be sent out on the network, the global variable
                uip_len is set to a value > 0. */
                if(uip_len > 0)
                {
                    uip_arp_out();
                    enc424j600_send();
                }
            }
#endif /* UIP_UDP */

            if(++arptimer == 2000)
            {
                uip_arp_timer();
                arptimer = 0;
            }
        }
    }
}

#pragma vector = TIMER1_A1_VECTOR
__interrupt void timer1_interrupt(void)
{
    switch( TA1IV )
    {
    case 2:
        TA1CCR1 += TCP_TIMER;
        ++ticks;
        break;
    case 10:
        OverflowCnt++;
        break;
    }
}

static BOOL Call_UDPProcess(WORD cnt)
{
    WORD idx;
    //Run the uip_process to send ButtonResp info
    for(idx=0; idx<cnt; idx++)
    {
        do_uip_process();

        //if sent successfully, then exit the loop
        if( CheckIsUDP_ProcessCompleted() )
            return TRUE;
    }
    return FALSE;
}
