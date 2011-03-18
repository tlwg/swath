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
	LongWordSeg();
	LongWordSeg(const char *dataPath);

protected:
	virtual int CreateSentence();
};

#endif // !defined(AFX_LONGWORDSEG_H__40352894_2A0D_11D3_B45C_00105A5C2417__INCLUDED_)
