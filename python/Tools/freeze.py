#!/usr/local/python-3.0rc2/bin/python3.0

import sys
import marshal

def freeze(filename):
    """
    Freeze the given file.
    """
    print("Freezing", filename)
    source = open(filename).read()
    code_object = compile(source, "<frozen %s>" % filename, "exec")
    return marshal.dumps(code_object)

def main():
    code_string = freeze(sys.argv[1])
    s = "unsigned char x[] = {"
    for i in range(0, len(code_string), 16):
        s += "\n\t"
        for c in code_string[i:i+16]:
            s += "%d," % c
    s += "\n};\n"
    print(s)

if __name__ == "__main__":
    main()
