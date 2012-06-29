// FilterHtml.cpp: implementation of the FilterHtml class.
//
//////////////////////////////////////////////////////////////////////

#include <string.h>
#include <ctype.h>
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
FilterHtml::GetNextToken (char* token, bool* thaiFlag)
{
  if ((fpin == NULL) || (feof (fpin) != 0))
    return false;

  if (chbuff == 0)
    {
      char c = ConvGetC (fpin, isUniIn);
      if (EOF == c)
        return false;
      *token = c;
    }
  else
    {
      *token = chbuff;
      chbuff = 0;
    }
  *thaiFlag = isThai (*token);
  // find a token containing only Thai characters or non-Thai+space characters
  for (;;)
    {
      int nextChar = ConvGetC (fpin, isUniIn);
      if (EOF == nextChar)
        break;
      if (((*token ^ nextChar) & 0x80)
          || isspace (nextChar) || '.' == nextChar)
        {
          if (*thaiFlag)
            {
              if ('\n' == nextChar)
                {
                  nextChar = ConvGetC (fpin, isUniIn);
                  if (EOF == nextChar)
                    break;
                  if (isThai (nextChar))
                    {
                      *++token = nextChar;
                      continue;
                    }
                }
            }
        }
      else
        {
          *++token = nextChar;
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
FilterHtml::Print (char* token, bool)
{
  fprintf (fpout, "%s", token);
}
