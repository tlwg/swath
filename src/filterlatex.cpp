// FilterLatex.cpp: implementation of the FilterLatex class.
//
//////////////////////////////////////////////////////////////////////

#include <string.h>
#include <wctype.h>
#include "filterlatex.h"
#include "worddef.h"
#include "convutil.h"
#include "utils.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FilterLatex::FilterLatex (FILE* filein, FILE* fileout,
                          bool isUniIn, bool isUniOut,
                          const wchar_t* wordBreakStr)
  : FilterX (filein, fileout, isUniIn, isUniOut, wordBreakStr),
    verbatim (false)
{
  buffer[0] = 0;
}

static int
consumeToken (wchar_t* token, int tokenSz, wchar_t* buffer, int nChars)
{
  int nCopy = min<int> (nChars, tokenSz - 1);
  wcsncpy (token, buffer, nCopy);
  token[nCopy] = 0;

  return nCopy;
}

bool
FilterLatex::GetNextToken (wchar_t* token, int tokenSz, bool* thaiFlag)
{
  if (0 == buffer[0] && !ConvGetS (buffer, N_ELM (buffer), fpin, isUniIn))
    return false;

  *token = 0;
  if (verbatim)
    {
      *thaiFlag = false;
      wchar_t* curPtr = buffer;
      wchar_t* stVer = wcsstr (buffer, L"\\end{verbatim}");
      if (!stVer)
        {
          curPtr += consumeToken (token, tokenSz, buffer, wcslen (buffer));
          wmemmove (buffer, curPtr, wcslen (curPtr) + 1);
        }
      else
        {
          int verbLen = wcslen (L"\\end{verbatim}");
          if (stVer - buffer + verbLen < tokenSz)
            {
              stVer += verbLen;
            }
          curPtr += consumeToken (token, tokenSz, buffer, stVer - buffer);
          wmemmove (buffer, curPtr, wcslen (curPtr) + 1);
          verbatim = false;
        }
      return true;
    }

  wchar_t* beginPtr = buffer;
  wchar_t* curPtr = buffer;
  wchar_t* lastAllowed = buffer + tokenSz - 1;
  wchar_t* pToken = token;

  while (curPtr < lastAllowed && *curPtr && iswpunct (*curPtr))
    {
      curPtr++;
    }
  if (curPtr == lastAllowed || 0 == *curPtr)
    goto get_cur_token_return;

  *thaiFlag = isThaiUni (*curPtr);
  if (*thaiFlag)
    {
      //for finding thai line + thailine +...+ thailine
      for (;;)
        {
          while (curPtr < lastAllowed && *curPtr && isThaiUni (*curPtr))
            {
              curPtr++;
            }
          if (curPtr == lastAllowed || 0 == *curPtr)
            goto get_cur_token_return;

          if (*curPtr != 0 && *curPtr != L'\n')
            {
              // non-Thai char is found
              beginPtr += consumeToken (pToken, tokenSz,
                                        beginPtr, curPtr - beginPtr);
              wmemmove (buffer, beginPtr, wcslen (beginPtr) + 1);
              return true;
            }

          // EOL is found
          if (*curPtr == L'\n')
            {
              *curPtr = 0;
            }
          int tkLen = consumeToken (pToken, tokenSz,
                                    beginPtr, curPtr - beginPtr);
          pToken += tkLen;
          tokenSz -= tkLen;
          lastAllowed -= tkLen;
          //if next a line is thai string, concat next line
          // to current line
          if (!ConvGetS (buffer, N_ELM (buffer), fpin, isUniIn))
            {
              if (tokenSz > 1)
                {
                  wcscpy (pToken, L"\n");
                }
              return true;  //next GetToken() must return false
            }
          beginPtr = curPtr = buffer;
#ifdef CAT_THAI_LINES
          if (!isThaiUni (*curPtr))    //not thai character
#endif
            {
              if (tokenSz > 1)
                {
                  wcscpy (pToken, L"\n");
                }
              return true;
            }
        }
    }
  else
    {
      while (curPtr < lastAllowed && *curPtr && !isThaiUni (*curPtr))
        {
          curPtr++;
        }

      beginPtr += consumeToken (pToken, tokenSz, beginPtr, curPtr - beginPtr);
      wmemmove (buffer, beginPtr, wcslen (beginPtr) + 1);

      if (wcsstr (token, L"\\begin{verbatim}") != NULL)
        {
          verbatim = true;      //entrance to verbatim model
        }
    }

  return true;

get_cur_token_return:
  beginPtr += consumeToken (pToken, tokenSz, beginPtr, curPtr - beginPtr);
  wmemmove (buffer, beginPtr, wcslen (beginPtr) + 1);
  return true;
}

void
FilterLatex::Print (const wchar_t* token, bool thaiFlag)
{
  ConvPrint (fpout, token, isUniOut);
  fflush (fpout);
}

