// FilterHtml.h: interface for the FilterHtml class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __FILTERHTML_H
#define __FILTERHTML_H

#if _MSC_VER >= 1000
#pragma once
#endif

#include "filterx.h"

class FilterHtml : public FilterX
{
public:
  FilterHtml (FILE* filein, FILE* fileout, bool isUniIn, bool isUniOut);

  bool GetNextToken (wchar_t* token, bool* thaiFlag);
  void Print (const wchar_t* token, bool thaiFlag);

private:
  wchar_t chbuff;
};

#endif
