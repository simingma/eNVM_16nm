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
    col = 19
    path_data = '../data/Current_Components_chip02_L36_LVT/'
    path_data_MUX_OFF = '../data/Current_Components_chip02_MUX_OFF_2/'
    #path_plot = '../plot/Current_Components_chip02_L36_LVT/'
    #path_data = '../data/Current_Components_chip02_MUX_OFF/'
    path_plot = '../plot/Current_Components_chip02_MUX_OFF_2/'
    VDD_IO = '_VDD_IO_2p8'
    Vg = '_Vg0'
    VsVb_ZeroIndex_VDmin = [('_VsVb0', 0, 0), ('_VsVb0p5', 5, 0.5), ('_VsVb1p0', 10, 1.0), ('_Vs0p5_Vb1p0', 5, 0.5)]

    VA_VB_list = ['_VAsource_VBdrain', '_VAdrain_VBsource']

    #VD = np.arange(0, 3.6+0.0001, 0.1) #SVT
    VD = np.arange(0, 2.8+0.0001, 0.1)  #ULVT
    #WL = ['100/30','210/30','100/60','210/60','100/90','210/90']
    #Yrange = [(-100,3000),(-100,3000),(-5,50),(-5,50),(-2,10),(-2,10)]

    figN = 1
    for (VsVb, ZeroIndex, VD_min) in VsVb_ZeroIndex_VDmin:
        #for col in column:
            for VA_VB in VA_VB_list:
                I_Drain = []
                I_Psub = []
                I_Source = []
                I_Drain_0 = []
                I_Psub_0 = []
                I_Source_0 = []
                fname = 'Current_Components'+VDD_IO+Vg+VsVb+'_Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+VA_VB
                fname_0 = 'Current_Components'+VDD_IO+Vg+VsVb+'_Chip'+str(chip).zfill(2)+'_MUX_OFF'+VA_VB+'_2'
                f = open(path_data + fname, 'rU')
                f_0 = open(path_data_MUX_OFF + fname_0, 'rU')
                I_Drain.append(re.findall(r'ID=(-*\d+\.\d+)',f.read()))
                I_Drain_0.append(re.findall(r'ID=(-*\d+\.\d+)',f_0.read()))
                f.seek(0, 0)
                f_0.seek(0, 0)
                I_Psub.append(re.findall(r'Isub=(-*\d+\.\d+)',f.read()))
                I_Psub_0.append(re.findall(r'Isub=(-*\d+\.\d+)',f_0.read()))
                f.seek(0, 0)
                f_0.seek(0, 0)
                I_Source.append(re.findall(r'IS=(-*\d+\.\d+)',f.read()))
                I_Source_0.append(re.findall(r'IS=(-*\d+\.\d+)',f_0.read()))
                f.close()
                f_0.close()
                print(len(I_Drain[0]), len(I_Psub[0]), len(I_Source[0])) #ULVT, should all be len(VD) = (3.2/0.1+1)=33
                print(len(I_Drain_0[0]), len(I_Psub_0[0]), len(I_Source_0[0])) #ULVT, should all be len(VD) = (3.2/0.1+1)=33
                #print(len(I_Drain[0]), len(I_Psub[0]), len(I_Source[0])) #SVT, should all be len(VD) = (3.6/0.1+1)=37
                Isense_Drain = np.zeros(len(VD))
                Isense_Psub = np.zeros(len(VD))
                Isense_Source = np.zeros(len(VD))
                Isense_Drain_0 = np.zeros(len(VD))
                Isense_Psub_0 = np.zeros(len(VD))
                Isense_Source_0 = np.zeros(len(VD))

                for k in np.arange(0, len(VD)):
                    Isense_Drain[k] = np.float64(I_Drain[0][k])
                    Isense_Psub[k] = np.float64(I_Psub[0][k])
                    Isense_Source[k] = np.float64(I_Source[0][k])
                    Isense_Drain_0[k] = np.float64(I_Drain_0[0][k])
                    Isense_Psub_0[k] = np.float64(I_Psub_0[0][k])
                    Isense_Source_0[k] = np.float64(I_Source_0[0][k])
                plt.figure(figN)
                #ID, = plt.plot(VD, 1e6*Isense_Drain, color = 'r')
                #Isub, = plt.plot(VD, 1e6*Isense_Psub, color = 'g')
                #IS, = plt.plot(VD, 1e6*Isense_Source, color = 'y')
                #ID, = plt.plot(VD, 1e6*(Isense_Drain-Isense_Drain[ZeroIndex]), color = 'r')
                #Isub, = plt.plot(VD, 1e6*(Isense_Psub-Isense_Psub[ZeroIndex]), color = 'g')
                #IS, = plt.plot(VD, 1e6*(Isense_Source-Isense_Source[ZeroIndex]), color = 'y')
                #ID, = plt.plot(VD, 1e6*(Isense_Drain-Isense_Drain_0), color = 'r')
                #Isub, = plt.plot(VD, 1e6*(Isense_Psub-Isense_Psub_0), color = 'g')
                #IS, = plt.plot(VD, 1e6*(Isense_Source-Isense_Source_0), color = 'y')
                ID, = plt.plot(VD, 1e6*Isense_Drain_0, color = 'r')
                Isub, = plt.plot(VD, 1e6*Isense_Psub_0, color = 'g')
                IS, = plt.plot(VD, 1e6*Isense_Source_0, color = 'y')
                plt.legend([ID, Isub, IS], ['ID', 'Isub', 'IS'], loc = 'best')
                plt.xlabel('VD (V)')
                plt.ylabel('Current (uA)')
                plt.title(fname+'_LVT', fontsize = 10)
                #plt.title(fname, fontsize = 12)
                plt.grid()
                #plt.xlim(xmin = VD_min)
                #plt.ylim(ymin = -5)
                #plt.savefig(path_plot+str(figN).zfill(3)+fname+'_LVT.pdf')
                plt.savefig(path_plot+str(figN).zfill(3)+fname+'.pdf')
                figN = figN+1
        #plt.semilogy(VAB, 1e6*I_leak_Psub_VAsource_VBdrain , color='green', linestyle='solid')

if __name__ == '__main__':
  main()
    
