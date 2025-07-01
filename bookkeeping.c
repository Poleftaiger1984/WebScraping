#include "libxl.h"
#include "bookkeeping.h"

BookHandle book;
SheetHandle sheet;
FormatHandle* titleFormat;
FormatHandle* detailsFormat;


int initExcel()
{
	/* Initialize Excel */

	book = xlCreateXMLBookCW();
	if (!book)
	{
		wprintf(L"Failed to create Excel book.\n");
		return 1;
	}

	sheet = xlBookAddSheetW(book, L"Sheet 1", NULL);
	if (!sheet)
	{
		wprintf(L"Failed to create Excel sheet.\n");
		xlBookReleaseW(book);
		return 1;
	}

	titleFormat = xlBookAddFormatFromStyleW(book, CELLSTYLE_HEADING1);
	detailsFormat = xlBookAddFormatFromStyleW(book, CELLSTYLE_ACCENT1);

	xlFormatSetAlignHW(titleFormat, ALIGNH_CENTER);
	xlFormatSetAlignHW(detailsFormat, ALIGNH_CENTER);


	/* Initialize Excel */
}

void writeToExcel(itemDetails* details)
{
	/* Write to EXCEL */

	int currentRow = 1;

	for (int p = 0; p < details->productCount; p++)
	{
		int variationCount = details->products[p].variationCount;
		int maxStoreRows = 0;

		int totalCols = variationCount * 4;

		xlSheetSetMergeW(sheet, currentRow, currentRow, 0, totalCols - 1);
		xlSheetSetMergeW(sheet, currentRow + 1, currentRow + 1, 0, totalCols - 1);

		xlSheetWriteStrW(sheet, currentRow, 0, L"PRODUCT", titleFormat);
		xlSheetWriteStrW(sheet, currentRow + 1, 0, details->products[p].productUrl, detailsFormat);

		for (int v = 0; v < variationCount; v++)
		{
			int varriationColStart = v * 4;

			xlSheetSetMergeW(sheet, currentRow + 3, currentRow + 3, varriationColStart, varriationColStart + 3);
			xlSheetSetMergeW(sheet, currentRow + 4, currentRow + 4, varriationColStart, varriationColStart + 3);

			xlSheetWriteStrW(sheet, currentRow + 3, varriationColStart, L"VARIATION", titleFormat);
			xlSheetWriteStrW(sheet, currentRow + 4, varriationColStart, details->products[p].variations[v].variationTitle, detailsFormat);

			xlSheetWriteStrW(sheet, currentRow + 6, varriationColStart + 0, L"Seller", titleFormat);
			xlSheetWriteStrW(sheet, currentRow + 6, varriationColStart + 1, L"Price", titleFormat);
			xlSheetWriteStrW(sheet, currentRow + 6, varriationColStart + 2, L"Shop Link", titleFormat);
			xlSheetWriteStrW(sheet, currentRow + 6, varriationColStart + 3, L"Contact Info", titleFormat);

			int storeCount = details->products[p].variations[v].storeCount;
			if (storeCount > maxStoreRows) maxStoreRows = storeCount;

			for (int s = 0; s < storeCount; s++)
			{
				StoreInfo* store = &details->products[p].variations[v].stores[s];
				int row = currentRow + 7 + s;

				xlSheetWriteStrW(sheet, row, varriationColStart + 0, store->storeName, detailsFormat);
				xlSheetWriteStrW(sheet, row, varriationColStart + 1, store->price, detailsFormat);
				xlSheetWriteStrW(sheet, row, varriationColStart + 2, store->link, detailsFormat);
				xlSheetWriteStrW(sheet, row, varriationColStart + 3, store->contactInfo, detailsFormat);

			}
		}
		// Move down by the minimum needed rows for this product + all the store rows
		currentRow += 7 + maxStoreRows + 2; //2 padding between products

		xlSheetSetAutoFitAreaW(sheet, 0, 0, currentRow + maxStoreRows, MAX_VARIATIONS + maxStoreRows);
	}

	/* Write to EXCEL */
}

bool saveExcel()
{
	if (!xlBookSaveW(book, L"prices.xlsx"))
	{
		printf("Failed to save Excel file.: %s\n\n", xlBookErrorMessageW(book));
		xlBookReleaseW(book);
		return false;
	}

	xlBookReleaseW(book);
	wprintf(L"\nData written to prices.xlsx successfully.\n");
	return true;
}
