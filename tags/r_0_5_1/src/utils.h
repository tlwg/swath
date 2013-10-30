// utils.h: utility functions.
//
//////////////////////////////////////////////////////////////////////

#ifndef __UTILS_H
#define __UTILS_H

#define N_ELM(a)  (sizeof (a)/sizeof (a)[0])

template <class T>
inline T min (T a, T b) { return (a < b) ? a : b; }

template <class T>
inline T max (T a, T b) { return (a > b) ? a : b; }

#endif // __UTILS_H
