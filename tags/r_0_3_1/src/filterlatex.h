// FilterLatex.h: interface for the FilterLatex class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(INC_FILTERLATEX)
#define INC_FILTERLATEX

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "filterx.h"


class FilterLatex : public FilterX  
{
public:
	void Print(char *token,bool thaiFlag);
	bool GetNextToken(char *token, bool *thaiFlag);
	FilterLatex(char *filein, char *fileout, int latexflag=0);
	virtual ~FilterLatex();

protected:
	bool winCharSet; //==true for Windows char set : false for Macintosh char set
	int latexFlag; // 0 do nothing 1 win char set 2 mac char set
	int idxVowelToneMark(unsigned char ch);
	bool isLongTailChar(unsigned char ch);
	void AdjustText (unsigned char *input, unsigned char *output);
private:
	char buffer[2000];
	bool verbatim;
};

#endif // !defined(AFX_FILTERLATEX_H__6AB8F3C5_2230_11D3_B44A_00105A5C2417__INCLUDED_)
