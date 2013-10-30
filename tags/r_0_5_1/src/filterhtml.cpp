// FilterHtml.cpp: implementation of the FilterHtml class.
//
//////////////////////////////////////////////////////////////////////

#include <string.h>
#include <wchar.h>
#include <wctype.h>
#include "filterhtml.h"
#include "worddef.h"
#include "convutil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FilterHtml::FilterHtml (FILE* filein, FILE* fileout,
                        bool isUniIn, bool isUniOut)
  : FilterX (filein, fileout, isUniIn, isUniOut, "<wbr>"),
    chbuff (0)
{
}

bool
FilterHtml::GetNextToken (wchar_t* token, int tokenSz, bool* thaiFlag)
{
  if ((fpin == NULL) || (feof (fpin) != 0) || tokenSz < 2)
    return false;

  if (chbuff == 0)
    {
      wchar_t wc = ConvGetC (fpin, isUniIn);
      if (EOF == wc)
        return false;
      *token = wc;
    }
  else
    {
      *token = chbuff;
      chbuff = 0;
    }
  *thaiFlag = isThaiUni (*token);
  // find a token containing only Thai characters or non-Thai+space characters
  for (;;)
    {
      int nextChar = ConvGetC (fpin, isUniIn);
      if (EOF == nextChar)
        break;
      if (iswspace (nextChar) || L'.' == nextChar
          || isThaiUni (*token) != isThaiUni (nextChar))
        {
          if (*thaiFlag)
            {
              if ('\n' == nextChar)
                {
                  nextChar = ConvGetC (fpin, isUniIn);
                  if (EOF == nextChar)
                    break;
                  if (isThaiUni (nextChar))
                    {
                      if (tokenSz < 2)
                        break;
                      *++token = nextChar;
                      --tokenSz;
                      continue;
                    }
                }
            }
        }
      else
        {
          if (tokenSz < 2)
            break;
          *++token = nextChar;
          --tokenSz;
          continue;
        }

      // not continued -> put back next char & stop
      chbuff = nextChar;
      break;
    }
  *++token = 0;

  return true;
}

void
FilterHtml::Print (const wchar_t* token, bool)
{
  ConvPrint (fpout, token, isUniOut);
}
