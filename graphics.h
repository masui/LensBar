/*
	$Date: 1997/02/19 15:02:06 $
	$Revision: 1.1 $
*/
#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

void moveto(int x, int y);
void setcolor(int color);
void line(int x1, int y1, int x2, int y2);
void rectangle(int x1, int y1, int x2, int y2);
void button(int x1, int y1, int x2, int y2);
void transparentbutton(int x1, int y1, int x2, int y2, int y3, int y4, int color);
void pan(int x1, int y1, int x2, int y2, int bgcolor);
void textbox(int x1, int y1, int x2, int y2, int bgcolor);
#ifdef OPENGL
float printstr(float x, float y, unsigned char *s, float limitwidth);
#else
float printstr(float x, float y, unsigned char *s,
	 fmfonthandle fa, fmfonthandle fk, float limitwidth);
#endif

#endif
