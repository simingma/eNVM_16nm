
import sys
import re
import os
import commands
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
from scipy.interpolate import interp1d

def IDSAT(chip, col, L, Nfin, VT_flavor, VG, VD, Nrow, Nhci, Npbti, 
        write_time=10, pulse_length=0.04, PulseCycle=3, figN = 1, 
        t = [0, 0.2, 0.4, 0.5, 0.6, 0.8, 1.0, 1.1, 1.2, 1.4, 1.6, 1.7],
        t_label = ['0', '0.2', '0.4', 'recovery\n~minutes', '0', '0.2', '0.4', 'recovery\n~minutes', '0', '0.2', '0.4', 'recovery\n~minutes'], path_data='', path_plot='', Imin=0, Imax=0):

    """ Plot IDSAT shifts over constant VG stress
    apply write_time of pulse_length VG pulse consecutively, with quick IDSAT measurments inserted in between pulses
    after these, rest for >5 minutes of recovery, followed by IDSAT and full IDS-VGS measurement
    cycle this procedure for PulseCycle time.

    VD applied to Nhci cells, followed by Npbti cells VD=0 (FN-tunneling or so-called PBTI CVS)
    cycled until the end of Nrow. """

# missing: it's more rigorous to extract OFF_leakages data and subtracted

    if path_data == '':
        path_data = '../data/VG_ConstPulse_chip'+str(chip).zfill(2)+'/'
    if path_plot == '':
        path_plot = '../plot/VG_ConstPulse_chip'+str(chip).zfill(2)+'_IDSAT/'
    if os.path.isdir(path_plot) == False:
        os.mkdir(path_plot)

    for (meas_dir, direction) in [('forward', 'VAsource_VBdrain'), ('reversed', 'VAdrain_VBsource')]:
        IDSAT = np.zeros((Nrow, PulseCycle*(1+1+write_time+1))) 
        #initially all IDSAT, then cell-by-cell 'fresh'+stressing IDSAT, finally recovery all IDSAT
        for cycle in np.arange(0, PulseCycle):
            IDSAT_all = []
            # Only one file: I always stress in VAsource_VBdrain direction, 
            # and measurements in both directions are all in this one file
            f = open(path_data+'Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+'_stress_VG_ConstPulse_VAsource_VBdrain_'+str(cycle+1).zfill(2),'rU')
            IDSAT_all.append(re.findall(r'_IDSAT_WL\[\d+\]_'+direction+'=(-*\d*\.\d+)',f.read()))
            f.close()

            if (len(IDSAT_all[0]) != Nrow*(1+1+write_time+1)):
                print('data file error!\ngrabed '+str(len(IDSAT_all[0]))+' IDSAT,\nbut expecting '+str(Nrow*(1+1+write_time+1))+' data\n')

            for row in np.arange(0, Nrow):
                IDSAT[row][cycle*(1+1+write_time+1)+0] = np.float64(IDSAT_all[0][row])
                IDSAT[row][cycle*(1+1+write_time+1)+1: (cycle+1)*(1+1+write_time+1)-1] = np.float64(IDSAT_all[0][Nrow+row*(1+write_time): Nrow+(row+1)*(1+write_time)])
                IDSAT[row][(cycle+1)*(1+1+write_time+1)-1] = np.float64(IDSAT_all[0][Nrow+Nrow*(1+write_time)+row])
        
        if (Imin < 0.01) and (Imax < 0.01):
            Imin = 1e6*np.amin(IDSAT)
            Imax = 1e6*np.amax(IDSAT)

        #plt.figure(figN)
        plt.title('L='+str(L)+', Nfin='+str(Nfin)+', '+VT_flavor+'\nIDSAT vs stress time, VGS='+str(VG)+'V, VDS='+str(VD)+'V\nIDSAT measured at VGS=0.8, VDS=0.8, '+meas_dir+' direction', fontsize=10)
        for cycle in np.arange(PulseCycle):
            row = 0
            while(row < Nrow):
                #print(row, Nrow)
                for hci in np.arange(0, Nhci):
                    HCI_fig, = plt.plot(cycle*0.2+np.append(np.arange(write_time*pulse_length*cycle, write_time*pulse_length*(cycle+1)+0.0001, pulse_length), np.array([write_time*pulse_length*(cycle+1)+0.1])), 1e6*IDSAT[row][cycle*(1+1+write_time+1)+1: (cycle+1)*(1+1+write_time+1)], color='r', linestyle='solid', marker='.')
                    row = row + 1
                for pbti in np.arange(0, Npbti):
                    PBTI_fig, = plt.plot(cycle*0.2+np.append(np.arange(write_time*pulse_length*cycle, write_time*pulse_length*(cycle+1)+0.0001, pulse_length), np.array([write_time*pulse_length*(cycle+1)+0.1])), 1e6*IDSAT[row][cycle*(1+1+write_time+1)+1: (cycle+1)*(1+1+write_time+1)], color='y', linestyle='solid', marker='*')
                    row = row + 1
        plt.xticks(t, t_label, rotation=30, fontsize=9)
        plt.ylim(Imin, Imax)
        plt.xlim(0, t[-1])
        plt.grid()
        plt.legend([HCI_fig, PBTI_fig], ['HCI', 'PBTI: VDS=0'], loc = 'best')
        plt.xlabel('time (sec)')
        plt.ylabel('IDSAT (uA)')
        plt.subplots_adjust(bottom=0.15)
        plt.savefig(path_plot+'Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+'_'+meas_dir+'.pdf')
        #plt.savefig(path_plot+'IDSAT_'+str(figN).zfill(3)+'_Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+'_'+meas_dir+'.pdf')
        plt.close()
        #figN = figN+1

if __name__ == '__main__':
  main()

