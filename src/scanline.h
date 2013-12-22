/*********************************************************************

  scanline.h

  Generic graphic functions.

*********************************************************************/

#ifndef SCANLINE_H
#define SCANLINE_H

void draw_scanline8(struct osd_bitmap *bitmap,int x,int y,int length,unsigned char *src, unsigned short *pens,int transparent_pen);
void draw_scanline16(struct osd_bitmap *bitmap,int x,int y,int length,unsigned short *src, unsigned short *pens,int transparent_pen);
void pdraw_scanline8(struct osd_bitmap *bitmap,int x,int y,int length,unsigned char *src, unsigned short *pens,int transparent_pen,int pri);
void pdraw_scanline16(struct osd_bitmap *bitmap,int x,int y,int length,unsigned short *src, unsigned short *pens,int transparent_pen,int pri);
void extract_scanline8(struct osd_bitmap *bitmap,int x,int y,int length,unsigned char *dst);
void extract_scanline16(struct osd_bitmap *bitmap,int x,int y,int length,unsigned short *dst);

#endif
