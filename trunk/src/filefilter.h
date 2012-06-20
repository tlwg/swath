// FileFilter.h: interface for the FileFilter class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __FILEFILTER_H
#define __FILEFILTER_H

#if _MSC_VER >= 1000
#pragma once
#endif

#include "filterx.h"

class FileFilter
{
public:
  static FilterX* CreateFilter (FILE* filein, FILE* fileout,
                                bool isUniIn, bool isUniOut,
                                const char *fileformat);
};

#endif
