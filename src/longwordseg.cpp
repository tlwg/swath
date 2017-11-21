// longwordseg.cpp: implementation of the LongWordSeg class.
//
//////////////////////////////////////////////////////////////////////

#include "wordstack.h"
#include "longwordseg.h"
#include <limits.h>

int
LongWordSeg::CreateSentence ()
{
  WordStack BackTrackStack;
  int penalty[MAXSEP];     // aggregated penalty at Sep[] positions

  const int senIdx = 0;
  const int bestSenIdx = 1;

  BackTrackStack.Push (WordState (0, 0));
  int bestPenalty = INT_MAX;

  while (!BackTrackStack.Empty())
    {
      WordState wState = BackTrackStack.Top ();
      BackTrackStack.Pop ();

      int Idx = wState.backState;
      int branchIdx = wState.branchState;
      int nextSepIdx = (Idx == 0) ? 0 : copySepData (senIdx, senIdx, Idx);
      penalty[nextSepIdx] = (nextSepIdx == 0) ? 0 : penalty[nextSepIdx - 1];

      while (Idx < textLen)
        {
          if (IdxSep[Idx] >= 0)
            {
              // some words start at Idx
              if (LinkSep[IdxSep[Idx] + branchIdx + 1] != -1)
                {
                  BackTrackStack.Push (WordState (Idx, branchIdx + 1));
                }
              Idx = LinkSep[IdxSep[Idx] + branchIdx];
              branchIdx = 0;
            }
          else
            {
              // skip unknown words and add penalty
              while (Idx < textLen && IdxSep[Idx] < 0)
                Idx++;
              ++penalty[nextSepIdx];
            }
          SepData[senIdx].Sep[nextSepIdx++] = Idx;
          penalty[nextSepIdx] = penalty[nextSepIdx - 1];
        }

      if (SepData[senIdx].Sep[nextSepIdx - 1] != textLen)
        {
          // unknown words at the end
          SepData[senIdx].Sep[nextSepIdx++] = textLen;
          penalty[nextSepIdx] = penalty[nextSepIdx - 1] + 1;
        }
      SepData[senIdx].Sep[nextSepIdx] = -1;

      // clean solution found, return it immediately
      if (penalty[nextSepIdx] == 0)
        return senIdx;

      // otherwise, try to minimize unknown words in best sentence
      if (penalty[nextSepIdx] < bestPenalty)
        {
          copySepData (senIdx, bestSenIdx, SepData[senIdx].Sep[nextSepIdx - 1]);
          bestPenalty = penalty[nextSepIdx];
        }
    }

  return bestSenIdx;
}
