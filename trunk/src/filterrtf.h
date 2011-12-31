// FilterRTF.h: interface for the FilterRTF class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __FILTERRTF_H
#define __FILTERRTF_H

#if _MSC_VER >= 1000
#pragma once
#endif

#include "filterx.h"

class FilterRTF : public FilterX
{
public:
  FilterRTF (FILE* filein, FILE* fileout);

  bool GetNextToken (char* token, bool* thaiFlag);
  void Print (char* token, bool thaiFlag);

private:
  //sequence of characters is    \ ' x x (one character)
  //states of this sequence are  1 2 3 4  
  static int chgCharState (char charin, int state);
  static bool isThaiChar (char *token, int *state);

private:
  int psState;
  char strbuff[5];
};

#endif
