// filterlambda.h: interface for the filterlambda class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __FILTERLAMBDA_H
#define __FILTERLAMBDA_H

#include "filterlatex.h"

class FilterLambda : public FilterLatex
{
public:
  FilterLambda (FILE* filein, FILE* fileout, bool isUniIn);
};

inline
FilterLambda::FilterLambda (FILE* filein, FILE* fileout, bool isUniIn)
  : FilterLatex (filein, fileout, isUniIn, true, L"^^^^200b")
{
}

#endif
