// FilterRTF.cpp: implementation of the FilterRTF class.
//
//////////////////////////////////////////////////////////////////////
#include <string.h>
#include <ctype.h>
#include "filterrtf.h"
#include "conv/utf8.h"
#include "worddef.h"

//////////////////////////////////////////////////////////////////////
// class RTFToken
//////////////////////////////////////////////////////////////////////
class RTFToken
{
public:
  enum ETokenType
  {
    RTK_TEXT,
    RTK_BYTE,
    RTK_UNI_COUNT,
    RTK_UNI_CODE,
    RTK_NONE
  };

public:
  RTFToken() : type (RTK_NONE), valLen (0) {}

  ETokenType  getType() const { return type; }
  const char* getVal() const  { return val; }

  void reset ();
  void set (char c);
  void set (char c, ETokenType t);
  void appendChar (char c);
  void terminate (ETokenType t);

private:
  ETokenType type;
  char       val[1024];
  int        valLen;
};

inline void
RTFToken::reset ()
{
  type = RTK_NONE;
  valLen = 0;
}

inline void
RTFToken::set (char c)
{
  val[0] = c;
  val[1] = '\0';
  valLen = 1;
}

inline void
RTFToken::set (char c, ETokenType t)
{
  set (c);
  type = t;
}

inline void
RTFToken::appendChar (char c)
{
  if (valLen + 1 < sizeof val)
    {
      val[valLen++] = c;
    }
}

inline void
RTFToken::terminate (ETokenType t)
{
  val[valLen] = '\0';
  type = t;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FilterRTF::FilterRTF (FILE* filein, FILE* fileout, bool isUniIn, bool isUniOut)
  : FilterX (filein, fileout, isUniIn, isUniOut,
    isUniOut ? "\xe2\x80\x8b" : "\\u8203\\'3f"), // U+200B
    psState (CS_START),
    curUTFReadBytes (1),
    curUTFWriteBytes (1)
{
  strbuff[0] = '\0';
}

bool
FilterRTF::GetNextToken (char* token, bool* thaiFlag)
{
  int   bytesToSkip = 0;
  bool  isCopySkipBytes = false;
  bool  charConsumed = true;
  int   c;
  RTFToken rtfToken;

  //sequence of characters is    \ ' x x (one character for Thai char)
  *token = '\0';

  if ((fpin == NULL) || (feof (fpin)))
    return false;

  // get first character to determine *thaiFlag
  if (strbuff[0] != '\0')
    {
      strcpy (token, strbuff);
      strbuff[0] = '\0';
      *thaiFlag = isThai (*token);
      token += strlen (token);
    }
  else
    {
      while (!charConsumed || (c = fgetc (fpin)) != EOF)
        {
          psState = chgCharState (psState, c, &charConsumed, &rtfToken);
          switch (rtfToken.getType())
            {
            case RTFToken::RTK_TEXT:
              strcpy (token, rtfToken.getVal());
              rtfToken.reset ();
              token += strlen (token);
              *thaiFlag = false;
              goto thai_flag_determined;
              break;

            case RTFToken::RTK_UNI_COUNT:
              sscanf (rtfToken.getVal(), "%d", &curUTFReadBytes);
              rtfToken.reset ();
              break;

            case RTFToken::RTK_UNI_CODE:
              {
                unichar uc;
                sscanf (rtfToken.getVal(), "%d", &uc);
                if (isThaiUni (uc))
                  {
                    *token++ = uni2tis (uc);
                    *token = '\0';
                    *thaiFlag = true;
                    isCopySkipBytes = false;
                  }
                else
                  {
                    strcpy (token, "\\u");
                    strcat (token, rtfToken.getVal());
                    *thaiFlag = false;
                    isCopySkipBytes = true;
                  }
                rtfToken.reset ();
                bytesToSkip = curUTFReadBytes;
              }
              break;

            case RTFToken::RTK_BYTE:
              if (bytesToSkip > 0)
                {
                  // skip bytes after \uDDDD
                  if (isCopySkipBytes)
                    {
                      strcpy (token, "\\'");
                      strcat (token, rtfToken.getVal());
                      token += strlen (token);
                    }
                  rtfToken.reset ();
                  if (0 == --bytesToSkip)
                    goto thai_flag_determined;
                }
              else
                {
                  // normal ANSI data bytes
                  sscanf (rtfToken.getVal(), "%x", token);
                  rtfToken.reset ();
                  ++token;
                  *thaiFlag = true;
                  goto thai_flag_determined;
                }
              break;
            }
        }
    }

thai_flag_determined:

  // continue reading until opposite type of chunk is found
  if (*thaiFlag)
    {
      // continue reading Thai chunk until non-Thai token is found
      while (!charConsumed || (c = fgetc (fpin)) != EOF)
        {
          psState = chgCharState (psState, c, &charConsumed, &rtfToken);
          switch (rtfToken.getType())
            {
            case RTFToken::RTK_TEXT:
              // non-Thai text is found -> stop
              strcpy (strbuff, rtfToken.getVal());
              rtfToken.reset ();
              goto end_of_chunk;
              break;

            case RTFToken::RTK_UNI_COUNT:
              sscanf (rtfToken.getVal(), "%d", &curUTFReadBytes);
              rtfToken.reset ();
              break;

            case RTFToken::RTK_UNI_CODE:
              {
                unichar uc;
                sscanf (rtfToken.getVal(), "%d", &uc);
                if (isThaiUni (uc))
                  {
                    *token++ = uni2tis (uc);
                    *token = '\0';
                    isCopySkipBytes = false;
                  }
                else
                  {
                    // non-Thai text is found -> prepare to stop
                    strcpy (strbuff, "\\u");
                    strcat (strbuff, rtfToken.getVal());
                    isCopySkipBytes = true;
                  }
                rtfToken.reset ();
                bytesToSkip = curUTFReadBytes;
              }
              break;

            case RTFToken::RTK_BYTE:
              if (bytesToSkip > 0)
                {
                  // skip bytes after \uDDDD
                  if (isCopySkipBytes)
                    {
                      strcat (strbuff, "\\'");
                      strcat (strbuff, rtfToken.getVal());
                    }
                  rtfToken.reset ();
                  // decrement bytes to skip
                  // stop if non-Thai Unicode is ended
                  if (0 == --bytesToSkip && isCopySkipBytes)
                    goto end_of_chunk;
                }
              else
                {
                  // normal ANSI data bytes
                  sscanf (rtfToken.getVal(), "%x", token);
                  rtfToken.reset ();
                  ++token;
                }
              break;
            }
        }
    }
  else
    {
      // continue reading non-Thai chunk until Thai token is found
      // To prevent too long chunk, also stop at white space.
      while (!charConsumed || (c = fgetc (fpin)) != EOF)
        {
          psState = chgCharState (psState, c, &charConsumed, &rtfToken);
          switch (rtfToken.getType())
            {
            case RTFToken::RTK_TEXT:
              strcpy (token, rtfToken.getVal());
              rtfToken.reset ();
              token += strlen (token);
              if (isspace (token[-1]))
                goto end_of_chunk;
              break;

            case RTFToken::RTK_UNI_COUNT:
              sscanf (rtfToken.getVal(), "%d", &curUTFReadBytes);
              rtfToken.reset ();
              break;

            case RTFToken::RTK_UNI_CODE:
              {
                unichar uc;
                sscanf (rtfToken.getVal(), "%d", &uc);
                if (isThaiUni (uc))
                  {
                    // Thai chunk is found -> stop
                    strbuff[0] = uni2tis (uc);
                    strbuff[1] = '\0';
                    isCopySkipBytes = false;
                  }
                else
                  {
                    strcpy (token, "\\u");
                    strcat (token, rtfToken.getVal());
                    isCopySkipBytes = true;
                  }
                rtfToken.reset ();
                bytesToSkip = curUTFReadBytes;
              }
              break;

            case RTFToken::RTK_BYTE:
              if (bytesToSkip > 0)
                {
                  // skip bytes after \uDDDD
                  if (isCopySkipBytes)
                    {
                      strcpy (token, "\\'");
                      strcat (token, rtfToken.getVal());
                      token += strlen (token);
                    }
                  rtfToken.reset ();
                  // decrement bytes to skip
                  // stop if Thai Unicode is ended
                  if (0 == --bytesToSkip && !isCopySkipBytes)
                    goto end_of_chunk;
                }
              else
                {
                  // normal ANSI data bytes
                  // Thai TIS-620 is assumed -> stop
                  sscanf (rtfToken.getVal(), "%x", strbuff);
                  rtfToken.reset ();
                  strbuff[1] = '\0';
                  goto end_of_chunk;
                }
              break;
            }
        }
    }

end_of_chunk:
  if (!charConsumed)
    {
      int len = strlen (strbuff);
      strbuff[len++] = c;
      strbuff[len] = '\0';
    }

  return true;
}

FilterRTF::ECharState
FilterRTF::chgCharState (ECharState state, char charIn, bool* charConsumed,
                         RTFToken* rtfToken)
{
  // assume it's consumed by default; let's overwrite it otherwise
  *charConsumed = true;

  //sequence of characters is    \ ' x x (one character)
  //states of this sequence are  1 2 3 4  
  //Assume thai character has format like \'xx.
  //Unicode sequences are:
  // - \ucN : set skip bytes after \uDDDD to N
  // - \uDDDD\'XX\'YY\'ZZ : DDDD decimal Unicode code point,
  //   followed by legacy data bytes "\xXX\xYY\xZZ" to be skipped by N bytes
  //   (as set by the last \ucN)
  //retune current state of character.
  switch (state)
    {
    case CS_START:
      if ('\\' == charIn)
        {
          // flush currently collected token
          if (rtfToken->getVal()[0] != 0)
            {
              rtfToken->terminate (RTFToken::RTK_TEXT);
            }

          return CS_ESCAPE;
        }

      rtfToken->appendChar (charIn);
      rtfToken->terminate (RTFToken::RTK_TEXT);
      return CS_START;

    case CS_ESCAPE:
      switch (charIn)
        {
        case '\'':
          return CS_CH_BYTE;
        case 'u':
          return CS_UNI;
        default:
          // not interesting sequence, make it normal text
          rtfToken->set ('\\');
          rtfToken->appendChar (charIn);
          rtfToken->terminate (RTFToken::RTK_TEXT);
          return CS_START;
        }
      break;

    case CS_CH_BYTE:
      if (isxdigit (charIn))
        {
          rtfToken->appendChar (charIn);
          return CS_CH_BYTE;
        }

      rtfToken->terminate (RTFToken::RTK_BYTE);
      goto end_number;

    case CS_UNI:
      if (isdigit (charIn))
        {
          rtfToken->set (charIn);
          return CS_UNI_CODE;
        }
      if ('c' == charIn)
        return CS_UNI_COUNT;

      // not interesting sequence, make it normal text
      rtfToken->set ('\\');
      rtfToken->appendChar ('u');
      if ('\\' == charIn)
        {
          rtfToken->terminate (RTFToken::RTK_TEXT);
          return CS_ESCAPE;
        }
      rtfToken->appendChar (charIn);
      rtfToken->terminate (RTFToken::RTK_TEXT);
      return CS_START;

    case CS_UNI_COUNT:
      if (isdigit (charIn))
        {
          rtfToken->appendChar (charIn);
          return CS_UNI_COUNT;
        }

      rtfToken->terminate (RTFToken::RTK_UNI_COUNT);
      goto end_number;

    case CS_UNI_CODE:
      if (isdigit (charIn))
        {
          rtfToken->appendChar (charIn);
          return CS_UNI_CODE;
        }

      rtfToken->terminate (RTFToken::RTK_UNI_CODE);
      goto end_number;
    }

  return CS_NONE;

end_number:
  if ('\\' == charIn)
    return CS_ESCAPE;

  *charConsumed = (' ' == charIn);
  return CS_START;
}

void
FilterRTF::Print (char* token, bool thaiFlag)
{
  if (!thaiFlag)
    {
      for (const char *p = token; *p; ++p)
        {
          printNonThai (*p);
        }
    }
  else
    {
      if (isUniOut)
        {
          UTF8Reader ur (token);
          unichar uc;

          for (const char* p = token; ur.Read (uc); p = ur.curPos())
            {
              if (*p & 0x80)
                {
                  const char *q = ur.curPos();
                  if (q - p != curUTFWriteBytes)
                    {
                      curUTFWriteBytes = q - p;
                      fprintf (fpout, "\\uc%d ", curUTFWriteBytes);
                    }
                  fprintf (fpout, "\\u%d", uc);
                  while (p != q)
                    {
                      fprintf (fpout, "\\'%02x", (unsigned char) *p);
                      ++p;
                    }
                }
              else
                {
                  printNonThai (*p);
                }
            }
        }
      else
        {
          while (*token != 0)
            {
              fprintf (fpout, (*token & 0x80) ? "\\'%02x" : "%c",
                       (unsigned char) *token);
              token++;
            }
        }
    }
}

void
FilterRTF::printNonThai (char c)
{
  switch (c)
    {
    case '}':
      if (curUTFWriteBytes != 1)
        {
          fprintf (fpout, " \\uc1 ");
        }
        /* fall through */
    case '{':
      curUTFWriteBytes = 1;
      break;
    }
  fprintf (fpout, "%c", c);
}

