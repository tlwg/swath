// maxwordseg.cpp: implementation of the MaxWordSeg class.
//
//////////////////////////////////////////////////////////////////////

#include "maxwordseg.h"
#include "wordstack.h"

// Return number of ambiguous areas.
int
MaxWordSeg::CreateSentence ()
{
  int stWord, enAmb;
  if (IdxSep[0] >= 0)
    {
      stWord = 1;
      enAmb = LinkSep[IdxSep[0]];
    }
  else
    {
      int i;
      for (i = 1; i < textLen && IdxSep[i] < 0; i++)
        ;

      if (i == textLen)
        {
          stWord = enAmb = textLen;
        }
      else
        {
          stWord = i + 1;
          enAmb = LinkSep[IdxSep[i]];
        }
    }

  int sepIdx = 0;
  int stAmb = 0;
  while (enAmb <= textLen)
    {
      bool singleWord = true;
      for (; stWord < enAmb; stWord++)
        {
          if (IdxSep[stWord] >= 0 && LinkSep[IdxSep[stWord]] > enAmb)
            {
              singleWord = false;
              enAmb = LinkSep[IdxSep[stWord]];
            }
        }
      if (IdxSep[enAmb] < 0)
        {
          // Check for unknown
          while (enAmb < textLen && IdxSep[enAmb] < 0)
            enAmb++;
          singleWord = false;
        }
      if (singleWord)
        {
          SepData[2].Sep[sepIdx++] = enAmb;
        }
      else
        {
          int idxSen = WordSegArea (stAmb, enAmb);
          sepIdx = saveSegment (sepIdx, idxSen, enAmb);
        }
      stAmb = enAmb;
      if (stAmb >= textLen)
        break;
      enAmb = LinkSep[IdxSep[stAmb]];
    }

  return 2;
}

int
MaxWordSeg::saveSegment (int sepIdx, int idxSen, int lastPoint)
{
  const short int *srcSep = SepData[idxSen].Sep;
  short int       *dstSep = SepData[2].Sep;

  while (*srcSep != -1 && *srcSep <= lastPoint)
    {
      dstSep[sepIdx++] = *srcSep++;
    }

  return sepIdx;
}

int
MaxWordSeg::WordSegArea (int stSeg, int enSeg)
{
  WordStack BackTrackStack;

  short int senIdx = 0, bestSenIdx = 0, prevSenIdx = 0;
  short int sepIdx = 0;
  short int scoreidx;
  int bestScore = 0;
  int score[MAXSEP];

  // ========================================
  // this loop gets the first sentence
  //      and Create Backtrack point.....
  // ========================================
  int Idx = stSeg;

  scoreidx = -1;
  SepData[0].Score = 10000;
  SepData[1].Score = 10000;
  while (Idx < enSeg) //(LinkSep[IdxSep[Idx]]!=enSeg)
    {
      // found some words that start with Idx character
      if (IdxSep[Idx] >= 0)
        {
          if (LinkSep[IdxSep[Idx] + 1] != -1)
            {
              BackTrackStack.Push (WordState (Idx, 0));
            }
          SepData[senIdx].Sep[sepIdx++] = LinkSep[IdxSep[Idx]];
          score[++scoreidx] = ++SepData[senIdx].Score;
          Idx = LinkSep[IdxSep[Idx]];
        }
      else
        {
          //at Idx there is no word in dictionary
          while (Idx < enSeg && IdxSep[Idx] < 0)
            Idx++;
          SepData[senIdx].Sep[sepIdx++] = Idx;
          SepData[senIdx].Score += 5;
          score[++scoreidx] = SepData[senIdx].Score;
        }
    }
  SepData[senIdx].Sep[sepIdx] = -1;
  bestScore = SepData[senIdx].Score;
  bestSenIdx = senIdx++;
  //================================================
  //create all posible sentences
  //using backtrack (use my stack not use recursive)
  //================================================
  int looptime = 0;
  while (!BackTrackStack.Empty () && looptime++ <= 200)
    {
      bool stopCreate = false;
      WordState wState = BackTrackStack.Top ();
      BackTrackStack.Pop ();

      wState.branchState++;
      int curState = LinkSep[IdxSep[wState.backState] + wState.branchState];
      if (curState == -1)
        continue;

      BackTrackStack.Push (wState);
      //create new sentence from prev sentence and save score to new sen.
      //change 1st parameter of copySepData from bestSenIdx to prevSenIdx
      short int nextSepIdx = copySepData (prevSenIdx, senIdx, wState.backState);
      scoreidx = nextSepIdx - 1;
      SepData[senIdx].Score = (scoreidx < 0) ? 10000 : score[scoreidx];

      //loop for filling the rest sep point of new sentence
      while (curState != enSeg)
        {
          SepData[senIdx].Sep[nextSepIdx++] = curState;
          SepData[senIdx].Score++;
          if (IdxSep[curState] < 0)
            {
              //unknown string, find its boundary and add penalty
              while (curState < enSeg && IdxSep[curState] < 0)
                curState++;
              SepData[senIdx].Score += 5;
            }
          else
            {
              if (LinkSep[IdxSep[curState] + 1] != -1)
                {
                  //another branch exists, push backtrack state
                  BackTrackStack.Push (WordState (curState, 0));
                }
              curState = LinkSep[IdxSep[curState]];
            }
          score[++scoreidx] = SepData[senIdx].Score;
          if (SepData[senIdx].Score >= bestScore - 1)
            {
              prevSenIdx = senIdx;
              stopCreate = true;
              break;
            }
        }
      if (stopCreate)
        continue;
      SepData[senIdx].Sep[nextSepIdx++] = enSeg;
      SepData[senIdx].Score++;
      SepData[senIdx].Sep[nextSepIdx] = -1;
      prevSenIdx = senIdx;
      if (SepData[senIdx].Score < bestScore)
        {
          bestScore = SepData[senIdx].Score;
          short int tmpidx = bestSenIdx;
          bestSenIdx = senIdx;
          senIdx = tmpidx;
        }
    }

  return bestSenIdx;
}
