// FileFilter.cpp: implementation of the FileFilter class.
//
//////////////////////////////////////////////////////////////////////

#include <string.h>
#include "filefilter.h"

#include "filterhtml.h"
#include "filterrtf.h"
#include "filterlatex.h"
#include "filterlambda.h"

FilterX* FileFilter::CreateFilter(char *filein, char *fileout, char *fileformat)
{
	if (strcmp((const char *)fileformat,"html")==0)
		return new FilterHtml(filein,fileout);
	else if (strcmp((const char *)fileformat,"latex")==0)
		return new FilterLatex(filein,fileout);
	else if (strcmp((const char *)fileformat,"lambda")==0)
		return new FilterLambda(filein,fileout);
	else if (strcmp((const char *)fileformat,"rtf")==0)
		return new FilterRTF(filein,fileout);
	else
		return NULL;
}

