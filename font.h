/*
	$Date: 1997/02/22 19:20:39 $
	$Revision: 1.2 $
*/
#ifndef _FONT_H_
#define _FONT_H_

#ifdef OPENGL
#else

#include <fmclient.h>

void fontinit();

extern fmfonthandle gothic;
extern fmfonthandle helvetica;

extern fmfonthandle defaultafont;
extern fmfonthandle defaultkfont;

#endif

#endif
