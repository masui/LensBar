/*
	$Date: 1998/04/06 12:28:09 $
	$Revision: 1.1 $
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifdef COCOA
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "font.h"
#include "textbox.h"
#include "graphics.h"

/* static char matched[MAXITEMS]; */

static void
display_textbox(struct _TextBox *tb, int mismatch)
{
	float x,width;
	unsigned char buf[1000];
	char *s;

	textbox(tb->x,tb->y,tb->x+tb->width,tb->y+tb->height,
		mismatch == 0 ? 0xffffff :
		mismatch == 1 ? 0xc0c0c0 :
		mismatch == 2 ? 0x808080 :
		mismatch == 3 ? 0x404040 : 0);

	strcpy(buf,tb->text);
	buf[tb->column] = '\0';

	width = 0.0;
	for(s=buf;*s;s++){
		width += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18,*s);
	}
	x = tb->x + 5.0 + width;
	buf[0] = (tb->text[tb->column] ? tb->text[tb->column] : ' ');
	buf[1] = '\0';
	width = glutBitmapWidth(GLUT_BITMAP_HELVETICA_18,buf[0]);

	setcolor(0x808080);
	rectangle(x, tb->y + 5, x+width,tb->y + 5 + 20);
	setcolor(0x000000);
	moveto(tb->x + 5,tb->y + 5);
	for(s=tb->text;*s;s++){
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*s);
	}
}

#define CTRL(X) ((X)-0x40)

static void
edit_textbox(struct _TextBox *tb, unsigned char c)
{
	int i;
	switch(c){
		case CTRL('H'):
		case 0x7f:
			if(tb->column > 0){
				for(i=tb->column;i<=tb->length;i++)
					tb->text[i-1] = tb->text[i];
				tb->column--;
				if(tb->length > 0) tb->length--;
			}
			break;
		case CTRL('D'):
			if(tb->length > tb->column){
				for(i=tb->column;i<=tb->length;i++)
					tb->text[i] = tb->text[i+1];
				if(tb->length > 0) tb->length--;
			}
			break;
		case CTRL('U'):
			tb->column = tb->length = 0;
			tb->text[0] = '\0';
			break;
		case CTRL('A'):
			tb->column = 0;
			break;
		case CTRL('E'):
			tb->column = tb->length;
			break;
		case CTRL('F'):
			tb->column++;
			if(tb->column > tb->length) tb->column = tb->length;
			break;
		case CTRL('B'):
			tb->column--;
			if(tb->column < 0) tb->column = 0;
			break;
		case CTRL('K'):
			tb->length = tb->column;
			tb->text[tb->length] = '\0';
			break;
		default:
			if(c >= ' '){
				for(i=tb->length;i>=tb->column;i--)
					tb->text[i+1] = tb->text[i];
				tb->text[tb->column++] = c;
				tb->length++;
			}
	}
}

TextBox *
newtextbox(int x, int y, int width, int height)
{
	TextBox *textbox;
	textbox = (TextBox*)malloc(sizeof(TextBox));
	textbox->x = x;
	textbox->y = y;
	textbox->width = width;
	textbox->height = height;
	textbox->length = 
	textbox->column = 0;
	textbox->text[0] = '\0';

	textbox->display = display_textbox;
	textbox->edit = edit_textbox;

	return textbox;
}
