#define _CRT_SECURE_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN  
#define NOMINMAX
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_TEXT_EDITING 
#include "raygui.h"
#include "x64/Release/resources/style_terminal.h"

#include <locale.h>
#include <wchar.h>

#include "bookkeeping.h"
#include "utfTranslator.h"
#include "globals.h"
#include "cJSON.h"

void setSelectors(bool* manualMode);
itemDetails* parseJSONOutput(const char* jsonInput);

int main()
{	
	/* Initialize */

	setlocale(LC_ALL, "en_US.UTF - 8");

	strncpy_s(script, MAX_PATH, getScriptPath(), MAX_PATH);

	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Scraper");
	Font myFont = LoadFont("resources/IBMPlexMono-Regular.ttf");
	SetTargetFPS(60);
	
	initExcel();

	/* Initialize */
	bool editMode = false;
	bool checkedManual = false;
	bool checkedTerminal = false;

	while (!WindowShouldClose())
	{
		BeginDrawing();

		Color background = { 12, 21, 5, 255 };

		GuiSetStyle(DEFAULT, TEXT_SIZE, 25);
		GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(GREEN));
		GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, ColorToInt(DARKGREEN));
		GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED, ColorToInt(GREEN));
		GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, ColorToInt(GRAY));
		GuiSetStyle(DEFAULT, BORDER_COLOR_FOCUSED, ColorToInt(GREEN));

		GuiSetStyle(BUTTON, BACKGROUND_COLOR, ColorToInt(GREEN));
		ClearBackground(background);
		GuiSetFont(myFont);
		
		Rectangle textBox = { 56, 80, MAX_SELECTOR_SIZE * 3, 50 };
		Rectangle checkBox = { 56, 190, 20, 20 };
		Rectangle startButton = { 56, WINDOW_HEIGHT - 100, 50, 70 };

		DrawRectangleLines(10, 10, WINDOW_WIDTH - 20, WINDOW_HEIGHT - 20, GRAY);
		DrawRectangleLines(20, 20, WINDOW_WIDTH - 40, WINDOW_HEIGHT - 40, GRAY);

		if (editMode && IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_V))
		{
			const char* clip = GetClipboardText();
			if (clip)
			{
				strncpy_s(url, sizeof(url), clip, MAX_SELECTOR_SIZE);
				url[sizeof(url) - 1] = '\0';
			}
		}

		GuiLabel((Rectangle) { 56, 40, 200, 50 }, "ENTER YOUR URL: ");
		if (GuiTextBox(textBox, url, sizeof(url), editMode))
			editMode = !editMode;
		/* Input */

		GuiLabel((Rectangle) { 56, 150, 200, 50 }, "ADVANCED SETTINGS ");

		if (GuiCheckBox((Rectangle) { 56, 200, 20, 20 }, "SHOW TERMINAL", &checkedTerminal));
		{
			checkedTerminal != checkedTerminal;
			toggleTerminalVisibility(checkedTerminal);
		}

		if(GuiCheckBox((Rectangle) { 56, 240, 20, 20 }, "MANUAL SELECTOR CONTROL", &checkedManual));
		{
			checkedManual != checkedManual;
			setSelectors(checkedManual);
		}
			
		if (GuiLabelButton(startButton, "BEGIN SCRAPING"))
		{
			snprintf(command, sizeof(command), "node \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\"",
				script,
				url,
				linkSelector,
				variationSelector,
				titleSelector,
				storeAreaSelector,
				storeSelector,
				priceSelector,
				storeItemsSelector,
				storeInfoSelector,
				storeEmailSelector);
			fp = _popen(command, "r");

			itemDetails* details = calloc(1, sizeof(itemDetails));

			char* jsonRaw = malloc(MAX_BUFFER);
			if (!jsonRaw)
			{
				fprintf(stderr, "Failed to allocate memory for the raw JSON buffer.\n");
				_pclose(fp);
				free(details);
				return 1;
			}

			size_t bytesRead = fread(jsonRaw, 1, MAX_BUFFER - 1, fp);
			jsonRaw[bytesRead] = '\0';
			_pclose(fp);

			if (bytesRead == 0) {
				fprintf(stderr, "No data was read from the script.\n");
				free(jsonRaw);
				free(details);
				return 1;
			}

			details = parseJSONOutput(jsonRaw);
			if (!details)
			{
				printf("Items array failed at allocation.\n");
				free(jsonRaw);
				free(details);
				return 1;
			}

			printf("Raw JSON output (%zu bytes):\n%s\n", bytesRead, jsonRaw);
			printf("Parsed product count: %d\n", details->productCount);
			wprintf(L"Variation Count: %d\n", details->products[0].variationCount);
			wprintf(L"Store Count: %d\n", details->products[0].variations[0].storeCount);

			/* Read JSON Output */

			/* Write to Excel */

			writeToExcel(details);

			/* Write to Excel */

			/* Save and Deallocate */

			if(saveExcel()) GuiLabel((Rectangle) { 250, WINDOW_HEIGHT - 100, 50, 70 }, "SAVED TO EXCEL SUCCESSFULLY");

			free(details);
			free(jsonRaw);

			/* Save and Deallocate */
		}
		else if (url == 0)
		{
			GuiLabel((Rectangle) { 250, WINDOW_HEIGHT - 100, 50, 70 }, "URL CANNOT BE EMPTY");
		}

		EndDrawing();
	}

	//printf("Enter your URL: \n");
	//fgets(url, sizeof(url), stdin);
	//url[strcspn(url, "\n")] = '\0'; //At the point where \n shows up replace it with null terminator

	CloseWindow();

	return 0;
}

void setSelectors(bool* manualMode)
{
	if (manualMode == false)
	{
		strncpy_s(linkSelector, MAX_SELECTOR_SIZE, "a.js-sku-link[data-e2e-testid=\"sku-title-link\"]", MAX_SELECTOR_SIZE);
		strncpy_s(variationSelector, MAX_SELECTOR_SIZE, "li.with-image.spec-variant", MAX_SELECTOR_SIZE);
		strncpy_s(titleSelector, MAX_SELECTOR_SIZE, "h1.page-title", MAX_SELECTOR_SIZE);
		strncpy_s(storeAreaSelector, MAX_SELECTOR_SIZE, "li[data-testid=\"product-card\"]", MAX_SELECTOR_SIZE);
		strncpy_s(storeSelector, MAX_SELECTOR_SIZE, ".shop-name", MAX_SELECTOR_SIZE);
		strncpy_s(priceSelector, MAX_SELECTOR_SIZE, "strong.dominant-price", MAX_SELECTOR_SIZE);
		strncpy_s(storeItemsSelector, MAX_SELECTOR_SIZE, "a.shop-products.icon", MAX_SELECTOR_SIZE);
		strncpy_s(storeInfoSelector, MAX_SELECTOR_SIZE, "div.shop-services a.shop-info", MAX_SELECTOR_SIZE);
		strncpy_s(storeEmailSelector, MAX_SELECTOR_SIZE, "div.shop-contact p:nth-of-type(3)", MAX_SELECTOR_SIZE);

	}

	else if (manualMode == true)
	{
		static int activeTextBox = -1;

		//printf("Enter the CSS selector for the item pages: \n");
		if (GuiTextBox((Rectangle) { 56, 270, MAX_SELECTOR_SIZE * 3, 20 }, linkSelector, sizeof(linkSelector), activeTextBox == SELECTOR_LINK))
		{
			activeTextBox = (activeTextBox == SELECTOR_LINK) ? -1 : SELECTOR_LINK;
		}

		//fgets(linkSelector, sizeof(linkSelector), stdin);
		//linkSelector[strcspn(linkSelector, "\n")] = '\0';

		//printf("Enter the CSS selector for item variations: \n");
		if (GuiTextBox((Rectangle) { 56, 300, MAX_SELECTOR_SIZE * 3, 20 }, variationSelector, sizeof(variationSelector), activeTextBox == SELECTOR_VARIATION))
		{
			activeTextBox = (activeTextBox == SELECTOR_VARIATION) ? -1 : SELECTOR_VARIATION;
		}
		/*fgets(variationSelector, sizeof(variationSelector), stdin);
		variationSelector[strcspn(variationSelector, "\n")] = '\0';*/

		//printf("Enter the CSS selector for the item titles: \n");
		if (GuiTextBox((Rectangle) { 56, 330, MAX_SELECTOR_SIZE * 3, 20 }, titleSelector, sizeof(titleSelector), activeTextBox == SELECTOR_TITLE))
		{
			activeTextBox = (activeTextBox == SELECTOR_TITLE) ? -1 : SELECTOR_TITLE;
		}
		/*fgets(titleSelector, sizeof(titleSelector), stdin);
		titleSelector[strcspn(titleSelector, "\n")] = '\0';*/

		//printf("Enter the CSS selector for store area: \n");
		if (GuiTextBox((Rectangle) { 56, 360, MAX_SELECTOR_SIZE * 3, 20 }, storeAreaSelector, sizeof(storeAreaSelector), activeTextBox == SELECTOR_AREA))
		{
			activeTextBox = (activeTextBox == SELECTOR_AREA) ? -1 : SELECTOR_AREA;
		}
		/*fgets(storeAreaSelector, sizeof(storeAreaSelector), stdin);
		storeAreaSelector[strcspn(storeAreaSelector, "\n")] = '\0';*/

		//printf("Enter the CSS selector for the store name: \n");
		if (GuiTextBox((Rectangle) { 56, 390, MAX_SELECTOR_SIZE * 3, 20 }, storeSelector, sizeof(storeSelector), activeTextBox == SELECTOR_STORE))
		{
			activeTextBox = (activeTextBox == SELECTOR_STORE) ? -1 : SELECTOR_STORE;
		}
		/*fgets(storeSelector, sizeof(storeSelector), stdin);
		storeSelector[strcspn(storeSelector, "\n")] = '\0';*/

		//printf("Enter the CSS selector for the store price: \n");
		if (GuiTextBox((Rectangle) { 56, 420, MAX_SELECTOR_SIZE * 3, 20 }, priceSelector, sizeof(priceSelector), activeTextBox == SELECTOR_PRICE))
		{
			activeTextBox = (activeTextBox == SELECTOR_PRICE) ? -1 : SELECTOR_PRICE;
		}
		/*fgets(priceSelector, sizeof(priceSelector), stdin);
		priceSelector[strcspn(priceSelector, "\n")] = '\0';*/

		//printf("Enter the CSS selector for the store's item page: \n");
		if (GuiTextBox((Rectangle) { 56, 450, MAX_SELECTOR_SIZE * 3, 20 }, storeItemsSelector, sizeof(storeItemsSelector), activeTextBox == SELECTOR_STORE_ITEMS))
		{
			activeTextBox = (activeTextBox == SELECTOR_STORE_ITEMS) ? -1 : SELECTOR_STORE_ITEMS;
		}
		/*fgets(storeItemsSelector, sizeof(storeItemsSelector), stdin);
		storeItemsSelector[strcspn(storeItemsSelector, "\n")] = '\0';*/

		//printf("Enter the CSS selector for the store's info page: \n");
		if (GuiTextBox((Rectangle) { 56, 480, MAX_SELECTOR_SIZE * 3, 20 }, storeInfoSelector, sizeof(storeInfoSelector), activeTextBox == SELECTOR_STORE_INFO))
		{
			activeTextBox = (activeTextBox == SELECTOR_STORE_INFO) ? -1 : SELECTOR_STORE_INFO;
		}
		/*fgets(storeInfoSelector, sizeof(storeInfoSelector), stdin);
		storeInfoSelector[strcspn(storeInfoSelector, "\n")] = '\0';*/

		//printf("Enter the CSS selector for the store's desired contact info: \n");
		if (GuiTextBox((Rectangle) { 56, 510, MAX_SELECTOR_SIZE * 3, 20 }, storeEmailSelector, sizeof(storeEmailSelector), activeTextBox == SELECTOR_STORE_EMAIL))
		{
			activeTextBox = (activeTextBox == SELECTOR_STORE_EMAIL) ? -1 : SELECTOR_STORE_EMAIL;
		}
		/*fgets(storeEmailSelector, sizeof(storeEmailSelector), stdin);
		storeEmailSelector[strcspn(storeEmailSelector, "\n")] = '\0';*/
	}
}


itemDetails* parseJSONOutput(const char* jsonInput)
{
	cJSON *root = cJSON_Parse(jsonInput);
	itemDetails *details = calloc(1, sizeof(itemDetails));

	if (!root)
	{
		printf("JSON parsing failed: %s.\n", cJSON_GetErrorPtr());
		return details;
	}

	if (!details)
	{
		printf("Memory allocation failed.\n");
		return NULL;
	}

	int productCount = cJSON_GetArraySize(root);
	for (int p = 0; p < productCount; p++)
	{
		cJSON* productObj = cJSON_GetArrayItem(root, p);
		cJSON* productUrl = cJSON_GetObjectItemCaseSensitive(productObj, "product");
		cJSON* variations = cJSON_GetObjectItemCaseSensitive(productObj, "variations");

		if (cJSON_IsString(productUrl))
		{
			utf8ToWideCharSafe(productUrl->valuestring, details->products[p].productUrl, MAX_STRING);
		}

		if (cJSON_IsArray(variations))
		{
			int variationCount = cJSON_GetArraySize(variations);
			for (int v = 0; v < variationCount && v < MAX_VARIATIONS; v++)
			{
				cJSON* variation = cJSON_GetArrayItem(variations, v);
				cJSON* title = cJSON_GetObjectItemCaseSensitive(variation, "title");
				cJSON* stores = cJSON_GetObjectItemCaseSensitive(variation, "stores");

				if (cJSON_IsString(title))
				{
					utf8ToWideCharSafe(title->valuestring, details->products[p].variations[v].variationTitle, MAX_STRING);
				}

				if (cJSON_IsArray(stores))
				{
					int storeCount = cJSON_GetArraySize(stores);
					details->products[p].variations[v].storeCount = (storeCount < MAX_STORES) ? storeCount : MAX_STORES;

					for (int s = 0; s < storeCount && s < MAX_STORES; s++)
					{
						cJSON* store    = cJSON_GetArrayItem(stores, s);
						cJSON* name     = cJSON_GetObjectItemCaseSensitive(store, "name");
						cJSON* price    = cJSON_GetObjectItemCaseSensitive(store, "price");
						cJSON* shopLink = cJSON_GetObjectItemCaseSensitive(store, "shopLink");
						cJSON* contact  = cJSON_GetObjectItemCaseSensitive(store, "email");

						StoreInfo* storeInfo = &details->products[p].variations[v].stores[s];

						if (cJSON_IsString(name))
						{
							utf8ToWideCharSafe(name->valuestring, storeInfo->storeName, MAX_STRING);
						}
						if (cJSON_IsString(price))
						{
							utf8ToWideCharSafe(price->valuestring, storeInfo->price, MAX_STRING);
						}
						if (cJSON_IsString(shopLink))
						{
							utf8ToWideCharSafe(shopLink->valuestring, storeInfo->link, MAX_STRING);
						}
						if(cJSON_IsString(contact))
						{
							utf8ToWideCharSafe(contact->valuestring, storeInfo->contactInfo, MAX_STRING);
						}
					}
				}

				details->products[p].variationCount++;
			}
		}

		details->productCount++;
	}

	cJSON_Delete(root);
	return details;
}