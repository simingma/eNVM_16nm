
import sys
import re
import os
import commands
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
from scipy.interpolate import interp1d

def IDSAT_horizontal_hist(chip, col, L, Nfin, VT_flavor, VG, VD, Nrow, Nhci, Npbti, 
        write_time=10, pulse_length=0.04, PulseCycle=3, figN = 1, 
        t_index = [1, 11, 12, 24, 25, 37, 38], 
        time_label = ['fresh', '400ms', 'recover', '800ms', 'recover', '1200ms', 'recover'],
        Num_bins = 50):

    path_data = '../data/VG_ConstPulse_chip'+str(chip).zfill(2)+'/'
    path_plot = '../plot/VG_ConstPulse_chip'+str(chip).zfill(2)+'_horizontal_hist/'
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

        hci_index = []
        row = 0
        while(row < Nrow):
            hci_index += range(row, row+Nhci)
            row += Nhci+Npbti
        print(len(hci_index))
#        n = []
#        bins = []
#        patches = []
#        for t in t_index:
#            n_, bins_, patches_ = plt.hist(1e6*IDSAT[hci_index, [t]*len(hci_index)], bins=Num_bins, normed = True)
#            n.append(n_)
#            bins.append(bins_)
#            patches.append(patches_)
#        Nmax = np.amax(np.array(n))
#        Imax = np.amax(np.array(bins)[:,-1])
#        Imin = np.amin(np.array(bins)[:,0])
        Imin = np.amin(1e6*IDSAT)
        Imax = np.amax(1e6*IDSAT)
        print(Imin, Imax)

        #plt.figure(figN)
        fig, ax = plt.subplots(nrows = 1, ncols = len(t_index), sharey=True)
        #plt.title('L='+str(L)+', Nfin='+str(Nfin)+', '+VT_flavor+'\nIDSAT distributions of HCI cells, VGS='+str(VG)+'V, VDS='+str(VD)+'V\nIDSAT measured at VGS=0.8, VDS=0.8, '+meas_dir+' direction', fontsize=10)
        fig.suptitle('L='+str(L)+', Nfin='+str(Nfin)+', '+VT_flavor+'\nIDSAT distributions of HCI cells, VGS='+str(VG)+'V, VDS='+str(VD)+'V\nIDSAT measured at VGS=0.8, VDS=0.8, '+meas_dir+' direction', fontsize=11)
        #ax[3].set_title('L='+str(L)+', Nfin='+str(Nfin)+', '+VT_flavor+'\nIDSAT distributions of HCI cells, VGS='+str(VG)+'V, VDS='+str(VD)+'V\nIDSAT measured at VGS=0.8, VDS=0.8, '+meas_dir+' direction', fontsize=10)
        n_max = 0
        for (t, label, axis) in zip(t_index, time_label, ax):
            #axis.hist(1e6*IDSAT[hci_index, [t]*len(hci_index)], bins=Num_bins, normed = True, orientation = 'horizontal')
            #Num_bins = int((Imax-Imin)/1.2) #approximately 1.25uA per bin
            n, bins, patches = axis.hist(1e6*IDSAT[hci_index, [t]*len(hci_index)], bins=Num_bins, normed = False, range=(Imin, Imax), orientation = 'horizontal')
            if np.amax(n) > n_max:
                n_max = np.amax(n)
            #plt.setp(axis, xticks = [0], xticklabels= [label])
            #axis.set_xticks([0])
            #axis.set_xticklabels([label])
            #axis.grid(True)
        print(n_max)
        for (label, axis) in zip(time_label, ax):
            axis.set_xlim(0, n_max+1)
            axis.set_xticks([0])
            axis.set_xticklabels([label])
            axis.grid(True)
        plt.subplots_adjust(wspace=0)
        ax[0].set_ylabel('IDSAT (uA)')
        plt.savefig(path_plot+'Chip'+str(chip).zfill(2)+'_Col_'+str(col).zfill(2)+'_'+meas_dir+'.pdf')
        plt.close()

            
if __name__ == '__main__':
  main()

