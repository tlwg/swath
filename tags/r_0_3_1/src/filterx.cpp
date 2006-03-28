// FilterX.cpp: implementation of the FilterX class.
//
//////////////////////////////////////////////////////////////////////
#include <string.h>
#include "filterx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FilterX::FilterX(char *filein,char *fileout)
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
		fpin=fopen(filein,"r");
	}
	if (fileout) {
		fpout=fopen(fileout,"w");
	}
}

FilterX::~FilterX()
{
	if (fpin!=stdin){
		fclose(fpin);
	}
	if (fpout!=stdout){
		fclose(fpout);
	}

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


