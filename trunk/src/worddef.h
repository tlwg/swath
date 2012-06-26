#ifndef __WORDDEF_H
#define __WORDDEF_H

#define MAXCHOICE 150           // NUMBER OF ALL POSSIBLE SENTENCES.
#define MAXLEN 2000             // maximum characters in a sentence.
#define MAXSEP 500              // maximum seperation points
                                //   (maximum words in a sentences)
#define MAXTAG 47               // number of all posible POSs.

struct wordState
{
  int backState;
  unsigned char branchState;

  wordState () {}
  wordState (int backState, unsigned char branchState)
    : backState (backState), branchState (branchState) {}
};

typedef struct wt_detail_def
{
  char tag_no;
  float prob;
} wt_detail_type;

typedef struct wt_data_def
{
  char no_pos;
  wt_detail_type detail[15];
} wt_data_type;

typedef struct CDAreaDef
{
  int st;
  int en;
} CDAreaType;

typedef struct SepTypeDef
{
  int Score;
  short int Sep[MAXSEP];
} SepType;

inline bool
isPunc (char ch)
{
//      if (  ((ch>=33)&&(ch<=47)) || ((ch>=58)&&(ch<=63))
//                    || ((ch>=91)&&(ch<=96))  )
  return ch == '\'' || ch == '"' || ch == '`' || ch == '.';
}

inline bool
isThai (char ch)
{
  return ch & 0x80;
}

inline bool
isThaiDigit (unsigned char ch)
{
  return 0xf0 <= ch && ch <= 0xf9;
}

inline bool
isThaiLongTailChar (unsigned char ch)
{
  return ch == 0xbb || ch == 0xbd || ch == 0xbf;
}

inline bool
isThaiUni (unsigned int uc)
{
  return (0x0e01 <= uc) && (uc <= 0x0e5b);
}

inline unsigned int
tis2uni (unsigned char ch)
{
  return (ch & 0x80) ? ch - 0xa0 + 0x0e00 : ch;
}

inline unsigned char
uni2tis (unsigned int uc)
{
  return isThaiUni (uc) ? uc - 0x0e00 + 0x0a0 : uc;
}

#endif
