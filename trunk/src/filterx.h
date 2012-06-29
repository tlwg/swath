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
  FilterX (FILE* filein, FILE* fileout, bool isUniIn, bool isUniOut,
           const char* wordBreakStr);
  virtual ~FilterX ();

  const char* GetWordBreak ();

  virtual bool GetNextToken (char* token, bool* thaiFlag) = 0;
  virtual void Print (char* token, bool thaiFlag) = 0;

protected:
  FILE* fpin;
  FILE* fpout;
  bool  isUniIn;
  bool  isUniOut;

private:
  const char* wordBreakStr;
};

inline
FilterX::FilterX (FILE* filein, FILE* fileout, bool isUniIn, bool isUniOut,
                  const char* wordBreakStr)
  : fpin (filein ? filein : stdin),
    fpout (fileout ? fileout : stdout),
    isUniIn (isUniIn),
    isUniOut (isUniOut),
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
