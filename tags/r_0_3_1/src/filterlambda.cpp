// filterlambda.cpp: implementation of the filterlambda class.
//
//////////////////////////////////////////////////////////////////////

#include "filterlambda.h"
#include <string.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FilterLambda::FilterLambda(char *filein, char *fileout ,int latexflag):FilterLatex( filein, fileout, latexflag)
{
		strcpy(wordBreakStr,"^^^^200c");
}

FilterLambda::~FilterLambda()
{

}
