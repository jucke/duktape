/*
 *  CBOR bindings for Duktape
 *
 *  https://tools.ietf.org/html/rfc7049#section-2.3
 */

#include <math.h>
#include "duktape.h"
#include "duk_cbor.h"

typedef struct {
	duk_context *ctx;
	duk_idx_t idx_buf;
	duk_size_t len;
	duk_size_t off;
} duk_cbor_encode_context;

typedef struct {
	int dummy;
} duk_cbor_decode_context;

static duk_uint8_t *duk__cbor_encode_reserve(duk_cbor_encode_context *enc_ctx, duk_size_t len) {
	duk_uint8_t *res;
	duk_size_t newlen;
	duk_size_t ignlen;

	while (enc_ctx->off + len > enc_ctx->len) {
		newlen = enc_ctx->len * 2U;
		if (newlen < enc_ctx->len) {
			(void) duk_range_error(enc_ctx->ctx, "too large");
		}
#if 1
		fprintf(stderr, "resize to %ld\n", (long) newlen);
#endif
		duk_resize_buffer(enc_ctx->ctx, enc_ctx->idx_buf, newlen);
		enc_ctx->len = newlen;
	}
	res = (duk_uint8_t *) duk_require_buffer(enc_ctx->ctx, enc_ctx->idx_buf, &ignlen) + enc_ctx->off;
	enc_ctx->off += len;
	return res;
}

static void duk__cbor_encode_emitbyte(duk_cbor_encode_context *enc_ctx, duk_uint8_t val) {
	duk_uint8_t *p;

	p = duk__cbor_encode_reserve(enc_ctx, 1);
	*p = val;
}

static void duk__cbor_encode_uint32(duk_cbor_encode_context *enc_ctx, duk_uint32_t u, duk_uint8_t base) {
	duk_uint8_t *p;

	if (u <= 23U) {
		duk__cbor_encode_emitbyte(enc_ctx, base + (duk_uint8_t) u);
	} else if (u <= 0xffUL) {
		p = duk__cbor_encode_reserve(enc_ctx, 1 + 1);
		*p++ = base + 0x18U;
		*p++ = (duk_uint8_t) u;
	} else if (u <= 0xffffUL) {
		p = duk__cbor_encode_reserve(enc_ctx, 1 + 2);
		*p++ = base + 0x19U;
		*p++ = (duk_uint8_t) ((u >> 8) & 0xffU);
		*p++ = (duk_uint8_t) (u & 0xffU);
	} else {
		p = duk__cbor_encode_reserve(enc_ctx, 1 + 4);
		*p++ = base + 0x1aU;
		*p++ = (duk_uint8_t) ((u >> 24) & 0xffU);
		*p++ = (duk_uint8_t) ((u >> 16) & 0xffU);
		*p++ = (duk_uint8_t) ((u >> 8) & 0xffU);
		*p++ = (duk_uint8_t) (u & 0xffU);
	}
}

static void duk__cbor_encode_double(duk_cbor_encode_context *enc_ctx, double d) {
	duk_uint8_t *p;
	union {
		duk_uint8_t x[8];
		double d;
	} u;

	if (isfinite(d)) {
		double t = floor(d);
		if (t == d && (d != 0.0 || signbit(d) == 0)) {
			if (d >= 0.0) {
				if (d <= 4294967295.0) {
					duk__cbor_encode_uint32(enc_ctx, (duk_uint32_t) d, 0x00U);
					return;
				}
			} else {
				if (d >= -4294967296.0) {
					duk__cbor_encode_uint32(enc_ctx, (duk_uint32_t) (-1.0 - d), 0x20U);
					return;
				}
			}
			/* 64-bit integers are not supported at present */
		}
	}

	u.d = d;
	p = duk__cbor_encode_reserve(enc_ctx, 1 + 8);
	*p++ = 0xfbU;
#if 0  /* FIXME: endianness */
	*p++ = u.x[0];
	*p++ = u.x[1];
	*p++ = u.x[2];
	*p++ = u.x[3];
	*p++ = u.x[4];
	*p++ = u.x[5];
	*p++ = u.x[6];
	*p++ = u.x[7];
#else
	*p++ = u.x[7];
	*p++ = u.x[6];
	*p++ = u.x[5];
	*p++ = u.x[4];
	*p++ = u.x[3];
	*p++ = u.x[2];
	*p++ = u.x[1];
	*p++ = u.x[0];
#endif
}

static void duk__cbor_encode_value(duk_cbor_encode_context *enc_ctx) {
	/* FIXME: add type tags for unambiguous cases to try to ensure
	 * output parses back without losing information, e.g. pointers.
	 */
	duk_uint8_t *p;
	duk_uint8_t *buf;
	const duk_uint8_t *str;
	duk_size_t len;

	/* When working with deeply recursive structures, this is important
	 * to ensure there's no effective depth limit.
	 */
	duk_require_stack(enc_ctx->ctx, 4);

	switch (duk_get_type(enc_ctx->ctx, -1)) {
	case DUK_TYPE_UNDEFINED:
		duk__cbor_encode_emitbyte(enc_ctx, 0xf7U);
		break;
	case DUK_TYPE_NULL:
		duk__cbor_encode_emitbyte(enc_ctx, 0xf6U);
		break;
	case DUK_TYPE_BOOLEAN:
		duk__cbor_encode_emitbyte(enc_ctx, duk_get_boolean(enc_ctx->ctx, -1) ?
		                                   0xf5U : 0xf4U);
		break;
	case DUK_TYPE_NUMBER:
		duk__cbor_encode_double(enc_ctx, duk_get_number(enc_ctx->ctx, -1));
		break;
	case DUK_TYPE_STRING:
		/* FIXME: transcode to UTF-8; replacements for invalid
		 * surrogate pairs, or at least WTF-8.
		 */
		str = (const duk_uint8_t *) duk_require_lstring(enc_ctx->ctx, -1, &len);
		if (len != (duk_uint32_t) len) {
			goto fail;
		}
		duk__cbor_encode_uint32(enc_ctx, (duk_uint32_t) len, 0x60U);
		p = duk__cbor_encode_reserve(enc_ctx, len);
		memcpy((void *) p, (const void *) str, len);
		break;
	case DUK_TYPE_OBJECT:
		/* XXX: Date support */
		if (duk_is_array(enc_ctx->ctx, -1)) {
			duk_size_t i;
			len = duk_get_length(enc_ctx->ctx, -1);
			if (len != (duk_uint32_t) len) {
				goto fail;
			}
			duk__cbor_encode_uint32(enc_ctx, (duk_uint32_t) len, 0x80U);
			for (i = 0; i < len; i++) {
				duk_get_prop_index(enc_ctx->ctx, -1, (duk_uarridx_t) i);
				duk__cbor_encode_value(enc_ctx);
			}
		} else if (duk_is_buffer_data(enc_ctx->ctx, -1)) {
			/* FIXME: code sharing */
			/* FIXME: tag type? */
			buf = (duk_uint8_t *) duk_require_buffer_data(enc_ctx->ctx, -1, &len);
			if (len != (duk_uint32_t) len) {
				goto fail;
			}
			duk__cbor_encode_uint32(enc_ctx, (duk_uint32_t) len, 0x40U);
			p = duk__cbor_encode_reserve(enc_ctx, len);
			memcpy((void *) p, (const void *) buf, len);
		} else {
			duk__cbor_encode_emitbyte(enc_ctx, 0xa0U + 0x1fU);  /* indefinite length */
			duk_enum(enc_ctx->ctx, -1, DUK_ENUM_OWN_PROPERTIES_ONLY);
			while (duk_next(enc_ctx->ctx, -1, 1 /*get_value*/)) {
				duk_insert(enc_ctx->ctx, -2);  /* [ ... key value ] -> [ ... value key ] */
				duk__cbor_encode_value(enc_ctx);
				duk__cbor_encode_value(enc_ctx);
			}
			duk_pop(enc_ctx->ctx);
			duk__cbor_encode_emitbyte(enc_ctx, 0xffU);  /* break */
		}
		break;
	case DUK_TYPE_BUFFER:
		buf = (duk_uint8_t *) duk_require_buffer(enc_ctx->ctx, -1, &len);
		if (len != (duk_uint32_t) len) {
			goto fail;
		}
		duk__cbor_encode_uint32(enc_ctx, (duk_uint32_t) len, 0x40U);
		p = duk__cbor_encode_reserve(enc_ctx, len);
		memcpy((void *) p, (const void *) buf, len);
		break;
	case DUK_TYPE_POINTER:
		/* FIXME: integer? memory representation bytes? */
		/* For now encode as 'undefined'. */
		duk__cbor_encode_emitbyte(enc_ctx, 0xf7U);
		break;
	case DUK_TYPE_LIGHTFUNC:
		/* FIXME: tag? */
		/* For now encode as an empty object. */
		duk__cbor_encode_emitbyte(enc_ctx, 0xa0U + 0);  /* zero-length */
		break;
	case DUK_TYPE_NONE:
	default:
		goto fail;
	}
	duk_pop(enc_ctx->ctx);
	return;

 fail:
	(void) duk_type_error(enc_ctx->ctx, "invalid type");
}

void duk_cbor_encode(duk_context *ctx, duk_idx_t idx, duk_uint_t encode_flags) {
	duk_cbor_encode_context enc_ctx;

	(void) encode_flags;

	idx = duk_require_normalize_index(ctx, idx);

	enc_ctx.ctx = ctx;
	enc_ctx.idx_buf = duk_get_top(ctx);
	enc_ctx.off = 0;
	enc_ctx.len = 64;

	duk_push_dynamic_buffer(ctx, enc_ctx.len);
	duk_dup(ctx, idx);
	duk__cbor_encode_value(&enc_ctx);
	duk_resize_buffer(enc_ctx.ctx, enc_ctx.idx_buf, enc_ctx.off);
	duk_replace(ctx, idx);
}

void duk_cbor_decode(duk_context *ctx, duk_idx_t idx, duk_uint_t decode_flags) {
	(void) idx;
	(void) decode_flags;
	(void) duk_type_error(ctx, "unimplemented");
}
