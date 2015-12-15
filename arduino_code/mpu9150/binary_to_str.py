#!/usr/bin/python

import sys
import struct

fname = sys.argv[1]

data = open(fname, "rb").read()

readable_data = struct.unpack("H"*(len(data)//2), data)

ts = readable_data[::15]
ax = readable_data[1::15]
ay = readable_data[2::15]
az = readable_data[3::15]

print(ts)
