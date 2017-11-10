// abswordseg.cpp: implementation of the AbsWordSeg class.
//
//////////////////////////////////////////////////////////////////////

#include "abswordseg.h"
#include <wctype.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AbsWordSeg::~AbsWordSeg ()
{
}

void
AbsWordSeg::CreateWordList (const Dict* dict)
{
  int cntLink = 0;
  Dict::State* curState = dict->root();

  int i = 0;
  while (i < textLen)
    {
      // Unleadable chars
      if (!IsLeadChar (text[i]) || (i > 0 && !IsLastChar (text[i - 1])))
        {
          IdxSep[i++] = -2;      //cannot leading for unknown word.
          continue;
        }

      wchar_t lead_ch = text[i];

      // Chunk of punctuation marks
      if (iswpunct (lead_ch))
        {
          IdxSep[i] = cntLink;
          while (++i < textLen && iswpunct (text[i]))
            {
              IdxSep[i] = -1;
            }
          LinkSep[cntLink++] = i;
          LinkSep[cntLink++] = -1;
          continue;
        }

      // Chunk of numbers
      if (iswdigit (lead_ch) || isThaiUniDigit (lead_ch))
        {
          IdxSep[i] = cntLink;
          while (++i < textLen)
            {
              wchar_t wc = text[i];
              if (!iswdigit (wc) && !isThaiUniDigit (wc)
                  && wc != L'.' && wc != L',')
                break;

              IdxSep[i] = -1;
            }
          LinkSep[cntLink++] = i;
          LinkSep[cntLink++] = -1;
          continue;
        }

      // Chunk of non-Thai characters
      if (!isThaiUni (lead_ch))
        {
          IdxSep[i] = cntLink;
          while (++i < textLen && !isThaiUni (text[i]))
            {
              IdxSep[i] = -1;
            }
          LinkSep[cntLink++] = i;
          LinkSep[cntLink++] = -1;
          continue;
        }

      // Thai character: find breakabilities starting at text[i]
      IdxSep[i] = -1;
      bool isWordFound = false;
      curState->rewind ();
      for (int j = i; j < textLen; j++)
        {
          if (text[j] == 0x0e46 && isWordFound)
            {
              //Mai-Ya-Mok -- break position
              LinkSep[cntLink - 1] = j + 1;
              break;
            }
          if (!curState->walk (text[j]))
            break;
          if (curState->isTerminal ())
            {
              //found word in dictionary
              //check whether it should be segmented here
              if (IsLeadChar (text[j + 1]) && !HasKaran (&text[j]))
                {
                  if (!isWordFound)
                    {
                      IdxSep[i] = cntLink;
                      isWordFound = true;
                    }
                  LinkSep[cntLink++] = j + 1;
                }
            }
        }
      if (isWordFound)
        LinkSep[cntLink++] = -1;
      ++i;
    }
  delete curState;
  LinkSep[cntLink] = -1;          // add terminator
}

bool
AbsWordSeg::IsLeadChar (wchar_t wc)
{
  return (wc < 0x007f)                      // All ASCII
         || (0x0e01 <= wc && wc <= 0x0e2e)  // KO KAI .. HO NOKHUK
         || (0x0e3f <= wc && wc <= 0x0e44)  // BAHT .. MAIMALAI
         || (0x0e4f <= wc && wc <= 0x0e5b); // FONGMAN .. KHOMUT
}

bool
AbsWordSeg::IsLastChar (wchar_t wc)
{
  return (wc < 0x007f)                      // All ASCII
         || (0x0e01 <= wc && wc <= 0x0e3f)  // KO KAI .. BAHT
         || (0x0e45 <= wc && wc <= 0x0e5b); // LAKKHANGYAO .. KHOMUT
}

bool
AbsWordSeg::HasKaran (const wchar_t* sen_ptr)
{
  for (int i = 1; i <= 3 && sen_ptr[i] != 0; i++)
    {
      if (sen_ptr[i] == 0x0e4c) // THANTHAKHAT
        return true;
    }
  return false;
}

int
AbsWordSeg::WordSeg (const Dict* dict, const wchar_t* senstr,
                     short int* outSeps, int outSepsSz)
{
  int bestidx;

  wcscpy (text, senstr);
  textLen = wcslen (senstr);
  CreateWordList (dict);
  SwapLinkSep ();
  bestidx = CreateSentence ();
  return GetBestSen (bestidx, outSeps, outSepsSz);
}

void
AbsWordSeg::SwapLinkSep ()
{
  int start = 0;
  while (LinkSep[start] != -1)
    {
      int end = start;
      while (LinkSep[++end] != -1)
        ;

      int upper = end - 1;
      while (start < upper)
        {
          short int tmp = LinkSep[start];
          LinkSep[start++] = LinkSep[upper];
          LinkSep[upper--] = tmp;
        }

      start = end + 1;
    }
}

// =======================================================
// function that copy previous seperation point from
// idxSen-1 to idxSen (copy from idx=0 to idx that
// has value= sepPoint).
// Return index of next seperation point that will be fill.
// ========================================================
unsigned short int
AbsWordSeg::copySepData (short int sourceIdxSen, short int targetIdxSen,
                         short int sepPoint)
{
  short int i = 0;
  if (sourceIdxSen == targetIdxSen)
    {
      while (SepData[sourceIdxSen].Sep[i] <= sepPoint)
        {
          i++;
          if (SepData[sourceIdxSen].Sep[i - 1] == sepPoint)
            break;
        }
    }
  else
    {
      while (SepData[sourceIdxSen].Sep[i] <= sepPoint)
        {
          SepData[targetIdxSen].Sep[i] = SepData[sourceIdxSen].Sep[i];
          i++;
          if (SepData[sourceIdxSen].Sep[i - 1] == sepPoint)
            break;
        }
    }
  return i;
}

int
AbsWordSeg::GetBestSen (int bestidx, short int* outSeps, int outSepsSz)
{
  int t;

  for (t = 0; t < outSepsSz && SepData[bestidx].Sep[t] != textLen; t++)
    {
      outSeps[t] = SepData[bestidx].Sep[t];
    }

  return t;
}
