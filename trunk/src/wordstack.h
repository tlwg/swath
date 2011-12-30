#if !defined(INC_WORDSTACK)
#define INC_WORDSTACK

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "worddef.h"

#define STACKSIZE 500

class WordStack
{
private:
  wordState stack[STACKSIZE];
  short int top_idx;

public:
  WordStack ();

  bool Push (wordState w);
  bool Pop ();
  wordState Top ();
  bool Empty ();
};

inline
WordStack::WordStack ()
  : top_idx (-1)
{
}

inline bool
WordStack::Push (wordState w)
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
WordStack::Empty ()
{
  return top_idx < 0;
}

inline wordState
WordStack::Top ()
{
  return (top_idx >= 0) ? stack[top_idx] : wordState (-1, 0);
}

#endif
