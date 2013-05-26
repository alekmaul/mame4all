#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include "minimal.h"
#include "pnd_frontend_list.h"

#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 240
#define BMP_SIZE ((SCREEN_WIDTH*SCREEN_HEIGHT)+(256*4)+54)

static unsigned char splash_bmp[BMP_SIZE];
static unsigned char menu_bmp[BMP_SIZE];

int game_num_avail=0;
static int last_game_selected=0;
char playemu[16] = "mame\0";
char playgame[16] = "builtinn\0";
char *rompath = NULL;
char *samplepath = NULL;

int pnd_freq=200;
int pnd_video_depth=16;
int pnd_video_aspect=1;
int pnd_video_rotate=0;
int pnd_video_filter=-1;
int pnd_video_sync=1;
int pnd_frameskip=-1;
int pnd_sound = 14;
int pnd_volume = 4;
int pnd_clock_cpu=100;
int pnd_clock_sound=100;
int pnd_cpu_cores=1;
int pnd_ramtweaks=1;
int pnd_cheat=0;

int master_volume = 100;

static void blit_bmp_8bpp(unsigned short *out, unsigned char *in) 
{
	int i,x,y;
	unsigned char r,g,b,c;
	in+=14; /* Skip HEADER */
	in+=40; /* Skip INFOHD */
	/* Set Palette */
	for (i=0;i<256;i++) {
		b=*in++;
		g=*in++;
		r=*in++;
		c=*in++;
		pnd_video_color8(i,r,g,b);
	}
	pnd_video_setpalette();
	/* Set Bitmap */	
	for (y=SCREEN_HEIGHT-1;y!=-1;y--) {
		for (x=0;x<SCREEN_WIDTH;x++) {
			*out++=pnd_palette_rgb[in[x+y*SCREEN_WIDTH]];
		}
	}
}

static void pnd_intro_screen(void) {
	char name[256];
	FILE *f;

	sprintf(name,"gfx/splash.bmp");
	f=fopen(name,"rb");
	if (f) {
		fread(splash_bmp,1,BMP_SIZE,f);
		fclose(f);
	}
	blit_bmp_8bpp(pnd_screen15,splash_bmp);
	pnd_video_flip();
	sprintf(name,"gfx/menu.bmp");
	f=fopen(name,"rb");
	if (f) {
		fread(menu_bmp,1,BMP_SIZE,f);
		fclose(f);
	}
}

static void game_list_init_nocache(void)
{
	int i;
	FILE *f;
	DIR *d=opendir(rompath?rompath:"roms");
	char game[32];
	if (d)
	{
		struct dirent *actual=readdir(d);
		while(actual)
		{
			for (i=0;i<NUMGAMES;i++)
			{
				if (drivers[i].available==0)
				{
					sprintf(game,"%s.zip",drivers[i].name);
					if (strcmp(actual->d_name,game)==0)
					{
						drivers[i].available=1;
						game_num_avail++;
						break;
					}
				}
			}
			actual=readdir(d);
		}
		closedir(d);
	}
	
	if (game_num_avail)
	{
		remove("frontend/mame.lst");
		sync();
		f=fopen("frontend/mame.lst","w");
		if (f)
		{
			for (i=0;i<NUMGAMES;i++)
			{
				fputc(drivers[i].available,f);
			}
			fclose(f);
			sync();
		}
	}
}

static void game_list_init_cache(void)
{
	FILE *f;
	int i;
	f=fopen("frontend/mame.lst","r");
	if (f)
	{
		for (i=0;i<NUMGAMES;i++)
		{
			drivers[i].available=fgetc(f);
			if (drivers[i].available)
				game_num_avail++;
		}
		fclose(f);
	}
	else
		game_list_init_nocache();
}

static void game_list_init(int argc)
{
	if (argc==1)
		game_list_init_nocache();
	else
		game_list_init_cache();
}

static void game_list_view(int *pos) {

	int i;
	int view_pos;
	int aux_pos=0;
	int screen_y = 45;
	int screen_x = 38;

	/* Draw background image */
	blit_bmp_8bpp(pnd_screen15,menu_bmp);

	/* Check Limits */
	if (*pos<0)
		*pos=game_num_avail-1;
	if (*pos>(game_num_avail-1))
		*pos=0;
					   
	/* Set View Pos */
	if (*pos<10) {
		view_pos=0;
	} else {
		if (*pos>game_num_avail-11) {
			view_pos=game_num_avail-21;
			view_pos=(view_pos<0?0:view_pos);
		} else {
			view_pos=*pos-10;
		}
	}

	/* Show List */
	for (i=0;i<NUMGAMES;i++) {
		if (drivers[i].available==1) {
			if (aux_pos>=view_pos && aux_pos<=view_pos+20) {
				pnd_gamelist_text_out( screen_x, screen_y, drivers[i].description);
				if (aux_pos==*pos) {
					pnd_gamelist_text_out( screen_x-10, screen_y,">" );
					pnd_gamelist_text_out( screen_x-13, screen_y-1,"-" );
				}
				screen_y+=8;
			}
			aux_pos++;
		}
	}
}

static void game_list_select (int index, char *game, char *emu) {
	int i;
	int aux_pos=0;
	for (i=0;i<NUMGAMES;i++)
	{
		if (drivers[i].available==1)
		{
			if(aux_pos==index)
			{
				strcpy(game,drivers[i].name);
				strcpy(emu,drivers[i].exe);
				pnd_cpu_cores=drivers[i].cores;
				break;
			}
			aux_pos++;
		}
	}
}

static char *game_list_description (int index)
{
	int i;
	int aux_pos=0;
	for (i=0;i<NUMGAMES;i++) {
		if (drivers[i].available==1) {
			if(aux_pos==index) {
				return(drivers[i].description);
			}
			aux_pos++;
		   }
	}
	return ((char *)0);
}

static int show_options(char *game)
{
	unsigned long ExKey=0;
	int selected_option=0;
	int x_Pos = 41;
	int y_PosTop = 58;
	int y_Pos = y_PosTop;
	int options_count = 10;
	char text[256];
	FILE *f;
	int i=0;

	/* Read game configuration */
	sprintf(text,"frontend/%s.cfg",game);
	f=fopen(text,"r");
	if (f) {
		fscanf(f,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",&pnd_freq,&pnd_video_depth,&pnd_video_aspect,&pnd_video_sync,
		&pnd_frameskip,&pnd_sound,&pnd_clock_cpu,&pnd_clock_sound,&pnd_cpu_cores,&pnd_ramtweaks,&i,&pnd_cheat,&pnd_volume,
		&pnd_video_rotate,&pnd_video_filter);
		fclose(f);
	}

	while(1)
	{
		y_Pos = y_PosTop;
		
		/* Draw background image */
		blit_bmp_8bpp(pnd_screen15,menu_bmp);

		/* Draw the options */
		strncpy (text,game_list_description(last_game_selected),33);
		text[32]='\0';
		pnd_gamelist_text_out(x_Pos,y_Pos-10,text);

		/* Video Aspect */
		y_Pos += 10;
		switch (pnd_video_aspect)
		{
			case 0: pnd_gamelist_text_out_fmt(x_Pos,y_Pos,"Video Aspect    Normal"); break;
			case 1: pnd_gamelist_text_out_fmt(x_Pos,y_Pos,"Video Aspect    Scale"); break;
			case 2: pnd_gamelist_text_out_fmt(x_Pos,y_Pos,"Video Aspect    4:3"); break;
			case 3: pnd_gamelist_text_out_fmt(x_Pos,y_Pos,"Video Aspect    Stretch"); break;
			case 4: pnd_gamelist_text_out_fmt(x_Pos,y_Pos,"Video Aspect    Fit"); break;
		}

		/* Video Rotation */
		y_Pos += 10;
		switch (pnd_video_rotate)
		{
			case 0:  pnd_gamelist_text_out_fmt(x_Pos,y_Pos,"Video Rotation  OFF"); break;
			case 1:  pnd_gamelist_text_out_fmt(x_Pos,y_Pos,"Video Rotation  ON"); break;
		}

		/* Video Filter */
		y_Pos += 10;
		switch (pnd_video_filter)
		{
			case -1: pnd_gamelist_text_out_fmt(x_Pos,y_Pos,"Video Filter    Auto"); break;
			case 0:  pnd_gamelist_text_out_fmt(x_Pos,y_Pos,"Video Filter    None"); break;
			case 1:  pnd_gamelist_text_out_fmt(x_Pos,y_Pos,"Video Filter    Default"); break;
		}
		
		/* Video Sync */
		y_Pos += 10;
		switch (pnd_video_sync)
		{
			case 0: pnd_gamelist_text_out(x_Pos,y_Pos, "Video Sync      Normal"); break;
			case 1: pnd_gamelist_text_out(x_Pos,y_Pos, "Video Sync      VSync"); break;
			case 2: pnd_gamelist_text_out(x_Pos,y_Pos, "Video Sync      DblBuf"); break;
			case -1: pnd_gamelist_text_out(x_Pos,y_Pos,"Video Sync      OFF"); break;
		}
		
		/* Frame-Skip */
		y_Pos += 10;
		if ((pnd_video_sync==-1) && (pnd_frameskip==-1)) pnd_frameskip=0;
		if(pnd_frameskip==-1) {
			pnd_gamelist_text_out_fmt(x_Pos,y_Pos, "Frame-Skip      Auto");
		}
		else{
			pnd_gamelist_text_out_fmt(x_Pos,y_Pos,"Frame-Skip      %d",pnd_frameskip);
		}

		/* Sound */
		y_Pos += 10;
		switch(pnd_sound)
		{
			case 0:  pnd_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound           %s","OFF"); break;
			case 1:  pnd_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound           %s","ON (15 KHz fast)"); break;
			case 2:  pnd_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound           %s","ON (22 KHz fast)"); break;
			case 3:  pnd_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound           %s","ON (33 KHz fast)"); break;
			case 4:  pnd_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound           %s","ON (44 KHz fast)"); break;
			case 5:  pnd_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound           %s","ON (11 KHz fast)"); break;
			case 6:  pnd_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound           %s","ON (15 KHz mono)"); break;
			case 7:  pnd_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound           %s","ON (22 KHz mono)"); break;
			case 8:  pnd_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound           %s","ON (33 KHz mono)"); break;
			case 9:  pnd_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound           %s","ON (44 KHz mono)"); break;
			case 10: pnd_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound           %s","ON (11 KHz mono)"); break;
			case 11: pnd_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound           %s","ON (15 KHz stereo)"); break;
			case 12: pnd_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound           %s","ON (22 KHz stereo)"); break;
			case 13: pnd_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound           %s","ON (33 KHz stereo)"); break;
			case 14: pnd_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound           %s","ON (44 KHz stereo)"); break;
			case 15: pnd_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound           %s","ON (11 KHz stereo)"); break;
		}

		/* CPU Clock */
		y_Pos += 10;
		pnd_gamelist_text_out_fmt(x_Pos,y_Pos,"CPU Clock       %d%%",pnd_clock_cpu);

		/* Audio Clock */
		y_Pos += 10;
		pnd_gamelist_text_out_fmt(x_Pos,y_Pos,"Audio Clock     %d%%",pnd_clock_sound);

		/* CPU cores */
		y_Pos += 10;
		switch (pnd_cpu_cores)
		{
			case 0: pnd_gamelist_text_out(x_Pos,y_Pos, "CPU ASM cores   None"); break;
			case 1: pnd_gamelist_text_out(x_Pos,y_Pos, "CPU ASM cores   Cyclone"); break;
			case 2: pnd_gamelist_text_out(x_Pos,y_Pos, "CPU ASM cores   DrZ80"); break;
			case 3: pnd_gamelist_text_out(x_Pos,y_Pos, "CPU ASM cores   Cyclone+DrZ80"); break;
			case 4: pnd_gamelist_text_out(x_Pos,y_Pos, "CPU ASM cores   DrZ80(snd)"); break;
			case 5: pnd_gamelist_text_out(x_Pos,y_Pos, "CPU ASM cores   Cyclone+DrZ80(snd)"); break;
		}

		/* Cheats */
		y_Pos += 10;
		if (pnd_cheat)
			pnd_gamelist_text_out(x_Pos,y_Pos,"Cheats          ON");
		else
			pnd_gamelist_text_out(x_Pos,y_Pos,"Cheats          OFF");

        /* Volume */
        y_Pos += 10;
        /* Does nothing on Pandora right now...
        if (pnd_sound == 0)
        {
            pnd_gamelist_text_out(x_Pos,y_Pos,"Volume          Disabled");
        }
        else
        {
            switch (pnd_volume)
            {
                case 1: pnd_gamelist_text_out(x_Pos,y_Pos,"Volume          Quiet"); break;
                case 2: pnd_gamelist_text_out(x_Pos,y_Pos,"Volume          Low"); break;
                case 3: pnd_gamelist_text_out(x_Pos,y_Pos,"Volume          Medium"); break;
                case 4: pnd_gamelist_text_out(x_Pos,y_Pos,"Volume          Maximum"); break;
            }
        }
        */
	
		y_Pos += 30;
		pnd_gamelist_text_out(x_Pos,y_Pos,"Press B to confirm, X to return\0");

		/* Show currently selected item */
		pnd_gamelist_text_out(x_Pos-16,y_PosTop+(selected_option*10)+10," >");

		pnd_video_flip();
		while(pnd_joystick_read(0)&0x8c0ff55) { pnd_timer_delay(150); }
		while(!(ExKey=pnd_joystick_read(0)&0x8c0ff55)) { }
		
		/* Map left/right to L/R */
		if(ExKey & PND_LEFT)  ExKey |= PND_L;
		if(ExKey & PND_RIGHT) ExKey |= PND_R;

		if(ExKey & PND_DOWN){
			selected_option++;
			selected_option = selected_option % options_count;
		}
		else if(ExKey & PND_UP){
			selected_option--;
			if(selected_option<0)
				selected_option = options_count - 1;
		}
		else if(ExKey & PND_R || ExKey & PND_L)
		{
			switch(selected_option) {
/*			case 0:
				switch (pnd_video_depth)
				{
					case -1: pnd_video_depth=8; break;
					case 8: pnd_video_depth=16; break;
					case 16: pnd_video_depth=-1; break;
				}
				break; */
			case 0:
				if(ExKey & PND_R)
				{
					pnd_video_aspect++;
					if (pnd_video_aspect>4)
						pnd_video_aspect=0;
				}
				else
				{
					pnd_video_aspect--;
					if (pnd_video_aspect<0)
						pnd_video_aspect=4;
				}
				break;
			case 1:
				if( pnd_video_rotate )
					pnd_video_rotate = 0;
				else
					pnd_video_rotate = 1;
				break;
			case 2:
				if(ExKey & PND_R) {
					pnd_video_filter++;
					if (pnd_video_filter>1)
						pnd_video_filter=-1;
				}
				else
				{
					pnd_video_filter--;
					if (pnd_video_filter<-1)
						pnd_video_filter=1;				
				}
				break;
			case 3:
				if(ExKey & PND_R) {
					pnd_video_sync++;
					if (pnd_video_sync>2)
						pnd_video_sync=-1;
					break;
				}
				else
				{
					pnd_video_sync--;
					if (pnd_video_sync<-1)
						pnd_video_sync=2;
					break;				
				}
			case 4:
				/* "Frame-Skip" */
				if(ExKey & PND_R)
				{
					pnd_frameskip ++;
					if (pnd_frameskip>11)
						pnd_frameskip=-1;
				}
				else
				{
					pnd_frameskip--;
					if (pnd_frameskip<-1)
						pnd_frameskip=11;
				}
				break;
			case 5:
				if(ExKey & PND_R)
				{
					pnd_sound ++;
					if (pnd_sound>15)
						pnd_sound=0;
				}
				else
				{
					pnd_sound--;
					if (pnd_sound<0)
						pnd_sound=15;
				}
				break;
			case 6:
				/* "CPU Clock" */
				if(ExKey & PND_R)
				{
					pnd_clock_cpu += 10; /* Add 10% */
					if (pnd_clock_cpu > 200) /* 200% is the max */
						pnd_clock_cpu = 200;
				}
				else
				{
					pnd_clock_cpu -= 10; /* Subtract 10% */
					if (pnd_clock_cpu < 10) /* 10% is the min */
						pnd_clock_cpu = 10;
				}
				break;
			case 7:
				/* "Audio Clock" */
				if(ExKey & PND_R)
				{
					pnd_clock_sound += 10; /* Add 10% */
					if (pnd_clock_sound > 200) /* 200% is the max */
						pnd_clock_sound = 200;
				}
				else{
					pnd_clock_sound -= 10; /* Subtract 10% */
					if (pnd_clock_sound < 10) /* 10% is the min */
						pnd_clock_sound = 10;
				}
				break;
			case 8:
				pnd_cpu_cores=(pnd_cpu_cores+1)%6;
				break;
			case 9:
				pnd_cheat=!pnd_cheat;
				break;
			/* case 10: */
                /* Volume */
                /*
                if(ExKey & PND_R)
                {
                    pnd_volume++;
                    if (pnd_volume > 4)
                        pnd_volume = 1;
                }
                else {
                    pnd_volume--;
                    if (pnd_volume < 1)
                        pnd_volume = 4;
                }
                break;
                */
			}
		}

		if ((ExKey & PND_A) || (ExKey & PND_B) || (ExKey & PND_START)) 
		{
			/* Write game configuration */
			sprintf(text,"frontend/%s.cfg",game);
			f=fopen(text,"w");
			if (f) {
				fprintf(f,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",pnd_freq,pnd_video_depth,pnd_video_aspect,pnd_video_sync,
				pnd_frameskip,pnd_sound,pnd_clock_cpu,pnd_clock_sound,pnd_cpu_cores,pnd_ramtweaks,i,pnd_cheat,pnd_volume,
				pnd_video_rotate,pnd_video_filter);
				fclose(f);
				sync();
			}

			/* Selected game will be run */
			return 1;
		}
		else if ((ExKey & PND_X) || (ExKey & PND_Y) || (ExKey & PND_SELECT))
		{
			/* Return To Menu */
			return 0;
		}
	}
}

static void pnd_exit(void)
{
	remove("frontend/mame.lst");
	sync();
	pnd_deinit();
}

static void select_game(char *emu, char *game)
{

	unsigned long ExKey;

	/* No Selected game */
	strcpy(game,"builtinn");

	/* Clean screen */
	pnd_video_flip();

	/* Wait until user selects a game */
	while(1)
	{
		game_list_view(&last_game_selected);
		pnd_video_flip();

		if( (pnd_joystick_read(0)&0x8c0ff55)) 
			pnd_timer_delay(100); 
		while(!(ExKey=pnd_joystick_read(0)&0x8c0ff55))
		{ 
			if ((ExKey & PND_L) && (ExKey & PND_R)) { pnd_exit(); }
		}

		if (ExKey & PND_UP) last_game_selected--;
		if (ExKey & PND_DOWN) last_game_selected++;
		if (ExKey & PND_L || ExKey & PND_LEFT) last_game_selected-=21;
		if (ExKey & PND_R || ExKey & PND_RIGHT) last_game_selected+=21;
		if ((ExKey & PND_L) && (ExKey & PND_R)) pnd_exit();

		if ((ExKey & PND_A) || (ExKey & PND_B) || (ExKey & PND_START))
		{
			/* Select the game */
			game_list_select(last_game_selected, game, emu);

			/* Emulation Options */
			if(show_options(game))
			{
				break;
			}
		}
	}
}

void execute_game (char *playemu, char *playgame)
{
	char *args[255];
	char str[8][64];
	int n=0;
	int i=0;
	
	/* executable */
	args[n]=playemu; n++;

	/* playgame */
	args[n]=playgame; n++;

	/* pnd_freq */
	args[n]="-clock"; n++;
	sprintf(str[i],"%d",pnd_freq);
	args[n]=str[i]; i++; n++;

	/* pnd_video_depth */
#ifdef PANDORA_FB_DRIVER_CAN_DO_8BPP
	if (pnd_video_depth==8)
	{
		args[n]="-depth"; n++;
		args[n]="8"; n++;
	}
	if (pnd_video_depth==16)
	{
		args[n]="-depth"; n++;
		args[n]="16"; n++;
	}
#else
	args[n]="-depth"; n++;
	args[n]="16"; n++;
#endif

	/* pnd_video_aspect */
	switch(pnd_video_aspect)
	{
		case 1: args[n]="-scale";   n++; break;
		case 2: args[n]="-aspect";  n++; break;
		case 3: args[n]="-stretch"; n++; break;
		case 4: args[n]="-fit"; n++; break;
		default: break;
	}
	
	if (pnd_video_rotate)
	{
		args[n]="-ror"; n++;
		args[n]="-rotatecontrols"; n++;
	}
	
	switch(pnd_video_filter)
	{
		case 0:
			args[n]="-filter"; n++;
			args[n]="none"; n++;
			break;
		case 1:
			args[n]="-filter"; n++;
			args[n]="default"; n++;
			break;
		default:
			/* Automatic */
			break;
	}
	
	/* pnd_video_sync */
	if (pnd_video_sync==1)
	{
		args[n]="-nodirty"; n++;
		args[n]="-waitvsync"; n++;
	}
	else if (pnd_video_sync==2)
	{
		args[n]="-nodirty"; n++;
	}
	else if (pnd_video_sync==-1)
	{
		args[n]="-nothrottle"; n++;
	}
	
	/* pnd_frameskip */
	if (pnd_frameskip>=0)
	{
		args[n]="-frameskip"; n++;
		sprintf(str[i],"%d",pnd_frameskip);
		args[n]=str[i]; i++; n++;
	}

	/* pnd_sound */
	if (pnd_sound==0)
	{
		args[n]="-soundcard"; n++;
		args[n]="0"; n++;
	}
	if ((pnd_sound==1) || (pnd_sound==6) || (pnd_sound==11))
	{
		args[n]="-samplerate"; n++;
		args[n]="16000"; n++;
	}
	if ((pnd_sound==2) || (pnd_sound==7) || (pnd_sound==12))
	{
		args[n]="-samplerate"; n++;
		args[n]="22050"; n++;
	}
	if ((pnd_sound==3) || (pnd_sound==8) || (pnd_sound==13))
	{
		args[n]="-samplerate"; n++;
		args[n]="32000"; n++;
	}
	if ((pnd_sound==4) || (pnd_sound==9) || (pnd_sound==14))
	{
		args[n]="-samplerate"; n++;
		args[n]="44100"; n++;
	}
	if ((pnd_sound==5) || (pnd_sound==10) || (pnd_sound==15))
	{
		args[n]="-samplerate"; n++;
		args[n]="11025"; n++;
	}
	if ((pnd_sound>=1) && (pnd_sound<=5))
	{
		args[n]="-fastsound"; n++;
	}
	if (pnd_sound>=11)
	{
		args[n]="-stereo"; n++;
	}

	/* pnd_clock_cpu */
	if (pnd_clock_cpu!=100)
	{
		args[n]="-uclock"; n++;
		sprintf(str[i],"%d",100-pnd_clock_cpu);
		args[n]=str[i]; i++; n++;
	}

	/* pnd_clock_sound */
	if (pnd_clock_cpu!=100)
	{
		args[n]="-uclocks"; n++;
		sprintf(str[i],"%d",100-pnd_clock_sound);
		args[n]=str[i]; i++; n++;
	}
	
	/* pnd_cpu_cores */
	if ((pnd_cpu_cores==1) || (pnd_cpu_cores==3) || (pnd_cpu_cores==5))
	{
		args[n]="-cyclone"; n++;
	}
	if ((pnd_cpu_cores==2) || (pnd_cpu_cores==3))
	{
		args[n]="-drz80"; n++;
	}
	if ((pnd_cpu_cores==4) || (pnd_cpu_cores==5))
	{
		args[n]="-drz80_snd"; n++;
	}

	if (pnd_ramtweaks)
	{
		args[n]="-ramtweaks"; n++;
	}
	
	if (pnd_cheat)
	{
		args[n]="-cheat"; n++;
	}

	switch (pnd_volume)
	{
		case 3: args[n]="-volume"; n++; args[n]="-4"; n++; break;
		case 2: args[n]="-volume"; n++; args[n]="-8"; n++; break;
		case 1: args[n]="-volume"; n++; args[n]="-10"; n++; break;
	    default: break; /* nothing, default to maximum volume */
	}

	if( rompath ) {
		args[n]="-rompath"; n++;
		args[n]=rompath; n++;
	}

	if( samplepath ) {
		args[n]="-samplepath"; n++;
		args[n]=samplepath; n++;
	}

	args[n]=NULL;
	
	for (i=0; i<n; i++)
	{
		printf("%s ",args[i]);
	}
	printf("\n");
	
	pnd_deinit();
	execv(args[0], args); 
}


int main (int argc, char **argv)
{
	FILE *f;

	rompath = getenv("ROMPATH");
	if( rompath && !*rompath ) rompath = NULL;
	
	samplepath = getenv("SAMPLEPATH");
	if( samplepath && !*samplepath ) samplepath = NULL;

	printf("ROMS: %s\n", rompath?rompath:"<default>");
	printf("Samples: %s\n", samplepath?samplepath:"<default>");

	/* Nearest neighbour filter for front end */
	pnd_fir_filter_set( PND_FIR_FILTER_NONE );
	
	/* Initialization */
	pnd_init(1000,8,22050,16,0,60);

	/* Show intro screen */
	pnd_intro_screen();

	/* Initialize list of available games */
	game_list_init(argc);
	if (game_num_avail==0)
	{
		pnd_gamelist_text_out(35, 110, "ERROR: NO AVAILABLE GAMES FOUND");
		pnd_video_flip();
		pnd_joystick_press(0);
		pnd_exit();
	}

	/* Read default configuration */
	f=fopen("frontend/mame.cfg","r");
	if (f) {
		fscanf(f,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",&pnd_freq,&pnd_video_depth,&pnd_video_aspect,&pnd_video_sync,
		&pnd_frameskip,&pnd_sound,&pnd_clock_cpu,&pnd_clock_sound,&pnd_cpu_cores,&pnd_ramtweaks,&last_game_selected,
		&pnd_cheat,&pnd_volume,&pnd_video_rotate,&pnd_video_filter);
		fclose(f);
	}
	
	/* Select Game */
	select_game(playemu,playgame); 

	/* Write default configuration */
	f=fopen("frontend/mame.cfg","w");
	if (f) {
		fprintf(f,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",pnd_freq,pnd_video_depth,pnd_video_aspect,pnd_video_sync,
		pnd_frameskip,pnd_sound,pnd_clock_cpu,pnd_clock_sound,pnd_cpu_cores,pnd_ramtweaks,last_game_selected,
		pnd_cheat,pnd_volume,pnd_video_rotate,pnd_video_filter);
		fclose(f);
		sync();
	}
	
	/* Execute Game */
	execute_game (playemu,playgame);
	
	exit (0);
}

