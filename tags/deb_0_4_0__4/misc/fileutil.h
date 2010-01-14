/*
 * fileutil.h - declarations of portable directory/file utility functions
 * Created: 11 Jul 1996
 * Author:  Theppitak Karoonboonyanan
 */

#ifndef FILEUTIL_INC
#define FILEUTIL_INC

// #ifdef __cplusplus
// extern "C" {
// #endif /* __cplusplus */

/* inherit all stdio utilities, such as remove(), rename(), etc. */
#include <stdio.h>

int copyfile(const char* from, const char* to);
int movefile(const char* from, const char* to);

// #ifdef __cplusplus
// }
// #endif /* __cplusplus */

#endif  // FILEUTIL_INC

