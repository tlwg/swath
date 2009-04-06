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
  FILE* fp;

  char *d2triepath = new char[strlen(wordsegdata) + 2 + strlen(D2TRIE)];
  strcpy(d2triepath, wordsegdata);
  strcat(d2triepath, PATHSEPERATOR);
  strcat(d2triepath, D2TRIE);    //printf("%s\n", dbranchpath); fflush(stdout);
  if ((fp = fopen(d2triepath, "r")) == NULL) {
	  delete[] d2triepath;
	  return 1;
  }
  delete[] d2triepath;
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
  return 0;
}

void ExitWordSegmentation(AbsWordSeg *wseg)
{
  delete wseg;
}

void WordSegmentation(AbsWordSeg *wseg, char *wbr, char *line, char* output)
{
	//RemoveJunkChar(line);
	wseg->WordSeg(line,output,wbr);
}

static void Usage(int verbose)
{
    printf("Usage: swath [mule|-v] [-b \"delimitor\"] [-d dict-dir]\n"
           "[-f html|rtf|latex|lambda] [-m long|max] [-u {u|t},{u|t}] [-help]\n");
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
	         "\t-u   : specify encodings of input and output in 'i,o' form,\n"
	         "\t       for input and output respectively, where 'i', 'o' is one of:\n"
		 "\t\tu        : The input/output is in UTF-8\n"
		 "\t\tt        : The input/output is in TIS-620\n"
	         "\t-help: display this help message\n");
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
  
  strcpy(wbr, "|");
  strcpy(mulestr,"");
  for (int iargc = 1; iargc < argc; iargc++) {
    if (strcmp("mule", argv[iargc]) == 0){
      strcpy(mulestr,"mule");
      mode = 1;
    } else if (strcmp("-b", argv[iargc]) == 0 && iargc + 1 < argc) {
      iargc++;
      delete[] wbr;
      wbr = new char[strlen(argv[iargc]) + 1];
      strcpy(wbr, argv[iargc]);
    } else if (strcmp("-d", argv[iargc]) == 0 && iargc + 1 < argc) {
      iargc++;
      wsegpath = new char[strlen(argv[iargc]) + 1];
      strcpy(wsegpath, argv[iargc]);
    } else if (strcmp("-f", argv[iargc]) == 0 && iargc + 1 < argc) {
      iargc++;
      fileformat = new char[strlen(argv[iargc]) + 1];
      strcpy(fileformat, argv[iargc]);
    } else if (strcmp("-v", argv[iargc]) == 0 ||
               strcmp("--verbose", argv[iargc]) == 0)
    {
      mode=0;
    } else if (strcmp("-m", argv[iargc]) == 0 && iargc + 1 < argc) {
      iargc++;
      method = new char[strlen(argv[iargc]) + 1];
      strcpy(method,argv[iargc]);
    } else if (strcmp("-l", argv[iargc]) == 0) {
      //send only token which has no white space in to wordseg
      wholeLine=true;
    } else if (strcmp("-u", argv[iargc]) ==0 && iargc + 1 < argc) {
      iargc++; // format
      unicode = new char[strlen(argv[iargc]) + 1];
      strcpy(unicode, argv[iargc]);
    } else if (strcmp("-help", argv[iargc]) == 0 ||
               strcmp("--help", argv[iargc]) == 0)
    {
      Usage(1);
      delete[] wbr;
      delete[] wsegpath;
      delete[] fileformat;
      delete[] method;
      delete[] unicode;
      return 1;
    } else {
      Usage(0);
      delete[] wbr;
      delete[] wsegpath;
      delete[] fileformat;
      delete[] method;
      delete[] unicode;
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
  
  char line[MAXCHAR+1], output[MAXCHAR*2+1];
  int i;
  int c;
  char leadch[3],folch[3];
  int retval;

  if  (( retval=InitWordSegmentation(".", &wseg)) > 0)
	  retval=InitWordSegmentation(wsegpath, &wseg);

  delete[] wsegpath;
  delete[] method;
  if (retval > 0) {
    delete[] wbr;
    return 1;
  }
  strcpy(leadch,"");
  strcpy(folch,"");



  FILE *tmpout=stdout, *tmpin=stdin;
  if (unicode!=NULL) { //Option -u
	  if (unicode[0]=='u') { //unicode input file.
		  tmpin = tmpfile();
		  conv('t', NULL, tmpin);
		  rewind(tmpin);
	  }
	  if (unicode[2]=='u'){
		  tmpout = tmpfile();
	  }
  }
  if (fileformat!=NULL) {
	  FilterX* FltX = FileFilter::CreateFilter(tmpin,tmpout,fileformat);
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
		WordSegmentation(wseg, wbr, line, output);
		FltX->Print(output,thaifag);
	  }
	  delete FltX;
  }else{
	  char stopstr[20];
	  if (strcmp(mulestr,"mule")==0)
		strcpy(stopstr,wbr);
	  else
		strcpy(stopstr,"");
	  for (;;) { // read until end of file.
		if (mode == 0) printf("Input : ");
		for (i = 0; ((c = fgetc(tmpin)) != '\n')
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
					WordSegmentation(wseg, wbr, buff, output);
					strcat(gout,output);
				}
				strcat(gout, wbr);
			}
		}else{
			WordSegmentation(wseg, wbr, line, output);
			strcpy(gout,output);
			strcat(gout,stopstr);
		}
		if (mode == 0) printf("Output: ");
		fprintf(tmpout, "%s\n", gout);
		if (feof(tmpin)) break;
	  }// end for (;;)
  }
  if (unicode!=NULL) {
	  if (unicode[2]=='u') {
		rewind(tmpout);
		conv('u', tmpout, NULL);
		fclose(tmpout);
	  }
	  if (unicode[0]=='u') {
		fclose(tmpin);
	  }
	  delete[] unicode;
  }

  delete[] fileformat;
  delete[] wbr;
  ExitWordSegmentation(wseg);

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
