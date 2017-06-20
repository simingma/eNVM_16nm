
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
from Charge_Pumping import Charge_Pumping

def main():

    #Charge_Pumping(7, 21, 36, 2, 'ULVT', 128, '../data/Charge_Pumping_Chip07/HCI_Chip07_Col21_60DC_60Pumping_5MHz_SweepVSVBVD_VSS_WL_0_VDD_WL_1p4_ELTM', '../plot/chip07/', 'HCI_VG1p8_VD2p0_12x0p2s_Charge_Pumping_', '', -1.9, 0, 0.1, 20, 0, 1.4, 60)
    #Charge_Pumping(7, 21, 36, 2, 'ULVT', 128, '../data/Charge_Pumping_Chip07/HCI_Chip07_Col21_60DC_60Pumping_MultiFreq_SweepVSVBVD_VSS_WL_0_VDD_WL_1p6_ELTM', '../plot/chip07/', 'HCI_VG1p8_VD2p0_12x0p2s_Charge_Pumping_VSS_WL_0_VDD_WL_1p6_MultiFreq_', '', -2.0, 0, 0.1, 21, 0, 1.6, 60, 8, [5000000, 2500000, 1000000, 100000, 10000, 1000, 100, 10], ['5MHz', '2.5MHz', '1MHz', '100kHz', '10kHz', '1kHz', '100Hz', '10Hz'], 0)
    Charge_Pumping(7, 21, 36, 2, 'ULVT', 128, '../data/Charge_Pumping_Chip07/HCI_Chip07_Col21_60DC_60Pumping_MultiFreq_SweepVSVBVD_VSS_WL_0_VDD_WL_1p6_ELTM', '../plot/chip07/', 'HCI_VG1p8_VD2p0_12x0p2s_Charge_Pumping_VSS_WL_0_VDD_WL_1p6_MultiFreq_', '', -2.0, 0, 0.1, 21, 0, 1.6, 60, 8, [5000000, 2500000, 1000000, 100000, 10000, 1000, 100, 10], ['5MHz', '2.5MHz', '1MHz', '100kHz', '10kHz', '1kHz', '100Hz', '10Hz'], 0, [0, 1, 2, 3, 4, 5, 6, 7], 5, [0, 1, 2, 3])

    hci_row_idx = range(0, 7)+range(8, 15)+range(16, 23)+range(24,31)+range(32, 39)+range(40, 47)+range(48, 55)+range(56, 63)+range(64, 71)+range(72, 79)+range(80, 87)+range(88, 95)+range(96, 103)+range(104, 111)+range(112, 119)+range(120, 127)
    pbti_row_idx = [7, 15, 23, 31, 39, 47, 55, 63, 71, 79, 87, 95, 103, 111, 119, 127]

    #IDSAT(7, 21, 36, 2, 'ULVT', 1.8, 2.0, 128, 7, 1, 12, 0.2, 1, 1, [0, 0.4, 0.8, 1.2, 1.6, 2.0, 2.4, 2.5], ['0', '0.4', '0.8', '1.2', '1.6', '2.0', '2.4', 'recovery'], '../data/Charge_Pumping_Chip07/', '../plot/chip07/', 55, 130)

    #IDS_VGS(7, 21, 36, 2, 'ULVT', 128, ['../data/Charge_Pumping_Chip07/Fresh_Chip07_Col21_Ids_Vgs_VAsource_VBdrain', '../data/Charge_Pumping_Chip07/Fresh_Chip07_Col21_Ids_Vgs_VAsource_VBdrain_01'], ['b', 'y'], '../plot/chip07/', 'Fresh_vs_AfterChargePumping_IDS-VGS_VaS-VbD_', range(0, 128), 'Fresh vs After Charge Pumping (check if degraded by pumping conditions), forward', 135) 
    #IDS_VGS(7, 21, 36, 2, 'ULVT', 128, ['../data/Charge_Pumping_Chip07/Fresh_Chip07_Col21_Ids_Vgs_VAdrain_VBsource', '../data/Charge_Pumping_Chip07/Fresh_Chip07_Col21_Ids_Vgs_VAdrain_VBsource_01'], ['b', 'y'], '../plot/chip07/', 'Fresh_vs_AfterChargePumping_IDS-VGS_VaD-VbS_', range(0, 128), 'Fresh vs After Charge Pumping (check if degraded by pumping conditions), reversed', 135) 

    #IDS_VGS(7, 21, 36, 2, 'ULVT', 128, ['../data/Charge_Pumping_Chip07/Stress_Chip07_Col21_Ids_Vgs_VAsource_VBdrain_01', '../data/Charge_Pumping_Chip07/Fresh_Chip07_Col21_Ids_Vgs_VAsource_VBdrain_01'], ['r', 'y'], '../plot/chip07/', 'Fresh-AfterChargePumping_vs_HCIstress_IDS-VGS_VaS-VbD_', hci_row_idx, 'Fresh After Charge Pumping vs stress (HCI: VG=1.8, VD=2.0, 0.2s x 12), forward', 135) 
    #IDS_VGS(7, 21, 36, 2, 'ULVT', 128, ['../data/Charge_Pumping_Chip07/Stress_Chip07_Col21_Ids_Vgs_VAdrain_VBsource_01', '../data/Charge_Pumping_Chip07/Fresh_Chip07_Col21_Ids_Vgs_VAdrain_VBsource_01'], ['r', 'y'], '../plot/chip07/', 'Fresh-AfterChargePumping_vs_HCIstress_IDS-VGS_VaD-VbS_', hci_row_idx, 'Fresh After Charge Pumping vs stress (HCI: VG=1.8, VD=2.0, 0.2s x 12), reversed', 135) 

    #IDS_VGS(7, 21, 36, 2, 'ULVT', 128, ['../data/Charge_Pumping_Chip07/Stress_Chip07_Col21_Ids_Vgs_VAsource_VBdrain_01', '../data/Charge_Pumping_Chip07/Fresh_Chip07_Col21_Ids_Vgs_VAsource_VBdrain_01'], ['r', 'y'], '../plot/chip07/', 'Fresh-AfterChargePumping_vs_PBTIstress_IDS-VGS_VaS-VbD_', pbti_row_idx, 'Fresh After Charge Pumping vs stress (PBTI: VG=1.8, VD=0, 0.2s x 12), forward', 135) 
    #IDS_VGS(7, 21, 36, 2, 'ULVT', 128, ['../data/Charge_Pumping_Chip07/Stress_Chip07_Col21_Ids_Vgs_VAdrain_VBsource_01', '../data/Charge_Pumping_Chip07/Fresh_Chip07_Col21_Ids_Vgs_VAdrain_VBsource_01'], ['r', 'y'], '../plot/chip07/', 'Fresh-AfterChargePumping_vs_PBTIstress_IDS-VGS_VaD-VbS_', pbti_row_idx, 'Fresh After Charge Pumping vs stress (PBTI: VG=1.8, VD=0, 0.2s x 12), reversed', 135) 

    #IDS_VGS(7, 21, 36, 2, 'ULVT', 128, ['../data/Charge_Pumping_Chip07/Stress_Chip07_Col21_Ids_Vgs_VAsource_VBdrain_01', '../data/Charge_Pumping_Chip07/Stress_Chip07_Col21_Ids_Vgs_VAsource_VBdrain_AfterChargePumping'], ['r', 'g'], '../plot/chip07/', 'HCIstressed_vs_AfterChargePumping_IDS-VGS_VaS-VbD_', hci_row_idx, 'Immediately after HCI stress (HCI: VG=1.8, VD=2.0, 0.2s x 12) vs After Charge Pumping, forward', 135) 
    #IDS_VGS(7, 21, 36, 2, 'ULVT', 128, ['../data/Charge_Pumping_Chip07/Stress_Chip07_Col21_Ids_Vgs_VAdrain_VBsource_01', '../data/Charge_Pumping_Chip07/Stress_Chip07_Col21_Ids_Vgs_VAdrain_VBsource_AfterChargePumping'], ['r', 'g'], '../plot/chip07/', 'HCIstressed_vs_AfterChargePumping_IDS-VGS_VaD-VbS_', hci_row_idx, 'Immediately after HCI stress (HCI: VG=1.8, VD=2.0, 0.2s x 12) vs After Charge Pumping, reversed', 135) 

    #IDS_VGS(7, 21, 36, 2, 'ULVT', 128, ['../data/Charge_Pumping_Chip07/Stress_Chip07_Col21_Ids_Vgs_VAsource_VBdrain_01', '../data/Charge_Pumping_Chip07/Stress_Chip07_Col21_Ids_Vgs_VAsource_VBdrain_AfterChargePumping'], ['r', 'g'], '../plot/chip07/', 'PBTIstressed_vs_AfterChargePumping_IDS-VGS_VaS-VbD_', pbti_row_idx, 'Immediately after PBTI stress (PBTI: VG=1.8, VD=0, 0.2s x 12) vs After Charge Pumping, forward', 135) 
    #IDS_VGS(7, 21, 36, 2, 'ULVT', 128, ['../data/Charge_Pumping_Chip07/Stress_Chip07_Col21_Ids_Vgs_VAdrain_VBsource_01', '../data/Charge_Pumping_Chip07/Stress_Chip07_Col21_Ids_Vgs_VAdrain_VBsource_AfterChargePumping'], ['r', 'g'], '../plot/chip07/', 'PBTIstressed_vs_AfterChargePumping_IDS-VGS_VaD-VbS_', pbti_row_idx, 'Immediately after PBTI stress (PBTI: VG=1.8, VD=0, 0.2s x 12) vs After Charge Pumping, reversed', 135) 

if __name__ == '__main__':
  main()
    
