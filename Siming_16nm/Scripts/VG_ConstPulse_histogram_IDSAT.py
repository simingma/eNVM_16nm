
import sys
import re
import os
import commands
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
#from scipy.interpolate import interp1d

def main():

    VD_values = 4 
    # VD applied to 10 transistors, followed by 2 transistors VD=0 (FN-tunneling or so-called PBTI CVS)
    # (10+2)*3=36
    write_time = 10
    pulse_length = 0.04
    # measure thourough I-V curves 10*0.04s pulses = 0.4s 

    PulseCycle = 3 # Cycled for 3 times: in total 3*0.4s = 1.2s 

    #WL = ['100/30','210/30','100/60','210/60','100/90','210/90']
    #VT_FLAVOR = ['SVT','LVT','ULVT']

    chip = 1
    vd_column = [(2.0, 20)]
    path_data = '../data/VG_ConstPulse_chip01/'
    path_plot = '../plot/VG_ConstPulse_chip01_histogram_IDSAT/'
    if os.path.isdir(path_plot) == False:
        os.mkdir(path_plot)

    figN = 1
    for (VD, col) in vd_column:
        Num_of_row = 32
        IDSAT_VAsource_VBdrain = np.zeros((Num_of_row, PulseCycle*(1+1+write_time+1)))
        IDSAT_VAdrain_VBsource = np.zeros((Num_of_row, PulseCycle*(1+1+write_time+1)))

        for cycle in np.arange(0, PulseCycle):
            IDSAT_VAsource_VBdrain_all = []
            IDSAT_VAdrain_VBsource_all = []
            f0=open(path_data+'Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+'_stress_VG_ConstPulse_VAsource_VBdrain_'+str(cycle+1).zfill(2),'rU')
            IDSAT_VAsource_VBdrain_all.append(re.findall(r'_IDSAT_WL\[\d+\]_VAsource_VBdrain=(-*\d*\.\d+)',f0.read()))
            f0.close()
            f0=open(path_data+'Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+'_stress_VG_ConstPulse_VAsource_VBdrain_'+str(cycle+1).zfill(2),'rU')
            IDSAT_VAdrain_VBsource_all.append(re.findall(r'_IDSAT_WL\[\d+\]_VAdrain_VBsource=(-*\d*\.\d+)',f0.read()))
            f0.close()

            print(len(IDSAT_VAsource_VBdrain_all), len(IDSAT_VAsource_VBdrain_all[0])) # should be (1, Num_of_row*(1+(1+write_time)+1)) = (1, 416)
            print(len(IDSAT_VAdrain_VBsource_all), len(IDSAT_VAdrain_VBsource_all[0])) # should be (1, Num_of_row*(1+(1+write_time)+1)) = (1, 416)

#TODO: figure out the resolution/bits of the floating point current variable!!!
            for row in np.arange(0, Num_of_row):
                IDSAT_VAsource_VBdrain[row][cycle*(1+write_time+1+1)+0] = np.float64(IDSAT_VAsource_VBdrain_all[0][row])
                IDSAT_VAsource_VBdrain[row][cycle*(1+write_time+1+1)+1: (cycle+1)*(1+write_time+1+1)-1] = np.float64(IDSAT_VAsource_VBdrain_all[0][Num_of_row+row*(write_time+1): Num_of_row+(row+1)*(write_time+1)])
                IDSAT_VAsource_VBdrain[row][(cycle+1)*(1+write_time+1+1)-1] = np.float64(IDSAT_VAsource_VBdrain_all[0][Num_of_row+Num_of_row*(write_time+1)+row])
                IDSAT_VAdrain_VBsource[row][cycle*(1+write_time+1+1)+0] = np.float64(IDSAT_VAdrain_VBsource_all[0][row])
                IDSAT_VAdrain_VBsource[row][cycle*(1+write_time+1+1)+1: (cycle+1)*(1+write_time+1+1)-1] = np.float64(IDSAT_VAdrain_VBsource_all[0][Num_of_row+row*(write_time+1): Num_of_row+(row+1)*(write_time+1)])
                IDSAT_VAdrain_VBsource[row][(cycle+1)*(1+write_time+1+1)-1] = np.float64(IDSAT_VAdrain_VBsource_all[0][Num_of_row+Num_of_row*(write_time+1)+row])

        #Imin = 1e6*(np.amin(np.amin(IDSAT_VAsource_VBdrain[col]), np.amin(IDSAT_VAdrain_VBsource[col])) - 20e-6)
        #Imax = 1e6*(np.amax(np.amax(IDSAT_VAsource_VBdrain[col]), np.amax(IDSAT_VAdrain_VBsource[col])) + 2e-6)

        time_index = [11, 12, 24, 25, 37, 38]
        time_label = ['400ms, measured immediately', '400ms, short-term recovery', '800ms, measured immediately', '800ms, short-term recovery', '1200ms, measured immediately', '1200ms, short-term recovery']

        Num_bins = 15

        plt.figure(400)
        n0, bins0, patches0 = plt.hist(1e6*np.append(IDSAT_VAsource_VBdrain[0: 7, 1], np.append(IDSAT_VAsource_VBdrain[8: 15, 1], np.append(IDSAT_VAsource_VBdrain[16: 23, 1], IDSAT_VAsource_VBdrain[24: 31, 1]))), bins=Num_bins, normed = True)
        n1, bins1, patches1 = plt.hist(1e6*np.append(IDSAT_VAsource_VBdrain[0: 7, time_index[0]], np.append(IDSAT_VAsource_VBdrain[8: 15, time_index[0]], np.append(IDSAT_VAsource_VBdrain[16: 23, time_index[0]], IDSAT_VAsource_VBdrain[24: 31, time_index[0]]))), bins=Num_bins, normed = True)
        n2, bins2, patches2 = plt.hist(1e6*np.append(IDSAT_VAsource_VBdrain[0: 7, time_index[1]], np.append(IDSAT_VAsource_VBdrain[8: 15, time_index[1]], np.append(IDSAT_VAsource_VBdrain[16: 23, time_index[1]], IDSAT_VAsource_VBdrain[24: 31, time_index[1]]))), bins=Num_bins, normed = True)
        n3, bins3, patches3 = plt.hist(1e6*np.append(IDSAT_VAsource_VBdrain[0: 7, time_index[2]], np.append(IDSAT_VAsource_VBdrain[8: 15, time_index[2]], np.append(IDSAT_VAsource_VBdrain[16: 23, time_index[2]], IDSAT_VAsource_VBdrain[24: 31, time_index[2]]))), bins=Num_bins, normed = True)
        n4, bins4, patches4 = plt.hist(1e6*np.append(IDSAT_VAsource_VBdrain[0: 7, time_index[3]], np.append(IDSAT_VAsource_VBdrain[8: 15, time_index[3]], np.append(IDSAT_VAsource_VBdrain[16: 23, time_index[3]], IDSAT_VAsource_VBdrain[24: 31, time_index[3]]))), bins=Num_bins, normed = True)
        n5, bins5, patches5 = plt.hist(1e6*np.append(IDSAT_VAsource_VBdrain[0: 7, time_index[4]], np.append(IDSAT_VAsource_VBdrain[8: 15, time_index[4]], np.append(IDSAT_VAsource_VBdrain[16: 23, time_index[4]], IDSAT_VAsource_VBdrain[24: 31, time_index[4]]))), bins=Num_bins, normed = True)
        
        n6, bins6, patches6 = plt.hist(1e6*np.append(IDSAT_VAsource_VBdrain[0: 7, time_index[5]], np.append(IDSAT_VAsource_VBdrain[8: 15, time_index[5]], np.append(IDSAT_VAsource_VBdrain[16: 23, time_index[5]], IDSAT_VAsource_VBdrain[24: 31, time_index[5]]))), bins=Num_bins, normed = True)

        Ymax = np.amax([np.amax(n0), np.amax(n1), np.amax(n2), np.amax(n3), np.amax(n4), np.amax(n5), np.amax(n6)])
        Imax = np.amax([bins0[-1], bins1[-1], bins2[-1],bins3[-1], bins4[-1], bins5[-1], bins6[-1]])
        Imin = np.amin([bins0[0], bins1[0], bins2[0], bins3[0], bins4[0], bins5[0], bins6[0]])

        figN = 1
        plt.figure(figN)
        plt.title('col['+str(col)+'], VDS='+str(VD)+', VGS=2\nIDSAT distribution of 28 fresh cells\nmeasured at VGS=0.8, VDS=0.8, stress(forward) direction', fontsize=10)
        IDSAT_fresh = np.append(IDSAT_VAsource_VBdrain[0: 7, 1], np.append(IDSAT_VAsource_VBdrain[8: 15, 1], np.append(IDSAT_VAsource_VBdrain[16: 23, 1], IDSAT_VAsource_VBdrain[24: 31, 1])))
        plt.hist(1e6*IDSAT_fresh, bins=Num_bins, normed = True, label='fresh', alpha=1, color = 'b')
        plt.legend(loc='upper center')
        plt.grid()
        plt.xlim(Imin, Imax)
        plt.ylim(0, Ymax)
        plt.xlabel('IDSAT (uA)')
        plt.ylabel('proportion out of 28 cells')
        plt.savefig(path_plot+'HIST_IDSAT_'+str(figN).zfill(3)+'_Col'+str(col).zfill(2)+'_forward.pdf')
        figN = figN+1

        for time in np.arange(2*PulseCycle):
            plt.figure(figN)
            plt.title('col['+str(col)+'], VDS='+str(VD)+', VGS=2\nIDSAT fresh vs stressed for '+time_label[time]+'\nmeasured at VGS=0.8, VDS=0.8, stress(forward) direction', fontsize=10)
            IDSAT_fresh = np.append(IDSAT_VAsource_VBdrain[0: 7, 1], np.append(IDSAT_VAsource_VBdrain[8: 15, 1], np.append(IDSAT_VAsource_VBdrain[16: 23, 1], IDSAT_VAsource_VBdrain[24: 31, 1])))
            IDSAT_at_time = np.append(IDSAT_VAsource_VBdrain[0: 7, time_index[time]], np.append(IDSAT_VAsource_VBdrain[8: 15, time_index[time]], np.append(IDSAT_VAsource_VBdrain[16: 23, time_index[time]], IDSAT_VAsource_VBdrain[24: 31, time_index[time]])))
            #n, bins, patches = plt.hist([IDSAT_fresh, IDSAT_at_time], bins=7, label=['fresh', 'stressed for '+time_label[time]], color = ['b', 'r'])
            plt.hist(1e6*IDSAT_fresh, bins=Num_bins, normed = True, label='fresh', alpha=0.4, color = 'b')
            plt.hist(1e6*IDSAT_at_time, bins=Num_bins, normed = True, label='stressed', alpha=0.6, color = 'r')
            plt.legend(loc='upper center')
            plt.grid()
            plt.xlim(Imin, Imax)
            plt.ylim(0, Ymax)
            # plt.legend([HCI, PBTI], ['HCI: VDS='+str(VDS[col][VT*36]), 'PBTI: VDS=0'], loc = 'best')
            plt.xlabel('IDSAT (uA)')
            plt.ylabel('proportion out of 28 cells')
            plt.savefig(path_plot+'HIST_IDSAT_'+str(figN).zfill(3)+'_Col'+str(col).zfill(2)+'_forward.pdf')
            figN = figN+1

        plt.figure(400)
        n0, bins0, patches0 = plt.hist(1e6*np.append(IDSAT_VAdrain_VBsource[0: 7, 1], np.append(IDSAT_VAdrain_VBsource[8: 15, 1], np.append(IDSAT_VAdrain_VBsource[16: 23, 1], IDSAT_VAdrain_VBsource[24: 31, 1]))), bins=Num_bins, normed = True)
        n1, bins1, patches1 = plt.hist(1e6*np.append(IDSAT_VAdrain_VBsource[0: 7, time_index[0]], np.append(IDSAT_VAdrain_VBsource[8: 15, time_index[0]], np.append(IDSAT_VAdrain_VBsource[16: 23, time_index[0]], IDSAT_VAdrain_VBsource[24: 31, time_index[0]]))), bins=Num_bins, normed = True)
        n2, bins2, patches2 = plt.hist(1e6*np.append(IDSAT_VAdrain_VBsource[0: 7, time_index[1]], np.append(IDSAT_VAdrain_VBsource[8: 15, time_index[1]], np.append(IDSAT_VAdrain_VBsource[16: 23, time_index[1]], IDSAT_VAdrain_VBsource[24: 31, time_index[1]]))), bins=Num_bins, normed = True)
        n3, bins3, patches3 = plt.hist(1e6*np.append(IDSAT_VAdrain_VBsource[0: 7, time_index[2]], np.append(IDSAT_VAdrain_VBsource[8: 15, time_index[2]], np.append(IDSAT_VAdrain_VBsource[16: 23, time_index[2]], IDSAT_VAdrain_VBsource[24: 31, time_index[2]]))), bins=Num_bins, normed = True)
        n4, bins4, patches4 = plt.hist(1e6*np.append(IDSAT_VAdrain_VBsource[0: 7, time_index[3]], np.append(IDSAT_VAdrain_VBsource[8: 15, time_index[3]], np.append(IDSAT_VAdrain_VBsource[16: 23, time_index[3]], IDSAT_VAdrain_VBsource[24: 31, time_index[3]]))), bins=Num_bins, normed = True)
        n5, bins5, patches5 = plt.hist(1e6*np.append(IDSAT_VAdrain_VBsource[0: 7, time_index[4]], np.append(IDSAT_VAdrain_VBsource[8: 15, time_index[4]], np.append(IDSAT_VAdrain_VBsource[16: 23, time_index[4]], IDSAT_VAdrain_VBsource[24: 31, time_index[4]]))), bins=Num_bins, normed = True)
        n6, bins6, patches6 = plt.hist(1e6*np.append(IDSAT_VAdrain_VBsource[0: 7, time_index[5]], np.append(IDSAT_VAdrain_VBsource[8: 15, time_index[5]], np.append(IDSAT_VAdrain_VBsource[16: 23, time_index[5]], IDSAT_VAdrain_VBsource[24: 31, time_index[5]]))), bins=Num_bins, normed = True)

        Ymax = np.amax([np.amax(n0), np.amax(n1), np.amax(n2), np.amax(n3), np.amax(n4), np.amax(n5), np.amax(n6)])
        Imax = np.amax([bins0[-1], bins1[-1], bins2[-1],bins3[-1], bins4[-1], bins5[-1], bins6[-1]])
        Imin = np.amin([bins0[0], bins1[0], bins2[0], bins3[0], bins4[0], bins5[0], bins6[0]])

        plt.figure(figN)
        plt.title('col['+str(col)+'], VDS='+str(VD)+', VGS=2\nIDSAT distribution of 28 fresh cells\nmeasured at VGS=0.8, VDS=0.8, reversed direction', fontsize=10)
        IDSAT_fresh = np.append(IDSAT_VAdrain_VBsource[0: 7, 1], np.append(IDSAT_VAdrain_VBsource[8: 15, 1], np.append(IDSAT_VAdrain_VBsource[16: 23, 1], IDSAT_VAdrain_VBsource[24: 31, 1])))
        plt.hist(1e6*IDSAT_fresh, bins=Num_bins, normed = True, label='fresh', alpha=1, color = 'b')
        plt.legend(loc='upper center')
        plt.grid()
        plt.xlim(Imin, Imax)
        plt.ylim(0, Ymax)
        plt.xlabel('IDSAT (uA)')
        plt.ylabel('proportion out of 28 cells')
        plt.savefig(path_plot+'HIST_IDSAT_'+str(figN).zfill(3)+'_Col'+str(col).zfill(2)+'_reversed.pdf')
        figN = figN+1

        for time in np.arange(2*PulseCycle):
            plt.figure(figN)
            plt.title('col['+str(col)+'], VDS='+str(VD)+', VGS=2\nIDSAT fresh vs stressed for '+time_label[time]+'\nmeasured at VGS=0.8, VDS=0.8, reversed direction', fontsize=10)
            IDSAT_fresh = np.append(IDSAT_VAdrain_VBsource[0: 7, 1], np.append(IDSAT_VAdrain_VBsource[8: 15, 1], np.append(IDSAT_VAdrain_VBsource[16: 23, 1], IDSAT_VAdrain_VBsource[24: 31, 1])))
            IDSAT_at_time = np.append(IDSAT_VAdrain_VBsource[0: 7, time_index[time]], np.append(IDSAT_VAdrain_VBsource[8: 15, time_index[time]], np.append(IDSAT_VAdrain_VBsource[16: 23, time_index[time]], IDSAT_VAdrain_VBsource[24: 31, time_index[time]])))
            #n, bins, patches = plt.hist([IDSAT_fresh, IDSAT_at_time], bins=7, label=['fresh', 'stressed for '+time_label[time]], color = ['b', 'r'])
            plt.hist(1e6*IDSAT_fresh, bins=Num_bins, normed = True, label='fresh', alpha=0.4, color = 'b')
            plt.hist(1e6*IDSAT_at_time, bins=Num_bins, normed = True, label='stressed', alpha=0.6, color = 'r')
            plt.legend(loc='upper center')
            plt.grid()
            plt.xlim(Imin, Imax)
            plt.ylim(0, Ymax)
            # plt.legend([HCI, PBTI], ['HCI: VDS='+str(VDS[col][VT*36]), 'PBTI: VDS=0'], loc = 'best')
            plt.xlabel('IDSAT (uA)')
            plt.ylabel('proportion out of 28 cells')
            plt.savefig(path_plot+'HIST_IDSAT_'+str(figN).zfill(3)+'_Col'+str(col).zfill(2)+'_reversed.pdf')
            figN = figN+1


if __name__ == '__main__':
  main()

