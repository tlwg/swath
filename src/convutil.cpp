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
ConvGetC (FILE* fpin, bool isUniIn)
{
  if (!isUniIn)
    return fgetc (fpin);

  int c = fgetc (fpin);
  if (EOF == c)
    return EOF;

  if (0 == (c & 0x80))
    return c;

  char uniBuff[10];
  uniBuff[0] = char (c);
  int  len = 1;
  for (c <<= 1; c & 0x80; c <<= 1)
    {
      int inp = fgetc (fpin);
      if (EOF == inp)
        return EOF;
      uniBuff[len++] = char (inp);
    }
  uniBuff[len] = '\0';

  char buffer[2];
  if (conv ('u', 't', uniBuff, buffer, sizeof buffer) != 0)
    return EOF;

  return buffer[0];
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
