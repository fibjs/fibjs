import ctypes
from ctypes import c_int32, c_char_p

lib = ctypes.cdll.LoadLibrary("bin/Darwin_arm64_release/libjs.dylib")
run_fibjs = lib.run_fibjs

run_fibjs.argtypes = (c_int32, ctypes.POINTER(c_char_p))
run_fibjs.restype = c_int32

argv = [b"", b"-e", b"console.log('hello world')"]
argv_p = (c_char_p * len(argv))(*argv)

run_fibjs(len(argv), argv_p)

input();
