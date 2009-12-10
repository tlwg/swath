// FileFilter.cpp: implementation of the FileFilter class.
//
//////////////////////////////////////////////////////////////////////

#include <string.h>
#include "filefilter.h"

#include "filterhtml.h"
#include "filterrtf.h"
#include "filterlatex.h"
#include "filterlambda.h"

FilterX* FileFilter::CreateFilter(FILE *filein, FILE *fileout,
                                  const char *fileformat)
{
	if (strcmp(fileformat, "html")==0)
		return new FilterHtml(filein,fileout);
	else if (strcmp(fileformat, "latex")==0)
		return new FilterLatex(filein,fileout);
	else if (strcmp(fileformat, "lambda")==0)
		return new FilterLambda(filein,fileout);
	else if (strcmp(fileformat, "rtf")==0)
		return new FilterRTF(filein,fileout);
	else
		return NULL;
}

