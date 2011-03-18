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

const char* FilterX::GetWordBreak()
{
	return wordBreakStr;
}

