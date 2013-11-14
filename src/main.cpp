#include "common.h"
#include "mdx.h"
#include "m3.h"

int fileFormat(const char *fname) {
	if (endsWith(fname, ".mdx")) {
		return 1;
	} else if (endsWith(fname, ".m3")) {
		return 2;
	}

	return 0;
}

int handleFile(const char *fname, uint32_t bitmask, uint8_t forceLinear, float threshold) {
	int format = fileFormat(fname);

	if (format) {
		char buffer[MAX_PATH + 1];
		
		memset(buffer, 0, MAX_PATH + 1);
		insertToFilename(fname, "_v2", buffer, MAX_PATH);

		printf("%s -> %s\n", fname, buffer);

		if (format == 1) {
			handleMDXFile(fname, buffer, bitmask, forceLinear, threshold);
		} else {
			handleM3File(fname, buffer, bitmask);
		}
	}
	
	return 1;
}

void handleDir(const char *fname, uint32_t bitmask, uint8_t forceLinear, float threshold) {
	DIR *d = opendir(fname);
	dirent *entry;
	char path[MAX_PATH];

	while ((entry = readdir(d))) {
		memset(path, 0, MAX_PATH);
		strcat(path, fname);
		strcat(path, "/");
		strcat(path, entry->d_name);

		if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
			if (isDir(path)) {
				handleDir(path, bitmask, forceLinear, threshold);
			} else {
				handleFile(path, bitmask, forceLinear, threshold);
			}
		}
	}
}

int main(int argc, char *argv[]) {
	int32_t i;
	uint32_t bitmask;

	if (isBigEndian()) {
		bitmask = 0x0000ffff;
	} else {
		bitmask = 0xffff0000;
	}

	uint8_t forceLinear = 0;
	float threshold = 0.001f;

	if (argc > 1) {
		for (i = 1; i < argc; i++) {
			if (strcmp(argv[i], "-p") == 0) {
				int32_t precision = atol(argv[++i]);
				precision = 32 - min(max(precision, 10), 32);

				if (isBigEndian()) {
					bitmask = 0xffffffff >> precision;
				} else {
					bitmask = 0xffffffff << precision;
				}
			} else if (strcmp(argv[i], "-l") == 0) {
				forceLinear = 1;
			} else if (strcmp(argv[i], "-t") == 0) {
				threshold = (float)atof(argv[++i]);
			} else if (strcmp(argv[i], "-v") == 0) {
				printf("MDX/M3 Optimizer version 1.3\nCopyright (c) 2013 Chananya Freiman (aka GhostWolf)");
			} else {
				if (isDir(argv[i])) {
					handleDir(argv[i], bitmask, forceLinear, threshold);
				} else {
					handleFile(argv[i], bitmask, forceLinear, threshold);
				}
			}
		}
	} else {
		printf("Usage: copt [OPTIONS] [INPUT1 [INPUT2 [...]]]\n"
			   "\n"
			   "INPUT[x]\n"
			   "\tPath to a MDX/M3 file or a directory\n"
		       "OPTIONS\n"
		       "\t-p PRECISION\tFloating point precision (default: 16)\n"
		       "\t-t THRESHOLD\tKeyframe threshold (default: 0.001)\n"
		       "\t-l\t\tForce linear keyframes (default: false)\n"
		       "\t-v\t\tShows the version\n\n");
	}

	return 0;
}