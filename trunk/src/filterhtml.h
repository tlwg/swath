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

  bool GetNextToken (char* token, bool* thaiFlag);
  void Print (char* token, bool thaiFlag);

private:
  char chbuff;
};

#endif
