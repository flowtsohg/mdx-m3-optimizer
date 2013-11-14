#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <math.h>

#include <vector>
#include <set>

#if defined(_WIN32) || defined(_WIN64) 
#include "dirent2.h"
#define strncasecmp _strnicmp 
#else
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <utime.h>
#endif

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

void setFloatPrecision(void *data, uint32_t entries, uint32_t precision);
int endsWith(const char *str, const char *suffix);
char *fileExt(const char *fname, char *buff, uint32_t buffSize);
uint32_t fileExtPos(const char *fname);
char *insertSubstring(const char *src, const char *substr, uint32_t offset, char *buff, uint32_t buffSize);
char *insertToFilename(const char *src, const char *substr, char *buff, uint32_t buffSize);
int isDir(const char *fname);
float reverseFloat(float a);
int isBigEndian();

#endif