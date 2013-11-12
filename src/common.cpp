// Copyright (C) Chananya Freiman (aka GhostWolf)

#include "common.h"

void floatPrecision(void *data, uint32_t entries, uint32_t precision) {
	uint32_t i;
	uint32_t *fdata = (uint32_t*)data;

	for (i = 0; i < entries; i++) {
		fdata[i] &= precision;
	}
}

int endswith(const char *str, const char *suffix) {
	 size_t lenstr, lensuffix;

    if (!str || !suffix) {
        return 0;
	}

    lenstr = strlen(str);
    lensuffix = strlen(suffix);
    
	if (lensuffix >  lenstr) {
        return 0;
	}

    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

char *fileext(const char *fname, char *buff, uint32_t buffSize) {
	const char *dot = strrchr(fname, '.');

	if (!dot) {
		return NULL;
	}

	uint32_t len = min(strlen(dot), buffSize);

	strncat(buff, dot, len);

	return buff;
}

uint32_t fileextpos(const char *fname) {
	const char *dot = strrchr(fname, '.');

	if (!dot) {
		return 0;
	}

	return dot - fname;
}

char *insertsubstring(const char *src, const char *substr, uint32_t offset, char *buff, uint32_t buffSize) {
	if (buffSize < strlen(src) + strlen(substr)) {
		return NULL;
	}

	strncat(buff, src, offset);
	strcat(buff, substr);
	strcat(buff, src + offset);

	return buff;
}

char *inserttofilename(const char *src, const char *substr, char *buff, uint32_t buffSize) {
	uint32_t pos = fileextpos(src);

	if (pos) {
		return insertsubstring(src, substr, pos, buff, buffSize);
	}

	return NULL;
}

int isdir(const char *fname) {
	struct stat st;

	if (stat(fname, &st)) {
		return 0;
	}

	if (S_ISDIR(st.st_mode)) {
		return 1;
	}

	return 0;
}
