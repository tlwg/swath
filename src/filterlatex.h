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
               const wchar_t* wordBreakStr = L"{\\wbr}");

  bool GetNextToken (wchar_t* token, int tokenSz, bool* thaiFlag);
  void Print (const wchar_t* token, bool thaiFlag);

private:
  wchar_t buffer[MAXLEN];
  bool verbatim;
};

#endif
