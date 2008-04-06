// FilterHtml.h: interface for the FilterHtml class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILTERHTML_H__5FA01276_2213_11D3_B449_00105A5C2417__INCLUDED_)
#define AFX_FILTERHTML_H__5FA01276_2213_11D3_B449_00105A5C2417__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "filterx.h"

class FilterHtml : public FilterX  
{
public:
	void Print(char *token,bool thaiFlag);
	bool GetNextToken(char *token, bool *thaiFlag);
	FilterHtml(FILE *filein, FILE *fileout);
	virtual ~FilterHtml();

};

#endif // !defined(AFX_FILTERHTML_H__5FA01276_2213_11D3_B449_00105A5C2417__INCLUDED_)
