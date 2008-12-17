// abswordseg.cpp: implementation of the AbsWordSeg class.
//
//////////////////////////////////////////////////////////////////////

//#include ""
#include "abswordseg.h"
#include "dictpath.h"
#ifdef __APPLE__
# include <stdlib.h>
#else
# include <malloc.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AbsWordSeg::AbsWordSeg(){ //Constructor
	IdxSep = (short int*)malloc (MAXLEN*sizeof(short int));
	LinkSep = (short int*)malloc(3*MAXLEN*sizeof(short int));
	if ((IdxSep==NULL)||(LinkSep==NULL)){
		printf("Cannot allocate memory\n");
		exit(0);
	}
	cntSep=0;
}

AbsWordSeg::~AbsWordSeg(){// Destructor
	delete IdxSep;
	delete LinkSep;
}

#define tis2uni(c)  ((c)&0x80?((c)-0xa0+0x0e00):(c))

void AbsWordSeg::CreateWordList(void){
char Buff[2000];
short int i,j,cntLink,en_word;
short int cntFound,amb_sep_cnt;
int data_idx;
unsigned char lead_ch;
TrieState *curState;

   strcpy(Buff,"");
   cntLink=0;
   amb_sep_cnt=0;
   curState=trie_root(MyDict);
   for(i=0;i<len;i++) { //word boundry start at i and end at j.
	
     if ( !IsLeadChar((unsigned char)sen[i])||
          !IsLastChar((unsigned char)sen[i-1]) ){
         IdxSep[i]=-2; //cannot leading for unknown word.
			continue;
      }
      lead_ch=(unsigned char)sen[i];
      // FIND SINGLE PUNCTUATION.
      if ( ((33<=lead_ch)&&(lead_ch<=47))||((58<=lead_ch)&&(lead_ch<=64))||
           ((91<=lead_ch)&&(lead_ch<=96))||((123<=lead_ch)&&(lead_ch<=126)) ){
			IdxSep[i]=cntLink;
      	LinkSep[cntLink++]=i+1;
      	LinkSep[cntLink++]=-1;
       	continue;
      } else if ( ((48<=lead_ch)&&(lead_ch<=57))||((240<=lead_ch)&&(lead_ch<=249)) ){
			// FIND STRING OF NUMBER.
		IdxSep[i]=cntLink;
      	while ( (((48<=lead_ch)&&(lead_ch<=57))||((240<=lead_ch)&&(lead_ch<=249))
         			|| (lead_ch=='.') || (lead_ch==',') )&& (sen[i]!='\0') )  {
		      lead_ch=(unsigned char)sen[++i];
         }
         LinkSep[cntLink++]=i;
         LinkSep[cntLink++]=-1;
         i--;
       	continue;
      }if ( ((65<=lead_ch)&&(lead_ch<=90))||((97<=lead_ch)&&(lead_ch<=122)) ){
            // FIND STRING OF ENGLISH.
			IdxSep[i]=cntLink;
      	while ( (((65<=lead_ch)&&(lead_ch<=90))||((97<=lead_ch)&&(lead_ch<=122)))
         			&& (sen[i]!='\0') ){
		      lead_ch=(unsigned char)sen[++i];
         }
         LinkSep[cntLink++]=i;
         LinkSep[cntLink++]=-1;
         i--;
       	continue;
      }
      cntFound=0;
      trie_state_rewind(curState);
      for (j = 0; i+j<len; j++) {
		 if ((sen[i+j]=='æ' )&&(cntFound!=0)) {//Mai-Ya-Mok -- Stop word point 17 July 2001
			LinkSep[cntLink-1]=i+j+1;
			break;
		 }
         if (!trie_state_walk(curState,tis2uni((unsigned char)sen[i+j]))) {
		 	break;
		 }
		 if( trie_state_is_terminal(curState) ){
            //TrieState *terState = trie_state_clone (curState);
            //trie_state_walk (terState, TRIE_CHAR_TERM);
            //data_idx=trie_state_get_data(terState);
            //trie_state_free (terState);
			//===========================================================
			//found word in dictionary
            //To check word boundary,Is it should be segment or not????
			//===========================================================
			if ( !((Has_Karun(&sen[i+j],&en_word)) || !IsLeadChar((unsigned char)sen[i+j+1])) ){
               LinkSep[cntLink]=i+j+1;
			   LinkSep[cntLink+1]=-1; //debug 24 nov
			   //LinkSepDataIdx[cntLink]=data_idx;
               cntFound++;
               if (cntFound==1) IdxSep[i]=cntLink;
               cntLink++;
			}
         }//end if WalkResult
	  } //end for j
      if (cntFound==0)
      	IdxSep[i]=-1; //
      else if (cntFound < 2000){
        //LinkSepDataIdx[cntLink]=-1;
      	LinkSep[cntLink++]=-1;
      }
   } //end for i
   trie_state_free(curState);
//   amb_sep[amb_sep_cnt].st_idx=-1;
	LinkSep[cntLink]=-1; 
	LinkSep[++cntLink]=-1; //add stop value;
}

bool AbsWordSeg::IsNumber(char *str){
    while (*str) {
        if ((*str < '0' || *str > '9') && *str != '.' && *str != ',')
            return false;
        str++;
    }
    return true;
}
bool AbsWordSeg::IsEnglish(char *str){
    while (*str) {
        if ((*str < 'A' || *str > 'Z') && (*str < 'a' || *str > 'z')
            && (*str != '.') && (*str != '-') && (*str != ' '))
        {
            return false;
        }
        str++;
    }
    return true;
}

bool AbsWordSeg::IsLeadChar(unsigned char ch){
	if ( ((0xcf<=ch)&&(ch<=0xd9)) || ((0xe6<=ch)&&(ch<=0xec)) )
		return false; //false
	else
		return true; //true
}
bool AbsWordSeg::IsLastChar(unsigned char ch){
	if ( ((0xe0<=ch)&&(ch<=0xe4))||(ch==0xd1) )
   	return false;
   else
   	return true;
}
bool AbsWordSeg::Has_Karun(char* sen_ptr,short int *k_idx){
short int i;
   *k_idx=-1;
	for (i=1;i<=3;i++){
      if (*(sen_ptr+i)=='\0')
      	break;
      if (*(sen_ptr+i)==-20){
         *k_idx=i;
         return true;
      }
   }
   return false;
}

bool AbsWordSeg::WordSeg(char* senstr,char *output,char *wbr){
int bestidx;
   strcpy(sen,senstr);
   len=strlen(senstr);
   InitData();
   CreateWordList();
   SwapLinkSep();
   bestidx=CreateSentence();
   GetBestSen(bestidx,wbr,output);
   
   return 0;
}

void AbsWordSeg::SwapLinkSep(){
short int st_idx,en_idx,end_point,tmp;

	st_idx=0,en_idx=0;
	while (LinkSep[st_idx]!=-1){
		while (LinkSep[en_idx++]!=-1);
        end_point=en_idx-1;
		en_idx-=2;
        while (st_idx<en_idx){
	        tmp=LinkSep[st_idx];
			//tmpI=LinkSepDataIdx[st_idx];
    	    LinkSep[st_idx]=LinkSep[en_idx];
			//LinkSepDataIdx[st_idx]=LinkSepDataIdx[en_idx];
			st_idx++;
			LinkSep[en_idx]=tmp;
			//LinkSepDataIdx[en_idx]=tmpI;
			en_idx--;
		}
        st_idx=end_point+1;
		en_idx=st_idx;
	}
}

void AbsWordSeg::GetWord(short int idxsen,short int idx,char *buff){
char *sen_ptr;
   sen_ptr=sen;
	if (idx > 0) {
		strncpy(buff,sen_ptr+SepData[idxsen].Sep[idx-1],SepData[idxsen].Sep[idx]-SepData[idxsen].Sep[idx-1]);
   	buff[SepData[idxsen].Sep[idx]-SepData[idxsen].Sep[idx-1]]='\0';
   }else{
   	strncpy(buff,sen_ptr,SepData[idxsen].Sep[0]);
      buff[SepData[idxsen].Sep[0]]='\0';
   }
}

void AbsWordSeg::InitData(){
int i;
	cntSep=0;
    for(i=0;(i<len) && (i<MAXLEN) ;i++){
    	IdxSep[i]=-1;
    }

}

// =======================================================
// function that copy previous seperation point from
// idxSen-1 to idxSen (copy from idx=0 to idx that
// has value= sepPoint).
// Return index of next seperation point that will be fill.
// ========================================================
unsigned short int AbsWordSeg::copySepData(short int sourceIdxSen,short int targetIdxSen,short int sepPoint){
short int i=0;
	if (sourceIdxSen==targetIdxSen) {
		while(SepData[sourceIdxSen].Sep[i]<=sepPoint){
			i++;
			if (SepData[sourceIdxSen].Sep[i-1]==sepPoint)
				break;
		}
	}else {
		while (SepData[sourceIdxSen].Sep[i]<=sepPoint){
			SepData[targetIdxSen].Sep[i]=SepData[sourceIdxSen].Sep[i];
			i++;
			if (SepData[sourceIdxSen].Sep[i-1]==sepPoint)
				break;
		}
	}
	return i;
}

void AbsWordSeg::GetBestSen(int bestidx,char *wbr,char *outstr){
int t;
char buff[2000];

	*outstr='\0';
	for (t=0;SepData[bestidx].Sep[t]!=len;t++){
		GetWord(bestidx,t,buff);
		strcat(outstr,buff);
		strcat(outstr,wbr);
    }
	GetWord(bestidx,t,buff);
	strcat(outstr,buff);
}

