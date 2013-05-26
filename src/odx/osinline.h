
#ifndef __OSINLINE__
#define __OSINLINE__

#include "minimal.h"

/* What goes herein depends heavily on the OS. */

#define DIRTY_H 256
#define DIRTY_V 100

extern char *dirty_new;
#define osd_mark_vector_dirty(x,y) dirty_new[((y)>>4) * DIRTY_H + ((x)>>4)] = 1

#define osd_cycles odx_timer_read

#define clip_short _clip_short
#define clip_short_pre() int a; const int m=0x7fff
#define _clip_short(x) { int sign = x >> 31; if (sign != (x >> 15)) x = sign ^ ((1 << 15) - 1); }

#define clip_short_ret _clip_short_ret
INLINE int _clip_short_ret(int x) { _clip_short(x); return x; }

#endif /* __OSINLINE__ */

