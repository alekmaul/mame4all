#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#include "minimal.h"

#include "odx_frontend_list.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define BMP_SIZE ((SCREEN_WIDTH*SCREEN_HEIGHT)+(256*4)+54)

#define COMPATCORES 1

static unsigned char splash_bmp[BMP_SIZE];
static unsigned char menu_bmp[BMP_SIZE];

int game_num_avail=0;
static int last_game_selected=0;
char playemu[16] = "mame\0";
char playgame[16] = "builtinn\0";

int odx_freq=336;       /* default dingoo Mhz */ 
int odx_video_depth=16; /* MAME video depth */
int odx_video_aspect=2; /* Scale best*/
int odx_video_sync=0;   /* No vsync */
int odx_frameskip=-1;
int odx_sound = 2;
int odx_volume = 3;
int odx_clock_cpu=100;
int odx_clock_sound=100;
int odx_cpu_cores=0;
int odx_ramtweaks=0;
int odx_cheat=0;
int odx_gsensor=0;

int master_volume = 100;

static void blit_bmp_8bpp(unsigned char *out, unsigned char *in) 
{
//	SDL_FillRect( layer, NULL, 0 );
	SDL_RWops *rw = SDL_RWFromMem(in, BMP_SIZE);
	SDL_Surface *temp = SDL_LoadBMP_RW(rw, 1);
	SDL_Surface *image;
	image = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);
	
	// Display image
 	//SDL_BlitSurface(image, 0, layer, 0);
	SDL_BlitSurface(image, 0, video, 0);
	SDL_FreeSurface(image);
}

static void odx_intro_screen(void) {
	char name[256];
	FILE *f;
	odx_video_flip();
	sprintf(name,"skins/splash.bmp");
	f=fopen(name,"rb");
	if (f) {
		fread(splash_bmp,1,77878,f);
		fclose(f);
	}
	blit_bmp_8bpp(od_screen8,splash_bmp);
	odx_video_flip();
	sprintf(name,"skins/menu.bmp");
	f=fopen(name,"rb");
	if (f) {
		fread(menu_bmp,1,77878,f);
		fclose(f);
	}
}

static void game_list_init_nocache(void)
{
	int i;
	FILE *f;
	DIR *d=opendir("roms");
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
		/* sync(); */
		f=fopen("frontend/mame.lst","w");
		if (f)
		{
			for (i=0;i<NUMGAMES;i++)
			{
				fputc(drivers[i].available,f);
			}
			fclose(f);
			/* sync(); */
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
	blit_bmp_8bpp(od_screen8,menu_bmp);

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
				odx_gamelist_text_out( screen_x, screen_y, drivers[i].description);
				if (aux_pos==*pos) {
					odx_gamelist_text_out( screen_x-10, screen_y,">" );
					odx_gamelist_text_out( screen_x-13, screen_y-1,"-" );
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
				//odx_cpu_cores=drivers[i].cores & 1;
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
	int options_count = 9;
	char text[256];
	FILE *f;
	int i=0;

	/* Read game configuration */
	sprintf(text,"frontend/%s.cfg",game);
	f=fopen(text,"r");
	if (f) {
		fscanf(f,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",&odx_freq,&odx_video_depth,&odx_video_aspect,&odx_video_sync,
		&odx_frameskip,&odx_sound,&odx_clock_cpu,&odx_clock_sound,&odx_cpu_cores,&odx_ramtweaks,&i,&odx_cheat,&odx_gsensor);
		fclose(f);
	}

	while(1)
	{
		int y_Pos = y_PosTop;
	
		/* Draw background image */
		blit_bmp_8bpp(od_screen8,menu_bmp);

		/* Draw the options */
		strncpy (text,game_list_description(last_game_selected),33);
		text[32]='\0';
		odx_gamelist_text_out(x_Pos,y_Pos-10,text);

		/* (0) Video Depth */
		y_Pos += 10;
		switch (odx_video_depth)
		{
			case -1: odx_gamelist_text_out_fmt(x_Pos,y_Pos,"Video Depth    Auto"); break;
			case 8:  odx_gamelist_text_out_fmt(x_Pos,y_Pos,"Video Depth    8 bit"); break;
			case 16: odx_gamelist_text_out_fmt(x_Pos,y_Pos,"Video Depth    16 bit"); break;
		}
		
		/* (1) Video Aspect */
		y_Pos += 10;
		switch (odx_video_aspect)
		{
			case 0: odx_gamelist_text_out_fmt(x_Pos,y_Pos,"Video Aspect   Normal"); break;
			case 1: odx_gamelist_text_out_fmt(x_Pos,y_Pos,"Video Aspect   Scale Horizontal"); break;
			case 2: odx_gamelist_text_out_fmt(x_Pos,y_Pos,"Video Aspect   Scale Best"); break;
			case 3: odx_gamelist_text_out_fmt(x_Pos,y_Pos,"Video Aspect   Scale Fast"); break;
			case 4: odx_gamelist_text_out_fmt(x_Pos,y_Pos,"Video Aspect   Scale Halfsize"); break;
			case 5: odx_gamelist_text_out_fmt(x_Pos,y_Pos,"Video Aspect   Rotate Normal"); break;
			case 6: odx_gamelist_text_out_fmt(x_Pos,y_Pos,"Video Aspect   Rotate Scale Horiz"); break;
			case 7: odx_gamelist_text_out_fmt(x_Pos,y_Pos,"Video Aspect   Rotate Best"); break;
			case 8: odx_gamelist_text_out_fmt(x_Pos,y_Pos,"Video Aspect   Rotate Fast"); break;
			case 9: odx_gamelist_text_out_fmt(x_Pos,y_Pos,"Video Aspect   Rotate Halfsize"); break;
		}
		
		/* (2) Video Sync */
		y_Pos += 10;
		switch (odx_video_sync)
		{
			case 1: odx_gamelist_text_out(x_Pos,y_Pos, "Video Sync     VSync"); break;
			case 0: odx_gamelist_text_out(x_Pos,y_Pos, "Video Sync     Normal"); break;
			case 2: odx_gamelist_text_out(x_Pos,y_Pos, "Video Sync     DblBuf"); break;
			case -1: odx_gamelist_text_out(x_Pos,y_Pos,"Video Sync     OFF"); break;
		}
		
		/* (3) Frame-Skip */
		y_Pos += 10;
		if ((odx_video_sync==-1) && (odx_frameskip==-1)) odx_frameskip=0;
		if(odx_frameskip==-1) {
			odx_gamelist_text_out_fmt(x_Pos,y_Pos, "Frame-Skip     Auto");
		}
		else{
			odx_gamelist_text_out_fmt(x_Pos,y_Pos,"Frame-Skip     %d",odx_frameskip);
		}

		/* (4) Sound */
		y_Pos += 10;
		switch(odx_sound)
		{
			case 0: odx_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound          %s","OFF"); break;
			case 1: odx_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound          %s","ON (15 KHz fast)"); break;
			case 2: odx_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound          %s","ON (22 KHz fast)"); break;
			case 3: odx_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound          %s","ON (33 KHz fast)"); break;
			case 4: odx_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound          %s","ON (44 KHz fast)"); break;
			case 5: odx_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound          %s","ON (11 KHz fast)"); break;
			case 6: odx_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound          %s","ON (15 KHz)"); break;
			case 7: odx_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound          %s","ON (22 KHz)"); break;
			case 8: odx_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound          %s","ON (33 KHz)"); break;
			case 9: odx_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound          %s","ON (44 KHz)"); break;
			case 10: odx_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound          %s","ON (11 KHz)"); break;
			case 11: odx_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound          %s","ON (15 KHz stereo)"); break;
			case 12: odx_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound          %s","ON (22 KHz stereo)"); break;
			case 13: odx_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound          %s","ON (33 KHz stereo)"); break;
			case 14: odx_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound          %s","ON (44 KHz stereo)"); break;
			case 15: odx_gamelist_text_out_fmt(x_Pos,y_Pos,"Sound          %s","ON (11 KHz stereo)"); break;
		}

		/* (5) CPU Clock */
		y_Pos += 10;
		odx_gamelist_text_out_fmt(x_Pos,y_Pos,"CPU Clock      %d%%",odx_clock_cpu);

		/* (6) Audio Clock */
		y_Pos += 10;
		odx_gamelist_text_out_fmt(x_Pos,y_Pos,"Audio Clock    %d%%",odx_clock_sound);

		/* (7) CPU cores */
		y_Pos += 10;
		switch (odx_cpu_cores)
		{
			case 0: odx_gamelist_text_out(x_Pos,y_Pos, "CPU FAST cores None"); break;
			case 1: odx_gamelist_text_out(x_Pos,y_Pos, "CPU FAST cores Fame"); break;
			default:odx_gamelist_text_out(x_Pos,y_Pos, "CPU FAST cores None"); odx_cpu_cores = 0; break;
		}

		/* (8) Cheats */
		y_Pos += 10;
		if (odx_cheat)
			odx_gamelist_text_out(x_Pos,y_Pos,"Cheats         ON");
		else
			odx_gamelist_text_out(x_Pos,y_Pos,"Cheats         OFF");

		//y_Pos += 30;
		//odx_gamelist_text_out(x_Pos,y_Pos,"Press B to confirm, X to return\0");

		/* Show currently selected item */
		odx_gamelist_text_out(x_Pos-16,y_PosTop+(selected_option*10)+10," >");

		odx_video_flip(); 
		while (odx_joystick_read()) { odx_timer_delay(100); }
		while(!(ExKey=odx_joystick_read())) { }
		if(ExKey & OD_DOWN){
			selected_option++;
			selected_option = selected_option % options_count;
		}
		else if(ExKey & OD_UP){
			selected_option--;
			if(selected_option<0)
				selected_option = options_count - 1;
		}
		else if(ExKey & OD_R || ExKey & OD_L)
		{
			switch(selected_option) {
			case 0:
				switch (odx_video_depth)
				{
					case -1: odx_video_depth=8; break;
					case 8: odx_video_depth=16; break;
					case 16: odx_video_depth=-1; break;
				}
				break;
			case 1:
				if(ExKey & OD_R)
				{
					odx_video_aspect++;
					if (odx_video_aspect>9)
						odx_video_aspect=0;
				}
				else
				{
					odx_video_aspect--;
					if (odx_video_aspect<0)
						odx_video_aspect=9;
				}
				break;
			case 2:
				odx_video_sync=odx_video_sync+1;
				if (odx_video_sync>2)
					odx_video_sync=-1;
				break;
			case 3:
				if(ExKey & OD_R)
				{
					odx_frameskip ++;
					if (odx_frameskip>11)
						odx_frameskip=-1;
				}
				else
				{
					odx_frameskip--;
					if (odx_frameskip<-1)
						odx_frameskip=11;
				}
				break;
			case 4:
				if(ExKey & OD_R)
				{
					odx_sound ++;
					if (odx_sound>15)
						odx_sound=0;
				}
				else
				{
					odx_sound--;
					if (odx_sound<0)
						odx_sound=15;
				}
				break;
			case 5:
				/* "CPU Clock" */
				if(ExKey & OD_R)
				{
					odx_clock_cpu += 10; /* Add 10% */
					if (odx_clock_cpu > 200) /* 200% is the max */
						odx_clock_cpu = 200;
				}
				else
				{
					odx_clock_cpu -= 10; /* Subtract 10% */
					if (odx_clock_cpu < 10) /* 10% is the min */
						odx_clock_cpu = 10;
				}
				break;
			case 6:
				/* "Audio Clock" */
				if(ExKey & OD_R)
				{
					odx_clock_sound += 10; /* Add 10% */
					if (odx_clock_sound > 200) /* 200% is the max */
						odx_clock_sound = 200;
				}
				else{
					odx_clock_sound -= 10; /* Subtract 10% */
					if (odx_clock_sound < 10) /* 10% is the min */
						odx_clock_sound = 10;
				}
				break;
			case 7: /* change for fast cpu core */
				odx_cpu_cores=(odx_cpu_cores+1)%2;
				break;
			case 8: /* Are we using cheats */
				odx_cheat=!odx_cheat;
				break;
			}
		}

		if ((ExKey & OD_A) || (ExKey & OD_START) || (ExKey & OD_SELECT)) 
		{
			/* Write game configuration */
			sprintf(text,"frontend/%s.cfg",game);
			f=fopen(text,"w");
			if (f) {
				fprintf(f,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",odx_freq,odx_video_depth,odx_video_aspect,odx_video_sync,
				odx_frameskip,odx_sound,odx_clock_cpu,odx_clock_sound,odx_cpu_cores,odx_ramtweaks,i,odx_cheat,odx_gsensor);
				fclose(f);
				/* sync(); */
			}

			/* Selected game will be run */
			return 1;
		}
		else if ((ExKey & OD_X) || (ExKey & OD_Y) || (ExKey & OD_B))
		{
			/* Return To Menu */
			return 0;
		}
	}
}

static void odx_exit(char *param)
{
	remove("frontend/mame.lst");
	/* sync(); */
	odx_deinit();
}

static void select_game(char *emu, char *game)
{

	unsigned long ExKey;

	/* No Selected game */
	strcpy(game,"builtinn");

	/* Clean screen */
	odx_video_flip();

	/* Wait until user selects a game */
	while(1)
	{
		game_list_view(&last_game_selected);
		odx_video_flip();

		if( (odx_joystick_read())) odx_timer_delay(100);

		while(!(ExKey=odx_joystick_read())) { 
		}

		if ((ExKey & OD_L) && (ExKey & OD_R) ) { odx_exit(""); }
		if (ExKey & OD_UP) last_game_selected--;
		if (ExKey & OD_DOWN) last_game_selected++;
		if (ExKey & OD_LEFT) last_game_selected-=21;
		if (ExKey & OD_RIGHT) last_game_selected+=21;

		if ((ExKey & OD_A) || (ExKey & OD_START) )
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

	/* odx_freq */
	args[n]="-clock"; n++;
	sprintf(str[i],"%d",odx_freq);
	args[n]=str[i]; i++; n++;

	/* odx_video_depth */
	if (odx_video_depth==8)
	{
		args[n]="-depth"; n++;
		args[n]="8"; n++;
	}
	if (odx_video_depth==16)
	{
		args[n]="-depth"; n++;
		args[n]="16"; n++;
	}

	/* odx_video_aspect */
	if ((odx_video_aspect==1) || (odx_video_aspect==6))
	{
		args[n]="-horizscale"; n++;
		args[n]="-nodirty"; n++;
	}
	if ((odx_video_aspect==2) || (odx_video_aspect==7))
	{
		args[n]="-bestscale"; n++;
		args[n]="-nodirty"; n++;
	}
	if ((odx_video_aspect==3) || (odx_video_aspect==8))
	{
		args[n]="-fastscale"; n++;
		args[n]="-nodirty"; n++;
	}
	if ((odx_video_aspect==4) || (odx_video_aspect==9))
	{
		args[n]="-halfscale"; n++;
		args[n]="-nodirty"; n++;
	}
	if ((odx_video_aspect>=5) && (odx_video_aspect<=9))
	{
		args[n]="-rotatecontrols"; n++;
		args[n]="-ror"; n++;
	}
	
	/* odx_video_sync */
	if (odx_video_sync==1)
	{
		args[n]="-nodirty"; n++;
		args[n]="-waitvsync"; n++;
	}
	else if ((odx_video_sync==2) || (odx_video_aspect==1) || (odx_video_aspect==9))
	{
		args[n]="-nodirty"; n++;
	}
	if (odx_video_sync==-1)
	{
		args[n]="-nothrottle"; n++;
	}
	
	/* odx_frameskip */
	if (odx_frameskip>=0)
	{
		args[n]="-frameskip"; n++;
		sprintf(str[i],"%d",odx_frameskip);
		args[n]=str[i]; i++; n++;
	}

	/* odx_sound */
	if (odx_sound==0)
	{
		args[n]="-soundcard"; n++;
		args[n]="0"; n++;
	}
	if ((odx_sound==1) || (odx_sound==6) || (odx_sound==11))
	{
		args[n]="-samplerate"; n++;
		args[n]="15000"; n++;
	}
	if ((odx_sound==2) || (odx_sound==7) || (odx_sound==12))
	{
		args[n]="-samplerate"; n++;
		args[n]="22050"; n++;
	}
	if ((odx_sound==3) || (odx_sound==8) || (odx_sound==13))
	{
		args[n]="-samplerate"; n++;
		args[n]="32000"; n++;
	}
	if ((odx_sound==4) || (odx_sound==9) || (odx_sound==14))
	{
		args[n]="-samplerate"; n++;
		args[n]="44100"; n++;
	}
	if ((odx_sound==5) || (odx_sound==10) || (odx_sound==15))
	{
		args[n]="-samplerate"; n++;
		args[n]="11025"; n++;
	}
	if ((odx_sound>=1) && (odx_sound<=5))
	{
		args[n]="-fastsound"; n++;
	}
	if (odx_sound>=11)
	{
		args[n]="-stereo"; n++;
	}

	/* odx_clock_cpu */
	if (odx_clock_cpu!=100)
	{
		args[n]="-uclock"; n++;
		sprintf(str[i],"%d",100-odx_clock_cpu);
		args[n]=str[i]; i++; n++;
	}

	/* odx_clock_sound */
	if (odx_clock_cpu!=100)
	{
		args[n]="-uclocks"; n++;
		sprintf(str[i],"%d",100-odx_clock_sound);
		args[n]=str[i]; i++; n++;
	}
	
	/* odx_cpu_cores */
	if ((odx_cpu_cores==1) )
	{
		args[n]="-fame"; n++;
	}
#if 0	
	if ((odx_cpu_cores==1) || (odx_cpu_cores==3) || (odx_cpu_cores==5))
	{
		args[n]="-cyclone"; n++;
	}
	if ((odx_cpu_cores==2) || (odx_cpu_cores==3))
	{
		args[n]="-drz80"; n++;
	}
	if ((odx_cpu_cores==4) || (odx_cpu_cores==5))
	{
		args[n]="-drz80_snd"; n++;
	}
#endif

	if (odx_ramtweaks)
	{
		args[n]="-ramtweaks"; n++;
	}
	
	if (odx_cheat)
	{
		args[n]="-cheat"; n++;
	}

	if (odx_video_aspect==24)
	{
		args[n]="-odx_rotated_video"; n++;
    	args[n]="-rol"; n++;
	}
    if (odx_video_aspect==25)
    {
		args[n]="-odx_rotated_video"; n++;
		args[n]="-rotatecontrols"; n++;
    }
    
	args[n]=NULL;
	
	for (i=0; i<n; i++)
	{
		printf("%s ",args[i]);
	}
	printf("\n");
	
	odx_deinit();
	execv(args[0], args); 
}


int main (int argc, char **argv)
{
	FILE *f;

	/* Open dingux Initialization */
	odx_init(1000,16,44100,16,0,60);

	/* Show intro screen */
	odx_intro_screen();

	/* Initialize list of available games */
	game_list_init(argc);
	if (game_num_avail==0)
	{
		odx_gamelist_text_out(35, 110, "ERROR: NO AVAILABLE GAMES FOUND");
		odx_video_flip();
		odx_joystick_press();
		odx_exit("");
	}

	/* Read default configuration */
	f=fopen("frontend/mame.cfg","r");
	if (f) {
		fscanf(f,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",&odx_freq,&odx_video_depth,&odx_video_aspect,&odx_video_sync,
		&odx_frameskip,&odx_sound,&odx_clock_cpu,&odx_clock_sound,&odx_cpu_cores,&odx_ramtweaks,&last_game_selected,&odx_cheat);
		fclose(f);
	}
	
	/* Select Game */
	select_game(playemu,playgame); 

	/* Write default configuration */
	f=fopen("frontend/mame.cfg","w");
	if (f) {
		fprintf(f,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",odx_freq,odx_video_depth,odx_video_aspect,odx_video_sync,
		odx_frameskip,odx_sound,odx_clock_cpu,odx_clock_sound,odx_cpu_cores,odx_ramtweaks,last_game_selected,odx_cheat);
		fclose(f);
		/* sync(); */
	}
	
	/* Execute Game */
	execute_game (playemu,playgame);
	
	exit (0);
}
