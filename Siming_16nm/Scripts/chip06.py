
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

def main():
    #IDSAT(6, 20, 36, 2, 'SVT', 1.8, 2.0, 32, 7, 1, 10, 0.2, 1, 1, [0, 0.4, 0.8, 1.2, 1.6, 2.0, 2.1], ['0', '0.4', '0.8', '1.2', '1.6', '2.0', 'recovery'], '../data/chip06/', '../plot/chip06/')
    #IDSAT(6, 32, 16, 2, 'SVT', 1.8, 1.7, 32, 7, 1, 10, 0.2, 1, 1, [0, 0.4, 0.8, 1.2, 1.6, 2.0, 2.1], ['0', '0.4', '0.8', '1.2', '1.6', '2.0', 'recovery'], '../data/chip06/', '../plot/chip06/')

    #IDS_VGS_stress(4, 20, 36, 2, 'SVT', 32, 1.8, 2.0, 7, 1)
    #IDS_VGS_stress(4, 32, 16, 2, 'SVT', 32, 1.8, 1.7, 7, 1)
    
    #IDSAT_horizontal_hist(4, 20, 36, 2, 'SVT', 1.8, 2.0, 32, 7, 1)
    #IDSAT_horizontal_hist(4, 32, 16, 2, 'SVT', 1.8, 1.7, 32, 7, 1)

    #IDSAT_separation(4, 20, 36, 2, 'SVT', 1.8, 2.0, 32, 7, 1)
    #IDSAT_separation(4, 32, 16, 2, 'SVT', 1.8, 1.7, 32, 7, 1)

    hci_row_idx = range(0, 7)+range(8, 15)+range(16, 23)+range(24,31)
    #IDS_VGS(6, 20, 36, 2, 'SVT', 32, ['../data/chip06/Fresh_Chip06_Col20_Ids_Vgs_VAsource_VBdrain', '../data/chip06/Stress_Chip06_Col20_Ids_Vgs_VAsource_VBdrain_01'], ['b', 'r'], '../plot/chip06/', 'Fresh_HCI-VG1p8VD2p0-2sec-01_IDS-VGS_VaS-VbD_', hci_row_idx, 'Fresh vs HCI (VG=1.8, VD=2.0, 0.2sec x 10), forward', 92) 
    #IDS_VGS(6, 20, 36, 2, 'SVT', 32, ['../data/chip06/Fresh_Chip06_Col20_Ids_Vgs_VAdrain_VBsource', '../data/chip06/Stress_Chip06_Col20_Ids_Vgs_VAdrain_VBsource_01'], ['b', 'r'], '../plot/chip06/', 'Fresh_HCI-VG1p8VD2p0-2sec-01_IDS-VGS_VaD-VbS_', hci_row_idx, 'Fresh vs HCI (VG=1.8, VD=2.0, 0.2sec x 10), reversed', 92) 

    #IDS_VGS(6, 32, 16, 2, 'SVT', 32, ['../data/chip06/Fresh_Chip06_Col32_Ids_Vgs_VAsource_VBdrain', '../data/chip06/Stress_Chip06_Col32_Ids_Vgs_VAsource_VBdrain_01'], ['b', 'r'], '../plot/chip06/', 'Fresh_HCI-VG1p8VD1p7-2sec-01_IDS-VGS_VaS-VbD_', hci_row_idx, 'Fresh vs HCI (VG=1.8, VD=1.7, 0.2sec x 10), forward', 120) 
    #IDS_VGS(6, 32, 16, 2, 'SVT', 32, ['../data/chip06/Fresh_Chip06_Col32_Ids_Vgs_VAdrain_VBsource', '../data/chip06/Stress_Chip06_Col32_Ids_Vgs_VAdrain_VBsource_01'], ['b', 'r'], '../plot/chip06/', 'Fresh_HCI-VG1p8VD1p7-2sec-01_IDS-VGS_VaD-VbS_', hci_row_idx, 'Fresh vs HCI (VG=1.8, VD=1.7, 0.2sec x 10), reversed', 120) 

    #IDS_VGS(6, 20, 36, 2, 'SVT', 32, ['../data/chip06/Stress_Chip06_Col20_Ids_Vgs_VAsource_VBdrain_01', '../data/chip06/Block_BD_Erase_Chip06_Col20_Ids_Vgs_VAsource_VBdrain_01'], ['r', 'g'], '../plot/chip06/', 'HCI-VG1p8VD2p0-2sec-01_Erase-VSVD0-VB2p4-2min-01_IDS-VGS_VaS-VbD_', hci_row_idx, 'HCI (VG=1.8, VD=2.0, 0.2sec x 10) vs Erase (VS=VD=0, VB=2.4, 2min), forward', 92) 
    #IDS_VGS(6, 20, 36, 2, 'SVT', 32, ['../data/chip06/Stress_Chip06_Col20_Ids_Vgs_VAdrain_VBsource_01', '../data/chip06/Block_BD_Erase_Chip06_Col20_Ids_Vgs_VAdrain_VBsource_01'], ['r', 'g'], '../plot/chip06/', 'HCI-VG1p8VD2p0-2sec-01_Erase-VSVD0-VB2p4-2min-01_IDS-VGS_VaD-VbS_', hci_row_idx, 'HCI (VG=1.8, VD=2.0, 0.2sec x 10) vs Erase (VS=VD=0, VB=2.4, 2min), reversed', 92) 

    IDS_VGS(6, 32, 16, 2, 'SVT', 32, ['../data/chip06/Stress_Chip06_Col32_Ids_Vgs_VAsource_VBdrain_01', '../data/chip06/Block_BD_Erase_Chip06_Col32_Ids_Vgs_VAsource_VBdrain_01'], ['r', 'g'], '../plot/chip06/', 'HCI-VG1p8VD1p7-2sec-01_Erase-VSVD0-VB2p4-5secx15-01_IDS-VGS_VaS-VbD_', hci_row_idx, 'HCI (VG=1.8, VD=1.7, 0.2sec x 10) vs Erase (VS=VD=0, VB=2.4, 5sec x 25), forward', 120) 
    IDS_VGS(6, 32, 16, 2, 'SVT', 32, ['../data/chip06/Stress_Chip06_Col32_Ids_Vgs_VAdrain_VBsource_01', '../data/chip06/Block_BD_Erase_Chip06_Col32_Ids_Vgs_VAdrain_VBsource_01'], ['r', 'g'], '../plot/chip06/', 'HCI-VG1p8VD1p7-2sec-01_Erase-VSVD0-VB2p4-5secx15-01_IDS-VGS_VaD-VbS_', hci_row_idx, 'HCI (VG=1.8, VD=1.7, 0.2sec x 10) vs Erase (VS=VD=0, VB=2.4, 5sec x 25), reversed', 120) 

if __name__ == '__main__':
  main()
    
