/* Title: USB Joystick library
   Version 0.2
   Written by Puck2099 (puck2099@gmail.com), (c) 2006.
   <http://www.gp32wip.com>
   
   If you use this library or a part of it, please, let it know.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdlib.h>
#include <stdio.h>		/* For the definition of NULL */
#include <sys/types.h>	        // For Device open
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>		// For Device read

#include <string.h>
#include <limits.h>		/* For the definition of PATH_MAX */


#include "usbjoy.h"
#include "wiz_lib.h"

/* The maximum number of joysticks we'll detect */
#define MAX_JOYSTICKS	32

#define test_bit(nr, addr) \
	(((1UL << ((nr) & 31)) & (((const unsigned int *) addr)[(nr) >> 5])) != 0)

/*
  Function: joy_config

  USB joystick configuration (key map and button's number).

  Parameters:

  joy - Selected joystick.
  fd - file discriptor.

  Returns:

  true/false.

*/
static bool joy_config(usbjoy * joy, int fd)
{
	int i, t;
	unsigned long keybit[40];
	unsigned long absbit[40];
	unsigned long relbit[40];

	/* See if this device uses the new unified event API */
	if ( (ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keybit)), keybit) >= 0) &&
	     (ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(absbit)), absbit) >= 0) &&
	     (ioctl(fd, EVIOCGBIT(EV_REL, sizeof(relbit)), relbit) >= 0) ) {
		//joystick->hwdata->is_hid = SDL_TRUE;

		/* Get the number of buttons, axes, and other thingamajigs */
		for ( i=BTN_JOYSTICK; i < KEY_MAX; ++i ) {
			if ( test_bit(i, keybit) ) {
#ifdef DEBUG_INPUT_EVENTS
				printf("Joystick has button: 0x%x\n", i);
#endif
				joy->key_map[i-BTN_MISC] =
						joy->numbuttons;
				++joy->numbuttons;
			}
		}
		for ( i=BTN_MISC; i < BTN_JOYSTICK; ++i ) {
			if ( test_bit(i, keybit) ) {
#ifdef DEBUG_INPUT_EVENTS
				printf("Joystick has button: 0x%x\n", i);
#endif
				joy->key_map[i-BTN_MISC] =
						joy->numbuttons;
				++joy->numbuttons;
			}
		}
	}
	return true;
}

/*
  Function: joy_open

  Opens a USB joystick and fills its information.

  Parameters:

  joynumber - Joystick's identifier (0 reserved for GP2X's builtin Joystick).

  Returns:

  Filled usbjoy structure.

*/
struct usbjoy * joy_open (int joynumber) {
	int fd, i;
	char path [128];
	struct usbjoy * joy = NULL;
	dev_t dev_nums[MAX_JOYSTICKS];  /* major/minor device numbers */
	struct stat sb;

	if (joynumber == 0) {
	}
	else if (joynumber > 0) {
		sprintf (path, "/dev/input/event%d", joynumber);
		fd = open(path, O_RDONLY, 0);
		if (fd > 0) {
			joy = (struct usbjoy *) malloc(sizeof(struct usbjoy));

			// Joystick's file descriptor
			joy->fd = fd;
			// Set the joystick to non-blocking read mode
			fcntl(joy->fd, F_SETFL, O_NONBLOCK);

			// Joystick's name
			ioctl(joy->fd, EVIOCGNAME(128*sizeof(char)), joy->name);
			printf("joy_open(%d) joy->name=%s\n", joynumber, joy->name);
			
			// Joystick's device
			sprintf (joy->device, path);
			joy_config(joy, joy->fd);
			printf("joy_open(%d) joy->numbuttons=%d\n", joynumber, joy->numbuttons);

			// Clean buttons and axes
			for (i=0; i<32; i++) joy->statebuttons[i] = 0;
			for (i=0; i<4; i++) joy->stateaxes[i] = 0;
		}
		else {
			printf ("ERROR: No Joystick found\n");
		}
	}
	return joy;
}

/*
  Function: joy_name

  Returns Joystick's name.

  Parameters:

  joy - Selected joystick.

  Returns:

  Joystick's name or NULL if <usbjoy> struct is empty.
*/
char * joy_name (struct usbjoy * joy) {
  if (joy != NULL)  return joy->name;
  else return NULL;
}


/*
  Function: joy_device

  Returns Joystick's device.

  Parameters:

  joy - Selected joystick.

  Returns:

  Joystick's device or NULL if <usbjoy> struct is empty.
*/
char * joy_device (struct usbjoy * joy) {
  if (joy != NULL)  return joy->device;
  else return NULL;
}


/*
  Function: joy_buttons

  Returns Joystick's buttons number.

  Parameters:

  joy - Selected joystick.

  Returns:

  Joystick's buttons or 0 if <usbjoy> struct is empty.
*/
int joy_buttons (struct usbjoy * joy) {
  if (joy != NULL) return joy->numbuttons;
  else return 0;
}


/*
  Function: joy_axes

  Returns Joystick's axes number.

  Parameters:

  joy - Selected joystick.

  Returns:

  Joystick's axes or 0 if <usbjoy> struct is empty.
*/
int joy_axes (struct usbjoy * joy) {
  if (joy != NULL) return joy->numaxes;
  else return 0;
}


/*
  Function: joy_update

  Updates Joystick's internal information (<statebuttons> and <stateaxes> fields).

  Parameters:

  joy - Selected joystick.

  Returns:

  0 - No events registered (no need to update).
  1 - Events registered (a button or axe has been pushed).
  -1 - Error: <usbjoy> struct is empty.
*/
extern int abs_x, abs_y, abs_z;
int joy_update (struct usbjoy * joy) 
{
	struct input_event events[32];
	int i, len;
	int code;
	int event = 0;

	if (joy != NULL)
	{
		while ((len=read(joy->fd, events, (sizeof events))) > 0) 
		{
			len /= sizeof(events[0]);         
			for ( i=0; i<len; ++i ) {
				code = events[i].code;
				switch (events[i].type) {
					case EV_ABS:
						if (code == ABS_X) 
						{
							abs_x = events[i].value - 128; 
							joy->stateaxes[JOYLEFT] = joy->stateaxes[JOYRIGHT] = 0;
							if (abs_x < -32) joy->stateaxes[JOYLEFT] = 1;
							if (abs_x >  32) joy->stateaxes[JOYRIGHT] = 1;
						}
						else if (code == ABS_Y)
						{
							abs_y = events[i].value - 128; 
							joy->stateaxes[JOYUP] = joy->stateaxes[JOYDOWN] = 0;
							if (abs_y < -32) joy->stateaxes[JOYUP] = 1;
							if (abs_y >  32) joy->stateaxes[JOYDOWN] = 1;
						}
						event = 1;
						break;
						
					case EV_KEY:
						{
							if (code >= BTN_MISC)
							{
								code -= BTN_MISC;
								joy->statebuttons[joy->key_map[code]] = events[i].value;
								event = 1;
							}
						}
						break;
					default:
					  break;
				}
			}
		}
	}
	else {
		event = -1;
	}   
	return event;
}


/*
  Function: joy_getbutton

  Returns Joystick's button information.

  Parameters:

  button - Button which value you want to know (from 0 to 31).
  joy - Selected joystick.

  Returns:

  0 - Button NOT pushed.
  1 - Button pushed.
  -1 - Error: <usbjoy> struct is empty.
*/
int joy_getbutton (int button, struct usbjoy * joy) {
  if (joy != NULL) {
    if (button < joy_buttons(joy)) return joy->statebuttons[button];
    else return 0;
  }
  else return -1;
}


/*
  Function: joy_getaxe

  Returns Joystick's axes information.

  Parameters:

  axe - Axe which value you want to know (see <Axes values>).
  joy - Selected joystick.

  Returns:

  0 - Direction NOT pushed.
  1 - Direction pushed.
  -1 - Error: <usbjoy> struct is empty.
*/
int joy_getaxe (int axe, struct usbjoy * joy) {
  if (joy != NULL) {
    if (axe < 4) return joy->stateaxes[axe];
    else return 0;
  }
  else return -1;
}


/*
  Function: joy_close

  Closes selected joystick's file descriptor and detroys it's fields.

  Parameters:

  joy - Selected joystick.

  Returns:

  0 - Joystick successfully closed.
  -1 - Error: <usbjoy> struct is empty.
*/
int joy_close (struct usbjoy * joy) {
  if (joy != NULL) {
    close (joy->fd);
    free (joy);
    return 0;
  }
  else return -1;
}
