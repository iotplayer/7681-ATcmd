#ifndef _APP_H_
#define _APP_H_
#include "xip_ovly.h"
extern volatile uint8    TaskStatus0;
extern uint8    HiSpeed;

#define GetBit(x, y)            (    ((x) & (1 << (y)))        )
#define SetBit(x, y)            (    (x) = (x) | (1 << (y))    )
#define ClearBit(x, y)            {    (x) &= (~(1 << (y)));    }
#define DSetBit(x, y)            (    (x) = (1 << (y))        )
#define MakeWord(HI, LO)        (    ((uint16)(LO)) | (((uint16)(HI))<<8) )    // Note! LO must be read first
#define HiByte(W)                (    (uint8)(((uint16)W) >> 8)    )
#define LoByte(W)                (    (uint8)(W)                )

void APP_Init(void)  XIP_ATTRIBUTE(".xipsec0");
#endif
