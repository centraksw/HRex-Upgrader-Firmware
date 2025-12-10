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

#define RESET_TIMEOUT                                   5000

#define VERSION                                         101

#define MAX_SEGMENTS                                    255

#define TIME_SLOT_US                                    250000

#define MAX_TCP_FAIL_COUNT                              50
#define DHCP_REINIT_COUNTER                             3000    //(3000 sec)  10 hours
#define TS_DHCP_REINIT_COUNTER                          144000  // 10 hours : 10 * 60 * 60 * 4

#define UIP_TCP_APP_STATE_SIZE                          4096
#define UPGRADE_BUF_SIZE                                512

#define MAX_SLOTS                                       48

#define RF_STATE                                        1
#define IR_STATE                                        2
#define PC_COM_REQ_STATE                                3
#define PC_COM_RES_STATE                                4

#define MAX_UPGRADE_SEGMENT_SIZE                        32

#define CMD_STAR_RESET                                  9
#define CMD_STAR_FIRMWARE_UPGRADE                       14

#define UDP_DISCOVERY                                   1
#define UDP_DHCP                                        2
#define UDP_TIME_REQUEST                                3
#define UDP_DEBUG                                       4

#define DBG_DISCOVERY                                   0x1
#define DBG_DHCP                                        0x2
#define DBG_PROFILE                                     0x3
#define DBG_BEST_BEACONS                                0x4
#define DBG_NO_TIMESERVER                               0x5
#define DBG_TCP_FAILURE                                 0x6
#define DBG_NO_BEACON                                   0x7
#define DBG_BEACON_FAIL                                 0x8
#define DBG_RESET                                       0x9

#define MAX_DISCOVERY_RETRY                             6
#define MAX_DHCP_RETRY                                  6

#define RESET_BY_COMMAND                                1
#define RESET_NO_BEACON                                 2
#define RESET_NO_DISCOVERY_RESP                         3
#define RESET_NO_DHCP                                   4
#define RESET_TCP_FAIL                                  6

#include <msp430f5359.h>
#include "pinconfig.h"
#include "main.h"

#endif // __DEFINES_H
