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

	int chip = 11;
	int col;
	char Measure_file[200];

/*	for (col = 0; col < 36; col++){
	    sprintf(Measure_file, "C:/GoogleDrive/working/Fresh_Chip%02d_Col%02d_Ids_Vgs_VAsource_VBdrain", chip, col);
	    IDS_VGS(Measure_file, col, chip, 0);
	    sprintf(Measure_file, "C:/GoogleDrive/working/Fresh_Chip%02d_Col%02d_Ids_Vgs_VAdrain_VBsource", chip, col);
	    IDS_VGS(Measure_file, col, chip, 1);
	}*/

/*	int col_list[3] = {18, 24, 30};

	double VGS[32] = {1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 
		          1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8};

	double VDS[3][32] = {{2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 	
		              2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4},
		             {1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8,  
			      2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2},
			     {1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7,  
			      2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0}};

       for(int i=0; i<3; i++){
	    col = col_list[i];

	    sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_40x10ms_stress_VG_ConstPulse_VAsource_VBdrain_01", chip, col);
	    stress_VG_ConstPulse(Measure_file, VDS[i], VGS, "10ms", chip, col, 0, 40);
            sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_40x10ms_Ids_Vgs_VAsource_VBdrain_01", chip, col);   
	    IDS_VGS(Measure_file, col, chip, 0);
            sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_40x10ms_Ids_Vgs_VAdrain_VGsource_01", chip, col);   
	    IDS_VGS(Measure_file, col, chip, 1);
	    
	    sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_20x40ms_stress_VG_ConstPulse_VAsource_VBdrain_02", chip, col);
	    stress_VG_ConstPulse(Measure_file, VDS[i], VGS, "40ms", chip, col, 0, 20);
            sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_20x40ms_Ids_Vgs_VAsource_VBdrain_02", chip, col);   
	    IDS_VGS(Measure_file, col, chip, 0);
            sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_20x40ms_Ids_Vgs_VAdrain_VGsource_02", chip, col);   
	    IDS_VGS(Measure_file, col, chip, 1);
	    
	    sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_12x200ms_stress_VG_ConstPulse_VAsource_VBdrain_03", chip, col);
	    stress_VG_ConstPulse(Measure_file, VDS[i], VGS, "200ms", chip, col, 0, 12);
            sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_12x200ms_Ids_Vgs_VAsource_VBdrain_03", chip, col);   
	    IDS_VGS(Measure_file, col, chip, 0);
            sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_12x200ms_Ids_Vgs_VAdrain_VGsource_03", chip, col);   
	    IDS_VGS(Measure_file, col, chip, 1);
       }
       */
	
	/******In reality, didn't run the following charge pumping test this time********/
/*	double CP_VDD_DIG = 1.6;
	double CP_VSS_WL = 0;
	double CP_VDD_WL = 1.6;
	double Num_of_ExtTrig = 60;

	double samp_rate = 10.0;
	double pumping_freq[2] = { 5000000, 1000 };
	int Num_of_freq = 2;
	
        int Num_of_VDBS = 17;
	double VDBS_list_Vr0[17] = { 0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6 };

	int col_list_128[3] = {21, 27, 33}
	for (int i = 0; i < 3; i++){
	    col = col_list_128[i];
	    sprintf(Measure_file, "C:/GoogleDrive/working/Fresh_Chip%02d_Col%02d_60Pumping_SweepVSVBVD_VSS_WL_0_VDD_WL_1p6_ELTM-Range200nA", chip, col);
	    Charge_Pumping_ELTM(Measure_file, VDBS_list_Vr0, Num_of_VDBS, CP_VDD_DIG, CP_VSS_WL, CP_VDD_WL, "ExtTrig_60_0p1sWidth_1sInterval", samp_rate, Num_of_freq, pumping_freq, Num_of_ExtTrig, chip, col, 0);
	}*/ 
	/******In reality, didn't run the above charge pumping test this time********/

/*	col = 21;*/

/*	sprintf(Measure_file, "C:/GoogleDrive/working/Fresh_Chip%02d_Col%02d_Ids_Vgs_VAsource_VBdrain", chip, col);
	IDS_VGS(Measure_file, col, chip, 0);
	sprintf(Measure_file, "C:/GoogleDrive/working/Fresh_Chip%02d_Col%02d_Ids_Vgs_VAdrain_VBsource", chip, col);
	IDS_VGS(Measure_file, col, chip, 1);*/
/*
	double VDS_col21 = 2.4;
	double VGS_col21 = 1.8;
*/
/*	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_programming_Chip%02d_Col%02d_2msPULSE_VG1p8_VD2p4_VAsource_VBdrain_01", chip, col);
	MLC_programming(Measure_file, VDS_col21, VGS_col21, "2ms", chip, col, 0, 150, 20, 0.00008);

        sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_2msPULSE_VG1p8_VD2p4_Ids_Vgs_VAsource_VBdrain_01", chip, col);   
	IDS_VGS(Measure_file, col, chip, 0);                                       
        sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_2msPULSE_VG1p8_VD2p4_Ids_Vgs_VAdrain_VBsource_01", chip, col);   
	IDS_VGS(Measure_file, col, chip, 1);*/


/*	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_programming_Chip%02d_Col%02d_10msPULSE_VG1p8_VD2p4_VAsource_VBdrain_02", chip, col);
	MLC_programming(Measure_file, VDS_col21, VGS_col21, "10ms", chip, col, 0, 170, 70, 0.00006);

        sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_10msPULSE_VG1p8_VD2p4_Ids_Vgs_VAsource_VBdrain_02", chip, col);   
	IDS_VGS(Measure_file, col, chip, 0);                                        
        sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_10msPULSE_VG1p8_VD2p4_Ids_Vgs_VAdrain_VBsource_02", chip, col);   
	IDS_VGS(Measure_file, col, chip, 1);


	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_programming_Chip%02d_Col%02d_40msPULSE_VG1p8_VD2p4_VAsource_VBdrain_03", chip, col);
	MLC_programming(Measure_file, VDS_col21, VGS_col21, "40ms", chip, col, 0, 170, 70, 0.00004);

        sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_40msPULSE_VG1p8_VD2p4_Ids_Vgs_VAsource_VBdrain_03", chip, col);   
	IDS_VGS(Measure_file, col, chip, 0);                                       
        sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_40msPULSE_VG1p8_VD2p4_Ids_Vgs_VAdrain_VBsource_03", chip, col);   
	IDS_VGS(Measure_file, col, chip, 1);*/

/*	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_programming_Chip%02d_Col%02d_200msPULSE_VG1p8_VD2p4_VAsource_VBdrain_04", chip, col);
	MLC_programming(Measure_file, VDS_col21, VGS_col21, "200ms", chip, col, 0, 280, 80, 0.00002);

	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_200msPULSE_VG1p8_VD2p4_Ids_Vgs_VAsource_VBdrain_04", chip, col);
	IDS_VGS(Measure_file, col, chip, 0);
	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_200msPULSE_VG1p8_VD2p4_Ids_Vgs_VAdrain_VBsource_04", chip, col);
	IDS_VGS(Measure_file, col, chip, 1);
*/

	int col_list[3] = {18, 24, 30};

	double VGS[32] = {1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 
		          1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8};

	double VDS[3][32] = {{2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 	
		              2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4, 2.4},
		             {1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8,  
			      2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2, 2.2},
			     {1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7,  
			      2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0}};

       //for(int i=0; i<3; i++){
       int i = 2; //only characterize col[30]
	    col = col_list[i];

/*	    sprintf(Measure_file, "C:/GoogleDrive/working/Fresh_Chip%02d_Col%02d_Ids_Vgs_VAsource_VBdrain", chip, col);
	    IDS_VGS(Measure_file, col, chip, 0);
	    sprintf(Measure_file, "C:/GoogleDrive/working/Fresh_Chip%02d_Col%02d_Ids_Vgs_VAdrain_VBsource", chip, col);
	    IDS_VGS(Measure_file, col, chip, 1);

	    sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_40x10ms_stress_VG_ConstPulse_VAsource_VBdrain_01", chip, col);
	    stress_VG_ConstPulse(Measure_file, VDS[i], VGS, "10ms", chip, col, 0, 40);
            sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_40x10ms_Ids_Vgs_VAsource_VBdrain_01", chip, col);   
	    IDS_VGS(Measure_file, col, chip, 0);
            sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_40x10ms_Ids_Vgs_VAdrain_VGsource_01", chip, col);   
	    IDS_VGS(Measure_file, col, chip, 1);
	    
	    sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_80x10ms_stress_VG_ConstPulse_VAsource_VBdrain_02", chip, col);
	    stress_VG_ConstPulse(Measure_file, VDS[i], VGS, "10ms", chip, col, 0, 80);
            sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_80x10ms_Ids_Vgs_VAsource_VBdrain_02", chip, col);   
	    IDS_VGS(Measure_file, col, chip, 0);
            sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_80x10ms_Ids_Vgs_VAdrain_VGsource_02", chip, col);   
	    IDS_VGS(Measure_file, col, chip, 1);
	    
	    sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_240x10ms_stress_VG_ConstPulse_VAsource_VBdrain_03", chip, col);
	    stress_VG_ConstPulse(Measure_file, VDS[i], VGS, "10ms", chip, col, 0, 240);
            sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_240x10ms_Ids_Vgs_VAsource_VBdrain_03", chip, col);   
	    IDS_VGS(Measure_file, col, chip, 0);
            sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_240x10ms_Ids_Vgs_VAdrain_VGsource_03", chip, col);   
	    IDS_VGS(Measure_file, col, chip, 1);
*/
		sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_180x40ms_stress_VG_ConstPulse_VAsource_VBdrain_04", chip, col);
		stress_VG_ConstPulse(Measure_file, VDS[i], VGS, "40ms", chip, col, 0, 180);
		sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_180x40ms_Ids_Vgs_VAsource_VBdrain_04", chip, col);
		IDS_VGS(Measure_file, col, chip, 0);
		sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_180x40ms_Ids_Vgs_VAdrain_VGsource_04", chip, col);
		IDS_VGS(Measure_file, col, chip, 1);

		i = 0; //only characterize col[18]
		col = col_list[i];

		sprintf(Measure_file, "C:/GoogleDrive/working/Fresh_Chip%02d_Col%02d_Ids_Vgs_VAsource_VBdrain", chip, col);
		IDS_VGS(Measure_file, col, chip, 0);
		sprintf(Measure_file, "C:/GoogleDrive/working/Fresh_Chip%02d_Col%02d_Ids_Vgs_VAdrain_VBsource", chip, col);
		IDS_VGS(Measure_file, col, chip, 1);

		sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_40x10ms_stress_VG_ConstPulse_VAsource_VBdrain_01", chip, col);
		stress_VG_ConstPulse(Measure_file, VDS[i], VGS, "10ms", chip, col, 0, 40);
		sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_40x10ms_Ids_Vgs_VAsource_VBdrain_01", chip, col);
		IDS_VGS(Measure_file, col, chip, 0);
		sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_40x10ms_Ids_Vgs_VAdrain_VGsource_01", chip, col);
		IDS_VGS(Measure_file, col, chip, 1);

		sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_80x10ms_stress_VG_ConstPulse_VAsource_VBdrain_02", chip, col);
		stress_VG_ConstPulse(Measure_file, VDS[i], VGS, "10ms", chip, col, 0, 80);
		sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_80x10ms_Ids_Vgs_VAsource_VBdrain_02", chip, col);
		IDS_VGS(Measure_file, col, chip, 0);
		sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_80x10ms_Ids_Vgs_VAdrain_VGsource_02", chip, col);
		IDS_VGS(Measure_file, col, chip, 1);

		sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_240x10ms_stress_VG_ConstPulse_VAsource_VBdrain_03", chip, col);
		stress_VG_ConstPulse(Measure_file, VDS[i], VGS, "10ms", chip, col, 0, 240);
		sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_240x10ms_Ids_Vgs_VAsource_VBdrain_03", chip, col);
		IDS_VGS(Measure_file, col, chip, 0);
		sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_240x10ms_Ids_Vgs_VAdrain_VGsource_03", chip, col);
		IDS_VGS(Measure_file, col, chip, 1);

		sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_180x40ms_stress_VG_ConstPulse_VAsource_VBdrain_04", chip, col);
		stress_VG_ConstPulse(Measure_file, VDS[i], VGS, "40ms", chip, col, 0, 180);
		sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_180x40ms_Ids_Vgs_VAsource_VBdrain_04", chip, col);
		IDS_VGS(Measure_file, col, chip, 0);
		sprintf(Measure_file, "C:/GoogleDrive/working/Chip%02d_Col%02d_HCI_180x40ms_Ids_Vgs_VAdrain_VGsource_04", chip, col);
		IDS_VGS(Measure_file, col, chip, 1);

       //}
	// Turn off PSU outputs after tests are done!
	_ibwrt(_VDD_DIG_VDD_WL, "OUTP:STAT OFF");
	_ibwrt(_VSS_WL_VSS_PW, "OUTP:STAT OFF");
	_ibwrt(_VDD_IO_V_NIDAQ, "OUTP:STAT OFF");
	_ibwrt(_VSPARE_VAB, "OUTP:STAT OFF");

	GpibEquipmentUnInit();

	return 0;
}
