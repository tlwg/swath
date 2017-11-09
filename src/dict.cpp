//
// dict.cpp - dictionary class
// Created: 2017-11-08
// Author: Theppitak Karoonboonyanan
//

#include "dict.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#ifdef WIN32
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif

#define DICT_FILENAME "swathdic.tri"

//
// class Dict
//

bool
Dict::open (const char* dictPath)
{
  char*       trieBuff = 0;
  const char* triePath;
  struct stat sb;

  if (stat (dictPath, &sb) == -1)
    {
      perror (dictPath);
      return false;
    }

  if (S_ISDIR (sb.st_mode))
    {
      trieBuff = new char[strlen (dictPath) + 2 + sizeof (DICT_FILENAME)];
      sprintf (trieBuff, "%s" PATH_SEP DICT_FILENAME, dictPath);
      triePath = trieBuff;
    }
  else if (S_ISREG (sb.st_mode))
    {
      triePath = dictPath;
    }
  else
    {
      fprintf (stderr, "%s is not a directory or regular file\n", dictPath);
      return false;
    }

  dict = trie_new_from_file (triePath);

  if (trieBuff)
    {
      delete[] trieBuff;
    }

  return dict != 0;
}

