#ifndef CONV_INC
#define CONV_INC

// inFormat, outFormat: 't' = TIS-620; 'u' = UTF-8
int conv(char inFormat, char outFormat, const char *inText,
         char *outText, int outLen);

#endif

