#ifndef FILTRE_H
#define	FILTRE_H

#include "xc.h"

int32_t filtre1100(int16_t x);
int32_t filtre900(int16_t x);
int32_t stage1100(int32_t x, int i);
int32_t stage900(int32_t x, int i);

#endif
