
import sys
import re
import os
import commands
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
#from scipy.interpolate import interp1d

def main():

    VD_values = 3 
    # VD applied to 10 transistors, followed by 2 transistors VD=0 (FN-tunneling or so-called PBTI CVS)
    # (10+2)*3=36
    write_time = 10
    pulse_length = 0.04
    # measure thourough I-V curves 10*0.04s pulses = 0.4s 

    PulseCycle = 3 # Cycled for 3 times: in total 3*0.4s = 1.2s 

    VGS = np.array([
	[ 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3 ], 

	[ 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3 ], 

	[ 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 
	  2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 
	  2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 
	  2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 
	  2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 
	  2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 
	  2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 
	  2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 
	  2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2 ], 

	[ 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 
	  2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 
	  2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 
	  2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 
	  2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 
	  2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 
	  2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 
	  2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 
	  2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2 ], 

	[ 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3 ], 

	[ 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 
	  2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3, 2.3 ], 
	])

    VDS = np.array([ 
	[ 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0, 0,
	  2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0, 0,
	  2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0, 0,
	  2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0, 0,
	  2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0, 0,
	  2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0, 0,
	  2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0, 0,
	  2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0, 0,
	  2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0, 0 ],

	[ 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0, 0,
	  2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0, 0,
	  2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0, 0,
	  2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0, 0,
	  2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0, 0,
	  2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0, 0,
	  2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0, 0,
	  2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0, 0,
	  2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0, 0 ],

	[ 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 0, 0,
	  2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 0, 0,
	  2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 0, 0,
	  2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 0, 0,
	  2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 0, 0,
	  2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 0, 0,
	  2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 0, 0,
	  2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 0, 0,
	  2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 0, 0 ],

	[ 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 0, 0,
	  2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 0, 0,
	  2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 0, 0,
	  2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 0, 0,
	  2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 0, 0,
	  2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 0, 0,
	  2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 0, 0,
	  2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 0, 0,
	  2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 2.7, 0, 0 ],

	[ 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 0, 0,
	  3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 0, 0,
	  3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 0, 0,
	  3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 0, 0,
	  3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 0, 0,
	  3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 0, 0,
	  3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 0, 0,
	  3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 0, 0,
	  3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 0, 0 ],

	[ 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 0, 0,
	  3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 0, 0,
	  3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 0, 0,
	  3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 0, 0,
	  3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 0, 0,
	  3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 0, 0,
	  3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 0, 0,
	  3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 0, 0,
	  3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 3.2, 0, 0 ]
	])


    WL = ['100/30','210/30','100/60','210/60','100/90','210/90']
    VT_FLAVOR = ['SVT','LVT','ULVT']

    chip = 16
    column = 6
    path_data = '../data/VG_ConstPulse_chip16/'
    path_plot = '../plot/VG_ConstPulse_chip16_violin_IDSAT/'
 #   os.mkdir(path_plot)

    IDSAT_VAsource_VBdrain = np.zeros((column, 108, PulseCycle*(1+1+write_time+1)))
    IDSAT_VAdrain_VBsource = np.zeros((column, 108, PulseCycle*(1+1+write_time+1)))

    for col in np.arange(0, column):
        for cycle in np.arange(0, PulseCycle):
            IDSAT_VAsource_VBdrain_all = []
            IDSAT_VAdrain_VBsource_all = []
            f0=open(path_data+'Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+'_stress_VG_ConstPulse_VAsource_VBdrain_'+str(cycle+1).zfill(2),'rU')
            IDSAT_VAsource_VBdrain_all.append(re.findall(r'_IDSAT_WL\[\d+\]_VAsource_VBdrain=(-*\d*\.\d+)',f0.read()))
            f0.close()
            f0=open(path_data+'Chip'+str(chip).zfill(2)+'_Col'+str(col).zfill(2)+'_stress_VG_ConstPulse_VAsource_VBdrain_'+str(cycle+1).zfill(2),'rU')
            IDSAT_VAdrain_VBsource_all.append(re.findall(r'_IDSAT_WL\[\d+\]_VAdrain_VBsource=(-*\d*\.\d+)',f0.read()))
            f0.close()

            print(len(IDSAT_VAsource_VBdrain_all), len(IDSAT_VAsource_VBdrain_all[0])) # should be (1, 108*(1+(1+write_time)+1)) = (1, 1404)
            print(len(IDSAT_VAdrain_VBsource_all), len(IDSAT_VAdrain_VBsource_all[0])) # should be (1, 108*(1+(1+write_time)+1)) = (1, 1404)

#TODO: figure out the resolution/bits of the floating point current variable!!!
            for row in np.arange(0, 108):
                IDSAT_VAsource_VBdrain[col][row][cycle*(1+write_time+1+1)+0] = np.float64(IDSAT_VAsource_VBdrain_all[0][row])
                IDSAT_VAsource_VBdrain[col][row][cycle*(1+write_time+1+1)+1: (cycle+1)*(1+write_time+1+1)-1] = np.float64(IDSAT_VAsource_VBdrain_all[0][108+row*(write_time+1): 108+(row+1)*(write_time+1)])
                IDSAT_VAsource_VBdrain[col][row][(cycle+1)*(1+write_time+1+1)-1] = np.float64(IDSAT_VAsource_VBdrain_all[0][108+108*(write_time+1)+row])
                IDSAT_VAdrain_VBsource[col][row][cycle*(1+write_time+1+1)+0] = np.float64(IDSAT_VAdrain_VBsource_all[0][row])
                IDSAT_VAdrain_VBsource[col][row][cycle*(1+write_time+1+1)+1: (cycle+1)*(1+write_time+1+1)-1] = np.float64(IDSAT_VAdrain_VBsource_all[0][108+row*(write_time+1): 108+(row+1)*(write_time+1)])
                IDSAT_VAdrain_VBsource[col][row][(cycle+1)*(1+write_time+1+1)-1] = np.float64(IDSAT_VAdrain_VBsource_all[0][108+108*(write_time+1)+row])

        #Imin = 1e6*(np.amin(np.amin(IDSAT_VAsource_VBdrain[col]), np.amin(IDSAT_VAdrain_VBsource[col])) - 20e-6)
        #Imax = 1e6*(np.amax(np.amax(IDSAT_VAsource_VBdrain[col]), np.amax(IDSAT_VAdrain_VBsource[col])) + 2e-6)

        time_index = [1, 11, 12, 24, 25, 37, 38]
        t_ticks = 3 + np.array([1, 6, 10, 15, 19, 24, 28])
        time_label = ['t=0\nfresh', '400ms\nimmediately', '~minutes\nrecovery', '800ms\nimmediately', 'minutes\nrecovery', '1200ms\nimmediately', '~minutes\nrecovery']

        for VT in np.arange(len(VT_FLAVOR)):
            figN = (col*len(VT_FLAVOR) + VT)*2
            plt.figure(figN)
            plt.title('W/L='+WL[col]+', '+ VT_FLAVOR[VT]+', VDS='+str(VDS[col][VT*36])+', VGS='+str(VGS[col][VT*36])+'\nIDSAT distribution of 30 fresh cells\nmeasured at VGS=0.9, VDS=0.9, stress(forward) direction', fontsize=10)
            IDSAT_fresh = 1e6*np.append(IDSAT_VAsource_VBdrain[col, VT*36: VT*36+10, 1], np.append(IDSAT_VAsource_VBdrain[col, VT*36+12: VT*36+22, 1], IDSAT_VAsource_VBdrain[col, VT*36+24: VT*36+34, 1]))
            plt.violinplot(IDSAT_fresh, positions=[t_ticks[0],], showmeans=True, showextrema=True, widths=3, points=100, bw_method=1)
            plt.grid()
            #plt.xlabel('IDSAT (uA)')
            plt.ylabel('IDSAT (uA)')

            for time in np.arange(1, 2*PulseCycle+1):
                IDSAT_at_time = 1e6*np.append(IDSAT_VAsource_VBdrain[col, VT*36: VT*36+10, time_index[time]], np.append(IDSAT_VAsource_VBdrain[col, VT*36+12: VT*36+22, time_index[time]], IDSAT_VAsource_VBdrain[col, VT*36+24: VT*36+34, time_index[time]]))
                plt.violinplot(IDSAT_at_time, positions=[t_ticks[time],], showmeans=True, showextrema=True, widths=3, points=100, bw_method=1)
            
            plt.xticks(t_ticks, time_label, rotation=30, fontsize=10)
            plt.subplots_adjust(bottom=0.12)
            plt.savefig(path_plot+'Violin_IDSAT_'+str(figN).zfill(3)+'_Col'+str(col).zfill(2)+'_'+VT_FLAVOR[VT]+'.pdf')
            figN = figN+1

            plt.figure(figN)
            plt.title('W/L='+WL[col]+', '+ VT_FLAVOR[VT]+', VDS='+str(VDS[col][VT*36])+', VGS='+str(VGS[col][VT*36])+'\nIDSAT distribution of 30 fresh cells\nmeasured at VGS=0.9, VDS=0.9, reversed direction', fontsize=10)
            IDSAT_fresh = 1e6*np.append(IDSAT_VAdrain_VBsource[col, VT*36: VT*36+10, 1], np.append(IDSAT_VAdrain_VBsource[col, VT*36+12: VT*36+22, 1], IDSAT_VAdrain_VBsource[col, VT*36+24: VT*36+34, 1]))
            plt.violinplot(IDSAT_fresh, positions=[t_ticks[0],], showmeans=True, showextrema=True, widths=3, points=100, bw_method=1)
            plt.grid()
            #plt.xlabel('IDSAT (uA)')
            plt.ylabel('IDSAT (uA)')

            for time in np.arange(1, 2*PulseCycle+1):
                IDSAT_at_time = 1e6*np.append(IDSAT_VAdrain_VBsource[col, VT*36: VT*36+10, time_index[time]], np.append(IDSAT_VAdrain_VBsource[col, VT*36+12: VT*36+22, time_index[time]], IDSAT_VAdrain_VBsource[col, VT*36+24: VT*36+34, time_index[time]]))
                plt.violinplot(IDSAT_at_time, positions=[t_ticks[time],], showmeans=True, showextrema=True, widths=3, points=100, bw_method=1)
            
            plt.xticks(t_ticks, time_label, rotation=30, fontsize=10)
            plt.subplots_adjust(bottom=0.12)
            plt.savefig(path_plot+'Violin_IDSAT_'+str(figN).zfill(3)+'_Col'+str(col).zfill(2)+'_'+VT_FLAVOR[VT]+'.pdf')


if __name__ == '__main__':
  main()

