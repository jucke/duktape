============
CBOR binding
============

C functions to encode/decode values in CBOR format, and a simple command
line utility to convert between JSON and CBOR.

Basic usage of the conversion tool:

```
$ make jsoncbor
[...]
$ cat test.json | ./jsoncbor -e   # writes CBOR to stdout
$ cat test.cbor | ./jsoncbor -d   # writes JSON to stdout
```

CBOR is a standard format for JSON-like binary interchange.  It is
faster and smaller, and can encode more data types than JSON.  In particular,
binary data can be serialized without encoding e.g. in base-64.  These
properties make it useful for e.g. storing state files, IPC, etc.

Direct support for CBOR is likely to be included in the Duktape API in the
future.  This extra will then become unnecessary.

Some CBOR shortcomings for preserving information:

- No property attribute or inheritance support.
- No DAGs or looped graphs.
- Array objects with properties lose their non-index properties.
- Buffer objects and views lose much of their detail besides the raw data.
- Ecmascript strings cannot be fully represented; strings must be UTF-8.
- Functions and native objects lose most of their detail.

Future work:
- https://datatracker.ietf.org/doc/draft-jroatch-cbor-tags/?include_text=1
  could be used for typed arrays.
