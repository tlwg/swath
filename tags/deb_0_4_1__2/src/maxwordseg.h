// maxwordseg.h: interface for the MaxWordSeg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(INC_MAXWORDSEG)
#define INC_MAXWORDSEG

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "abswordseg.h"

class MaxWordSeg : public AbsWordSeg  
{
public:
	MaxWordSeg();
	~MaxWordSeg();

protected:
	short int *score;
	void saveSegment(int idxSen, int lastPoint);
	int noAmbArea;
	int sepIdx;
	int findAmbArea(); //return number of ambiguous area.
	int WordSegArea(int stSeg,int enSeg);
protected:
	virtual int CreateSentence();
};

#endif // !defined(AFX_MAXWORDSEG_H__40352891_2A0D_11D3_B45C_00105A5C2417__INCLUDED_)
