#!/usr/bin/python

import sys
import struct

fname = sys.argv[1]

data = open(fname, "rb").read()

readable_data = struct.unpack("h"*(len(data)//2), data)

lines = [[0]]*(len(readable_data)//7)


for i in range(len(lines)):
    j = i*7
    lines[i] = [num for num in readable_data[j:j+7]]


with open("from_binary",'w') as file:

    for line in lines:
        for elem in line:
            file.write(str(elem)+'\t')
        file.write('\n')


