#include "clrfnc.h"
#include <stdio.h>

void red() { printf("\033[1;31m"); }
void green() { printf("\033[1;32m"); }
void yellow() { printf("\033[1;33m"); }
void blue() { printf("\033[1;34m"); }
void reset() { printf("\033[0m"); }