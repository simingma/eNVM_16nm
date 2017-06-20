
import sys
import re
import os
import commands
import numpy as np
import matplotlib.pyplot as plt
#from scipy.interpolate import interp1d

def main():
    chip = 2
    col = 33
    path_data = '../data/VG_realvalue_chip02/'
    path_plot = '../plot/VG_realvalue_chip02/'
    if os.path.isdir(path_plot) == False:
        os.mkdir(path_plot)

    VDD_WL = np.arange(0.8, 2.2+0.0001, 0.1)  

    figN = 1
    for direction in ['_VAsource_VBdrain', '_VAdrain_VBsource']:
        I_Drain = []
        I_Psub = []
        I_Source = []
        fname = 'VG_realvalue_Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+direction
        f = open(path_data + fname, 'rU')
        I_Drain.append(re.findall(r'ID=(-*\d+\.\d+)',f.read()))
        f.seek(0, 0)
        I_Psub.append(re.findall(r'Isub=(-*\d+\.\d+)',f.read()))
        f.seek(0, 0)
        I_Source.append(re.findall(r'IS=(-*\d+\.\d+)',f.read()))
        f.close()
        print(len(I_Drain[0]), len(I_Psub[0]), len(I_Source[0])) #should all be len(VDD_WL) = ((2.2-0.8)/0.1+1)=15
        Isense_Drain = np.zeros(len(VDD_WL))
        Isense_Psub = np.zeros(len(VDD_WL))
        Isense_Source = np.zeros(len(VDD_WL))

        for k in np.arange(0, len(VDD_WL)):
            Isense_Drain[k] = np.float64(I_Drain[0][k])
            Isense_Psub[k] = np.float64(I_Psub[0][k])
            Isense_Source[k] = np.float64(I_Source[0][k])
        plt.figure(figN)
        ID, = plt.plot(VDD_WL, 1e6*Isense_Drain, color = 'r')
        Isub, = plt.plot(VDD_WL, 1e6*Isense_Psub, color = 'g')
        IS, = plt.plot(VDD_WL, 1e6*Isense_Source, color = 'y')
        plt.legend([ID, Isub, IS], ['ID', 'Isub', 'IS'], loc = 'best')
        plt.xlabel('VDD_WL (V)')
        plt.ylabel('Total Current (uA)')
        plt.title('Total current of "turned off" col'+str(col)+' when cranking up VDD_WL', fontsize = 10)
        plt.grid()
        plt.savefig(path_plot+str(figN).zfill(2)+'_'+fname+'.pdf')
        figN = figN+1
        #plt.semilogy(VAB, 1e6*I_leak_Psub_VAsource_VBdrain , color='green', linestyle='solid')

if __name__ == '__main__':
  main()
    
