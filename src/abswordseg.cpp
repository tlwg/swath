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

  for (int i = 0; i < textLen; i++)
    {
      if (!IsLeadChar (text[i]) || (i > 0 && !IsLastChar (text[i - 1])))
        {
          IdxSep[i] = -2;        //cannot leading for unknown word.
          continue;
        }
      wchar_t lead_ch = text[i];
      // FIND STRING OF PUNCTUATION.
      if (iswpunct (lead_ch))
        {
          IdxSep[i] = cntLink;
          do
            {
              lead_ch = text[++i];
            }
          while (lead_ch != 0 && iswpunct (lead_ch));
          LinkSep[cntLink++] = i;
          LinkSep[cntLink++] = -1;
          i--;
          continue;
        }
      // FIND STRING OF NUMBER.
      if (iswdigit (lead_ch) || isThaiUniDigit (lead_ch))
        {
          IdxSep[i] = cntLink;
          do
            {
              lead_ch = text[++i];
            }
          while (lead_ch != 0 && iswdigit (lead_ch) || isThaiUniDigit (lead_ch)
                 || lead_ch == L'.' || lead_ch == L',');
          LinkSep[cntLink++] = i;
          LinkSep[cntLink++] = -1;
          i--;
          continue;
        }
      // FIND STRING OF NON-THAI
      if (!isThaiUni (lead_ch))
        {
          IdxSep[i] = cntLink;
          do
            {
              lead_ch = text[++i];
            }
          while (lead_ch != 0 && !isThaiUni (lead_ch));
          LinkSep[cntLink++] = i;
          LinkSep[cntLink++] = -1;
          i--;
          continue;
        }
      int cntFound = 0;
      curState->rewind ();
      for (int j = i; j < textLen; j++)
        {
          if (text[j] == 0x0e46 && cntFound != 0)
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
                  if (cntFound == 0)
                    IdxSep[i] = cntLink;
                  LinkSep[cntLink++] = j + 1;
                  LinkSep[cntLink] = -1;
                  cntFound++;
                }
            }
        }
      if (cntFound == 0)
        IdxSep[i] = -1;
      else if (cntFound < 2000)
        {
          //LinkSepDataIdx[cntLink]=-1;
          LinkSep[cntLink++] = -1;
        }
    }
  delete curState;
  LinkSep[cntLink] = -1;
  LinkSep[++cntLink] = -1;        //add stop value
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
  InitData ();
  CreateWordList (dict);
  SwapLinkSep ();
  bestidx = CreateSentence ();
  return GetBestSen (bestidx, outSeps, outSepsSz);
}

void
AbsWordSeg::SwapLinkSep ()
{
  short int st_idx, en_idx, end_point, tmp;

  st_idx = 0;
  en_idx = 0;
  while (LinkSep[st_idx] != -1)
    {
      while (LinkSep[en_idx++] != -1)
        ;
      end_point = en_idx - 1;
      en_idx -= 2;
      while (st_idx < en_idx)
        {
          tmp = LinkSep[st_idx];
          //tmpI=LinkSepDataIdx[st_idx];
          LinkSep[st_idx] = LinkSep[en_idx];
          //LinkSepDataIdx[st_idx]=LinkSepDataIdx[en_idx];
          st_idx++;
          LinkSep[en_idx] = tmp;
          //LinkSepDataIdx[en_idx]=tmpI;
          en_idx--;
        }
      st_idx = end_point + 1;
      en_idx = st_idx;
    }
}

void
AbsWordSeg::InitData ()
{
  for (int i = 0; i < textLen; i++)
    {
      IdxSep[i] = -1;
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
