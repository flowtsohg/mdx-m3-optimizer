// Copyright (C) Chananya Freiman (aka GhostWolf)

#include "mdx.h"

/*
void printTag(uint32_t tag) {
	unsigned char bytes[4];
	unsigned long n = tag;
	bytes[0] = (n >> 24) & 0xFF;
	bytes[1] = (n >> 16) & 0xFF;
	bytes[2] = (n >> 8) & 0xFF;
	bytes[3] = n & 0xFF;
	printf("%c%c%c%c", bytes[3], bytes[2], bytes[1], bytes[0]);
}
*/

#define MDLX_TAG 0x584c444d
#define GEOS_TAG 0x534f4547
#define SEQS_TAG 0x53514553
#define LAYS_TAG 0x5359414c
#define TXAN_TAG 0x4e415854
#define GEOA_TAG 0x414f4547
#define LITE_TAG 0x4554494c
#define ATCH_TAG 0x48435441
#define PREM_TAG 0x4d455250
#define PRE2_TAG 0x32455250
#define RIBB_TAG 0x42424952
#define CAMS_TAG 0x534d4143
#define MTLS_TAG 0x534c544d
#define BONE_TAG 0x454e4f42
#define HELP_TAG 0x504c4548
#define PIVT_TAG 0x54564950

#define KMTF_TAG 0x46544d4b
#define KMTA_TAG 0x41544d4b
#define KTAT_TAG 0x5441544b
#define KTAR_TAG 0x5241544b
#define KTAS_TAG 0x5341544b
#define KGAO_TAG 0x4f41474b
#define KGAC_TAG 0x4341474b
#define KLAS_TAG 0x53414c4b
#define KLAE_TAG 0x45414c4b
#define KLAC_TAG 0x43414c4b
#define KLAI_TAG 0x49414c4b
#define KLBI_TAG 0x49424c4b
#define KLBC_TAG 0x43424c4b
#define KLAV_TAG 0x56414c4b
#define KATV_TAG 0x5654414b
#define KPEE_TAG 0x4545504b
#define KPEG_TAG 0x4745504b
#define KPLN_TAG 0x4e4c504b
#define KPLT_TAG 0x544c504b
#define KPEL_TAG 0x4c45504b
#define KPES_TAG 0x5345504b
#define KPEV_TAG 0x5645504b
#define KP2S_TAG 0x5332504b
#define KP2R_TAG 0x5232504b
#define KP2L_TAG 0x4c32504b
#define KP2G_TAG 0x4732504b
#define KP2E_TAG 0x4532504b
#define KP2N_TAG 0x4e32504b
#define KP2W_TAG 0x5732504b
#define KP2V_TAG 0x5632504b
#define KRHA_TAG 0x4148524b
#define KRHB_TAG 0x4248524b
#define KRAL_TAG 0x4c41524b
#define KRCO_TAG 0x4f43524b
#define KRTX_TAG 0x5854524b
#define KRVS_TAG 0x5356524b
#define KCTR_TAG 0x5254434b
#define KTTR_TAG 0x5254544b
#define KCRL_TAG 0x4c52434b
#define KGTR_TAG 0x5254474b
#define KGRT_TAG 0x5452474b
#define KGSC_TAG 0x4353474b

bool compareArrays(const std::vector<float> &a, const std::vector<float> &b, float threshold) {
	for (uint32_t i = 0; i < a.size(); i++) {
		float c = a[i];
		float d = b[i];
		float diff = c - d;

		if (diff < -threshold || diff > threshold) {
			return false;
		}
	}

	return true;
}

template<class T>
T readValue(FILE *fp) {
	T x;

	if (!fread(&x, sizeof(T), 1, fp)) {
		return T(0);
	}

	return x;
}

template<class T>
void writeValue(FILE *fp, T x) {
	fwrite(&x, sizeof(T), 1, fp);
}

template<class T>
std::vector<T> readArray(FILE *fp, uint32_t count) {
	std::vector<T> x;

	x.resize(count);

	if (!fread(&x[0], sizeof(T), count, fp)) {
		return x;
	}

	return x;
}

template<class T>
void writeArray(FILE *fp, const std::vector<T> &x) {
	fwrite(&x[0], sizeof(T), x.size(), fp);
}

struct Entry {
	uint32_t inclusiveSize;

	Entry() {}
	virtual ~Entry() {}

	virtual void write(FILE *fp) {};
	virtual uint32_t optimize(const std::set<int32_t> &edges, int forceLinear, float threshold) { return 0; };
};

void writeEntries(FILE *fp, std::vector<Entry*> entries) {
	for (uint32_t i = 0; i < entries.size(); i++) {
		entries[i]->write(fp);
	}
}

uint32_t optimizeEntries(std::vector<Entry*> entries, const std::set<int32_t> &edges, int forceLinear, float threshold) {
		uint32_t reducedSize = 0;

		for (uint32_t i = 0; i < entries.size(); i++) {
			reducedSize += entries[i]->optimize(edges, forceLinear, threshold);
		}

		return reducedSize;
	}

struct Track : Entry {
	int32_t key;
	std::vector<float> data;

	Track() {}

	Track(FILE *fp, uint32_t elements, uint32_t bitmask) {
		key = readValue<int32_t>(fp);
		data = readArray<float>(fp, elements);

		floatPrecision(&data[0], data.size(), bitmask);
	}

	void write(FILE *fp) {
		writeValue<int32_t>(fp, key);
		writeArray<float>(fp, data);
	}
};

struct TrackSet : Entry {
	uint32_t tag;
	uint32_t count;
	uint32_t interpolationType;
	uint32_t u1;
	std::vector<Entry*> tracks;
	
	uint32_t elements;
	uint32_t trackSize;

	TrackSet() {}

	TrackSet(FILE *fp, uint32_t bitmask) {
		tag = readValue<uint32_t>(fp);
		count = readValue<uint32_t>(fp);
		interpolationType = readValue<uint32_t>(fp);
		u1 = readValue<uint32_t>(fp);

		elements = 0;
		
		if (tag == KTAR_TAG || tag == KGRT_TAG) {
			elements = 4;
		} else if (tag == KTAT_TAG || tag == KTAS_TAG || tag == KGAC_TAG || tag == KLAC_TAG || tag == KLBC_TAG || tag == KRCO_TAG || tag == KCTR_TAG || tag == KTTR_TAG || tag == KGTR_TAG || tag == KGSC_TAG) {
			elements = 3;
		} else {
			elements = 1;
		}

		if (interpolationType > 1) {
			elements *= 3;
		}

		for (uint32_t i = 0; i < count; i++) {
			if (tag == KMTF_TAG || tag == KRTX_TAG || tag == KCRL_TAG) {
				tracks.push_back(new Track(fp, elements, 0xffffffff));
			} else {
				tracks.push_back(new Track(fp, elements, bitmask));
			}
		}

		trackSize = 4 + elements * 4;
		inclusiveSize = 16 + (count * trackSize);
	}

	void write(FILE *fp) {
		writeValue<uint32_t>(fp, tag);
		writeValue<uint32_t>(fp, count);
		writeValue<uint32_t>(fp, interpolationType);
		writeValue<uint32_t>(fp, u1);
		writeEntries(fp, tracks);
	}

	uint32_t optimize(const std::set<int32_t> &edges, int forceLinear, float threshold) {
		uint32_t reducedSize = 0;

		if (forceLinear && interpolationType > 1) {
			interpolationType = 1;

			reducedSize += (elements * 8 * count);

			for (uint32_t i = 0; i < tracks.size(); i++) {
				Track *track = static_cast<Track*>(tracks[i]);

				track->data.resize(elements / 3);
			}
		}

		if (tracks.size() > 2) {
			std::vector<Entry*> finalTracks;

			finalTracks.push_back(tracks[0]);

			for (uint32_t i = 1; i < tracks.size() - 1; i++) {
				Track *a = static_cast<Track*>(tracks[i - 1]);
				Track *b = static_cast<Track*>(tracks[i]);
				Track *c = static_cast<Track*>(tracks[i + 1]);

				if (edges.count(b->key) || !compareArrays(a->data, b->data, threshold) || !compareArrays(b->data, c->data, threshold)) {
					finalTracks.push_back(b);
				} else {
					reducedSize += trackSize;
				}
			}

			finalTracks.push_back(tracks[tracks.size() - 1]);

			count = finalTracks.size();
			tracks = finalTracks;
		}

		return reducedSize;
	}
};

std::vector<Entry*> parseTrackSet(FILE *fp, uint32_t size, uint32_t bitmask) {
	std::vector<Entry*> entries;
	uint32_t totalInclusiveSize = 0;

	while (totalInclusiveSize < size) {
		TrackSet *entry = new TrackSet(fp, bitmask);

		totalInclusiveSize += entry->inclusiveSize;

		entries.push_back(entry);
	}

	return entries;
}

struct TrackChunk : Entry {
	std::vector<Entry*> tracksets;

	TrackChunk() {}

	TrackChunk(FILE *fp, uint32_t size, uint32_t bitmask) {
		tracksets = parseTrackSet(fp, size, bitmask);
	}

	void write(FILE *fp) {
		writeEntries(fp, tracksets);
	}

	uint32_t optimize(const std::set<int32_t> &edges, int forceLinear, float threshold) {
		return optimizeEntries(tracksets, edges, forceLinear, threshold);
	}
};

struct Sequence : Entry {
	std::vector<uint8_t> u1;
	uint32_t intervalStart;
	uint32_t intervalEnd;
	std::vector<uint8_t> u2;

	Sequence(FILE *fp, uint32_t bitmask) {
		u1 = readArray<uint8_t>(fp, 80);
		intervalStart = readValue<uint32_t>(fp);
		intervalEnd = readValue<uint32_t>(fp);
		u2 = readArray<uint8_t>(fp, 44);
	}

	void write(FILE *fp) {
		writeArray<uint8_t>(fp, u1);
		writeValue<uint32_t>(fp, intervalStart);
		writeValue<uint32_t>(fp, intervalEnd);
		writeArray<uint8_t>(fp, u2);
	}
};

struct GeosetAnimationCameraNodeLayerTextureAnimation : Entry {
	std::vector<uint8_t> u1;
	TrackChunk tracks;

	GeosetAnimationCameraNodeLayerTextureAnimation() {}

	GeosetAnimationCameraNodeLayerTextureAnimation(FILE *fp, uint32_t bitmask, uint32_t u1size) {
		inclusiveSize = readValue<uint32_t>(fp);
		u1 = readArray<uint8_t>(fp, u1size);
		tracks = TrackChunk(fp, inclusiveSize - 4 - u1size, bitmask);
	}

	void write(FILE *fp) {
		writeValue<uint32_t>(fp, inclusiveSize);
		writeArray<uint8_t>(fp, u1);
		tracks.write(fp);
	}

	uint32_t optimize(const std::set<int32_t> &edges, int forceLinear, float threshold) {
		uint32_t reducedSize = tracks.optimize(edges, forceLinear, threshold);

		inclusiveSize -= reducedSize;

		return reducedSize;
	}
};

struct Bone : Entry {
	GeosetAnimationCameraNodeLayerTextureAnimation node;
	std::vector<uint8_t> u1;

	Bone(FILE *fp, uint32_t bitmask) {
		node = GeosetAnimationCameraNodeLayerTextureAnimation(fp, bitmask, 92);
		u1 = readArray<uint8_t>(fp, 8);
		inclusiveSize = node.inclusiveSize + 8;
	}

	void write(FILE *fp) {
		node.write(fp);
		writeArray<uint8_t>(fp, u1);
	}

	uint32_t optimize(const std::set<int32_t> &edges, int forceLinear, float threshold) {
		uint32_t reducedSize = node.optimize(edges, forceLinear, threshold);

		inclusiveSize -= reducedSize;

		return reducedSize;
	}
};

struct LightAttachmentParticleEmitterParticleEmitter2RibbonEmitter : Entry {
	GeosetAnimationCameraNodeLayerTextureAnimation node;
	std::vector<uint8_t> u1;
	TrackChunk tracks;
	

	LightAttachmentParticleEmitterParticleEmitter2RibbonEmitter(FILE *fp, uint32_t bitmask, uint32_t u1size) {
		inclusiveSize = readValue<uint32_t>(fp);
		node = GeosetAnimationCameraNodeLayerTextureAnimation(fp, bitmask, 92);
		u1 = readArray<uint8_t>(fp, u1size);
		tracks = TrackChunk(fp, inclusiveSize - 4 - u1size - node.inclusiveSize, bitmask);
	}

	void write(FILE *fp) {
		writeValue<uint32_t>(fp, inclusiveSize);
		node.write(fp);
		writeArray<uint8_t>(fp, u1);
		tracks.write(fp);
	}

	uint32_t optimize(const std::set<int32_t> &edges, int forceLinear, float threshold) {
		uint32_t reducedSize = node.optimize(edges, forceLinear, threshold) + tracks.optimize(edges, forceLinear, threshold);

		inclusiveSize -= reducedSize;

		return reducedSize;
	}
};

struct Material : Entry {
	std::vector<uint8_t> u1;
	uint32_t u2;
	uint32_t count;
	std::vector<Entry*> layers;

	Material(FILE *fp, uint32_t bitmask) {
		inclusiveSize = readValue<uint32_t>(fp);
		u1 = readArray<uint8_t>(fp, 8);
		
		if (inclusiveSize > 12) {
			u2 = readValue<uint32_t>(fp);
			count = readValue<uint32_t>(fp);

			for (uint32_t i = 0; i < count; i++) {
				layers.push_back(new GeosetAnimationCameraNodeLayerTextureAnimation(fp, bitmask, 24));
			}
		}
	}

	void write(FILE *fp) {
		writeValue<uint32_t>(fp, inclusiveSize);
		writeArray<uint8_t>(fp, u1);
		
		if (inclusiveSize > 12) {
			writeValue<uint32_t>(fp, u2);
			writeValue<uint32_t>(fp, count);
			writeEntries(fp, layers);
		}
	}

	uint32_t optimize(const std::set<int32_t> &edges, int forceLinear, float threshold) {
		uint32_t reducedSize = optimizeEntries(layers, edges, forceLinear, threshold);

		inclusiveSize -= reducedSize;

		return reducedSize;
	}
};

struct UVSet : Entry {
	uint32_t tag;
	uint32_t count;
	std::vector<float> coordinates;

	UVSet() {}

	UVSet(FILE *fp, uint32_t bitmask) {
		tag = readValue<uint32_t>(fp);
		count = readValue<uint32_t>(fp);
		coordinates = readArray<float>(fp, count * 2);

		floatPrecision(&coordinates[0], coordinates.size(), bitmask);
	}

	void write(FILE *fp) {
		writeValue<uint32_t>(fp, tag);
		writeValue<uint32_t>(fp, count);
		writeArray<float>(fp, coordinates);
	}
};

struct Geoset : Entry {
	uint32_t u1;
	uint32_t positionsCount;
	std::vector<float> positions;
	uint32_t u2;
	uint32_t normalsCount;
	std::vector<float> normals;
	uint32_t u3;
	uint32_t u4;
	std::vector<uint8_t> u5;
	uint32_t u6;
	uint32_t u7;
	std::vector<uint8_t> u8;
	uint32_t u9;
	uint32_t u10;
	std::vector<uint8_t> u11;
	uint32_t u12;
	uint32_t u13;
	std::vector<uint8_t> u14;
	uint32_t u15;
	uint32_t u16;
	std::vector<uint8_t> u17;
	uint32_t u18;
	uint32_t u19;
	std::vector<uint8_t> u20;
	std::vector<uint8_t> u21;
	uint32_t u22;
	std::vector<uint8_t> u23;
	uint32_t u24;
	uint32_t uvsetCount;
	std::vector<Entry*> uvsets;

	Geoset(FILE *fp, uint32_t bitmask) {
		inclusiveSize = readValue<uint32_t>(fp);
		u1 = readValue<uint32_t>(fp);
		positionsCount = readValue<uint32_t>(fp);
		positions = readArray<float>(fp, positionsCount * 3);
		
		floatPrecision(&positions[0], positions.size(), bitmask);
		
		u2 = readValue<uint32_t>(fp);
		normalsCount = readValue<uint32_t>(fp);
		normals = readArray<float>(fp, normalsCount * 3);

		floatPrecision(&normals[0], normals.size(), bitmask);

		u3 = readValue<uint32_t>(fp);
		u4 = readValue<uint32_t>(fp);
		u5 = readArray<uint8_t>(fp, u4 * 4);
		u6 = readValue<uint32_t>(fp);
		u7 = readValue<uint32_t>(fp);
		u8 = readArray<uint8_t>(fp, u7 * 4);
		u9 = readValue<uint32_t>(fp);
		u10 = readValue<uint32_t>(fp);
		u11 = readArray<uint8_t>(fp, u10 * 2);
		u12 = readValue<uint32_t>(fp);
		u13 = readValue<uint32_t>(fp);
		u14 = readArray<uint8_t>(fp, u13);
		u15 = readValue<uint32_t>(fp);
		u16 = readValue<uint32_t>(fp);
		u17 = readArray<uint8_t>(fp, u16 * 4);
		u18 = readValue<uint32_t>(fp);
		u19 = readValue<uint32_t>(fp);
		u20 = readArray<uint8_t>(fp, u19 * 4);
		u21 = readArray<uint8_t>(fp, 40);
		u22 = readValue<uint32_t>(fp);
		u23 = readArray<uint8_t>(fp, u22 * 28);
		u24 = readValue<uint32_t>(fp);
		uvsetCount = readValue<uint32_t>(fp);

		for (uint32_t i = 0; i < uvsetCount; i++) {
			uvsets.push_back(new UVSet(fp, bitmask));
		}
	}

	void write (FILE *fp) {
		writeValue<uint32_t>(fp, inclusiveSize);
		writeValue<uint32_t>(fp, u1);
		writeValue<uint32_t>(fp, positionsCount);
		writeArray<float>(fp, positions);
		writeValue<uint32_t>(fp, u2);
		writeValue<uint32_t>(fp, normalsCount);
		writeArray<float>(fp, normals);
		writeValue<uint32_t>(fp, u3);
		writeValue<uint32_t>(fp, u4);
		writeArray<uint8_t>(fp, u5);
		writeValue<uint32_t>(fp, u6);
		writeValue<uint32_t>(fp, u7);
		writeArray<uint8_t>(fp, u8);
		writeValue<uint32_t>(fp, u9);
		writeValue<uint32_t>(fp, u10);
		writeArray<uint8_t>(fp, u11);
		writeValue<uint32_t>(fp, u12);
		writeValue<uint32_t>(fp, u13);
		writeArray<uint8_t>(fp, u14);
		writeValue<uint32_t>(fp, u15);
		writeValue<uint32_t>(fp, u16);
		writeArray<uint8_t>(fp, u17);
		writeValue<uint32_t>(fp, u18);
		writeValue<uint32_t>(fp, u19);
		writeArray<uint8_t>(fp, u20);
		writeArray<uint8_t>(fp, u21);
		writeValue<uint32_t>(fp, u22);
		writeArray<uint8_t>(fp, u23);
		writeValue<uint32_t>(fp, u24);
		writeValue<uint32_t>(fp, uvsetCount);
		writeEntries(fp, uvsets);
	}

	uint32_t optimize(const std::set<int32_t> &edges, int forceLinear, float threshold) {
		return 0;
	}
};

struct Chunk {
	uint32_t tag;
	uint32_t size;

	Chunk(uint32_t t, uint32_t s) : tag(t),size(s) {}

	virtual void write(FILE *fp) {
		writeValue<uint32_t>(fp, tag);
		writeValue<uint32_t>(fp, size);
	}

	virtual uint32_t optimize(const std::set<int32_t> &edges, int forceLinear, float threshold) { return 0; }
};

struct UnusedChunk : Chunk {
	std::vector<uint8_t> data;

	UnusedChunk(uint32_t t, uint32_t s, FILE *fp) : Chunk(t,s) {
		data = readArray<uint8_t>(fp, size);
	}

	void write(FILE *fp) {
		Chunk::write(fp);
		writeArray<uint8_t>(fp, data);

		//printTag(header.tag);
		//printf("\t%d\t%d\t%d\n", data.size(), data.size(), 0);
	}

	uint32_t optimize(const std::set<int32_t> &edges, int forceLinear, float threshold) {
		return 0;
	}
};

std::vector<Entry*> parseEntryChunk(FILE *fp, uint32_t tag, uint32_t size, uint32_t bitmask) {
	std::vector<Entry*> entries;
	uint32_t totalInclusiveSize = 0;

	while (totalInclusiveSize < size) {
		Entry *entry;

		if (tag == MTLS_TAG) {
			entry = new Material(fp, bitmask);
		} else if (tag == TXAN_TAG) {
			entry = new GeosetAnimationCameraNodeLayerTextureAnimation(fp, bitmask, 0);
		} else if (tag == GEOS_TAG) {
			entry = new Geoset(fp, bitmask);
		} else if (tag == GEOA_TAG) {
			entry = new GeosetAnimationCameraNodeLayerTextureAnimation(fp, bitmask, 24);
		} else if (tag == BONE_TAG) {
			entry = new Bone(fp, bitmask);
		} else if (tag == LITE_TAG) {
			entry = new LightAttachmentParticleEmitterParticleEmitter2RibbonEmitter(fp, bitmask, 44);
		} else if (tag == ATCH_TAG) {
			entry = new LightAttachmentParticleEmitterParticleEmitter2RibbonEmitter(fp, bitmask, 264);
		} else if (tag == HELP_TAG) {
			entry = new GeosetAnimationCameraNodeLayerTextureAnimation(fp, bitmask, 92);
		} else if (tag == PREM_TAG) {
			entry = new LightAttachmentParticleEmitterParticleEmitter2RibbonEmitter(fp, bitmask, 284);
		} else if (tag == PRE2_TAG) {
			entry = new LightAttachmentParticleEmitterParticleEmitter2RibbonEmitter(fp, bitmask, 171);
		} else if (tag == RIBB_TAG) {
			entry = new LightAttachmentParticleEmitterParticleEmitter2RibbonEmitter(fp, bitmask, 52);
		} else if (tag == CAMS_TAG) {
			entry = new GeosetAnimationCameraNodeLayerTextureAnimation(fp, bitmask, 116);
		}

		totalInclusiveSize += entry->inclusiveSize;

		entries.push_back(entry);
	}

	return entries;
}

struct WrapperChunk : Chunk {
	std::vector<Entry*> entries;
	uint32_t origSize;

	WrapperChunk(uint32_t tag, uint32_t size, FILE *fp, uint32_t bitmask) : Chunk(tag, size) {
		entries = parseEntryChunk(fp, tag, size, bitmask);
		origSize = size;
	}

	void write(FILE *fp) {
		Chunk::write(fp);
		writeEntries(fp, entries);

		//printTag(header.tag);
		//printf("\t%d\t%d\t%d\n", origSize, header.size, origSize - header.size);
	}

	uint32_t optimize(const std::set<int32_t> &edges, int forceLinear, float threshold) {
		uint32_t reducedSize = optimizeEntries(entries, edges, forceLinear, threshold);

		size -= reducedSize;

		return reducedSize;
	}
};

struct PivotPointChunk : Chunk {
	std::vector<float> points;

	PivotPointChunk(uint32_t tag, uint32_t size, FILE *fp, uint32_t bitmask) : Chunk(tag,size) {
		points = readArray<float>(fp, size / 4);

		floatPrecision(&points[0], points.size(), bitmask);
	}

	void write(FILE *fp) {
		Chunk::write(fp);
		writeArray<float>(fp, points);

		//printf("PIVT\t%d\t%d\t%d\n", header.size, header.size, 0);
	}

	uint32_t optimize(const std::set<int32_t> &edges, int forceLinear, float threshold) {
		return 0;
	}
};

struct SequenceChunk : Chunk {
	std::vector<Entry*> entries;

	SequenceChunk(uint32_t tag, uint32_t size, FILE *fp, uint32_t bitmask) : Chunk(tag,size) {
		for (uint32_t i = 0; i < size / 132; i++) {
			entries.push_back(new Sequence(fp, bitmask));
		}
	}

	void write(FILE *fp) {
		Chunk::write(fp);
		writeEntries(fp, entries);

		//printf("SEQS\t%d\t%d\t%d\n", header.size, header.size, 0);
	}

	uint32_t optimize(const std::set<int32_t> &edges, int forceLinear, float threshold) {
		return 0;
	}
};

struct MDXFile {
	std::vector<Chunk*> chunks;
	SequenceChunk* seqs;
};

int readMDXFile(const char *fname, MDXFile *fd, uint32_t bitmask) {
	FILE *fp = fopen(fname, "rb");
	uint32_t tag;
	uint32_t size;
	int ret = 0;

	if (fp) {
		fread(&tag, 4, 1, fp);
		
		if (tag == MDLX_TAG) {
			ret = 1;

			// Read the chunks with their headers
			while (fread(&tag, 4, 1, fp)) {
				fread(&size, 4, 1, fp);

				if (tag == MTLS_TAG || tag == TXAN_TAG || tag == GEOS_TAG || tag == GEOA_TAG || tag == BONE_TAG || tag == LITE_TAG || tag == ATCH_TAG || tag == HELP_TAG || tag == PREM_TAG || tag == PRE2_TAG || tag == RIBB_TAG || tag == CAMS_TAG) {
					fd->chunks.push_back(new WrapperChunk(tag, size, fp, bitmask));
				} else if (tag == SEQS_TAG) {
					SequenceChunk* seqs = new SequenceChunk(tag, size, fp, bitmask);
				
					fd->chunks.push_back(seqs);
					fd->seqs = seqs;
				} else if (tag == PIVT_TAG) {
					fd->chunks.push_back(new PivotPointChunk(tag, size, fp, bitmask));
				} else {
					fd->chunks.push_back(new UnusedChunk(tag, size, fp));
				}
			}
		} else {
      printf("Oops, %s is not a valid MDX file\n", fin);
    }

		fclose(fp);
	} else {
    printf("Oops, failed to open %s\n", fin);
  }

	return ret;
}

int optimizeMDXFile(MDXFile *fd, uint8_t forceLinear, float threshold) {
	std::set<int32_t> edges;

	for (uint32_t i = 0; i < fd->seqs->entries.size(); i++) {
		Sequence *seq = static_cast<Sequence*>(fd->seqs->entries[i]);

		edges.insert(seq->intervalStart);
		edges.insert(seq->intervalEnd);
	}

	for (uint32_t i = 0; i < fd->chunks.size(); i++) {
		fd->chunks[i]->optimize(edges, forceLinear, threshold);
	}

	return 1;
}

int writeMDXFile(const char *fname, MDXFile *fd) {
	uint32_t i;
	FILE *fp = fopen(fname, "wb");

	if (fp) {
		fwrite("MDLX", 1, 4, fp);

		for (i = 0; i < fd->chunks.size(); i++) {
			fd->chunks[i]->write(fp);
		}

		fclose(fp);

		return 1;
	} else {
    printf("Oops, failed to open %s, are you sure you have permissions in this location?\n", fname);
  }

	printf("Failed to open %s\n", fname);

	return 0;
}

void handleMDXFile(const char *fin, const char *fout, uint32_t bitmask, uint8_t forceLinear, float threshold) {
	MDXFile fd;

	if (readMDXFile(fin, &fd, bitmask)) {
		optimizeMDXFile(&fd, forceLinear, threshold);
		writeMDXFile(fout, &fd);
	}
}