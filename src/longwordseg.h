// longwordseg.h: interface for the LongWordSeg class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __LONGWORDSEG_H
#define __LONGWORDSEG_H

#include "abswordseg.h"

class LongWordSeg : public AbsWordSeg
{
protected:
  virtual int CreateSentence ();
};

#endif
