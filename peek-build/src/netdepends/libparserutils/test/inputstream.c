#include <inttypes.h>
#include <stdio.h>

#include <parserutils/parserutils.h>
#include <parserutils/charset/utf8.h>
#include <parserutils/input/inputstream.h>

#include "utils/utils.h"

#include "testutils.h"

#ifdef __riscos
const char * const __dynamic_da_name = "InputStream";
int __dynamic_da_max_size = 128*1024*1024;
#endif

static void *myrealloc(void *ptr, size_t len, void *pw)
{
	UNUSED(pw);

	return realloc(ptr, len);
}

int main(int argc, char **argv)
{
	parserutils_inputstream *stream;
	FILE *fp;
	size_t len, origlen;
#define CHUNK_SIZE (4096)
	uint8_t buf[CHUNK_SIZE];
	const uint8_t *c;
	size_t clen;

	if (argc != 3) {
		printf("Usage: %s <aliases_file> <filename>\n", argv[0]);
		return 1;
	}

	/* Initialise library */
	assert(parserutils_initialise(argv[1], myrealloc, NULL) ==
			PARSERUTILS_OK);

	assert(parserutils_inputstream_create("UTF-8", 1, NULL,
			myrealloc, NULL, &stream) == PARSERUTILS_OK);

	fp = fopen(argv[2], "rb");
	if (fp == NULL) {
		printf("Failed opening %s\n", argv[2]);
		return 1;
	}

	fseek(fp, 0, SEEK_END);
	origlen = len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	while (len >= CHUNK_SIZE) {
		size_t read = fread(buf, 1, CHUNK_SIZE, fp);
		assert(read == CHUNK_SIZE);

		assert(parserutils_inputstream_append(stream,
				buf, CHUNK_SIZE) == PARSERUTILS_OK);

		len -= CHUNK_SIZE;

		while (parserutils_inputstream_peek(stream, 0, &c, &clen) !=
				PARSERUTILS_NEEDDATA) {
			parserutils_inputstream_advance(stream, clen);
		}
	}

	if (len > 0) {
		size_t read = fread(buf, 1, len, fp);
		assert(read == len);

		assert(parserutils_inputstream_append(stream,
				buf, len) == PARSERUTILS_OK);

		len = 0;
	}

	fclose(fp);

	assert(parserutils_inputstream_insert(stream,
			(const uint8_t *) "hello!!!",
			SLEN("hello!!!")) == PARSERUTILS_OK);

	assert(parserutils_inputstream_append(stream, NULL, 0) ==
			PARSERUTILS_OK);

	while (parserutils_inputstream_peek(stream, 0, &c, &clen) !=
			PARSERUTILS_EOF) {
		parserutils_inputstream_advance(stream, clen);
	}

	parserutils_inputstream_destroy(stream);

	assert(parserutils_finalise(myrealloc, NULL) == PARSERUTILS_OK);

	printf("PASS\n");

	return 0;
}

