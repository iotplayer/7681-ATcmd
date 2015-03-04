
#ifndef   __XIP_OVLY_H__
#define   __XIP_OVLY_H__
#include "types.h"


/*
 * XIP part
 */

//".xipsec0"
//".xipsec1"
//".xipsec2"

#if (XIP_ENABLE != 0)
#define  XIP_ATTRIBUTE(x)    __attribute__ ((section(x)))
#else
#define  XIP_ATTRIBUTE(x)
#endif


/*
 * OVERLAY part
 */
#if (OVERLAY_ENABLE != 0)
#define  OVLY_ATTRIBUTE(x)    __attribute__ ((section(x)))
#else
#define     OVLY_ATTRIBUTE(x)
#endif

enum ovly_index {VMA, SIZE, LMA, RAMN};
extern uint32  _ovly_table[][4];

enum ovly_ram_index {RAMNO, OVLYNO, OVLYCNT};
extern uint32  _ovly_ram_table[][3];

extern uint32 _novlys;
extern uint32 _novlyr;


uint8 overlayload(uint32 ovlyno);
void overlayunload(uint32 ovlyno);

#endif
