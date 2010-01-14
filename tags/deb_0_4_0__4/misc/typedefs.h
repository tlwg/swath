//
// typedefs.h - general types & machine-dependent types
// Created : 23 May 1996
// Author  : Theppitak Karoonboonyanan
//

#ifndef TYPEDEFS_INC
#define TYPEDEFS_INC

#include <limits.h>

//
// General Types
//

// Simulate ISO C++ bool type if not provided by the C++ compiler

# if defined(_BOOL)
#   define __HAS_BOOL
# elif defined(__BOOL)
#   define __HAS_BOOL
# elif defined(__GNUC__)
#   if (__GNUC__ > 2) || (__GNUC__ == 2 && __GNUC_MINOR__ >= 6)
#     define __HAS_BOOL
#   endif
# elif defined(_MSC_VER) && _MSC_VER > 1000
#   define __HAS_BOOL
# elif defined(__BORLANDC__) && __BORLANDC__ >= 0x500
#   define __HAS_BOOL
# endif

# if !defined(__HAS_BOOL)
    enum { false = 0, true = 1 };
    typedef int bool;
# endif // __HAS_BOOL


//
// Machine-Dependent Types
//

#if 0
typedef unsigned char  byte;    //  8-bit unsigned
typedef unsigned short word;    // 16-bit unsigned
typedef unsigned long  dword;   // 32-bit unsigned

typedef signed char    int8;    //  8-bit signed
typedef short          int16;   // 16-bit signed
typedef long           int32;   // 32-bit signed
typedef unsigned char  uint8;   //  8-bit unsigned
typedef unsigned short uint16;  // 16-bit unsigned
typedef unsigned long  uint32;  // 32-bit unsigned
#endif

# if UCHAR_MAX == 0xff
#   ifndef UINT8_TYPEDEF
#     define UINT8_TYPEDEF
      typedef unsigned char  uint8;
#   endif // UINT8_TYPEDEF
# endif // UCHAR_MAX

# if SCHAR_MAX == 0x7f
#   ifndef INT8_TYPEDEF
#     define INT8_TYPEDEF
      typedef signed char    int8;
#   endif // INT8_TYPEDEF
# endif // SCHAR_MAX

# if UINT_MAX == 0xffff
#   ifndef UINT16_TYPEDEF
#     define UINT16_TYPEDEF
      typedef unsigned int   uint16;
#   endif // UINT16_TYPEDEF
# endif // UINT_MAX

# if INT_MAX == 0x7fff
#   ifndef INT16_TYPEDEF
#     define INT16_TYPEDEF
      typedef int            int16;
#   endif // INT16_TYPEDEF
# endif // INT_MAX

# if USHRT_MAX == 0xffff
#   ifndef UINT16_TYPEDEF
#     define UINT16_TYPEDEF
      typedef unsigned short uint16;
#   endif // UINT16_TYPEDEF
# endif // USHRT_MAX

# if SHRT_MAX == 0x7fff
#   ifndef INT16_TYPEDEF
#     define INT16_TYPEDEF
      typedef short          int16;
#   endif // INT16_TYPEDEF
# endif // SHRT_MAX

# if UINT_MAX == 0xffffffff
#   ifndef UINT32_TYPEDEF
#     define UINT32_TYPEDEF
      typedef unsigned int   uint32;
#   endif // UINT32_TYPEDEF
# endif // UINT_MAX

# if INT_MAX == 0x7fffffff
#   ifndef INT32_TYPEDEF
#     define INT32_TYPEDEF
      typedef int            int32;
#   endif // INT32_TYPEDEF
# endif // INT_MAX

# if ULONG_MAX == 0xffffffff
#   ifndef UINT32_TYPEDEF
#     define UINT32_TYPEDEF
      typedef unsigned long  uint32;
#   endif // UINT32_TYPEDEF
# endif // ULONG_MAX

# if LONG_MAX == 0x7fffffff
#   ifndef INT32_TYPEDEF
#     define INT32_TYPEDEF
      typedef long           int32;
#   endif // INT32_TYPEDEF
# endif // LONG_MAX

# ifndef UINT8_TYPEDEF
#   error "uint8 type is undefined!"
# endif
# ifndef INT8_TYPEDEF
#   error "int8 type is undefined!"
# endif
# ifndef UINT16_TYPEDEF
#   error "uint16 type is undefined!"
# endif
# ifndef INT16_TYPEDEF
#   error "int16 type is undefined!"
# endif
# ifndef UINT32_TYPEDEF
#   error "uint32 type is undefined!"
# endif
# ifndef INT32_TYPEDEF
#   error "int32 type is undefined!"
# endif

typedef uint8  byte;
typedef uint16 word;
typedef uint32 dword;

//
// Limits & Constants
//

const int MaxPathLen     = 1024;
const int MaxFileNameLen = 12;

//
// Debugging Aid
//

# ifndef ASSERT
#   include <assert.h>
#   define ASSERT(e)  assert(e)
# endif // ASSERT

# ifndef VERIFY
#   ifdef NDEBUG
#     define VERIFY(e)  ((void)(e))
#   else
#     define VERIFY(e)  ASSERT(e)
#   endif // NDEBUG
# endif // VERIFY

#endif // TYPEDEFS_INC

