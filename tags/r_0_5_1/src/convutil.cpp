// convutil.cpp - Utility functions for character encoding conversion
//
//////////////////////////////////////////////////////////////////////

#include "convutil.h"
#include "conv/utf8.h"
#include "conv/tis620.h"
#include "worddef.h"
#include <string.h>

bool
ConvGetS (wchar_t* buffer, int buffSz, FILE* fpin, bool isUniIn)
{
  char* line = new char[buffSz];
  if (!fgets (line, buffSz, fpin))
    goto fail;

  TextReader* reader;
  if (isUniIn)
    reader = new UTF8Reader (line);
  else
    reader = new TIS620Reader (line);
  unichar uc;
  wchar_t* wp;
  for (wp = buffer; reader->Read (uc); ++wp)
    {
      *wp = uc;
    }
  *wp = 0;
  delete reader;

  delete[] line;
  return true;

fail:
  delete[] line;
  return false;
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

  UTF8Reader ur (uniBuff);
  unichar uc;
  ur.Read (uc);

  return uc;
}

bool
ConvPrint (FILE* fpout, const wchar_t* wcs, bool isUniOut)
{
  char output[MAXLEN * 2];
  TextWriter* writer;
  if (isUniOut)
    writer = new UTF8Writer (output, sizeof output);
  else
    writer = new TIS620Writer (output, sizeof output);

  while (*wcs)
    {
      if (!writer->Write (*wcs++))
        {
          delete writer;
          return false;
        }
    }
  writer->Write (0);
  fprintf (fpout, "%s", output);

  delete writer;
  return true;
}

wchar_t*
Ascii2WcsCopy (wchar_t* dst, const char* src)
{
  while (*src)
    {
      *dst++ = *src++;
    }
  *dst = 0;
  return dst;
}

wchar_t*
Ascii2WcsCat (wchar_t* dst, const char* src)
{
  while (*dst)
    {
      ++dst;
    }
  return Ascii2WcsCopy (dst, src);
}

wchar_t*
Ascii2WcsNCopy (wchar_t* dst, const char* src, int n)
{
  while (*src && n > 0)
    {
      *dst++ = *src++;
      n--;
    }
  if (n > 0)
    {
      *dst = 0;
    }
  return dst;
}

wchar_t*
Ascii2WcsNCat (wchar_t* dst, const char* src, int n)
{
  while (*dst)
    {
      ++dst;
    }
  return Ascii2WcsNCopy (dst, src, n);
}

