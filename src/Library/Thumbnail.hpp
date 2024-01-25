#ifndef THUMBNAIL_HPP
#define THUMBNAIL_HPP 1

#include <cstdint>
#include <cstddef>

#include "qoi.h"

struct Thumbnail;
struct ThumbnailData;

typedef bool (*ThumbnailProcessCb)(const struct Thumbnail &thumbnail, uint32_t pixels_offset, const qoi_rgba_t *pixels, size_t pixels_count);

struct Thumbnail
{
	uint16_t width;
	uint16_t height;

	uint32_t pixel_count;

	enum ImageFormat {
		Invalid = 0,
		Qoi,
	} imageFormat;

	qoi_desc qoi;

	bool IsValid();

	int Init();
	int DecodeChunk(struct ThumbnailData &data, ThumbnailProcessCb callback);
};

struct ThumbnailData
{
	uint16_t size;
	unsigned char buffer[1024];

	bool IsValid();
};

#endif /* ifndef THUMBNAIL_HPP */
