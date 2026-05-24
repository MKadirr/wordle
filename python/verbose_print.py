import sys
import builtins

VERBOSE = False

def print(string : str):
    if VERBOSE:
        builtins.print(string, file=sys.stderr)