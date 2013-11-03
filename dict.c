/*
	$Date: 2001/01/14 12:03:00 $
	$Revision: 1.1 $
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dict.h"

#ifdef CAL
#define LISTFILE "./list"
#define MAXWORDS 10000
#endif

#ifdef DICTIONARY
#define LISTFILE "eiwa.mds"
#define MAXWORDS 40000
#endif

#ifdef YAHOO
#define LISTFILE "./yahoo.txt"
#define MAXWORDS 280000
#endif

#ifdef CPROGRAM
#define LISTFILE "./lensbar.txt"
#define MAXWORDS 2000
#endif

#ifdef MENU
#define LISTFILE "./menu.txt"
#define MAXWORDS 40000
#endif

#ifdef MENU2
#define LISTFILE "./menu2.txt"
#define MAXWORDS 40000
#endif

#ifdef TVSEARCHER
#define LISTFILE "./TVtitle"
#define ROMANFILE "./TVroman"
#define MAXWORDS 40000
#endif

char *words[MAXWORDS];
char *roman[MAXWORDS];
int nwords = 0;

static FILE *fm,*fi,*fr;

void readdic()
{
	unsigned char buf[10000];
	char s1[1000],s2[1000];
	char *s,*d;
	static int read = 0;
	int pos = 0;
	fprintf(stderr,"midashi = %s\n",LISTFILE);

	if(read) return;

	fm = fopen(LISTFILE,"r");
	if(fm == NULL) exit(0);
//	fi = fopen(IMIFILE,"r");
//	if(fi == NULL) exit(0);
#ifdef TVSEARCHER
	fr = fopen(ROMANFILE,"r");
	if(fr == NULL) exit(0);
#endif

	fprintf(stderr,"Reading %s...\n",LISTFILE);
	while(fgets(buf,10000,fm)){
		*s1 = *s2 = '\0';
		s = buf;
		d = s1;
		while(*s == ' ' || *s == '\t') *d++ = *s++;
		while(*s && *s != '\n' && *s != '\t') *d++ = *s++;
		*d = '\0';
		if(*s == '\t'){
			s++;
			d = s2;
			while(*s && *s != '\n') *d++ = *s++;
			*d = '\0';
		}
		words[nwords] = strdup(s1);
		roman[nwords] = (strlen(s2) > 0 ? strdup(s2) : "");
#ifdef TVSEARCHER
		fgets(buf,10000,fr);
		len = strlen(buf);
		if(buf[len-1] == '\n')
			buf[len-1] = '\0';
		roman[nwords] = strdup(buf);
#endif
/*		fgets(buf,1000,fi); */
/*		pos = ftell(fi); */
		nwords++;
		if(nwords % 5000 == 0)
			fprintf(stderr,"Read %d words...\n",nwords);
	}
	fprintf(stderr,"%d words read.\n",nwords);
// 	fprintf(stderr,"Done\n");
	fclose(fm);
	read = 1;
}
