

#include "usbjoy_mame.h"
#include "wiz_lib.h"

struct usbjoy * joys [MAX_JOYSTICK];
int num_of_joys = 0;

void usbjoy_init (void) {
	int i;

	for (i=0; i<MAX_JOYSTICK; i++) {
		joys[i] = joy_open (i+1);
		if (joy_buttons(joys[i])>0) { num_of_joys++; }
	}

	printf ("\n\nFound %d Joystick(s)\n",num_of_joys);
	for (i=0; i < num_of_joys; i++) {
		printf ("+-Joystick %d: %s\n",i+1,joy_name(joys[i]));
	}
}

void usbjoy_close (void) {
	int i;
	for (i=0; i<num_of_joys; i++) {
		joy_close(joys[i]);
	}
}


unsigned int usbjoy_check (struct usbjoy * joy) {
 int q = 0;
 unsigned int joyExKey = 0;

 
 if (joy != NULL) {
	/* Update USB Joypad Information */
	joy_update(joy);
	
	if (joy_getaxe(JOYUP, joy))    { joyExKey |= WIZ_UP; }
	if (joy_getaxe(JOYDOWN, joy))  { joyExKey |= WIZ_DOWN; }
	if (joy_getaxe(JOYLEFT, joy))  { joyExKey |= WIZ_LEFT; }
	if (joy_getaxe(JOYRIGHT, joy)) { joyExKey |= WIZ_RIGHT; }
	
	/* loop through joy buttons to check if they are pushed */
	for (q=0; q<joy_buttons (joy); q++) {
		if (joy_getbutton (q, joy)) // Button Press
		{
			joyExKey |= wiz_key_value(q);
		}
	}
	return joyExKey;
 } else {
	joyExKey = 0;
	return joyExKey;
 }
}
