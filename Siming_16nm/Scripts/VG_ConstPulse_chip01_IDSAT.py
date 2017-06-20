
import sys
import re
import os
import commands
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
#from scipy.interpolate import interp1d

def main():

    """
# extract OFF_leakages from chip02's data
    chip = 14
    column = 6
    path_data = '../data/OFF_leakages_chip14_IsubAutoRange/'

    VAB = np.arange(0, 3.2+0.0001, 0.05)
    WL = ['100/30','210/30','100/60','210/60','100/90','210/90']

    I_leak_Drain_VAsource_VBdrain  = np.zeros((column, len(VAB)))
    I_leak_Psub_VAsource_VBdrain   = np.zeros((column, len(VAB)))

    for col in np.arange(0, column):
        Isense_Drain_VAsource_VBdrain  = []
        #Isense_Source_VAsource_VBdrain = []
        Isense_Psub_VAsource_VBdrain   = []
        #Isense_PCB_VAsource_VBdrain    = []

        f0=open(path_data+'OFF_leakages_VDD_IO_3p2V_Vg0V_Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+'_VAsource_VBdrain_IsubAutoRange','rU')
        Isense_Drain_VAsource_VBdrain.append(re.findall(r'ID=(-*\d+\.\d+)',f0.read()))
        f0.close()
        f0=open(path_data+'OFF_leakages_VDD_IO_3p2V_Vg0V_Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+'_VAsource_VBdrain_IsubAutoRange','rU')
        Isense_Psub_VAsource_VBdrain.append(re.findall(r'Isub=(-*\d+\.\d+)',f0.read()))
        f0.close()

        print(len(Isense_Drain_VAsource_VBdrain), len(Isense_Drain_VAsource_VBdrain[0])) # should be (1, 3.2/0.05+1) = (1, 65)
        print(len(Isense_Psub_VAsource_VBdrain), len(Isense_Psub_VAsource_VBdrain[0])) # should be (1, 3.2/0.05+1) = (1, 65)

        for k in np.arange(0, len(VAB)):
            I_leak_Drain_VAsource_VBdrain[col][k] = np.float64(Isense_Drain_VAsource_VBdrain[0][k]) 
            I_leak_Psub_VAsource_VBdrain[col][k]  = np.float64(Isense_Psub_VAsource_VBdrain[0][k])  

    """

    VD_values = 4 
    # VD applied to 8 transistors, followed by 1 transistors VD=0 (FN-tunneling or so-called PBTI CVS)
    # (7+1)*4=36
    write_time = 10
    pulse_length = 0.04
    # measure thourough I-V curves 10*0.04s pulses = 0.4s 

    PulseCycle = 3 # Cycled for 3 times: in total 3*0.4s = 1.2s 

    #WL = ['100/30','210/30','100/60','210/60','100/90','210/90']
    #VT_FLAVOR = ['SVT','LVT','ULVT']

    chip = 1
    vd_column = [(1.5, 8), (2.0, 20)]
    path_data = '../data/VG_ConstPulse_chip01/'
    path_plot = '../plot/VG_ConstPulse_chip01_IDSAT/'
    #os.mkdir(path_plot)

    #IDSAT_VAsource_VBdrain = np.zeros((column, Num_of_row, PulseCycle*(1+1+write_time+1)))
    #IDSAT_VAdrain_VBsource = np.zeros((column, Num_of_row, PulseCycle*(1+1+write_time+1)))
    #Isub_ExtTrig = np.zeros((108,write_time))
    #ID_prog_ExtTrig = np.zeros((108,write_time))


    figN = 1
    #for col in np.arange(0, column):
    for (VD, col) in vd_column:
        Num_of_row = 32
        IDSAT_VAsource_VBdrain = np.zeros((Num_of_row, PulseCycle*(1+1+write_time+1)))
        IDSAT_VAdrain_VBsource = np.zeros((Num_of_row, PulseCycle*(1+1+write_time+1)))
        for cycle in np.arange(0, PulseCycle):
            IDSAT_VAsource_VBdrain_all = []
            IDSAT_VAdrain_VBsource_all = []
            #Isub_ExtTrig_all = []
            #ID_prog_ExtTrig_all = []

            #f0 = open(path_data+'Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+'_stress_VG_ConstPulse_VAsource_VBdrain','rU')
            #Isub_ExtTrig_all.append(re.findall(r'Stress_\d+PULSE_WL\[\d+\]_Isub=(-*\d*\.\d+)',f0.read()))
            #f0.close()
            #f0 = open(path_data+'Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+'_stress_VG_ConstPulse_VAsource_VBdrain','rU')
            #ID_prog_ExtTrig_all.append(re.findall(r'Stress_\d+PULSE_WL\[\d+\]_ID_program=(-*\d*\.\d+)',f0.read()))
            #f0.close()
            f0=open(path_data+'Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+'_stress_VG_ConstPulse_VAsource_VBdrain_'+str(cycle+1).zfill(2),'rU')
            IDSAT_VAsource_VBdrain_all.append(re.findall(r'_IDSAT_WL\[\d+\]_VAsource_VBdrain=(-*\d*\.\d+)',f0.read()))
            f0.close()
            f0=open(path_data+'Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+'_stress_VG_ConstPulse_VAsource_VBdrain_'+str(cycle+1).zfill(2),'rU')
            IDSAT_VAdrain_VBsource_all.append(re.findall(r'_IDSAT_WL\[\d+\]_VAdrain_VBsource=(-*\d*\.\d+)',f0.read()))
            f0.close()

            print(len(IDSAT_VAsource_VBdrain_all), len(IDSAT_VAsource_VBdrain_all[0])) # should be (1, Num_of_row*(1+(1+write_time)+1)) = (1, 416)
            print(len(IDSAT_VAdrain_VBsource_all), len(IDSAT_VAdrain_VBsource_all[0])) # should be (1, Num_of_row*(1+(1+write_time)+1)) = (1, 416)
            #print(len(Isub_ExtTrig_all), len(Isub_ExtTrig_all[0])) # should be (1, Num_of_row*write_time*Num_of_ExtTrig) = (1, 4320)
            #print(len(ID_prog_ExtTrig_all), len(ID_prog_ExtTrig_all[0])) # should be (1, Num_of_row*write_time*Num_of_ExtTrig) = (1, 4320)

#TODO: figure out the resolution/bits of the floating point current variable!!!
            for row in np.arange(0, Num_of_row):
                IDSAT_VAsource_VBdrain[row][cycle*(1+write_time+1+1)+0] = np.float64(IDSAT_VAsource_VBdrain_all[0][row])
                IDSAT_VAsource_VBdrain[row][cycle*(1+write_time+1+1)+1: (cycle+1)*(1+write_time+1+1)-1] = np.float64(IDSAT_VAsource_VBdrain_all[0][Num_of_row+row*(write_time+1): Num_of_row+(row+1)*(write_time+1)])
                IDSAT_VAsource_VBdrain[row][(cycle+1)*(1+write_time+1+1)-1] = np.float64(IDSAT_VAsource_VBdrain_all[0][Num_of_row+Num_of_row*(write_time+1)+row])
                IDSAT_VAdrain_VBsource[row][cycle*(1+write_time+1+1)+0] = np.float64(IDSAT_VAdrain_VBsource_all[0][row])
                IDSAT_VAdrain_VBsource[row][cycle*(1+write_time+1+1)+1: (cycle+1)*(1+write_time+1+1)-1] = np.float64(IDSAT_VAdrain_VBsource_all[0][Num_of_row+row*(write_time+1): Num_of_row+(row+1)*(write_time+1)])
                IDSAT_VAdrain_VBsource[row][(cycle+1)*(1+write_time+1+1)-1] = np.float64(IDSAT_VAdrain_VBsource_all[0][Num_of_row+Num_of_row*(write_time+1)+row])
                #Isub_ExtTrig[row]=np.float64(Isub_ExtTrig_all[0][row*write_time: (row+1)*write_time])
                #ID_prog_ExtTrig[row]=np.float64(ID_prog_ExtTrig_all[0][row*write_time: (row+1)*write_time])

        Imin = 1e6*(np.amin(np.amin(IDSAT_VAsource_VBdrain), np.amin(IDSAT_VAdrain_VBsource)) - 18e-6)
        Imax = 1e6*(np.amax(np.amax(IDSAT_VAsource_VBdrain), np.amax(IDSAT_VAdrain_VBsource)) + 3e-6)

        t = [0, 0.2, 0.4, 0.5, 0.6, 0.8, 1.0, 1.1, 1.2, 1.4, 1.6, 1.7]
        t_label = ['0', '0.2', '0.4', 'recovery\n~minutes', '0', '0.2', '0.4', 'recovery\n~minutes', '0', '0.2', '0.4', 'recovery\n~minutes']

        #for VT in np.arange(len(VT_FLAVOR)):
            #figN = (col*len(VT_FLAVOR) + VT)*10
        plt.figure(figN)
        plt.title('Col['+str(col)+'], IDSAT (10 x 40ms Pulses) x 3 Cycles, VGS=2V, VDS='+str(VD)+'\nIDSAT measured at VGS=0.8, VDS=0.8, stress(forward) direction', fontsize=10)
        for VGVD in np.arange(VD_values):
            for cycle in np.arange(PulseCycle):
                for row in np.arange(8*VGVD, 8*(VGVD+1)-1, 1):
                    HCI, = plt.plot(cycle*0.2+np.append(np.arange(write_time*pulse_length*cycle, write_time*pulse_length*(cycle+1)+0.0001, pulse_length), np.array([write_time*pulse_length*(cycle+1)+0.1])), 1e6*IDSAT_VAsource_VBdrain[row][cycle*(1+write_time+1+1)+1: (cycle+1)*(1+write_time+1+1)], color='r', linestyle='solid', marker='.')
                for row in np.arange(8*(VGVD+1)-1, 8*(VGVD+1), 1):
                    PBTI, = plt.plot(cycle*0.2+np.append(np.arange(write_time*pulse_length*cycle, write_time*pulse_length*(cycle+1)+0.0001, pulse_length), np.array([write_time*pulse_length*(cycle+1)+0.1])), 1e6*IDSAT_VAsource_VBdrain[row][cycle*(1+write_time+1+1)+1: (cycle+1)*(1+write_time+1+1)], color='y', linestyle='solid', marker='*')
        plt.xticks(t, t_label, rotation=30, fontsize=9)
        plt.ylim(Imin, Imax)
        plt.xlim(0, 1.7)
        plt.grid()
        plt.legend([HCI, PBTI], ['HCI', 'PBTI: VDS=0'], loc = 'best')
        plt.xlabel('time (sec)')
        plt.ylabel('IDSAT (uA)')
        plt.subplots_adjust(bottom=0.15)
        plt.savefig(path_plot+'IDSAT_'+str(figN).zfill(3)+'_Col'+str(col).zfill(2)+'_forward.pdf')
        figN = figN+1

        plt.figure(figN)
        plt.title('Col['+str(col)+'], IDSAT (10 x 40ms Pulses) x 3 Cycles, VGS=2V, VDS='+str(VD)+'\nIDSAT measured at VGS=0.8, VDS=0.8, reversed direction', fontsize=10)
        for VGVD in np.arange(VD_values):
            for cycle in np.arange(PulseCycle):
                for row in np.arange(8*VGVD, 8*(VGVD+1)-1, 1):
                    HCI, = plt.plot(cycle*0.2+np.append(np.arange(write_time*pulse_length*cycle, write_time*pulse_length*(cycle+1)+0.0001, pulse_length), np.array([write_time*pulse_length*(cycle+1)+0.1])), 1e6*IDSAT_VAdrain_VBsource[row][cycle*(1+write_time+1+1)+1: (cycle+1)*(1+write_time+1+1)], color='r', linestyle='solid', marker='.')
                for row in np.arange(8*(VGVD+1)-1, 8*(VGVD+1), 1):
                    PBTI, = plt.plot(cycle*0.2+np.append(np.arange(write_time*pulse_length*cycle, write_time*pulse_length*(cycle+1)+0.0001, pulse_length), np.array([write_time*pulse_length*(cycle+1)+0.1])), 1e6*IDSAT_VAdrain_VBsource[row][cycle*(1+write_time+1+1)+1: (cycle+1)*(1+write_time+1+1)], color='y', linestyle='solid', marker='*')
        plt.xticks(t, t_label, rotation=30, fontsize=9)
        plt.ylim(Imin, Imax)
        plt.xlim(0, 1.7)
        plt.grid()
        plt.legend([HCI, PBTI], ['HCI', 'PBTI: VDS=0'], loc = 'best')
        plt.xlabel('time (sec)')
        plt.ylabel('IDSAT (uA)')
        plt.subplots_adjust(bottom=0.15)
        plt.savefig(path_plot+'IDSAT_'+str(figN).zfill(3)+'_Col'+str(col).zfill(2)+'_reversed.pdf')
        figN = figN+1


if __name__ == '__main__':
  main()

