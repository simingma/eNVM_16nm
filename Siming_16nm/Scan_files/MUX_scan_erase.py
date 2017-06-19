#!/usr/bin/python26 -tt

import sys
import re
import os
import commands
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
#from scipy.interpolate import interp1d

def main():
    column = 36
    fread = open('MUX_address_enable', 'r')
    for col in np.arange(column):
        fwrite = open('MUX_Col'+str(col).zfill(2)+'_VAsource_VBdrain_erase', 'w')
        read_line = fread.readline()
        for bit in np.array([4, 3, 2, 1, 0]):
            if read_line[bit] == '1':
                fwrite.write('1')
            if read_line[bit] == '0':
                fwrite.write('0')
        for bit in np.array([5, 6, 7, 8]):
            if read_line[bit] == '1':
                fwrite.write('0')
            if read_line[bit] == '0':
                fwrite.write('1')
        fwrite.write('1')
        #scan_line = np.fromstring(read_line, dtype=np.bool, count=10)
        #scan_line[5] = 1-scan_line[5]
        #scan_line[6] = 1-scan_line[6]
        #scan_line[7] = 1-scan_line[7]
        #scan_line[8] = 1-scan_line[8]
        fwrite.close()

        fwrite = open('MUX_Col'+str(col).zfill(2)+'_VAdrain_VBsource_erase', 'w')
        read_line = fread.readline()
        for bit in np.array([4, 3, 2, 1, 0]):
            if read_line[bit] == '1':
                fwrite.write('1')
            if read_line[bit] == '0':
                fwrite.write('0')
        for bit in np.array([5, 6, 7, 8]):
            if read_line[bit] == '1':
                fwrite.write('0')
            if read_line[bit] == '0':
                fwrite.write('1')
        fwrite.write('1')
        fwrite.close()


if __name__ == '__main__':
  main()

