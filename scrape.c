#define WIN32_LEAN_AND_MEAN  
#define NOMINMAX
#include "raylib.h"

#include <locale.h>
#include <wchar.h>

#include "bookkeeping.h"
#include "utfTranslator.h"
#include "globals.h"
#include "cJSON.h"

itemDetails* parseJSONOutput(const char* jsonInput);
void utf8ToWideCharSafe(const char* input, wchar_t* output, int outputSize);

int main()
{	
	setlocale(LC_ALL, "en_US.UTF - 8");

	/* Build JS Path */

	strncpy_s(script, MAX_PATH, getScriptPath(), MAX_PATH);

	/* Build JS Path */

	/* Initialize Excel */
	
	initExcel();

	/* Initialize Excel */


	/* Input */

	printf("Enter your URL: \n");
	fgets(url, sizeof(url), stdin);
	url[strcspn(url, "\n")] = '\0'; //At the point where \n shows up replace it with null terminator

	printf("Enter the CSS selector for the item pages: \n");
	fgets(linkSelector, sizeof(linkSelector), stdin);
	linkSelector[strcspn(linkSelector, "\n")] = '\0';

	printf("Enter the CSS selector for item variations: \n");
	fgets(variationSelector, sizeof(variationSelector), stdin);
	variationSelector[strcspn(variationSelector, "\n")] = '\0';

	printf("Enter the CSS selector for the item titles: \n");
	fgets(titleSelector, sizeof(titleSelector), stdin);
	titleSelector[strcspn(titleSelector, "\n")] = '\0';

	printf("Enter the CSS selector for store area: \n");
	fgets(storeAreaSelector, sizeof(storeAreaSelector), stdin);
	storeAreaSelector[strcspn(storeAreaSelector, "\n")] = '\0';

	printf("Enter the CSS selector for the store name: \n");
	fgets(storeSelector, sizeof(storeSelector), stdin);
	storeSelector[strcspn(storeSelector, "\n")] = '\0';

	printf("Enter the CSS selector for the store price: \n");
	fgets(priceSelector, sizeof(priceSelector), stdin);
	priceSelector[strcspn(priceSelector, "\n")] = '\0';

	printf("Enter the CSS selector for the store's item page: \n");
	fgets(storeItemsSelector, sizeof(storeItemsSelector), stdin);
	storeItemsSelector[strcspn(storeItemsSelector, "\n")] = '\0';

	printf("Enter the CSS selector for the store's info page: \n");
	fgets(storeInfoSelector, sizeof(storeInfoSelector), stdin);
	storeInfoSelector[strcspn(storeInfoSelector, "\n")] = '\0';

	printf("Enter the CSS selector for the store's desired contact info: \n");
	fgets(storeEmailSelector, sizeof(storeEmailSelector), stdin);
	storeEmailSelector[strcspn(storeEmailSelector, "\n")] = '\0';

	/* Input */

	/* Construct command and run JS Script */

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

	if (!fp)
	{
		perror("Failed to run Node Script. Make sure node.js is installed");
		return 1;
	}

	/* Construct command and run JS Script */

	/* Read JSON Output */

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

	writeToExcel(details);

	/* Write to Excel */

	/* Write to Excel */

	/* Save and Deallocate */

	saveExcel();

	free(details);
	free(jsonRaw);

	/* Save and Deallocate */

	return 0;
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