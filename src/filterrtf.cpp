// FilterRTF.cpp: implementation of the FilterRTF class.
//
//////////////////////////////////////////////////////////////////////
#include <string.h>
#include <ctype.h>
#include <wctype.h>
#include "filterrtf.h"
#include "conv/utf8.h"
#include "convutil.h"
#include "utils.h"
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
  bool        isEmpty() const { return valLen > 0; }

  void reset ();
  void set (char c);
  void appendChar (char c);
  void terminate (ETokenType t);

private:
  ETokenType type;
  char       val[1024];
  size_t     valLen;
};

inline void
RTFToken::reset ()
{
  type = RTK_NONE;
  val[0] = 0;
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
             isUniOut ? L"\\u8203\\'e2\\'80\\'8b" : L"\\u8203\\'3f"), // U+200B
    psState (CS_START),
    curUTFReadBytes (1),
    curUTFWriteBytes (1)
{
  strbuff[0] = 0;
}

bool
FilterRTF::GetNextToken (wchar_t* token, int tokenSz, bool* thaiFlag)
{
  int   nCopy;
  int   bytesToSkip = 0;
  bool  isCopySkipBytes = false;
  bool  charConsumed = true;
  int   c;
  RTFToken rtfToken;

  *token = 0;

  if (!fpin || feof (fpin))
    return false;

  // get first character to determine *thaiFlag
  if (0 != strbuff[0])
    {
      *thaiFlag = isThaiUni (strbuff[0]);
      nCopy = min<int> (tokenSz - 1, wcslen (strbuff));
      token = wcpncpy (token, strbuff, nCopy);
      *token = 0;
      tokenSz -= nCopy;
      wmemmove (strbuff, strbuff + nCopy, wcslen (strbuff + nCopy) + 1);
    }
  else
    {
      while (!charConsumed || (c = fgetc (fpin)) != EOF)
        {
          psState = chgCharState (psState, c, &charConsumed, &rtfToken);
          switch (rtfToken.getType())
            {
            case RTFToken::RTK_TEXT:
              nCopy = min<int> (tokenSz - 1, strlen (rtfToken.getVal()));
              token = Ascii2WcsNCopy (token, rtfToken.getVal(), nCopy);
              *token = 0;
              tokenSz -= nCopy;
              rtfToken.reset ();
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
                    if (tokenSz >= 2)
                      {
                        *token++ = uc;
                        *token = 0;
                        --tokenSz;
                      }
                    *thaiFlag = true;
                    isCopySkipBytes = false;
                  }
                else
                  {
                    int len = strlen (rtfToken.getVal());
                    if (len + 2 <= tokenSz - 1)
                      {
                        token = wcpcpy (token, L"\\u");
                        token = Ascii2WcsCopy (token, rtfToken.getVal());
                        tokenSz -= len + 2;
                      }

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
                      int len = strlen (rtfToken.getVal());
                      if (len + 2 <= tokenSz - 1)
                        {
                          token = wcpcpy (token, L"\\'");
                          token = Ascii2WcsCopy (token, rtfToken.getVal());
                          tokenSz -= len + 2;
                        }
                    }
                  rtfToken.reset ();
                  if (0 == --bytesToSkip)
                    goto thai_flag_determined;
                }
              else
                {
                  // normal ANSI data bytes
                  int ch;
                  sscanf (rtfToken.getVal(), "%x", &ch);
                  rtfToken.reset ();
                  if (tokenSz >= 2)
                    {
                      *token++ = tis2uni (ch);
                      *token = 0;
                      --tokenSz;
                    }
                  *thaiFlag = isThai (ch);
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
              nCopy = min<int> (N_ELM (strbuff) - 1,
                                strlen (rtfToken.getVal()));
              *Ascii2WcsNCopy (strbuff, rtfToken.getVal(), nCopy) = 0;
              rtfToken.reset ();
              goto end_of_chunk;

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
                    if (tokenSz >= 2)
                      {
                        *token++ = uc;
                        *token = 0;
                        --tokenSz;
                      }
                    isCopySkipBytes = false;
                  }
                else
                  {
                    // non-Thai text is found -> prepare to stop
                    wchar_t* p = wcpcpy (strbuff, L"\\u");
                    Ascii2WcsCopy (p, rtfToken.getVal());
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
                      wcscat (strbuff, L"\\'");
                      Ascii2WcsCat (strbuff, rtfToken.getVal());
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
                  int ch;
                  sscanf (rtfToken.getVal(), "%x", &ch);
                  rtfToken.reset ();
                  // if non-Thai -> stop
                  if (!isThai (ch))
                    goto end_of_chunk;
                  if (tokenSz >= 2)
                    {
                      *token++ = tis2uni (ch);
                      *token = 0;
                      --tokenSz;
                    }
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
              nCopy = min<int> (tokenSz - 1, strlen (rtfToken.getVal()));
              token = Ascii2WcsNCopy (token, rtfToken.getVal(), nCopy);
              *token = 0;
              tokenSz -= nCopy;
              rtfToken.reset ();
              if (iswspace (token[-1]))
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
                    strbuff[0] = uc;
                    strbuff[1] = 0;
                    isCopySkipBytes = false;
                  }
                else
                  {
                    int len = strlen (rtfToken.getVal());
                    if (len + 2 <= tokenSz - 1)
                      {
                        token = wcpcpy (token, L"\\u");
                        token = Ascii2WcsCopy (token, rtfToken.getVal());
                        tokenSz -= len + 2;
                      }
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
                      int len = strlen (rtfToken.getVal());
                      if (len + 2 <= tokenSz - 1)
                        {
                          token = wcpcpy (token, L"\\'");
                          token = Ascii2WcsCopy (token, rtfToken.getVal());
                          tokenSz -= len + 2;
                        }
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
                  int ch;
                  sscanf (rtfToken.getVal(), "%x", &ch);
                  rtfToken.reset ();
                  if (isThai (ch))
                    {
                      // Thai chunk is found -> stop
                      strbuff[0] = tis2uni (ch);
                      strbuff[1] = 0;
                      goto end_of_chunk;
                    }
                  if (tokenSz >= 2)
                    {
                      *token++ = ch;
                      *token = 0;
                      --tokenSz;
                    }
                }
              break;
            }
        }
    }

end_of_chunk:
  if (!charConsumed)
    {
      int len = wcslen (strbuff);
      strbuff[len++] = c;
      strbuff[len] = 0;
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
          if (!rtfToken->isEmpty())
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

static void PrintEscapedUTF8 (FILE* fpout, wchar_t wc);

void
FilterRTF::Print (const wchar_t* token, bool thaiFlag)
{
  if (!thaiFlag)
    {
      for (const wchar_t *p = token; *p; ++p)
        {
          printNonThai (*p);
        }
    }
  else
    {
      if (isUniOut)
        {
          for (const wchar_t* p = token; *p; ++p)
            {
              if (isThaiUni (*p))
                {
                  int bytes = UTF8Bytes (*p);
                  if (bytes != curUTFWriteBytes)
                    {
                      curUTFWriteBytes = bytes;
                      fprintf (fpout, "\\uc%d ", curUTFWriteBytes);
                    }

                  fprintf (fpout, "\\u%d", *p);
                  PrintEscapedUTF8 (fpout, *p);
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
              if (isThaiUni (*token))
                {
                  fprintf (fpout, "\\'%02x", uni2tis (*token));
                }
              else if (*token >= 0x80)
                {
                  fprintf (fpout, "\\'%02x", (unsigned char) *token);
                }
              else
                {
                  fprintf (fpout, "%c", (unsigned char) *token);
                }
              token++;
            }
        }
    }
}

void
FilterRTF::printNonThai (wchar_t wc)
{
  switch (wc)
    {
    case L'}':
      if (curUTFWriteBytes != 1)
        {
          fprintf (fpout, " \\uc1 ");
        }
        /* fall through */
    case L'{':
      curUTFWriteBytes = 1;
      break;
    }
  if (wc < 0x80)
    {
      fprintf (fpout, "%c", char (wc));
    }
  else
    {
      PrintEscapedUTF8 (fpout, wc);
    }
}

static void
PrintEscapedUTF8 (FILE* fpout, wchar_t wc)
{
  char utfBuff[10];
  UTF8Writer uw (utfBuff, sizeof utfBuff);
  uw.Write (wc);
  uw.Write (0);
  for (const char* u = utfBuff; *u; ++u)
    {
      fprintf (fpout, "\\'%02x", (unsigned char) *u);
    }
}
