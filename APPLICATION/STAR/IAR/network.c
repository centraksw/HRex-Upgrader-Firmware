#include "network.h"
#include "enc424j600.h"
#include "uip.h"

uint16_t enc424j600_poll(void)
{
  uint16_t len = 0;

  len = enc424j600PacketReceive(1500, uip_buf);

  return len;
}

void enc424j600_send(void)
{
  enc424j600PacketSend(uip_len, (unsigned char *)uip_buf);
}
