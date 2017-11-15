// longwordseg.cpp: implementation of the LongWordSeg class.
//
//////////////////////////////////////////////////////////////////////

#include "wordstack.h"
#include "longwordseg.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LongWordSeg::LongWordSeg ()
  : AbsWordSeg ()
{
}

int
LongWordSeg::CreateSentence ()
{
  WordStack BackTrackStack;

  short int senIdx = 0;
  short int sepIdx = 0, Idx = 0;

  // ========================================
  // this loop gets the first sentence
  //      and Create Backtrack point.....
  // ========================================

  while (Idx < textLen) // (LinkSep[IdxSep[Idx]]!=len)
    {
      // found some words that start with Idx character
      if (IdxSep[Idx] >= 0)
        {
          if (LinkSep[IdxSep[Idx] + 1] != -1)
            {
              BackTrackStack.Push (WordState (Idx, 0));
            }
          Idx = LinkSep[IdxSep[Idx]];
        }
      else
        {
          //at Idx there is no word in dictionary
          while (Idx < textLen && IdxSep[Idx] < 0)
            Idx++;
        }
      SepData[senIdx].Sep[sepIdx++] = Idx;
    }
  if (SepData[senIdx].Sep[sepIdx - 1] != textLen)
    {
      SepData[senIdx].Sep[sepIdx++] = textLen;
    }
  SepData[senIdx].Sep[sepIdx] = -1;

  return 0;
}
