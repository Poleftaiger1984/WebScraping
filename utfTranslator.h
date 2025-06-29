#ifndef UTFTRANSLATOR_H
#define UTFTRANSLATOR_H
#include <wchar.h>


char exePath[260];
char scriptPath[260];

const char* getScriptPath();
void utf8ToWideCharSafe(const char* input, wchar_t* output, int outputSize);

#endif // !UTFTRANSLATOR_H
