# For manual testing; say 'make' in extras/cbor and run ./test.

VALGRIND=valgrind
.PHONY: all
all: clean test jsoncbor test-runs

.PHONY: clean
clean:
	-rm -f test jsoncbor

test: test.c
	gcc -std=c99 -Wall -Wextra -g -ggdb -o $@ -I../../src/ -I. ../../src/duktape.c duk_cbor.c test.c -lm

jsoncbor: jsoncbor.c
	gcc -std=c99 -Wall -Wextra -O2 -o $@ -I../../src/ -I. ../../src/duktape.c duk_cbor.c jsoncbor.c -lm

.PHONY: test-runs
test-runs: test enc-primitives enc-number enc-buffer enc-string enc-array enc-object enc-misc

.PHONY: enc-primitives
enc-primitives: test
	$(VALGRIND) ./test 'void 0' | python2 cbordecode.py
	$(VALGRIND) ./test 'null' | python2 cbordecode.py
	$(VALGRIND) ./test 'true' | python2 cbordecode.py
	$(VALGRIND) ./test 'false' | python2 cbordecode.py

.PHONY: enc-number
enc-number: test
	$(VALGRIND) ./test '0' | python2 cbordecode.py
	$(VALGRIND) ./test '0.1' | python2 cbordecode.py
	$(VALGRIND) ./test '1' | python2 cbordecode.py
	$(VALGRIND) ./test '23' | python2 cbordecode.py
	$(VALGRIND) ./test '24' | python2 cbordecode.py
	$(VALGRIND) ./test '25' | python2 cbordecode.py
	$(VALGRIND) ./test '255' | python2 cbordecode.py
	$(VALGRIND) ./test '256' | python2 cbordecode.py
	$(VALGRIND) ./test '65535' | python2 cbordecode.py
	$(VALGRIND) ./test '65536' | python2 cbordecode.py
	$(VALGRIND) ./test '65536.9' | python2 cbordecode.py
	$(VALGRIND) ./test '4294967295' | python2 cbordecode.py
	$(VALGRIND) ./test '4294967296' | python2 cbordecode.py
	$(VALGRIND) ./test '4294967297' | python2 cbordecode.py
	$(VALGRIND) ./test '0xdeadbeef' | python2 cbordecode.py
	$(VALGRIND) ./test '3.141592653589793' | python2 cbordecode.py
	$(VALGRIND) ./test '-0' | python2 cbordecode.py
	$(VALGRIND) ./test '-0.1' | python2 cbordecode.py
	$(VALGRIND) ./test '-1' | python2 cbordecode.py
	$(VALGRIND) ./test '-23' | python2 cbordecode.py
	$(VALGRIND) ./test '-24' | python2 cbordecode.py
	$(VALGRIND) ./test '-25' | python2 cbordecode.py
	$(VALGRIND) ./test '-255' | python2 cbordecode.py
	$(VALGRIND) ./test '-256' | python2 cbordecode.py
	$(VALGRIND) ./test '-65535' | python2 cbordecode.py
	$(VALGRIND) ./test '-65536' | python2 cbordecode.py
	$(VALGRIND) ./test '-65536.9' | python2 cbordecode.py
	$(VALGRIND) ./test '-4294967295' | python2 cbordecode.py
	$(VALGRIND) ./test '-4294967296' | python2 cbordecode.py
	$(VALGRIND) ./test '-4294967297' | python2 cbordecode.py
	$(VALGRIND) ./test '-0xdeadbeef' | python2 cbordecode.py
	$(VALGRIND) ./test '-3.141592653589793' | python2 cbordecode.py
	$(VALGRIND) ./test '1/0' | python2 cbordecode.py
	$(VALGRIND) ./test '0/0' | python2 cbordecode.py
	$(VALGRIND) ./test '-1/0' | python2 cbordecode.py

.PHONY: enc-buffer
enc-buffer: test
	$(VALGRIND) ./test '(function () { var p = Uint8Array.allocPlain(0); return p; })()' | python2 cbordecode.py
	$(VALGRIND) ./test '(function () { var p = Uint8Array.allocPlain(1); p[0] = 0xfe; return p; })()' | python2 cbordecode.py
	$(VALGRIND) ./test '(function () { var p = Uint8Array.allocPlain(2); p[0] = 0xc0; p[1] = 0x80; return p; })()' | python2 cbordecode.py
	$(VALGRIND) ./test '(function () { var p = Uint8Array.allocPlain(3); p[0] = 0x31; p[1] = 0x4a; p[2] = 0x7a; return p; })()' | python2 cbordecode.py
	$(VALGRIND) ./test '(function () { var p = Uint8Array.allocPlain(23); p[22] = 0xfe; return p; })()' | python2 cbordecode.py
	$(VALGRIND) ./test '(function () { var p = Uint8Array.allocPlain(24); p[23] = 0xfe; return p; })()' | python2 cbordecode.py
	$(VALGRIND) ./test '(function () { var p = Uint8Array.allocPlain(255); p[254] = 0xfe; return p; })()' | python2 cbordecode.py
	$(VALGRIND) ./test '(function () { var p = Uint8Array.allocPlain(256); p[255] = 0xfe; return p; })()' | python2 cbordecode.py
	$(VALGRIND) ./test '(function () { var p = Uint8Array.allocPlain(256); p[255] = 0xfe; return p; })()' | python2 cbordecode.py
	$(VALGRIND) ./test '(function () { var p = new Uint16Array([ 1, 2, 3, 4 ]); return p; })()' | python2 cbordecode.py

.PHONY: enc-string
enc-string: test
	$(VALGRIND) ./test '""' | python2 cbordecode.py
	$(VALGRIND) ./test '"foo"' | python2 cbordecode.py
	$(VALGRIND) ./test '"foo\u20acbar"' | python2 cbordecode.py
	$(VALGRIND) ./test '"foo\ud800bar"' | python2 cbordecode.py  # unpaired surrogate
	$(VALGRIND) ./test '"\u4321\u4321\u4321\u4321\u4321\u4321\u4321xy"' | python2 cbordecode.py
	$(VALGRIND) ./test '"\u4321\u4321\u4321\u4321\u4321\u4321\u4321xyz"' | python2 cbordecode.py
	$(VALGRIND) ./test '"\u4321\u4321\u4321\u4321\u4321\u4321\u4321xyzw"' | python2 cbordecode.py
	$(VALGRIND) ./test '"\u4321\u4321\u4321\u4321\u4321\u4321\u4321xyzw......................................................................................................................................................................................................................................"' | python2 cbordecode.py
	$(VALGRIND) ./test '"\u4321\u4321\u4321\u4321\u4321\u4321\u4321xyzw......................................................................................................................................................................................................................................!"' | python2 cbordecode.py

.PHONY: enc-array
enc-array: test
	$(VALGRIND) ./test '[]' | python2 cbordecode.py
	$(VALGRIND) ./test '[ "foo", "bar", "quux" ]' | python2 cbordecode.py
	# FIXME: other lengths
	$(VALGRIND) ./test '([[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[{ foo: 123 }]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]])' | python2 cbordecode.py

.PHONY: enc-object
enc-object: test
	$(VALGRIND) ./test '({})' | python2 cbordecode.py
	$(VALGRIND) ./test '({ foo: 123 })' | python2 cbordecode.py
	$(VALGRIND) ./test '({ foo: 123, bar: 234, quux: 345 })' | python2 cbordecode.py
	$(VALGRIND) ./test '({ foo: 123, bar: [ "foo", "bar", { baz: true } ], quux: 345 })' | python2 cbordecode.py
	$(VALGRIND) ./test '({foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:{foo:123}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}})' | python2 cbordecode.py

.PHONY: enc-misc
enc-misc: test
	$(VALGRIND) ./test '({ jsonrpc: "2.0", id: "foo-1", method: "Add", params: { a: 123, b: 234 }})' | python2 cbordecode.py
