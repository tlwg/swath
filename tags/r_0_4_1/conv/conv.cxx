//
// conv.cc - Main Program for text converter program
// Created: 19 Jan 1999
// Author:  Theppitak Karoonboonyanan <thep@links.nectec.or.th>
//

#include <stdio.h>
#include "convfact.h"
#include "utf8.h"
#include "tis620.h"

//
// SYNOPSIS
//    conv ([-t|-u], [source], [target]);
// DESCRIPTION
//    'conv' converts text data from the file specified by 'source' and
//    writes the result to 'target' file. If either 'source' or 'target'
//    is omitted, standard input and standard output will be assumed.
// OPTIONS
//    -t  Converts UTF-8 to TIS-620
//    -u  Converts TIS-620 to UTF-8 (default)
//

static int do_conv(char format, FILE *input, FILE *output)
{
    ETextFormat inputFormat = TIS620;
    ETextFormat outputFormat = UTF8;

    if (format == 't') {
        inputFormat = UTF8;
        outputFormat = TIS620;
    } else {
        inputFormat = TIS620;
        outputFormat = UTF8;
    }

    TextReader* pReader = CreateTextReader(inputFormat, input);
    TextWriter* pWriter = CreateTextWriter(outputFormat, output);

    if (pReader && pWriter) {
        TransferText(pReader, pWriter);
    }

    delete pReader;
    delete pWriter;

    return 0;
}

int conv(char format, const char *inputFileName, const char *outputFileName)
{
    FILE*  input = stdin;
    FILE*  output = stdout;

    if (inputFileName) {
        input = fopen(inputFileName, "r");
    }
    if (outputFileName) {
        output = fopen(outputFileName, "w");
    }

    do_conv(format, input, output);

    if (inputFileName)  { fclose(input); }
    if (outputFileName) { fclose(output); }

    return 0;
}

int conv(char format, FILE *input, FILE *output)
{
    if (!input) {
        input = stdin;
    }
    if (!output) {
        output = stdout;
    }

    do_conv(format, input, output);

    fflush(output);

    return 0;
}

