
import sys
import re
import os
import commands
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
from scipy.interpolate import interp1d


def Charge_Pumping(chip, col, L, Nfin, VT_flavor, Nrow, data_file, path_plot, plot_file, title, Vbot_Vsbd_min, Vbot_Vsbd_max, Vbot_Vsbd_step, Num_of_steps, VSS_WL, VDD_WL, Num_of_ExtTrig, Num_of_freq = 1, pumping_freq = [5000000], freq_legend = ['5MHz'], VSBD_descend = 1, plot_freq_idx = [0], DCcorrection_freq_idx = 0, plot_freq_idx_DCcorrected = [0], CounterIdleHigh = 0):

    if os.path.isdir(path_plot) == False:
        os.mkdir(path_plot)

    Vbot_Vsbd = np.arange(Vbot_Vsbd_min, Vbot_Vsbd_max + 0.0001, Vbot_Vsbd_step)

    """
    Isub_without_pumping = []
    f=open(data_file,'rU')
    Isub_without_pumping.append(re.findall(r'Isub_without_pumping=(-*\d*\.\d+)',f.read()))
    f.close()
    """
    Isub_with_pumping = []
    f=open(data_file,'rU')
    Isub_with_pumping.append(re.findall(r'Isub_with_pumping=(-*\d*\.\d+)',f.read()))
    f.close()
    """
    print(len(Isub_with_pumping[0]), len(Isub_without_pumping[0]))
    if (len(Isub_without_pumping[0]) != (Num_of_steps*Num_of_ExtTrig) or len(Isub_with_pumping[0]) != (Num_of_freq*Num_of_steps*Num_of_ExtTrig)):
        print('data file error!')
    """
    print(len(Isub_with_pumping[0]))
    if (len(Isub_with_pumping[0]) != (Num_of_freq*Num_of_steps*Num_of_ExtTrig)):
        print('data file error!')

    Isub_pumping = np.zeros((Num_of_freq, Num_of_steps))
    axe_handle = ['']*Num_of_freq
    for (freq_idx, counter1_freq) in zip(range(Num_of_freq), pumping_freq):
        for Isub_idx in np.arange(0, Num_of_steps):
            Isub_pumping[freq_idx][Isub_idx] = 1e9*np.mean(np.float64(Isub_with_pumping[0][Num_of_freq*Isub_idx*Num_of_ExtTrig+freq_idx*Num_of_ExtTrig : Num_of_freq*Isub_idx*Num_of_ExtTrig+freq_idx*Num_of_ExtTrig+Num_of_ExtTrig]))
        if freq_idx in plot_freq_idx:
            if VSBD_descend == 1:
                axe_handle[freq_idx], = plt.plot(np.arange(Vbot_Vsbd_min, Vbot_Vsbd_max+0.0001, Vbot_Vsbd_step), Isub_pumping[freq_idx], marker='.')
            if VSBD_descend == 0:
                axe_handle[freq_idx], = plt.plot(np.arange(Vbot_Vsbd_max, Vbot_Vsbd_min-0.0001, (-1)*Vbot_Vsbd_step), Isub_pumping[freq_idx], marker='.')

    plt.title('L='+str(L)+', Nfin='+str(Nfin)+', '+VT_flavor+', '+'VSS_WL(Vbot)='+str(VSS_WL)+', VDD_WL='+str(VDD_WL)+', sweep VS=VB=VD(Vsbd), Isub_with_pumping', fontsize=9)
    #plt.axis([VG_min, VG_max, 0, Ymax])
    plt.xlim(Vbot_Vsbd_min, Vbot_Vsbd_max)
    plt.xlabel('Vbot - Vsbd (V)')
    plt.ylabel('Isub_with_pumping (nA)')
    axe_to_plot = []
    legend_to_plot = []
    for plt_idx in plot_freq_idx:
        axe_to_plot.append(axe_handle[plt_idx])
        legend_to_plot.append(freq_legend[plt_idx])
    plt.legend(axe_to_plot, legend_to_plot)
    plt.grid()
    plt.savefig(path_plot+plot_file+'_Isub_with_pumping_Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+'.pdf')
    plt.close()
    
    """
    Isub_DC = np.zeros(Num_of_steps)
    for Isub_idx in np.arange(0, Num_of_steps):
        Isub_DC[Isub_idx] = 1e9*np.mean(np.float64(Isub_without_pumping[0][Isub_idx*Num_of_ExtTrig:(Isub_idx+1)*Num_of_ExtTrig]))
    if VSBD_descend == 1:
        plt.plot(np.arange(Vbot_Vsbd_min, Vbot_Vsbd_max+0.0001, Vbot_Vsbd_step), Isub_DC, marker='.')
    if VSBD_descend == 0:
        plt.plot(np.arange(Vbot_Vsbd_max, Vbot_Vsbd_min-0.0001, (-1)*Vbot_Vsbd_step), Isub_DC, marker='.')

    plt.title('L='+str(L)+', Nfin='+str(Nfin)+', '+VT_flavor+', '+'VSS_WL(Vbot)='+str(VSS_WL)+', VDD_WL='+str(VDD_WL)+', sweep VS=VB=VD(Vsbd), Isub_without_pumping', fontsize=9)
    #plt.axis([VG_min, VG_max, 0, Ymax])
    plt.xlim(Vbot_Vsbd_min, Vbot_Vsbd_max)
    plt.xlabel('Vbot - Vsbd (V)')
    plt.ylabel('Isub_without_pumping (nA)')
    plt.grid()
    plt.savefig(path_plot+plot_file+'_Isub_without_pumping_Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+'.pdf')
    plt.close()

#temporary: test settling time    
    Isub_without_pumping_10min=[]
    f=open(data_file,'rU')
    Isub_without_pumping_10min.append(re.findall(r'Isub_without_pumping_10min_TESTING_CounterIdleLow=(-*\d*\.\d+)',f.read()))
    f.close()
    print(len(Isub_without_pumping_10min[0]))
    if len(Isub_without_pumping_10min[0]) != (Num_of_steps*Num_of_ExtTrig):
        print('data file error!')
    Isub_DC_10min = np.zeros(Num_of_steps)
    for Isub_idx in np.arange(0, Num_of_steps):
        Isub_DC_10min[Isub_idx] = 1e9*np.mean(np.float64(Isub_without_pumping_10min[0][Isub_idx*Num_of_ExtTrig:(Isub_idx+1)*Num_of_ExtTrig]))
    if VSBD_descend == 1:
        plt.plot(np.arange(Vbot_Vsbd_min, Vbot_Vsbd_max+0.0001, Vbot_Vsbd_step), Isub_DC_10min, marker='.')
    if VSBD_descend == 0:
        plt.plot(np.arange(Vbot_Vsbd_max, Vbot_Vsbd_min-0.0001, (-1)*Vbot_Vsbd_step), Isub_DC_10min, marker='.')

    plt.title('L='+str(L)+', Nfin='+str(Nfin)+', '+VT_flavor+', '+'VSS_WL(Vbot)='+str(VSS_WL)+', VDD_WL='+str(VDD_WL)+', sweep VS=VB=VD(Vsbd), Isub_without_pumping_10min', fontsize=9)
    #plt.axis([VG_min, VG_max, 0, Ymax])
    plt.xlim(Vbot_Vsbd_min, Vbot_Vsbd_max)
    plt.xlabel('Vbot - Vsbd (V)')
    plt.ylabel('Isub_without_pumping_10min (nA)')
    plt.grid()
    plt.savefig(path_plot+plot_file+'_Isub_without_pumping_10min_Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+'.pdf')
    plt.close()
#temporary: test settling time    

    Icp = np.zeros((Num_of_freq, Num_of_steps))
    axe_handle = ['']*Num_of_freq
    for (freq_idx, counter1_freq) in zip(np.arange(Num_of_freq), pumping_freq):
        for Isub_idx in np.arange(0, Num_of_steps):
            Icp[freq_idx][Isub_idx] = Isub_pumping[freq_idx][Isub_idx] - Isub_DC[Isub_idx]
            #print(np.float64(Isub_with_pumping[0][Isub_idx*Num_of_ExtTrig:(Isub_idx+1)*Num_of_ExtTrig]))
            #print(np.float64(Isub_without_pumping[0][Isub_idx*Num_of_ExtTrig:(Isub_idx+1)*Num_of_ExtTrig]))
        if freq_idx in plot_freq_idx:
            if VSBD_descend == 1:
                axe_handle[freq_idx], = plt.plot(np.arange(Vbot_Vsbd_min, Vbot_Vsbd_max+0.0001, Vbot_Vsbd_step), Icp[freq_idx], marker='.')
            if VSBD_descend == 0:
                axe_handle[freq_idx], = plt.plot(np.arange(Vbot_Vsbd_max, Vbot_Vsbd_min-0.0001, (-1)*Vbot_Vsbd_step), Icp[freq_idx], marker='.')

    plt.title('L='+str(L)+', Nfin='+str(Nfin)+', '+VT_flavor+', '+'VSS_WL(Vbot)='+str(VSS_WL)+', VDD_WL='+str(VDD_WL)+', sweep VS=VB=VD(Vsbd), charge pumping curves', fontsize=9)
    #plt.axis([VG_min, VG_max, 0, Ymax])
    plt.xlim(Vbot_Vsbd_min, Vbot_Vsbd_max)
    plt.xlabel('Vbot - Vsbd (V)')
    plt.ylabel('Icp = Isub_with_pumping-Isub_without_pumping (nA)')
    plt.grid()
    axe_to_plot = []
    legend_to_plot = []
    for plt_idx in plot_freq_idx:
        axe_to_plot.append(axe_handle[plt_idx])
        legend_to_plot.append(freq_legend[plt_idx])
    plt.legend(axe_to_plot, legend_to_plot)
    plt.savefig(path_plot+plot_file+'Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+'Pumping_minus_NoPumping.pdf')
    plt.close()
    """

    if DCcorrection_freq_idx != 0:
        Icp_DCcorrected = np.zeros((Num_of_freq, Num_of_steps))
        axe_handle = ['']*Num_of_freq
        for (freq_idx, counter1_freq) in zip(np.arange(Num_of_freq), pumping_freq):
            for Isub_idx in np.arange(0, Num_of_steps):
                Icp_DCcorrected[freq_idx][Isub_idx] = Isub_pumping[freq_idx][Isub_idx] - Isub_pumping[DCcorrection_freq_idx][Isub_idx]
            if freq_idx in plot_freq_idx_DCcorrected:
                if VSBD_descend == 1:
                    axe_handle[freq_idx], = plt.plot(np.arange(Vbot_Vsbd_min, Vbot_Vsbd_max+0.0001, Vbot_Vsbd_step), Icp_DCcorrected[freq_idx], marker='.')
                if VSBD_descend == 0:
                    axe_handle[freq_idx], = plt.plot(np.arange(Vbot_Vsbd_max, Vbot_Vsbd_min-0.0001, (-1)*Vbot_Vsbd_step), Icp_DCcorrected[freq_idx], marker='.')

        plt.title('L='+str(L)+', Nfin='+str(Nfin)+', '+VT_flavor+', '+'VSS_WL(Vbot)='+str(VSS_WL)+', VDD_WL='+str(VDD_WL)+', sweep VS=VB=VD(Vsbd), charge pumping curves, DC corrected by subtracting freq='+freq_legend[DCcorrection_freq_idx], fontsize=7)
        #plt.axis([VG_min, VG_max, 0, Ymax])
        plt.xlim(Vbot_Vsbd_min, Vbot_Vsbd_max)
        plt.xlabel('Vbot - Vsbd (V)')
        plt.ylabel('Icp = Isub_with_pumping-Isub_with_pumping @' + freq_legend[DCcorrection_freq_idx] +' (nA)')
        plt.grid()
        axe_to_plot = []
        legend_to_plot = []
        for plt_idx in plot_freq_idx_DCcorrected:
            axe_to_plot.append(axe_handle[plt_idx])
            legend_to_plot.append(freq_legend[plt_idx])
        plt.legend(axe_to_plot, legend_to_plot)
        plt.savefig(path_plot+plot_file+'Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+'Pumping_minus_DCcorrection.pdf')
        plt.close()

    """
    if CounterIdleHigh == 1:
        Isub_without_pumping_CounterIdleHigh = []
        f=open(data_file,'rU')
        Isub_without_pumping_CounterIdleHigh.append(re.findall(r'Isub_without_pumping.+min_CounterIdleHigh=(-*\d*\.\d+)',f.read()))
        f.close()
        print(len(Isub_without_pumping_CounterIdleHigh[0]))
        if len(Isub_without_pumping_CounterIdleHigh[0]) != (Num_of_steps*Num_of_ExtTrig):
            print('data file error!')

        Isub_DC_CounterIdleHigh = np.zeros(Num_of_steps)
        for Isub_idx in np.arange(0, Num_of_steps):
            Isub_DC_CounterIdleHigh[Isub_idx] = 1e9*np.mean(np.float64(Isub_without_pumping_CounterIdleHigh[0][Isub_idx*Num_of_ExtTrig:(Isub_idx+1)*Num_of_ExtTrig]))
        if VSBD_descend == 1:
            plt.plot(np.arange(Vbot_Vsbd_min, Vbot_Vsbd_max+0.0001, Vbot_Vsbd_step), Isub_DC_CounterIdleHigh, marker='.')
        if VSBD_descend == 0:
            plt.plot(np.arange(Vbot_Vsbd_max, Vbot_Vsbd_min-0.0001, (-1)*Vbot_Vsbd_step), Isub_DC_CounterIdleHigh, marker='.')

        plt.title('L='+str(L)+', Nfin='+str(Nfin)+', '+VT_flavor+', '+'VSS_WL(Vbot)='+str(VSS_WL)+', VDD_WL='+str(VDD_WL)+', sweep VS=VB=VD(Vsbd), Isub_without_pumping_CounterIdleHigh', fontsize=8)
        #plt.axis([VG_min, VG_max, 0, Ymax])
        plt.xlim(Vbot_Vsbd_min, Vbot_Vsbd_max)
        plt.xlabel('Vbot - Vsbd (V)')
        plt.ylabel('Isub_without_pumping_CounterIdleHigh (nA)')
        plt.grid()
        plt.savefig(path_plot+plot_file+'_Isub_without_pumping_CounterIdleHigh_Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+'.pdf')
        plt.close()

#Temperary: test settling time 
        Isub_without_pumping_CounterIdleHigh_10min = []
        f=open(data_file,'rU')
        Isub_without_pumping_CounterIdleHigh_10min.append(re.findall(r'Isub_without_pumping_10min_TESTING_CounterIdleHigh=(-*\d*\.\d+)',f.read()))
        f.close()
        print(len(Isub_without_pumping_CounterIdleHigh_10min[0]))
        if len(Isub_without_pumping_CounterIdleHigh_10min[0]) != (Num_of_steps*Num_of_ExtTrig):
            print('data file error!')

        Isub_DC_CounterIdleHigh_10min = np.zeros(Num_of_steps)
        for Isub_idx in np.arange(0, Num_of_steps):
            Isub_DC_CounterIdleHigh_10min[Isub_idx] = 1e9*np.mean(np.float64(Isub_without_pumping_CounterIdleHigh_10min[0][Isub_idx*Num_of_ExtTrig:(Isub_idx+1)*Num_of_ExtTrig]))
        if VSBD_descend == 1:
            plt.plot(np.arange(Vbot_Vsbd_min, Vbot_Vsbd_max+0.0001, Vbot_Vsbd_step), Isub_DC_CounterIdleHigh_10min, marker='.')
        if VSBD_descend == 0:
            plt.plot(np.arange(Vbot_Vsbd_max, Vbot_Vsbd_min-0.0001, (-1)*Vbot_Vsbd_step), Isub_DC_CounterIdleHigh_10min, marker='.')

        plt.title('L='+str(L)+', Nfin='+str(Nfin)+', '+VT_flavor+', '+'VSS_WL(Vbot)='+str(VSS_WL)+', VDD_WL='+str(VDD_WL)+', sweep VS=VB=VD(Vsbd), Isub_without_pumping_CounterIdleHigh_10min', fontsize=8)
        #plt.axis([VG_min, VG_max, 0, Ymax])
        plt.xlim(Vbot_Vsbd_min, Vbot_Vsbd_max)
        plt.xlabel('Vbot - Vsbd (V)')
        plt.ylabel('Isub_without_pumping_CounterIdleHigh_10min (nA)')
        plt.grid()
        plt.savefig(path_plot+plot_file+'_Isub_without_pumping_CounterIdleHigh_10min_Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+'.pdf')
        plt.close()
#Temperary: test settling time 


        Icp_DCcorrected_AverageHighLow = np.zeros((Num_of_freq, Num_of_steps))
        axe_handle = ['']*Num_of_freq
        for (freq_idx, counter1_freq) in zip(np.arange(Num_of_freq), pumping_freq):
            for Isub_idx in np.arange(0, Num_of_steps):
                Icp_DCcorrected_AverageHighLow[freq_idx][Isub_idx] = Isub_pumping[freq_idx][Isub_idx] - 0.5 * (Isub_DC[Isub_idx] + Isub_DC_CounterIdleHigh[Isub_idx])
            if freq_idx in plot_freq_idx_DCcorrected:
                if VSBD_descend == 1:
                    axe_handle[freq_idx], = plt.plot(np.arange(Vbot_Vsbd_min, Vbot_Vsbd_max+0.0001, Vbot_Vsbd_step), Icp_DCcorrected_AverageHighLow[freq_idx], marker='.')
                if VSBD_descend == 0:
                    axe_handle[freq_idx], = plt.plot(np.arange(Vbot_Vsbd_max, Vbot_Vsbd_min-0.0001, (-1)*Vbot_Vsbd_step), Icp_DCcorrected_AverageHighLow[freq_idx], marker='.')

        plt.title('L='+str(L)+', Nfin='+str(Nfin)+', '+VT_flavor+', '+'VSS_WL(Vbot)='+str(VSS_WL)+', VDD_WL='+str(VDD_WL)+', sweep VS=VB=VD(Vsbd), charge pumping curves, DC corrected by subtracting the average of DC WL high and low', fontsize=5)
        #plt.axis([VG_min, VG_max, 0, Ymax])
        plt.xlim(Vbot_Vsbd_min, Vbot_Vsbd_max)
        plt.xlabel('Vbot - Vsbd (V)')
        plt.ylabel('Icp = Isub_with_pumping-Isub_without_pumping-WL(high + low)*0.5 (nA)', fontsize=8)
        plt.grid()
        axe_to_plot = []
        legend_to_plot = []
        for plt_idx in plot_freq_idx_DCcorrected:
            axe_to_plot.append(axe_handle[plt_idx])
            legend_to_plot.append(freq_legend[plt_idx])
        plt.legend(axe_to_plot, legend_to_plot)
        plt.savefig(path_plot+plot_file+'Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+'Pumping_minus_DCcorrection_AverageHighLow.pdf')
        plt.close()
    """



def Charge_Pumping_compare(chips, curve_legend, col, L, Nfin, VT_flavor, Nrow, data_files, path_plot, plot_file, title, Vbot_Vsbd_min, Vbot_Vsbd_max, Vbot_Vsbd_step, Num_of_steps, VSS_WL, VDD_WL, Num_of_ExtTrig, Num_of_freq, pumping_freq, VSBD_descend, DCcorrection_freq_idx, freq_DCcorrection, plot_freq_idx_DCcorrected):

    if os.path.isdir(path_plot) == False:
        os.mkdir(path_plot)

    Vbot_Vsbd = np.arange(Vbot_Vsbd_min, Vbot_Vsbd_max + 0.0001, Vbot_Vsbd_step)

    axe_handle = []
    axe_norm_handle = []
    for (curve_idx, data_file) in zip(range(len(data_files)), data_files):
        """
        Isub_without_pumping = []
        f=open(data_file,'rU')
        Isub_without_pumping.append(re.findall(r'Isub_without_pumping=(-*\d*\.\d+)',f.read()))
        f.close()
        """
        Isub_with_pumping = []
        f=open(data_file,'rU')
        Isub_with_pumping.append(re.findall(r'Isub_with_pumping=(-*\d*\.\d+)',f.read()))
        f.close()
        """
        print(len(Isub_with_pumping[0]), len(Isub_without_pumping[0]))
        if (len(Isub_without_pumping[0]) != (Num_of_steps*Num_of_ExtTrig) or len(Isub_with_pumping[0]) != (Num_of_freq[curve_idx]*Num_of_steps*Num_of_ExtTrig)):
            print('data file error!')
        """
        print(len(Isub_with_pumping[0]))
        if (len(Isub_with_pumping[0]) != (Num_of_freq[curve_idx]*Num_of_steps*Num_of_ExtTrig)):
            print('data file error!')

        Isub_pumping = np.zeros((Num_of_freq[curve_idx], Num_of_steps))
        for (freq_idx, counter1_freq) in zip(range(Num_of_freq[curve_idx]), pumping_freq[curve_idx]):
            for Isub_idx in np.arange(0, Num_of_steps):
                Isub_pumping[freq_idx][Isub_idx] = 1e9*np.mean(np.float64(Isub_with_pumping[0][Num_of_freq[curve_idx]*Isub_idx*Num_of_ExtTrig+freq_idx*Num_of_ExtTrig : Num_of_freq[curve_idx]*Isub_idx*Num_of_ExtTrig+freq_idx*Num_of_ExtTrig+Num_of_ExtTrig]))
        
        """
        Isub_DC = np.zeros(Num_of_steps)
        for Isub_idx in np.arange(0, Num_of_steps):
            Isub_DC[Isub_idx] = 1e9*np.mean(np.float64(Isub_without_pumping[0][Isub_idx*Num_of_ExtTrig:(Isub_idx+1)*Num_of_ExtTrig]))
        """
        Icp_DCcorrected = np.zeros((Num_of_freq[curve_idx], Num_of_steps))
        for (freq_idx, counter1_freq) in zip(np.arange(Num_of_freq[curve_idx]), pumping_freq[curve_idx]):
            for Isub_idx in np.arange(0, Num_of_steps):
                Icp_DCcorrected[freq_idx][Isub_idx] = Isub_pumping[freq_idx][Isub_idx] - Isub_pumping[DCcorrection_freq_idx[curve_idx]][Isub_idx]
            if freq_idx in plot_freq_idx_DCcorrected[curve_idx]:
                if VSBD_descend == 1:
                    plt.figure(1)
                    axe, = plt.plot(np.arange(Vbot_Vsbd_min, Vbot_Vsbd_max+0.0001, Vbot_Vsbd_step), Icp_DCcorrected[freq_idx], marker='.')
                    plt.figure(2)
                    axe_norm, = plt.plot(np.arange(Vbot_Vsbd_min, Vbot_Vsbd_max+0.0001, Vbot_Vsbd_step), Icp_DCcorrected[freq_idx]/np.amax(Icp_DCcorrected[freq_idx]), marker='.')
                if VSBD_descend == 0:
                    plt.figure(1)
                    axe, = plt.plot(np.arange(Vbot_Vsbd_max, Vbot_Vsbd_min-0.0001, (-1)*Vbot_Vsbd_step), Icp_DCcorrected[freq_idx], marker='.')
                    plt.figure(2)
                    axe_norm, = plt.plot(np.arange(Vbot_Vsbd_max, Vbot_Vsbd_min-0.0001, (-1)*Vbot_Vsbd_step), Icp_DCcorrected[freq_idx]/np.amax(Icp_DCcorrected[freq_idx]), marker='.')
                axe_handle.append(axe)
                axe_norm_handle.append(axe_norm)

    plt.figure(1)
    plt.title(title+'L='+str(L)+', Nfin='+str(Nfin)+', '+VT_flavor+', '+'VSS_WL(Vbot)='+str(VSS_WL)+', VDD_WL='+str(VDD_WL)+', sweep VS=VB=VD(Vsbd), charge pumping curves DC corrected by '+freq_DCcorrection, fontsize=7)
    #plt.axis([VG_min, VG_max, 0, Ymax])
    plt.xlim(Vbot_Vsbd_min, Vbot_Vsbd_max)
    plt.xlabel('Vbot - Vsbd (V)')
    plt.ylabel('Icp = Isub_with_pumping-Isub_with_pumping @' + freq_DCcorrection +' (nA)')
    plt.grid()
    plt.legend(axe_handle, curve_legend, fontsize = 8)
    plt.savefig(path_plot+plot_file+'_Col'+str(col).zfill(2)+'Pumping_minus_DCcorrection.pdf')
    plt.close()

    plt.figure(2)
    plt.title(title+'L='+str(L)+', Nfin='+str(Nfin)+', '+VT_flavor+', '+'VSS_WL(Vbot)='+str(VSS_WL)+', VDD_WL='+str(VDD_WL)+', sweep VS=VB=VD(Vsbd), charge pumping curves DC corrected by '+freq_DCcorrection+', normalized by DC corrected Icp,max', fontsize=6)
    #plt.axis([VG_min, VG_max, 0, Ymax])
    plt.xlim(Vbot_Vsbd_min, Vbot_Vsbd_max)
    plt.xlabel('Vbot - Vsbd (V)')
    plt.ylabel('Icp/Icp,max')
    plt.grid()
    plt.legend(axe_norm_handle, curve_legend, fontsize = 8)
    plt.savefig(path_plot+plot_file+'_Col'+str(col).zfill(2)+'Pumping_minus_DCcorrection_normalized.pdf')
    plt.close()

if __name__ == '__main__':
  main()

