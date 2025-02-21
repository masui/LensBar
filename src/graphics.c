/*
	$Date: 2001/01/13 15:09:47 $
	$Revision: 1.1 $
*/
#include <stdio.h>
#include <math.h>
#ifdef COCOA
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "graphics.h"

void
moveto(int x, int y)
{
	glRasterPos2f(x, y);
}

void
setcolor(int color)
{
	glColor3f((color & 0xff)/256.0,
		((color & 0xff00) >> 8)/256.0,
		((color & 0xff0000) >> 16)/256.0);
}

void
rectangle(int x1, int y1, int x2, int y2)
{
	glRectf(x1,y1,x2,y2);
}

void
line(int x1, int y1, int x2, int y2)
{
	glBegin(GL_LINE_STRIP);
		glVertex2i(x1,y1);
		glVertex2i(x2,y2);
	glEnd();
}

void
button(int x1, int y1, int x2, int y2)
{
	setcolor(0x808080);
	rectangle(x1,y1,x2,y2);
	setcolor(0xc0c0c0);
	line(x1,y1,x1,y2);		line(x1,y2,x2,y2);
	line(x1+1,y1+1,x1+1,y2-1);	line(x1+1,y2-1,x2-1,y2-1);
	setcolor(0x404040);
	line(x2,y1,x2,y2);		line(x1,y1,x2,y1);
	line(x2-1,y1+1,x2-1,y2-1);	line(x1+1,y1+1,x2-1,y1+1);
}

void
transparentbutton(int x1, int y1, int x2, int y2, int y3, int y4, int color)
{
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.75,0.75,0.0,0.60);
	rectangle(x1,y3,x2,y4);
	glBlendFunc(GL_ONE,GL_ZERO);

	setcolor(0xc0c0c0);
	line(x1,y1,x1,y2);		line(x1,y2,x2,y2);
	line(x1+1,y1+1,x1+1,y2-1);	line(x1+1,y2-1,x2-1,y2-1);
	setcolor(0x404040);
	line(x2,y1,x2,y2);		line(x1,y1,x2,y1);
	line(x2-1,y1+1,x2-1,y2-1);	line(x1+1,y1+1,x2-1,y1+1);
}

void
pan(int x1, int y1, int x2, int y2, int bgcolor)
{
	setcolor(bgcolor);
	rectangle(x1,y1,x2,y2);
	setcolor(0x404040);
	line(x1,y1,x1,y2);		line(x1,y2,x2,y2);
	line(x1-1,y1-1,x1-1,y2+1);	line(x1-1,y2+1,x2+1,y2+1);
	setcolor(0xc0c0c0);
	line(x2,y1,x2,y2);		line(x1,y1,x2,y1);
	line(x2+1,y1-1,x2+1,y2+1);	line(x1-1,y1-1,x2+1,y1-1);
}

void
textbox(int x1, int y1, int x2, int y2, int bgcolor)
{
	setcolor(bgcolor);
	rectangle(x1,y1,x2,y2);
	setcolor(0x404040);
	line(x1,y1,x1,y2);		line(x1,y2,x2,y2);
	line(x1-1,y1-1,x1-1,y2+1);	line(x1-1,y2+1,x2+1,y2+1);
	setcolor(0xc0c0c0);
	line(x2,y1,x2,y2);		line(x1,y1,x2,y1);
	line(x2+1,y1-1,x2+1,y2+1);	line(x1-1,y1-1,x2+1,y1-1);
}

#ifdef OLD
float
printstr(float x, float y, unsigned char *s, float limitwidth)
{
	int column = 0;
	float width = 0.0;
	if(!s) return 0.0;

	glRasterPos2f(x,y);
	for(;*s && width < limitwidth;s++){
		if(*s == '\t'){
			int i;
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,' ');
			width += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18,' ');
			for(i=column+1;i%8!=0;i++){
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,' ');
				width += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18,' ');
			}
			column = i;
			continue;
		}
		else {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*s);
			width += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18,*s);
			column++;
		}
	}
	return width;
}
#endif


#define JIS16FONT "data/jis16"
#define ROW (0x74-0x21)
#define COLUMN (0x7f-0x21)
static unsigned char *dot16 = NULL;

static int jdotdraw(unsigned char *str, float x, float y)
{
	FILE *f;
	register int i,j,k;
	int size;
	int column;
	unsigned char *p;
	int ub, lb;
	GLubyte dot[16/8*16];
	int ix,iy;

	if(! dot16){
		size = ROW*COLUMN*16*16/8;
		f = fopen(JIS16FONT,"rb");
		if(f != NULL){
			dot16 = (unsigned char*)malloc(size);
			fread(dot16,1,size,f);
			fclose(f);
		}
		else{
			fprintf(stderr, "Can't open font file %s\n", JIS16FONT);
			return NULL;
		}
	}
	ub = *str & 0x7f;
	lb = *(str+1) & 0x7f;
	column = (16+7)/8;
	p = &dot16[(COLUMN*(ub-0x21)+(lb-0x21))*16*column];
	for(i=0;i<32;i++){
		j = i/2; k = i%2;
		dot[i] = p[2*(15-j)+k];
	}
	ix = (int)x; iy = (int)y;
//	glClearColor(1.0,1.0,1.0,1.0);
	glClearIndex(0.0);
//	glColor3f(0.0,0.0,1.0);
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
	glRasterPos2i(ix,iy);
	glBitmap(16,16,0.0,0.0,0.0,0.0,dot);
}

static void *asciifont = GLUT_BITMAP_HELVETICA_18;

void
setasciifont(void *font)
{
	asciifont = font;
}

float
printstr(float x, float y, unsigned char *s, float limitwidth)
{
	int column = 0;
	float width = 0.0;
	float w;
	if(!s) return 0.0;

	for(;*s && width < limitwidth;s++){
		glRasterPos2f(x,y);
		if(*s == '\t'){
			int i;
			glutBitmapCharacter(asciifont,' ');
			w = glutBitmapWidth(asciifont,' ');
			width += w;
			x += w;
			for(i=column+1;i%8!=0;i++){
				glutBitmapCharacter(asciifont,' ');
				w = glutBitmapWidth(asciifont,' ');
				width += w;
				x += w;
			}
			column = i;
			continue;
		}
		else if (*s < 0x80){
			glutBitmapCharacter(asciifont,*s);
			w = glutBitmapWidth(asciifont,*s);
			width += w;
			x += w;
			column++;
		}
		else {
			jdotdraw(s,x,y-2);
			w = 16.0;
			x += w;
			width += w;
			s++;
		}
	}
	return width;
}
