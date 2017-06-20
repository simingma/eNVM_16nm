
import sys
import re
import os
import commands
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
from scipy.interpolate import interp1d
from VG_ConstPulse_IDSAT import IDSAT
from I_V_curves import hist_IDS_VGS, IDS_VGS_stress, IDS_VGS
from VG_ConstPulse_horizontal_hist import IDSAT_horizontal_hist
from VG_ConstPulse_separation import IDSAT_separation

def main():
    #IDSAT(4, 20, 36, 2, 'SVT', 1.8, 2.0, 32, 7, 1)
    #IDSAT(4, 32, 16, 2, 'SVT', 1.8, 1.7, 32, 7, 1)

    #IDS_VGS_stress(4, 20, 36, 2, 'SVT', 32, 1.8, 2.0, 7, 1)
    #IDS_VGS_stress(4, 32, 16, 2, 'SVT', 32, 1.8, 1.7, 7, 1)
    
    #IDSAT_horizontal_hist(4, 20, 36, 2, 'SVT', 1.8, 2.0, 32, 7, 1)
    #IDSAT_horizontal_hist(4, 32, 16, 2, 'SVT', 1.8, 1.7, 32, 7, 1)

    #IDSAT_separation(4, 20, 36, 2, 'SVT', 1.8, 2.0, 32, 7, 1)
    #IDSAT_separation(4, 32, 16, 2, 'SVT', 1.8, 1.7, 32, 7, 1)

    hci_row_idx = range(0, 7)+range(8, 15)+range(16, 23)+range(24,31)
    #IDS_VGS(4, 20, 36, 2, 'SVT', 32, ['../data/Chip4_BD_Erase/Retention_1month_Chip04_Col20_Ids_Vgs_VAsource_VBdrain', '../data/Chip4_BD_Erase/Block_BD_Erase_Chip04_Col20_Ids_Vgs_VAsource_VBdrain_05'], ['y', 'm'], '../plot/chip04_BD_Erase/', 'Retention-1month_Block-BD-Erase-05_IDS-VGS_VaS-VbD_', hci_row_idx, '1month retention after HCI (VG=1.8, VD=2.0) vs total Block BD Erase, forward', 92) 
    #IDS_VGS(4, 20, 36, 2, 'SVT', 32, ['../data/Chip4_BD_Erase/Retention_1month_Chip04_Col20_Ids_Vgs_VAdrain_VBsource', '../data/Chip4_BD_Erase/Block_BD_Erase_Chip04_Col20_Ids_Vgs_VAdrain_VBsource_05'], ['y', 'm'], '../plot/chip04_BD_Erase/', 'Retention-1month_Block-BD-Erase-05_IDS-VGS_VaD-VbS_', hci_row_idx, '1month retention after HCI (VG=1.8, VD=2.0) vs total Block BD Erase, reversed', 92) 
    hist_IDS_VGS(0, 4, 20, 36, 2, 'SVT', 32, ['../data/Chip4_BD_Erase/Retention_1month_Chip04_Col20_Ids_Vgs_VAdrain_VBsource'], ['m'], '../plot/chip04_BD_Erase/', 'Retention-1month_IDS-VGS0p8_VaD-VbS_', hci_row_idx, '1month retention (IDS at VGS=0.8) after HCI (VG=1.8, VD=2.0), reversed', 40, 100, 46.340600000000002, 91.337001999999998) 

if __name__ == '__main__':
  main()
    
