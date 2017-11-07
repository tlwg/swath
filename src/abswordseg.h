// AbsWordSeg.h: interface for the AbsWordSeg class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __ABSWORDSEG_H
#define __ABSWORDSEG_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <wchar.h>
#include <datrie/trie.h>
#include "worddef.h"

class AbsWordSeg
{
public:
  AbsWordSeg ();
  virtual ~AbsWordSeg ();

  bool InitDict (const char* dictPath);
  int WordSeg (const wchar_t* senstr, short int* outSeps, int outSepsSz);

protected:
  //============Function for Wordseg=====================
  void SwapLinkSep ();
  int GetBestSen (int bestidx, short int* outSeps, int outSepsSz);
  void CreateWordList (void);
  virtual int CreateSentence () = 0;    //return idx of best sen
  unsigned short int copySepData (short int sourceIdxSen,
                                  short int targetIdxSen,
                                  short int sepPoint);

protected:
  SepType SepData[3];
  unsigned short int textLen;
  wchar_t text[MAXLEN];
  short int LinkSep[3 * MAXLEN];
  short int IdxSep[MAXLEN];

private:
  void InitData ();

  //============Check Character Type Function============
  static bool IsLeadChar (wchar_t wc);
  static bool IsLastChar (wchar_t wc);
  //============Check Karan rule=========================
  static bool HasKaran (const wchar_t* sen_ptr);

private:
    Trie*  MyDict;
};

#endif
