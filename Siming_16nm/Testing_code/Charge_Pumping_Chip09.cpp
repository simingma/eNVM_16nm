/********************************************************
Main Program
********************************************************/
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "StdAfx.h"
#include "NIDAQmx.h"
#include <stdlib.h>
#include <windows.h>
#include "ni488.h"
#include <stdio.h>
#include <string>
#include <visa.h>
#include <time.h>
#include "TestFunctions.h"

#include <ostream>
#include <conio.h>
#include <strstream>
#include <assert.h>
#include <comdef.h>

char outchannel_str[] = "Dev1/port0/line0:7";
char inchannel_str[] = "Dev1/ai0";
char outchannel_str_USB6008[] = "Dev2/port0/line0:7,Dev2/port1/line0:1";

int scanChainRead_Error = 0;

int Num_of_row[36] = { 32, 32, 32, 128, 128, 128, 32, 32, 32, 128, 128, 128, 32, 32, 32, 128, 128, 128, 32, 32, 32, 128, 128, 128, 32, 32, 32, 128, 128, 128, 32, 32, 32, 128, 128, 128 };
double VDD_typical = 0.8;

int main(void) {

	GpibEquipmentInit();
	// Setting up all voltages:

	//ATTENTION!
	E3646A_SetVoltage_CurrentLimit(_VDD_DIG_VDD_WL, 1, VDD_typical, 1.2); //VDD_DIG=VDD_typicalV, 1200mA limit
	E3646A_SetVoltage_CurrentLimit(_VDD_DIG_VDD_WL, 2, VDD_typical, 0.2); //VDD_WL=VDD_typicalV, 200mA limit

	//ATTENTION: increase VSS_WL current limit
	E3646A_SetVoltage_CurrentLimit(_VSS_WL_VSS_PW, 1, 0, 0.2);    //VSS_WL=0V, 12mA limit

	//ATTENTION! might need to increase the current limit on VSS_PW for erasing!!!
	E3646A_SetVoltage_CurrentLimit(_VSS_WL_VSS_PW, 2, 0, 0.03);    //VSS_PW=0V, 30mA limit
	//E3646A_SetVoltage_CurrentLimit(_VSS_WL_VSS_PW, 2, 0, 0.012);    //VSS_PW=0V, 12mA limit

	// double VALUE_VDD_IO = 1.8;
	double VALUE_VDD_IO = 2.4;
	//	E3646A_SetVoltage_CurrentLimit(_VDD_IO_V_NIDAQ, 1, 1.8, 0.01); //VDD_IO=1.8V, 10mA limit
	E3646A_SetVoltage_CurrentLimit(_VDD_IO_V_NIDAQ, 1, VALUE_VDD_IO, 0.05); //VDD_IO=2.4V, VAB to VDD_IO (ESD clamp) diode 0.6~0.7V turn on
	//ATTENTION!


	//      E3646A_SetVoltage_CurrentLimit(_VDD_IO_V_NIDAQ, 2, 5.0, 0.02); //V_NIDAQ=5V, 10mA limit 
	// limit quiescent DC current on V_NIDAQ!!!
	//_VSPARE is used for VS source terminal ("PSU_NEG")
	E3646A_SetVoltage_CurrentLimit(_VSPARE_VAB, 1, 0, 0.02);
	//E3646A_SetVoltage_CurrentLimit(_VSPARE_VAB, 1, 0, 0.012);
	//the positive output of VAB connects to the VD drain terminal ("PSU_POS"), while its negative output is grounded
	E3646A_SetVoltage_CurrentLimit(_VSPARE_VAB, 2, 0.0, 0.02);
	//E3646A_SetVoltage_CurrentLimit(_VSPARE_VAB, 2, 0.0, 0.012);     // 12mA limit, same as DMM range  
	// *** configure |VB - VA|=0V before turning on PSU output!***
	// Turn on PSU outputs:
	_ibwrt(_VDD_DIG_VDD_WL, "OUTP:STAT ON");
	_ibwrt(_VSS_WL_VSS_PW, "OUTP:STAT ON");
	_ibwrt(_VDD_IO_V_NIDAQ, "OUTP:STAT ON");
	_ibwrt(_VSPARE_VAB, "OUTP:STAT ON");

	/* (2) using the implicit timing, software on demand */
	scan_selfcheck("../Scan_files/NIDAQ_test_data", 1);
	//fool-proof, should alway work

	scan("../Scan_files/Scan_all_zero", 0, 100000.0);

	int chip = 9;
	int col;
	char Measure_file[200];

//	double CP_VDD_DIG = 1.7;
	double CP_VDD_DIG = 1.6;
	double CP_VSS_WL = 0;
//	double CP_VDD_WL = 1.7;
	double CP_VDD_WL = 1.6;
	double Num_of_ExtTrig = 60;

	double samp_rate = 10.0;
	//	double pumping_freq = 5000000.0;
	double pumping_freq[2] = { 5000000, 1000 };
	int Num_of_freq = 2;

        int Num_of_VDBS = 17;
	double VDBS_list_Vr0[17] = { 0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6 };


	int col_list[4] = {21, 23, 33, 35};
/*	for (int i = 0; i < 4; i++){
	    col = col_list[i];
	    sprintf(Measure_file, "C:/GoogleDrive/working/Fresh_Chip%02d_Col%02d_60Pumping_SweepVSVBVD_VSS_WL_0_VDD_WL_1p6_ELTM", chip, col);
	    Charge_Pumping_ELTM(Measure_file, VDBS_list_Vr0, Num_of_VDBS, CP_VDD_DIG, CP_VSS_WL, CP_VDD_WL, "ExtTrig_60_0p1sWidth_1sInterval", samp_rate, Num_of_freq, pumping_freq, Num_of_ExtTrig, chip, col, 0);
	}*/

	for (int i = 0; i < 4; i++){
	    col = col_list[i];
	    sprintf(Measure_file, "C:/GoogleDrive/working/HCIstress01_1x1p2s_Chip%02d_Col%02d_60Pumping_SweepVSVBVD_VSS_WL_0_VDD_WL_1p6_ELTM", chip, col);
	    Charge_Pumping_ELTM(Measure_file, VDBS_list_Vr0, Num_of_VDBS, CP_VDD_DIG, CP_VSS_WL, CP_VDD_WL, "ExtTrig_60_0p1sWidth_1sInterval", samp_rate, Num_of_freq, pumping_freq, Num_of_ExtTrig, chip, col, 0);
	}

/*	for (int i = 0; i < 4; i++){
	    col = col_list[i];
	    sprintf(Measure_file, "C:/GoogleDrive/working/Fresh_Chip%02d_Col%02d_Ids_Vgs_VAsource_VBdrain", chip, col);
	    IDS_VGS(Measure_file, col, chip, 0);
	    sprintf(Measure_file, "C:/GoogleDrive/working/Fresh_Chip%02d_Col%02d_Ids_Vgs_VAdrain_VBsource", chip, col);
	    IDS_VGS(Measure_file, col, chip, 1);
	}

	double VGS[128] = { 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 
						1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 
						1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 
						1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8 };

	int Num_of_Pulse = 1;
	int PulseCycle;

	col = 21;
	double VDS_col21[128] = { 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
		                    2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
							2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
							2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0 };
	for (PulseCycle = 1; PulseCycle <= 1; PulseCycle++){
	    sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_stress_VG_ConstPulse_VG1p8_VD2p0_1x1p2s_VAsource_VBdrain_%02d", chip, col, PulseCycle);
	    stress_VG_ConstPulse(Measure_file, VDS_col21, VGS, "1200ms", chip, col, 0, Num_of_Pulse);

	    sprintf(Measure_file, "C:/GoogleDrive/working/HCI_VG1p8_VD2p0_1x1p2s_Chip%02d_Col%02d_Ids_Vgs_VAsource_VBdrain_%02d", chip, col, PulseCycle);
	    IDS_VGS(Measure_file, col, chip, 0);
	    sprintf(Measure_file, "C:/GoogleDrive/working/HCI_VG1p8_VD2p0_1x1p2s_Chip%02d_Col%02d_Ids_Vgs_VAdrain_VBsource_%02d", chip, col, PulseCycle);
	    IDS_VGS(Measure_file, col, chip, 1);
	}

	col = 23;
	double VDS_col23[128] = { 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4,
		2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4,
		2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4,
		2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4 };
	for (PulseCycle = 1; PulseCycle <= 1; PulseCycle++){
	    sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_stress_VG_ConstPulse_VG1p8_VD2p4_1x1p2s_VAsource_VBdrain_%02d", chip, col, PulseCycle);
	    stress_VG_ConstPulse(Measure_file, VDS_col23, VGS, "1200ms", chip, col, 0, Num_of_Pulse);

	    sprintf(Measure_file, "C:/GoogleDrive/working/HCI_VG1p8_VD2p4_1x1p2s_Chip%02d_Col%02d_Ids_Vgs_VAsource_VBdrain_%02d", chip, col, PulseCycle);
	    IDS_VGS(Measure_file, col, chip, 0);
	    sprintf(Measure_file, "C:/GoogleDrive/working/HCI_VG1p8_VD2p4_1x1p2s_Chip%02d_Col%02d_Ids_Vgs_VAdrain_VBsource_%02d", chip, col, PulseCycle);
	    IDS_VGS(Measure_file, col, chip, 1);
	}

	col = 33;
	double VDS_col33[128] = { 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
		2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
		2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
		2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0 };
	for (PulseCycle = 1; PulseCycle <= 1; PulseCycle++){
	    sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_stress_VG_ConstPulse_VG1p8_VD2p0_1x1p2s_VAsource_VBdrain_%02d", chip, col, PulseCycle);
	    stress_VG_ConstPulse(Measure_file, VDS_col33, VGS, "1200ms", chip, col, 0, Num_of_Pulse);

	    sprintf(Measure_file, "C:/GoogleDrive/working/HCI_VG1p8_VD2p0_1x1p2s_Chip%02d_Col%02d_Ids_Vgs_VAsource_VBdrain_%02d", chip, col, PulseCycle);
	    IDS_VGS(Measure_file, col, chip, 0);
	    sprintf(Measure_file, "C:/GoogleDrive/working/HCI_VG1p8_VD2p0_1x1p2s_Chip%02d_Col%02d_Ids_Vgs_VAdrain_VBsource_%02d", chip, col, PulseCycle);
	    IDS_VGS(Measure_file, col, chip, 1);
	}

	col = 35;
	double VDS_col35[128] = { 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7,
		1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7,
		1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7,
		1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7 };
	for (PulseCycle = 1; PulseCycle <= 1; PulseCycle++){
	    sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_stress_VG_ConstPulse_VG1p8_VD1p7_1x1p2s_VAsource_VBdrain_%02d", chip, col, PulseCycle);
	    stress_VG_ConstPulse(Measure_file, VDS_col35, VGS, "1200ms", chip, col, 0, Num_of_Pulse);

	    sprintf(Measure_file, "C:/GoogleDrive/working/HCI_VG1p8_VD1p7_1x1p2s_Chip%02d_Col%02d_Ids_Vgs_VAsource_VBdrain_%02d", chip, col, PulseCycle);
	    IDS_VGS(Measure_file, col, chip, 0);
	    sprintf(Measure_file, "C:/GoogleDrive/working/HCI_VG1p8_VD1p7_1x1p2s_Chip%02d_Col%02d_Ids_Vgs_VAdrain_VBsource_%02d", chip, col, PulseCycle);
	    IDS_VGS(Measure_file, col, chip, 1);
	}
*/
	// Turn off PSU outputs after tests are done!
	_ibwrt(_VDD_DIG_VDD_WL, "OUTP:STAT OFF");
	_ibwrt(_VSS_WL_VSS_PW, "OUTP:STAT OFF");
	_ibwrt(_VDD_IO_V_NIDAQ, "OUTP:STAT OFF");
	_ibwrt(_VSPARE_VAB, "OUTP:STAT OFF");

	GpibEquipmentUnInit();

	return 0;
}
