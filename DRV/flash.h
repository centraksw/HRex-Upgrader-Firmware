#ifndef __FLASH_H
#define __FLASH_H

void Flash_Read(void *data, DWORD Addr, BYTE nBytes);
void Flash_Clear(char *Addr);
void Flash_Write(void *data, DWORD Addr, WORD nBytes);
void Flash_Rewrite(void *data, DWORD Addr, BYTE nBytes);
#endif
