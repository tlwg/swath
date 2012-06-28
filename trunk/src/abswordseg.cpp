// abswordseg.cpp: implementation of the AbsWordSeg class.
//
//////////////////////////////////////////////////////////////////////

#include "abswordseg.h"
#include "dictpath.h"
#include "worddef.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AbsWordSeg::AbsWordSeg ()
  : MyDict (NULL)
{
}

AbsWordSeg::~AbsWordSeg ()
{
  if (MyDict)
    {
      trie_free (MyDict);
    }
}

bool
AbsWordSeg::InitDict (const char* dictPath)
{
  char* trieBuff = NULL;
  const char* triePath;
  struct stat sb;

  if (stat (dictPath, &sb) == -1)
    {
      perror (dictPath);
      return false;
    }

  if (S_ISDIR (sb.st_mode))
    {
      trieBuff = new char[strlen (dictPath) + 2 + sizeof (D2TRIE)];
      sprintf (trieBuff, "%s" PATHSEPERATOR D2TRIE, dictPath);
      triePath = trieBuff;
    }
  else if (S_ISREG (sb.st_mode))
    {
      triePath = dictPath;
    }
  else
    {
      fprintf (stderr, "%s is not a directory or regular file\n", dictPath);
      return false;
    }

  MyDict = trie_new_from_file (triePath);

  if (trieBuff)
    {
      delete[] trieBuff;
    }

  return MyDict != NULL;
}


void
AbsWordSeg::CreateWordList (void)
{
  int cntLink = 0;
  int amb_sep_cnt = 0;
  TrieState* curState = trie_root (MyDict);

  for (int i = 0; i < len; i++)
    {                                //word boundry start at i and end at j.
      if (!IsLeadChar ((unsigned char) sen[i])
          || (i > 0 && !IsLastChar ((unsigned char) sen[i - 1])))
        {
          IdxSep[i] = -2;        //cannot leading for unknown word.
          continue;
        }
      unsigned char lead_ch = (unsigned char) sen[i];
      // FIND STRING OF PUNCTUATION.
      if (ispunct (lead_ch))
        {
          IdxSep[i] = cntLink;
          do
            {
              lead_ch = (unsigned char) sen[++i];
            }
          while (lead_ch != '\0' && ispunct (lead_ch));
          LinkSep[cntLink++] = i;
          LinkSep[cntLink++] = -1;
          i--;
          continue;
        }
      // FIND STRING OF NUMBER.
      if (isdigit (lead_ch) || isThaiDigit (lead_ch))
        {
          IdxSep[i] = cntLink;
          do
            {
              lead_ch = (unsigned char) sen[++i];
            }
          while (lead_ch != '\0' && isdigit (lead_ch) || isThaiDigit (lead_ch)
                 || lead_ch == '.' || lead_ch == ',');
          LinkSep[cntLink++] = i;
          LinkSep[cntLink++] = -1;
          i--;
          continue;
        }
      // FIND STRING OF ENGLISH.
      if (isalpha (lead_ch))
        {
          IdxSep[i] = cntLink;
          do
            {
              lead_ch = (unsigned char) sen[++i];
            }
          while (lead_ch != '\0' && isalpha (lead_ch));
          LinkSep[cntLink++] = i;
          LinkSep[cntLink++] = -1;
          i--;
          continue;
        }
      int cntFound = 0;
      trie_state_rewind (curState);
      for (int j = 0; i + j < len; j++)
        {
          if (sen[i + j] == 0xe6 && cntFound != 0)
            {                        //Mai-Ya-Mok -- break position
              LinkSep[cntLink - 1] = i + j + 1;
              break;
            }
          if (!trie_state_walk (curState, tis2uni ((unsigned char) sen[i + j])))
            break;
          if (trie_state_is_terminal (curState))
            {
              short int en_word;
              //found word in dictionary
              //check whether it should be segmented here
              if (IsLeadChar ((unsigned char) sen[i + j + 1])
                  && !Has_Karun (&sen[i + j], &en_word))
                {
                  LinkSep[cntLink] = i + j + 1;
                  LinkSep[cntLink + 1] = -1;
                  cntFound++;
                  if (cntFound == 1)
                    IdxSep[i] = cntLink;
                  cntLink++;
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
  trie_state_free (curState);
//   amb_sep[amb_sep_cnt].st_idx=-1;
  LinkSep[cntLink] = -1;
  LinkSep[++cntLink] = -1;        //add stop value
}

bool
AbsWordSeg::IsLeadChar (unsigned char ch)
{
  return (ch < 0xcf || 0xda < ch) && (ch < 0xe5 || 0xee < ch);
}

bool
AbsWordSeg::IsLastChar (unsigned char ch)
{
  return (ch < 0xe0 || 0xe4 < ch) && ch != 0xd1;
}

bool
AbsWordSeg::Has_Karun (const char* sen_ptr, short int* k_idx)
{
  short int i;

  *k_idx = -1;
  for (i = 1; i <= 3 && sen_ptr[i] != '\0'; i++)
    {
      if ((unsigned char) sen_ptr[i] == 0xec)
        {
          *k_idx = i;
          return true;
        }
    }
  return false;
}

int
AbsWordSeg::WordSeg (const char* senstr, short int* outSeps, int outSepsSz)
{
  int bestidx;

  strcpy (sen, senstr);
  len = strlen (senstr);
  InitData ();
  CreateWordList ();
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
  for (int i = 0; i < len; i++)
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

  for (t = 0; SepData[bestidx].Sep[t] != len && t < outSepsSz; t++)
    {
      outSeps[t] = SepData[bestidx].Sep[t];
    }

  return t;
}
