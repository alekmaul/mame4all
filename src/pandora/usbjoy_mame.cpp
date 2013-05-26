#include "minimal.h"

struct usbjoy * joys [4];
int num_of_joys = 0;

void pnd_usbjoy_init (void) {
	int i;

	for (i=0; i<4; i++) {
		joys[i] = joy_open (i+1);
		if (joy_buttons(joys[i])>0) { num_of_joys++; }
	}

	printf ("\n\nFound %d Joystick(s)\n",num_of_joys);
	for (i=0; i < num_of_joys; i++) {
		printf ("+-Joystick %d: %s\n",i+1,joy_name(joys[i]));
	}
}

void pnd_usbjoy_close (void) {
	int i;
	for (i=0; i<num_of_joys; i++) {
		joy_close(joys[i]);
	}
}

int pnd_usbjoy_check (struct usbjoy * joy) {
 int q, joyExKey = 0;
 if (joy != NULL) {
	/* Update USB Joypad Information */
	joy_update(joy);
	
	if (joy_getaxe(JOYUP, joy))    { joyExKey |= PND_UP; }
	if (joy_getaxe(JOYDOWN, joy))  { joyExKey |= PND_DOWN; }
	if (joy_getaxe(JOYLEFT, joy))  { joyExKey |= PND_LEFT; }
	if (joy_getaxe(JOYRIGHT, joy)) { joyExKey |= PND_RIGHT; }

	/* loop through joy buttons to check if they are pushed */
	for (q=0; q<joy_buttons (joy); q++) {
		if (joy_getbutton (q, joy)) {
			if (q == 0)  { joyExKey |= PND_Y; }
			if (q == 1)  { joyExKey |= PND_B; }
			if (q == 2)  { joyExKey |= PND_X; }
			if (q == 3)  { joyExKey |= PND_A; }

			if (q == 4)  { joyExKey |= PND_L; }
			if (q == 5)  { joyExKey |= PND_R; }
			if (q == 6)  { joyExKey |= PND_L; } /* left shoulder button 2 */
			if (q == 7)  { joyExKey |= PND_R; } /* right shoulder button 2 */
			if (q == 8)  { joyExKey |= PND_SELECT; }
			if (q == 9)  { joyExKey |= PND_START; }
		}
	}
	return joyExKey;
 } else {
	joyExKey = 0;
	return joyExKey;
 }
}
