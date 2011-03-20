// FilterX.h: interface for the FilterX class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILTERX_H__5FA01275_2213_11D3_B449_00105A5C2417__INCLUDED_)
#define AFX_FILTERX_H__5FA01275_2213_11D3_B449_00105A5C2417__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <stdio.h>

class FilterX  
{
public:

	const char* GetWordBreak();
	virtual bool GetNextToken(char *token, bool *thaiFlag)=0;
	virtual void Print(char *token, bool thaiFlag)=0;
	FilterX(FILE *filein, FILE *fileout);
	virtual ~FilterX();

protected:
	char chbuff; //character buff
	FILE *fpin;
	FILE *fpout;
	const char *wordBreakStr;

private:
	bool fileopen;
};

#endif // !defined(AFX_FILTERX_H__5FA01275_2213_11D3_B449_00105A5C2417__INCLUDED_)
