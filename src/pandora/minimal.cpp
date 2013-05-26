/*

  GP2X minimal library v0.A by rlyeh, (c) 2005. emulnation.info@rlyeh (swap it!)

  Thanks to Squidge, Robster, snaff, Reesy and NK, for the help & previous work! :-)

  Adapted for Pandora by Steve Maddison <steve@cosam.org> August 2010

  License
  =======

  Free for non-commercial projects (it would be nice receiving a mail from you).
  Other cases, ask me first.

  GamePark Holdings is not allowed to use this library and/or use parts from it.

*/

#include "minimal.h"

#include <errno.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <SDL.h>
#include <SDL_audio.h>
#ifndef FBIO_WAITFORVSYNC
#define FBIO_WAITFORVSYNC _IOW('F', 0x20, __u32)
#endif

unsigned long 			key_dev[2];
int 					key_devs = 0;
unsigned long			fb_dev = 0;
void					*fb_ptr[2] = { NULL, NULL };
unsigned int			fb_mmap_size = 0;
struct fb_var_screeninfo vinfo;
static unsigned long 	pnd_ticks_per_second;
unsigned char 			*pnd_screen8;
unsigned short 			*pnd_screen15;
unsigned int			pnd_nflip;
volatile unsigned short	pnd_palette[512];
unsigned short			pnd_palette_rgb[256];
SDL_AudioSpec 			pnd_audio_spec;
int						pnd_audio_buffer_len=0;
int						pnd_sndlen=0;
int						pnd_vol = 100;
int						pnd_sound_rate=22050;
int						pnd_sound_stereo=1;
int 					pnd_pal_50hz=0;
int						pnd_clock=200;
int 					rotate_controls=0;
int						pnd_phys_width = PHYS_SCREEN_WIDTH;
int						pnd_phys_height = PHYS_SCREEN_HEIGHT;
unsigned char			pnd_keys[PND_KEY_MAX];

extern int master_volume;
#define MAX_SAMPLE_RATE (44100*2)

#include <stdio.h>
#include <string.h>

//#define PND_DEBUG 1

#define PND_FBDEV "/dev/fb1"

static int pnd_run_cmd( const char *cmd ) {
	int ret = 0;

#ifdef PND_DEBUG
	printf( "pnd_run_cmd: '%s'\n", cmd );
#endif	
	ret = system( cmd );
	
	if( ret != 0 ) {
		fprintf( stderr, "Couldn't run command '%s': return value = %d\n", ret );
		return -1;
	}
	
	return 0;
}

int pnd_fb_set( int fb_w, int fb_h, int scr_w, int scr_h, int bpp, int buffers ) {
	char cmd[256];
	
	if( scr_w > PHYS_SCREEN_WIDTH || scr_h > PHYS_SCREEN_HEIGHT ) {
		fprintf( stderr, "Requested screen dimensions (%dx%d) exceed physical maximum (%dx%d)\n",
			scr_w, scr_h, PHYS_SCREEN_WIDTH, PHYS_SCREEN_HEIGHT );
		return -1;
	}

	if( fb_w > scr_w || fb_h > scr_h ) {
		fprintf( stderr, "Requested frame buffer dimensions (%dx%d) screen dimensions (%dx%d)\n",
			fb_w, fb_h, scr_w, scr_h );
		return -1;
	}	

	snprintf( cmd, sizeof(cmd), "ofbset -fb %s -pos %d %d -size %d %d -mem %d -en 1",
		PND_FBDEV, (PHYS_SCREEN_WIDTH-scr_w)/2, (PHYS_SCREEN_HEIGHT-scr_h)/2, scr_w, scr_h, fb_w * fb_h * (bpp/8) * buffers );
	if( pnd_run_cmd(cmd) != 0 )
		return -1;
	
	snprintf( cmd, sizeof(cmd), "fbset -fb %s -g %d %d %d %d %d",
		PND_FBDEV, fb_w, fb_h, fb_w, fb_h*buffers, bpp );
	if( pnd_run_cmd(cmd) != 0 )
		return -1;
	
	return 0;
}

int pnd_fb_reset( void ) {
	char cmd[256];
	snprintf( cmd, sizeof(cmd), "ofbset -fb %s -pos 0 0 -size 0 0 -mem 0 -en 0", PND_FBDEV );

	if( pnd_run_cmd(cmd) != 0 )
		return -1;

	return 0;
}

int pnd_fir_filter_set( int f ) {
	char cmd[256];
	char *filter_name = "default";
	
	switch( f ) {
		case PND_FIR_FILTER_NONE:
			filter_name = "none";
			break;
		case PND_FIR_FILTER_DEFAULT:
		default:
			break;
	}

	snprintf( cmd, sizeof(cmd), "sudo /usr/pandora/scripts/op_videofir.sh %s", filter_name );

	if( pnd_run_cmd(cmd) != 0 )
		return -1;

	return 0;
}

void pnd_video_flip(void)
{
	ioctl(fb_dev, FBIOGET_VSCREENINFO, &vinfo);
	vinfo.yoffset = vinfo.yres * pnd_nflip;
	ioctl(fb_dev, FBIOPAN_DISPLAY, &vinfo);

	pnd_nflip=pnd_nflip?0:1;
	
  	pnd_screen15=(unsigned short *)fb_ptr[pnd_nflip];
  	pnd_screen8=(unsigned char *)pnd_screen15;
}


void pnd_video_flip_single(void)
{
  	pnd_nflip=0;
  		
	ioctl(fb_dev, FBIOGET_VSCREENINFO, &vinfo);
	vinfo.yoffset = vinfo.yres * pnd_nflip;
	ioctl(fb_dev, FBIOPAN_DISPLAY, &vinfo);
	
  	pnd_screen15=(unsigned short *)fb_ptr[pnd_nflip]; 
  	pnd_screen8=(unsigned char *)pnd_screen15; 
}

void pnd_video_wait_vsync(void) {
	int fbarg = 0;
	ioctl(fb_dev, FBIO_WAITFORVSYNC, &fbarg);
}

void pnd_video_setpalette(void)
{
#ifdef HAVE_FB_PALETTE
	struct fb_cmap fbcmap;
	unsigned char *c = (unsigned char *)pnd_palette;
	unsigned short red[256];
	unsigned short green[256];
	unsigned short blue[256];
	int i = 0;
	
	if( !fb_dev )
		return;

	fbcmap.start = 0;
	fbcmap.len = 256;
	fbcmap.red = red;
	fbcmap.green = green;
	fbcmap.blue = blue;
	fbcmap.transp = NULL;

    if( ioctl(fb_dev, FBIOGETCMAP, &fbcmap) == -1 ) {
    	fprintf(stderr, "Error getting frame buffer (%d) palette: %s\n", fb_dev, strerror(errno));
    }
	
	for( i = 0 ; i < 256 ; i++ ) {
		blue[i] = *c++;
		green[i] = *c++;
		red[i] = *c++;
		c++;
	}
	
    if( ioctl(fb_dev, FBIOPUTCMAP, &fbcmap) == -1 ) {
    	fprintf(stderr, "Error setting frame buffer (%d) palette: %s\n", fb_dev, strerror(errno));
    }
#else
	unsigned char *c = (unsigned char *)pnd_palette;
	int i;

	for( i = 0 ; i < 256 ; i++ ) {
		pnd_palette_rgb[i] = *c++ >> 3;
		pnd_palette_rgb[i] |= (*c++&0xf8) << 3;
		pnd_palette_rgb[i] |= (*c++&0xf8) << 8;
		pnd_palette_rgb[i] |= (*c++) << 5;
	}
#endif
}

unsigned long pnd_joystick_read(int n)
{
  	static unsigned long res=0;
	
	if (n==0)
	{
#define NUM_INPUT_EVENTS 64
		struct input_event ev[NUM_INPUT_EVENTS];
		int nread = 0;
		int i = 0;
		int dev = 0;

		// Read events from the key devices
		for( dev = 0 ; dev < key_devs ; dev++ ) {
			nread = read( key_dev[dev], ev, sizeof(struct input_event) * NUM_INPUT_EVENTS );
			if( nread >= (int)sizeof(struct input_event) ) {
				for( i = 0 ; i < nread / sizeof(struct input_event) ; i++ ) {
					if ( ev[i].type == EV_KEY ) {
						if( ev[i].value == 0 ) {
							// Key up
							switch( ev[i].code ) {
								case KEY_UP:			res &= ~(rotate_controls ? PND_LEFT : PND_UP); break;
								case KEY_DOWN:			res &= ~(rotate_controls ? PND_RIGHT : PND_DOWN); break;
								case KEY_LEFT:			res &= ~(rotate_controls ? PND_DOWN : PND_LEFT); break;
								case KEY_RIGHT:			res &= ~(rotate_controls ? PND_UP : PND_RIGHT); break;
								case KEY_LEFTALT:		res &= ~PND_START; break;
								case KEY_LEFTCTRL:		res &= ~PND_SELECT; break;
								case KEY_RIGHTSHIFT:	res &= ~PND_L; break;
								case KEY_RIGHTCTRL: 	res &= ~PND_R; break;
								case KEY_HOME:			res &= ~PND_A; break;
								case KEY_END:			res &= ~PND_B; break;
								case KEY_PAGEDOWN:		res &= ~PND_X; break;
								case KEY_PAGEUP:		res &= ~PND_Y; break;
								default: break;
							}
							if( ev[i].code < PND_KEY_MAX ) {
								if( rotate_controls ) {
									switch( ev[i].code ) {
										case KEY_UP:	pnd_keys[KEY_LEFT] = 0; break;
										case KEY_DOWN:	pnd_keys[KEY_RIGHT] = 0; break;
										case KEY_LEFT:	pnd_keys[KEY_DOWN] = 0; break;
										case KEY_RIGHT:	pnd_keys[KEY_UP] = 0; break;
										default: pnd_keys[ev[i].code] = 0; break;
									}
								}
								else {
									pnd_keys[ev[i].code] = 0;
								}
							}
						}
						else if( ev[i].value == 1 ) {
							// Key down
							switch( ev[i].code ) {
								case KEY_UP:			res |= (rotate_controls ? PND_LEFT : PND_UP); break;
								case KEY_DOWN:			res |= (rotate_controls ? PND_RIGHT : PND_DOWN); break;
								case KEY_LEFT:			res |= (rotate_controls ? PND_DOWN : PND_LEFT); break;
								case KEY_RIGHT:			res |= (rotate_controls ? PND_UP : PND_RIGHT); break;
								case KEY_LEFTALT:		res |= PND_START; break;
								case KEY_LEFTCTRL:		res |= PND_SELECT; break;
								case KEY_RIGHTSHIFT:	res |= PND_L; break;
								case KEY_RIGHTCTRL: 	res |= PND_R; break;
								case KEY_HOME:			res |= PND_A; break;
								case KEY_END:			res |= PND_B; break;
								case KEY_PAGEDOWN:		res |= PND_X; break;
								case KEY_PAGEUP:		res |= PND_Y; break;
								default: break;
							}
							if( ev[i].code < PND_KEY_MAX ) {
								if( rotate_controls ) {
									switch( ev[i].code ) {
										case KEY_UP:	pnd_keys[KEY_LEFT] = 1; break;
										case KEY_DOWN:	pnd_keys[KEY_RIGHT] = 1; break;
										case KEY_LEFT:	pnd_keys[KEY_DOWN] = 1; break;
										case KEY_RIGHT:	pnd_keys[KEY_UP] = 1; break;
										default: pnd_keys[ev[i].code] = 1; break;
									}
								}
								else {
									pnd_keys[ev[i].code] = 1;
								}
							}
						}
					}
				}
			}
		}
	}

	if (num_of_joys>n)
	{
		res |= pnd_usbjoy_check(joys[n]);
	}

	return res;
}

unsigned long pnd_joystick_press (int n)
{
	unsigned long ExKey=0;
	while(pnd_joystick_read(n) == 0 ) { pnd_timer_delay(150); }
	while(!(ExKey=pnd_joystick_read(n) ==0 )) { }
	return ExKey;
}

void pnd_sound_volume(int vol)
{
 	if( vol < 0 ) vol = 0;
 	if( vol > 100 ) vol = 100;

 	if( vol > 0 ) {
 		master_volume = vol;
 		if( pnd_vol == 0 ) {
 			SDL_PauseAudio(0);
 			if( pnd_audio_spec.userdata )
	 			memset( pnd_audio_spec.userdata, 0 , pnd_audio_buffer_len );
 			pnd_sndlen = 0;
 		}
 	}
 	else {
		SDL_PauseAudio(1);
 	}
 	
 	pnd_vol = vol;
}

void pnd_timer_delay(unsigned long ticks)
{
	unsigned long ini=pnd_timer_read();
	while (pnd_timer_read()-ini<ticks);
}

unsigned long pnd_timer_read(void)
{
	struct timeval tval;
	gettimeofday(&tval, 0);
	return ((tval.tv_sec*1000000)+tval.tv_usec)/pnd_ticks_per_second;
}

unsigned long pnd_timer_read_real(void)
{
	struct timeval tval;
	gettimeofday(&tval, 0);
	return (tval.tv_sec*1000000)+tval.tv_usec;
}

unsigned long pnd_timer_read_scale(void)
{
 	return pnd_ticks_per_second;
}

void pnd_timer_profile(void)
{
	static unsigned long i=0;
	if (!i) i=pnd_timer_read_real();
	else {
		printf("%u\n",pnd_timer_read_real()-i);
		i=0;	
	}
}

void pnd_sound_play(void *buff, int len)
{
	if( pnd_sndlen+len > pnd_audio_buffer_len ) {
		/* Overrun */
		pnd_sndlen = 0;
		return;
	}
	
	SDL_LockAudio();
	memcpy( pnd_audio_spec.userdata + pnd_sndlen, buff, len );
	pnd_sndlen += len;
	SDL_UnlockAudio();
}

static void pnd_sound_callback(void *data, Uint8 *stream, int len)
{
	if( pnd_sndlen < len ) {
#ifdef PND_DEBUG
		printf("Audio underrun (%d/%d bytes)\n", pnd_sndlen, len);
#endif
		memcpy(stream, data, pnd_sndlen );
		pnd_sndlen = 0;
		return;
	}

	memcpy( stream, data, len );
	pnd_sndlen -= len;
	memcpy( data, data + len, pnd_sndlen );
}

void pnd_sound_thread_start(void)
{
	pnd_sndlen=0;

#ifdef PND_DEBUG
	printf("Setting audio to %dKHz %s\n", pnd_sound_rate, pnd_sound_stereo ? "stereo" : "mono");
#endif

    pnd_audio_spec.freq = pnd_sound_rate;
    pnd_audio_spec.channels = pnd_sound_stereo ? 2: 1;

	pnd_audio_buffer_len = pnd_audio_spec.samples * pnd_audio_spec.channels * 2 * 64;
	void *audiobuf = malloc( pnd_audio_buffer_len );
	memset( audiobuf, 0 , pnd_audio_buffer_len );
	pnd_audio_spec.userdata=audiobuf;

	if ( SDL_OpenAudio(&pnd_audio_spec, NULL) < 0 )
		fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
	
	SDL_PauseAudio(0);
}

void pnd_sound_thread_stop(void)
{
	SDL_CloseAudio();

	if( pnd_audio_spec.userdata ) {
		free( pnd_audio_spec.userdata );
		pnd_audio_spec.userdata = NULL;
	}
}

void pnd_init(int ticks_per_second, int bpp, int rate, int bits, int stereo, int Hz)
{
	char buf[32];
	int i, fd = 0;

  	pnd_ticks_per_second=1000000/ticks_per_second;

	for( i = 0; 1 ; i++ ) {
		sprintf( buf, "/dev/input/event%i", i );

		fd = open( buf, O_RDONLY|O_NONBLOCK );
		if( fd < 0 ) {
			break;
		}

		ioctl( fd, EVIOCGNAME(sizeof(buf)), buf );
		if( strcmp( buf, "keypad" ) == 0 || strcmp( buf, "gpio-keys" ) == 0 ) {
#ifdef PND_DEBUG
			printf("Key device: /dev/input/event%i (%s)\n", i, buf);
#endif
			key_dev[key_devs++] = fd;
		}
		else {
			close( fd );
		}
	}

	/* All keys unpressed. */
	for( i = 0 ; i < PND_KEY_MAX ; i++ ) {
		pnd_keys[i] = 0;
	}

	/* USB Joysticks Initialization */
	pnd_usbjoy_init();

	/* General audio stuff */
	SDL_Init(SDL_INIT_AUDIO);
    pnd_audio_spec.freq = rate;
	if( bits == 16 )
    	pnd_audio_spec.format = AUDIO_S16SYS;
    else
    	pnd_audio_spec.format = AUDIO_S8;
    pnd_audio_spec.channels = stereo ? 2: 1;
    pnd_audio_spec.samples = 1024;
    pnd_audio_spec.callback = pnd_sound_callback;
    pnd_audio_spec.userdata = NULL;

	pnd_set_video_mode(bpp,400,240);

	pnd_video_color8(0,0,0,0);
	pnd_video_color8(255,255,255,255);
	pnd_video_setpalette();
}

#if defined(__cplusplus)
extern "C" {
#endif
extern int fcloseall(void);
#if defined(__cplusplus)
}
#endif

void pnd_deinit(void)
{
	ioctl(fb_dev, FBIOGET_VSCREENINFO, &vinfo);

	memset( fb_ptr[0], 0, fb_mmap_size );
	munmap( fb_ptr[0], fb_mmap_size );
	close(fb_dev);

	pnd_fir_filter_set( PND_FIR_FILTER_DEFAULT );

	SDL_CloseAudio();

	fcloseall(); /*close all files*/
	
	pnd_fb_reset();
}

void pnd_set_clock(int mhz)
{

}

void pnd_set_video_mode(int bpp,int width,int height)
{
	if( fb_ptr[0] && fb_mmap_size ) {
		memset( fb_ptr[0], 0, fb_mmap_size );
		munmap( fb_ptr[0], fb_mmap_size );
		fb_ptr[0] = NULL;
		fb_ptr[1] = NULL;
	}

#ifdef PND_DEBUG
	printf("Attempting to set video mode %dx%d @%dbpp\n", width, height, bpp);
#endif

	if( pnd_fb_set( width, height, pnd_phys_width, pnd_phys_height, 16, 2 ) != 0 )
		exit(-1);

	if( !fb_dev ) {
		fb_dev = open(PND_FBDEV, O_RDWR);
	}
	if( !fb_dev ) {
	  	printf("Couldn't open frame buffer device: %s\n", strerror(errno));
	  	exit(-1);
	}

	ioctl(fb_dev, FBIOGET_VSCREENINFO, &vinfo);
#ifdef PND_DEBUG
	printf("FB: %dx%d @%dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel );
#endif

	fb_mmap_size = vinfo.xres*vinfo.yres*(vinfo.bits_per_pixel/8)*2;
	fb_ptr[0] = mmap(0, fb_mmap_size, PROT_READ|PROT_WRITE, MAP_SHARED, fb_dev, 0 );
	if( fb_ptr[0] == MAP_FAILED ) {
		printf("Couldn't map frame buffer device to memory: %s\n", strerror(errno));
		pnd_deinit();
		exit(-1);
	}
	fb_ptr[1] = fb_ptr[0] + fb_mmap_size/2;

	pnd_screen15=(unsigned short *)fb_ptr[0];
	pnd_screen8=(unsigned char *)pnd_screen15;
	pnd_nflip=0;

	memset( fb_ptr[0], 0, fb_mmap_size );
}

static unsigned char fontdata8x8[] =
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x3C,0x42,0x99,0xBD,0xBD,0x99,0x42,0x3C,0x3C,0x42,0x81,0x81,0x81,0x81,0x42,0x3C,
	0xFE,0x82,0x8A,0xD2,0xA2,0x82,0xFE,0x00,0xFE,0x82,0x82,0x82,0x82,0x82,0xFE,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x64,0x74,0x7C,0x38,0x00,0x00,
	0x80,0xC0,0xF0,0xFC,0xF0,0xC0,0x80,0x00,0x01,0x03,0x0F,0x3F,0x0F,0x03,0x01,0x00,
	0x18,0x3C,0x7E,0x18,0x7E,0x3C,0x18,0x00,0xEE,0xEE,0xEE,0xCC,0x00,0xCC,0xCC,0x00,
	0x00,0x00,0x30,0x68,0x78,0x30,0x00,0x00,0x00,0x38,0x64,0x74,0x7C,0x38,0x00,0x00,
	0x3C,0x66,0x7A,0x7A,0x7E,0x7E,0x3C,0x00,0x0E,0x3E,0x3A,0x22,0x26,0x6E,0xE4,0x40,
	0x18,0x3C,0x7E,0x3C,0x3C,0x3C,0x3C,0x00,0x3C,0x3C,0x3C,0x3C,0x7E,0x3C,0x18,0x00,
	0x08,0x7C,0x7E,0x7E,0x7C,0x08,0x00,0x00,0x10,0x3E,0x7E,0x7E,0x3E,0x10,0x00,0x00,
	0x58,0x2A,0xDC,0xC8,0xDC,0x2A,0x58,0x00,0x24,0x66,0xFF,0xFF,0x66,0x24,0x00,0x00,
	0x00,0x10,0x10,0x38,0x38,0x7C,0xFE,0x00,0xFE,0x7C,0x38,0x38,0x10,0x10,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x1C,0x1C,0x18,0x00,0x18,0x18,0x00,
	0x6C,0x6C,0x24,0x00,0x00,0x00,0x00,0x00,0x00,0x28,0x7C,0x28,0x7C,0x28,0x00,0x00,
	0x10,0x38,0x60,0x38,0x0C,0x78,0x10,0x00,0x40,0xA4,0x48,0x10,0x24,0x4A,0x04,0x00,
	0x18,0x34,0x18,0x3A,0x6C,0x66,0x3A,0x00,0x18,0x18,0x20,0x00,0x00,0x00,0x00,0x00,
	0x30,0x60,0x60,0x60,0x60,0x60,0x30,0x00,0x0C,0x06,0x06,0x06,0x06,0x06,0x0C,0x00,
	0x10,0x54,0x38,0x7C,0x38,0x54,0x10,0x00,0x00,0x18,0x18,0x7E,0x18,0x18,0x00,0x00,
	0x00,0x00,0x00,0x00,0x18,0x18,0x30,0x00,0x00,0x00,0x00,0x00,0x3E,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x04,0x08,0x10,0x20,0x40,0x00,0x00,
	0x38,0x4C,0xC6,0xC6,0xC6,0x64,0x38,0x00,0x18,0x38,0x18,0x18,0x18,0x18,0x7E,0x00,
	0x7C,0xC6,0x0E,0x3C,0x78,0xE0,0xFE,0x00,0x7E,0x0C,0x18,0x3C,0x06,0xC6,0x7C,0x00,
	0x1C,0x3C,0x6C,0xCC,0xFE,0x0C,0x0C,0x00,0xFC,0xC0,0xFC,0x06,0x06,0xC6,0x7C,0x00,
	0x3C,0x60,0xC0,0xFC,0xC6,0xC6,0x7C,0x00,0xFE,0xC6,0x0C,0x18,0x30,0x30,0x30,0x00,
	0x78,0xC4,0xE4,0x78,0x86,0x86,0x7C,0x00,0x7C,0xC6,0xC6,0x7E,0x06,0x0C,0x78,0x00,
	0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x18,0x00,0x00,0x18,0x18,0x30,
	0x1C,0x38,0x70,0xE0,0x70,0x38,0x1C,0x00,0x00,0x7C,0x00,0x00,0x7C,0x00,0x00,0x00,
	0x70,0x38,0x1C,0x0E,0x1C,0x38,0x70,0x00,0x7C,0xC6,0xC6,0x1C,0x18,0x00,0x18,0x00,
	0x3C,0x42,0x99,0xA1,0xA5,0x99,0x42,0x3C,0x38,0x6C,0xC6,0xC6,0xFE,0xC6,0xC6,0x00,
	0xFC,0xC6,0xC6,0xFC,0xC6,0xC6,0xFC,0x00,0x3C,0x66,0xC0,0xC0,0xC0,0x66,0x3C,0x00,
	0xF8,0xCC,0xC6,0xC6,0xC6,0xCC,0xF8,0x00,0xFE,0xC0,0xC0,0xFC,0xC0,0xC0,0xFE,0x00,
	0xFE,0xC0,0xC0,0xFC,0xC0,0xC0,0xC0,0x00,0x3E,0x60,0xC0,0xCE,0xC6,0x66,0x3E,0x00,
	0xC6,0xC6,0xC6,0xFE,0xC6,0xC6,0xC6,0x00,0x7E,0x18,0x18,0x18,0x18,0x18,0x7E,0x00,
	0x06,0x06,0x06,0x06,0xC6,0xC6,0x7C,0x00,0xC6,0xCC,0xD8,0xF0,0xF8,0xDC,0xCE,0x00,
	0x60,0x60,0x60,0x60,0x60,0x60,0x7E,0x00,0xC6,0xEE,0xFE,0xFE,0xD6,0xC6,0xC6,0x00,
	0xC6,0xE6,0xF6,0xFE,0xDE,0xCE,0xC6,0x00,0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00,
	0xFC,0xC6,0xC6,0xC6,0xFC,0xC0,0xC0,0x00,0x7C,0xC6,0xC6,0xC6,0xDE,0xCC,0x7A,0x00,
	0xFC,0xC6,0xC6,0xCE,0xF8,0xDC,0xCE,0x00,0x78,0xCC,0xC0,0x7C,0x06,0xC6,0x7C,0x00,
	0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x00,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0x7C,0x00,
	0xC6,0xC6,0xC6,0xEE,0x7C,0x38,0x10,0x00,0xC6,0xC6,0xD6,0xFE,0xFE,0xEE,0xC6,0x00,
	0xC6,0xEE,0x3C,0x38,0x7C,0xEE,0xC6,0x00,0x66,0x66,0x66,0x3C,0x18,0x18,0x18,0x00,
	0xFE,0x0E,0x1C,0x38,0x70,0xE0,0xFE,0x00,0x3C,0x30,0x30,0x30,0x30,0x30,0x3C,0x00,
	0x60,0x60,0x30,0x18,0x0C,0x06,0x06,0x00,0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00,
	0x18,0x3C,0x66,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
	0x30,0x30,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x3C,0x06,0x3E,0x66,0x66,0x3C,0x00,
	0x60,0x7C,0x66,0x66,0x66,0x66,0x7C,0x00,0x00,0x3C,0x66,0x60,0x60,0x66,0x3C,0x00,
	0x06,0x3E,0x66,0x66,0x66,0x66,0x3E,0x00,0x00,0x3C,0x66,0x66,0x7E,0x60,0x3C,0x00,
	0x1C,0x30,0x78,0x30,0x30,0x30,0x30,0x00,0x00,0x3E,0x66,0x66,0x66,0x3E,0x06,0x3C,
	0x60,0x7C,0x76,0x66,0x66,0x66,0x66,0x00,0x18,0x00,0x38,0x18,0x18,0x18,0x18,0x00,
	0x0C,0x00,0x1C,0x0C,0x0C,0x0C,0x0C,0x38,0x60,0x60,0x66,0x6C,0x78,0x6C,0x66,0x00,
	0x38,0x18,0x18,0x18,0x18,0x18,0x18,0x00,0x00,0xEC,0xFE,0xFE,0xFE,0xD6,0xC6,0x00,
	0x00,0x7C,0x76,0x66,0x66,0x66,0x66,0x00,0x00,0x3C,0x66,0x66,0x66,0x66,0x3C,0x00,
	0x00,0x7C,0x66,0x66,0x66,0x7C,0x60,0x60,0x00,0x3E,0x66,0x66,0x66,0x3E,0x06,0x06,
	0x00,0x7E,0x70,0x60,0x60,0x60,0x60,0x00,0x00,0x3C,0x60,0x3C,0x06,0x66,0x3C,0x00,
	0x30,0x78,0x30,0x30,0x30,0x30,0x1C,0x00,0x00,0x66,0x66,0x66,0x66,0x6E,0x3E,0x00,
	0x00,0x66,0x66,0x66,0x66,0x3C,0x18,0x00,0x00,0xC6,0xD6,0xFE,0xFE,0x7C,0x6C,0x00,
	0x00,0x66,0x3C,0x18,0x3C,0x66,0x66,0x00,0x00,0x66,0x66,0x66,0x66,0x3E,0x06,0x3C,
	0x00,0x7E,0x0C,0x18,0x30,0x60,0x7E,0x00,0x0E,0x18,0x0C,0x38,0x0C,0x18,0x0E,0x00,
	0x18,0x18,0x18,0x00,0x18,0x18,0x18,0x00,0x70,0x18,0x30,0x1C,0x30,0x18,0x70,0x00,
	0x00,0x00,0x76,0xDC,0x00,0x00,0x00,0x00,0x10,0x28,0x10,0x54,0xAA,0x44,0x00,0x00,
};

static void pnd_text(unsigned short *scr, int x, int y, char *text, int color)
{
	unsigned int i,l;
	scr=scr+x+y*vinfo.xres;

	for (i=0;i<strlen(text);i++) {
		
		for (l=0;l<8;l++) {
			scr[l*vinfo.xres+0]=(fontdata8x8[((text[i])*8)+l]&0x80)?pnd_palette_rgb[color]:scr[l*vinfo.xres+0];
			scr[l*vinfo.xres+1]=(fontdata8x8[((text[i])*8)+l]&0x40)?pnd_palette_rgb[color]:scr[l*vinfo.xres+1];
			scr[l*vinfo.xres+2]=(fontdata8x8[((text[i])*8)+l]&0x20)?pnd_palette_rgb[color]:scr[l*vinfo.xres+2];
			scr[l*vinfo.xres+3]=(fontdata8x8[((text[i])*8)+l]&0x10)?pnd_palette_rgb[color]:scr[l*vinfo.xres+3];
			scr[l*vinfo.xres+4]=(fontdata8x8[((text[i])*8)+l]&0x08)?pnd_palette_rgb[color]:scr[l*vinfo.xres+4];
			scr[l*vinfo.xres+5]=(fontdata8x8[((text[i])*8)+l]&0x04)?pnd_palette_rgb[color]:scr[l*vinfo.xres+5];
			scr[l*vinfo.xres+6]=(fontdata8x8[((text[i])*8)+l]&0x02)?pnd_palette_rgb[color]:scr[l*vinfo.xres+6];
			scr[l*vinfo.xres+7]=(fontdata8x8[((text[i])*8)+l]&0x01)?pnd_palette_rgb[color]:scr[l*vinfo.xres+7];
		}
		scr+=8;
	} 
}

void pnd_gamelist_text_out(int x, int y, char *eltexto)
{
	char texto[43];
	strncpy(texto,eltexto,42);
	texto[42]=0;
	if (texto[0]!='-')
		pnd_text(pnd_screen15,x+1,y+1,texto,0);
	pnd_text(pnd_screen15,x,y,texto,255);
}

/* Variadic functions guide found at http://www.unixpapa.com/incnote/variadic.html */
void pnd_gamelist_text_out_fmt(int x, int y, char* fmt, ...)
{
	char strOut[128];
	va_list marker;
	
	va_start(marker, fmt);
	vsprintf(strOut, fmt, marker);
	va_end(marker);	

	pnd_gamelist_text_out(x, y, strOut);
}

static int log=0;

void pnd_printf_init(void)
{
	log=0;
}

static void pnd_text_log(char *texto)
{
	if (!log)
	{
		memset(pnd_screen15,0,vinfo.xres*vinfo.yres);
	}
	pnd_text(pnd_screen15,0,log,texto,255);
	log+=8;
	if(log>239) log=0;
}

/* Variadic functions guide found at http://www.unixpapa.com/incnote/variadic.html */
void pnd_printf(char* fmt, ...)
{
	int i,c;
	char strOut[4096];
	char str[41];
	va_list marker;
	
	va_start(marker, fmt);
	vsprintf(strOut, fmt, marker);
	va_end(marker);	

	c=0;
	for (i=0;i<strlen(strOut);i++)
	{
		str[c]=strOut[i];
		if (str[c]=='\n')
		{
			str[c]=0;
			pnd_text_log(str);
			c=0;
		}
		else if (c==39)
		{
			str[40]=0;
			pnd_text_log(str);
			c=0;
		}		
		else
		{
			c++;
		}
	}
}

