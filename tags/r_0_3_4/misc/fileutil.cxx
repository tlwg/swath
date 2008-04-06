/*
 * fileutil.cpp - file utility functions
 * Created : 11 Jul 1996
 * Author:  Theppitak Karoonboonyanan
 */

#include "misc/fileutil.h"

#define FILEBUFFSIZE 1024

static char buf[FILEBUFFSIZE];

int copyfile(const char* from, const char* to)
{
    FILE *fFrom = fopen(from, "rb");
    FILE *fTo   = fopen(to, "wb+");
    size_t  nRead;

    if (fFrom == NULL)  { return -1; }
    if (fTo   == NULL)  { return -1; }

    while ((nRead = fread(buf, 1, sizeof buf, fFrom)) > 0) {
        if (fwrite(buf, 1, nRead, fTo) != nRead)  { return -1; }
    }

    fclose(fTo);
    fclose(fFrom);

    return 0;
}

int movefile(const char* from, const char* to)
{
    /* try renaming first, if fail, make a copy */
    if (rename(from, to) == 0) {
        return 0;
    } else {
        if (copyfile(from, to) == 0) {
            remove(from);
            return 0;
        }
    }

    return -1;
}

