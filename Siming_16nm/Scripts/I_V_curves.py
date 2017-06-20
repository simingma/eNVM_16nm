
import sys
import re
import os
import commands
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
from scipy.interpolate import interp1d

def hist_IDS_VGS(VG_idx, chip, col, L, Nfin, VT_flavor, Nrow, data_files, colors, path_plot, plot_file, row_idx, title, Ymin, Ymax, Imin, Imax, VD = 0.8, VG_min = 0.2, VG_max = 0.8, Num_bins = 50):

    if os.path.isdir(path_plot) == False:
        os.mkdir(path_plot)

    VDD_WL = np.arange(VG_max, VG_min -0.0001, -0.05)
    #Ymax = 0
#    for direction in ['VAsource_VBdrain', 'VAdrain_VBsource']:
    fig, ax = plt.subplots(nrows = 1, ncols = 1)
    n_max = 0
    for (data_file, color) in zip(data_files, colors):
        Isense = []
        #f=open(path_data+'Fresh_Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+'_Ids_Vgs_'+direction,'rU')
        f=open(data_file,'rU')
        Isense.append(re.findall(r'Isense=(-*\d*\.\d+)',f.read()))
        f.close()
        print(len(Isense), len(Isense[0]))
        if (len(Isense) != 1 or len(Isense[0]) != (Nrow+1)*len(VDD_WL)):
            print('data file error!')
        #I_leak_VAsource_VBdrain = np.zeros(len(VDD_WL))
        #I_leak_VAdrain_VBsource = np.zeros(len(VDD_WL))
        #for k in np.arange(len(VDD_WL)):
        #    I_leak_VAsource_VBdrain[k] = np.float64(Isense_VAsource_VBdrain[k][0])
        #    I_leak_VAdrain_VBsource[k] = np.float64(Isense_VAdrain_VBsource[k][0])
        IDS_VGS = np.zeros((Nrow, len(VDD_WL)))
        for row in np.arange(0, Nrow):
            IDS_VGS[row][:] = np.array(np.float64(Isense[0][(row+1)*len(VDD_WL): (row+2)*len(VDD_WL)]))

        #Ymax = max(1e6 * np.amax(IDS_VGS), Ymax)
        #plt.figure(figN)
        #plt.title('L='+str(L)+', Nfin='+str(Nfin)+', '+VT_flavor+', #'+str(Nrow)+' fresh nmos IDS-VGS curves', fontsize=10)
        n, bins, pathes = ax.hist(1e6*IDS_VGS[row_idx, [VG_idx]*len(row_idx)], bins=Num_bins, normed = False, range=(Imin, Imax), orientation = 'horizontal', color = color)
        if np.amax(n) > n_max:
            n_max = np.amax(n)
    print(n_max)
    ax.set_xlim(0, n_max*4)
    ax.set_ylim(Ymin, Ymax)
    ax.grid(True)
    plt.savefig(path_plot+plot_file+'Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+'.pdf')
    #plt.savefig(path_plot+'IDS_VGS_'+str(figN).zfill(2)+'_Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+'_'+direction+'.pdf')
    #figN = figN + 1
    plt.close()

def IDS_VGS(chip, col, L, Nfin, VT_flavor, Nrow, data_files, colors, path_plot, plot_file, row_idx, title, Ymax, VD = 0.8, VG_min = 0.2, VG_max = 0.8):

    if os.path.isdir(path_plot) == False:
        os.mkdir(path_plot)

    VDD_WL = np.arange(VG_max, VG_min -0.0001, -0.05)
    #Ymax = 0
#    for direction in ['VAsource_VBdrain', 'VAdrain_VBsource']:
    for (data_file, color) in zip(data_files, colors):
        Isense = []
        #f=open(path_data+'Fresh_Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+'_Ids_Vgs_'+direction,'rU')
        f=open(data_file,'rU')
        Isense.append(re.findall(r'Isense=(-*\d*\.\d+)',f.read()))
        f.close()
        print(len(Isense), len(Isense[0]))
        if (len(Isense) != 1 or len(Isense[0]) != (Nrow+1)*len(VDD_WL)):
            print('data file error!')
        #I_leak_VAsource_VBdrain = np.zeros(len(VDD_WL))
        #I_leak_VAdrain_VBsource = np.zeros(len(VDD_WL))
        #for k in np.arange(len(VDD_WL)):
        #    I_leak_VAsource_VBdrain[k] = np.float64(Isense_VAsource_VBdrain[k][0])
        #    I_leak_VAdrain_VBsource[k] = np.float64(Isense_VAdrain_VBsource[k][0])
        IDS_VGS = np.zeros((Nrow, len(VDD_WL)))
        for row in np.arange(0, Nrow):
            IDS_VGS[row][:] = np.array(np.float64(Isense[0][(row+1)*len(VDD_WL): (row+2)*len(VDD_WL)]))

        #Ymax = max(1e6 * np.amax(IDS_VGS), Ymax)
        #plt.figure(figN)
        #plt.title('L='+str(L)+', Nfin='+str(Nfin)+', '+VT_flavor+', #'+str(Nrow)+' fresh nmos IDS-VGS curves', fontsize=10)
        for row in row_idx:
            plt.plot(VDD_WL, 1e6*IDS_VGS[row], color=color, linestyle='solid', marker='.', alpha = 0.4)
    plt.title('L='+str(L)+', Nfin='+str(Nfin)+', '+VT_flavor+', '+title+' IDS-VGS curves', fontsize=7)
    plt.axis([VG_min, VG_max, 0, Ymax])
    plt.xlabel('VGS (V)')
    plt.ylabel('IDS (uA)')
    plt.grid()
    plt.savefig(path_plot+plot_file+'Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+'.pdf')
    #plt.savefig(path_plot+'IDS_VGS_'+str(figN).zfill(2)+'_Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+'_'+direction+'.pdf')
    #figN = figN + 1
    plt.close()

def IDS_VGS_stress(chip, col, L, Nfin, VT_flavor, Nrow, VGS, VDS, Nhci, Npbti, VD = 0.8, VG_min = 0.2, VG_max = 0.8, PulseCycle = 3, stress_time=['400ms','800ms','1200ms']):

    path_data = '../data/VG_ConstPulse_chip'+str(chip).zfill(2)+'/'
    path_plot = '../plot/VG_ConstPulse_chip'+str(chip).zfill(2)+'_IDS_VGS/'

    if os.path.isdir(path_plot) == False:
        os.mkdir(path_plot)

    VDD_WL = np.arange(VG_max, VG_min -0.0001, -0.05)
    for direction in ['VAsource_VBdrain', 'VAdrain_VBsource']:
        Isense = []
        f=open(path_data+'Fresh_Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+'_Ids_Vgs_'+direction,'rU')
        Isense.append(re.findall(r'Isense=(-*\d*\.\d+)',f.read()))
        f.close()
        print(len(Isense), len(Isense[0]))
        if (len(Isense) != 1 or len(Isense[0]) != (Nrow+1)*len(VDD_WL)):
            print('data file error!')
        #I_leak_VAsource_VBdrain = np.zeros(len(VDD_WL))
        #for k in np.arange(len(VDD_WL)):
        #    I_leak_VAsource_VBdrain[k] = np.float64(Isense_VAsource_VBdrain[k][0])
        IDS_VGS = np.zeros((Nrow, len(VDD_WL)))
        for row in np.arange(0, Nrow):
            IDS_VGS[row][:] = np.array(np.float64(Isense[0][(row+1)*len(VDD_WL): (row+2)*len(VDD_WL)]))
        
        IDS_VGS_stress = np.zeros((PulseCycle, Nrow, len(VDD_WL)))
        for cycle in np.arange(PulseCycle):
            Isense = []
            f=open(path_data+'Stress_Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+'_Ids_Vgs_'+direction+'_'+str(cycle+1).zfill(2),'rU')
            Isense.append(re.findall(r'Isense=(-*\d*\.\d+)',f.read()))
            f.close()
            print(len(Isense), len(Isense[0]))
            if (len(Isense) != 1 or len(Isense[0]) != (Nrow+1)*len(VDD_WL)):
                print('data file error!')
            for row in np.arange(0, Nrow):
                IDS_VGS_stress[cycle,row,:] = np.array(np.float64(Isense[0][(row+1)*len(VDD_WL): (row+2)*len(VDD_WL)]))


        Ymax = 1e6 * np.amax([np.amax(IDS_VGS), np.amax(IDS_VGS_stress)])
        #plt.figure(figN)
        plt.title('L='+str(L)+', Nfin='+str(Nfin)+', '+VT_flavor+', fresh nmos IDS-VGS curves ('+direction+')', fontsize=10)
        row = 0
        while(row < Nrow):
            for hci in np.arange(0, Nhci):
                fresh, = plt.plot(VDD_WL, 1e6*IDS_VGS[row], color='b', linestyle='solid', marker='.')
                row += 1
            for pbti in np.arange(0, Npbti):
                row += 1

        plt.axis([VG_min, VG_max, 0, Ymax])
        plt.xlabel('VGS (V)')
        plt.ylabel('IDS (uA)')
        plt.grid()
        plt.savefig(path_plot+'Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+'_'+plot_file+'.pdf')
        plt.close()

        for cycle in np.arange(PulseCycle):
            plt.title('L='+str(L)+', Nfin='+str(Nfin)+', '+VT_flavor+', IDS-VGS curves ('+direction+') of fresh and '+stress_time[cycle]+' HCI\n VDS='+str(VDS)+'V, VGS='+str(VGS)+'V', fontsize=10)
            row = 0
            while(row < Nrow):
                for hci in np.arange(0, Nhci):
                    fresh, = plt.plot(VDD_WL, 1e6*IDS_VGS[row], color='b', linestyle='solid', marker='.')
                    stressed, = plt.plot(VDD_WL, 1e6*IDS_VGS_stress[cycle,row], color='r', linestyle='solid', marker='.')
                    row += 1
                for pbti in np.arange(0, Npbti):
                    row += 1

            plt.legend([fresh, stressed], ['fresh', 'HCI stress'], loc = 'best')
            plt.axis([VG_min, VG_max, 0, Ymax])
            plt.xlabel('VGS (V)')
            plt.ylabel('IDS (uA)')
            plt.grid()
            plt.savefig(path_plot+'Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+'_'+direction+'_'+str(cycle+1)+'.pdf')
            plt.close()

if __name__ == '__main__':
  main()

