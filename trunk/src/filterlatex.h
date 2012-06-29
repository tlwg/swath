// FilterLatex.h: interface for the FilterLatex class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __FILTERLATEX_H
#define __FILTERLATEX_H

#if _MSC_VER >= 1000
#pragma once
#endif

#include "worddef.h"
#include "filterx.h"


class FilterLatex : public FilterX
{
public:
  FilterLatex (FILE* filein, FILE* fileout, bool isUniIn, bool isUniOut,
               int latexflag = 0, const char *wordBreakStr = "{\\wbr}");

  bool GetNextToken (wchar_t* token, bool* thaiFlag);
  void Print (const wchar_t* token, bool thaiFlag);

private:
  int AdjustText (const unsigned char* input, unsigned char* output,
                  int output_sz);

private:
  bool winCharSet;  // true for Windows char set; false for Mac char set
  int latexFlag;    // 0 do nothing; 1 win char set; 2 mac char set
  wchar_t buffer[MAXLEN];
  bool verbatim;
};

#endif
