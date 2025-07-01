#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib, "Shlwapi.lib")

#define WINDOW_HEIGHT 720
#define WINDOW_WIDTH 500

#define MAX_PRODUCTS 40
#define MAX_VARIATIONS 40
#define MAX_STORES 100
#define MAX_STRING 256
#define MAX_SELECTOR_SIZE 128
#define MAX_PATH 260

#define MAX_BUFFER (1024 * 1024 * 2) //2MB

FILE *fp;

char script[MAX_PATH];

char url[MAX_SELECTOR_SIZE];
char linkSelector[MAX_SELECTOR_SIZE];
char variationSelector[MAX_SELECTOR_SIZE];
char titleSelector[MAX_SELECTOR_SIZE];
char storeAreaSelector[MAX_SELECTOR_SIZE];
char storeSelector[MAX_SELECTOR_SIZE];
char priceSelector[MAX_SELECTOR_SIZE];
char storeItemsSelector[MAX_SELECTOR_SIZE];
char storeInfoSelector[MAX_SELECTOR_SIZE];
char storeEmailSelector[MAX_SELECTOR_SIZE];

char command[1024];
wchar_t jsonOutput[MAX_BUFFER];

//int rows = 2;

typedef struct StoreInfo
{
	wchar_t storeName[MAX_STRING];
	wchar_t price[MAX_STRING];
	wchar_t link[MAX_STRING];
	wchar_t contactInfo[MAX_STRING];

}StoreInfo;

typedef struct Variation
{
	wchar_t variationTitle[MAX_STRING];
	StoreInfo stores[MAX_STORES];
	int storeCount;

}Variation;

typedef struct Product
{
	wchar_t productUrl[MAX_STRING];
	Variation variations[MAX_VARIATIONS];
	int variationCount;

}Product;

typedef struct itemDetails
{
	Product products[MAX_PRODUCTS];
	int productCount;

}itemDetails;

typedef enum {
	SELECTOR_LINK		 = 0,
	SELECTOR_VARIATION	 = 1,
	SELECTOR_TITLE		 = 2,
	SELECTOR_AREA		 = 3,
	SELECTOR_STORE		 = 4,
	SELECTOR_PRICE		 = 5,
	SELECTOR_STORE_ITEMS = 6,
	SELECTOR_STORE_INFO  = 7,
	SELECTOR_STORE_EMAIL = 8
}Selectors;

#endif // !GLOBALS_H

