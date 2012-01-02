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

protected:
  virtual int CreateSentence ();

private:
  int saveSegment (int sepIdx, int idxSen, int lastPoint);
  int findAmbArea ();           //return number of ambiguous area.
  int WordSegArea (int stSeg, int enSeg);
};

#endif
