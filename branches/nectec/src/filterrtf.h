// FilterRTF.h: interface for the FilterRTF class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILTERRTF_H__5FA01278_2213_11D3_B449_00105A5C2417__INCLUDED_)
#define AFX_FILTERRTF_H__5FA01278_2213_11D3_B449_00105A5C2417__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "filterx.h"

class FilterRTF : public FilterX  
{
public:
	void Print(char *token,bool thaiFlag);
	bool GetNextToken(char *token, bool *thaiFlag);
	FilterRTF(char *filein, char *fileout);
	virtual ~FilterRTF();

private:
	int psState;
	char strbuff[5];
	//sequence of characters is    \ ' x x (one character)
	//states of this sequence are  1 2 3 4 	
	int chgCharState(char charin, int state);
	bool isThaiChar(char *token, int *state);

};

#endif // !defined(AFX_FILTERRTF_H__5FA01278_2213_11D3_B449_00105A5C2417__INCLUDED_)
