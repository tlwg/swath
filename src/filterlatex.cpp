// FilterLatex.cpp: implementation of the FilterLatex class.
//
//////////////////////////////////////////////////////////////////////

#include <string.h>
#include "filterlatex.h"
#include "worddef.h"
//Normal Characters (tonemarks & vowels ) of both of Windows and Macintosh
//Not according Position of vowels or tone mark.

						//|====OffsetHigh======| |===========Offset Normal============|
int WinMacNormal[12]     = {232, 233, 234, 235, 236, 209, 212, 213, 214, 215, 231, 237};

//Character Sets for Macintosh
int MacOffsetLeft[12]    = {131, 132, 133, 134, 135, 146, 148, 149, 150, 151, 147, 143};
int MacOffsetLeftHigh[5] = {152, 153, 154, 155, 156};
int MacOffsetNormal[5]   = {136, 137, 138, 139, 140};

//Character Sets for Windows (not unicode)
int WinOffsetLeft[12]    = {134, 135, 136, 137, 138, 152, 129, 130, 131, 132, 154, 153};
int WinOffsetLeftHigh[5] = {155, 156, 157, 158, 159};
int WinOffsetNormal[5]   = {139, 140, 141, 142, 143};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FilterLatex::FilterLatex(char *filein, char *fileout ,int latexflag): FilterX (filein,fileout)
{
	strcpy(wordBreakStr,"{\\wbr}");
	strcpy(prefixStr,"");
	strcpy(suffixStr,"");
	strcpy(buffer,"");
	verbatim=false; // Is in verbatim mode??
	latexFlag=latexflag;	
	if (latexFlag==1)
		winCharSet=true;
}

FilterLatex::~FilterLatex()
{

}

bool FilterLatex::GetNextToken(char * token, bool * thaiFlag)
{
char *curPtr;
char *stPtr, tmpch;
char *stVer;

	if (strcmp(buffer,"")==0) 
		if (fgets(buffer,1999,fpin)==NULL)
			return false;
	curPtr=buffer;
	stPtr=buffer;
	strcpy(token,"");
	if (verbatim) {
		*thaiFlag=false;
		if ((stVer=strstr(buffer,"\\end{verbatim}"))==NULL){
			strcpy(token,buffer);
			strcpy(buffer,"");
			return true;
		}else{
			stVer+=strlen("\\end{verbatim}");
			tmpch=*stVer;
			*stVer=0;
			strcpy(token,buffer);
			*stVer=tmpch;
			strcpy(buffer,stVer);
			verbatim=false;
			return true;
		}
	}

	while (isPunc(*curPtr))
		curPtr++;
	*thaiFlag=(*curPtr < 0)? true:false;
	curPtr++;
	if (*thaiFlag){
		for(;;){ //for finding thai line + thailine +...+ thailine
			while (*curPtr<0) {  //isPunc(*curPtr);
				curPtr++;
			}
			if ((*curPtr!=0)&&(*curPtr!='\n')){
				tmpch=*curPtr;
				*curPtr=0;
				strcat(token,stPtr);
				*curPtr=tmpch;
				strcpy(buffer,curPtr); //store new buffer
				return true;
			}else{
				if (*curPtr=='\n')
					*curPtr=0; // remove new line character
				strcat(token,stPtr);
				strcpy(buffer,"");
				//if next a line is thai string, concat next line
				// to current line
				if (fgets(buffer,1999,fpin)==NULL){		
					strcat(token,"\n");
					return true; //next GetToken() must return false
				}
				curPtr=buffer;
				if (*curPtr>0){ //not thai character
					strcat(token,"\n");
					return true;
				}
			}
		}
	}else{
		while ((*curPtr>0) || (isPunc(*curPtr)) || (isSpace(*curPtr)) ){
			curPtr++;
		}
		if (*curPtr!=0){
			tmpch=*curPtr;
			*curPtr=0;
			strcpy(token,stPtr);
			*curPtr=tmpch;
			strcpy(buffer,curPtr); //store new buffer
		}else{
			strcpy(token,stPtr);
			strcpy(buffer,""); //clear buffer
		}
		if ( strstr(token,"\\begin{verbatim}")!=NULL ){
			verbatim=true; //entrance to verbatim model
		}
	}

	return true; //no error occur.
}

void FilterLatex::Print(char * token, bool  thaiFlag)
{
int lwbr;
	lwbr=strlen(wordBreakStr);
	if (thaiFlag){
		if (latexFlag!=0){
			char *output=new char[2000];
			AdjustText((unsigned char *)token,(unsigned char *) output);
			fprintf(fpout,"%s",output);
			delete output;
		}else{
			fprintf(fpout,"%s",token);
		}
	}else{
		fprintf(fpout,"%s",token);
	}
	fflush(fpout);
}

void FilterLatex::AdjustText(unsigned char * input, unsigned char * output)
{
int idxNormal;
int cntChar=1;
bool chgchar=false;
unsigned char *tmpInput;
unsigned char *tmpOutput;
	tmpInput=input;
	tmpOutput=output;
	while(*input!=0) {
		//Sara-Amm must split to Sara-Arr + NiKhaHit(circle)
		chgchar=false;
		if (*input==211) {
			if (cntChar>1) {
				if ( isLongTailChar(input[-1]) ) 
					//case Long Tail+Sara-Amm
					*output=(winCharSet==true)? 153:143; //offset left		
				else if ((idxNormal=idxVowelToneMark(input[-1]))<12){
					//case character+ToneMark+Sara-Amm
					if (cntChar>2){
						if (isLongTailChar(input[-2])){					
							*output=(winCharSet==true) ?
								WinOffsetLeftHigh[idxNormal]:MacOffsetLeftHigh[idxNormal]; //offset left
							output[-1]=(winCharSet==true)? 153:143;	//offset left
						}else{
							*output=input[-1];
							output[-1]=237;
						}
					}
				}else
					*output=237;
			}else
				*output=237;
			*(++output)=210; //Sara-Arr
			chgchar=true;
		}else if ((idxNormal=idxVowelToneMark(*input))<12){
			if (cntChar>1){
				if (isLongTailChar(input[-1])){
					// Long Tail Char + Vowel or Tonemarks.
					*output=(winCharSet==true)? 
						WinOffsetLeft[idxNormal]: MacOffsetLeft[idxNormal];
					chgchar=true;
				} else if (idxVowelToneMark(input[-1])<12) {
					//char + Vowel + Tone Mark
					if (cntChar>2){
						if (isLongTailChar(input[-2])){
							//Long Tail Char + Vowel + Tone Mark
							*output=(winCharSet==true)? 
								WinOffsetLeftHigh[idxNormal]: MacOffsetLeftHigh[idxNormal];
							chgchar=true;
						}
					}					
				} else{
					//Normal Char + Tone Mark
					if ((idxNormal<5)&&(input[1]!=211)){
						*output=(winCharSet==true)? 
							WinOffsetNormal[idxNormal]: MacOffsetNormal[idxNormal];
						chgchar=true;
					}
				}
			}
		}else if ((*input==216)||(*input==217)){ //Sara-Ui. Sara-U
			if (cntChar>1){
				switch (input[-1]) {
					case 173: //YoYing
						output[-1]=144;
						break;
					case 174: //DoChaDa
						chgchar=true;
						*output=252;
						break;
					case 175: // ToPaTak
						chgchar=true;
						*output=253;
						break;
					case 176: //ThoSanTan
						output[-1]=(winCharSet==true)? 128:159;
				}
			}				
		}
		if (!chgchar)
			*output=*input;
		output++;
		input++;
		cntChar++;
	}
	*output=0;
	input=tmpInput;
	output=tmpOutput;

}

bool FilterLatex::isLongTailChar(unsigned char ch)
{
	return ((ch==187) || (ch==189) || (ch==191)) ? true:false;
}

int FilterLatex::idxVowelToneMark(unsigned char ch)
{
int i=0;

	while ( (WinMacNormal[i]!=ch)&&(i<12) )
		i++;
	return i;
}




/*
				if ((*(token+1)=='\n')&&(isPunc(*token))) {
					token=&token[-1];
				}else if (*(token+1)=='\n'){
					if (feof(fpin)!=0) break;
					chbuff=(char)fgetc(fpin);
					if (isPunc(chbuff))
						*(token+1)=chbuff;
					chbuff=0;
					break;
				}else{
					chbuff=*(token+1);
					*(token+1)=0; //insert '\0';
					break;
				}
*/

/*
old get next token before 9 July 1999
bool FilterLatex::GetNextToken(char * token, bool * thaiFlag)
{
char *sttoken;
int tmp;
char tmpch;
//int chlead; //==1 follow string ==-1 before Thai string
	if ((fpin==NULL)||(feof(fpin)!=0))
		return false;
	sttoken=token;
	if (chbuff==0)
		*token=(char)fgetc(fpin);
	else
		*token=chbuff;
	*thaiFlag=((*token)<0)? true:false;
	//lood find token that contain the Thai+punc or Eng+space+punc string
	while (feof(fpin)==0){
		*(token+1)=(char)fgetc(fpin);
		if (((((*token)^(*(token+1)))&0x80)!=0 )  
			   || (tmp=isSpace((int)*(token+1)))) {
			//for stop searching the same type string.
			if (*(token+1)=='\n') {

				chbuff=fgetc(fpin);
				if ((chbuff=='\n')||(*token=='\n')){
					ungetc(chbuff,fpin);	
				}else{
					ungetc(chbuff,fpin);
					if (isPunc(*token)){
						continue;
					}else {
						chbuff=fgetc(fpin);
						if (isPunc(chbuff)){
							*(token+1)=0;
							break;
						}else{
							token++;
							break;
						}
					}			
				}
			} 
			if (*thaiFlag) {
				if (*(token+1)=='\n'){
					if (feof(fpin)!=0) break;
					chbuff=(char)fgetc(fpin);
					if ( (chbuff<0) ) //||(isPunc(chbuff)) )
						*(token+1)=chbuff;
					else{
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
	*(token+1)=0;
	token=sttoken;
	return true; //no error occur.

}


*/
