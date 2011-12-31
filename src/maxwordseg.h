// maxwordseg.h: interface for the MaxWordSeg class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __MAXWORDSEG_H
#define __MAXWORDSEG_H

#if _MSC_VER >= 1000
#pragma once
#endif

#include "abswordseg.h"

class MaxWordSeg : public AbsWordSeg
{
public:
  MaxWordSeg ();
  ~MaxWordSeg ();

protected:
  short int* score;
  void saveSegment (int idxSen, int lastPoint);
  int noAmbArea;
  int sepIdx;
  int findAmbArea ();           //return number of ambiguous area.
  int WordSegArea (int stSeg, int enSeg);

protected:
  virtual int CreateSentence ();
};

#endif
