// ==========================================
// Author: Chaivit Poovanich
// October 14, 1998
// Modified by : Paisarn Charoenpornsawat
// 1999
// ==========================================
// New features.
//       1. Support many formats: LaTeX, HTML,
//               Text, RTF (for Opensource, Emacs 1999)
//       2. Remove garbage characters.
//               (for a. virach requirement 2000)
//       3. Select wordseg algorithms, Bi-gram
//           Maximal Matching, Longest Matching
//               (for Opensource, Emacs 1999)
// ===========================================


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

#include "worddef.h"
#include "filefilter.h"
#include "filterx.h"
#include "longwordseg.h"
#include "maxwordseg.h"
#include "convutil.h"
#include "utils.h"

enum TextToken {
  TTOK_EOT    = -1,
  TTOK_WSPACE = 0,
  TTOK_THAI   = 1,
  TTOK_ETC    = 2,
};

static TextToken SplitToken (wchar_t** str, wchar_t* token);

static bool
OpenDict (Dict* dict, const char* dictpath)
{
  // Dict search order:
  // 1. dictpath, if not NULL, exit on failure
  // 2. ${SWATHDICT} env, if not NULL, continue on failure
  // 3. current dir, continue on failure
  // 4. WORDSEGDATADIR macro
  if (dictpath)
    {
      if (dict->open (dictpath))
        return true;
    }
  else
    {
      dictpath = getenv ("SWATHDICT");
      if (dictpath && dict->open (dictpath))
        return true;

      if (dict->open ("."))
        return true;

      if (dict->open (WORDSEGDATA_DIR))
        return true;
    }

  return false;
}

static AbsWordSeg*
InitWordSegmentation (const char* method)
{
  if (method)
    {
      if (strcmp (method, "long") == 0)
        return new LongWordSeg;
      else if (strcmp (method, "max") == 0)
        return new MaxWordSeg;
    }

  // fallback
  return new MaxWordSeg;
}

static void
ExitWordSegmentation (AbsWordSeg* wseg)
{
  delete wseg;
}

static void
WordSegmentation (AbsWordSeg* wseg, const Dict* dict, const wchar_t* wbr,
                  const wchar_t* line, wchar_t* output, int outputSz)
{
  short int *seps = new short int [outputSz];
  int nSeps = wseg->WordSeg (dict, line, seps, outputSz);

  const wchar_t* pLine = line;
  int wbrLen = wcslen (wbr);
  for (int i = 0; i < nSeps; i++)
    {
      const wchar_t* wordEnd = line + seps[i];
      int outLen = wordEnd - pLine;
      if (outLen >= outputSz)
        break;
      wcsncpy (output, pLine, outLen);
      output[outLen] = 0;
      output += outLen;
      outputSz -= outLen;

      if (wbrLen >= outputSz)
        break;
      wcsncpy (output, wbr, wbrLen);
      output += wbrLen;
      outputSz -= wbrLen;

      pLine = wordEnd;
    }

  wcsncpy (output, pLine, outputSz);

  delete[] seps;
}

static void
Version ()
{
  printf ("swath " VERSION "\n");
  printf
    ("Copyright (C) 2001-2006 Phaisarn Charoenpornsawat <phaisarn@nectec.or.th>\n"
     "Copyright (C) 2001-2014 Theppitak Karoonboonyanan <theppitak@gmail.com>\n"
     "License: GNU GPL version 2 or later <http://gnu.org/licenses/gpl-2.0.html>\n"
     "This is free software; you are free to change and redistribute it.\n"
     "There is NO WARRANTY, to the extent permitted by law.\n");
}

static void
Usage (int verbose)
{
  fprintf (stderr,
           "Usage: swath [mule|-v] [-b \"delimiter\"] [-d dict-dir]\n"
           "[-f html|rtf|latex|lambda] [-m long|max] [-u {u|t},{u|t}] [-help]\n");
  if (verbose)
    {
      fprintf (stderr,
               "Options:\n"
               "\tmule : for use with mule\n"
               "\t-v   : verbose mode\n"
               "\t-b   : define a word delimiter string for the output\n"
               "\t-d   : specify dictionary path\n"
               "\t-f   : specify format of the input\n"
               "\t\thtml     : HTML file\n"
               "\t\trtf      : RTF file\n"
               "\t\tlatex    : LaTeX file\n"
               "\t\tlambda   : The input and output are same as latex, except that\n"
               "\t\t           the word delimiter is ^^^^200b\n"
               "\t-m   : choose word matching scheme when analyzing\n"
               "\t\tlong     : longest matching scheme\n"
               "\t\tmax      : maximal matching scheme\n"
               "\t-u   : specify encodings of input and output in 'i,o' form,\n"
               "\t       for input and output respectively, where 'i', 'o' is one of:\n"
               "\t\tu        : The input/output is in UTF-8\n"
               "\t\tt        : The input/output is in TIS-620\n"
               "\t-help: display this help message\n");
    }
}

#ifndef WORDSEGDATA_DIR
#define WORDSEGDATA_DIR "/usr/local/lib/wordseg"
#endif

int
main (int argc, char* argv[])
{
  const char* wbr = NULL;
  const char* dictpath = NULL;
  const char* method = NULL;
  const char* fileformat = NULL;
  const char* unicode = NULL;
  bool verbose = false;
  bool muleMode;
  bool thaiFlag;

  muleMode = false;
  for (int iargc = 1; iargc < argc; iargc++)
    {
      if (strcmp ("mule", argv[iargc]) == 0)
        {
          muleMode = true;
          verbose = false;
        }
      else if (strcmp ("-b", argv[iargc]) == 0 && iargc + 1 < argc)
        {
          wbr = argv[++iargc];
        }
      else if (strcmp ("-d", argv[iargc]) == 0 && iargc + 1 < argc)
        {
          dictpath = argv[++iargc];
        }
      else if (strcmp ("-f", argv[iargc]) == 0 && iargc + 1 < argc)
        {
          fileformat = argv[++iargc];
        }
      else if (strcmp ("-v", argv[iargc]) == 0 ||
               strcmp ("--verbose", argv[iargc]) == 0)
        {
          verbose = true;
        }
      else if (strcmp ("-m", argv[iargc]) == 0 && iargc + 1 < argc)
        {
          method = argv[++iargc];
        }
      else if (strcmp ("-u", argv[iargc]) == 0 && iargc + 1 < argc)
        {
          unicode = argv[++iargc];
        }
      else if (strcmp ("-V", argv[iargc]) == 0 ||
               strcmp ("--version", argv[iargc]) == 0)
        {
          Version ();
          return 0;
        }
      else if (strcmp ("-help", argv[iargc]) == 0 ||
               strcmp ("--help", argv[iargc]) == 0)
        {
          Usage (1);
          return 1;
        }
      else
        {
          Usage (0);
          return 1;
        }
    }

  if (verbose)
    printf ("*** Word Segmentation ***\n");

  Dict dict;
  if (!OpenDict (&dict, dictpath))
    return 1;

  AbsWordSeg* wseg = InitWordSegmentation (method);
  if (!wseg)
    return 1;

  bool  isUniIn = false;
  bool  isUniOut = false;
  if (unicode != NULL)
    {                           //Option -u
      if (unicode[0] == 'u')
        {
          isUniIn = true;
        }
      if (unicode[2] == 'u')
        {
          isUniOut = true;
        }
    }

  wchar_t wLine[MAXLEN];
  wchar_t wsegOut[MAXLEN * 2];

  if (fileformat != NULL)
    {
      FilterX* FltX = FileFilter::CreateFilter (stdin, stdout,
                                                isUniIn, isUniOut,
                                                fileformat);
      if (FltX == NULL)
        {
          fprintf (stderr, "Invalid file format: %s\n", fileformat);
          ExitWordSegmentation (wseg);
          return 1;
        }
      while (FltX->GetNextToken (wLine, N_ELM (wLine), &thaiFlag))
        {
          if (!thaiFlag)
            {
              FltX->Print (wLine, thaiFlag);
              continue;
            }
          WordSegmentation (wseg, &dict, FltX->GetWordBreak(), wLine,
                            wsegOut, N_ELM (wsegOut));
          FltX->Print (wsegOut, thaiFlag);
        }
      delete FltX;
    }
  else
    {
      wchar_t* wcwbr_buff = NULL;
      const wchar_t* wcwbr = L"|";
      if (wbr)
        {
          wcwbr = wcwbr_buff = ConvStrDup (wbr, isUniOut);
        }
      while (!feof (stdin))
        {
          if (verbose)
            printf ("Input : ");

          if (!ConvGetS (wLine, N_ELM (wLine), stdin, isUniIn))
            break;
          int len = wcslen (wLine);
          if (L'\n' == wLine [len - 1])
            wLine [--len] = 0;

          if (verbose)
            printf ("Output: ");

          if (0 == wLine[0])
            {
              printf ("\n");
              continue;
            }

          int tokenFlag;
          wchar_t* startStr = wLine;
          wchar_t wToken[MAXLEN];

          while ((tokenFlag = SplitToken (&startStr, wToken)) != TTOK_EOT)
            {
              switch (tokenFlag)
                {
                  case TTOK_THAI:
                    WordSegmentation (wseg, &dict, wcwbr, wToken, wsegOut,
                                      N_ELM (wsegOut));
                    ConvPrint (stdout, wsegOut, isUniOut);
                    break;

                  case TTOK_WSPACE:
                  case TTOK_ETC:
                  default:
                    ConvPrint (stdout, wToken, isUniOut);
                    break;
                }
              if (muleMode)
                {
                  ConvPrint (stdout, wcwbr, isUniOut);
                }
            }
          printf ("\n");
        }
      if (wcwbr_buff)
        free (wcwbr_buff);
    }

  ExitWordSegmentation (wseg);

  return 0;
}

static TextToken
SplitToken (wchar_t** str, wchar_t* token)
{
  if (**str == 0)
    return TTOK_EOT;

  if (iswspace (**str))
    {
      while (**str != 0 && iswspace (**str))
        {
          *token++ = *(*str)++;
        }
      *token = 0;
      return TTOK_WSPACE;
    }
  else if (isThaiUni (**str))
    {
      while (**str != 0 && isThaiUni (**str))
        {
          *token++ = *(*str)++;
        }
      *token = 0;
      return TTOK_THAI;
    }
  else
    {
      while (**str != 0 && !iswspace (**str) && !isThaiUni (**str))
        {
          *token++ = *(*str)++;
        }
      *token = 0;
      return TTOK_ETC;
    }
}

