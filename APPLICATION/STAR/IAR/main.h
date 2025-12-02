#ifndef __MAIN_H
#define __MAIN_H

#include "defines.h"

#ifdef __DEBUG_MODE__
#define STAR_ID                 0x07
#endif

WORD Timer_Ticks(void);

extern BOOL blnStarUpgradeProcess;

#endif //__MAIN_H
