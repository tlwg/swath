// filterlambda.h: interface for the filterlambda class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILTERLAMBDA_H__0D404F43_F0D6_11D4_ACC5_00D0B7929F0B__INCLUDED_)
#define AFX_FILTERLAMBDA_H__0D404F43_F0D6_11D4_ACC5_00D0B7929F0B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "filterlatex.h"

class FilterLambda : public FilterLatex  
{
public:
	FilterLambda(char *filein, char *fileout ,int latexflag=0);
	virtual ~FilterLambda();

};

#endif // !defined(AFX_FILTERLAMBDA_H__0D404F43_F0D6_11D4_ACC5_00D0B7929F0B__INCLUDED_)
