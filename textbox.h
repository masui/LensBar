/*
	$Date: 1997/02/22 16:05:01 $
	$Revision: 1.1 $
*/
#ifndef _TEXTBOX_H_
#define _TEXTBOX_H_

#ifndef OPENGL
#include <fmclient.h>
#endif

#define MAXTEXTLEN 1000

struct _TextBox; /* これが必要?? */

typedef void Func0(struct _TextBox *tb, int i);
typedef void Func1(struct _TextBox *tb, char c);

typedef struct _TextBox {
	int x,y;			/* 位置 */
	int width,height;		/* 大きさ */
	unsigned char text[MAXTEXTLEN];
	int length;
	int column;
#ifndef OPENGL
	fmfonthandle asciifont;
#endif

	Func0 *display;
	Func1 *edit;
} TextBox;

TextBox *newtextbox(int,int,int,int);

#endif
