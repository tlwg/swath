// FilterX.h: interface for the FilterX class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __FILTER_X_H
#define __FILTER_X_H

#if _MSC_VER >= 1000
#pragma once
#endif

#include <stdio.h>

class FilterX
{
public:
  FilterX (FILE* filein, FILE* fileout, const char* wordBreakStr);
  virtual ~FilterX ();

  const char* GetWordBreak ();

  virtual bool GetNextToken (char* token, bool* thaiFlag) = 0;
  virtual void Print (char* token, bool thaiFlag) = 0;

protected:
  char  chbuff;
  FILE* fpin;
  FILE* fpout;

private:
  const char* wordBreakStr;
};

inline
FilterX::FilterX (FILE* filein, FILE* fileout, const char* wordBreakStr)
  : chbuff (0),
    fpin (filein ? filein : stdin),
    fpout (fileout ? fileout : stdout),
    wordBreakStr (wordBreakStr)
{
}

inline
FilterX::~FilterX ()
{
  fflush (fpout);
}

inline const char*
FilterX::GetWordBreak ()
{
  return wordBreakStr;
}

#endif
