
import sys
import re
import os
import commands
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
from scipy.interpolate import interp1d
from VG_ConstPulse_IDSAT import IDSAT
from I_V_curves import IDS_VGS_stress, IDS_VGS
from VG_ConstPulse_horizontal_hist import IDSAT_horizontal_hist
from VG_ConstPulse_separation import IDSAT_separation
from Charge_Pumping import Charge_Pumping_compare

def main():

    Charge_Pumping_compare([8, 7], ['Fresh', 'HCI stressed'], 21, 36, 2, 'ULVT', 128, ['../data/Charge_Pumping_Chip08/Fresh_Chip08_Col21_60DC_60Pumping_MultiFreq_SweepVSVBVD_VSS_WL_0_VDD_WL_1p6_ELTM', '../data/Charge_Pumping_Chip07/HCI_Chip07_Col21_60DC_60Pumping_MultiFreq_SweepVSVBVD_VSS_WL_0_VDD_WL_1p6_ELTM'], '../plot/chip08_07/', 'FreshChip08_vs_HCIChip07_VG1p8VD2p0_12x0p2s_Charge_Pumping_VSS_WL_0_VDD_WL_1p6_5MHz_', 'Fresh-Chip08 vs HCI Chip07 (VG=1.8, VD=2.0, 12 x 0.2s, col[21], 5MHz\n', -2.0, 0, 0.1, 21, 0, 1.6, 60, [3, 8], [[5000000, 2500000, 1000], [5000000, 2500000, 1000000, 100000, 10000, 1000, 100, 10]], 0, [2, 5], '1kHz', [[0], [0]])

    Charge_Pumping_compare([8, 7], ['Fresh', 'HCI stressed'], 21, 36, 2, 'ULVT', 128, ['../data/Charge_Pumping_Chip08/Fresh_Chip08_Col21_60DC_60Pumping_MultiFreq_SweepVSVBVD_VSS_WL_0_VDD_WL_1p6_ELTM', '../data/Charge_Pumping_Chip07/HCI_Chip07_Col21_60DC_60Pumping_MultiFreq_SweepVSVBVD_VSS_WL_0_VDD_WL_1p6_ELTM'], '../plot/chip08_07/', 'FreshChip08_vs_HCIChip07_VG1p8VD2p0_12x0p2s_Charge_Pumping_VSS_WL_0_VDD_WL_1p6_2p5MHz_', 'Fresh-Chip08 vs HCI Chip07 (VG=1.8, VD=2.0, 12 x 0.2s, col[21], 2.5MHz\n', -2.0, 0, 0.1, 21, 0, 1.6, 60, [3, 8], [[5000000, 2500000, 1000], [5000000, 2500000, 1000000, 100000, 10000, 1000, 100, 10]], 0, [2, 5], '1kHz', [[1], [1]])

if __name__ == '__main__':
  main()
    
