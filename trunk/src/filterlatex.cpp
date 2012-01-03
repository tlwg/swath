// FilterLatex.cpp: implementation of the FilterLatex class.
//
//////////////////////////////////////////////////////////////////////

#include <string.h>
#include "filterlatex.h"
#include "worddef.h"
//Normal Characters (tonemarks & vowels ) of both of Windows and Macintosh
//Not according Position of vowels or tone mark.

//|====OffsetHigh======| |===========Offset Normal============|
static int WinMacNormal[12] =
  { 232, 233, 234, 235, 236, 209, 212, 213, 214, 215, 231, 237 };

//Character Sets for Macintosh
static int MacOffsetLeft[12] =
  { 131, 132, 133, 134, 135, 146, 148, 149, 150, 151, 147, 143 };
static int MacOffsetLeftHigh[5] = { 152, 153, 154, 155, 156 };
static int MacOffsetNormal[5] = { 136, 137, 138, 139, 140 };

//Character Sets for Windows (not unicode)
static int WinOffsetLeft[12] =
  { 134, 135, 136, 137, 138, 152, 129, 130, 131, 132, 154, 153 };
static int WinOffsetLeftHigh[5] = { 155, 156, 157, 158, 159 };
static int WinOffsetNormal[5] = { 139, 140, 141, 142, 143 };

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

static int
idxVowelToneMark (unsigned char ch)
{
  int i = 0;

  while (i < 12 && WinMacNormal[i] != ch)
    i++;
  return i;
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

int
FilterLatex::AdjustText (const unsigned char* input, unsigned char* output,
                         int output_sz)
{
  int idxNormal;
  int cntChar = 1;
  unsigned char* out_p = output;

  while (*input != 0 && out_p - output < output_sz)
    {
      *out_p = *input;

      //Sara-Amm must split to Sara-Arr + NiKhaHit(circle)
      if (*input == 211)
        {
          if (cntChar > 1)
            {
              //case Long Tail+Sara-Am
              if (isThaiLongTailChar (input[-1]))
                {
                  *out_p = (winCharSet == true) ? 153 : 143;     //offset left
                }
              else if ((idxNormal = idxVowelToneMark (input[-1])) < 12)
                {
                  //case character+ToneMark+Sara-Am
                  if (cntChar > 2)
                    {
                      if (isThaiLongTailChar (input[-2]))
                        {
                          //offset left
                          *out_p = winCharSet ? WinOffsetLeftHigh[idxNormal]
                                              : MacOffsetLeftHigh[idxNormal];
                          out_p[-1] = winCharSet ? 153 : 143;
                        }
                      else
                        {
                          *out_p = input[-1];
                          out_p[-1] = 237;
                        }
                    }
                }
              else
                {
                  *out_p = 237;
                }
            }
          else
            {
              *out_p = 237;
            }
          *(++out_p) = 210;    //Sara-Aa
        }
      else if ((idxNormal = idxVowelToneMark (*input)) < 12)
        {
          if (cntChar > 1)
            {
              if (isThaiLongTailChar (input[-1]))
                {
                  // Long Tail Char + Vowel or Tonemarks.
                  *out_p = winCharSet ? WinOffsetLeft[idxNormal]
                                      : MacOffsetLeft[idxNormal];
                }
              else if (idxVowelToneMark (input[-1]) < 12)
                {
                  //char + Vowel + Tone Mark
                  if (cntChar > 2)
                    {
                      if (isThaiLongTailChar (input[-2]))
                        {
                          //Long Tail Char + Vowel + Tone Mark
                          *out_p = winCharSet ? WinOffsetLeftHigh[idxNormal]
                                              : MacOffsetLeftHigh[idxNormal];
                        }
                    }
                }
              else
                {
                  //Normal Char + Tone Mark
                  if ((idxNormal < 5) && (input[1] != 211))
                    {
                      *out_p = winCharSet ? WinOffsetNormal[idxNormal]
                                          : MacOffsetNormal[idxNormal];
                    }
                }
            }
        }
      else if ((*input == 216) || (*input == 217))
        {                       //Sara-Ui. Sara-U
          if (cntChar > 1)
            {
              switch (input[-1])
                {
                case 173:      //YoYing
                  out_p[-1] = 144;
                  break;
                case 174:      //DoChaDa
                  *out_p = 252;
                  break;
                case 175:      // ToPaTak
                  *out_p = 253;
                  break;
                case 176:      //ThoThan
                  out_p[-1] = winCharSet ? 128 : 159;
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

