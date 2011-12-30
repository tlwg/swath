// longwordseg.h: interface for the LongWordSeg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(INC_LONGWORDSEG)
#define INC_LONGWORDSEG

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "abswordseg.h"

class LongWordSeg : public AbsWordSeg
{
public:
  LongWordSeg ();

protected:
  virtual int CreateSentence ();
};

#endif
