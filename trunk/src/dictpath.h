
// Author: Chaivit Poovanich
// October 14, 1998

#ifndef DICTPATH_INC
#define DICTPATH_INC

extern char *d2branchpath;	// Path for
extern char *d2tailpath;	// Path for d2tail.tri


#ifdef WIN32
#define PATHSEPERATOR "\\"
#else
#define PATHSEPERATOR "/"
#endif

#define D2BRANCH "swathdic.br"
#define D2TAIL   "swathdic.tl"


#endif
