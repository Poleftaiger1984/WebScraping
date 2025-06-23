#include <locale.h>
#include <wchar.h>

#include "libxl.h"
#include "globals.h"
#include "raylib.h"

int main()
{	
	//Title selector : a.js-sku-link[data-e2e-testid="sku-title-link"]
	//Price selector : a.js-sku-link[data-e2e-testid="sku-price-link"]

	setlocale(LC_ALL, "en_US.UTF-8");
	BookHandle book = xlCreateXMLBookCW();

	if (!book)
	{
		wprintf(L"Failed to create Excel book.\n");
		return 1;
	}

	SheetHandle sheet = xlBookAddSheetW(book, L"Sheet 1", NULL);
	
	if (!sheet)
	{
		wprintf(L"Failed to create Excel sheet.\n");
		xlBookReleaseW(book);
		return 1;
	}

	printf("Enter your URL: \n");
	scanf_s("%511s", url, (unsigned)sizeof(url));

	printf("Enter the CSS selector for the item titles: \n");
	scanf_s("%511s", titleSelector, (unsigned)sizeof(titleSelector));

	printf("Enter the CSS selector for the item prices: \n");
	scanf_s("%511s", priceSelector, (unsigned)sizeof(priceSelector));

	snprintf(command, sizeof(command), "node packages/node_modules/puppeteer/itemscraping.js \"%s\" \"%s\" \"%s\"", url, titleSelector, priceSelector);

	fp = _popen(command, "r");

	if (fp == NULL)
	{
		perror("Failed to run Node Script");
		return 1;
	}

	//printf("Laptop Titles:\n");

	while (fgetws(buffer, sizeof(buffer)/sizeof(wchar_t), fp))
	{
		size_t length = wcslen(buffer);

		if (length > 0 && buffer[length - 1] == L"\n")
		{
			buffer[length - 1] = L'\0';
		}

		xlSheetWriteStrW(sheet, rows++, 1, buffer, NULL);
	}
		

	if (!xlBookSaveW(book, L"prices.xlsx"))
	{
		wprintf(L"Failed to save Excel file.: L%s\n\n", xlBookErrorMessageW(book));
		xlBookReleaseW(book);
		return 1;
	}

	xlBookReleaseW(book);
	wprintf(L"\nData written to prices.xlsx successfully.\n");

	_pclose(fp);
	return 0;
}