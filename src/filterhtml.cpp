// FilterHtml.cpp: implementation of the FilterHtml class.
//
//////////////////////////////////////////////////////////////////////

#include <string.h>
#include <ctype.h>
#include "filterhtml.h"
#include "worddef.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FilterHtml::FilterHtml (FILE* filein, FILE* fileout)
  : FilterX (filein, fileout)
{
  wordBreakStr = "<wbr>";
}

FilterHtml::~FilterHtml ()
{
}

bool
FilterHtml::GetNextToken (char* token, bool* thaiFlag)
{
  char* sttoken;

  if ((fpin == NULL) || (feof (fpin) != 0))
    return false;
  sttoken = token;
  if (chbuff == 0)
    *token = (char) fgetc (fpin);
  else
    {
      *token = chbuff;
      chbuff = 0;               //clear the character buffer.
    }
  *thaiFlag = isThai (*token);
  //find a token that containing only Thai characters or Eng+space characters
  while (feof (fpin) == 0)
    {
      token[1] = (char) fgetc (fpin);
      if (((token[0] ^ token[1]) & 0x80)
          || isspace ((int) token[1]) || token[1] == '.')
        {
          if (*thaiFlag)
            {
              if (token[1] == '\n')
                {
                  if (feof (fpin) != 0)
                    break;
                  chbuff = (char) fgetc (fpin);
                  if (chbuff & 0x80)
                    {
                      token[1] = chbuff;
                      chbuff = 0;
                    }
                  else
                    {
                      // chbuff = token[1];
                      token[1] = 0;
                      break;
                    }
                }
              else
                {
                  chbuff = token[1];
                  token[1] = 0;
                  break;
                }
            }
          else
            {
              // Eng+space Token
              chbuff = token[1];
              token[1] = 0;
              break;
            }
        }
      token++;
    }
  if (feof (fpin) != 0)
    token[1] = 0;
  token = sttoken;

  return true;
}

void
FilterHtml::Print (char* token, bool)
{
  fprintf (fpout, "%s", token);
}
