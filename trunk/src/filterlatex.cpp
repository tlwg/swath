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
                          const char* wordBreakStr)
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

//Normal Characters (tonemarks & vowels ) of both of Windows and Macintosh
//Not according Position of vowels or tone mark.

// ---------------------
// 8-bit character codes
// ---------------------
static int CharTopMarks[] =
  {
    0xe8, 0xe9, 0xea, 0xeb, 0xec, // tone marks & Thanthakhat
    0x00
  };
static int CharAboveMarks[] =
  {
    0xd1, 0xd4, 0xd5, 0xd6, 0xd7, // upper vowels
    0xe7,                         // Maitaikhu
    0xed,                         // Nikhahit
    0x00
  };
static int CharBelowMarks[] =
  {
    0xd8, 0xd9, 0xda,             // below vowels + Phinthu
    0x00
  };

// ------------------------
// 8-bit Glyph sets for Mac
// ------------------------
static int MacTopMarksLow[] =
  {
    0x88, 0x89, 0x8a, 0x8b, 0x8c, // tone marks & Thanthakhat
    0x00
  };
static int MacTopMarksLowLeft[] =
  {
    0x83, 0x84, 0x85, 0x86, 0x87, // tone marks & Thanthakhat
    0x00
  };
static int MacTopMarksLeft[] =
  {
    0x98, 0x99, 0x9a, 0x9b, 0x9c, // tone marks & Thanthakhat
    0x00
  };
static int MacAboveMarksLeft[] =
  {
    0x92, 0x94, 0x95, 0x96, 0x97, // upper vowels
    0x93,                         // Maitaikhu
    0x8f,                         // Nikhahit
    0x00
  };
static int MacBelowMarksLow[] =
  {
    0xfc, 0xfd, 0xfe,             // Sara U, Sara Uu, Phinthu
    0x00
  };

// 8-bit Glyph sets for Windows (not unicode)
static int WinTopMarksLow[] =
  {
    0x8b, 0x8c, 0x8d, 0x8e, 0x8f, // tone marks & Thanthakhat
    0x00
  };
static int WinTopMarksLowLeft[] =
  {
    0x86, 0x87, 0x88, 0x89, 0x8a, // tone marks & Thanthakhat
    0x00
  };
static int WinTopMarksLeft[] =
  {
    0x9b, 0x9c, 0x9d, 0x9e, 0x9f, // tone marks & Thanthakhat
    0x00
  };
static int WinAboveMarksLeft[] =
  {
    0x98, 0x81, 0x82, 0x83, 0x84, // upper vowels
    0x9a,                         // Maitaikhu
    0x99,                         // Nikhahit
    0x00
  };
static int WinBelowMarksLow[] =
  {
    0xfc, 0xfd, 0xfe,             // Sara U, Sara Uu, Phinthu
    0x00
  };

static int
idxTopMark (unsigned char ch)
{
  for (int i = 0; CharTopMarks[i]; i++)
    if (CharTopMarks[i] == ch)
      return i;

  return -1;
}

static int
idxAboveMark (unsigned char ch)
{
  for (int i = 0; CharAboveMarks[i]; i++)
    if (CharAboveMarks[i] == ch)
      return i;

  return -1;
}

static int
idxBelowMark (unsigned char ch)
{
  for (int i = 0; CharBelowMarks[i]; i++)
    if (CharBelowMarks[i] == ch)
      return i;

  return -1;
}

