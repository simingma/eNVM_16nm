
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

#include <winbase.h>
#include "WAModbus.h"


char outchannel_str[] = "Dev1/port0/line0:7";
char inchannel_str[] = "Dev1/ai0";
char outchannel_str_USB6008[] = "Dev2/port0/line0:7,Dev2/port1/line0:1";

int scanChainRead_Error = 0;

int Num_of_row[36] = { 32, 32, 32, 128, 128, 128, 32, 32, 32, 128, 128, 128, 32, 32, 32, 128, 128, 128, 32, 32, 32, 128, 128, 128, 32, 32, 32, 128, 128, 128, 32, 32, 32, 128, 128, 128 };
double VDD_typical = 0.8;

int powerOFF_bake_powerON(char *Measure_file, short room_temperature, short bake_temperature, DWORD bake_time) {
	// Turn off PSU outputs before increasing temperature: unbiased baking
	_ibwrt(_VDD_DIG_VDD_WL, "OUTP:STAT OFF");
	_ibwrt(_VSS_WL_VSS_PW, "OUTP:STAT OFF");
	_ibwrt(_VDD_IO_V_NIDAQ, "OUTP:STAT OFF");
	_ibwrt(_VSPARE_VAB, "OUTP:STAT OFF");

	write_SP(Measure_file, room_temperature, bake_temperature, bake_time);

	// Turn PSU back on, using the initial configurations
	_ibwrt(_VDD_DIG_VDD_WL, "OUTP:STAT ON");
	_ibwrt(_VSS_WL_VSS_PW, "OUTP:STAT ON");
	_ibwrt(_VDD_IO_V_NIDAQ, "OUTP:STAT ON");
	_ibwrt(_VSPARE_VAB, "OUTP:STAT ON");

	return 0;
}


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


	///***test things out: powerOFF_bake_powerON ************/
	//char Measure_file[200];
	//sprintf(Measure_file, "C:/GoogleDrive/working/TEST_temperature-chamber_write_SP_125C-17min_Modbus-RTU_RS-232");
	//powerOFF_bake_powerON(Measure_file, 210, 1250, 1020000);


	/* (2) using the implicit timing, software on demand */
	scan_selfcheck("../Scan_files/NIDAQ_test_data", 1);
	//fool-proof, should alway work

	scan("../Scan_files/Scan_all_zero", 0, 100000.0);

	int chip = 17;
	int col;
	char Measure_file[200];


	col = 33;

	DWORD baking_times[2] = {14400000, 14400000}; // {4 hours}
//	DWORD baking_times[7] = { 3600000,   7200000,  10800000,  14400000,  28800000,  57600000, 129600000}; // {1, 2, 3, 4, 8, 16, 36} hours
	//cummulative = {1, 3, 6, 10, 18, 34, 70}
	short room_temperature = 210;
	short bake_temperature = 850;

/*	sprintf(Measure_file, "C:/GoogleDrive/working/Fresh_Chip%02d_Col%02d_Ids_Vgs_VAsource_VBdrain", chip, col);
	IDS_VGS(Measure_file, col, chip, 0);
	sprintf(Measure_file, "C:/GoogleDrive/working/Fresh_Chip%02d_Col%02d_Ids_Vgs_VAdrain_VBsource", chip, col);
	IDS_VGS(Measure_file, col, chip, 1); */
/*	for (int t=1; t<2; t++){
	    sprintf(Measure_file, "C:/GoogleDrive/working/Fresh_Chip%02d_Col%02d_Ids_Vgs_VAsource_VBdrain_bake%02d", chip, col, t+1);
//	    powerOFF_bake_powerON(Measure_file, room_temperature, bake_temperature, baking_times[t]);
	    IDS_VGS(Measure_file, col, chip, 0);
	    sprintf(Measure_file, "C:/GoogleDrive/working/Fresh_Chip%02d_Col%02d_Ids_Vgs_VAdrain_VBsource_bake%02d", chip, col, t+1);
	    IDS_VGS(Measure_file, col, chip, 1);
	}
	

	double VDS_col33 = 2.0;
	double VGS_col33 = 1.8;

	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_programming_Chip%02d_Col%02d_1msPULSE_VG1p8_VD2p0_VAsource_VBdrain_01", chip, col);
	MLC_programming(Measure_file, VDS_col33, VGS_col33, "1ms", chip, col, 0, 280, 0, 0.000110);

        sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_1msPULSE_VG1p8_VD2p0_Ids_Vgs_VAsource_VBdrain_01", chip, col);   
	IDS_VGS(Measure_file, col, chip, 0);                                       
        sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_1msPULSE_VG1p8_VD2p0_Ids_Vgs_VAdrain_VBsource_01", chip, col);   
	IDS_VGS(Measure_file, col, chip, 1);
	for (int t=0; t<7; t++){
	    sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_1msPULSE_VG1p8_VD2p0_Ids_Vgs_VAsource_VBdrain_01_85C_bake%02d", chip, col, t+1);
	    powerOFF_bake_powerON(Measure_file, room_temperature, bake_temperature, baking_times[t]);
	    IDS_VGS(Measure_file, col, chip, 0);
	    sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_1msPULSE_VG1p8_VD2p0_Ids_Vgs_VAdrain_VBsource_01_85C_bake%02d", chip, col, t+1);
	    IDS_VGS(Measure_file, col, chip, 1);
	}


	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_programming_Chip%02d_Col%02d_3msPULSE_VG1p8_VD2p0_VAsource_VBdrain_02", chip, col);
	MLC_programming(Measure_file, VDS_col33, VGS_col33, "3ms", chip, col, 0, 280, 0, 0.000095);

        sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_3msPULSE_VG1p8_VD2p0_Ids_Vgs_VAsource_VBdrain_02", chip, col);   
	IDS_VGS(Measure_file, col, chip, 0);                                        
        sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_3msPULSE_VG1p8_VD2p0_Ids_Vgs_VAdrain_VBsource_02", chip, col);   
	IDS_VGS(Measure_file, col, chip, 1);
	for (int t=0; t<7; t++){
	    sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_3msPULSE_VG1p8_VD2p0_Ids_Vgs_VAsource_VBdrain_02_85C_bake%02d", chip, col, t+1);
	    powerOFF_bake_powerON(Measure_file, room_temperature, bake_temperature, baking_times[t]);
	    IDS_VGS(Measure_file, col, chip, 0);
	    sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_3msPULSE_VG1p8_VD2p0_Ids_Vgs_VAdrain_VBsource_02_85C_bake%02d", chip, col, t+1);
	    IDS_VGS(Measure_file, col, chip, 1);
	}


	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_programming_Chip%02d_Col%02d_9msPULSE_VG1p8_VD2p0_VAsource_VBdrain_03", chip, col);
	MLC_programming(Measure_file, VDS_col33, VGS_col33, "9ms", chip, col, 0, 280, 0, 0.000080);

        sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_9msPULSE_VG1p8_VD2p0_Ids_Vgs_VAsource_VBdrain_03", chip, col);   
	IDS_VGS(Measure_file, col, chip, 0);                                       
        sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_9msPULSE_VG1p8_VD2p0_Ids_Vgs_VAdrain_VBsource_03", chip, col);   
	IDS_VGS(Measure_file, col, chip, 1);
	for (int t=0; t<7; t++){
	    sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_9msPULSE_VG1p8_VD2p0_Ids_Vgs_VAsource_VBdrain_03_85C_bake%02d", chip, col, t+1);
	    powerOFF_bake_powerON(Measure_file, room_temperature, bake_temperature, baking_times[t]);
	    IDS_VGS(Measure_file, col, chip, 0);
	    sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_9msPULSE_VG1p8_VD2p0_Ids_Vgs_VAdrain_VBsource_03_85C_bake%02d", chip, col, t+1);
	    IDS_VGS(Measure_file, col, chip, 1);
	}


	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_programming_Chip%02d_Col%02d_27msPULSE_VG1p8_VD2p0_VAsource_VBdrain_04", chip, col);
	MLC_programming(Measure_file, VDS_col33, VGS_col33, "27ms", chip, col, 0, 280, 0, 0.000065);

	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_27msPULSE_VG1p8_VD2p0_Ids_Vgs_VAsource_VBdrain_04", chip, col);
	IDS_VGS(Measure_file, col, chip, 0);
	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_27msPULSE_VG1p8_VD2p0_Ids_Vgs_VAdrain_VBsource_04", chip, col);
	IDS_VGS(Measure_file, col, chip, 1);
	for (int t=0; t<7; t++){
	    sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_27msPULSE_VG1p8_VD2p0_Ids_Vgs_VAsource_VBdrain_04_85C_bake%02d", chip, col, t+1);
	    powerOFF_bake_powerON(Measure_file, room_temperature, bake_temperature, baking_times[t]);
	    IDS_VGS(Measure_file, col, chip, 0);
	    sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_27msPULSE_VG1p8_VD2p0_Ids_Vgs_VAdrain_VBsource_04_85C_bake%02d", chip, col, t+1);
	    IDS_VGS(Measure_file, col, chip, 1);
	}


	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_programming_Chip%02d_Col%02d_81msPULSE_VG1p8_VD2p0_VAsource_VBdrain_05", chip, col);
	MLC_programming(Measure_file, VDS_col33, VGS_col33, "81ms", chip, col, 0, 280, 0, 0.000050);

	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_81msPULSE_VG1p8_VD2p0_Ids_Vgs_VAsource_VBdrain_05", chip, col);
	IDS_VGS(Measure_file, col, chip, 0);
	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_81msPULSE_VG1p8_VD2p0_Ids_Vgs_VAdrain_VBsource_05", chip, col);
	IDS_VGS(Measure_file, col, chip, 1);
	for (int t=0; t<7; t++){
	    sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_81msPULSE_VG1p8_VD2p0_Ids_Vgs_VAsource_VBdrain_05_85C_bake%02d", chip, col, t+1);
	    powerOFF_bake_powerON(Measure_file, room_temperature, bake_temperature, baking_times[t]);
	    IDS_VGS(Measure_file, col, chip, 0);
	    sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_81msPULSE_VG1p8_VD2p0_Ids_Vgs_VAdrain_VBsource_05_85C_bake%02d", chip, col, t+1);
	    IDS_VGS(Measure_file, col, chip, 1);
	}


	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_programming_Chip%02d_Col%02d_243msPULSE_VG1p8_VD2p0_VAsource_VBdrain_06", chip, col);
	MLC_programming(Measure_file, VDS_col33, VGS_col33, "243ms", chip, col, 0, 280, 0, 0.000035);

	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_243msPULSE_VG1p8_VD2p0_Ids_Vgs_VAsource_VBdrain_06", chip, col);
	IDS_VGS(Measure_file, col, chip, 0);
	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_243msPULSE_VG1p8_VD2p0_Ids_Vgs_VAdrain_VBsource_06", chip, col);
	IDS_VGS(Measure_file, col, chip, 1);
	for (int t=0; t<7; t++){
	    sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_243msPULSE_VG1p8_VD2p0_Ids_Vgs_VAsource_VBdrain_06_85C_bake%02d", chip, col, t+1);
	    powerOFF_bake_powerON(Measure_file, room_temperature, bake_temperature, baking_times[t]);
	    IDS_VGS(Measure_file, col, chip, 0);
	    sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_243msPULSE_VG1p8_VD2p0_Ids_Vgs_VAdrain_VBsource_06_85C_bake%02d", chip, col, t+1);
	    IDS_VGS(Measure_file, col, chip, 1);
	}


	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_programming_Chip%02d_Col%02d_729msPULSE_VG1p8_VD2p0_VAsource_VBdrain_07", chip, col);
	MLC_programming(Measure_file, VDS_col33, VGS_col33, "729ms", chip, col, 0, 280, 0, 0.000020);

	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_729msPULSE_VG1p8_VD2p0_Ids_Vgs_VAsource_VBdrain_07", chip, col);
	IDS_VGS(Measure_file, col, chip, 0);
	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_729msPULSE_VG1p8_VD2p0_Ids_Vgs_VAdrain_VBsource_07", chip, col);
	IDS_VGS(Measure_file, col, chip, 1);
	for (int t=0; t<7; t++){
	    sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_729msPULSE_VG1p8_VD2p0_Ids_Vgs_VAsource_VBdrain_07_85C_bake%02d", chip, col, t+1);
	    powerOFF_bake_powerON(Measure_file, room_temperature, bake_temperature, baking_times[t]);
	    IDS_VGS(Measure_file, col, chip, 0);
	    sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_729msPULSE_VG1p8_VD2p0_Ids_Vgs_VAdrain_VBsource_07_85C_bake%02d", chip, col, t+1);
	    IDS_VGS(Measure_file, col, chip, 1);
	}
	*/

	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_VG1p8_VD2p0_Ids_Vgs_VAsource_VBdrain_07_36days-RoomTemp", chip, col);
	IDS_VGS(Measure_file, col, chip, 0);
	sprintf(Measure_file, "C:/GoogleDrive/working/MLC_Chip%02d_Col%02d_VG1p8_VD2p0_Ids_Vgs_VAdrain_VBsource_07_36days-RoomTemp", chip, col);
	IDS_VGS(Measure_file, col, chip, 1);
	sprintf(Measure_file, "C:/GoogleDrive/working/ALL_IDSAT_Chip%02d_Col%02d_36days-RoomTemp", chip, col);
        ALL_IDSAT(Measure_file, chip, col, 0);


	double VS = 0;
	double VB = 0;
	double VG = 0;
	sprintf(Measure_file, "C:/GoogleDrive/working/Current_Components_VDD_IO_2p4_Vg0_VsVb0_Chip%02d_Col%02d_VAsource_VBdrain", chip, col);
        Drain_leakage(Measure_file, VS, VB, VG, col, chip, 0, 1);
	sprintf(Measure_file, "C:/GoogleDrive/working/Current_Components_VDD_IO_2p4_Vg0_VsVb0_Chip%02d_Col%02d_VAdrain_VBsource", chip, col);
        Drain_leakage(Measure_file, VS, VB, VG, col, chip, 1, 1);



	// Turn off PSU outputs after tests are done!
	_ibwrt(_VDD_DIG_VDD_WL, "OUTP:STAT OFF");
	_ibwrt(_VSS_WL_VSS_PW, "OUTP:STAT OFF");
	_ibwrt(_VDD_IO_V_NIDAQ, "OUTP:STAT OFF");
	_ibwrt(_VSPARE_VAB, "OUTP:STAT OFF");

	GpibEquipmentUnInit();
	return 0;
}
