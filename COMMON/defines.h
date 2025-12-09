#ifndef __DEFINES_H
#define __DEFINES_H

typedef unsigned char       BOOL;
typedef void		    VOID;
typedef unsigned long       UINT32;
typedef signed long         INT32;

typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned long       DWORD;
typedef unsigned char       UINT8;
typedef unsigned short      UINT16;
typedef signed char         INT8;
typedef signed short        INT16;

typedef unsigned char       bool;
typedef unsigned short      uint16_t;
typedef unsigned char       uint8_t;
#ifndef FALSE
    #define FALSE 0
#endif

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef NULL
    #define NULL 0
#endif

#define RESET_TIMEOUT                   5000

#define VERSION                         102

#define MAX_SEGMENTS                    255
#define MAX_TAGSPIDER_SEGMENTS          30
#define MAX_TIME_SLOT                   288

#define PLUS_10_DBM                     0xC1
#define PLUS_8_DBM                      0xC6
#define PLUS_5_DBM                      0x85
#define PLUS_4_DBM                      0x88
#define PLUS_3_DBM                      0x8B
#define PLUS_2_DBM                      0x8D
#define PLUS_0_DBM                      0x3F
#define MINUS_2_DBM                     0x3C
#define MINUS_4_DBM                     0x56
#define MINUS_6_DBM                     0x69
#define MINUS_8_DBM                     0x27
#define MINUS_10_DBM                    0x6D
#define MINUS_15_DBM                    0x1B
#define MINUS_20_DBM                    0x0D
#define MINUS_30_DBM                    0x03
#define MINUS_50_DBM                    0x00

#define PLUS_5_1_DBM                    0x83   /* 19.9 mA */
#define PLUS_1_8_DBM                    0x8C   /* 17.5 mA */
#define MINUS_1_0_DBM                   0x61   /* 15.7 mA */

#define RF_STATUS_OK                        1
#define RF_STATUS_NODATA                    2
#define RF_STATUS_INSUFFICIENT_DATA_ERROR   3
#define RF_STATUS_TOO_BIG_PACKET_ERROR      4
#define RF_STATUS_CRC_ERROR                 5
#define RF_STATUS_CHKSUM_ERROR              6
#define RF_STATUS_WRONG_BEACON_PKT_ERROR    7
#define RF_STATUS_TIMEOUT_ERROR             8

#define ALERT_INFO_LEN              6
#define RF_HDR_ALERT_INFO           8
#define TIME_SLOT                       250
#define TIME_SLOT_US                    250000

#define BASE_DUTY_CYCLE_US              12000000
#define BASE_DUTY_CYCLE_24_US           24000000

#define BEACON_CHANNEL                  120
#define RF_CHANNEL                      170

////////////////////////////////////////////////////////////////////////////////
#define PAGE_DELAY_250_MS               1
#define PAGE_DELAY_1_5_SEC              6
#define PAGE_DELAY_3_SECS               12        // 3 secs
#define PAGE_DELAY_12_5_SEC             50
#define PAGE_DELAY_9_5_SEC              38
#define PAGE_DELAY_6_5_SEC              26
#define PAGE_DELAY_5_MINS               1204      // 5 mins    5 * 60 * 4

////////////////////////////////////////////////////////////////////////////////
#define SPIDER_LOW_BATTERY_THRESHOLD    700
#define TEMP_TAG_LBI_CYCLE_COUNTER      14400    // 10 Mins  (10 * 60 * 4)
#define LOW_BATTERY_CYCLE_COUNTER       2400    // 10 Mins  (10 * 60 * 4)
#define TEMPERATURE_SLOTS_COUNTER       1200    // 5 Mins   (5 * 60 * 4)
#define IAM_ALIVE_INTERVAL              1200
#define TURN_ON_LED_COUNTER             12      // 3 Sec

//////////////////////////////////////////////////////////////////////////////
////// BITRATE_76_8_KBPS
//////////////////////////////////////////////////////////////////////////////
#define BEACON_RECEIVE_TIME             6800        //5400
#define BS_FLYOVER_TIME                 20
#define TAG_FLYOVER_TIME                30
#define BS_BEACON_RECEIVE_TIME          244000            //242560        //1911

#define PULLBOX_TAG_NUM_ACTIVE_SLOTS    120         // 30 Secs (30 * 4)
#define PULLBOX_IR_RECEIVE_INTERVAL     360         // 90 Secs
#define NUM_MD_ACTIVE_CYCLES            2

#define SPIDER_I_AM_ALIVE_INTERVAL      14400       // 1 Hour. (60 * 60 * 4)

#define MAX_TCP_FAIL_COUNT          50
#define DHCP_REINIT_COUNTER         3000        //(3000 sec)  10 hours
#define TS_DHCP_REINIT_COUNTER      144000      // 10 hours : 10 * 60 * 60 * 4

#define UIP_TCP_APP_STATE_SIZE      4096
#define UPGRADE_BUF_SIZE            512

#define PAGING_INFO                 0
#define LOCATION_INFO               1
#define RESPONSE_REQUEST            2
#define RSSI_DATA                   3

#define MAX_RSSI_DATA               (UIP_TCP_APP_STATE_SIZE-10)

#define PAGING_REQUEST_OFFSET       0
#define RESPONSE_REQUEST_OFFSET     3100
#define CONTAMINATED_ROOMS_OFFSET   3200

#define MAX_ALLOW_DEVS              300
#define MAX_DEVICE_DATA             250

#define MAX_DELAY_INDEX             16
#define MAX_LOCATION_DATA_RETRY     3
#define MAX_SPIDER_INFO_RETRY       3
#define MAX_STAR_BEACON_FAIL        2
#define MAX_TAG_BEACON_FAIL         6
#define MAX_SPIDER_BEACON_FAIL      6
#define MAX_SPIDER_DATA_FAIL        2
#define MAX_LOCATION_DATA_FAIL      2
#define MAX_NO_MD_SLOTS             8


#define MAX_TAG_PSEUDO_SYNC_FAIL    3
#define MAX_TAG_PSEUDO_SYNC         2

#define MAX_SPIDER_PSEUDO_SYNC_FAIL 3
#define MAX_SPIDER_PSEUDO_SYNC      2

#define MAX_NO_BEACON_COUNT         10

#define MAX_ZERO_COUNTER            4

#define OPEN_ALL_GATES              15

#define MAX_FREQUENCY               13
#define BCAST_COUNT                 3

#define PROFILE_MASK                0x7F

#define NUM_SLOTS_PER_PC_COM        12

#define IR_SLOT_ONE                 5

#define FIRST_IR_SLOT               0
#define SECOND_IR_SLOT              1
#define THIRD_IR_SLOT               2
#define FOURTH_IR_SLOT              3

#define MAX_SLOTS                   48

#define RF_STATE                    1
#define IR_STATE                    2
#define PC_COM_REQ_STATE            3
#define PC_COM_RES_STATE            4

#define RF_HDR_PAGING_REQUEST       1
#define RF_HDR_PAGING_RESPONSE      2
#define RF_HDR_BUTTON_RESP_DATA     3
#define RF_HDR_BUTTON_RESP_DATA_ACK 4
#define RF_HDR_SPIDER_INFO          5

#define RF_HDR_PAGING_REQUEST_EX    6
#define RF_HDR_PAGING_RESPONSE_EX   7
#define RF_HDR_TAG_INFO_EX          8
#define RF_HDR_ACK_EX               9
#define RF_HDR_SPIDER_INFO_EX       10
#define RF_HDR_BEACON_EX            11

#define RF_HDR_FW_PACKET            12

#define RF_HDR_HYGIENE_DATA         14
#define RF_HDR_HYGIENE_ACK          15

#if defined(__TAG_1222)
#define MAX_PKT_LEN                 16
#define MAX_UPGRADE_SLOTS           1
#define MAX_UPGRADE_BYTE_SIZE       1
#define MAX_UPGRADE_SEGMENT_SIZE    1
#else
#define MAX_PKT_LEN                 40
#define MAX_UPGRADE_SLOTS           480
#define MAX_UPGRADE_BYTE_SIZE       64
#define MAX_UPGRADE_SEGMENT_SIZE    32
#endif

#define UPGRADE_PKT_PER_CYCLE       4
#define FW_PKT_LEN                  35
#define FW_FINISH_CMD_LEN           34

#define BEACON_PKT_LEN              4
#define STAR_ACK_PKT_LEN            6
#define PAGE_REQ_PKT_LEN            7
#define PAGE_RES_PKT_LEN            9
#define TAG_INFO_LEN                9
#define SPIDER_INFO_LEN             7
#define DEBUG_INFO_LEN              6

#define BEACON_PKT_LEN_EX           5
#define STAR_ACK_PKT_LEN_EX         8
#define PAGE_REQ_PKT_LEN_EX         7
#define PAGE_RES_PKT_LEN_EX         12
#define TAG_INFO_LEN_EX             11
#define SPIDER_INFO_LEN_EX          11
#define BUTTON_RESP_DATA_LEN        7
#define BUTTON_RESP_DATA_ACK_PKT_LEN   4

#define HYGIENE_DATA_PKT_LEN        6
#define HYGIENE_ACK_PKT_LEN         4

#define TAG_ALLOW                   1
#define TAG_DISALLOW                2

#define IR_PROFILE_1_5_SECS         0
#define IR_PROFILE_3_0_SECS         1


#define SPIDER_NORMAL                     0
#define SPIDER_A1                         1
#define SPIDER_B1                         2
#define SPIDER_A2                         3
#define SPIDER_B2                         4
#define SPIDER_C2                         5
#define SPIDER_A3                         6
#define SPIDER_B3                         7
#define SPIDER_C3                         8
#define SPIDER_D3                         9
#define SPIDER_VW_MASTERA                 10
#define SPIDER_VW_MASTERB                 11

#define SPIDER_VW_WIRELESS_MASTERA        13
#define SPIDER_VW_WIRELESS_MASTERB        14
#define SPIDER_VW_WIRELESS_SLAVEA         15
#define SPIDER_VW_WIRELESS_SLAVEB         16
#define SPIDER_BLOCKING                   17
#define SPIDER_VW_WOLVERHAMPTON_A         18
#define SPIDER_VW_WOLVERHAMPTON_B         19
#define SPIDER_VW_WOLVERHAMPTON_C         20

#define TAG_REGULAR                       0
#define TAG_HYGIENE                       1
#define TAG_TEMPERATURE                   2
#define TAG_ERU                           3

#define SPIDER_REGULAR                    0
#define SPIDER_DIM                        1

#define REGULAR_STAR                      0
#define TIME_STAR                         1

#define TAG_UPGRADE                       0
#define MONITOR_UPGRADE                   1

#define CMD_TAG_RESET			0x1
#define CMD_TAG_TURN_ON                 0x2
#define CMD_TAG_SET_FREQUENCY		0x3
#define CMD_TAG_SET_PROFILE		0x4
#define CMD_TAG_GET_PROFILE		0x5
#define CMD_TAG_GET_VERSION		0x6
#define CMD_TAG_GET_BATTERY_STATUS	0x7
#define CMD_TAG_GET_TEMPERATURE		0x8
#define CMD_TAG_ALERT                   0x9
#define CMD_TAG_SET_MEASUREMENT_RATE    0xA
#define CMD_TAG_GET_MEASUREMENT_RATE    0xB
#define CMD_TAG_UPGRADE_INFO		0xC
#define CMD_TAG_SET_SLEEP_MODE          0xD
#define TURN_ON_LED		        0x8
#define TURN_ON_BUZZER		        0x4
#define TURN_ON_GPIO		        0x2
#define CLEAR_INDICATOR                 0x0
#define TURN_ON_VIBRATOR                0x2
#define TURN_OFF_VIBRATOR               0x0

#define CMD_SPIDER_RESET		0x1
#define CMD_SPIDER_SET_FREQUENCY	0x3
#define CMD_SPIDER_SET_PROFILE		0x4
#define CMD_SPIDER_GET_PROFILE		0x5
#define CMD_SPIDER_GET_VERSION		0x6
#define CMD_SPIDER_GET_BATTERY_STATUS	0x7
#define CMD_SPIDER_UPGRADE_INFO		0xC

#define CMD_TIMESTAR_FIRMWARE_UPGRADE   4
#define CMD_SPIDER_FIRMWARE_UPGRADE     5
#define CMD_STAR_SET_TX_POWER	        6
#define CMD_STAR_SET_SIMPLE_MODE        7
#define CMD_STOP_FIRMWARE_UPGRADE       8
#define CMD_STAR_RESET                  9
#define CMD_STAR_GET_PROFILE            10
#define CMD_STAR_GET_ALLOWLIST          11

#define CMD_STAR_SET_RX_TX_STATE        12
#define CMD_STAR_WAKEUP_SPIDERS         13
#define CMD_STAR_FIRMWARE_UPGRADE       14


#define BCAST_CMD_WAKEUP	    3

#define LED_ON                      1
#define LED_ON_TIME                 8    // 2 msec   8*8*30.51 = 1952.64 us

#define PART1           1
#define PART2           2

#define UDP_DISCOVERY               1
#define UDP_DHCP                    2
#define UDP_TIME_REQUEST            3
#define UDP_DEBUG                   4
#define UDP_BUTTON_RESP             5

#define ERR_STAR_ID		    1
#define ERR_BEACON_SLOT		    2
#define ERR_RSSI_THEASHOLD	    3
#define ERR_BEST_RSSI		    4

#define STAR_MASTER_TIME_SERVER     0
#define STAR_BEACON_GEN             1
#define STAR_REGULAR                2
#define STAR_ETHERNET_SYNC          3
#define STAR_SLAVE_TIME_SERVER      4

#define TX_RX_STOP                  0x0
#define TX_ALLOW_PAGING             0x1
#define TX_ALLOW_BEACON             0x2
#define RX_ALLOW_PAGING             0x4

#define TX_RX_ALLOW_BOTH            0x7

#ifdef __DEBUG_MODE__
#define MAX_BEACONS             2
#else
#define MAX_BEACONS             10
#endif

#define DBG_DISCOVERY           0x1
#define DBG_DHCP                0x2
#define DBG_PROFILE             0x3
#define DBG_BEST_BEACONS        0x4
#define DBG_NO_TIMESERVER       0x5
#define DBG_TCP_FAILURE         0x6
#define DBG_NO_BEACON           0x7
#define DBG_BEACON_FAIL         0x8
#define DBG_RESET               0x9
#define DBG_BEACON_SEARCH       0xA
#define DBG_RESYNC              0xB
#define DBG_GETTIME_FAILURE     0xC
#define DBG_BEACON_FAIL_COUNT   0xD
#define DBG_OFFSET_OVERLIMIT    0xE

#define MAX_DISCOVERY_RETRY         6
#define MAX_DHCP_RETRY              6
#define MAX_GETTIME_RETRY           12

#define RESET_BY_COMMAND            1
#define RESET_NO_BEACON             2
#define RESET_NO_DISCOVERY_RESP     3
#define RESET_NO_DHCP               4
#define RESET_FAILED_TO_GETTIME     5
#define RESET_TCP_FAIL              6
#define RESET_WRONG_DELAY           7


#ifndef MSB
    #define MSB 0x80
#endif

#if defined(__TAG_1222)
#include "msp430x12x2.h"
#endif

#if defined(__STAR_BS_1611)
#include "msp430x16x.h"
#endif

#if defined(__STAR_5x)
#include "msp430f5659.h"
#endif

#include "pinconfig.h"
#include "main.h"


#endif // __DEFINES_H
