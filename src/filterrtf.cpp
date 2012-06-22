// FilterRTF.cpp: implementation of the FilterRTF class.
//
//////////////////////////////////////////////////////////////////////
#include <string.h>
#include <ctype.h>
#include "filterrtf.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FilterRTF::FilterRTF (FILE* filein, FILE* fileout, bool isUniIn, bool isUniOut)
  : FilterX (filein, fileout, isUniIn, isUniOut,
    isUniOut ? "\\u8203\\'e2\\'80\\'8b" : "\\'dc"), // U+200B or \xDC
    psState (0)
{
  strbuff[0] = '\0';
}

bool
FilterRTF::GetNextToken (char* token, bool* thaiFlag)
{
  char* sttoken;
  char* tmp;
  int nextState, c;

  //sequence of characters is    \ ' x x (one character for Thai char)
  *token = '\0';
  sttoken = token;

  if ((fpin == NULL) || (feof (fpin)))
    return false;
  if (strbuff[0] == '\0')
    {
      int i;
      for (i = 0; i < 4; i++)
        {
          c = fgetc (fpin);
          if (c == EOF)
            {
              *thaiFlag = false;
              *token = 0;
              token = sttoken;
              return false;
            }
          *token++ = c;
          nextState = chgCharState (c, psState);
          if (nextState <= psState)
            break;
          psState = nextState;
        }
      *thaiFlag = (i == 4);
      if (*thaiFlag)
        {
          *token = 0;
          sscanf (token - 2, "%x", &c);
          token[-4] = (char) c;
          token -= 3;
        }
    }
  else
    {
      strcpy (token, strbuff);
      *thaiFlag = strlen (strbuff) > 1 || *token > 0;
      token += strlen (strbuff);
    }
  if (feof (fpin))
    {
      token = sttoken;
      return false;
    }
  c = fgetc (fpin);
  if (c == EOF)
    {
      *thaiFlag = false;
      *token = 0;
      token = sttoken;
      return false;
    }
  *token = c;


  if (!(*thaiFlag))
    {
      //Loop finding a sequence of Non-Thai characters
      //So to stop finding seq of Non-Thai, must find Thai character
      while (!isspace (*token++) &&
             (nextState = chgCharState (token[-1], psState)) != 4)
        {
          psState = nextState;
          c = fgetc (fpin);
          if (c == EOF)
            break;
          *token = c;
        }
      *token = 0;
      //found thai character
      if (nextState == 4)
        {
          psState = 4;
          sscanf (token - 2, "%x", &c);
          strbuff[0] = (char) c;
          token[-4] = 0;
        }
      else
        {
          psState = 0;
          strbuff[0] = '\0';
        }

    }
  else
    {
      //Loop finding a sequence of Thai characters
      //So to stop finding seq of Thai, must find non-thai character
      while (((nextState = chgCharState (*(token++), psState)) > psState)
             || (psState == 4 && nextState == 1))
        {
          psState = nextState;
          if (psState == 4)
            {
              *token = 0;
              sscanf (token - 2, "%x", &c);
              token[-4] = c;
              token -= 3;
            }
          c = fgetc (fpin);
          if (c == EOF)
            break;
          *token = c;
        }
      *token = 0;
      tmp = &token[-1 * (psState % 4) - 1];
      strcpy (strbuff, tmp);
      *tmp = 0;
    }
  token = sttoken;
  //chbuff save only previous char.
  //Except in case prev strings is \?, chbuff save only ? char
  return true;

}

void
FilterRTF::Print (char* token, bool thaiFlag)
{
  if (!thaiFlag)
    {
      fprintf (fpout, "%s", token);
    }
  else
    {
      while (*token != 0)
        {
          fprintf (fpout, "\\'%02x", (unsigned char) *token);
          token++;
        }
    }
}

int
FilterRTF::chgCharState (char charin, int state)
{
  //sequence of characters is    \ ' x x (one character)
  //states of this sequence are  1 2 3 4  
  //Assume thai character has format like \'xx.
  //retune current state of character.
  switch (state)
    {
    case 0:
      return (charin == '\\') ? 1 : 0;
    case 1:
      if (charin == '\'')
        return 2;
      if (charin == '\\')
        return 1;
      return 0;
    case 2:
      if (isxdigit (charin))
        return 3;
      if (charin == '\\')
        return 1;
      return 0;
    case 3:
      if (isxdigit (charin))
        return 4;
      if (charin == '\\')
        return 1;
      return 0;
    case 4:
      return (charin == '\\') ? 1 : 0;
    }

  return 0;
}

bool
FilterRTF::isThaiChar (char* token, int* state)
{
  //return state and pointer to the character.
  int len;
  int nextstate;

  len = strlen (token);
  if (len < 4)
    {
      token += len;
      *state = 0;
      return false;
    }
  while ((nextstate = chgCharState (*token, *state)) > *state)
    {
      token++;
      *state = nextstate;
    }
  *state = nextstate;
  token++;

  return *state == 4;
}
