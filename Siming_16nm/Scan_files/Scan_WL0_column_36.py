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
    row = 128
    for col in np.arange(column):
        f = open('Scan_Col'+str(col).zfill(2)+'_WL0_NOpulse', 'w')
        for c in np.arange(column):
            if(c != (column -1 - col)):
                for r in np.arange(row):
                    f.write('00001111\n')
                    f.write('01001111\n')
                    f.write('00001111\n')
                    f.write('00101111\n')
            if(c == (column -1 - col)):
                for r in np.arange(row-1):
                    f.write('00001111\n')
                    f.write('01001111\n')
                    f.write('00001111\n')
                    f.write('00101111\n')
                f.write('10001111\n')
                f.write('11001111\n')
                f.write('10001111\n')
                f.write('10101111\n')
        f.write('00001111\n')
        f.write('00001111')
        f.close()

        f = open('Scan_Col'+str(col).zfill(2)+'_WL0_pulse', 'w')
        for c in np.arange(column):
            if(c != (column -1 - col)):
                for r in np.arange(row):
                    f.write('00001111\n')
                    f.write('01001111\n')
                    f.write('00001111\n')
                    f.write('00101111\n')
            if(c == (column -1 - col)):
                for r in np.arange(row-1):
                    f.write('00001111\n')
                    f.write('01001111\n')
                    f.write('00001111\n')
                    f.write('00101111\n')
                f.write('10001111\n')
                f.write('11001111\n')
                f.write('10001111\n')
                f.write('10101111\n')
        f.write('00001111\n')
        f.write('00011111')
        f.close()


if __name__ == '__main__':
  main()

