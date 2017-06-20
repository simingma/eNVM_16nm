
import sys
import re
import os
import commands
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
from scipy.interpolate import interp1d
from VG_ConstPulse_IDSAT import IDSAT
from VG_ConstPulse_horizontal_hist import IDSAT_horizontal_hist

def main():
    #IDSAT(1, 20, 36, 2, 'SVT', 2.0, 2.0, 32, 7, 1)
    IDSAT_horizontal_hist(1, 20, 36, 2, 'SVT', 2.0, 2.0, 32, 7, 1)

if __name__ == '__main__':
  main()
    
