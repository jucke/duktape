#include <stdio.h>
#include "duktape.h"
#include "duk_cbor.h"

static void push_stdin(duk_context *ctx) {
	unsigned char *buf;
	size_t off;
	size_t len;
	size_t got;

	off = 0;
	len = 256;
	buf = (unsigned char *) duk_push_dynamic_buffer(ctx, len);

	for (;;) {
#if 1
		fprintf(stderr, "reading %ld of input\n", (long) (len - off));
#endif
		got = fread(buf + off, 1, len - off, stdin);
		if (got == 0U) {
			break;
		}
		off += got;
		if (len - off < 256U) {
			size_t newlen = len * 2U;
			buf = (unsigned char *) duk_resize_buffer(ctx, -1, newlen);
			len = newlen;
		}
		
	}
	buf = (unsigned char *) duk_resize_buffer(ctx, -1, off);

	duk_push_lstring(ctx, (const char *) buf, off);
	duk_remove(ctx, -2);
}

static void usage_and_exit(void) {
	fprintf(stderr, "Usage: ./test -e  # encode JSON stdin to CBOR stdout\n");
	fprintf(stderr, "       ./test -d  # decode CBOR stdin to JSON stdout\n");
	exit(1);
}

static duk_ret_t encode_helper(duk_context *ctx, void *udata) {
	unsigned char *buf;
	size_t len;

	(void) udata;

	push_stdin(ctx);
	duk_json_decode(ctx, -1);
	duk_cbor_encode(ctx, -1, 0);

	buf = (unsigned char *) duk_require_buffer_data(ctx, -1, &len);
	fwrite((void *) buf, 1, len, stdout);
	return 0;
}

static duk_ret_t decode_helper(duk_context *ctx, void *udata) {
	(void) udata;
	return duk_type_error(ctx, "unimplemented");
}

int main(int argc, char *argv[]) {
	duk_context *ctx;
	duk_int_t rc;
	int encode = 0;
	int exitcode = 0;

	if (argc < 2) {
		usage_and_exit();
	}
	if (strcmp(argv[1], "-e") == 0) {
		encode = 1;
	} else if (strcmp(argv[1], "-d") == 0) {
		;
	} else {
		usage_and_exit();
	}

	ctx = duk_create_heap_default();
	if (!ctx) {
		return 1;
	}

	rc = duk_safe_call(ctx, encode ? encode_helper : decode_helper, NULL, 0, 0);
	if (rc != 0) {
		fprintf(stderr, "%s\n", duk_safe_to_string(ctx, -1));
		exitcode = 1;
	}

	duk_destroy_heap(ctx);

	return exitcode;
}
