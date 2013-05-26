
#ifndef __OSINLINE__
#define __OSINLINE__

/* What goes herein depends heavily on the OS. */

#define DIRTY_H 256
#define DIRTY_V 1600/16

extern char *dirty_new;
#define osd_mark_vector_dirty(x,y) dirty_new[(y)/16 * DIRTY_H + (x)/16] = 1

#define vec_mult _vec_mult
INLINE int _vec_mult(int x, int y)
{
    int res_hi, res_lo;

    __asm__ __volatile__
    ("smull\t%1,%0,%2,%3"
    : "=r"(res_hi), "=r"(res_lo)
    : "r"(x), "r"(y)
    );

    return res_hi;
}

#include "wiz_lib.h"
#define osd_cycles wiz_timer_read

//dst=(src>>1)+(dst>>1);
#define mix_sample _mix_sample
#define _mix_sample(dst,src) \
	__asm__ __volatile__ \
	( " mov %2, %2, asr #1 \n" \
	" add %0, %1, %2 , asr #1 \n" \
	: "=r" (dst) \
	: "r"  (src),"r" (dst) \
	)

//if (x < -32768)
//  x = -32768;
//else if (x > 32767)
//  x = 32767;
#define clip_short _clip_short
#define clip_short_pre() int a; const int m=0x7fff
#define _clip_short(b) __asm__ __volatile__ ("mov %1,%2,asr#15;teq %1,%2,asr#31;eorne %0,%3,%2,asr#31" : "=r" (b) , "=r" (a) : "0" (b), "r"(m) : "cc" )

#endif /* __OSINLINE__ */
