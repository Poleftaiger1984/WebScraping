#ifndef BOOKKEEPING_H
#define BOOKKEEPING_H

#include "globals.h"
#include <stdbool.h>

int initExcel();
void writeToExcel(itemDetails* details);
bool saveExcel();

#endif // !BOOKKEEPING_H
