// AbsWordSeg.h: interface for the AbsWordSeg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(INC_ABSWORDSEG)
#define INC_ABSWORDSEG

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <datrie/trie.h>
#include "worddef.h"

class AbsWordSeg  
{
public:
	AbsWordSeg();
	AbsWordSeg(const char *dataPath);
	virtual ~AbsWordSeg();
	bool WordSeg(char* senstr,char *output,const char *wbr);
	
protected:
	SepType SepData[3];
	unsigned short int len;
	short int cntSep;
	char sen[MAXLEN];
	short int *LinkSep;
	short int *IdxSep;
	//============Function for Wordseg=====================
	virtual void SwapLinkSep();
	virtual void GetBestSen(int bestidx,const char *wbr,char *outstr);
	virtual void CreateWordList(void);
	virtual int CreateSentence()=0; //return idx of best sen
	virtual unsigned short int copySepData(short int sourceIdxSen,short int targetIdxSen,short int sepPoint);
	void GetWord(short int idxsen,short int idx,char *buff);

private:
	void Construct();
	void InitData();

	//============Check Character Type Function============
	static bool IsLeadChar(unsigned char ch);
	static bool IsLastChar(unsigned char ch);
	static bool IsNumber(char *str);
	static bool IsEnglish(char *str);
	//============Check Karan rule=========================
	static bool Has_Karun(char* sen_ptr,short int *k_idx);

private:
	Trie *MyDict;
};

#endif // !defined(AFX_ABSWORDSEG_H__A0F72574_2912_11D3_B459_00105A5C2417__INCLUDED_)
