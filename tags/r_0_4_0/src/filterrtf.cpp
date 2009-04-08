// FilterRTF.cpp: implementation of the FilterRTF class.
//
//////////////////////////////////////////////////////////////////////
#include <string.h>
#include "worddef.h"
#include "filterrtf.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FilterRTF::FilterRTF(FILE *filein, FILE *fileout)
    : FilterX (filein, fileout)
{
	psState=0;
	wordBreakStr[0]=0xDC;
	wordBreakStr[1]=0;
	prefixStr[0]='\0';
	suffixStr[0]='\0';
	strbuff[0]='\0';

}

FilterRTF::~FilterRTF()
{

}

bool FilterRTF::GetNextToken(char * token, bool * thaiFlag)
{
char *sttoken,*tmp;
int nextState,i=0,l,c;

	//sequence of characters is    \ ' x x (one character for Thai char)
	*token='\0';
	sttoken=token;
	
	if ((fpin==NULL)||(feof(fpin)))
		return false;
	if (strbuff[0]=='\0'){
		do {
			if ((c=fgetc(fpin))==EOF) {
				*thaiFlag=false;
				*token=0;
				token=sttoken;
				return false;
			}
			*token=c;
			nextState=chgCharState(*(token++),psState);
			if (nextState <= psState)
				break;
			psState=nextState;
			i++;
		} while (i<4);
		*thaiFlag=(i==4)?true:false;			
		if (*thaiFlag){
			*token=0;
			tmp=&token[-2];
			sscanf(tmp,"%x",&token[-4]);
			token=&token[-3];
		}
	}else{
		strcpy(token,strbuff);
		*thaiFlag=(((l=strlen(strbuff))>1) ||(*token>0) )? false : true;
		token+=strlen(strbuff);
	}
	if (feof(fpin)){
		token=sttoken;
		return false;
	}
	if ((c=fgetc(fpin))==EOF) {
		*thaiFlag=false;
		*token=0;
		token=sttoken;
		return false;
	}
	*token=c;
	

 	if (!(*thaiFlag)){
		//Loop finding a sequence of Non-Thai characters
		//So to stop finding seq of Non-Thai, must find Thai character
		while (((!isSpace(*token++)) && 
			   (nextState=chgCharState(token[-1],psState))!=4)){
			psState=nextState;
			if ((c=fgetc(fpin))==EOF) break;
			*token=c;
		}
		*token=0;
		//found thai character
		if (nextState==4) {
			psState=4;
			tmp=&token[-2];
			sscanf(tmp,"%x",strbuff);
			token[-4]=0;
		}else{
			psState=0;
			strbuff[0]='\0';
		}

	}else{
		//Loop finding a sequence of Thai characters
		//So to stop finding seq of Thai, must find non-thai character
		while (((nextState=chgCharState(*(token++),psState))>psState)
			    ||(psState==4 && nextState==1)){
			psState=nextState;
			if (psState==4){
				*token=0;
				tmp=&token[-2];
				sscanf(tmp,"%x",&token[-4]);
				token=&token[-3];
			}
			if ((c=fgetc(fpin))==EOF) break;
			*token=c;
		}		
		*token=0;
		tmp=&token[-1*(psState%4)-1];
		strcpy(strbuff,tmp);
		*tmp=0;
	}
	token=sttoken;
	//chbuff save only previous char.
	//Except in case prev strings is \?, chbuff save only ? char
	return true; //no error occur.

}

void FilterRTF::Print(char * token, bool thaiFlag)
{
	if (!thaiFlag){
		fprintf(fpout,"%s",token);
	}else{
		while (*token!=0){
			fprintf(fpout,"\\'%02x",(unsigned char)*token);
			token++;
		}
	}
}

int FilterRTF::chgCharState(char charin, int state)
{
	//sequence of characters is    \ ' x x (one character)
	//states of this sequence are  1 2 3 4 	
	//Assume thai character has format like \'xx.
	//retune current state of character.
	switch (state) {
		case 0:
			return  (charin=='\\') ? 1 : 0;
		case 1:
			if (charin=='\'')
				return 2;
			else if (charin=='\\')
				return 1;
			else
				return 0;
		case 2:
			if (isHex(charin))
				return 3;
			else if (charin=='\\')
				return 1;
			else
				return 0;
		case 3:
			if (isHex(charin))
				return 4;
			else if (charin=='\\')
				return 1;
			else
				return 0;
		case 4:
			return (charin=='\\') ? 1 : 0;
	}
	return 0;
}

bool FilterRTF::isThaiChar(char *token, int *state)
{  //retune true for thai char.
	//retunrn state and pointer to the character .
int len;
int nextstate;

	if ((len=strlen(token))<4){
		token+=len;
		*state=0;
		return false;
	}
	while ((nextstate=chgCharState(*token,*state))>*state){
		token++;
		*state=nextstate;
	}
	*state=nextstate;
	token++;
	if (*state==4){ 
		return true;
	}else{
		return false;
	}

}

