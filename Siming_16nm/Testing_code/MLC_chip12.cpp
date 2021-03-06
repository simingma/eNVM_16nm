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

	int chip = 12;
	int col;
	char Measure_file[200];

/*
	col = 21;
	double VDS_col21;
	double VGS_col21;

	sprintf(Measure_file, "C:/GoogleDrive/working/Fresh_Chip%02d_Col%02d_Ids_Vgs_VAsource_VBdrain", chip, col);
	IDS_VGS(Measure_file, col, chip, 0);
	sprintf(Measure_file, "C:/GoogleDrive/working/Fresh_Chip%02d_Col%02d_Ids_Vgs_VAdrain_VBsource", chip, col);
	IDS_VGS(Measure_file, col, chip, 1);

	/******* (VG, VD) for level 1 **********/
/*	VDS_col21 = 2.4;
	VGS_col21 = 0.9;

	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_programming_Chip%02d_Col%02d_200msPULSE_VG0p9_VD2p4_VAsource_VBdrain_01", chip, col);
	MLC_programming(Measure_file, VDS_col21, VGS_col21, "200ms", chip, col, 0, 1000, 100, 0.00008);

        sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_200msPULSE_VG0p9_VD2p4_Ids_Vgs_VAsource_VBdrain_01", chip, col);   
	IDS_VGS(Measure_file, col, chip, 0);                                       
        sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_200msPULSE_VG0p9_VD2p4_Ids_Vgs_VAdrain_VBsource_01", chip, col);   
	IDS_VGS(Measure_file, col, chip, 1);

	/******* (VG, VD) for level 2 **********/
/*        VDS_col21 = 2.4;
        VGS_col21 = 1.2;

	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_programming_Chip%02d_Col%02d_200msPULSE_VG1p2_VD2p4_VAsource_VBdrain_02", chip, col);
	MLC_programming(Measure_file, VDS_col21, VGS_col21, "200ms", chip, col, 0, 1000, 100, 0.00006);

        sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_200msPULSE_VG1p2_VD2p4_Ids_Vgs_VAsource_VBdrain_02", chip, col);   
	IDS_VGS(Measure_file, col, chip, 0);                                        
        sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_200msPULSE_VG1p2_VD2p4_Ids_Vgs_VAdrain_VBsource_02", chip, col);   
	IDS_VGS(Measure_file, col, chip, 1);


	/******* (VG, VD) for level 3 **********/
/*        VDS_col21 = 2.4;
        VGS_col21 = 1.5;

	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_programming_Chip%02d_Col%02d_200msPULSE_VG1p5_VD2p4_VAsource_VBdrain_03", chip, col);
	MLC_programming(Measure_file, VDS_col21, VGS_col21, "200ms", chip, col, 0, 1000, 100, 0.00004);

        sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_200msPULSE_VG1p5_VD2p4_Ids_Vgs_VAsource_VBdrain_03", chip, col);   
	IDS_VGS(Measure_file, col, chip, 0);                                       
        sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_200msPULSE_VG1p5_VD2p4_Ids_Vgs_VAdrain_VBsource_03", chip, col);   
	IDS_VGS(Measure_file, col, chip, 1);


	/******* (VG, VD) for level 4 **********/
/*        VDS_col21 = 2.4;
        VGS_col21 = 1.8;

	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_programming_Chip%02d_Col%02d_200msPULSE_VG1p8_VD2p4_VAsource_VBdrain_04", chip, col);
	MLC_programming(Measure_file, VDS_col21, VGS_col21, "200ms", chip, col, 0, 1000, 100, 0.00002);

	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_200msPULSE_VG1p8_VD2p4_Ids_Vgs_VAsource_VBdrain_04", chip, col);
	IDS_VGS(Measure_file, col, chip, 0);
	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_200msPULSE_VG1p8_VD2p4_Ids_Vgs_VAdrain_VBsource_04", chip, col);
	IDS_VGS(Measure_file, col, chip, 1);
	*/


	col = 33;

/*	sprintf(Measure_file, "C:/GoogleDrive/working/Fresh_Chip%02d_Col%02d_Ids_Vgs_VAsource_VBdrain", chip, col);
	IDS_VGS(Measure_file, col, chip, 0);
	sprintf(Measure_file, "C:/GoogleDrive/working/Fresh_Chip%02d_Col%02d_Ids_Vgs_VAdrain_VBsource", chip, col);
	IDS_VGS(Measure_file, col, chip, 1);
	*/

	double VDS_col33 = 2.0;
	double VGS_col33 = 1.8;

/*
	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_programming_Chip%02d_Col%02d_2msPULSE_VG1p8_VD2p0_VAsource_VBdrain_01", chip, col);
	MLC_programming(Measure_file, VDS_col33, VGS_col33, "2ms", chip, col, 0, 280, 0, 0.000106);

        sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_2msPULSE_VG1p8_VD2p0_Ids_Vgs_VAsource_VBdrain_01", chip, col);   
	IDS_VGS(Measure_file, col, chip, 0);                                       
        sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_2msPULSE_VG1p8_VD2p0_Ids_Vgs_VAdrain_VBsource_01", chip, col);   
	IDS_VGS(Measure_file, col, chip, 1);
	*/

        sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_2msPULSE_VG1p8_VD2p0_Ids_diode_VAsource_VBdrain_01", chip, col);   
	IDS_diode(Measure_file, col, chip, 0);                                       
        sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_2msPULSE_VG1p8_VD2p0_Ids_diode_VAdrain_VBsource_01", chip, col);   
	IDS_diode(Measure_file, col, chip, 1);


	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_programming_Chip%02d_Col%02d_10msPULSE_VG1p8_VD2p0_VAsource_VBdrain_02", chip, col);
	MLC_programming(Measure_file, VDS_col33, VGS_col33, "10ms", chip, col, 0, 280, 0, 0.000071);

        sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_10msPULSE_VG1p8_VD2p0_Ids_Vgs_VAsource_VBdrain_02", chip, col);   
	IDS_VGS(Measure_file, col, chip, 0);                                        
        sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_10msPULSE_VG1p8_VD2p0_Ids_Vgs_VAdrain_VBsource_02", chip, col);   
	IDS_VGS(Measure_file, col, chip, 1);

        sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_10msPULSE_VG1p8_VD2p0_Ids_diode_VAsource_VBdrain_02", chip, col);   
	IDS_diode(Measure_file, col, chip, 0);                                        
        sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_10msPULSE_VG1p8_VD2p0_Ids_diode_VAdrain_VBsource_02", chip, col);   
	IDS_diode(Measure_file, col, chip, 1);


	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_programming_Chip%02d_Col%02d_40msPULSE_VG1p8_VD2p0_VAsource_VBdrain_03", chip, col);
	MLC_programming(Measure_file, VDS_col33, VGS_col33, "40ms", chip, col, 0, 280, 0, 0.000042);

        sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_40msPULSE_VG1p8_VD2p0_Ids_Vgs_VAsource_VBdrain_03", chip, col);   
	IDS_VGS(Measure_file, col, chip, 0);                                       
        sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_40msPULSE_VG1p8_VD2p0_Ids_Vgs_VAdrain_VBsource_03", chip, col);   
	IDS_VGS(Measure_file, col, chip, 1);

        sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_40msPULSE_VG1p8_VD2p0_Ids_diode_VAsource_VBdrain_03", chip, col);   
	IDS_diode(Measure_file, col, chip, 0);                                       
        sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_40msPULSE_VG1p8_VD2p0_Ids_diode_VAdrain_VBsource_03", chip, col);   
	IDS_diode(Measure_file, col, chip, 1);


	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_programming_Chip%02d_Col%02d_200msPULSE_VG1p8_VD2p0_VAsource_VBdrain_04", chip, col);
	MLC_programming(Measure_file, VDS_col33, VGS_col33, "200ms", chip, col, 0, 280, 0, 0.0000205);

	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_200msPULSE_VG1p8_VD2p0_Ids_Vgs_VAsource_VBdrain_04", chip, col);
	IDS_VGS(Measure_file, col, chip, 0);
	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_200msPULSE_VG1p8_VD2p0_Ids_Vgs_VAdrain_VBsource_04", chip, col);
	IDS_VGS(Measure_file, col, chip, 1);

	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_200msPULSE_VG1p8_VD2p0_Ids_diode_VAsource_VBdrain_04", chip, col);
	IDS_diode(Measure_file, col, chip, 0);
	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_200msPULSE_VG1p8_VD2p0_Ids_diode_VAdrain_VBsource_04", chip, col);
	IDS_diode(Measure_file, col, chip, 1);


	// Turn off PSU outputs after tests are done!
	_ibwrt(_VDD_DIG_VDD_WL, "OUTP:STAT OFF");
	_ibwrt(_VSS_WL_VSS_PW, "OUTP:STAT OFF");
	_ibwrt(_VDD_IO_V_NIDAQ, "OUTP:STAT OFF");
	_ibwrt(_VSPARE_VAB, "OUTP:STAT OFF");

	GpibEquipmentUnInit();

	return 0;
}
