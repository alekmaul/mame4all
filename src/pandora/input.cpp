#include <linux/input.h>
#include "allegro.h"
#include "driver.h"

int use_mouse;
int joystick;

unsigned long ExKey1=0;
unsigned long ExKey2=0;
unsigned long ExKey3=0;
unsigned long ExKey4=0;
int num_joysticks=4;
#include "minimal.h"

extern int rotate_controls;

static struct KeyboardInfo keylist[] =
{
	{ "A",			KEY_A,				KEYCODE_A },
	{ "B",			KEY_B,				KEYCODE_B },
	{ "C",			KEY_C,				KEYCODE_C },
	{ "D",			KEY_D,				KEYCODE_D },
	{ "E",			KEY_E,				KEYCODE_E },
	{ "F",			KEY_F,				KEYCODE_F },
	{ "G",			KEY_G,				KEYCODE_G },
	{ "H",			KEY_H,				KEYCODE_H },
	{ "I",			KEY_I,				KEYCODE_I },
	{ "J",			KEY_J,				KEYCODE_J },
	{ "K",			KEY_K,				KEYCODE_K },
	{ "L",			KEY_L,				KEYCODE_L },
	{ "M",			KEY_M,				KEYCODE_M },
	{ "N",			KEY_N,				KEYCODE_N },
	{ "O",			KEY_O,				KEYCODE_O },
	{ "P",			KEY_P,				KEYCODE_P },
	{ "Q",			KEY_Q,				KEYCODE_Q },
	{ "R",			KEY_R,				KEYCODE_R },
	{ "S",			KEY_S,				KEYCODE_S },
	{ "T",			KEY_T,				KEYCODE_T },
	{ "U",			KEY_U,				KEYCODE_U },
	{ "V",			KEY_V,				KEYCODE_V },
	{ "W",			KEY_W,				KEYCODE_W },
	{ "X",			KEY_X,				KEYCODE_X },
	{ "Y",			KEY_Y,				KEYCODE_Y },
	{ "Z",			KEY_Z,				KEYCODE_Z },
	{ "0",			KEY_0,				KEYCODE_0 },
	{ "1",			KEY_1,				KEYCODE_1 },
	{ "2",			KEY_2,				KEYCODE_2 },
	{ "3",			KEY_3,				KEYCODE_3 },
	{ "4",			KEY_4,				KEYCODE_4 },
	{ "5",			KEY_5,				KEYCODE_5 },
	{ "6",			KEY_6,				KEYCODE_6 },
	{ "7",			KEY_7,				KEYCODE_7 },
	{ "8",			KEY_8,				KEYCODE_8 },
	{ "9",			KEY_9,				KEYCODE_9 },
	{ "0 PAD",		KEY_KP0,			KEYCODE_0_PAD },
	{ "1 PAD",		KEY_KP1,			KEYCODE_1_PAD },
	{ "2 PAD",		KEY_KP2,			KEYCODE_2_PAD },
	{ "3 PAD",		KEY_KP3,			KEYCODE_3_PAD },
	{ "4 PAD",		KEY_KP4,			KEYCODE_4_PAD },
	{ "5 PAD",		KEY_KP5,			KEYCODE_5_PAD },
	{ "6 PAD",		KEY_KP6,			KEYCODE_6_PAD },
	{ "7 PAD",		KEY_KP7,			KEYCODE_7_PAD },
	{ "8 PAD",		KEY_KP8,			KEYCODE_8_PAD },
	{ "9 PAD",		KEY_KP9,			KEYCODE_9_PAD },
	{ "F1",			KEY_F1,				KEYCODE_F1 },
	{ "F2",			KEY_F2,				KEYCODE_F2 },
	{ "F3",			KEY_F3,				KEYCODE_F3 },
	{ "F4",			KEY_F4,				KEYCODE_F4 },
	{ "F5",			KEY_F5,				KEYCODE_F5 },
	{ "F6",			KEY_F6,				KEYCODE_F6 },
	{ "F7",			KEY_F7,				KEYCODE_F7 },
	{ "F8",			KEY_F8,				KEYCODE_F8 },
	{ "F9",			KEY_F9,				KEYCODE_F9 },
	{ "F10",		KEY_F10,			KEYCODE_F10 },
	{ "F11",		KEY_F11,			KEYCODE_F11 },
	{ "F12",		KEY_F12,			KEYCODE_F12 },
	{ "ESC",		KEY_ESC,			KEYCODE_ESC },
	{ "~",			KEY_GRAVE,			KEYCODE_TILDE },
	{ "-",          KEY_MINUS,          KEYCODE_MINUS },
	{ "=",          KEY_EQUAL,			KEYCODE_EQUALS },
	{ "BKSPACE",	KEY_BACKSPACE,		KEYCODE_BACKSPACE },
	{ "TAB",		KEY_TAB,			KEYCODE_TAB },
	{ "[",          KEY_LEFTBRACE,      KEYCODE_OPENBRACE },
	{ "]",          KEY_RIGHTBRACE,		KEYCODE_CLOSEBRACE },
	{ "ENTER",		KEY_ENTER,			KEYCODE_ENTER },
	{ ";",          KEY_SEMICOLON,      KEYCODE_COLON },
	{ ":",          KEY_APOSTROPHE,     KEYCODE_QUOTE },
	{ "\\",         KEY_BACKSLASH,      KEYCODE_BACKSLASH },
	{ "<",          KEY_COMMA,			KEYCODE_BACKSLASH2 },
	{ ",",          KEY_COMMA,          KEYCODE_COMMA },
	{ ".",          KEY_DOT,			KEYCODE_STOP },
	{ "/",          KEY_SLASH,          KEYCODE_SLASH },
	{ "SPACE",		KEY_SPACE,			KEYCODE_SPACE },
	{ "INS",		KEY_INSERT,			KEYCODE_INSERT },
	{ "DEL",		KEY_DELETE,			KEYCODE_DEL },
	{ "(A)",		KEY_HOME,			KEYCODE_HOME },
	{ "(B)",		KEY_END,			KEYCODE_END },
	{ "(Y)",		KEY_PAGEUP,			KEYCODE_PGUP },
	{ "(X)",		KEY_PAGEDOWN,		KEYCODE_PGDN },
	{ "LEFT",		KEY_LEFT,			KEYCODE_LEFT },
	{ "RIGHT",		KEY_RIGHT,			KEYCODE_RIGHT },
	{ "UP",			KEY_UP,				KEYCODE_UP },
	{ "DOWN",		KEY_DOWN,			KEYCODE_DOWN },
	{ "/ PAD",      KEY_KPSLASH,		KEYCODE_SLASH_PAD },
	{ "* PAD",      KEY_KPASTERISK,     KEYCODE_ASTERISK },
	{ "- PAD",      KEY_KPMINUS,		KEYCODE_MINUS_PAD },
	{ "+ PAD",      KEY_KPPLUS,			KEYCODE_PLUS_PAD },
	{ ". PAD",      KEY_KPDOT,			KEYCODE_DEL_PAD },
	{ "ENTER PAD",  KEY_KPENTER,		KEYCODE_ENTER_PAD },
	{ "PRTSCR",     KEY_PRINT,			KEYCODE_PRTSCR },
	{ "PAUSE",      KEY_PAUSE,          KEYCODE_PAUSE },
	{ "LSHIFT",		KEY_LEFTSHIFT,		KEYCODE_LSHIFT },
	{ "(L)",		KEY_RIGHTSHIFT,		KEYCODE_RSHIFT },
	{ "SELECT",		KEY_LEFTCTRL,		KEYCODE_LCONTROL },
	{ "(R)",		KEY_RIGHTCTRL,		KEYCODE_RCONTROL },
	{ "START",		KEY_LEFTALT,		KEYCODE_LALT },
	{ "ALTGR",		KEY_RIGHTALT,		KEYCODE_RALT },
	{ "LWIN",		KEY_LEFTMETA,		KEYCODE_LWIN },
	{ "RWIN",		KEY_RIGHTMETA,		KEYCODE_RWIN },
	{ "PANDORA",	KEY_MENU,			KEYCODE_MENU },
	{ "SCRLOCK",    KEY_SCROLLLOCK,		KEYCODE_SCRLOCK },
	{ "NUMLOCK",    KEY_NUMLOCK,        KEYCODE_NUMLOCK },
	{ "CAPSLOCK",   KEY_CAPSLOCK,       KEYCODE_CAPSLOCK },
	{ 0, 0, 0 }	/* end of table */
};


/* return a list of all available keys */
const struct KeyboardInfo *osd_get_key_list(void)
{
	return keylist;
}

int osd_is_key_pressed(int keycode)
{	if (keycode >= KEY_MAX) return 0;

	if (keycode == KEY_PAUSE)
	{
		static int pressed,counter;
		int res;

		res = pnd_keys[KEY_PAUSE] ^ pressed;
		if (res)
		{
			if (counter > 0)
			{
				if (--counter == 0)
					pressed = pnd_keys[KEY_PAUSE];
			}
			else counter = 10;
		}

		return res;
	}

	return pnd_keys[keycode];
}


int osd_wait_keypress(void)
{
	/*
	clear_keybuf();
	return readkey() >> 8;
	*/
	return 0;
}


int osd_readkey_unicode(int flush)
{
	/*
	if (flush) clear_keybuf();
	if (keypressed())
		return ureadkey(NULL);
	else
		return 0;
	*/
	return 0;
}


/*
  limits:
  - 7 joysticks
  - 15 sticks on each joystick
  - 63 buttons on each joystick

  - 256 total inputs
*/
#define JOYCODE(joy,stick,axis_or_button,dir) \
		((((dir)&0x03)<<14)|(((axis_or_button)&0x3f)<<8)|(((stick)&0x1f)<<3)|(((joy)&0x07)<<0))

#define GET_JOYCODE_JOY(code) (((code)>>0)&0x07)
#define GET_JOYCODE_STICK(code) (((code)>>3)&0x1f)
#define GET_JOYCODE_AXIS(code) (((code)>>8)&0x3f)
#define GET_JOYCODE_BUTTON(code) GET_JOYCODE_AXIS(code)
#define GET_JOYCODE_DIR(code) (((code)>>14)&0x03)

/* use otherwise unused joystick codes for the three mouse buttons */
#define MOUSE_BUTTON(button) JOYCODE(1,0,button,1)


#define MAX_JOY 256
#define MAX_JOY_NAME_LEN 40

static struct JoystickInfo joylist[MAX_JOY] =
{
	/* will be filled later */
	{ 0, 0, 0 }	/* end of table */
};

static char joynames[MAX_JOY][MAX_JOY_NAME_LEN+1];	/* will be used to store names for the above */


static int joyequiv[][2] =
{
	{ JOYCODE(1,1,1,1),	JOYCODE_1_LEFT },
	{ JOYCODE(1,1,1,2),	JOYCODE_1_RIGHT },
	{ JOYCODE(1,1,2,1),	JOYCODE_1_UP },
	{ JOYCODE(1,1,2,2),	JOYCODE_1_DOWN },
	{ JOYCODE(1,0,1,0),	JOYCODE_1_BUTTON1 },
	{ JOYCODE(1,0,2,0),	JOYCODE_1_BUTTON2 },
	{ JOYCODE(1,0,3,0),	JOYCODE_1_BUTTON3 },
	{ JOYCODE(1,0,4,0),	JOYCODE_1_BUTTON4 },
	{ JOYCODE(1,0,5,0),	JOYCODE_1_BUTTON5 },
	{ JOYCODE(1,0,6,0),	JOYCODE_1_BUTTON6 },
	{ JOYCODE(2,1,1,1),	JOYCODE_2_LEFT },
	{ JOYCODE(2,1,1,2),	JOYCODE_2_RIGHT },
	{ JOYCODE(2,1,2,1),	JOYCODE_2_UP },
	{ JOYCODE(2,1,2,2),	JOYCODE_2_DOWN },
	{ JOYCODE(2,0,1,0),	JOYCODE_2_BUTTON1 },
	{ JOYCODE(2,0,2,0),	JOYCODE_2_BUTTON2 },
	{ JOYCODE(2,0,3,0),	JOYCODE_2_BUTTON3 },
	{ JOYCODE(2,0,4,0),	JOYCODE_2_BUTTON4 },
	{ JOYCODE(2,0,5,0),	JOYCODE_2_BUTTON5 },
	{ JOYCODE(2,0,6,0),	JOYCODE_2_BUTTON6 },
	{ JOYCODE(3,1,1,1),	JOYCODE_3_LEFT },
	{ JOYCODE(3,1,1,2),	JOYCODE_3_RIGHT },
	{ JOYCODE(3,1,2,1),	JOYCODE_3_UP },
	{ JOYCODE(3,1,2,2),	JOYCODE_3_DOWN },
	{ JOYCODE(3,0,1,0),	JOYCODE_3_BUTTON1 },
	{ JOYCODE(3,0,2,0),	JOYCODE_3_BUTTON2 },
	{ JOYCODE(3,0,3,0),	JOYCODE_3_BUTTON3 },
	{ JOYCODE(3,0,4,0),	JOYCODE_3_BUTTON4 },
	{ JOYCODE(3,0,5,0),	JOYCODE_3_BUTTON5 },
	{ JOYCODE(3,0,6,0),	JOYCODE_3_BUTTON6 },
	{ JOYCODE(4,1,1,1),	JOYCODE_4_LEFT },
	{ JOYCODE(4,1,1,2),	JOYCODE_4_RIGHT },
	{ JOYCODE(4,1,2,1),	JOYCODE_4_UP },
	{ JOYCODE(4,1,2,2),	JOYCODE_4_DOWN },
	{ JOYCODE(4,0,1,0),	JOYCODE_4_BUTTON1 },
	{ JOYCODE(4,0,2,0),	JOYCODE_4_BUTTON2 },
	{ JOYCODE(4,0,3,0),	JOYCODE_4_BUTTON3 },
	{ JOYCODE(4,0,4,0),	JOYCODE_4_BUTTON4 },
	{ JOYCODE(4,0,5,0),	JOYCODE_4_BUTTON5 },
	{ JOYCODE(4,0,6,0),	JOYCODE_4_BUTTON6 },
	{ 0,0 }
};


static void init_joy_list(void)
{
	int tot,i,j,k;
	char buf[256];

	tot = 0;

	/* first of all, map mouse buttons */
	for (j = 0;j < 3;j++)
	{
		sprintf(buf,"MOUSE B%d",j+1);
		strncpy(joynames[tot],buf,MAX_JOY_NAME_LEN);
		joynames[tot][MAX_JOY_NAME_LEN] = 0;
		joylist[tot].name = joynames[tot];
		joylist[tot].code = MOUSE_BUTTON(j+1);
		tot++;
	}

	for (i = 0;i < num_joysticks;i++)
	{
		for (j = 0;j < 1;j++)
		{
			for (k = 0;k < 2;k++)
			{
				sprintf(buf,"J%d %s %s -",i+1,"JoystickAxis","-");
				strncpy(joynames[tot],buf,MAX_JOY_NAME_LEN);
				joynames[tot][MAX_JOY_NAME_LEN] = 0;
				joylist[tot].name = joynames[tot];
				joylist[tot].code = JOYCODE(i+1,j+1,k+1,1);
				tot++;

				sprintf(buf,"J%d %s %s +",i+1,"JoystickAxis","+");
				strncpy(joynames[tot],buf,MAX_JOY_NAME_LEN);
				joynames[tot][MAX_JOY_NAME_LEN] = 0;
				joylist[tot].name = joynames[tot];
				joylist[tot].code = JOYCODE(i+1,j+1,k+1,2);
				tot++;
			}
		}
		for (j = 0;j < 6;j++)
		{
			sprintf(buf,"J%d %s",i+1,"JoystickButton");
			strncpy(joynames[tot],buf,MAX_JOY_NAME_LEN);
			joynames[tot][MAX_JOY_NAME_LEN] = 0;
			joylist[tot].name = joynames[tot];
			joylist[tot].code = JOYCODE(i+1,0,j+1,0);
			tot++;
		}
	}

	/* terminate array */
	joylist[tot].name = 0;
	joylist[tot].code = 0;
	joylist[tot].standardcode = 0;

	/* fill in equivalences */
	for (i = 0;i < tot;i++)
	{
		joylist[i].standardcode = JOYCODE_OTHER;

		j = 0;
		while (joyequiv[j][0] != 0)
		{
			if (joyequiv[j][0] == joylist[i].code)
			{
				joylist[i].standardcode = joyequiv[j][1];
				break;
			}
			j++;
		}
	}
}


/* return a list of all available joys */
const struct JoystickInfo *osd_get_joy_list(void)
{
	return joylist;
}

static int is_joy_button_pressed (int button, int ExKey)
{
	switch (button)
	{
		case 0: return ExKey & PND_B; break;
		case 1: return ExKey & PND_X; break;
		case 2: return ExKey & PND_A; break;
		case 3: return ExKey & PND_Y; break;
		case 4: return ExKey & PND_L; break;
		case 5: return ExKey & PND_R; break;
		default: break;
	}
	return 0; 
}

#define JOY_LEFT_PRESSED is_joy_axis_pressed(0,1,ExKey1)
#define JOY_RIGHT_PRESSED is_joy_axis_pressed(0,2,ExKey1)
#define JOY_UP_PRESSED is_joy_axis_pressed(1,1,ExKey1)
#define JOY_DOWN_PRESSED is_joy_axis_pressed(1,2,ExKey1)

static int is_joy_axis_pressed (int axis, int dir, int ExKey)
{
	/* Normal controls */
	if (axis==0)
	{
		switch (dir)
		{
			case 1: return ExKey & PND_LEFT; break;
			case 2: return ExKey & PND_RIGHT; break;
			default: return 0; break;
		}
	}
	else
	{
		switch (dir)
		{
			case 1: return ExKey & PND_UP; break;
			case 2: return ExKey & PND_DOWN; break;
			default: return 0; break;
		}
	}
}

int osd_is_joy_pressed(int joycode)
{
	int joy_num,stick;


	/* special case for mouse buttons */
	switch (joycode)
	{
		case MOUSE_BUTTON(1):
			return ExKey1 & PND_B; break;
		case MOUSE_BUTTON(2):
			return ExKey1 & PND_X; break;
		case MOUSE_BUTTON(3):
			return ExKey1 & PND_A; break;
	}

	joy_num = GET_JOYCODE_JOY(joycode);

	/* do we have as many sticks? */
	if (joy_num == 0 || joy_num > num_joysticks)
		return 0;
	joy_num--;

	stick = GET_JOYCODE_STICK(joycode);

	if (stick == 0)
	{
		/* buttons */
		int button;

		button = GET_JOYCODE_BUTTON(joycode);
		if (button == 0 || button > 6)
			return 0;
		button--;

		switch (joy_num)
		{
			case 0: return is_joy_button_pressed(button, ExKey1); break;
			case 1: return is_joy_button_pressed(button, ExKey2); break;
			case 2: return is_joy_button_pressed(button, ExKey3); break;
			case 3: return is_joy_button_pressed(button, ExKey4); break;
			default: break;
		}
	}
	else
	{
		/* sticks */
		int axis,dir;

		if (stick > 1)
			return 0;
		stick--;

		axis = GET_JOYCODE_AXIS(joycode);
		dir = GET_JOYCODE_DIR(joycode);

		if (axis == 0 || axis > 2)
			return 0;
		axis--;

		switch (joy_num)
		{
			case 0: return is_joy_axis_pressed(axis, dir, ExKey1); break;
			case 1: return is_joy_axis_pressed(axis, dir, ExKey2); break;
			case 2: return is_joy_axis_pressed(axis, dir, ExKey3); break;
			case 3: return is_joy_axis_pressed(axis, dir, ExKey4); break;
			default: break;
		}
	}

	return 0;
}

static void poll_joystick(void)
{
	extern int num_of_joys;

	switch (num_of_joys)
	{
		case 4: ExKey4=pnd_joystick_read(3);
		case 3: ExKey3=pnd_joystick_read(2);
		case 2: ExKey2=pnd_joystick_read(1);
		case 1: ExKey1=pnd_joystick_read(0); break;
		default:
			ExKey1=pnd_joystick_read(0);
			ExKey2=ExKey1;
			if(ExKey2&PND_START) ExKey2^=PND_START;
			if(ExKey2&PND_SELECT) ExKey2^=PND_SELECT;
			ExKey3=ExKey2;
			ExKey4=ExKey2;
			break;
	}
}

void osd_poll_joysticks(void)
{
	if (joystick > JOY_TYPE_NONE)
		poll_joystick();
}

int pos_analog_x=0;
int pos_analog_y=0;

/* return a value in the range -128 .. 128 (yes, 128, not 127) */
void osd_analogjoy_read(int player,int *analog_x, int *analog_y)
{
	*analog_x = *analog_y = 0;

	/* is there an analog joystick at all? */
	if (player+1 > num_joysticks || joystick == JOY_TYPE_NONE)
		return;

	if( (!(JOY_LEFT_PRESSED)) && (!(JOY_RIGHT_PRESSED)) ) {
		pos_analog_x=0;
	} else {
		if(JOY_LEFT_PRESSED) pos_analog_x-=5;
		if(JOY_RIGHT_PRESSED) pos_analog_x+=5;
	}
	if( (!(JOY_UP_PRESSED)) && (!(JOY_DOWN_PRESSED)) ) {
		pos_analog_y=0;
	} else {
		if(JOY_UP_PRESSED) pos_analog_y-=5; 
		if(JOY_DOWN_PRESSED) pos_analog_y+=5;
	 }
	
	if (pos_analog_x<-128) pos_analog_x=-128;
	if (pos_analog_x>128) pos_analog_x=128;
	if (pos_analog_y<-128) pos_analog_y=-128;
	if (pos_analog_y>128) pos_analog_y=128;
	
	*analog_x = pos_analog_x;
	*analog_y = pos_analog_y;
}


int osd_joystick_needs_calibration (void)
{
	return 0;
}


void osd_joystick_start_calibration (void)
{
}

char *osd_joystick_calibrate_next (void)
{
	return 0;
}

void osd_joystick_calibrate (void)
{
}

void osd_joystick_end_calibration (void)
{
}

void osd_trak_read(int player,int *deltax,int *deltay)
{
	if (player != 0 || use_mouse == 0)
		*deltax = *deltay = 0;
	else
	{
		*deltax = *deltay = 0;
		if(JOY_LEFT_PRESSED) *deltax=-5;
	  	if(JOY_RIGHT_PRESSED) *deltax=5;
	  	if(JOY_UP_PRESSED) *deltay=5; 
	 	if(JOY_DOWN_PRESSED) *deltay=-5;
	}
}


#ifndef MESS
#ifndef TINY_COMPILE
extern int no_of_tiles;
extern struct GameDriver driver_neogeo;
#endif
#endif

void osd_customize_inputport_defaults(struct ipd *defaults)
{
}



void osd_led_w(int led,int on) {
}



void msdos_init_input (void)
{
	if (joystick == JOY_TYPE_NONE)
		logerror("Joystick not found\n");
	else
		logerror("Installed %s %s\n","Joystick", "Pandora");

	init_joy_list();

	if (use_mouse)
		use_mouse = 1;
	else
		use_mouse = 0;
}


void msdos_shutdown_input(void)
{
}
