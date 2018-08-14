// FilterRTF.h: interface for the FilterRTF class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __FILTERRTF_H
#define __FILTERRTF_H

#include "filterx.h"

// Private implementation class
class RTFToken;

class FilterRTF : public FilterX
{
public:
  FilterRTF (FILE* filein, FILE* fileout, bool isUniIn, bool isUniOut);

  bool GetNextToken (wchar_t* token, int tokenSz, bool* thaiFlag);
  void Print (const wchar_t* token, bool thaiFlag);

private:
  enum ECharState
  {
    CS_START,
    CS_ESCAPE,
    CS_CH_BYTE,
    CS_UNI,
    CS_UNI_COUNT,
    CS_UNI_CODE
  };

private:
  static ECharState chgCharState (ECharState state, char charIn,
                                  bool* charConsumed, RTFToken* rtfToken);

private:
  void  printNonThai (wchar_t wc);

private:
  ECharState psState;
  wchar_t    strbuff[40];
  int  curUTFReadBytes;
  int  curUTFWriteBytes;
};

#endif
