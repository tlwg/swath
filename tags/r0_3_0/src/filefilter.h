// FileFilter.h: interface for the FileFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEFILTER_H__5FA01279_2213_11D3_B449_00105A5C2417__INCLUDED_)
#define AFX_FILEFILTER_H__5FA01279_2213_11D3_B449_00105A5C2417__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "filterhtml.h"
#include "filterrtf.h"
#include "filterlatex.h"
#include "filterlambda.h"

class FileFilter
{
public:
	FilterX * CreateFilter(char *filein, char *fileout, char *fileformat);
	FileFilter();
	virtual ~FileFilter();
};

#endif // !defined(AFX_FILEFILTER_H__5FA01279_2213_11D3_B449_00105A5C2417__INCLUDED_)
