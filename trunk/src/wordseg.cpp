// ==========================================
// Author: Chaivit Poovanich
// October 14, 1998
// Modified by : Paisarn Charoenpornsawat
// 1999
// ==========================================
// New features.
//	 1. Support many formats: LaTex, HTML,
//		 Text, RTF (for Opensource, Eamac 1999)
//	 2. Remove garbage characters.
//		 (for a. virach requirement 2000)
//	 3. Select wordseg algorithms, Bi-gram
//	     Maximal Matching, Longest Matching
//		 (for Opensource, Eamac 1999)
// ===========================================


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "filefilter.h"
#include "filterx.h"
#include "dictpath.h"
#include "longwordseg.h"
#include "maxwordseg.h"
#include "conv/conv.h"

// Return
// 0: successful
// 1: can not get WORDSEGDATA environment variable
// 2: not found alldict.bin
// 3: not found dbranch.tri
// 4: not found pbranch.tri
// 5: not found dtail.tri
// 6: not found ptail.tri
char *method=NULL;
char mulestr[5];
char buff[2000],gout[5000];
char *startStr;

int myisspace(int ch);
int SplitToken(char **str, char *token);
bool IsJunkChar(unsigned char ch);
void RemoveJunkChar(char *str);



int InitWordSegmentation(const char *wordsegdata, AbsWordSeg **wseg)
{
  int wsegpathlen = strlen(wordsegdata);
  FILE* fp;


  d2branchpath = new char[wsegpathlen + 2 + strlen(D2BRANCH)];
  strcpy(d2branchpath, wordsegdata);
  strcat(d2branchpath, PATHSEPERATOR);
  strcat(d2branchpath, D2BRANCH);    //printf("%s\n", dbranchpath); fflush(stdout);
  if ((fp = fopen(d2branchpath, "r")) == NULL) 
	  return 1;
  fclose(fp);

  d2tailpath = new char[wsegpathlen + 2 + strlen(D2TAIL)];
  strcpy(d2tailpath, wordsegdata);
  strcat(d2tailpath, PATHSEPERATOR);
  strcat(d2tailpath, D2TAIL);    //printf("%s\n", dtailpath); fflush(stdout);
  if ((fp = fopen(d2tailpath, "r")) == NULL) 
	  return 2;
  fclose(fp);

  if (method==NULL){
	  *wseg=new MaxWordSeg(wordsegdata);
	  return 0;
  }
  if (strcmp(method,"long")==0){
	  *wseg=new LongWordSeg(wordsegdata);
  }else{
	  *wseg=new MaxWordSeg(wordsegdata);
  }
  delete method;
  return 0;
}

void ExitWordSegmentation(AbsWordSeg **wseg)
{
  delete d2branchpath;
  delete d2tailpath;
  delete *wseg;
}

void WordSegmentation(AbsWordSeg **wseg, char *wbr, char *line, char* output)
{
	//RemoveJunkChar(line);
	(*wseg)->WordSeg(line,output,wbr);
}

static void Usage(int verbose)
{
    printf("Usage: swath [mule|-v] [-b \"delimitor\"] [-d dict-dir]\n"
           "[-f html|rtf|latex|lambda] [-m long|max] [-l] [-help]\n");
    if (verbose) {
	  printf("Options:\n"
	         "\tmule : for use with mule\n"
	         "\t-v   : verbose mode\n"
	         "\t-b   : define a word delimitor string for the output\n"
	         "\t-d   : specify dictionary path\n"
	         "\t-f   : specify format of the input\n"
	         "\t\thtml     : HTML file\n"
	         "\t\trtf      : RTF file\n"
	         "\t\tlatex    : LaTeX file\n"
	         "\t\tlambda   : The input and output are same as latex, except that\n"
	         "\t\t           the word delimitor is ^^^^^^^^200c\n"
//	         "\t\twinlatex : LaTeX file shaping on Windows\n"
//	         "\t\tmaclatex : LaTeX file shaping on Macintosh\n"
	         "\t-m   : choose word matching scheme when analyzing\n"
	         "\t\tlong     : longest matching scheme\n"
	         "\t\tmax      : maximal matching scheme\n"
//	         "\t-l   : line processing(effect only in a bigram algo.)\n"
	         "\t-help: Help\n");
    }
}

#ifndef WORDSEGDATA_DIR
#define WORDSEGDATA_DIR "/usr/local/lib/wordseg"
#endif
//#define WORDSEGDATA_DIR "./data"

#define MAXCHAR 2000

int main(int argc, char *argv[])
{
  char mode = 1;  // 0 = display, 1 = don't display message
  char *wbr = new char[20];
  char *wsegpath = NULL;
  char *fileformat = NULL;
  char *unicode = NULL;
  bool thaifag;
  bool wholeLine=false;
  FILE *fpin=stdin, *fpout=stdout;
  
  strcpy(wbr, "|");
  strcpy(mulestr,"");
  for (int iargc = 1; iargc < argc; iargc++) {
	  if (strcmp("mule", argv[iargc]) == 0){
		strcpy(mulestr,"mule");
		mode = 1;
	  }else if (strcmp("-b", argv[iargc]) == 0 && iargc + 1 < argc) {
      iargc++;
      delete wbr;
      wbr = new char[strlen(argv[iargc]) + 1];
      strcpy(wbr, argv[iargc]);
      continue;
    } else if (strcmp("-d", argv[iargc]) == 0 && iargc + 1 < argc) {
      iargc++;
      wsegpath = new char[strlen(argv[iargc]) + 1];
      strcpy(wsegpath, argv[iargc]);
      continue;
    } else if (strcmp("-f", argv[iargc]) == 0 && iargc + 1 < argc) {
      iargc++;
      fileformat = new char[strlen(argv[iargc]) + 1];
      strcpy(fileformat, argv[iargc]);
      continue;
    } else if (strcmp("-v", argv[iargc]) == 0 ) {
	  mode=0;
    } else if (strcmp("-m", argv[iargc]) == 0 && iargc + 1 < argc) {
      iargc++;
      method = new char[strlen(argv[iargc]) + 1];
	  strcpy(method,argv[iargc]);
      continue;
	} else if (strcmp("-l", argv[iargc]) == 0 ) {
		//send only token which has no white space in to wordseg
		wholeLine=true;
	} else if (strcmp("-u", argv[iargc]) ==0 ) {
		iargc++; // format
		unicode = new char[strlen(argv[iargc]) + 1];
		strcpy(unicode, argv[iargc]);
	}else if (strcmp("-help", argv[iargc]) == 0 ) {
		Usage(1);
		delete wbr;
		delete wsegpath;
		return 1;
	}else{
		Usage(0);
		delete wbr;
		delete wsegpath;
		return 1;
	}
  }

  if (mode == 0) printf("*** Word Segmentation ***\n");

  if (wsegpath == NULL) {
    char *env = getenv("WORDSEGDATA");
    if (env != NULL) {
      wsegpath = new char[strlen(env) + 1];
      strcpy(wsegpath, env);
    }
    else {
      wsegpath = new char[strlen(WORDSEGDATA_DIR) + 1];
      strcpy(wsegpath, WORDSEGDATA_DIR);
    }
  }

  AbsWordSeg *wseg;
  FileFilter *FileFlt;
  FilterX *FltX;
  
  char line[MAXCHAR+1], output[MAXCHAR*2+1];
  int i;
  int c;
  char leadch[3],folch[3];
  int retval;

  if  (( retval=InitWordSegmentation(".", &wseg)) > 0)
	  retval=InitWordSegmentation(wsegpath, &wseg);

  delete wsegpath;
  if (retval > 0) {
    delete wbr;
    return 1;
  }
  strcpy(leadch,"");
  strcpy(folch,"");



  char *tmpout=NULL, *tmpin=NULL;
  if (unicode!=NULL) { //Option -u
	  if (unicode[0]=='u') { //unicode input file.
		  tmpin = tmpnam( NULL );
		  conv('t', NULL, tmpin);
	  }
	  if (unicode[2]=='u'){
		  tmpout = tmpnam( NULL );
	  }
  }
  if (fileformat!=NULL) {
	  FileFlt=new FileFilter();
	  FltX=FileFlt->CreateFilter(tmpin,tmpout,fileformat);
	  if (FltX==NULL) {
		printf("Invalid file format: %s\n", fileformat);
		// FIXME: still mem leak hmm..
		return 1;
	  }
	  FltX->GetWordBreak(wbr);
	  while (FltX->GetNextToken(line,&thaifag)){
		if (!thaifag){
			FltX->Print(line,thaifag);
			continue;
		}
		WordSegmentation(&wseg, wbr, line, output);
		FltX->Print(output,thaifag);
	  }
  }else{
  if (tmpin)
	fpin=fopen(tmpin,"r"); //set fpin
  if (tmpout)
	fpout=fopen(tmpout, "w"); //set fpout

	  char stopstr[20];
	  if (strcmp(mulestr,"mule")==0)
		  strcpy(stopstr,wbr);
	  else
		  strcpy(stopstr,"");
	  for (;;) { // read until end of file.
		if (mode == 0) printf("Input : ");
		for (i = 0; ((c = fgetc( fpin )) != '\n')
			&& (i <= MAXCHAR) && (c!=EOF);i++) 
			line[i] = (char)c;
		line[i] = 0;
		strcpy(gout,"");
		//if (strcmp(line, "exit") == 0) break;
		if (strcmp(line,"")==0) 
			if (c==EOF) break; else continue;
		int tokenFlag;
		startStr=line;
		if (!wholeLine) {
 			while ((tokenFlag=SplitToken(&startStr,buff))>=0) {
				if (tokenFlag==-1)
					break;
				if (tokenFlag==0) {
					strcat(gout,buff);
				} else {
					WordSegmentation(&wseg, wbr, buff, output);
					strcat(gout,output);
				}
				strcat(gout, wbr);
			}
		}else{
			WordSegmentation(&wseg, wbr, line, output);
			strcpy(gout,output);
			strcat(gout,stopstr);
		}
		if (mode == 0) printf("Output: ");
		fprintf(fpout,"%s\n", gout);
		fflush(fpout);
		if (feof(fpin)!=0) break;
	  }// end for (;;)
  }
  if (unicode!=NULL) {
	  if (unicode[2]=='u') {
		  if (fpout!=stdout)
			fclose(fpout);
		  conv('u', tmpout, NULL);
		  remove(tmpout);
	  }
	  if (unicode[0]=='u') {
		  if (fpout!=stdin)
			fclose(fpin);
		  remove(tmpin);
	  }
	  delete unicode;
  }

  delete fileformat;
  delete wbr;
  ExitWordSegmentation(&wseg);

  return 0;
}

int SplitToken(char **str, char *token){
//return -1 in case there is no data in str
//return 0 for a token which is contain only white spaces.
//return 1 for a token which is contain only alpha charecters.
int i=0;

	if (**str==0)
		return -1;
	if (myisspace(**str)!=0){
		//found white space.
		while ( (myisspace(**str)!=0) && (**str!=0) ) {
			*token=**str;
			(*str)++;
			token++;
		}
		*token=0;
		return 0;
	}else{
		while ( (myisspace(**str)==0) && (**str!=0) ) {
			*token=**str;
			(*str)++;
			token++;
		}
		*token=0;				
		return 1;
	}
}

int myisspace(int ch){
	
	if ((ch>=9 && ch <=13) || (ch==32))
		return ch;
	else
		return 0;

}

bool IsJunkChar(unsigned char ch){
	if ((ch==9)||(ch==10)||(ch==13)||(ch >=32 && ch<=126)||(ch>=161 && ch<=251))
		return false;
	else
		return true;
}

void RemoveJunkChar(char *str){
char *srcStr,*desStr;
	srcStr=str;
	desStr=str;
	while (*srcStr!=0) {
		if (!IsJunkChar(*srcStr)){
			*desStr=*srcStr;
			desStr++;
		}
		srcStr++;
	}
	*desStr=0;
}
