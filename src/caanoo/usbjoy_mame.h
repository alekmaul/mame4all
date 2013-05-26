#ifndef __USBJOYMAME__
#define __USBJOYMAME__

#include "usbjoy.h"

#define MAX_JOYSTICK 4

extern struct usbjoy * joys [MAX_JOYSTICK];
extern int num_of_joys;
extern int usbjoy_player1;
extern int usbjoy_tate;

extern void usbjoy_init(void);
extern void usbjoy_close (void);
extern unsigned int usbjoy_check (struct usbjoy * joy);

#endif /* __USBJOYMAME__ */
