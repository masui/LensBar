/*
	$Date: 2001/01/14 13:48:18 $
	$Revision: 1.8 $

	LensBar: フィルタ機能付きスクロール
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

#include "lensbar.h"
#include "search.h"
#include "font.h"
#include "graphics.h"

#define LBBGCOLOR	0xc0ffff
#define LBTEXTCOLOR	0x000000
#define LBSELCOLOR	0xc0c0c0

void calcdoi(LensBar *lb);
void calcmatch(LensBar *lb, char *pattern);
static int displine(LensBar *lb, int line, int x, int y, int displayp);

extern LensBar *lb;

int dayofweek(int year,int mon,int day)
{
	if(mon == 1 || mon == 2){
		year--;
		mon += 12;
	}
	return (year + year/4 - year/100 + year/400
		+ (13*mon+8)/5 + day) % 7;
}

void
doubleclick(LensBar *lb,int ind)
{
#ifdef CAL
	char *s;
	char command[100];
	char arg1[100],arg2[100],arg3[100];
	int x,y,xi,yi;
	double scale;

	s = lb->words[ind];
	while(*s == '\t') s++;
	for(;*s;s++) if(*s == '\t') break;
	for(;*s;s++) if(*s != '\t') break;
// fprintf(stderr,"DoubleClick: %s\n",s);
	sscanf(s,"%s %s %s %s",command,arg1,arg2,arg3);
	if(strcmp(command,"cal")==0){
		int year,mon,day;
		int dow;
		year = atoi(arg1); mon = atoi(arg2); day = atoi(arg3);
		if(day == 0){
			xi = (mon-1) % 3 * (7+1);
			yi = (year-1997) * 32 + (mon-1) / 3 * 8;
			yi = 100 - yi;
			x = xi * 200 + 600;
			y = yi * 160 - 340;
			scale = 0.5;
		}
		else {
			dow = dayofweek(year,mon,day);
			xi = (mon-1) % 3 * (7+1) + dow;
			yi = (year-1997) * 32 + (mon-1)/3*8 + (day+dayofweek(year,mon,1)-1)/ 7;
			yi = 100 - yi;
			x = xi * 200;
			y = yi * 160;
			scale = 3;
		}
		printf(".pad moveto %d %d %f 1000\n",x,y,scale);
	}
	else if(strcmp(command,"map")==0){
		float latitude,longitude;
		float x,y,scale;
		float min,sec;
		// 10進数に変換
		latitude = atof(arg1);
		min = (int)((latitude - (int)latitude) * 100);
		min = min / 60.0;
		sec = (int)((latitude * 100 - (int)(latitude * 100)) * 1000);
		sec = sec / 36000.0;
		latitude = (int)latitude + min + sec;

		longitude = atof(arg2);
		min = (int)((longitude - (int)longitude) * 100);
		min = min / 60.0;
		sec = (int)((longitude * 100 - (int)(longitude * 100)) * 1000);
		sec = sec / 36000.0;
		longitude = (int)longitude + min + sec;

		scale = atof(arg3);

		x = (longitude - 139.3) * 10000 * 0.85;
		y = (latitude - 35.3) * 10000;

		printf(".pad moveto %f %f %f 1000\n",x,y,scale);

	}
	else if(strcmp(command,"person")==0){
		int x,y;
		x = atoi(arg1); y = atoi(arg2);
		scale = 3.0;
		printf(".pad moveto %d %d %f 1000\n",x,y,scale);
	}
	fflush(stdout);
#endif
}

void
timerfunc(int value)
{
	int v;
	if(lb->dragging){
		if(lb->onbar == UPPERBAR){
			v = lb->mousey - (lb->knoby + lb->knobheight);
			v /= 3;
			if(v >= lb->lineheight) v = lb->lineheight;
			
			if(lb->clineposy - v >= 0){
				lb->clineposy -= v;
			}
			else {
				lb->clineposy = lb->itempos[-1];
				lb->clineindex = lb->itemindex[-1];
				lb->clineposy -= v;
			}
			display();
		}
		else if(lb->onbar == LOWERBAR){
			v = lb->knoby - lb->mousey;
			v /= 3;
			if(v >= lb->lineheight) v = lb->lineheight;
			
			if(lb->clineposy + v < lb->height - lb->lineheight){
				lb->clineposy += v;
			}
			else {
				lb->clineposy = lb->itempos[1];
				lb->clineindex = lb->itemindex[1];
				lb->clineposy += v;
			}
			display();
		}
	}
	if(lb->dragging){
		glutTimerFunc(100,timerfunc,0);
	}
}

static int
bits(int n) // nを表現するのに必要なビット数
{
	int i,b;
	i = 1;
	for(b=0;i<n;b++){
		i *= 2;
	}
	if(b == 0) b = 1;
	return b;
}

LensBar *
newlb(char **words, char **pats, int nitems, int x, int y, int width, int height)
{
	int i,j,n;
	char *s;
	LensBar *lb;

	lb = (LensBar*)malloc(sizeof(LensBar));

	lb->x = x;
	lb->y = y;
	lb->width = width;
	lb->height = height;
	lb->words = words;
	lb->pats = pats;
	lb->oldpat[0] = '\0';

	lb->margin = 20;
	lb->sbx = lb->x + lb->margin;
	lb->sby = lb->y + lb->margin;
	lb->sbwidth = 30;
	lb->sbheight = lb->height - 2 * lb->margin;
	lb->marginx = 5;
	lb->marginy = 20;
	lb->boxx = lb->sbx + lb->marginx;
	lb->boxy = lb->sby + lb->marginy;
	lb->boxwidth = lb->sbwidth - 2 * lb->marginx;
	lb->boxheight = lb->sbheight - 2 * lb->marginy;
	lb->knobmargin = 15;
	lb->knobwidth = lb->sbwidth - 2 * (lb->marginx + 5);
	lb->knobx = lb->sbx + lb->marginx + 5;

	lb->svx = lb->sbx + lb->sbwidth + /* lb->margin */ + 30;
	lb->svy = lb->y + lb->margin;
	lb->svwidth = lb->width - lb->sbwidth - 3 * lb->margin;
	lb->svheight = lb->height - 2 * lb->margin;
	lb->zoom = 0.0;
	i = 1;
	for(lb->maxzoom=0;;lb->maxzoom++){
		if(i >= nitems) break;
		i *= 2;
	}
// printf("maxzoom=%d\n",lb->maxzoom);
	lb->clineposy = lb->svheight / 2;

	lb->lineheight = 24.0;

	lb->itemindex = lb->_itemindex+MAXITEMS;
	lb->itempos = lb->_itempos+MAXITEMS;
	lb->firstindex = 
	lb->lastindex = 0;

	lb->nitems = nitems;
	lb->matched = (char*)malloc(nitems);
	lb->displayed = (char*)malloc(nitems);
	lb->displayed1 = (char*)malloc(nitems);
	lb->clineindex = 0;
	for(lb->pow=0,n=1;n<nitems;){ /* nitemsを表現するのに必要なビット数 */
		lb->pow++;
		n *= 2;
	}
	lb->doi = (int*)calloc(nitems,sizeof(int));

	lb->maxlevel = -1;
	for(i=0;i<100;i++) lb->level[i] = 0;
	lb->indent = (int*)calloc(nitems,sizeof(int));
	for(i=0;i<nitems;i++){
		j = 0; 
		for(s=words[i];*s;s++){
			if(isspace(*s)) j++;
			else break;
		}
		lb->indent[i] = j;
		lb->level[j]++;
		if(j > lb->maxlevel) lb->maxlevel = j;
	}
	lb->maxdoi = 0;
	for(i=0;i<=lb->maxlevel;i++){
		lb->maxdoi += bits(lb->level[i]);
	}

	lb->displine = displine;

	calcmatch(lb," ");

	return lb;
}

void
freelb(LensBar *lb)
{
	free(lb->matched);
	free(lb->displayed);
	free(lb->displayed1);
	free(lb->doi);
	free(lb);
}

static powi(int x, int y)
{
	int result = 1;
	while(y-- > 0) result *= x;
	return result;
}

lbdisplay(LensBar *lb)
{
	int i,j,k,pow;
	int x,y;
	int firstline,lastline;
	int izoom,oldizoom;
	double fzoom;
	int idx;
	int p0,p1;
	int h;
int lasty;
int lasti;

//printf("lbdisplay\n");
//calcdoi(lb); // この場所で大丈夫か??? 不要だと思う 2001/1/14 

	/*
		スクロールビュー表示
	*/
	izoom = (int)floor(lb->zoom);
	fzoom = lb->zoom - (double)izoom;

	// izoomのレベルとizoom+1のレベルで表示すべき行を計算
	// (izoom+1の方が表示が多いようにする)
	for(i=0;i<lb->nitems;i++){
		lb->displayed[i] = (lb->doi[i] >= izoom ? 0 : 1);
		lb->displayed1[i] = (lb->doi[i] >= izoom+1 ? 0 : 1);
	}

	pan(lb->svx,lb->svy,lb->svx +lb->svwidth,lb->svy+lb->svheight,LBBGCOLOR);
	x = 10;
	y = lb->clineposy;

	// clineindexの選択行バックグラウンド
	setcolor(LBSELCOLOR);
	rectangle(lb->svx+2, lb->svy+y-4, lb->svx+lb->svwidth-1, lb->svy+y-4+lb->lineheight);

	// clineindexの行表示
	setcolor(0x000000);
	h = lb->displine(lb,lb->clineindex,lb->svx+x,lb->svy+y,YES);

	lb->itemindex[0] = lb->clineindex;
	lb->itempos[0] = y;
lasty = y;
lasti = lb->clineindex;

	idx = lb->clineindex;
	i=idx-1;
	p0 = lb->clineposy + h;
//	p0 = lb->clineposy;
	lb->firstindex = 0;

	while(1){
		if(i<0) break;
		if(lb->displayed[i]){
			k = 0;
			for(j=idx-1;j>=i;j--){
				if(lb->displayed1[j]) k += lb->displine(lb,j,lb->svx+x,lb->svy+y,NO);
			}
			p1 = lb->clineposy + k;
			y = (int)((double)p1 * fzoom + (double)p0 * (1.0-fzoom));
			if(y >= lb->svheight - lb->lineheight) break;
/* 行間に線を引く */
#define LBGAPCOLOR	0x608888
setcolor(LBGAPCOLOR);
if(lasti - i > 1){
rectangle(lb->svx + 1, lb->svy+(lasty+y+lb->lineheight-4)/2-2,
       lb->svx + lb->svwidth - 1, lb->svy+(lasty+y+lb->lineheight-4)/2-1);
/* rectangle(lb->svx + 20, lb->svy+lasty+h-4, lb->svx + lb->svwidth - 20, lb->svy+y-4); */
}

lasty = y;
lasti = i;

			h = lb->displine(lb,i,lb->svx+x,lb->svy+y,YES);
			if(lb->firstindex < -MAXITEMS+1) break;
			lb->firstindex--;
			lb->itemindex[lb->firstindex] = i;
			lb->itempos[lb->firstindex] = y;
			p0 += h;
		}
		i--;
	}

	idx = lb->clineindex;
lasty = lb->itempos[0];
lasti = lb->clineindex;
	i=idx+1;
	p0 = lb->clineposy;
	lb->lastindex = 0;
	while(1){
		if(i>=lb->nitems) break;
		if(lb->displayed[i]){
			k = 0;
			for(j=idx+1;j<i;j++){
				if(lb->displayed1[j]) k += lb->displine(lb,j,lb->svx+x,lb->svy+y,NO);
			}
			p1 = lb->clineposy - k;
			y = (int)((double)p1 * fzoom + (double)p0 * (1.0-fzoom));
			h = lb->displine(lb,i,lb->svx+x,lb->svy+y,NO);
			if(y-h < 0) break;
setcolor(LBGAPCOLOR);
if(i - lasti > 1){
/* line(lb->svx + 20, lb->svy+(lasty+y+lb->lineheight-4)/2 - h, lb->svx + lb->svwidth - 20,	 lb->svy+(lasty+y+lb->lineheight-4)/2 - h); */
// line(lb->svx + 20, lb->svy+(lasty+y-4)/2, lb->svx + lb->svwidth - 20, lb->svy+(lasty+y-4)/2);
rectangle(lb->svx + 1, lb->svy+(lasty+y-4)/2-2,
       lb->svx + lb->svwidth - 1, lb->svy+(lasty+y-4)/2-1);
}
lasty = y-h;
lasti = i;
			h = lb->displine(lb,i,lb->svx+x,lb->svy+y - h,YES);
			if(lb->lastindex >= MAXITEMS-1) break;
			lb->lastindex++; 
			lb->itemindex[lb->lastindex] = i;
			lb->itempos[lb->lastindex] = y-h;
			p0 -= h;
		}
		i++;
	}
	/*
		スクロールバー表示
	*/
	pan(lb->sbx,lb->sby,lb->sbx+lb->sbwidth,lb->sby+lb->sbheight,
		0xc0ffff);
	// スクロールバー中の1行が何マッチに相当するかを計算
	for(i=0;i<lb->boxheight;i++){
		lb->matchnum[i] = 0;
	}
	for(i=0;i<lb->nitems;i++){
		lb->matchnum[i * lb->boxheight / lb->nitems] += lb->displayed[i];
	}
	for(i=lb->boxheight-1;i>=0;i--){
		int color;
		float v1[2],v2[2];
		float density = (float)(lb->matchnum[i]) * (float)(lb->boxheight) / (float)lb->nitems;
		int displayed = 0;

		int r,g,b;
		int rl,gl,bl,rd,gd,bd;
		int dark,light,mainc;
		r = 255; g = 100, b = 38;
r = 160; g = 0; b = 120;
r = 0x80; g = 0x80; b = 0;
r = 0x80; g = 0x80; b = 0xff;
		rl = r * 3 / 2; if(rl > 0x100) rl = 255;
		gl = g * 3 / 2; if(gl > 0x100) gl = 255;
		bl = b * 3 / 2; if(bl > 0x100) bl = 255;
		rd = r / 2; if(rd > 0x100) rd = 255;
		gd = g / 2; if(gd > 0x100) gd = 255;
		bd = b / 2; if(bd > 0x100) bd = 255;
		dark = (bd << 16 | gd << 8 | rd);
		light = (bl << 16 | gl << 8 | rl);
		mainc = (b << 16 | g << 8 | r);
		if(density > 0.0){
			long v[2];
			setcolor(light);
			line(lb->boxx,lb->boxy + lb->boxheight - i,
			     lb->boxx + lb->boxwidth,lb->boxy + lb->boxheight - i);
			line(lb->boxx,lb->boxy + lb->boxheight - i - 1,
			     lb->boxx + 1,lb->boxy + lb->boxheight - i - 1);
			setcolor(mainc);
			line(lb->boxx + 2,lb->boxy + lb->boxheight - i - 1,
			     lb->boxx + lb->boxwidth - 2,lb->boxy + lb->boxheight - i - 1);
			setcolor(dark);
			line(lb->boxx + lb->boxwidth - 1,lb->boxy + lb->boxheight - i - 1,
			     lb->boxx + lb->boxwidth,lb->boxy + lb->boxheight - i - 1);
			displayed = 1;
		}
		else {
			if(displayed){
				displayed = 0;
				setcolor(dark);
				line(lb->boxx,lb->boxy + lb->boxheight - i,
				     lb->boxx + lb->boxwidth,lb->boxy + lb->boxheight - i);
			}		
			else {
				setcolor(0xc0ffff);
				line(lb->boxx,lb->boxy + lb->boxheight - i,
				     lb->boxx + lb->boxwidth,lb->boxy + lb->boxheight - i);
			}
		}
	}
	// ノブ
	lastline = lb->itemindex[lb->lastindex] * lb->boxheight / lb->nitems;
	firstline = lb->itemindex[lb->firstindex] * lb->boxheight / lb->nitems;
/*
	lb->knoby = lb->boxy + lb->boxheight - lastline - lb->knobmargin;
	lb->knobheight = lastline - firstline + 2 * lb->knobmargin;
*/
	if(lastline - firstline < 30){
		lb->knobmargin = (30-(lastline-firstline))/2;
		lb->knobheight = 30;
		lb->knoby = lb->boxy + lb->boxheight - lastline - lb->knobmargin;
	}
	else {
		lb->knobmargin = 0;
		lb->knobheight = lastline - firstline;
		lb->knoby = lb->boxy + lb->boxheight - lastline;
	}

//	button(lb->knobx, lb->knoby, lb->knobx + lb->knobwidth, lb->knoby + lb->knobheight);
	transparentbutton(lb->sbx-5,lb->knoby,
		lb->sbx+lb->sbwidth+5,lb->knoby+lb->knobheight,
		lb->boxy + lb->boxheight - lastline,
		lb->boxy + lb->boxheight - firstline,
		0xa000cccc);

	// 台形表示
	{
		GLint v[2];
glEnable(GL_POINT_SMOOTH);
glEnable(GL_LINE_SMOOTH);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
		glColor4f(0.75,0.75,0.0,0.63);
		glBegin(GL_POLYGON);
			v[0] = lb->knobx + lb->knobwidth;
			v[0] = lb->sbx+lb->sbwidth+5,lb->knoby+lb->knobheight;
			v[1] = lb->boxy + lb->boxheight - firstline;
			glVertex2iv(v);
			v[0] = lb->svx - 1;
			v[1] = lb->svy+lb->svheight;
			glVertex2iv(v);
			v[0] = lb->svx - 1;
			v[1] = lb->svy;
			glVertex2iv(v);
			v[0] = lb->sbx+lb->sbwidth+5,lb->knoby+lb->knobheight;
			v[1] = lb->boxy + lb->boxheight - lastline;
			glVertex2iv(v);
			v[0] = lb->sbx+lb->sbwidth+5,lb->knoby+lb->knobheight;
			v[1] = lb->boxy + lb->boxheight - firstline;
			glVertex2iv(v);
		glEnd();
		glBlendFunc(GL_ONE,GL_ZERO);
glDisable(GL_POINT_SMOOTH);
glDisable(GL_LINE_SMOOTH);

	}
}

static int
displine(LensBar *lb, int line, int x, int y, int displayp)
/*
	1行表示ルーチン。表示内容によって変更可能。
*/
{

#ifdef CAL
	if(displayp){
		char buf[1000],*s;
		setcolor(LBTEXTCOLOR);
		strcpy(buf,lb->words[line]);
		s = buf;
		while(*s == '\t') s++;
		for(;*s;s++) if(*s == '\t') break;
		*s = '\0';
		printstr((float)x,(float)y,buf,lb->svwidth-20);
	}
#else
	if(displayp){
		setcolor(LBTEXTCOLOR);
		printstr((float)x,(float)y,lb->words[line],lb->svwidth-20);
	}
#endif
	return lb->lineheight;
}

void
lbmotion(LensBar *lb, int x, int y)
{
	int v;
	y = glutGet(GLUT_WINDOW_HEIGHT) - y;
	lb->mousex = x;
	lb->mousey = y;

	// スクロールビュー内?
	if(lb->startx >= lb->svx && lb->startx <= lb->svx + lb->svwidth &&
	   lb->starty >= lb->svy && lb->starty <= lb->svy + lb->svheight){
		/*	
			上下
		*/
		lb->clineposy = lb->origclineposy + (y - lb->starty);
		/*	
			左右
		*/
#ifdef YAHOO
		lb->zoom = lb->origzoom + (double)(x - lb->startx)/4.0;
#else
		lb->zoom = lb->origzoom + (double)(x - lb->startx)/10.0;
#endif
	}
	else if(lb->startx >= lb->boxx && lb->startx <= lb->boxx + lb->boxwidth &&
	   lb->starty >= lb->boxy && lb->starty <= lb->boxy + lb->boxheight){
		if(lb->onbar == ONKNOB){
			lb->clineindex = lb->origclineindex -
				(y - lb->starty) * lb->nitems / lb->boxheight;
			if(lb->clineindex < 0) lb->clineindex = 0;
			if(lb->clineindex >= lb->nitems) lb->clineindex = lb->nitems - 1;
			for(;lb->clineindex >= 0; lb->clineindex--){
				if(lb->displayed[lb->clineindex]) break;
			}
			if(lb->clineindex < 0){
				lb->clineindex = 0;
				for(;lb->clineindex < lb->nitems; lb->clineindex++){
					if(lb->displayed[lb->clineindex]) break;
				}
			}
//			calcdoi(lb);
		}
	}
//	display(); // これを使うとイベントがキューに溜ってしまう
	glutPostRedisplay(); // これが正しい使いかた??
}

void
lbmouse(LensBar *lb, int button, int state, int x, int y)
{
	int i, sel;
	int selindex;

	y = glutGet(GLUT_WINDOW_HEIGHT) - y;
	lb->mousex = x;
	lb->mousey = y;

// printf("lbmouse %d:%d\n",x,y);
	lb->startx = x;
	lb->starty = y;
	lb->origzoom = lb->zoom;
	lb->dragging = 0;

	// スクロールビュー内?
	if(x >= lb->svx && x <= lb->svx + lb->svwidth &&
	   y >= lb->svy && y <= lb->svy + lb->svheight &&
	   state == GLUT_DOWN){
		sel = lb->lastindex;
		for(i=lb->firstindex;i<lb->lastindex;i++){
			if((lb->itempos[i] + lb->lineheight +
				lb->itempos[i+1]) / 2 < y - lb->svy){
				sel = i;
				break;
			}
		}
		selindex = lb->itemindex[sel];
		if(lb->oldselindex == selindex){
			doubleclick(lb,selindex);
		}
		lb->oldselindex = selindex;
		lb->clineindex = selindex;
		calcdoi(lb);
		lb->origclineposy = lb->clineposy = lb->itempos[sel];
	}
	else if(x >= lb->boxx && x <= lb->boxx + lb->boxwidth &&
	   y >= lb->boxy && y <= lb->boxy + lb->boxheight &&
	   state == GLUT_DOWN){
		lb->origclineindex = lb->clineindex;
		if(y < lb->knoby + lb->knobheight && y > lb->knoby){
			lb->onbar = ONKNOB;
		}
		else if(y >= lb->knoby + lb->knobheight){
			glutTimerFunc(100,timerfunc,0);
			lb->dragging = 1;
			lb->onbar = UPPERBAR;
		}
		else if(y <= lb->knoby){
			glutTimerFunc(100,timerfunc,0);
			lb->dragging = 1;
			lb->onbar = LOWERBAR;
		}
	}
}

void
calcmatch(LensBar *lb, char *pattern)
{
	register int i,j;
	int mismatch;
	int nmatch;
	char pat[100];

	if(strcmp(lb->oldpat,pattern) == 0) return;
	strcpy(lb->oldpat,pattern);

	sprintf(pat,"%s ",pattern);
	for(mismatch=0;mismatch<MAXMISMATCH ;mismatch++){
		makepat(pat,mismatch);
		for(i=nmatch=0;i<lb->nitems;i++){
			nmatch += (lb->matched[i] = match((unsigned char*)(lb->pats[i])) ? 1 : 0);
		}
		if(nmatch) break;
	}
	lb->mismatch = mismatch;
	if(!nmatch) return;
// printf("nmatch = %d\n",nmatch);
	lb->zoom = 1.0;
	if(! lb->matched[lb->clineindex]){
		for(i=j=0;i<lb->nitems;i++){
			if(lb->matched[i]){
				if(++j > nmatch/2){
					lb->clineindex = i;
					break;
				}
			}
		}
		lb->clineposy = lb->height / 2;
	}

	calcdoi(lb); // 重要度を計算!!!
}

void
calcdoi(LensBar *lb)
//
//	clineindexを中心とした重要度を計算する。
//
{
	register int i,j;
	int d,k,count,mask;
	char *s;
	int n;
	int countmatch, countnomatch;
#if defined(DICTIONARY) || defined(FILEBROWSER) || defined(TVSEARCHER)
	count = 0;
	for(i=lb->clineindex;i>=0;i--){
		if(! lb->matched[i]){
			lb->doi[i] = 100;
		}
		else {
			for(mask=1,j=0;j<lb->pow;j++,mask<<=1){
				if(mask & count) break;
			}
			lb->doi[i] = -j;
			count++;
		}
	}
	count = 0;
	for(i=lb->clineindex;i<lb->nitems;i++){
		if(! lb->matched[i]){
			lb->doi[i] = 100;
		}
		else {
			for(mask=1,j=0;j<lb->pow;j++,mask<<=1){
				if(mask & count) break;
			}
			lb->doi[i] = -j;
			count++;
		}
	}
#endif
#if defined(YAHOO) || defined(CPROGRAM) || defined(MENU) || defined(MENU2) || defined(CAL)
	int curlevel;
	int b;

	curlevel = 0;
	for(k=0;k<=lb->maxlevel;k++){
		b = bits(lb->level[k]);
		countmatch = countnomatch = 1;
		for(i=lb->clineindex-1;i>=0;i--){
			if(k != lb->indent[i]) continue;

			if(lb->matched[i]){
				for(mask=1,j=0;j<b;j++,mask<<=1){
					if(mask & countmatch) break;
				}
				lb->doi[i] = curlevel + bits(lb->level[k]) - j - 1 - lb->maxdoi;
				countmatch++;
			}
			else {
				for(mask=1,j=0;j<b;j++,mask<<=1){
					if(mask & countnomatch) break;
				}
				lb->doi[i] = curlevel + bits(lb->level[k]) - j;
				countnomatch++;
			}
		}
		curlevel += bits(lb->level[k]);
	}

	curlevel = 0;
	for(k=0;k<=lb->maxlevel;k++){
		countmatch = countnomatch = 1;
		for(i=lb->clineindex+1;i<lb->nitems;i++){
			if(k != lb->indent[i]) continue;

			if(lb->matched[i]){
				for(mask=1,j=0;j<b;j++,mask<<=1){
					if(mask & countmatch) break;
				}
				lb->doi[i] = curlevel + bits(lb->level[k]) - j - 1 - lb->maxdoi;
				countmatch++;
			}
			else {
				for(mask=1,j=0;j<b;j++,mask<<=1){
					if(mask & countnomatch) break;
				}
				lb->doi[i] = curlevel + bits(lb->level[k]) - j;
				countnomatch++;
			}
		}
		curlevel += bits(lb->level[k]);
	}
#endif
#if defined(CPROGRAM)
	for(i=0;i<lb->nitems;i++){
		s = lb->words[i];
		if(strncmp(s,"//",2)==0) lb->doi[i] = lb->maxdoi+1;
		else if(*s == '\0') lb->doi[i] = lb->maxdoi+1;
	}
#endif
}
