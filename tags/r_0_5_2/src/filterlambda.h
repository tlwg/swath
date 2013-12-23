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
  FilterLambda (FILE* filein, FILE* fileout, bool isUniIn, int latexflag = 0);
};

inline
FilterLambda::FilterLambda (FILE* filein, FILE* fileout, bool isUniIn,
                            int latexflag)
  : FilterLatex (filein, fileout, isUniIn, true, latexflag, "^^^^200b")
{
}

#endif
