// AbsWordSeg.h: interface for the AbsWordSeg class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __ABSWORDSEG_H
#define __ABSWORDSEG_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <datrie/trie.h>
#include "worddef.h"

class AbsWordSeg
{
public:
  AbsWordSeg ();
  virtual ~AbsWordSeg ();

  bool InitDict (const char* dictPath);
  int WordSeg (const char* senstr, short int* outSeps, int outSepsSz);

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
  char text[MAXLEN];
  short int LinkSep[3 * MAXLEN];
  short int IdxSep[MAXLEN];

private:
  void InitData ();

  //============Check Character Type Function============
  static bool IsLeadChar (unsigned char ch);
  static bool IsLastChar (unsigned char ch);
  //============Check Karan rule=========================
  static bool Has_Karun (const char* sen_ptr, short int* k_idx);

private:
    Trie*  MyDict;
};

#endif
