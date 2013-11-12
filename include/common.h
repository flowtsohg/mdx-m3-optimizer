#ifndef COMMON_H
#define COMMON_H

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <math.h>

#include <vector>
#include <set>

#ifdef _WIN32
#include "dirent.h"
#else
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <utime.h>
#endif

void floatPrecision(void *data, uint32_t entries, uint32_t precision);
int endswith(const char *str, const char *suffix);
char *fileext(const char *fname, char *buff, uint32_t buffSize);
uint32_t fileextpos(const char *fname);
char *insertsubstring(const char *src, const char *substr, uint32_t offset, char *buff, uint32_t buffSize);
char *inserttofilename(const char *src, const char *substr, char *buff, uint32_t buffSize);
int isdir(const char *fname);

#endif