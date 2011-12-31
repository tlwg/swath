// filterlambda.h: interface for the filterlambda class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __FILTERLAMBDA_H
#define __FILTERLAMBDA_H

#if _MSC_VER > 1000
#pragma once
#endif

#include "filterlatex.h"

class FilterLambda : public FilterLatex
{
public:
  FilterLambda (FILE* filein, FILE* fileout, int latexflag = 0);
};

inline
FilterLambda::FilterLambda (FILE * filein, FILE * fileout, int latexflag)
  : FilterLatex (filein, fileout, latexflag)
{
  wordBreakStr = "^^^^200c";
}

#endif
