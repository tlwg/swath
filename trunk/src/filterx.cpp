// FilterX.cpp: implementation of the FilterX class.
//
//////////////////////////////////////////////////////////////////////
#include <string.h>
#include "filterx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FilterX::FilterX(FILE *filein, FILE *fileout)
{
/*  initial string (difference in each format)
	strcpy(wordBreakStr,"\\tb ");
	strcpy(prefixStr,"{\\thai ");
	strcpy(suffixStr,"}");
*/
	chbuff=0;
	fpin=stdin;
	fpout=stdout;

	if (filein) {
		fpin = filein;
	}
	if (fileout) {
		fpout = fileout;
	}
}

FilterX::~FilterX()
{
	fflush(fpout);
}

bool FilterX::GetWordBreak(char *wbr)
{
	strcpy(wbr,wordBreakStr);
	return true;
}

bool FilterX::GetPrefix(char *preStr)
{
	strcpy(preStr,prefixStr);
	return true;
}

bool FilterX::GetSuffix(char *suffStr)
{
	strcpy(suffStr,suffixStr);
	return true;
}


