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
#include <ctype.h>
#include <time.h>
#include <wchar.h>
#include <wctype.h>

#include "filefilter.h"
#include "filterx.h"
#include "dictpath.h"
#include "longwordseg.h"
#include "maxwordseg.h"
#include "convutil.h"
#include "utils.h"

static int SplitToken (wchar_t** str, wchar_t* token);

// Return
// 0: successful
// 1: can not get dictionary
static int
InitWordSegmentation (const char* dictpath,
                      const char* method, AbsWordSeg** pWseg)
{
  if (method == NULL)
    {
      *pWseg = new MaxWordSeg;
    }
  else if (strcmp (method, "long") == 0)
    {
      *pWseg = new LongWordSeg;
    }
  else
    {
      *pWseg = new MaxWordSeg;
    }

  // Dict search order:
  // 1. dictpath, if not NULL, exit on failure
  // 2. ${SWATHDICT} env, if not NULL, continue on failure
  // 3. current dir, continue on failure
  // 4. WORDSEGDATADIR macro
  if (dictpath)
    {
      if (!(*pWseg)->InitDict (dictpath))
        {
          delete *pWseg;
          return 1;
        }
      return 0;
    }

  dictpath = getenv ("SWATHDICT");
  if (dictpath && (*pWseg)->InitDict (dictpath))
    return 0;

  if ((*pWseg)->InitDict ("."))
    return 0;

  if ((*pWseg)->InitDict (WORDSEGDATA_DIR))
    return 0;

  // All fail
  delete *pWseg;
  return 1;
}

static void
ExitWordSegmentation (AbsWordSeg* wseg)
{
  delete wseg;
}

static void
WordSegmentation (AbsWordSeg* wseg, const char* wbr, const wchar_t* line,
                  wchar_t* output, int outputSz)
{
  short int *seps = new short int [outputSz];
  int nSeps = wseg->WordSeg (line, seps, outputSz);

  const wchar_t* pLine = line;
  int wbrLen = strlen (wbr);
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
      Ascii2WcsCopy (output, wbr);
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
     "Copyright (C) 2001-2009 Theppitak Karoonboonyanan <thep@linux.thai.net>\n"
     "License: GNU GPL version 2 or later <http://gnu.org/licenses/gpl-2.0.html>\n"
     "This is free software; you are free to change and redistribute it.\n"
     "There is NO WARRANTY, to the extent permitted by law.\n");
}

static void
Usage (int verbose)
{
  fprintf (stderr,
           "Usage: swath [mule|-v] [-b \"delimitor\"] [-d dict-dir]\n"
           "[-f html|rtf|latex|lambda] [-m long|max] [-u {u|t},{u|t}] [-help]\n");
  if (verbose)
    {
      fprintf (stderr,
               "Options:\n"
               "\tmule : for use with mule\n"
               "\t-v   : verbose mode\n"
               "\t-b   : define a word delimitor string for the output\n"
               "\t-d   : specify dictionary path\n"
               "\t-f   : specify format of the input\n"
               "\t\thtml     : HTML file\n"
               "\t\trtf      : RTF file\n"
               "\t\tlatex    : LaTeX file\n"
               "\t\tlambda   : The input and output are same as latex, except that\n"
               "\t\t           the word delimitor is ^^^^200b\n"
//                "\t\twinlatex : LaTeX file shaping on Windows\n"
//                "\t\tmaclatex : LaTeX file shaping on Macintosh\n"
               "\t-m   : choose word matching scheme when analyzing\n"
               "\t\tlong     : longest matching scheme\n"
               "\t\tmax      : maximal matching scheme\n"
//                "\t-l   : line processing(effect only in a bigram algo.)\n"
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
//#define WORDSEGDATA_DIR "./data"

int
main (int argc, char* argv[])
{
  char mode = 1;                // 0 = display, 1 = don't display message
  const char* wbr;
  const char* dictpath = NULL;
  const char* method = NULL;
  const char* fileformat = NULL;
  const char* unicode = NULL;
  bool muleMode;
  bool thaiFlag;
  bool wholeLine = false;

  wbr = "|";
  muleMode = false;
  for (int iargc = 1; iargc < argc; iargc++)
    {
      if (strcmp ("mule", argv[iargc]) == 0)
        {
          muleMode = true;
          mode = 1;
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
          mode = 0;
        }
      else if (strcmp ("-m", argv[iargc]) == 0 && iargc + 1 < argc)
        {
          method = argv[++iargc];
        }
      else if (strcmp ("-l", argv[iargc]) == 0)
        {
          //send only token which has no white space in to wordseg
          wholeLine = true;
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

  if (mode == 0)
    printf ("*** Word Segmentation ***\n");

  AbsWordSeg* wseg;

  if (InitWordSegmentation (dictpath, method, &wseg) != 0)
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
          // FIXME: still mem leak hmm..
          return 1;
        }
      wbr = FltX->GetWordBreak ();
      while (FltX->GetNextToken (wLine, N_ELM (wLine), &thaiFlag))
        {
          if (!thaiFlag)
            {
              FltX->Print (wLine, thaiFlag);
              continue;
            }
          WordSegmentation (wseg, wbr, wLine, wsegOut, N_ELM (wsegOut));
          FltX->Print (wsegOut, thaiFlag);
        }
      delete FltX;
    }
  else
    {
      const char *stopstr = muleMode ? wbr : "";
      while (!feof (stdin))
        {
          if (mode == 0)
            printf ("Input : ");

          if (!ConvGetS (wLine, N_ELM (wLine), stdin, isUniIn))
            break;
          int len = wcslen (wLine);
          if (L'\n' == wLine [len - 1])
            wLine [--len] = 0;

          if (mode == 0)
            printf ("Output: ");

          if (0 == wLine[0])
            {
              printf ("\n");
              continue;
            }

          if (!wholeLine)
            {
              int tokenFlag;
              wchar_t* startStr = wLine;
              wchar_t wToken[MAXLEN];

              while ((tokenFlag = SplitToken (&startStr, wToken)) >= 0)
                {
                  if (tokenFlag == 0)
                    {
                      ConvPrint (stdout, wToken, isUniOut);
                    }
                  else
                    {
                      WordSegmentation (wseg, wbr, wToken, wsegOut,
                                        N_ELM (wsegOut));
                      ConvPrint (stdout, wsegOut, isUniOut);
                    }
                  printf ("%s", stopstr);
                }
            }
          else
            {
              WordSegmentation (wseg, wbr, wLine, wsegOut, N_ELM (wsegOut));
              ConvPrint (stdout, wsegOut, isUniOut);
              printf ("%s", stopstr);
            }
          printf ("\n");
        }
    }

  ExitWordSegmentation (wseg);

  return 0;
}

//return -1 in case there is no data in str
//return 0 for a token which is contain only white spaces.
//return 1 for a token which is contain only alpha charecters.
static int
SplitToken (wchar_t** str, wchar_t* token)
{
  if (**str == 0)
    return -1;

  if (iswspace (**str))
    {
      while (**str != 0 && iswspace (**str))
        {
          *token++ = *(*str)++;
        }
      *token = 0;
      return 0;
    }
  else
    {
      while (**str != 0 && !iswspace (**str))
        {
          *token++ = *(*str)++;
        }
      *token = 0;
      return 1;
    }
}

