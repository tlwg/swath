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
  : FilterX (filein, fileout, wordBreakStr)
{
  buffer[0] = '\0';
  verbatim = false;             // Is in verbatim mode??
  latexFlag = latexflag;
  if (latexFlag == 1)
    winCharSet = true;
}

FilterLatex::~FilterLatex ()
{
}

bool
FilterLatex::GetNextToken (char* token, bool* thaiFlag)
{
  char* curPtr;
  char* stPtr;
  char* stVer;

  if (buffer[0] == '\0')
    if (fgets (buffer, 1999, fpin) == NULL)
      return false;
  curPtr = buffer;
  stPtr = buffer;
  *token = '\0';
  if (verbatim)
    {
      *thaiFlag = false;
      stVer = strstr (buffer, "\\end{verbatim}");
      if (stVer == NULL)
        {
          strcpy (token, buffer);
          buffer[0] = '\0';
          return true;
        }
      else
        {
          stVer += strlen ("\\end{verbatim}");
          strncpy (token, buffer, stVer - buffer);
          token[stVer - buffer] = '\0';
          memmove (buffer, stVer, strlen (stVer) + 1);
          verbatim = false;
          return true;
        }
    }

  while (isPunc (*curPtr))
    curPtr++;
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
              strncat (token, stPtr, curPtr - stPtr);
              token[prevLen + (curPtr - stPtr)] = '\0';
              //store new buffer
              memmove (buffer, curPtr, strlen (curPtr) + 1);
              return true;
            }
          else
            {
              if (*curPtr == '\n')
                *curPtr = 0;
              strcat (token, stPtr);
              buffer[0] = '\0';
              //if next a line is thai string, concat next line
              // to current line
              if (fgets (buffer, 1999, fpin) == NULL)
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
          strncpy (token, stPtr, curPtr - stPtr);
          token[curPtr - stPtr] = '\0';
          //store new buffer
          memmove (buffer, curPtr, strlen (curPtr) + 1);
        }
      else
        {
          strcpy (token, stPtr);
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
  if (thaiFlag)
    {
      if (latexFlag != 0)
        {
          char* output = new char[2000];
          AdjustText ((unsigned char*) token, (unsigned char*) output);
          fprintf (fpout, "%s", output);
          delete output;
        }
      else
        {
          fprintf (fpout, "%s", token);
        }
    }
  else
    {
      fprintf (fpout, "%s", token);
    }
  fflush (fpout);
}

void
FilterLatex::AdjustText (unsigned char* input, unsigned char* output)
{
  int idxNormal;
  int cntChar = 1;
  bool chgchar = false;
  unsigned char* tmpInput;
  unsigned char* tmpOutput;

  tmpInput = input;
  tmpOutput = output;
  while (*input != 0)
    {
      //Sara-Amm must split to Sara-Arr + NiKhaHit(circle)
      chgchar = false;
      if (*input == 211)
        {
          if (cntChar > 1)
            {
              //case Long Tail+Sara-Am
              if (isLongTailChar (input[-1]))
                {
                  *output = (winCharSet == true) ? 153 : 143;     //offset left
                }
              else if ((idxNormal = idxVowelToneMark (input[-1])) < 12)
                {
                  //case character+ToneMark+Sara-Am
                  if (cntChar > 2)
                    {
                      if (isLongTailChar (input[-2]))
                        {
                          //offset left
                          *output = winCharSet ? WinOffsetLeftHigh[idxNormal]
                                               : MacOffsetLeftHigh[idxNormal];
                          output[-1] = winCharSet ? 153 : 143;
                        }
                      else
                        {
                          *output = input[-1];
                          output[-1] = 237;
                        }
                    }
                }
              else
                {
                  *output = 237;
                }
            }
          else
            {
              *output = 237;
            }
          *(++output) = 210;    //Sara-Aa
          chgchar = true;
        }
      else if ((idxNormal = idxVowelToneMark (*input)) < 12)
        {
          if (cntChar > 1)
            {
              if (isLongTailChar (input[-1]))
                {
                  // Long Tail Char + Vowel or Tonemarks.
                  *output = winCharSet ? WinOffsetLeft[idxNormal]
                                       : MacOffsetLeft[idxNormal];
                  chgchar = true;
                }
              else if (idxVowelToneMark (input[-1]) < 12)
                {
                  //char + Vowel + Tone Mark
                  if (cntChar > 2)
                    {
                      if (isLongTailChar (input[-2]))
                        {
                          //Long Tail Char + Vowel + Tone Mark
                          *output = winCharSet ? WinOffsetLeftHigh[idxNormal]
                                               : MacOffsetLeftHigh[idxNormal];
                          chgchar = true;
                        }
                    }
                }
              else
                {
                  //Normal Char + Tone Mark
                  if ((idxNormal < 5) && (input[1] != 211))
                    {
                      *output = winCharSet ? WinOffsetNormal[idxNormal]
                                           : MacOffsetNormal[idxNormal];
                      chgchar = true;
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
                  output[-1] = 144;
                  break;
                case 174:      //DoChaDa
                  chgchar = true;
                  *output = 252;
                  break;
                case 175:      // ToPaTak
                  chgchar = true;
                  *output = 253;
                  break;
                case 176:      //ThoThan
                  output[-1] = winCharSet ? 128 : 159;
                }
            }
        }
      if (!chgchar)
        {
          *output = *input;
        }
      output++;
      input++;
      cntChar++;
    }
  *output = 0;
  input = tmpInput;
  output = tmpOutput;
}

bool
FilterLatex::isLongTailChar (unsigned char ch)
{
  return ch == 187 || ch == 189 || ch == 191;
}

int
FilterLatex::idxVowelToneMark (unsigned char ch)
{
  int i = 0;

  while (i < 12 && WinMacNormal[i] != ch)
    i++;
  return i;
}

