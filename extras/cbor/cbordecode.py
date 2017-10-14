#!/usr/bin/env python2

def main():
    import sys
    import cbor
    import json

    data = sys.stdin.read()
    print('LEN: %d' % len(data))
    print('HEX: ' + data.encode('hex'))
    doc = cbor.loads(data)
    print('REPR: ' + repr(doc))
    try:
        print('JSON: ' + json.dumps(doc))
    except:
        print('JSON: cannot encode')

if __name__ == '__main__':
    main()
