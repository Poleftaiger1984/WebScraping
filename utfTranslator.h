#ifndef UTFTRANSLATOR_H
#define UTFTRANSLATOR_H
#include <wchar.h>
#include <stdbool.h>


char exePath[260];
char scriptPath[260];

const char* getScriptPath();
void utf8ToWideCharSafe(const char* input, wchar_t* output, int outputSize);
void toggleTerminalVisibility(bool check);

#endif // !UTFTRANSLATOR_H
