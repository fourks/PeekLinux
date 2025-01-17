#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libcss/libcss.h>

#include "utils/utils.h"

#include "dump.h"
#include "testutils.h"

/** \todo at some point, we need to extend this to handle nested blocks */
typedef struct line_ctx {
	size_t buflen;
	size_t bufused;
	uint8_t *buf;

	size_t explen;
	size_t expused;
	char *exp;

	bool indata;
	bool inerrors;
	bool inexp;

	bool inrule;
} line_ctx;

static bool handle_line(const char *data, size_t datalen, void *pw);
static void parse_expected(line_ctx *ctx, const char *data, size_t len);
static void run_test(const uint8_t *data, size_t len, 
		const char *exp, size_t explen);

static void *myrealloc(void *data, size_t len, void *pw)
{
	UNUSED(pw);

	return realloc(data, len);
}

static void *counting_realloc(void *data, size_t len, void *pw)
{
	size_t *counter = (size_t *)pw;
	
	if (data == NULL)
		*counter += 1;
	else if (len == 0)
		*counter -= 1;
	
	return realloc(data, len);
}

static css_error resolve_url(void *pw,
		const char *base, lwc_string *rel, lwc_string **abs)
{
	UNUSED(pw);
	UNUSED(base);

	/* About as useless as possible */
	*abs = lwc_string_ref(rel);

	return CSS_OK;
}

static void
printing_lwc_iterator(lwc_string *str, void *pw)
{
	UNUSED(pw);
	
	printf(" DICT: %*s\n", (int)(lwc_string_length(str)), lwc_string_data(str));
}

int main(int argc, char **argv)
{
	line_ctx ctx;
	size_t counter = 0;
	
	if (argc != 3) {
		printf("Usage: %s <aliases_file> <filename>\n", argv[0]);
		return 1;
	}

	assert(css_initialise(argv[1], myrealloc, NULL) == CSS_OK);

	ctx.buflen = parse_filesize(argv[2]);
	if (ctx.buflen == 0)
		return 1;

	ctx.buf = malloc(ctx.buflen);
	if (ctx.buf == NULL) {
		printf("Failed allocating %u bytes\n",
				(unsigned int) ctx.buflen);
		return 1;
	}

	ctx.buf[0] = '\0';
	ctx.bufused = 0;
	ctx.explen = 0;
	ctx.expused = 0;
	ctx.exp = NULL;
	ctx.indata = false;
	ctx.inerrors = false;
	ctx.inexp = false;

	assert(lwc_initialise(counting_realloc, &counter, 0) == lwc_error_ok);
	
	assert(parse_testfile(argv[2], handle_line, &ctx) == true);

	/* and run final test */
	if (ctx.bufused > 0)
		run_test(ctx.buf, ctx.bufused, ctx.exp, ctx.expused);

	free(ctx.buf);

	assert(css_finalise(myrealloc, NULL) == CSS_OK);
	
	printf("INFO: Counter is %zu\n", counter);
	lwc_iterate_strings(printing_lwc_iterator, NULL);
	
	assert(counter == 2);

	printf("PASS\n");

	return 0;
}

bool handle_line(const char *data, size_t datalen, void *pw)
{
	line_ctx *ctx = (line_ctx *) pw;

	if (data[0] == '#') {
		if (ctx->inexp) {
			/* This marks end of testcase, so run it */

			run_test(ctx->buf, ctx->bufused, 
					ctx->exp, ctx->expused);

			ctx->buf[0] = '\0';
			ctx->bufused = 0;

			ctx->expused = 0;
		}

		if (ctx->indata && strncasecmp(data+1, "errors", 6) == 0) {
			ctx->indata = false;
			ctx->inerrors = true;
			ctx->inexp = false;
		} else if (ctx->inerrors && 
				strncasecmp(data+1, "expected", 8) == 0) {
			ctx->indata = false;
			ctx->inerrors = false;
			ctx->inexp = true;
			ctx->inrule = false;
		} else if (ctx->inexp && strncasecmp(data+1, "data", 4) == 0) {
			ctx->indata = true;
			ctx->inerrors = false;
			ctx->inexp = false;
		} else if (ctx->indata) {
			memcpy(ctx->buf + ctx->bufused, data, datalen);
			ctx->bufused += datalen;
		} else {
			ctx->indata = (strncasecmp(data+1, "data", 4) == 0);
			ctx->inerrors = (strncasecmp(data+1, "errors", 6) == 0);
			ctx->inexp = (strncasecmp(data+1, "expected", 8) == 0);
		}
	} else {
		if (ctx->indata) {
			memcpy(ctx->buf + ctx->bufused, data, datalen);
			ctx->bufused += datalen;
		}
		if (ctx->inexp) {
			parse_expected(ctx, data, datalen);
		}
	}

	return true;
}

void parse_expected(line_ctx *ctx, const char *data, size_t len)
{
	while (ctx->expused + len >= ctx->explen) {
		size_t required = ctx->explen == 0 ? 64 : ctx->explen * 2;
		char *temp = realloc(ctx->exp, required);
		if (temp == NULL) {
			assert(0 && "No memory for expected output");
		}

		ctx->exp = temp;
		ctx->explen = required;
	}

	memcpy(ctx->exp + ctx->expused, data, len);

	ctx->expused += len;
}

void run_test(const uint8_t *data, size_t len, const char *exp, size_t explen)
{
	css_stylesheet *sheet;
	css_error error;
	char *buf;
	size_t buflen;
	static int testnum;

	buf = malloc(2 * explen);
	if (buf == NULL) {
		assert(0 && "No memory for result data");
	}
	buflen = 2 * explen;
	
	assert(css_stylesheet_create(CSS_LEVEL_21, "UTF-8", "foo", NULL,
			false, false, myrealloc, NULL, resolve_url, NULL, 
			&sheet) == CSS_OK);

	error = css_stylesheet_append_data(sheet, data, len);
	if (error != CSS_OK && error != CSS_NEEDDATA) {
		printf("Failed appending data: %d\n", error);
		assert(0);
	}

	assert(css_stylesheet_data_done(sheet) == CSS_OK);

	testnum++;

	dump_sheet(sheet, buf, &buflen);

	if (2 * explen - buflen != explen || memcmp(buf, exp, explen) != 0) {
		printf("Expected (%u):\n%.*s\n", 
				(int) explen, (int) explen, exp);
		printf("Result (%u):\n%.*s\n", (int) (2 * explen - buflen),
			(int) (2 * explen - buflen), buf);
		assert(0 && "Result doesn't match expected");
	}

	css_stylesheet_destroy(sheet);

	printf("Test %d: PASS\n", testnum);
}

