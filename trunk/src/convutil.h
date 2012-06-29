// convutil.h - Utility functions for character encoding conversion
//
//////////////////////////////////////////////////////////////////////

#ifndef __CONVUTIL_H
#define __CONVUTIL_H

#include <stdio.h>
#include <wchar.h>

#define N_ELM(a)  (sizeof (a)/sizeof (a)[0])

bool ConvGetS (wchar_t* buffer, int buffSz, FILE* fpin, bool isUniIn);
int  ConvGetC (FILE* fpin, bool isUniIn);

bool ConvPrint (FILE* fpout, const wchar_t* wcs, bool isUniOut);

int  ConvCopy (char *dst, int dstSz, const char* tisSrc, bool isUniOut);

wchar_t* Ascii2WcsCopy (wchar_t* dst, const char* src);
wchar_t* Ascii2WcsCat (wchar_t* dst, const char* src);

#endif
