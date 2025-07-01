#include "utfTranslator.h"
#include <Shlwapi.h>
#include <Windows.h>

const char* getScriptPath()
{
	GetModuleFileNameA(NULL, exePath, MAX_PATH);
	PathRemoveFileSpecA(exePath);
	snprintf(scriptPath, MAX_PATH, "%s/packages/node_modules/puppeteer/itemscraping.js", exePath);

	return scriptPath;
}

void utf8ToWideCharSafe(const char* input, wchar_t* output, int outputSize)
{
	if (!input || !output || outputSize <= 0) return;

	int length = MultiByteToWideChar(CP_UTF8, 0, input, -1, output, outputSize);

	if (length == 0 || length >= outputSize)
	{
			output[0] = L'\0';
	}
	
}

void toggleTerminalVisibility(bool check)
{
	if (check)
	{
		HWND hwnd = GetConsoleWindow();
		ShowWindow(hwnd, SW_SHOW);
	}
	else
	{
		HWND hwnd = GetConsoleWindow();
		ShowWindow(hwnd, SW_HIDE);
	}
}
