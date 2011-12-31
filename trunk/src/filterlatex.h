// FilterLatex.h: interface for the FilterLatex class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __FILTERLATEX_H
#define __FILTERLATEX_H

#if _MSC_VER >= 1000
#pragma once
#endif

#include "filterx.h"


class FilterLatex : public FilterX
{
public:
  FilterLatex (FILE* filein, FILE* fileout, int latexflag = 0,
               const char *wordBreakStr = "{\\wbr}");
  virtual ~FilterLatex ();

  void Print (char* token, bool thaiFlag);
  bool GetNextToken (char* token, bool* thaiFlag);

protected:
  bool winCharSet;  // true for Windows char set; false for Mac char set
  int latexFlag;    // 0 do nothing; 1 win char set; 2 mac char set

  void AdjustText (unsigned char* input, unsigned char* output);

private:
  char buffer[2000];
  bool verbatim;
};

#endif
