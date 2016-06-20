// convutil.h - Utility functions for character encoding conversion
//
//////////////////////////////////////////////////////////////////////

#ifndef __CONVUTIL_H
#define __CONVUTIL_H

#include <stdio.h>
#include <wchar.h>

bool ConvGetS (wchar_t* buffer, int buffSz, FILE* fpin, bool isUniIn);
int  ConvGetC (FILE* fpin, bool isUniIn);

bool ConvPrint (FILE* fpout, const wchar_t* wcs, bool isUniOut);
wchar_t* ConvStrDup (const char* s, bool isUniIn);

wchar_t* Ascii2WcsCopy (wchar_t* dst, const char* src);
wchar_t* Ascii2WcsCat (wchar_t* dst, const char* src);
wchar_t* Ascii2WcsNCopy (wchar_t* dst, const char* src, int n);
wchar_t* Ascii2WcsNCat (wchar_t* dst, const char* src, int n);

#endif
