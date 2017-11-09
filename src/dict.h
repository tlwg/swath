//
// dict.h - dictionary class
// Created: 2017-11-08
// Author: Theppitak Karoonboonyanan
//

#ifndef __DICT_H
#define __DICT_H

#include <datrie/trie.h>
#include <wchar.h>

class Dict
{
public:
  class State
  {
    friend class Dict;

  public:
    ~State ();

    bool isTerminal () const;
    void rewind ();
    bool walk (wchar_t c);

  private:
    State (TrieState* s);

  private:
    TrieState* state;
  };

public:
  Dict ();
  explicit Dict (const char* dictPath);
  ~Dict ();

  bool   open (const char* dictPath);
  State* root () const;

private:
  Trie*  dict;
};


//
// class Dict::State
//

inline
Dict::State::State (TrieState *s)
  : state (s) {}

inline
Dict::State::~State ()
{
  trie_state_free (state);
}

inline bool
Dict::State::isTerminal() const
{
  return trie_state_is_terminal (state);
}

inline void
Dict::State::rewind ()
{
  trie_state_rewind (state);
}

inline bool
Dict::State::walk (wchar_t c)
{
  return trie_state_walk (state, c);
}


//
// class Dict
//

inline
Dict::Dict ()
  : dict (0) {}

inline
Dict::Dict (const char* dictPath)
{
  open (dictPath);
}

inline
Dict::~Dict ()
{
  if (dict)
    {
      trie_free (dict);
    }
}

inline Dict::State*
Dict::root () const
{
  return new State (trie_root (dict));
}

#endif // __DICT_H

