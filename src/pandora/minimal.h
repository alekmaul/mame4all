/*

  GP2X minimal library v0.A by rlyeh, (c) 2005. emulnation.info@rlyeh (swap it!)

  Thanks to Squidge, Robster, snaff, Reesy and NK, for the help & previous work! :-)

  License
  =======

  Free for non-commercial projects (it would be nice receiving a mail from you).
  Other cases, ask me first.

  GamePark Holdings is not allowed to use this library and/or use parts from it.

*/

#include <fcntl.h>
#include <linux/fb.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdarg.h>
#include "usbjoy_mame.h"

#ifndef __MINIMAL_H__
#define __MINIMAL_H__

#define pnd_video_color8(C,R,G,B) (pnd_palette[((C)<<1)+0]=((G)<<8)|(B),pnd_palette[((C)<<1)+1]=(R))
#define pnd_video_color15(R,G,B,A) ((((R)&0xF8)<<8)|(((G)&0xF8)<<3)|(((B)&0xF8)>>3)|((A)<<5))

#define pnd_video_getr15(C) (((C)>>8)&0xF8)
#define pnd_video_getg15(C) (((C)>>3)&0xF8)
#define pnd_video_getb15(C) (((C)<<3)&0xF8)

#define PHYS_SCREEN_WIDTH  800
#define PHYS_SCREEN_HEIGHT 480

enum {
	PND_FIR_FILTER_NONE = 0,
	PND_FIR_FILTER_DEFAULT
};

enum  { PND_UP=0x1,       PND_LEFT=0x4,       PND_DOWN=0x10,  PND_RIGHT=0x40,
        PND_START=1<<8,   PND_SELECT=1<<9,    PND_L=1<<10,    PND_R=1<<11,
        PND_A=1<<12,      PND_B=1<<13,        PND_X=1<<14,    PND_Y=1<<15 };

#define PND_KEY_MAX 256

extern volatile unsigned short 	pnd_palette[512];
extern unsigned short		pnd_palette_rgb[256];
extern unsigned char 		*pnd_screen8;
extern unsigned short 		*pnd_screen15;
extern int			pnd_sound_rate;
extern int			pnd_sound_stereo;
extern int 			pnd_pal_50hz;
extern int			pnd_clock;
extern int			pnd_phys_width;
extern int			pnd_phys_height;
extern unsigned char	pnd_keys[PND_KEY_MAX];

extern void pnd_init(int tickspersecond, int bpp, int rate, int bits, int stereo, int hz);
extern void pnd_deinit(void);
extern void pnd_timer_delay(unsigned long ticks);
extern void pnd_sound_play(void *buff, int len);
extern void pnd_video_flip(void);
extern void pnd_video_flip_single(void);
extern void pnd_video_setpalette(void);
extern unsigned long pnd_joystick_read(int n);
extern unsigned long pnd_joystick_press (int n);
extern void pnd_sound_volume(int vol);
extern void pnd_sound_thread_mute(void);
extern void pnd_sound_thread_start(void);
extern void pnd_sound_thread_stop(void);
extern void pnd_sound_set_rate(int rate);
extern void pnd_sound_set_stereo(int stereo);
extern unsigned long pnd_timer_read(void);
extern unsigned long pnd_timer_read_real(void);
extern unsigned long pnd_timer_read_scale(void);
extern void pnd_timer_profile(void);
extern void pnd_video_wait_vsync(void);
extern void *pnd_malloc(unsigned int size);
extern void pnd_free(void *ptr);
extern void *pnd_realloc(void *ptr, unsigned int size);
extern void *pnd_calloc(unsigned int nmemb, unsigned int size);

extern void pnd_set_video_mode(int bpp,int width,int height);
extern int pnd_fir_filter_set(int f);
extern void pnd_set_clock(int mhz);

#ifdef __cplusplus
extern "C" {
#endif
void flushcache(void *start_address, void *end_address, int flags);
#ifdef __cplusplus
} /* End of extern "C" */
#endif

extern void pnd_printf(char* fmt, ...);
extern void pnd_printf_init(void);
extern void pnd_gamelist_text_out(int x, int y, char *eltexto);
extern void pnd_gamelist_text_out_fmt(int x, int y, char* fmt, ...);

#endif
