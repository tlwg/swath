// FilterRTF.h: interface for the FilterRTF class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __FILTERRTF_H
#define __FILTERRTF_H

#if _MSC_VER >= 1000
#pragma once
#endif

#include "filterx.h"

// Private implementation class
class RTFToken;

class FilterRTF : public FilterX
{
public:
  FilterRTF (FILE* filein, FILE* fileout, bool isUniIn, bool isUniOut);

  bool GetNextToken (char* token, bool* thaiFlag);
  void Print (char* token, bool thaiFlag);

private:
  enum ECharState
  {
    CS_START,
    CS_ESCAPE,
    CS_CH_BYTE,
    CS_UNI,
    CS_UNI_COUNT,
    CS_UNI_CODE,
    CS_NONE
  };

private:
  static ECharState chgCharState (ECharState state, char charIn,
                                  bool* charConsumed, RTFToken* rtfToken);

private:
  void  printNonThai (char c);

private:
  ECharState psState;
  char strbuff[5];
  int  curUTFReadBytes;
  int  curUTFWriteBytes;
};

#endif
