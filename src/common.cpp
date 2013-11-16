#include "common.h"

void setFloatPrecision(void *data, uint32_t entries, uint32_t precision) {
  uint32_t i;
  uint32_t *fdata = (uint32_t*)data;

  for (i = 0; i < entries; i++) {
    fdata[i] &= precision;
  }
}

int endsWith(const char *str, const char *suffix) {
  size_t lenstr, lensuffix;

  if (!str || !suffix) {
    return 0;
  }

  lenstr = strlen(str);
  lensuffix = strlen(suffix);

  if (lensuffix >  lenstr) {
    return 0;
  }

  return strncasecmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

char *fileExt(const char *fname, char *buff, uint32_t buffSize) {
  const char *dot = strrchr(fname, '.');

  if (!dot) {
    return NULL;
  }

  uint32_t len = min(strlen(dot), buffSize);

  strncat(buff, dot, len);

  return buff;
}

uint32_t fileExtPos(const char *fname) {
  const char *dot = strrchr(fname, '.');

  if (!dot) {
    return 0;
  }

  return dot - fname;
}

char *insertSubstring(const char *src, const char *substr, uint32_t offset, char *buff, uint32_t buffSize) {
  if (buffSize < strlen(src) + strlen(substr)) {
    return NULL;
  }

  strncat(buff, src, offset);
  strcat(buff, substr);
  strcat(buff, src + offset);

  return buff;
}

char *insertToFilename(const char *src, const char *substr, char *buff, uint32_t buffSize) {
  uint32_t pos = fileExtPos(src);

  if (pos) {
    return insertSubstring(src, substr, pos, buff, buffSize);
  }

  return NULL;
}

int isDir(const char *fname) {
  struct stat st;

  if (stat(fname, &st)) {
    return 0;
  }

  if (S_ISDIR(st.st_mode)) {
    return 1;
  }

  return 0;
}

float reverseFloat(float a) {
   float b;
   char *c = (char*)&a;
   char *d = (char*)&b;

   d[0] = c[3];
   d[1] = c[2];
   d[2] = c[1];
   d[3] = c[0];

   return b;
}

int isBigEndian() {
    union {
        uint32_t i;
        char c[4];
    } bint = {0x01020304};

    return bint.c[0] == 1; 
}