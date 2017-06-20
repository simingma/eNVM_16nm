
import sys
import re
import os
import commands
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
from scipy.interpolate import interp1d
from I_V_curves import IDS_VGS

def main():
    IDS_VGS(2, 33, 16, 2, 'ULVT', 128, '../data/VG_realvalue_chip02/', '../plot/VG_realvalue_chip02/')

if __name__ == '__main__':
  main()
    
