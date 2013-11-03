/*
	$Date: 1997/01/07 22:08:45 $
	$Revision: 1.1 $
*/
#ifndef _SEARCH_H_
#define _SEARCH_H_

void makepat(unsigned char *pat, int mismatch);
int match(unsigned char *text);

#define MAXMISMATCH 4

#endif
