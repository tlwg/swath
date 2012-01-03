// FilterLatex.cpp: implementation of the FilterLatex class.
//
//////////////////////////////////////////////////////////////////////

#include <string.h>
#include "filterlatex.h"
#include "worddef.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FilterLatex::FilterLatex (FILE* filein, FILE* fileout, int latexflag,
                          const char* wordBreakStr)
  : FilterX (filein, fileout, wordBreakStr),
    winCharSet (latexflag == 1),
    latexFlag (latexflag),
    verbatim (false)
{
  buffer[0] = '\0';
}

bool
FilterLatex::GetNextToken (char* token, bool* thaiFlag)
{
  if (buffer[0] == '\0')
    if (fgets (buffer, sizeof buffer, fpin) == NULL)
      return false;

  *token = '\0';
  if (verbatim)
    {
      *thaiFlag = false;
      char* stVer = strstr (buffer, "\\end{verbatim}");
      if (!stVer)
        {
          strcpy (token, buffer);
          buffer[0] = '\0';
        }
      else
        {
          stVer += strlen ("\\end{verbatim}");
          strncpy (token, buffer, stVer - buffer);
          token[stVer - buffer] = '\0';
          memmove (buffer, stVer, strlen (stVer) + 1);
          verbatim = false;
        }
      return true;
    }

  char* curPtr = buffer;

  while (isPunc (*curPtr))
    {
      curPtr++;
    }
  *thaiFlag = isThai (*curPtr);
  curPtr++;
  if (*thaiFlag)
    {
      //for finding thai line + thailine +...+ thailine
      for (;;)
        {
          while (isThai (*curPtr))
            {
              curPtr++;
            }
          if (*curPtr != 0 && *curPtr != '\n')
            {
              int prevLen = strlen (token);
              strncat (token, buffer, curPtr - buffer);
              token[prevLen + (curPtr - buffer)] = '\0';
              //store new buffer
              memmove (buffer, curPtr, strlen (curPtr) + 1);
              return true;
            }
          else
            {
              if (*curPtr == '\n')
                {
                  *curPtr = '\0';
                }
              strcat (token, buffer);
              buffer[0] = '\0';
              //if next a line is thai string, concat next line
              // to current line
              if (fgets (buffer, sizeof buffer, fpin) == NULL)
                {
                  strcat (token, "\n");
                  return true;  //next GetToken() must return false
                }
              curPtr = buffer;
#ifdef CAT_THAI_LINES
              if (!isThai (*curPtr))    //not thai character
#endif
                {
                  strcat (token, "\n");
                  return true;
                }
            }
        }
    }
  else
    {
      while (*curPtr && !isThai (*curPtr))
        {
          curPtr++;
        }
      if (*curPtr != 0)
        {
          strncpy (token, buffer, curPtr - buffer);
          token[curPtr - buffer] = '\0';
          //store new buffer
          memmove (buffer, curPtr, strlen (curPtr) + 1);
        }
      else
        {
          strcpy (token, buffer);
          buffer[0] = '\0';     //clear buffer
        }
      if (strstr (token, "\\begin{verbatim}") != NULL)
        {
          verbatim = true;      //entrance to verbatim model
        }
    }

  return true;                  //no error occur.
}

void
FilterLatex::Print (char* token, bool thaiFlag)
{
  if (thaiFlag && latexFlag != 0)
    {
      unsigned char output[2000];
      AdjustText ((unsigned char*) token, output, sizeof output);
      fprintf (fpout, "%s", output);
    }
  else
    {
      fprintf (fpout, "%s", token);
    }
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

int
FilterLatex::AdjustText (const unsigned char* input, unsigned char* output,
                         int output_sz)
{
  int idxNormal;
  int cntChar = 1;
  unsigned char* out_p = output;

  while (*input != 0 && out_p < output + output_sz)
    {
      *out_p = *input;

      // decompose Sara-Am into Nikhahit + Sara-Aa
      if (*input == 0xd3)
        {
          if (cntChar > 1)
            {
              // long tail + Sara-Am
              if (isThaiLongTailChar (input[-1]))
                {
                  *out_p = winCharSet ? 0x99 : 0x8f;     // left Nikhahit
                }
              else if ((idxNormal = idxTopMark (input[-1])) != -1)
                {
                  // character + tone mark + Sara-Am
                  if (cntChar > 2)
                    {
                      if (isThaiLongTailChar (input[-2]))
                        {
                          // left tone mark
                          *out_p = winCharSet ? WinTopMarksLeft[idxNormal]
                                              : MacTopMarksLeft[idxNormal];
                          // left Nikhahit
                          out_p[-1] = winCharSet ? 0x99 : 0x8f;
                        }
                      else
                        {
                          *out_p = input[-1];
                          out_p[-1] = 0xed;
                        }
                    }
                }
              else
                {
                  *out_p = 0xed;
                }
            }
          else
            {
              *out_p = 0xed;
            }

          if (out_p < output + output_sz)
            {
              *(++out_p) = 0xd2;    // Sara-Aa
            }
        }
      else if ((idxNormal = idxTopMark (*input)) != -1)
        {
          if (cntChar > 1)
            {
              if (isThaiLongTailChar (input[-1]))
                {
                  // long tail + tone mark
                  *out_p = winCharSet ? WinTopMarksLowLeft[idxNormal]
                                      : MacTopMarksLowLeft[idxNormal];
                }
              else if (idxAboveMark (input[-1]) != -1)
                {
                  // character + upper vowel + tone mark
                  if (cntChar > 2)
                    {
                      if (isThaiLongTailChar (input[-2]))
                        {
                          // long tail + upper vowel + tone mark
                          *out_p = winCharSet ? WinTopMarksLeft[idxNormal]
                                              : MacTopMarksLeft[idxNormal];
                        }
                    }
                }
              else
                {
                  // character + tone mark
                  if (input[1] != 0xd3)
                    {
                      *out_p = winCharSet ? WinTopMarksLow[idxNormal]
                                          : MacTopMarksLow[idxNormal];
                    }
                }
            }
        }
      else if ((idxNormal = idxAboveMark (*input)) != -1)
        {
          if (cntChar > 1)
            {
              if (isThaiLongTailChar (input[-1]))
                {
                  // long tail + upper vowel
                  *out_p = winCharSet ? WinAboveMarksLeft[idxNormal]
                                      : MacAboveMarksLeft[idxNormal];
                }
            }
        }
      else if ((idxNormal = idxBelowMark (*input)) != -1)
        {
          if (cntChar > 1)
            {
              switch (input[-1])
                {
                case 0xad:      // Yo Ying
                  out_p[-1] = 0x90;
                  break;
                case 0xae:      // Do Chada
                case 0xaf:      // To Patak
                  *out_p = winCharSet ? WinBelowMarksLow[idxNormal]
                                      : MacBelowMarksLow[idxNormal];
                  break;
                case 0xb0:      // Tho Than
                  out_p[-1] = winCharSet ? 0x80 : 0x9f;
                  break;
                }
            }
        }
      out_p++;
      input++;
      cntChar++;
    }
  *out_p = 0;

  return out_p - output;
}

