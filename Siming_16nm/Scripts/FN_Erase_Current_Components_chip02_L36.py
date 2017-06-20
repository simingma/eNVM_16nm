#!/usr/bin/python26 -tt

import sys
import re
import os
import commands
import numpy as np
import matplotlib.pyplot as plt
#from scipy.interpolate import interp1d

def main():
    chip = 2
    #column = [20, 23, 2, 5] #SVT
    #column = [18, 21, 0, 3] #ULVT
    column = [19, 22, 1, 4] #LVT
    path_data = '../data/FN_Erase_Current_Components_chip02_L36_LVT/'
    path_data_MUX_OFF = '../data/FN_Erase_Current_Components_chip02_MUX_OFF/'
    path_plot = '../plot/FN_Erase_Current_Components_chip02_L36_LVT/'
    path_plot_MUX_OFF = '../plot/FN_Erase_Current_Components_chip02_MUX_OFF/'
    VDD_IO = '_VDD_IO_2p8'
    VDD_WL = '_VDD_WL_2p2'
    Vg = '_Vg0'
    #VD = np.arange(0, 3.6+0.0001, 0.1) #SVT
    VSVBVD = np.arange(0, 2.8+0.0001, 0.1)  #LVT

    I_Drain_0 = []
    I_Psub_0 = []
    I_Source_0 = []
    fname_0 = 'FN_Erase'+VDD_IO+VDD_WL+Vg+'_Chip'+str(chip).zfill(2)+'_MUX_OFF_VAdrain_VBsource'
    f_0 = open(path_data_MUX_OFF + fname_0, 'rU')
    I_Drain_0.append(re.findall(r'ID=(-*\d+\.\d+)',f_0.read()))
    f_0.seek(0, 0)
    I_Psub_0.append(re.findall(r'Isub=(-*\d+\.\d+)',f_0.read()))
    f_0.seek(0, 0)
    I_Source_0.append(re.findall(r'IS=(-*\d+\.\d+)',f_0.read()))
    f_0.close()
    print(len(I_Drain_0[0]), len(I_Psub_0[0]), len(I_Source_0[0])) #LVT, should all be len(VSVBVD) = (2.8/0.1+1)=29
    Isense_Drain_0 = np.zeros(len(VSVBVD))
    Isense_Psub_0 = np.zeros(len(VSVBVD))
    Isense_Source_0 = np.zeros(len(VSVBVD))

    for k in np.arange(0, len(VSVBVD)):
        Isense_Drain_0[k] = np.float64(I_Drain_0[0][k])
        Isense_Psub_0[k] = np.float64(I_Psub_0[0][k])
        Isense_Source_0[k] = np.float64(I_Source_0[0][k])

    figN = 0
    plt.plot(figN)
    ID, = plt.plot(VSVBVD, 1e6*Isense_Drain_0, color = 'r')
    Isub, = plt.plot(VSVBVD, 1e6*Isense_Psub_0, color = 'g')
    IS, = plt.plot(VSVBVD, 1e6*Isense_Source_0, color = 'y')
    plt.legend([ID, Isub, IS], ['ID', 'Isub', 'IS'], loc = 'best')
    plt.xlabel('VS=VB=VD (V)')
    plt.ylabel('Current (uA)')
    plt.title(fname_0, fontsize = 10)
    plt.grid()
    plt.savefig(path_plot_MUX_OFF+str(figN).zfill(2)+'_'+fname_0+'.pdf')
    figN = figN+1
    #plt.semilogy(VAB, 1e6*I_leak_Psub_VAsource_VBdrain , color='green', linestyle='solid')
    for col in column:
        I_Drain = []
        I_Psub = []
        I_Source = []
        fname = 'FN_Erase'+VDD_IO+VDD_WL+Vg+'_Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)
        f = open(path_data + fname, 'rU')
        I_Drain.append(re.findall(r'ID=(-*\d+\.\d+)',f.read()))
        f.seek(0, 0)
        I_Psub.append(re.findall(r'Isub=(-*\d+\.\d+)',f.read()))
        f.seek(0, 0)
        I_Source.append(re.findall(r'IS=(-*\d+\.\d+)',f.read()))
        f.close()
        print(len(I_Drain[0]), len(I_Psub[0]), len(I_Source[0])) #LVT, should all be len(VSVBVD) = (2.8/0.1+1)=29
        Isense_Drain = np.zeros(len(VSVBVD))
        Isense_Psub = np.zeros(len(VSVBVD))
        Isense_Source = np.zeros(len(VSVBVD))

        for k in np.arange(0, len(VSVBVD)):
            Isense_Drain[k] = np.float64(I_Drain[0][k])
            Isense_Psub[k] = np.float64(I_Psub[0][k])
            Isense_Source[k] = np.float64(I_Source[0][k])

        plt.figure(figN)
        ID, = plt.plot(VSVBVD, 1e6*Isense_Drain, color = 'r')
        Isub, = plt.plot(VSVBVD, 1e6*Isense_Psub, color = 'g')
        IS, = plt.plot(VSVBVD, 1e6*Isense_Source, color = 'y')
        plt.legend([ID, Isub, IS], ['ID', 'Isub', 'IS'], loc = 'best')
        plt.xlabel('VS=VB=VD (V)')
        plt.ylabel('Current (uA)')
        plt.title(fname, fontsize = 12)
        plt.grid()
        plt.savefig(path_plot+str(figN).zfill(2)+'_'+fname+'.pdf')
        plt.close()
        figN = figN+1

        plt.figure(figN)
        ID, = plt.plot(VSVBVD, 1e6*(Isense_Drain-Isense_Drain_0), color = 'r')
        Isub, = plt.plot(VSVBVD, 1e6*(Isense_Psub-Isense_Psub_0), color = 'g')
        IS, = plt.plot(VSVBVD, 1e6*(Isense_Source-Isense_Source_0), color = 'y')
        plt.legend([ID, Isub, IS], ['ID', 'Isub', 'IS'], loc = 'best')
        plt.xlabel('VS=VB=VD (V)')
        plt.ylabel('Current (uA)')
        plt.title(fname+' Subtract MUX_OFF', fontsize = 11)
        plt.grid()
        plt.savefig(path_plot+str(figN).zfill(2)+'_'+fname+'_Subtract_MUX_OFF.pdf')
        plt.close()
        figN = figN+1


if __name__ == '__main__':
  main()
    
