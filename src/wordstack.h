#ifndef __WORDSTACK_H
#define __WORDSTACK_H

#if _MSC_VER >= 1000
#pragma once
#endif

#include "worddef.h"

#define STACKSIZE 500

class WordStack
{
public:
  WordStack ();

  bool Push (WordState w);
  bool Pop ();
  WordState Top () const;
  bool Empty () const;

private:
  WordState stack[STACKSIZE];
  short int top_idx;
};

inline
WordStack::WordStack ()
  : top_idx (-1)
{
}

inline bool
WordStack::Push (WordState w)
{
  if (top_idx < STACKSIZE - 1)
    {
      stack[++top_idx] = w;
      return true;
    }
  return false;
}

inline bool
WordStack::Pop ()
{
  if (top_idx >= 0)
    {
      top_idx--;
      return true;
    }
  return false;
}

inline bool
WordStack::Empty () const
{
  return top_idx < 0;
}

inline WordState
WordStack::Top () const
{
  return (top_idx >= 0) ? stack[top_idx] : WordState (-1, 0);
}

#endif
