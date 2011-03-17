#ifndef __WORDDEF_H

#define __WORDDEF_H

#define MAXCHOICE 150 // NUMBER OF ALL POSSIBLE SENTENCES.
#define MAXLEN 2000   // maximum characters in a sentence.
#define MAXSEP 500   // maximun of seperation points (maximun words in a sentences)
#define MAXTAG 47     // number of all posible POSs.

typedef struct wordStateDef {
	int backState;
	unsigned char branchState;
} wordStateType;

typedef struct wt_detail_def {
  char tag_no;
  float prob;
} wt_detail_type;

typedef struct wt_data_def{
   char no_pos;
   wt_detail_type detail[15];
} wt_data_type;

typedef struct CDAreaDef{
	int st;
	int en;
} CDAreaType;

typedef struct SepTypeDef {
   float Score;
   short int Sep[MAXSEP];
} SepType;

bool isSpace(char ch);
bool isHex(char ch);
bool isPunc(char ch);
bool isThai(char ch);

#endif
