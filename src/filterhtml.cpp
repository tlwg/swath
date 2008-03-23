// FilterHtml.cpp: implementation of the FilterHtml class.
//
//////////////////////////////////////////////////////////////////////

#include <string.h>
#include "filterhtml.h"
#include "worddef.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FilterHtml::FilterHtml(FILE *filein, FILE *fileout) : FilterX(filein,fileout) 
{
	strcpy(wordBreakStr,"<wbr>");
	strcpy(prefixStr,"");
	strcpy(suffixStr,"");
}

FilterHtml::~FilterHtml()
{

}

bool FilterHtml::GetNextToken(char *token, bool *thaiFlag)
{
char *sttoken;
int tmp=0;

	if ((fpin==NULL)||(feof(fpin)!=0))
		return false;
	sttoken=token;
	if (chbuff==0)
		*token=(char)fgetc(fpin);
	else{
		*token=chbuff;
		chbuff=0; //clear the character buffer.
	}
	*thaiFlag=((*token)&0x80)? true:false;
	//loop for finding a token that containing only Thai characters or Eng+space characters
	while (feof(fpin)==0){
		*(token+1)=(char)fgetc(fpin);
		if (((((*token)^(*(token+1)))&0x80)!=0 ) || (tmp=isSpace((int)*(token+1))) || (*(token+1)=='.') ) {
			if (*thaiFlag) {
				if (*(token+1)=='\n'){
					if (feof(fpin)!=0) break;
					chbuff=(char)fgetc(fpin);
					if (chbuff&0x80){
						*(token+1)=chbuff;
						chbuff=0;
					}else{
						//chbuff=*(token+1);
						*(token+1)=0;
						break;
					}
				}else{
					chbuff=*(token+1);
					*(token+1)=0;
					break;
				}
			}else{ //Eng+space Token
				chbuff=*(token+1);
				*(token+1)=0; //insert '\0';
				break;
			}
		}
		token++;
	}
	if (feof(fpin)!=0) *(token+1)=0;
	token=sttoken;
	return true; //no error occur.

}

void FilterHtml::Print(char *token, bool thaiFlag)
{
	fprintf(fpout,"%s",token);
}

