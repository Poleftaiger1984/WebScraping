#ifndef BOOKKEEPING_H
#define BOOKKEEPING_H

#include "globals.h"

int initExcel();
void writeToExcel(itemDetails* details);
void saveExcel();

#endif // !BOOKKEEPING_H
