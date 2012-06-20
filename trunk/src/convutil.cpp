// convutil.cpp - Utility functions for character encoding conversion
//
//////////////////////////////////////////////////////////////////////

#include "convutil.h"
#include "conv/conv.h"
#include "worddef.h"
#include <string.h>

bool
ConvGetS (char* buffer, int buffSz, FILE* fpin, bool isUniIn)
{
  if (!isUniIn)
    return fgets (buffer, buffSz, fpin) != NULL;

  char uniBuff[MAXLEN*3 + 1];
  if (!fgets (uniBuff, sizeof uniBuff, fpin))
    return false;
  return conv ('u', 't', uniBuff, buffer, buffSz) == 0;
}

int
ConvCopy (char *dst, int dstSz, const char* tisSrc, bool isUniOut)
{
  int copyLen = 0;
  if (isUniOut)
    {
      char uniBuff[MAXLEN*3 + 1];
      conv ('t', 'u', tisSrc, uniBuff, sizeof uniBuff);
      copyLen = strlen (uniBuff);
      if (copyLen > dstSz)
        return -1;
      strcpy (dst, uniBuff);
    }
  else
    {
      copyLen = strlen (tisSrc);
      if (copyLen > dstSz)
        return -1;
      strcpy (dst, tisSrc);
    }

  return copyLen;
}
