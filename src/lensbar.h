/*
	$Date: 1998/04/02 16:35:08 $
	$Revision: 1.1 $
*/
#ifndef _LENSBAR_H_
#define _LENSBAR_H_

#ifdef OPENGL
#else
#include <fmclient.h>
#endif

#define YES 1
#define NO 0

/*
                 marginx
                  $B"+"*(B
        $B",(#(!(!(!(!($",(B       $B(#(!(!(!(!(!(!(!(!(!(!(!(!(!(!(!(!($(B
        $B("("(B        $B("(Bmarginy  $B("",(B                              $B("(B
        $B("("(#(!(!($(""-(B       $B("("(B                              $B("(B
        $B("("("(B    $B("("",(B       $B("("(B                              $B("(B
        $B("("("(B    $B("("("(B       $B("("(B                              $B("(B
        $B("("("(#($("("("(B       $B("("(B                              $B("(B
        $B("("("("("("("("(B       $B("("(B                              $B("(B
 sbheight $B("("(&(%("("(Bboxh     $B("(Bsvheight                        $B("(B
        $B("("("(B    $B("("("(B       $B("("(B                              $B("(B
        $B("("("(B    $B("("("(B       $B("("(B                              $B("(B
        $B("("("(B    $B("("("(B       $B("("(B                              $B("(B
        $B("("("(B    $B("("("(B       $B("("(B                              $B("(B
        $B("("(&(!(!(%(""-(B       $B("("(B                              $B("(B
        $B("("(B        $B("(B         $B(""-(B                              $B("(B
        $B"-(&(!(!(!(!(%(B         $B(&(!(!(!(!(!(!(!(!(!(!(!(!(!(!(!(!(%(B
          $B"+(Bsbwidth$B"*(B           $B"+(!(!(!(!(!(!(Bsvwidth$B(!(!(!(!(!(!"*(B
*/

#define MAXSBHEIGHT 1000
#define MAXITEMS 50
#define MAXMISMATCH 4

struct _LensBar;

typedef int DispLineFunc(struct _LensBar*,int,int,int,int);

typedef struct _LensBar {
	int x,y;			/* $B0LCV(B */
	int width,height;		/* $BBg$-$5(B */
	int margin;			/* $B%9%/%m!<%k%P!<$H%9%/%m!<%k%S%e!<$H$N%^!<%8%s(B */

	int sbx,sby;			/* $B%9%/%m!<%k%P!<0LCV(B */
	int sbwidth,sbheight;		/* $BBg$-$5(B */
	int boxx,boxy;			/* $B%U%#%k%?NN0h0LCV(B */
	int boxwidth,boxheight;		/* $B%U%#%k%?NN0hBg$-$5(B */
	int knobwidth,knobheight;	/* $B%N%V$NBg$-$5(B */
	int knobx,knoby;		/* $B%N%V$N0LCV(B */
	int knobmargin;			/* $B%N%V$N9b$5$N:G>.CM(B */
	int marginx,marginy;		/* $B%^!<%8%s(B */
	int matchnum[MAXSBHEIGHT];	/* $B@~0lK\J,$KBP$9$k%^%C%AMWAG?t(B [0..boxheight] */

	int svx,svy;			/* $B%9%/%m!<%k%S%e!<0LCV(B */
	int svwidth,svheight;		/* $BBg$-$5(B */
#ifdef OPENGL
#else
	fmfonthandle afont,kfont;	/* ASCII/$B4A;z%U%)%s%H(B */
#endif
	int lineheight;			/* $B%U%)%s%H%5%$%:(B */

	int clineposy;			/* $BCeL\C18l$N(BY$B:BI8(B */
	double zoom;			/* $B%:!<%_%s%0%l%Y%k(B */
	int maxzoom;

	int _itempos[2*MAXITEMS+1];
	int *itempos;			/* $BI=<($5$l$k9`L\$NI=<(0LCV(B */
	int _itemindex[2*MAXITEMS+1];
	int *itemindex;			/* $BI=<($5$l$k9`L\$N(Bwords[]$BFb%$%s%G%/%9(B */
	int firstindex,lastindex;	/* $BI=<($5$l$k:G=i$H:G8e$N9T$N(Bwords[]$BFb%$%s%G%/%9(B */

	int nitems;			/* $B%G!<%?9`L\?t(B */
	int pow;			// nitems$B$rI=8=$9$k$N$KI,MW$J%S%C%H?t(B
	char **words;			/* $B%G!<%?%j%9%H(B [0..nitems] */
	char **pats;			/* $B%^%C%A%s%0BP>](B [0..nitems] */
	int *doi;			// $B=EMWEY(B (Degree Of Interest)
	int *indent;			// $B%$%s%G%s%H(B
	int level[100];
	int maxlevel;
	int maxdoi;

	char *matched;			/* $B%Q%?%s%^%C%A7k2L(B [0..nitems] */
	char *displayed;		/* $B%l%Y%k(Bn$B$GI=<($5$l$k$+$I$&$+(B [0..nitems] */
	char *displayed1;		/* $B%l%Y%k(Bn+1$BI=<($5$l$k$+$I$&$+(B [0..nitems] */
	int clineindex;			/* $BCeL\9T$N(Bwords[]$BFb$N%$%s%G%/%9(B */

	int mismatch;			// $B8!:w%_%9%^%C%A?t(B

	DispLineFunc *displine;		/* 1$B9TI=<(4X?t(B */

	char oldpat[100];		/* $BA02s$N%Q%?%s(B */

#ifdef OPENGL			    
	enum { UPPERBAR, ONKNOB, LOWERBAR} onbar;
	double origzoom;
	int oldselindex;
	int origclineposy;
	int origclineindex;
	int startx, starty;
	int mousex, mousey;
	int dragging;
#endif
} LensBar;

LensBar *
newlb(char **words, char **pats, int nitems, int x, int y, int width, int height);

void freelb(LensBar *lb);

#ifdef OPENGL
void lbmotion(LensBar *lb, int x, int y);
void lbmouse(LensBar *lb, int button, int state, int x, int y);
#endif

#endif
