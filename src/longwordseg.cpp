// longwordseg.cpp: implementation of the LongWordSeg class.
//
//////////////////////////////////////////////////////////////////////

#include "wordstack.h"
#include "longwordseg.h"
#include "dictpath.h"
#include <string.h>
#include <stdio.h>
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
  short int nextSepIdx = 0, curState;

  wordState wState;
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
              wState.backState = Idx;
              wState.branchState = 0;
              BackTrackStack.Push (wState);
            }
          SepData[senIdx].Sep[sepIdx++] = LinkSep[IdxSep[Idx]];
          if (LinkSep[IdxSep[Idx]] == textLen)
            break;
          Idx = LinkSep[IdxSep[Idx]];
        }
      else
        {
          //at Idx there is no word in dictionary
          while (Idx < textLen && IdxSep[Idx] < 0)
            Idx++;
          SepData[senIdx].Sep[sepIdx++] = Idx;
          if (Idx == textLen)
            break;
        }
    }
  if (SepData[senIdx].Sep[sepIdx - 1] == textLen)
    {
      SepData[senIdx].Sep[sepIdx] = -1;
    }
  else
    {
      SepData[senIdx].Sep[sepIdx++] = textLen;
      SepData[senIdx].Sep[sepIdx] = -1;
    }

  return 0;
}
