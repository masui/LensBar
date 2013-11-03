/*
	$Date: 1998/04/09 11:05:09 $
	$Revision: 1.4 $
*/
#include <stdio.h>
#ifdef COCOA
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "font.h"
#include "search.h"
#include "lensbar.h"
#include "textbox.h"
#include "dict.h"
#include "graphics.h"

#ifdef CAL
#define WINPOSX 0
#define WINPOSY 600
#define WINWIDTH 410
#define WINHEIGHT 450
#define WINTITLE "CAL"
#endif

#ifdef DICTIONARY
#define WINPOSX 100
#define WINPOSY 100
#define WINWIDTH 310
#define WINHEIGHT 350
#define WINTITLE "Dictionary"
#endif

#ifdef YAHOO
#define WINPOSX 100
#define WINPOSY 100
#define WINWIDTH 710
#define WINHEIGHT 700
#define WINTITLE "Editor"
#endif

#ifdef CPROGRAM
#define WINPOSX 100
#define WINPOSY 100
#define WINWIDTH 710
#define WINHEIGHT 700
#define WINTITLE "Editor"
#endif

#ifdef MENU
#define WINPOSX 100
#define WINPOSY 100
#define WINWIDTH 560
#define WINHEIGHT 500
#define WINTITLE "Menu"
#endif

#ifdef MENU2
#define WINPOSX 100
#define WINPOSY 100
#define WINWIDTH 560
#define WINHEIGHT 500
#define WINTITLE "Menu"
#endif

#ifdef FILEBROWSER
#define WINPOSX 100
#define WINPOSY 100
#define WINWIDTH 770
#define WINHEIGHT 730
#define WINTITLE "FileBrowser"
extern char *files[];
extern int nfiles;
extern char *lines[];
extern int nlines;
#endif

#ifdef TVSEARCHER
#define WINPOSX 100
#define WINPOSY 100
#define WINWIDTH 710
#define WINHEIGHT 700
#define WINTITLE "TV Searcher"
#endif

LensBar *lb,*lb2;
TextBox *tb;

void display(void);
void reshape(int w, int h);
void kbcommand(char *key);

void motionfunc(int x, int y)
{
	lbmotion(lb, x, y);
}

void mousefunc(int button, int state, int x, int y)
{
	lbmouse(lb, button, state, x, y);
}

void kbdfunc(unsigned char key,int x, int y){
	static char prevtext[100] = "";
	char keybuf[2];
	keybuf[0] = key;
	keybuf[1] = '\0';
	if(key == '\n' || key == '\r'){
		doubleclick(lb,lb->clineindex);
	}
	kbcommand(keybuf);
	if(strcmp(tb->text,prevtext)){
		calcmatch(lb,tb->text);
		display();
		strcpy(prevtext,tb->text);
	}
}


main(int argc, char **argv)
{

#ifdef FILEBROWSER
	int index = -1;
	char filename[1000];
	char *s;
#endif

	glutInit(&argc, argv);
	glutInitWindowSize(WINWIDTH, WINHEIGHT);
	glutInitWindowPosition(WINPOSX, WINPOSY);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow(WINTITLE);
	glEnable(GL_BLEND);

#ifdef FILEBROWSER
	readfiles();
#else
	readdic();
#endif

#ifdef CAL
	printf("source \"/usr/people/masui/Systems/pad-0.9p1-irix/draw/pad.tcl\"\n");
	printf("source \"demo.pad\"\n");
	fflush(stdout);
#endif

#ifdef CAL
	lb = newlb(words,words,nwords,0,0,400,400);
	tb = newtextbox(20,400,270,30);
#endif
#ifdef DICTIONARY
	lb = newlb(words,words,nwords,0,0,300,300);
	tb = newtextbox(20,300,270,30);
#endif
#ifdef YAHOO
	lb = newlb(words,roman,nwords,0,0,700,650);
	tb = newtextbox(20,650,270,30);
#endif
#ifdef CPROGRAM
	lb = newlb(words,words,nwords,0,0,700,650);
	tb = newtextbox(20,650,270,30);
#endif
#ifdef MENU
	lb = newlb(words,words,nwords,0,0,550,450);
	tb = newtextbox(20,450,270,30);
#endif
#ifdef MENU2
	lb = newlb(words,words,nwords,0,0,550,450);
	tb = newtextbox(20,450,270,30);
#endif
#ifdef FILEBROWSER
	lb = newlb(files,roman,nfiles,0,280,660,300);
	lb->lineheight = 15.0;
	tb = newtextbox(20,580,300,30);
#endif

#ifdef TVSEARCHER
	lb = newlb(words,roman,nwords,0,0,700,650);
	tb = newtextbox(20,650,270,30);
#endif

	printf("xxxxx\n");

	display();

	glutDisplayFunc(display);
	glutKeyboardFunc(kbdfunc);
	glutReshapeFunc(reshape);
	glutMotionFunc(motionfunc);
	glutMouseFunc(mousefunc);

	glutMainLoop();

	return 0;
}

void
reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D((GLfloat)0, (GLfloat)w, (GLfloat)0, (GLfloat)h);
//gluOrtho2D(0,310,0,360);
	glMatrixMode(GL_MODELVIEW);
}

void
display(void)
{
	setcolor(0xc08080);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.5,0.5,0.75,1.0);

	lbdisplay(lb);
	if(lb2) lbdisplay(lb2);

	tb->display(tb,lb->mismatch);

	glutSwapBuffers();
}

void
kbcommand(char *key)
{
	unsigned char *s;

	for(s=key;*s;s++){
		tb->edit(tb,*s);
	}
}
