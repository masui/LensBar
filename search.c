/*
	$Date: 1997/01/07 22:07:49 $
	$Revision: 1.1 $

	表駆動のパタンマッチャ

*/
#ifdef _WINDOWS
#include "stdafx.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "search.h"

#define MAXSTATES 2000
#define MAXCHAR 0x100

#define INITPAT 0x80000000

static unsigned long shiftpat[MAXCHAR];
static unsigned long epsilon;
static unsigned long acceptpat;

typedef unsigned char *Pointer;

Pointer trans[MAXSTATES * MAXCHAR];
int acceptp[MAXSTATES];
static unsigned long statemask;

static unsigned long statepat[MAXSTATES][MAXMISMATCH];
static int nstates = 0;

#define HASHSIZE 9973
static unsigned long statepat_hash[HASHSIZE][MAXMISMATCH];
static int statenum[HASHSIZE];
static int hashpos[MAXSTATES];
static int patlen;

static void
hashclear()
{
	register int i;
	for(i=0;i<HASHSIZE;i++)
		statenum[i] = -1;
}

void
bcopy(char *from, char *to, register int num)
{
	while(num--){ *to++ = *from++; }
}
int
bcmp(char *from, char *to, register int num)
{
	while(num--){ if(*to++ != *from++) return 1; }
	return 0;
}

static int mishits = 0;

static int
patstate(unsigned long *pattern, int mismatch)
{
	register int i;
	register unsigned long *p;
	register unsigned long h = 0;
	int add = 1;
	int mishit = 0;

	/* ハッシュの計算 */
	p = pattern;
	for(i=0;i<=mismatch;i++){
		h = (h >> patlen) ^ *p++;
	}
	h = h % HASHSIZE;
/* printf("patstate(%08x) hash = %d\n",pattern[0],h); */
	for(;;){
		if(statenum[h] < 0){ /* 新エントリ */
			if(nstates >= MAXSTATES-1){
				return -1;
			}
			bcopy((char*)pattern,(char*)statepat_hash[h],
				sizeof(long)*(mismatch+1));
			statenum[h] = nstates;
			hashpos[nstates] = h;
			if(mishit) mishits += mishit;
			return nstates++;
		}
		if(bcmp((char*)statepat_hash[h],(char*)pattern,
			sizeof(long)*(mismatch+1)) == 0){
			return statenum[h];
		}
		else {
			h += add;
			h %= HASHSIZE;
			add++;
			mishit++;
		}
	}
}

#ifdef DEBUG
dumpdfa()
{
	int c;
	int i;
	printf("   ");
/*
	for(c='0'; c<='E'; c++){
		printf(" %c ",c);
	}
	printf("\n");
*/
	for(i=0;i<nstates;i++){
		printf("%2d ",i);
		for(c=0x00; c<0x100; c++){
			printf("%02d ",((Pointer*)(trans[i*MAXCHAR+c])-trans)/MAXCHAR);
		}
		printf("\n");
	}
}
#endif

static int
makedfa(int mismatch)
{
	register int i;
	register long cp,cp1,np1;
	register unsigned long *curpatp, *newpatp;
	int state,newstate;
	unsigned long *curpat, newpat[MAXMISMATCH];
	unsigned int c;
	unsigned long ep = epsilon;
	unsigned long sp;
	Pointer *tp;
	unsigned long *shiftpatp;
	int accept;

	nstates = 0;

	newpat[0] = INITPAT;
	for(i=1;i<=mismatch;i++){
		newpat[i] = 0;
	}
	hashclear();
	mishits = 0;
/* printf("newpat[0] = %08x\n",newpat[0]); */
	patstate(newpat,mismatch);
	for(state=0; state < nstates && state < MAXSTATES; state++){
		Pointer defaultp;
		int defaultstate;

		curpat = statepat_hash[hashpos[state]];
		{
			accept = 0;
			curpatp = curpat;
			newpatp = newpat;
			cp = *curpatp++;
			*newpatp++ = np1 = (cp & ep);
			accept |= (np1 & acceptpat);
			for(i=1;i<=mismatch;i++){
				cp1 = cp;
				cp = *curpatp++;
				*newpatp++ = np1 = 
				   ((cp & ep)
				   | cp1
				   | (cp1 >> 1)
				   | (np1 >> 1) /* ε遷移 */
				   ) & statemask;
				accept |= (np1 & acceptpat);
			}
			defaultstate = patstate(newpat,mismatch);
			defaultp = (Pointer)(trans + (defaultstate << 8));
			acceptp[defaultstate] = accept;
		}
#ifdef ALPHAONLY
		tp = &(trans[state * MAXCHAR + 0x20]);
		shiftpatp = &(shiftpat[0x20]);
		for(c=0x20;c<='z';c++){
#else
		tp = &(trans[state * MAXCHAR + 0x00]);
		shiftpatp = &(shiftpat[0x00]);
		for(c=0;c<0x100;c++){
#endif
			sp = *shiftpatp++;
			if(sp){
				accept = 0;
				curpatp = curpat;
				newpatp = newpat;
				cp = *curpatp++;
				*newpatp++ = np1 = (cp & ep) | ((cp & sp) >> 1);
				accept |= (np1 & acceptpat);
				for(i=1;i<=mismatch;i++){
					cp1 = cp;
					cp = *curpatp++;
					*newpatp++ = np1 = 
					   ((cp & ep)
					   | ((cp & sp) >> 1)
					   | cp1
					   | (cp1 >> 1)
					   | (np1 >> 1) /* ε遷移 */
					   ) & statemask;
					accept |= (np1 & acceptpat);
				}
				newstate = patstate(newpat,mismatch);
				if(newstate < 0) return 0;
				*tp++ = (Pointer)(trans + (newstate << 8));
				acceptp[newstate] = accept;
			}
			else {
				*tp++ = defaultp;
			}
		}
	}
/* printf("states = %d\nmishit = %d\n",nstates,mishits); */
	return 1;
}

void
makepat(unsigned char *s, int mismatch)
{
	int i;
	unsigned long mask = INITPAT;

/* printf("makepat_trans(%s)\n",s); */
	statemask = INITPAT;
	epsilon = 0;
	patlen = 0;
	for(i=0;i<0x100;i++) shiftpat[i] = 0;
	for(;*s;s++){
		if(*s == ' '){
			epsilon |= mask;
		}
		else {
			shiftpat[*s] |= mask;
			if(isupper(*s)){
				shiftpat[tolower(*s)] |= mask;
			}
			else if(islower(*s)){
				shiftpat[toupper(*s)] |= mask;
			}
			mask >>= 1;
			statemask |= mask;
			patlen++;
		}
	}
	acceptpat = mask;
	makedfa(mismatch);
#ifdef DEBUG
	dumpdfa();
#endif
}

int
match(register unsigned char *s)
{
	register Pointer *state = trans;
	for(;*s;s++){
		state = (Pointer*)(state[*s]);
	}
	return acceptp[(state-trans)>>8];
}

#ifdef TEST
main(int argc, char **argv)
{
	unsigned char *pat;
	unsigned char buf[1000];
	int mismatch;
	if(argc <= 1){
		fprintf(stderr,"%% grep pat\n");
		exit(0);
	}
	pat = (unsigned char*)argv[1];
	while(fgets(buf,1000,stdin)){
		buf[strlen(buf)-1] = '\0';
		for(mismatch=0; mismatch<MAXMISMATCH;mismatch++){
			makepat(pat,mismatch);
			if(match(buf)){
				printf("%d: %s\n",mismatch,buf);
				break;
			}
		}
	}
}
#endif
