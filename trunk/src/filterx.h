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
  FilterX (FILE * filein, FILE * fileout);
  virtual ~FilterX ();

  const char *GetWordBreak ();

  virtual bool GetNextToken (char *token, bool * thaiFlag) = 0;
  virtual void Print (char *token, bool thaiFlag) = 0;

protected:
  char chbuff;
  FILE *fpin;
  FILE *fpout;
  const char *wordBreakStr;

private:
  bool fileopen;
};

#endif
