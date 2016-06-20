// FilterX.h: interface for the FilterX class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __FILTER_X_H
#define __FILTER_X_H

#if _MSC_VER >= 1000
#pragma once
#endif

#include <stdio.h>
#include <wchar.h>

class FilterX
{
public:
  FilterX (FILE* filein, FILE* fileout, bool isUniIn, bool isUniOut,
           const wchar_t* wordBreakStr);
  virtual ~FilterX ();

  const wchar_t* GetWordBreak ();

  virtual bool GetNextToken (wchar_t* token, int tokenSz, bool* thaiFlag) = 0;
  virtual void Print (const wchar_t* token, bool thaiFlag) = 0;

protected:
  FILE* fpin;
  FILE* fpout;
  bool  isUniIn;
  bool  isUniOut;

private:
  const wchar_t* wordBreakStr;
};

inline
FilterX::FilterX (FILE* filein, FILE* fileout, bool isUniIn, bool isUniOut,
                  const wchar_t* wordBreakStr)
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

inline const wchar_t*
FilterX::GetWordBreak ()
{
  return wordBreakStr;
}

#endif
