// maxwordseg.cpp: implementation of the MaxWordSeg class.
//
//////////////////////////////////////////////////////////////////////

#include "maxwordseg.h"
#include "wordstack.h"
#include <stdio.h>
#include <string.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MaxWordSeg::MaxWordSeg ()
  : AbsWordSeg ()
{
}

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
          stWord = i;
          enAmb = LinkSep[IdxSep[stWord]];
        }
    }

  int sepIdx = 0;
  int stAmb = 0;
  while (enAmb <= textLen)
    {
      bool singleWord = true;
      for (; stWord < enAmb && stWord < textLen; stWord++)
        {
          if (IdxSep[stWord] < 0)
            continue;
          if (LinkSep[IdxSep[stWord]] > enAmb)
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
  int i;

  for (i = 0; SepData[idxSen].Sep[i] < lastPoint; i++)
    {
      SepData[2].Sep[sepIdx++] = SepData[idxSen].Sep[i];
    }
  if (SepData[idxSen].Sep[i] == lastPoint)
    {
      SepData[2].Sep[sepIdx++] = SepData[idxSen].Sep[i];
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
              BackTrackStack.Push (wordState (Idx, 0));
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
      wordState wState = BackTrackStack.Top ();
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
      bool foundUnk;
      while (curState != enSeg)
        {
          foundUnk = false;
          if (IdxSep[curState] < 0)
            {                   //found unknown string.
              SepData[senIdx].Sep[nextSepIdx++] = curState;
              SepData[senIdx].Score++;
              score[++scoreidx] = SepData[senIdx].Score;
              while (curState < enSeg && IdxSep[curState] < 0)
                curState++;
              foundUnk = true;
              if (curState == enSeg)
                break;
            }
          else if (LinkSep[IdxSep[curState] + 1] != -1)
            {
              //having another branch
              //then it should push backtrack state into Stack.
              BackTrackStack.Push (wordState (curState, 0));
            }
          SepData[senIdx].Sep[nextSepIdx++] = curState;
          if (foundUnk)
            {
              SepData[senIdx].Score += 5;
              foundUnk = false;
            }
          else
            SepData[senIdx].Score++;
          score[++scoreidx] = SepData[senIdx].Score;
          if (SepData[senIdx].Score >= bestScore - 1)
            {
              prevSenIdx = senIdx;
              stopCreate = true;
              break;
            }
          curState = LinkSep[IdxSep[curState]];
        }                       //finish create a new sentence.
      if (stopCreate)
        continue;
      SepData[senIdx].Sep[nextSepIdx] = enSeg;
      SepData[senIdx].Score += (foundUnk) ? 5 : 1;
      SepData[senIdx].Sep[nextSepIdx + 1] = -1;
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
