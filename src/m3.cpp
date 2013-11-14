#include "m3.h"

#define MD34_TAG 0x4d443334

#define REAL_TAG 0x5245414c
#define VEC2_TAG 0x56454332
#define VEC3_TAG 0x56454333
#define VEC4_TAG 0x56454334
#define QUAT_TAG 0x51554154
#define BNDS_TAG 0x424e4453
#define IREF_TAG 0x49524546
#define U8___TAG 0x55385f5f

typedef struct IndexEntry {
	uint32_t tag;
	uint32_t offset;
	uint32_t entries;
	uint32_t version;
} IndexEntry;

typedef struct Reference {
	uint32_t entries;
	uint32_t index;
	uint32_t flags;
} Reference;

typedef struct MD34 {
	uint32_t tag;
	uint32_t offset;
	uint32_t entries;
	Reference modelHeader;
} MD34;

typedef struct Chunk {
	uint32_t size;
	uint8_t *data;
} Chunk;

typedef struct M3File {
	MD34 header;
	IndexEntry *indexEntries;
	Chunk *chunks;
	Reference vertices;
	uint8_t uvsets;
} M3File;

int readM3File(const char *fname, M3File *fd) {
	uint32_t i;
	FILE *fp = fopen(fname, "rb");
	int ret = 0;

	if (fp) {
		fread(&fd->header, sizeof(MD34), 1, fp);

		if (fd->header.tag == MD34_TAG) {
			ret = 1;

			fd->indexEntries = (IndexEntry*)malloc(sizeof(IndexEntry) * fd->header.entries);
			fd->chunks = (Chunk*)malloc(sizeof(Chunk) * fd->header.entries);

			// Go to the index
			fseek(fp, fd->header.offset, SEEK_SET);

			// Read the index
			fread(fd->indexEntries, sizeof(IndexEntry), fd->header.entries, fp);
		
			// Get the chunk sizes
			for (i = 0; i < fd->header.entries - 1; i++) {
				fd->chunks[i].size = fd->indexEntries[i + 1].offset - fd->indexEntries[i].offset;
			}

			// The size of the last chunk is the offset in the header minus its own offset
			fd->chunks[fd->header.entries - 1].size = fd->header.offset - fd->indexEntries[fd->header.entries - 1].offset;

			// Go to the vertex information in the model header chunk
			fseek(fp, fd->indexEntries[fd->header.modelHeader.index].offset + 96, SEEK_SET);
		
			// Get the number of texture coordinate sets
			uint32_t vertexFlags;
		
			fread(&vertexFlags, 4, 1, fp);

			if (vertexFlags & 0x20000) {
				fd->uvsets = 1;
			} else if (vertexFlags & 0x40000) {
				fd->uvsets = 2;
			} else if (vertexFlags & 0x80000) {
				fd->uvsets = 3;
			} else if (vertexFlags & 0x100000) {
				fd->uvsets = 4;
			}

			// Get the vertices reference
			fread(&fd->vertices, sizeof(Reference), 1, fp);

			// Go back to the first chunk
			fseek(fp, 0, SEEK_SET);

			// Read all the chunks
			for (i = 0; i < fd->header.entries; i++) {
				fd->chunks[i].data = (uint8_t*)malloc(fd->chunks[i].size);
				fread(fd->chunks[i].data, 1, fd->chunks[i].size, fp);
			}
		} else {
			printf("Oops, %s is not a valid M3 file\n", fname);
		}

		fclose(fp);
	} else {
		printf("Oops, failed to open %s\n", fname);
	}

	return ret;
}

int writeM3File(const char *fname, M3File *fd, uint32_t bitmask) {
	uint32_t i;
	FILE *fp = fopen(fname, "wb");

	if (fp) {
		// Write the chunks
		for (i = 0; i < fd->header.entries; i++) {
			IndexEntry *entry = &fd->indexEntries[i];
			uint32_t tag = entry->tag;

			if (tag == REAL_TAG) {
				setFloatPrecision(fd->chunks[i].data, entry->entries, bitmask);
			} else if (tag == VEC2_TAG) {
				setFloatPrecision(fd->chunks[i].data, entry->entries * 2, bitmask);
			} else if (tag == VEC3_TAG) {
				setFloatPrecision(fd->chunks[i].data, entry->entries * 3, bitmask);
			} else if (tag == VEC4_TAG) {
				setFloatPrecision(fd->chunks[i].data, entry->entries * 4, bitmask);
			} else if (tag == QUAT_TAG) {
				setFloatPrecision(fd->chunks[i].data, entry->entries * 4, bitmask);
			} else if (tag == BNDS_TAG) {
				setFloatPrecision(fd->chunks[i].data, entry->entries * 7, bitmask);
			} else if (tag == IREF_TAG) {
				setFloatPrecision(fd->chunks[i].data, entry->entries * 16, bitmask);
			} else if (tag == U8___TAG && i == fd->vertices.index) {
				uint32_t vertexSize = 28 + 4 * fd->uvsets;
				uint32_t vertexCount = fd->chunks[i].size / vertexSize;
				uint32_t j, k;

				for (j = 0, k = 0; j < vertexCount; j++, k += vertexSize) {
					setFloatPrecision(&fd->chunks[i].data[k], 3, bitmask);
				}
			}

			fwrite(fd->chunks[i].data, fd->chunks[i].size, 1, fp);
		}

		// Write the index
		fwrite(fd->indexEntries, sizeof(IndexEntry), fd->header.entries, fp);

		fclose(fp);

		return 1;
	}

	printf("Oops, failed to open %s, are you sure you have write permissions in this location?\n", fname);

	return 0;
}

void handleM3File(const char *fin, const char *fout, uint32_t bitmask) {
	M3File fd;

	if (readM3File(fin, &fd)) {
		writeM3File(fout, &fd, bitmask);
	}
}