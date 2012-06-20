// convutil.h - Utility functions for character encoding conversion
//
//////////////////////////////////////////////////////////////////////

#ifndef __CONVUTIL_H
#define __CONVUTIL_H

#include <stdio.h>

bool ConvGetS (char* buffer, int buffSz, FILE* fpin, bool isUniIn);
int  ConvGetC (FILE* fpin, bool isUniIn);

int  ConvCopy (char *dst, int dstSz, const char* tisSrc, bool isUniOut);

#endif
