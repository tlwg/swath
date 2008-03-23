// filterlambda.cpp: implementation of the filterlambda class.
//
//////////////////////////////////////////////////////////////////////

#include "filterlambda.h"
#include <string.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FilterLambda::FilterLambda(FILE *filein, FILE *fileout, int latexflag)
    : FilterLatex(filein, fileout, latexflag)
{
	strcpy(wordBreakStr,"^^^^200c");
}

FilterLambda::~FilterLambda()
{

}
