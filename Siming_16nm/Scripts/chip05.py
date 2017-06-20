
import sys
import re
import os
import commands
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
from scipy.interpolate import interp1d
from VG_ConstPulse_IDSAT import IDSAT
from I_V_curves import IDS_VGS
from VG_ConstPulse_horizontal_hist import IDSAT_horizontal_hist
from VG_ConstPulse_separation import IDSAT_separation

def main():

    hci_row_idx = range(0, 7)+range(8, 15)+range(16, 23)+range(24,31)
    pbti_row_idx = [7, 15, 23, 31]

    #IDS_VGS(5, 30, 16, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Fresh_Chip05_Col30_Ids_Vgs_VAsource_VBdrain', '../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col30_Ids_Vgs_VAsource_VBdrain_00'], ['b', 'r'], '../plot/chip05/', 'Fresh_StressVG2p4VD1p0-5sec-01_IDS-VGS_VaS-VbD_', hci_row_idx, 'Fresh vs 1st Stress, VG=2.4, VD=1.0, 5sec, forward', 170) 
    #IDS_VGS(5, 30, 16, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Fresh_Chip05_Col30_Ids_Vgs_VAdrain_VBsource', '../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col30_Ids_Vgs_VAdrain_VBsource_00'], ['b', 'r'], '../plot/chip05/', 'Fresh_StressVG2p4VD1p0-5sec-01_IDS-VGS_VaD-VbS_', hci_row_idx, 'Fresh vs 1st Stress, VG=2.4, VD=1.0, 5sec, reversed', 170) 

    #IDS_VGS(5, 30, 16, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Fresh_Chip05_Col30_Ids_Vgs_VAsource_VBdrain', '../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col30_Ids_Vgs_VAsource_VBdrain_00'], ['b', 'r'], '../plot/chip05/', 'Fresh_StressVG2p4VD0-pbti5sec-01_IDS-VGS_VaS-VbD_', pbti_row_idx, 'Fresh vs 1st Stress, VG=2.4, VD=0, 5sec, forward', 170) 
    #IDS_VGS(5, 30, 16, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Fresh_Chip05_Col30_Ids_Vgs_VAdrain_VBsource', '../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col30_Ids_Vgs_VAdrain_VBsource_00'], ['b', 'r'], '../plot/chip05/', 'Fresh_StressVG2p4VD0-pbti5sec-01_IDS-VGS_VaD-VbS_', pbti_row_idx, 'Fresh vs 1st Stress, VG=2.4, VD=0, 5sec, reversed', 170) 

    #IDS_VGS(5, 30, 16, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col30_Ids_Vgs_VAsource_VBdrain_00', '../data/Stacked_VG_Erase_Chip05/Block_Erase_Chip05_Col30_Ids_Vgs_VAsource_VBdrain_00'], ['r', 'g'], '../plot/chip05/', 'StressVG2p4VD1p0-5sec-01_BlockEraseVSVBVD2p4-2min-01_IDS-VGS_VaS-VbD_', hci_row_idx, '1st Stress (VG=2.4, VD=1.0, 5sec) vs 1st Erase (VS=VB=VD=2.4, 2min), forward', 170) 
    #IDS_VGS(5, 30, 16, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col30_Ids_Vgs_VAdrain_VBsource_00', '../data/Stacked_VG_Erase_Chip05/Block_Erase_Chip05_Col30_Ids_Vgs_VAdrain_VBsource_00'], ['r', 'g'], '../plot/chip05/', 'StressVG2p4VD1p0-5sec-01_BlockEraseVSVBVD2p4-2min-01_IDS-VGS_VaD-VbS_', hci_row_idx, '1st Stress (VG=2.4, VD=1.0, 5sec) vs 1st Erase (VS=VB=VD=2.4, 2min), reversed', 170) 

    #IDS_VGS(5, 30, 16, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col30_Ids_Vgs_VAsource_VBdrain_00', '../data/Stacked_VG_Erase_Chip05/Block_Erase_Chip05_Col30_Ids_Vgs_VAsource_VBdrain_00'], ['r', 'g'], '../plot/chip05/', 'StressVG2p4VD0-pbti5sec-01_BlockEraseVSVBVD2p4-2min-01_IDS-VGS_VaS-VbD_', pbti_row_idx, '1st Stress (VG=2.4, VD=0, 5sec) vs 1st Erase (VS=VB=VD=2.4, 2min), forward', 170) 
    #IDS_VGS(5, 30, 16, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col30_Ids_Vgs_VAdrain_VBsource_00', '../data/Stacked_VG_Erase_Chip05/Block_Erase_Chip05_Col30_Ids_Vgs_VAdrain_VBsource_00'], ['r', 'g'], '../plot/chip05/', 'StressVG2p4VD0-pbti5sec-01_BlockEraseVSVBVD2p4-2min-01_IDS-VGS_VaD-VbS_', pbti_row_idx, '1st Stress (VG=2.4, VD=0, 5sec) vs 1st Erase (VS=VB=VD=2.4, 2min), reversed', 170) 

    #IDS_VGS(5, 30, 16, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Block_Erase_Chip05_Col30_Ids_Vgs_VAsource_VBdrain_00', '../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col30_Ids_Vgs_VAsource_VBdrain_01'], ['g', 'y'], '../plot/chip05/', 'BlockEraseVSVBVD2p4-2min-01_StressVG2p4VD1p0-5sec-02_IDS-VGS_VaS-VbD_', hci_row_idx, '1st Erase (VS=VB=VD=2.4, 2min) vs 2nd Stress (VG=2.4, VD=1.0, 5sec), forward', 170) 
    #IDS_VGS(5, 30, 16, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Block_Erase_Chip05_Col30_Ids_Vgs_VAdrain_VBsource_00', '../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col30_Ids_Vgs_VAdrain_VBsource_01'], ['g', 'y'], '../plot/chip05/', 'BlockEraseVSVBVD2p4-2min-01_StressVG2p4VD1p0-5sec-02_IDS-VGS_VaD-VbS_', hci_row_idx, '1st Erase (VS=VB=VD=2.4, 2min) vs 2nd Stress (VG=2.4, VD=1.0, 5sec), reversed', 170) 

    #IDS_VGS(5, 30, 16, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Block_Erase_Chip05_Col30_Ids_Vgs_VAsource_VBdrain_00', '../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col30_Ids_Vgs_VAsource_VBdrain_01'], ['g', 'y'], '../plot/chip05/', 'BlockEraseVSVBVD2p4-2min-01_StressVG2p4VD0-pbti5sec-02_IDS-VGS_VaS-VbD_', pbti_row_idx, '1st Erase (VS=VB=VD=2.4, 2min) vs 2nd Stress (VG=2.4, VD=0, 5sec), forward', 170) 
    #IDS_VGS(5, 30, 16, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Block_Erase_Chip05_Col30_Ids_Vgs_VAdrain_VBsource_00', '../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col30_Ids_Vgs_VAdrain_VBsource_01'], ['g', 'y'], '../plot/chip05/', 'BlockEraseVSVBVD2p4-2min-01_StressVG2p4VD0-pbti5sec-02_IDS-VGS_VaD-VbS_', pbti_row_idx, '1st Erase (VS=VB=VD=2.4, 2min) vs 2nd Stress (VG=2.4, VD=0, 5sec), reversed', 170) 

    #IDS_VGS(5, 30, 16, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col30_Ids_Vgs_VAsource_VBdrain_01', '../data/Stacked_VG_Erase_Chip05/Block_Erase_Chip05_Col30_Ids_Vgs_VAsource_VBdrain_01'], ['y', 'm'], '../plot/chip05/', 'StressVG2p4VD1p0-5sec-02_BlockEraseVSVBVD2p4-2min-02_IDS-VGS_VaS-VbD_', hci_row_idx, '2nd Stress (VG=2.4, VD=1.0, 5sec) vs 2nd Erase (VS=VB=VD=2.4, 2min), forward', 170) 
    #IDS_VGS(5, 30, 16, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col30_Ids_Vgs_VAdrain_VBsource_01', '../data/Stacked_VG_Erase_Chip05/Block_Erase_Chip05_Col30_Ids_Vgs_VAdrain_VBsource_01'], ['y', 'm'], '../plot/chip05/', 'StressVG2p4VD1p0-5sec-02_BlockEraseVSVBVD2p4-2min-02_IDS-VGS_VaD-VbS_', hci_row_idx, '2nd Stress (VG=2.4, VD=1.0, 5sec) vs 2nd Erase (VS=VB=VD=2.4, 2min), reversed', 170) 

    #IDS_VGS(5, 30, 16, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col30_Ids_Vgs_VAsource_VBdrain_01', '../data/Stacked_VG_Erase_Chip05/Block_Erase_Chip05_Col30_Ids_Vgs_VAsource_VBdrain_01'], ['y', 'm'], '../plot/chip05/', 'StressVG2p4VD0-pbti5sec-02_BlockEraseVSVBVD2p4-2min-02_IDS-VGS_VaS-VbD_', pbti_row_idx, '2nd Stress (VG=2.4, VD=0, 5sec) vs 2nd Erase (VS=VB=VD=2.4, 2min), forward', 170) 
    #IDS_VGS(5, 30, 16, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col30_Ids_Vgs_VAdrain_VBsource_01', '../data/Stacked_VG_Erase_Chip05/Block_Erase_Chip05_Col30_Ids_Vgs_VAdrain_VBsource_01'], ['y', 'm'], '../plot/chip05/', 'StressVG2p4VD0-pbti5sec-02_BlockEraseVSVBVD2p4-2min-02_IDS-VGS_VaD-VbS_', pbti_row_idx, '2nd Stress (VG=2.4, VD=0, 5sec) vs 2nd Erase (VS=VB=VD=2.4, 2min), reversed', 170) 

    IDS_VGS(5, 18, 36, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Fresh_Chip05_Col18_Ids_Vgs_VAsource_VBdrain', '../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col18_Ids_Vgs_VAsource_VBdrain_00'], ['b', 'r'], '../plot/chip05/', 'Fresh_StressVG2p4VD1p0-5sec-01_IDS-VGS_VaS-VbD_', hci_row_idx, 'Fresh vs 1st Stress, VG=2.4, VD=1.0, 5sec, forward', 125) 
    IDS_VGS(5, 18, 36, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Fresh_Chip05_Col18_Ids_Vgs_VAdrain_VBsource', '../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col18_Ids_Vgs_VAdrain_VBsource_00'], ['b', 'r'], '../plot/chip05/', 'Fresh_StressVG2p4VD1p0-5sec-01_IDS-VGS_VaD-VbS_', hci_row_idx, 'Fresh vs 1st Stress, VG=2.4, VD=1.0, 5sec, reversed', 125) 

    IDS_VGS(5, 18, 36, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Fresh_Chip05_Col18_Ids_Vgs_VAsource_VBdrain', '../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col18_Ids_Vgs_VAsource_VBdrain_00'], ['b', 'r'], '../plot/chip05/', 'Fresh_StressVG2p4VD0-pbti5sec-01_IDS-VGS_VaS-VbD_', pbti_row_idx, 'Fresh vs 1st Stress, VG=2.4, VD=0, 5sec, forward', 125) 
    IDS_VGS(5, 18, 36, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Fresh_Chip05_Col18_Ids_Vgs_VAdrain_VBsource', '../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col18_Ids_Vgs_VAdrain_VBsource_00'], ['b', 'r'], '../plot/chip05/', 'Fresh_StressVG2p4VD0-pbti5sec-01_IDS-VGS_VaD-VbS_', pbti_row_idx, 'Fresh vs 1st Stress, VG=2.4, VD=0, 5sec, reversed', 125) 

    IDS_VGS(5, 18, 36, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col18_Ids_Vgs_VAsource_VBdrain_00', '../data/Stacked_VG_Erase_Chip05/Block_Erase_Chip05_Col18_Ids_Vgs_VAsource_VBdrain_00'], ['r', 'g'], '../plot/chip05/', 'StressVG2p4VD1p0-5sec-01_BlockEraseVSVBVD2p4-2min-01_IDS-VGS_VaS-VbD_', hci_row_idx, '1st Stress (VG=2.4, VD=1.0, 5sec) vs 1st Erase (VS=VB=VD=2.4, 2min), forward', 125) 
    IDS_VGS(5, 18, 36, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col18_Ids_Vgs_VAdrain_VBsource_00', '../data/Stacked_VG_Erase_Chip05/Block_Erase_Chip05_Col18_Ids_Vgs_VAdrain_VBsource_00'], ['r', 'g'], '../plot/chip05/', 'StressVG2p4VD1p0-5sec-01_BlockEraseVSVBVD2p4-2min-01_IDS-VGS_VaD-VbS_', hci_row_idx, '1st Stress (VG=2.4, VD=1.0, 5sec) vs 1st Erase (VS=VB=VD=2.4, 2min), reversed', 125) 

    IDS_VGS(5, 18, 36, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col18_Ids_Vgs_VAsource_VBdrain_00', '../data/Stacked_VG_Erase_Chip05/Block_Erase_Chip05_Col18_Ids_Vgs_VAsource_VBdrain_00'], ['r', 'g'], '../plot/chip05/', 'StressVG2p4VD0-pbti5sec-01_BlockEraseVSVBVD2p4-2min-01_IDS-VGS_VaS-VbD_', pbti_row_idx, '1st Stress (VG=2.4, VD=0, 5sec) vs 1st Erase (VS=VB=VD=2.4, 2min), forward', 125) 
    IDS_VGS(5, 18, 36, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col18_Ids_Vgs_VAdrain_VBsource_00', '../data/Stacked_VG_Erase_Chip05/Block_Erase_Chip05_Col18_Ids_Vgs_VAdrain_VBsource_00'], ['r', 'g'], '../plot/chip05/', 'StressVG2p4VD0-pbti5sec-01_BlockEraseVSVBVD2p4-2min-01_IDS-VGS_VaD-VbS_', pbti_row_idx, '1st Stress (VG=2.4, VD=0, 5sec) vs 1st Erase (VS=VB=VD=2.4, 2min), reversed', 125) 

    IDS_VGS(5, 18, 36, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Block_Erase_Chip05_Col18_Ids_Vgs_VAsource_VBdrain_00', '../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col18_Ids_Vgs_VAsource_VBdrain_01'], ['g', 'y'], '../plot/chip05/', 'BlockEraseVSVBVD2p4-2min-01_StressVG2p4VD1p0-5sec-02_IDS-VGS_VaS-VbD_', hci_row_idx, '1st Erase (VS=VB=VD=2.4, 2min) vs 2nd Stress (VG=2.4, VD=1.0, 5sec), forward', 125) 
    IDS_VGS(5, 18, 36, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Block_Erase_Chip05_Col18_Ids_Vgs_VAdrain_VBsource_00', '../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col18_Ids_Vgs_VAdrain_VBsource_01'], ['g', 'y'], '../plot/chip05/', 'BlockEraseVSVBVD2p4-2min-01_StressVG2p4VD1p0-5sec-02_IDS-VGS_VaD-VbS_', hci_row_idx, '1st Erase (VS=VB=VD=2.4, 2min) vs 2nd Stress (VG=2.4, VD=1.0, 5sec), reversed', 125) 

    IDS_VGS(5, 18, 36, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Block_Erase_Chip05_Col18_Ids_Vgs_VAsource_VBdrain_00', '../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col18_Ids_Vgs_VAsource_VBdrain_01'], ['g', 'y'], '../plot/chip05/', 'BlockEraseVSVBVD2p4-2min-01_StressVG2p4VD0-pbti5sec-02_IDS-VGS_VaS-VbD_', pbti_row_idx, '1st Erase (VS=VB=VD=2.4, 2min) vs 2nd Stress (VG=2.4, VD=0, 5sec), forward', 125) 
    IDS_VGS(5, 18, 36, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Block_Erase_Chip05_Col18_Ids_Vgs_VAdrain_VBsource_00', '../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col18_Ids_Vgs_VAdrain_VBsource_01'], ['g', 'y'], '../plot/chip05/', 'BlockEraseVSVBVD2p4-2min-01_StressVG2p4VD0-pbti5sec-02_IDS-VGS_VaD-VbS_', pbti_row_idx, '1st Erase (VS=VB=VD=2.4, 2min) vs 2nd Stress (VG=2.4, VD=0, 5sec), reversed', 125) 

    IDS_VGS(5, 18, 36, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col18_Ids_Vgs_VAsource_VBdrain_01', '../data/Stacked_VG_Erase_Chip05/Block_Erase_Chip05_Col18_Ids_Vgs_VAsource_VBdrain_01'], ['y', 'm'], '../plot/chip05/', 'StressVG2p4VD1p0-5sec-02_BlockEraseVSVBVD2p4-2min-02_IDS-VGS_VaS-VbD_', hci_row_idx, '2nd Stress (VG=2.4, VD=1.0, 5sec) vs 2nd Erase (VS=VB=VD=2.4, 2min), forward', 125) 
    IDS_VGS(5, 18, 36, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col18_Ids_Vgs_VAdrain_VBsource_01', '../data/Stacked_VG_Erase_Chip05/Block_Erase_Chip05_Col18_Ids_Vgs_VAdrain_VBsource_01'], ['y', 'm'], '../plot/chip05/', 'StressVG2p4VD1p0-5sec-02_BlockEraseVSVBVD2p4-2min-02_IDS-VGS_VaD-VbS_', hci_row_idx, '2nd Stress (VG=2.4, VD=1.0, 5sec) vs 2nd Erase (VS=VB=VD=2.4, 2min), reversed', 125) 

    IDS_VGS(5, 18, 36, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col18_Ids_Vgs_VAsource_VBdrain_01', '../data/Stacked_VG_Erase_Chip05/Block_Erase_Chip05_Col18_Ids_Vgs_VAsource_VBdrain_01'], ['y', 'm'], '../plot/chip05/', 'StressVG2p4VD0-pbti5sec-02_BlockEraseVSVBVD2p4-2min-02_IDS-VGS_VaS-VbD_', pbti_row_idx, '2nd Stress (VG=2.4, VD=0, 5sec) vs 2nd Erase (VS=VB=VD=2.4, 2min), forward', 125) 
    IDS_VGS(5, 18, 36, 2, 'ULVT', 32, ['../data/Stacked_VG_Erase_Chip05/Stacked_VG_Chip05_Col18_Ids_Vgs_VAdrain_VBsource_01', '../data/Stacked_VG_Erase_Chip05/Block_Erase_Chip05_Col18_Ids_Vgs_VAdrain_VBsource_01'], ['y', 'm'], '../plot/chip05/', 'StressVG2p4VD0-pbti5sec-02_BlockEraseVSVBVD2p4-2min-02_IDS-VGS_VaD-VbS_', pbti_row_idx, '2nd Stress (VG=2.4, VD=0, 5sec) vs 2nd Erase (VS=VB=VD=2.4, 2min), reversed', 125) 

if __name__ == '__main__':
  main()
    
