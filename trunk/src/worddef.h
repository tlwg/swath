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

inline bool isSpace(char ch)
{
	return (ch==' ') || ('\t'<=ch && ch<='\r');
}

inline bool isHex(char ch)
{
	return ('0'<=ch && ch<='9') || ('a'<=ch && ch<='f');
}

inline bool isPunc(char ch)
{
//	if (  ((ch>=33)&&(ch<=47)) || ((ch>=58)&&(ch<=63))
//		      || ((ch>=91)&&(ch<=96))  )
	return ch=='\'' || ch=='"' || ch=='`' || ch=='.';
}

inline bool isThai(char ch)
{
	return ch & 0x80;
}

#endif
