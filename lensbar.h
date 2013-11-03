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
                  ←→
        ↑┌────┐↑       ┌────────────────┐
        ││        │marginy  │↑                              │
        ││┌──┐│↓       ││                              │
        │││    ││↑       ││                              │
        │││    │││       ││                              │
        │││┌┐│││       ││                              │
        ││││││││       ││                              │
 sbheight ││└┘││boxh     │svheight                        │
        │││    │││       ││                              │
        │││    │││       ││                              │
        │││    │││       ││                              │
        │││    │││       ││                              │
        ││└──┘│↓       ││                              │
        ││        │         │↓                              │
        ↓└────┘         └────────────────┘
          ←sbwidth→           ←──────svwidth──────→
*/

#define MAXSBHEIGHT 1000
#define MAXITEMS 50
#define MAXMISMATCH 4

struct _LensBar;

typedef int DispLineFunc(struct _LensBar*,int,int,int,int);

typedef struct _LensBar {
	int x,y;			/* 位置 */
	int width,height;		/* 大きさ */
	int margin;			/* スクロールバーとスクロールビューとのマージン */

	int sbx,sby;			/* スクロールバー位置 */
	int sbwidth,sbheight;		/* 大きさ */
	int boxx,boxy;			/* フィルタ領域位置 */
	int boxwidth,boxheight;		/* フィルタ領域大きさ */
	int knobwidth,knobheight;	/* ノブの大きさ */
	int knobx,knoby;		/* ノブの位置 */
	int knobmargin;			/* ノブの高さの最小値 */
	int marginx,marginy;		/* マージン */
	int matchnum[MAXSBHEIGHT];	/* 線一本分に対するマッチ要素数 [0..boxheight] */

	int svx,svy;			/* スクロールビュー位置 */
	int svwidth,svheight;		/* 大きさ */
#ifdef OPENGL
#else
	fmfonthandle afont,kfont;	/* ASCII/漢字フォント */
#endif
	int lineheight;			/* フォントサイズ */

	int clineposy;			/* 着目単語のY座標 */
	double zoom;			/* ズーミングレベル */
	int maxzoom;

	int _itempos[2*MAXITEMS+1];
	int *itempos;			/* 表示される項目の表示位置 */
	int _itemindex[2*MAXITEMS+1];
	int *itemindex;			/* 表示される項目のwords[]内インデクス */
	int firstindex,lastindex;	/* 表示される最初と最後の行のwords[]内インデクス */

	int nitems;			/* データ項目数 */
	int pow;			// nitemsを表現するのに必要なビット数
	char **words;			/* データリスト [0..nitems] */
	char **pats;			/* マッチング対象 [0..nitems] */
	int *doi;			// 重要度 (Degree Of Interest)
	int *indent;			// インデント
	int level[100];
	int maxlevel;
	int maxdoi;

	char *matched;			/* パタンマッチ結果 [0..nitems] */
	char *displayed;		/* レベルnで表示されるかどうか [0..nitems] */
	char *displayed1;		/* レベルn+1表示されるかどうか [0..nitems] */
	int clineindex;			/* 着目行のwords[]内のインデクス */

	int mismatch;			// 検索ミスマッチ数

	DispLineFunc *displine;		/* 1行表示関数 */

	char oldpat[100];		/* 前回のパタン */

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
