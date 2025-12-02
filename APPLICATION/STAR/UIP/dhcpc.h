#ifndef __DHCPC_H__
#define __DHCPC_H__

#include "timer.h"
#include "pt.h"

struct dhcpc_state {
  struct pt pt;
  char state;
  struct uip_udp_conn *conn;
  struct timer timer;
  u16_t ticks;
  const void *mac_addr;
  int mac_len;
  
  u8_t serverid[4];

  u16_t lease_time[2];
  u16_t ipaddr[2];
  u16_t netmask[2];
  u16_t dnsaddr[2];
  u16_t default_router[2];
};

void dhcpc_init(const void *mac_addr, int mac_len);

void dhcpc_reinit();

PT_THREAD(handle_dhcp(void));

void dhcpc_request(void);

void dhcpc_configured(const struct dhcpc_state *s);

typedef struct dhcpc_state uip_udp_appstate_t;

u8_t Get_Dhcp_Status();

#endif /* __DHCPC_H__ */
