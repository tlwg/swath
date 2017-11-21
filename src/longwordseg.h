// longwordseg.h: interface for the LongWordSeg class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __LONGWORDSEG_H
#define __LONGWORDSEG_H

#if _MSC_VER >= 1000
#pragma once
#endif

#include "abswordseg.h"

class LongWordSeg : public AbsWordSeg
{
protected:
  virtual int CreateSentence ();
};

#endif
