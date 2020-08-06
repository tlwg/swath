#include <wchar.h>

wchar_t*
wcpcpy (wchar_t* dest, const wchar_t* src)
{
  wcscpy (dest, src);
  return dest + wcslen (dest);
}

