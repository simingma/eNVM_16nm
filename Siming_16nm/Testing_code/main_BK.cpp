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

int Num_of_row[36] = {32, 32, 32, 128, 128, 128, 32, 32, 32, 128, 128, 128, 32, 32, 32, 128, 128, 128, 32, 32, 32, 128, 128, 128, 32, 32, 32, 128, 128, 128, 32, 32, 32, 128, 128, 128};
double VDD_typical = 0.8

int main(void) {

	GpibEquipmentInit();
	// Setting up all voltages:

	//ATTENTION!
	E3646A_SetVoltage_CurrentLimit(_VDD_DIG_VDD_WL, 1, VDD_typical, 1.2); //VDD_DIG=VDD_typicalV, 12mA limit
	E3646A_SetVoltage_CurrentLimit(_VDD_DIG_VDD_WL, 2, VDD_typical, 0.1); //VDD_WL=VDD_typicalV, 12mA limit

	E3646A_SetVoltage_CurrentLimit(_VSS_WL_VSS_PW, 1, 0, 0.012);    //VSS_WL=0V, 12mA limit
	E3646A_SetVoltage_CurrentLimit(_VSS_WL_VSS_PW, 2, 0, 0.012);    //VSS_PW=0V, 12mA limit

	double VALUE_VDD_IO = 1.8;
//	double VALUE_VDD_IO = 3.2;
//	E3646A_SetVoltage_CurrentLimit(_VDD_IO_V_NIDAQ, 1, 1.8, 0.01); //VDD_IO=1.8V, 10mA limit
	E3646A_SetVoltage_CurrentLimit(_VDD_IO_V_NIDAQ, 1, VALUE_VDD_IO, 0.05); //VDD_IO=2.4V, VAB to VDD_IO (ESD clamp) diode 0.6~0.7V turn on
	//ATTENTION!


//      E3646A_SetVoltage_CurrentLimit(_VDD_IO_V_NIDAQ, 2, 5.0, 0.02); //V_NIDAQ=5V, 10mA limit 
	                                                               // limit quiescent DC current on V_NIDAQ!!!
	E3646A_SetVoltage_CurrentLimit(_VSPARE_VAB, 1, 0, 0.001);
	E3646A_SetVoltage_CurrentLimit(_VSPARE_VAB, 2, 0.0, 0.012);     // 12mA limit, same as DMM range  
	                                                               // *** configure |VB - VA|=0V before turning on PSU output!***
	// Turn on PSU outputs:
	_ibwrt(_VDD_DIG_VDD_WL, "OUTP:STAT ON");
	_ibwrt(_VSS_WL_VSS_PW, "OUTP:STAT ON");
	_ibwrt(_VDD_IO_V_NIDAQ, "OUTP:STAT ON");
	_ibwrt(_VSPARE_VAB, "OUTP:STAT ON");

	printf("I_VDD_DIG = %f\n", 0.001*E3646A_MeasAvgCurrent(_VDD_DIG_VDD_WL, 1));
	printf("I_VDD_WL = %f\n", 0.001*E3646A_MeasAvgCurrent(_VDD_DIG_VDD_WL, 2));
	printf("I_VSS_WL = %f\n", 0.001*E3646A_MeasAvgCurrent(_VSS_WL_VSS_PW, 1));
	printf("I_VSS_PW = %f\n", 0.001*E3646A_MeasAvgCurrent(_VSS_WL_VSS_PW, 2));
	printf("I_VDD_IO = %f\n", 0.001*E3646A_MeasAvgCurrent(_VDD_IO_V_NIDAQ, 1));
//	printf("I_V_NIDAQ = %f\n", 0.001*E3646A_MeasAvgCurrent(_VDD_IO_V_NIDAQ, 2));
	printf("I_VAB = %f\n", 0.001*E3646A_MeasAvgCurrent(_VSPARE_VAB, 2));

	
	//scan equal auto self-test, arbitrary scanin pattern:

	/* (1) using the counter explicit sample frequency timing */
	/* SampFreq = 100000*/
//	scan("../Scan_files/NIDAQ_test_data", 1, 100000.0); 
	//if the V_NIDAQ level SOUT after levelshifter is connected to AI: scan equal = 0 
	//    => this frequency is too fast for the level shifter to catch up
	//if the VDD_IO level SOUT (directly from the chip pad) connects to NIDAQ AI: scan equal = 1 

//	scan("../Scan_files/NIDAQ_test_data", 1, 10000.0); 
	//scan equal = 1, either case

//	scan("../Scan_files/NIDAQ_test_data", 1, 1000.0); 
	//scan equal =0 !!!???, either case?

	/* (2) using the implicit timing, software on demand */
	scan_selfcheck("../Scan_files/NIDAQ_test_data", 1);
	//fool-proof, should alway work

	scan("../Scan_files/Scan_all_zero", 0, 100000.0);

	int chip = 0;
	int col;
	char Measure_file[200];
	

        for (col = 0; col < 36; col = col + 1){
		sprintf(Measure_file, "../Data/Fresh_Chip%02d_Col%02d_Ids_Vgs_VAsource_VBdrain", chip, col);
		IDS_VGS(Measure_file, col, chip, 0);
		sprintf(Measure_file, "../Data/Fresh_Chip%02d_Col%02d_Ids_Vgs_VAdrain_VBsource", chip, col);
		IDS_VGS(Measure_file, col, chip, 1);
		}

/*	for (col = 0; col < 6; col = col+1){
		sprintf(Measure_file, "../Data/OFF_leakages_VDD_IO_3p2V_Vg0V_Chip%02d_Col%02d_VAsource_VBdrain_IsubAutoRange", chip, col);
		Drain_leakage(Measure_file, col, chip, 0);
//		sprintf(Measure_file, "../Data/Drain_leakage_VDD_IO_2p4V_Vg0V_Chip%02d_Col%02d_VAdrain_VBsource", chip, col);
//		Drain_leakage(Measure_file, col, chip, 1);

		
//    	        sprintf(Measure_file, "../Data/Fresh_Chip%02d_Col%02d_Ids_Vds_VAsource_VBdrain", chip, col);
//		IDS_VDS(Measure_file, col, chip, 0);
		sprintf(Measure_file, "../Data/Fresh_Chip%02d_Col%02d_Ids_Vgs_VAsource_VBdrain", chip, col);
		IDS_VGS(Measure_file, col, chip, 0);
//		sprintf(Measure_file, "../Data/Fresh_Chip%02d_Col%02d_Ids_Vds_VAdrain_VBsource", chip, col);
//		IDS_VDS(Measure_file, col, chip, 1);
		sprintf(Measure_file, "../Data/Fresh_Chip%02d_Col%02d_Ids_Vgs_VAdrain_VBsource", chip, col);
		IDS_VGS(Measure_file, col, chip, 1);
		
	}*/

	// Turn off PSU outputs after tests are done!
	_ibwrt(_VDD_DIG_VDD_WL, "OUTP:STAT OFF");
	_ibwrt(_VSS_WL_VSS_PW, "OUTP:STAT OFF");
	_ibwrt(_VDD_IO_V_NIDAQ, "OUTP:STAT OFF");
	_ibwrt(_VSPARE_VAB, "OUTP:STAT OFF");

 	GpibEquipmentUnInit();

	return 0;
}
int ALL_IDSAT(char* Measure_file, int chip, int col, int direction){

	char direction_char_stress[200], direction_char_mirror[200];
	char MUX_Address_file_stress[200], MUX_Address_file_mirror[200];

	if (direction == 0){
		sprintf(direction_char_stress, "VAsource_VBdrain");
		sprintf(MUX_Address_file_stress, "../Scan_files/MUX_Col%02d_VAsource_VBdrain", col);
		sprintf(direction_char_mirror, "VAdrain_VBsource");
		sprintf(MUX_Address_file_mirror, "../Scan_files/MUX_Col%02d_VAdrain_VBsource", col);
	}
	else{
		sprintf(direction_char_stress, "VAdrain_VBsource");
		sprintf(MUX_Address_file_stress, "../Scan_files/MUX_Col%02d_VAdrain_VBsource", col);
		sprintf(direction_char_mirror, "VAsource_VBdrain");
		sprintf(MUX_Address_file_mirror, "../Scan_files/MUX_Col%02d_VAsource_VBdrain", col);
	}


	FILE *f_ptr;
	if ((f_ptr = fopen(Measure_file, "w")) == NULL){
		printf("Cannot open%s.\n", Measure_file);
		return FAIL;
	}

	int scan_equal = scan_selfcheck("../Scan_files/NIDAQ_test_data", 1);
	fprintf(f_ptr, "scan equal =%d\n", scan_equal);
	scan("../Scan_files/Scan_all_zero", 0, 100000.0);

	// scan in WL[0]=1 in column[col], pulse=0
	char f_scan[200];
	sprintf(f_scan, "../Scan_files/Scan_Col%02d_WL0_NOpulse", col);
	scan(f_scan, 0, 100000.0);

	int row;
	float Isense;

	MM34401A_MeasCurrent_Config(_MM34401A, 10, "IMM", 0.1, 1, 1);
	for (row = 0; row < Num_of_row[col]; row++){
		//MM34401A_MeasCurrent_Config(_MM34401A, 10, "IMM", 0.1, 1, 1);
		fprintf(f_ptr, "WL[%d]\n", row);
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VDD_typical); //VDD_DIG=VDD_typicalV
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical); // change VDD_WL => Vgs of WL selected transisor
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);
		DO_USB6008(MUX_Address_file_stress);
		scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
		E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
		Isense = MM34401A_MeasCurrent(_MM34401A); //measure IDSAT + leakage current through Current Meter
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);

		fprintf(f_ptr, "ALL_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_stress, Isense);
		//debug: TODO these printf's waste resource/time???
		//printf("Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_stress, Isense);

		DO_USB6008(MUX_Address_file_mirror);
		scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
		E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
		Isense = MM34401A_MeasCurrent(_MM34401A); //measure IDSAT + leakage current through Current Meter
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);

		fprintf(f_ptr, "ALL_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_mirror, Isense);
		//debug:
		//printf("Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_mirror, Isense);

		// shift down one row
		scan("../Scan_files/Scan_shift_1_NOpulse", 0, 100000.0);
	}

	E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
	scan("../Scan_files/MUX_OFF", 0, 100000.0);

	scan_equal = scan_selfcheck("../Scan_files/NIDAQ_test_data", 1);
	fprintf(f_ptr, "scan equal =%d\n", scan_equal);
	scan("../Scan_files/Scan_all_zero", 0, 100000.0);

	fclose(f_ptr);
	return 0;
}

int Block_Erase(char* Measure_file, double VS_D, double VG, double VPW, char* pulse_width_char, int Num_of_ExtTrig, int chip, int col, int direction){

	char direction_char_stress[200], direction_char_mirror[200];
	char MUX_Address_file_stress[200], MUX_Address_file_mirror[200];

	if (direction == 0){
		sprintf(direction_char_stress, "VAsource_VBdrain");
		sprintf(MUX_Address_file_stress, "../Scan_files/MUX_Col%02d_VAsource_VBdrain", col);
		sprintf(direction_char_mirror, "VAdrain_VBsource");
		sprintf(MUX_Address_file_mirror, "../Scan_files/MUX_Col%02d_VAdrain_VBsource", col);
	}
	else{
		sprintf(direction_char_stress, "VAdrain_VBsource");
		sprintf(MUX_Address_file_stress, "../Scan_files/MUX_Col%02d_VAdrain_VBsource", col);
		sprintf(direction_char_mirror, "VAsource_VBdrain");
		sprintf(MUX_Address_file_mirror, "../Scan_files/MUX_Col%02d_VAsource_VBdrain", col);
	}


	double samp_rate = 1000.0; // sampling rate = 1000 is required to use these scan files!!!
	/*	"char* pulse_width_char" is used for naming the "WL PULSE scan file"
	all these "WL PULSE + ExtTrigger files" need to use 1000.0 sampling rate, to guarantee 10ms between ExtTrig;
	the "total number of triggers" = "WL PULSE width" / 10ms */
	char f_scan_WLpulse_ExtTrig[200];
	sprintf(f_scan_WLpulse_ExtTrig, "../Scan_files/%sPULSE_MUX_ON_%dExtTrig_1000SampRate", pulse_width_char, Num_of_ExtTrig);

	// scan in WL[0]=1 in column[col], pulse=0
/*	char f_scan[200];
	sprintf(f_scan, "../Scan_files/Scan_Col%02d_WL0_NOpulse", col);
	scan(f_scan, 0, 100000.0); */

	scan("../Scan_files/Scan_all_zero", 0, 100000.0);

	FILE *f_ptr;
	if ((f_ptr = fopen(Measure_file, "w")) == NULL){
		printf("Cannot open%s.\n", Measure_file);
		return FAIL;
	}

	//row: WL[row] number (row=0~107), t: the number of WLpulse (t=1~Num_of_Pulse), j: number of ExtTrig measurement within one WLpulse
	int row, t, j;
	float NPLCycles = 0.2;  //integration time for Multi-EstTrigger-ed measurements within a WLpulse
	float Isense;
	//the DMM's internal memory can hold 512 readings at most => the maximum number of (ExtTrig) measurements before fetching to output buffer
	float Current[512];
	//the output buffer is formatted as "SD.DDDDDDDDSEDD,SD.DDDDDDDDSEDD,SD.DDDDDDDDSEDD,....", so 15+1(comma) = 16 characters for each reading
	char StrCurrent[16];
	char RdBuffer[12001];

//	for (row = 0; row < Num_of_row[col]; row++){

/*		MM34401A_MeasCurrent_Config(_MM34401A, 10, "IMM", 0.1, 1, 1);
		fprintf(f_ptr, "WL[%d]\n", row);
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VDD_typical); //VDD_DIG=VDD_typicalV
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical); // change VDD_WL => Vgs of WL selected transisor
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);
		DO_USB6008(MUX_Address_file_stress);
		scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
		E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
		Isense = MM34401A_MeasCurrent(_MM34401A); //measure IDSAT + leakage current through Current Meter
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);

		fprintf(f_ptr, "Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_stress, Isense);
		//debug: TODO these printf's waste resource/time???
		printf("Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_stress, Isense);

		DO_USB6008(MUX_Address_file_mirror);
		scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
		E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
		Isense = MM34401A_MeasCurrent(_MM34401A); //measure IDSAT + leakage current through Current Meter
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);

		fprintf(f_ptr, "Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_mirror, Isense);
		//debug:
		printf("Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_mirror, Isense); */

//		for (t = 1; t <= Num_of_Pulse; t++){

			E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
			scan("../Scan_files/MUX_OFF", 0, 100000.0);
			DO_USB6008(MUX_Address_file_stress);

			scan("../Scan_files/MUX_ON", 0, 100000.0);

			//multiple triggers, single sample
			MM34401A_MeasCurrent_Config(_MM34401A, NPLCycles, "EXT", 0.0, 1, Num_of_ExtTrig);
			
			//multiple triggers, single sample
			MM34410A_6_MeasCurrent_Config(_MM34410A_6, NPLCycles, "EXT", 0.0, 1, Num_of_ExtTrig);


			_ibwrt(_MM34401A, "INITiate");
			_ibwrt(_MM34410A_6, "INITiate");

			E3646A_SetVoltage(_VSS_WL_VSS_PW, 1, VG);    //VSS_WL=VG, 12mA limit
			E3646A_SetVoltage(_VSPARE_VAB, 2, VS_D);     //VS=VD=VS_D
//			E3646A_SetVoltage_CurrentLimit(_VSS_WL_VSS_PW, 2, VPW, 0.012);    //VSS_PW, 12mA limit
			E3646A_SetVoltage(_VSS_WL_VSS_PW, 2, VPW);
//			E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VGS[row]); //VDD_DIG=VDD_WL=VGS, avoid any un-intentional crowbar current or turn-on diodes
//			E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VGS[row]); //VDD_WL=VGS

			// 30ms initial delay is built into the beginning of the scan file to allow VDS/VDD_DIG/VDD_WL PSUs to transition->settle
			// before toggling PULSE and trigger Isub measurment
			//multiple triggers, single sample
			scan(f_scan_WLpulse_ExtTrig, 0, samp_rate);

//			E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical); //VDD_WL=VDD_typicalV
//			E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VDD_typical); //VDD_DIG=VDD_typicalV
			E3646A_SetVoltage(_VSS_WL_VSS_PW, 2, 0);
//			E3646A_SetVoltage_CurrentLimit(_VSS_WL_VSS_PW, 2, 0, 0.012);    //VSS_PW=0V, 12mA limit
			E3646A_SetVoltage(_VSPARE_VAB, 2, 0);
			E3646A_SetVoltage(_VSS_WL_VSS_PW, 1, 0);
			scan("../Scan_files/MUX_OFF", 0, 100000.0);

			_ibwrt(_MM34401A, "FETCh?");
			_ibrd(_MM34401A, RdBuffer, 12000);
			RdBuffer[ibcntl] = '\0';         // Null terminate the ASCII string    
			//printf("%s\n", RdBuffer);

			for (j = 0; j < Num_of_ExtTrig; j++){
				//output buffer of the DMM is formatted as CSV, of all the readings fetched from its internal memory 
				//each reading has 15 charaters, plus one comma
				strncpy(StrCurrent, RdBuffer + j * 16, 15);
				StrCurrent[15] = '\0';
				sscanf(StrCurrent, "%f", &Current[j]);
				fprintf(f_ptr, "Erase_I_VS_D=%.12f\n", Current[j]);
				//debug:
				//printf("Time=%dms, Current=%.12f\n", 10 * j, Current[j]);
			}

			_ibwrt(_MM34410A_6, "FETCh?");
			_ibrd(_MM34410A_6, RdBuffer, 12000);
			RdBuffer[ibcntl] = '\0';         // Null terminate the ASCII string    
			//printf("%s\n", RdBuffer);

			for (j = 0; j < Num_of_ExtTrig; j++){
				//output buffer of the DMM is formatted as CSV, of all the readings fetched from its internal memory 
				//each reading has 15 charaters, plus one comma
				strncpy(StrCurrent, RdBuffer + j * 16, 15);
				StrCurrent[15] = '\0';
				sscanf(StrCurrent, "%f", &Current[j]);
				fprintf(f_ptr, "Erase_Isub=%.12f\n", Current[j]);
				//debug:
				//printf("Time=%dms, Current=%.12f\n", 10 * j, Current[j]);
			}

/*			MM34401A_MeasCurrent_Config(_MM34401A, 10, "IMM", 0.1, 1, 1);
			//	E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VDD_typical); //VDD_DIG=VDD_typicalV
			//E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical); // change VDD_WL => Vgs of WL selected transisor
			//E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
			//scan("../Scan_files/MUX_OFF", 0, 100000.0);
			//DO_USB6008(MUX_Address_file_stress); 
			scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
			E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
			Isense = MM34401A_MeasCurrent(_MM34401A); //measure Ids + leakage current through Current Meter
			E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
			scan("../Scan_files/MUX_OFF", 0, 100000.0);

			fprintf(f_ptr, "Stress_%02dPULSE_IDSAT_WL[%d]_%s=%.12f\n", t, row, direction_char_stress, Isense);
			//debug:
			printf("Stress_%02dPULSE_IDSAT_WL[%d]_%s=%.12f\n", t, row, direction_char_stress, Isense);

			DO_USB6008(MUX_Address_file_mirror);
			scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
			E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
			Isense = MM34401A_MeasCurrent(_MM34401A); //measure Ids + leakage current through Current Meter
			E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
			scan("../Scan_files/MUX_OFF", 0, 100000.0);

			fprintf(f_ptr, "Stress_%02dPULSE_IDSAT_WL[%d]_%s=%.12f\n", t, row, direction_char_mirror, Isense);
			//debug:
			printf("Stress_%02dPULSE_IDSAT_WL[%d]_%s=%.12f\n", t, row, direction_char_mirror, Isense); */
//		}

		// shift down one row
//		scan("../Scan_files/Scan_shift_1_NOpulse", 0, 100000.0);
//	}
	E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
	scan("../Scan_files/MUX_OFF", 0, 100000.0);
	fclose(f_ptr);
	return 0;
}

int Erase_VG_ConstPulse(char* Measure_file, double VDS, double VGS, char* pulse_width_char, int chip, int col, int direction, int Num_of_Pulse, int Num_of_Trigger){

	char direction_char_stress[200], direction_char_mirror[200];
	char MUX_Address_file_stress[200], MUX_Address_file_mirror[200];

	if (direction == 0){
		sprintf(direction_char_stress, "VAsource_VBdrain");
		sprintf(MUX_Address_file_stress, "../Scan_files/MUX_Col%02d_VAsource_VBdrain", col);
		sprintf(direction_char_mirror, "VAdrain_VBsource");
		sprintf(MUX_Address_file_mirror, "../Scan_files/MUX_Col%02d_VAdrain_VBsource", col);
	}
	else{
		sprintf(direction_char_stress, "VAdrain_VBsource");
		sprintf(MUX_Address_file_stress, "../Scan_files/MUX_Col%02d_VAdrain_VBsource", col);
		sprintf(direction_char_mirror, "VAsource_VBdrain");
		sprintf(MUX_Address_file_mirror, "../Scan_files/MUX_Col%02d_VAsource_VBdrain", col);
	}


	double samp_rate = 1000.0; // sampling rate = 1000 is required to use these scan files!!!
	/*	"char* pulse_width_char" is used for naming the "WL PULSE scan file"
	all these "WL PULSE + ExtTrigger files" need to use 1000.0 sampling rate, to guarantee 10ms between ExtTrig;
	the "total number of triggers" = "WL PULSE width" / 10ms */
	char f_scan_WLpulse_ExtTrig[200];
	sprintf(f_scan_WLpulse_ExtTrig, "../Scan_files/%sPULSE_MUX_ON_%dExtTrig_1000SampRate", pulse_width_char, Num_of_Trigger);

	FILE *f_ptr;
	if ((f_ptr = fopen(Measure_file, "w")) == NULL){
		printf("Cannot open%s.\n", Measure_file);
		return FAIL;
	}

	int scan_equal = scan_selfcheck("../Scan_files/NIDAQ_test_data", 1);
	fprintf(f_ptr, "scan equal =%d\n", scan_equal);
	scan("../Scan_files/Scan_all_zero", 0, 100000.0);

	// scan in WL[0]=1 in column[col], pulse=0
	char f_scan[200];
	sprintf(f_scan, "../Scan_files/Scan_Col%02d_WL0_NOpulse", col);
	scan(f_scan, 0, 100000.0);

	//row: WL[row] number (row=0~107), t: the number of WLpulse (t=1~Num_of_Pulse), j: number of ExtTrig measurement within one WLpulse
	int row, t, j;
	float NPLCycles = 0.2;  //integration time for Multi-ExtTrigger-ed measurements within a WLpulse
	//float NPLCycles = 1.0;
	float Isense;
	//the DMM's internal memory can hold 512 readings at most => the maximum number of (ExtTrig) measurements before fetching to output buffer
	float Current[512];
	//the output buffer is formatted as "SD.DDDDDDDDSEDD,SD.DDDDDDDDSEDD,SD.DDDDDDDDSEDD,....", so 15+1(comma) = 16 characters for each reading
	char StrCurrent[16];
	char RdBuffer[12001]; //for multi-trigger/sample, innitiate->fetch data retrival

/*	MM34401A_MeasCurrent_Config(_MM34401A, 10, "IMM", 0.1, 1, 1);
	for (row = 0; row < Num_of_row[col]; row++){
		//MM34401A_MeasCurrent_Config(_MM34401A, 10, "IMM", 0.1, 1, 1);
		fprintf(f_ptr, "WL[%d]\n", row);
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VDD_typical); //VDD_DIG=VDD_typicalV
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical); // change VDD_WL => Vgs of WL selected transisor
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);
		DO_USB6008(MUX_Address_file_stress);
		scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
		E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
		Isense = MM34401A_MeasCurrent(_MM34401A); //measure IDSAT + leakage current through Current Meter
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);

		fprintf(f_ptr, "ALL_Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_stress, Isense);
		//debug: TODO these printf's waste resource/time???
		//printf("Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_stress, Isense);

		DO_USB6008(MUX_Address_file_mirror);
		scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
		E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
		Isense = MM34401A_MeasCurrent(_MM34401A); //measure IDSAT + leakage current through Current Meter
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);

		fprintf(f_ptr, "ALL_Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_mirror, Isense);
		//debug:
		//printf("Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_mirror, Isense);

		// shift down one row
		scan("../Scan_files/Scan_shift_1_NOpulse", 0, 100000.0);
	}
	E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
	scan("../Scan_files/MUX_OFF", 0, 100000.0);

	// scan in WL[0]=1 in column[col], pulse=0
	sprintf(f_scan, "../Scan_files/Scan_Col%02d_WL0_NOpulse", col);
	scan(f_scan, 0, 100000.0); */
	//	MM34401A_MeasCurrent_Config(_MM34401A, 10, "IMM", 0.1, 1, 1);
	MM34410A_6_MeasCurrent_Config(_MM34410A_6, NPLCycles, "EXT", 0.0, 1, Num_of_Trigger);

	for (row = 0; row < Num_of_row[col]; row++){
		MM34401A_MeasCurrent_Config(_MM34401A, 10, "IMM", 0.1, 1, 1);
		fprintf(f_ptr, "WL[%d]\n", row);
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VDD_typical); //VDD_DIG=VDD_typicalV
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical); // change VDD_WL => Vgs of WL selected transisor
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);
		DO_USB6008(MUX_Address_file_stress);
		scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
		E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
		Isense = MM34401A_MeasCurrent(_MM34401A); //measure IDSAT + leakage current through Current Meter
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);

		fprintf(f_ptr, "PreErase_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_stress, Isense);
		//debug: TODO these printf's waste resource/time???
		//printf("Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_stress, Isense);

		DO_USB6008(MUX_Address_file_mirror);
		scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
		E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
		Isense = MM34401A_MeasCurrent(_MM34401A); //measure IDSAT + leakage current through Current Meter
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);

		fprintf(f_ptr, "PreErase_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_mirror, Isense);
		//debug:
		//printf("Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_mirror, Isense);

		for (t = 1; t <= Num_of_Pulse; t++){

			E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
			scan("../Scan_files/MUX_OFF", 0, 100000.0);
			DO_USB6008(MUX_Address_file_stress);

			scan("../Scan_files/MUX_ON", 0, 100000.0);

			//multiple triggers, single sample
			MM34401A_MeasCurrent_Config(_MM34401A, NPLCycles, "EXT", 0.0, 1, Num_of_Trigger);
			//char RdBuffer[12001];

			_ibwrt(_MM34401A, "INITiate");
			_ibwrt(_MM34410A_6, "INITiate");

/*			E3646A_SetVoltage(_VSPARE_VAB, 2, VDS[row]);     //VA=VDS
			E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VGS[row]); //VDD_DIG=VDD_WL=VGS, avoid any un-intentional crowbar current or turn-on diodes
			E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VGS[row]); //VDD_WL=VGS */

			E3646A_SetVoltage(_VSPARE_VAB, 2, VDS);     //VA=VDS
			E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VGS); //VDD_DIG=VDD_WL=VGS, avoid any un-intentional crowbar current or turn-on diodes
			E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VGS); //VDD_WL=VGS

			// 30ms initial delay is built into the beginning of the scan file to allow VDS/VDD_DIG/VDD_WL PSUs to transition->settle
			// before toggling PULSE and trigger Isub measurment
			//multiple triggers, single sample
			scan(f_scan_WLpulse_ExtTrig, 0, samp_rate);

			E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical); //VDD_WL=VDD_typicalV
			E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VDD_typical); //VDD_DIG=VDD_typicalV
			E3646A_SetVoltage(_VSPARE_VAB, 2, 0);
			scan("../Scan_files/MUX_OFF", 0, 100000.0);

			_ibwrt(_MM34401A, "FETCh?");
			_ibrd(_MM34401A, RdBuffer, 12000);
			RdBuffer[ibcntl] = '\0';         // Null terminate the ASCII string    
			//printf("%s\n", RdBuffer);

			for (j = 0; j < Num_of_Trigger; j++){
				//output buffer of the DMM is formatted as CSV, of all the readings fetched from its internal memory 
				//each reading has 15 charaters, plus one comma
				strncpy(StrCurrent, RdBuffer + j * 16, 15);
				StrCurrent[15] = '\0';
				sscanf(StrCurrent, "%f", &Current[j]);
				fprintf(f_ptr, "Erase_%02dPULSE_WL[%d]_ID_program=%.12f\n", t, row, Current[j]);
				//debug:
				//printf("Time=%dms, Current=%.12f\n", 10 * j, Current[j]);
			}

			_ibwrt(_MM34410A_6, "FETCh?");
			_ibrd(_MM34410A_6, RdBuffer, 12000);
			RdBuffer[ibcntl] = '\0';         // Null terminate the ASCII string    
			//printf("%s\n", RdBuffer);

			for (j = 0; j < Num_of_Trigger; j++){
				//output buffer of the DMM is formatted as CSV, of all the readings fetched from its internal memory 
				//each reading has 15 charaters, plus one comma
				strncpy(StrCurrent, RdBuffer + j * 16, 15);
				StrCurrent[15] = '\0';
				sscanf(StrCurrent, "%f", &Current[j]);
				fprintf(f_ptr, "Erase_%02dPULSE_WL[%d]_Isub=%.12f\n", t, row, Current[j]);
				//debug:
				//printf("Time=%dms, Current=%.12f\n", 10 * j, Current[j]);
			}

			MM34401A_MeasCurrent_Config(_MM34401A, 10, "IMM", 0.1, 1, 1);
			/*	E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VDD_typical); //VDD_DIG=VDD_typicalV
			E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical); // change VDD_WL => Vgs of WL selected transisor
			E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
			scan("../Scan_files/MUX_OFF", 0, 100000.0);
			DO_USB6008(MUX_Address_file_stress); */
			scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
			E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
			Isense = MM34401A_MeasCurrent(_MM34401A); //measure Ids + leakage current through Current Meter
			E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
			scan("../Scan_files/MUX_OFF", 0, 100000.0);

			fprintf(f_ptr, "Erase_%02dPULSE_IDSAT_WL[%d]_%s=%.12f\n", t, row, direction_char_stress, Isense);
			//debug:
			//printf("Stress_%02dPULSE_IDSAT_WL[%d]_%s=%.12f\n", t, row, direction_char_stress, Isense);

			DO_USB6008(MUX_Address_file_mirror);
			scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
			E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
			Isense = MM34401A_MeasCurrent(_MM34401A); //measure Ids + leakage current through Current Meter
			E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
			scan("../Scan_files/MUX_OFF", 0, 100000.0);

			fprintf(f_ptr, "Erase_%02dPULSE_IDSAT_WL[%d]_%s=%.12f\n", t, row, direction_char_mirror, Isense);
			//debug:
			//printf("Stress_%02dPULSE_IDSAT_WL[%d]_%s=%.12f\n", t, row, direction_char_mirror, Isense);
		}

		// shift down one row
		scan("../Scan_files/Scan_shift_1_NOpulse", 0, 100000.0);
	}
	E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
	scan("../Scan_files/MUX_OFF", 0, 100000.0);


	// scan in WL[0]=1 in column[col], pulse=0
	sprintf(f_scan, "../Scan_files/Scan_Col%02d_WL0_NOpulse", col);
	scan(f_scan, 0, 100000.0);

	MM34401A_MeasCurrent_Config(_MM34401A, 10, "IMM", 0.1, 1, 1);
	for (row = 0; row < Num_of_row[col]; row++){
		//MM34401A_MeasCurrent_Config(_MM34401A, 10, "IMM", 0.1, 1, 1);
		fprintf(f_ptr, "WL[%d]\n", row);
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VDD_typical); //VDD_DIG=VDD_typicalV
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical); // change VDD_WL => Vgs of WL selected transisor
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);
		DO_USB6008(MUX_Address_file_stress);
		scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
		E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
		Isense = MM34401A_MeasCurrent(_MM34401A); //measure IDSAT + leakage current through Current Meter
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);

		fprintf(f_ptr, "ALL_Final_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_stress, Isense);
		//debug: TODO these printf's waste resource/time???
		//printf("Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_stress, Isense);

		DO_USB6008(MUX_Address_file_mirror);
		scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
		E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
		Isense = MM34401A_MeasCurrent(_MM34401A); //measure IDSAT + leakage current through Current Meter
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);

		fprintf(f_ptr, "ALL_Final_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_mirror, Isense);
		//debug:
		//printf("Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_mirror, Isense);

		// shift down one row
		scan("../Scan_files/Scan_shift_1_NOpulse", 0, 100000.0);
	}
	E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
	scan("../Scan_files/MUX_OFF", 0, 100000.0);

	scan_equal = scan_selfcheck("../Scan_files/NIDAQ_test_data", 1);
	fprintf(f_ptr, "scan equal =%d\n", scan_equal);
	scan("../Scan_files/Scan_all_zero", 0, 100000.0);

	fclose(f_ptr);
	return 0;
}


int stress_VG_ConstPulse(char* Measure_file, double* VDS, double* VGS, char* pulse_width_char, int chip, int col, int direction, int Num_of_Pulse){

	char direction_char_stress[200], direction_char_mirror[200];
	char MUX_Address_file_stress[200], MUX_Address_file_mirror[200];

	if (direction == 0){
		sprintf(direction_char_stress, "VAsource_VBdrain");
		sprintf(MUX_Address_file_stress, "../Scan_files/MUX_Col%02d_VAsource_VBdrain", col);
		sprintf(direction_char_mirror, "VAdrain_VBsource");
		sprintf(MUX_Address_file_mirror, "../Scan_files/MUX_Col%02d_VAdrain_VBsource", col);
	}
	else{
		sprintf(direction_char_stress, "VAdrain_VBsource");
		sprintf(MUX_Address_file_stress, "../Scan_files/MUX_Col%02d_VAdrain_VBsource", col);
		sprintf(direction_char_mirror, "VAsource_VBdrain");
		sprintf(MUX_Address_file_mirror, "../Scan_files/MUX_Col%02d_VAsource_VBdrain", col);
	}


	double samp_rate = 1000.0; // sampling rate = 1000 is required to use these scan files!!!
	/*	"char* pulse_width_char" is used for naming the "WL PULSE scan file"
	all these "WL PULSE + ExtTrigger files" need to use 1000.0 sampling rate, to guarantee 10ms between ExtTrig;
	the "total number of triggers" = "WL PULSE width" / 10ms */
	char f_scan_WLpulse_ExtTrig[200];
	sprintf(f_scan_WLpulse_ExtTrig, "../Scan_files/%sPULSE_MUX_ON_%dExtTrig_1000SampRate", pulse_width_char, 1);

	FILE *f_ptr;
	if ((f_ptr = fopen(Measure_file, "w")) == NULL){
		printf("Cannot open%s.\n", Measure_file);
		return FAIL;
	}

	int scan_equal = scan_selfcheck("../Scan_files/NIDAQ_test_data", 1);
	fprintf(f_ptr, "scan equal =%d\n", scan_equal);
	scan("../Scan_files/Scan_all_zero", 0, 100000.0);

	// scan in WL[0]=1 in column[col], pulse=0
	char f_scan[200];
	sprintf(f_scan, "../Scan_files/Scan_Col%02d_WL0_NOpulse", col);
	scan(f_scan, 0, 100000.0);

	//row: WL[row] number (row=0~107), t: the number of WLpulse (t=1~Num_of_Pulse), j: number of ExtTrig measurement within one WLpulse
	int row, t, j;
	float NPLCycles = 0.2;  //integration time for Multi-ExtTrigger-ed measurements within a WLpulse
	//float NPLCycles = 1.0;
	float Isense;
	//the DMM's internal memory can hold 512 readings at most => the maximum number of (ExtTrig) measurements before fetching to output buffer
	float Current[512];
	//the output buffer is formatted as "SD.DDDDDDDDSEDD,SD.DDDDDDDDSEDD,SD.DDDDDDDDSEDD,....", so 15+1(comma) = 16 characters for each reading
	char StrCurrent[16];
	char RdBuffer[12001]; //for multi-trigger/sample, innitiate->fetch data retrival

	MM34401A_MeasCurrent_Config(_MM34401A, 10, "IMM", 0.1, 1, 1);
	for (row = 0; row < Num_of_row[col]; row++){
		//MM34401A_MeasCurrent_Config(_MM34401A, 10, "IMM", 0.1, 1, 1);
		fprintf(f_ptr, "WL[%d]\n", row);
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VDD_typical); //VDD_DIG=VDD_typicalV
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical); // change VDD_WL => Vgs of WL selected transisor
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);
		DO_USB6008(MUX_Address_file_stress);
		scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
		E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
		Isense = MM34401A_MeasCurrent(_MM34401A); //measure IDSAT + leakage current through Current Meter
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);

		fprintf(f_ptr, "ALL_Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_stress, Isense);
		//debug: TODO these printf's waste resource/time???
		//printf("Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_stress, Isense);

		DO_USB6008(MUX_Address_file_mirror);
		scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
		E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
		Isense = MM34401A_MeasCurrent(_MM34401A); //measure IDSAT + leakage current through Current Meter
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);

		fprintf(f_ptr, "ALL_Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_mirror, Isense);
		//debug:
		//printf("Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_mirror, Isense);

		// shift down one row
		scan("../Scan_files/Scan_shift_1_NOpulse", 0, 100000.0);
	}
	E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
	scan("../Scan_files/MUX_OFF", 0, 100000.0);

	// scan in WL[0]=1 in column[col], pulse=0
	sprintf(f_scan, "../Scan_files/Scan_Col%02d_WL0_NOpulse", col);
	scan(f_scan, 0, 100000.0);
	//	MM34401A_MeasCurrent_Config(_MM34401A, 10, "IMM", 0.1, 1, 1);
	MM34410A_6_MeasCurrent_Config(_MM34410A_6, NPLCycles, "EXT", 0.0, 1, 1);

	for (row = 0; row < Num_of_row[col]; row++){
		MM34401A_MeasCurrent_Config(_MM34401A, 10, "IMM", 0.1, 1, 1);
		fprintf(f_ptr, "WL[%d]\n", row);
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VDD_typical); //VDD_DIG=VDD_typicalV
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical); // change VDD_WL => Vgs of WL selected transisor
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);
		DO_USB6008(MUX_Address_file_stress);
		scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
		E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
		Isense = MM34401A_MeasCurrent(_MM34401A); //measure IDSAT + leakage current through Current Meter
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);

		fprintf(f_ptr, "Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_stress, Isense);
		//debug: TODO these printf's waste resource/time???
		//printf("Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_stress, Isense);

		DO_USB6008(MUX_Address_file_mirror);
		scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
		E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
		Isense = MM34401A_MeasCurrent(_MM34401A); //measure IDSAT + leakage current through Current Meter
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);

		fprintf(f_ptr, "Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_mirror, Isense);
		//debug:
		//printf("Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_mirror, Isense);

		for (t = 1; t <= Num_of_Pulse; t++){

			E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
			scan("../Scan_files/MUX_OFF", 0, 100000.0);
			DO_USB6008(MUX_Address_file_stress);

			scan("../Scan_files/MUX_ON", 0, 100000.0);

			//multiple triggers, single sample
			MM34401A_MeasCurrent_Config(_MM34401A, NPLCycles, "EXT", 0.0, 1, 1);
			//char RdBuffer[12001];

			_ibwrt(_MM34401A, "INITiate");
			_ibwrt(_MM34410A_6, "INITiate");

			E3646A_SetVoltage(_VSPARE_VAB, 2, VDS[row]);     //VA=VDS
			E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VGS[row]); //VDD_DIG=VDD_WL=VGS, avoid any un-intentional crowbar current or turn-on diodes
			E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VGS[row]); //VDD_WL=VGS

			// 30ms initial delay is built into the beginning of the scan file to allow VDS/VDD_DIG/VDD_WL PSUs to transition->settle
			// before toggling PULSE and trigger Isub measurment
			//multiple triggers, single sample
			scan(f_scan_WLpulse_ExtTrig, 0, samp_rate);

			E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical); //VDD_WL=VDD_typicalV
			E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VDD_typical); //VDD_DIG=VDD_typicalV
			E3646A_SetVoltage(_VSPARE_VAB, 2, 0);
			scan("../Scan_files/MUX_OFF", 0, 100000.0);

			_ibwrt(_MM34401A, "FETCh?");
			_ibrd(_MM34401A, RdBuffer, 12000);
			RdBuffer[ibcntl] = '\0';         // Null terminate the ASCII string    
			//printf("%s\n", RdBuffer);

			for (j = 0; j < 1; j++){
				//output buffer of the DMM is formatted as CSV, of all the readings fetched from its internal memory 
				//each reading has 15 charaters, plus one comma
				strncpy(StrCurrent, RdBuffer + j * 16, 15);
				StrCurrent[15] = '\0';
				sscanf(StrCurrent, "%f", &Current[j]);
				fprintf(f_ptr, "Stress_%02dPULSE_WL[%d]_ID_program=%.12f\n", t, row, Current[j]);
				//debug:
				//printf("Time=%dms, Current=%.12f\n", 10 * j, Current[j]);
			}

			_ibwrt(_MM34410A_6, "FETCh?");
			_ibrd(_MM34410A_6, RdBuffer, 12000);
			RdBuffer[ibcntl] = '\0';         // Null terminate the ASCII string    
			//printf("%s\n", RdBuffer);

			for (j = 0; j < 1; j++){
				//output buffer of the DMM is formatted as CSV, of all the readings fetched from its internal memory 
				//each reading has 15 charaters, plus one comma
				strncpy(StrCurrent, RdBuffer + j * 16, 15);
				StrCurrent[15] = '\0';
				sscanf(StrCurrent, "%f", &Current[j]);
				fprintf(f_ptr, "Stress_%02dPULSE_WL[%d]_Isub=%.12f\n", t, row, Current[j]);
				//debug:
				//printf("Time=%dms, Current=%.12f\n", 10 * j, Current[j]);
			}

			MM34401A_MeasCurrent_Config(_MM34401A, 10, "IMM", 0.1, 1, 1);
			/*	E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VDD_typical); //VDD_DIG=VDD_typicalV
			E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical); // change VDD_WL => Vgs of WL selected transisor
			E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
			scan("../Scan_files/MUX_OFF", 0, 100000.0);
			DO_USB6008(MUX_Address_file_stress); */
			scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
			E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
			Isense = MM34401A_MeasCurrent(_MM34401A); //measure Ids + leakage current through Current Meter
			E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
			scan("../Scan_files/MUX_OFF", 0, 100000.0);

			fprintf(f_ptr, "Stress_%02dPULSE_IDSAT_WL[%d]_%s=%.12f\n", t, row, direction_char_stress, Isense);
			//debug:
			//printf("Stress_%02dPULSE_IDSAT_WL[%d]_%s=%.12f\n", t, row, direction_char_stress, Isense);

			DO_USB6008(MUX_Address_file_mirror);
			scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
			E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
			Isense = MM34401A_MeasCurrent(_MM34401A); //measure Ids + leakage current through Current Meter
			E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
			scan("../Scan_files/MUX_OFF", 0, 100000.0);

			fprintf(f_ptr, "Stress_%02dPULSE_IDSAT_WL[%d]_%s=%.12f\n", t, row, direction_char_mirror, Isense);
			//debug:
			//printf("Stress_%02dPULSE_IDSAT_WL[%d]_%s=%.12f\n", t, row, direction_char_mirror, Isense);
		}

		// shift down one row
		scan("../Scan_files/Scan_shift_1_NOpulse", 0, 100000.0);
	}
	E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
	scan("../Scan_files/MUX_OFF", 0, 100000.0);


	// scan in WL[0]=1 in column[col], pulse=0
	sprintf(f_scan, "../Scan_files/Scan_Col%02d_WL0_NOpulse", col);
	scan(f_scan, 0, 100000.0);
	
	MM34401A_MeasCurrent_Config(_MM34401A, 10, "IMM", 0.1, 1, 1);
	for (row = 0; row < Num_of_row[col]; row++){
		//MM34401A_MeasCurrent_Config(_MM34401A, 10, "IMM", 0.1, 1, 1);
		fprintf(f_ptr, "WL[%d]\n", row);
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VDD_typical); //VDD_DIG=VDD_typicalV
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical); // change VDD_WL => Vgs of WL selected transisor
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);
		DO_USB6008(MUX_Address_file_stress);
		scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
		E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
		Isense = MM34401A_MeasCurrent(_MM34401A); //measure IDSAT + leakage current through Current Meter
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);

		fprintf(f_ptr, "ALL_Final_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_stress, Isense);
		//debug: TODO these printf's waste resource/time???
		//printf("Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_stress, Isense);

		DO_USB6008(MUX_Address_file_mirror);
		scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
		E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
		Isense = MM34401A_MeasCurrent(_MM34401A); //measure IDSAT + leakage current through Current Meter
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);

		fprintf(f_ptr, "ALL_Final_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_mirror, Isense);
		//debug:
		//printf("Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_mirror, Isense);

		// shift down one row
		scan("../Scan_files/Scan_shift_1_NOpulse", 0, 100000.0);
	}
	E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
	scan("../Scan_files/MUX_OFF", 0, 100000.0);

	scan_equal = scan_selfcheck("../Scan_files/NIDAQ_test_data", 1);
	fprintf(f_ptr, "scan equal =%d\n", scan_equal);
	scan("../Scan_files/Scan_all_zero", 0, 100000.0);

	fclose(f_ptr);
	return 0;
}


int stress_VG_RampPulse_Isub(char* Measure_file, double* VDS, double* VGS, char* pulse_width_char, int chip, int col, int direction, int Num_of_Pulse){

	char direction_char_stress[200], direction_char_mirror[200];
	char MUX_Address_file_stress[200], MUX_Address_file_mirror[200];

	if (direction == 0){
		sprintf(direction_char_stress, "VAsource_VBdrain");
		sprintf(MUX_Address_file_stress, "../Scan_files/MUX_Col%02d_VAsource_VBdrain", col);
		sprintf(direction_char_mirror, "VAdrain_VBsource");
		sprintf(MUX_Address_file_mirror, "../Scan_files/MUX_Col%02d_VAdrain_VBsource", col);
	}
	else{
		sprintf(direction_char_stress, "VAdrain_VBsource");
		sprintf(MUX_Address_file_stress, "../Scan_files/MUX_Col%02d_VAdrain_VBsource", col);
		sprintf(direction_char_mirror, "VAsource_VBdrain");
		sprintf(MUX_Address_file_mirror, "../Scan_files/MUX_Col%02d_VAsource_VBdrain", col);
	}


	double samp_rate = 1000.0; // sampling rate = 1000 is required to use these scan files!!!
	/*	"char* pulse_width_char" is used for naming the "WL PULSE scan file"
	all these "WL PULSE + ExtTrigger files" need to use 1000.0 sampling rate, to guarantee 10ms between ExtTrig;
	the "total number of triggers" = "WL PULSE width" / 10ms */
	char f_scan_WLpulse_ExtTrig[200];
	sprintf(f_scan_WLpulse_ExtTrig, "../Scan_files/%sPULSE_MUX_ON_%dExtTrig_1000SampRate", pulse_width_char, 1);

	FILE *f_ptr;
	if ((f_ptr = fopen(Measure_file, "w")) == NULL){
		printf("Cannot open%s.\n", Measure_file);
		return FAIL;
	}

	int scan_equal = scan_selfcheck("../Scan_files/NIDAQ_test_data", 1);
	fprintf(f_ptr, "scan equal =%d\n", scan_equal);
	scan("../Scan_files/Scan_all_zero", 0, 100000.0);

	// scan in WL[0]=1 in column[col], pulse=0
	char f_scan[200];
	sprintf(f_scan, "../Scan_files/Scan_Col%02d_WL0_NOpulse", col);
	scan(f_scan, 0, 100000.0);

	//row: WL[row] number (row=0~107), t: the number of WLpulse (t=1~Num_of_Pulse), j: number of ExtTrig measurement within one WLpulse
	int row, t, j;
	float NPLCycles = 0.2;  //integration time for Multi-ExtTrigger-ed measurements within a WLpulse
	//float NPLCycles = 1.0;
	float Isense;  
	//the DMM's internal memory can hold 512 readings at most => the maximum number of (ExtTrig) measurements before fetching to output buffer
	float Current[512]; 
	//the output buffer is formatted as "SD.DDDDDDDDSEDD,SD.DDDDDDDDSEDD,SD.DDDDDDDDSEDD,....", so 15+1(comma) = 16 characters for each reading
	char StrCurrent[16];
	char RdBuffer[12001]; //for multi-trigger/sample, innitiate->fetch data retrival

	MM34401A_MeasCurrent_Config(_MM34401A, 10, "IMM", 0.1, 1, 1);
	for (row = 0; row < Num_of_row[col]; row++){
		//MM34401A_MeasCurrent_Config(_MM34401A, 10, "IMM", 0.1, 1, 1);
		fprintf(f_ptr, "WL[%d]\n", row);
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VDD_typical); //VDD_DIG=VDD_typicalV
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical); // change VDD_WL => Vgs of WL selected transisor
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);
		DO_USB6008(MUX_Address_file_stress);
		scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
		E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
		Isense = MM34401A_MeasCurrent(_MM34401A); //measure IDSAT + leakage current through Current Meter
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);

		fprintf(f_ptr, "ALL_Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_stress, Isense);
		//debug: TODO these printf's waste resource/time???
		//printf("Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_stress, Isense);

		DO_USB6008(MUX_Address_file_mirror);
		scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
		E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
		Isense = MM34401A_MeasCurrent(_MM34401A); //measure IDSAT + leakage current through Current Meter
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);

		fprintf(f_ptr, "ALL_Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_mirror, Isense);
		//debug:
		//printf("Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_mirror, Isense);

		// shift down one row
		scan("../Scan_files/Scan_shift_1_NOpulse", 0, 100000.0);
	}
	E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
	scan("../Scan_files/MUX_OFF", 0, 100000.0);

	// scan in WL[0]=1 in column[col], pulse=0
	sprintf(f_scan, "../Scan_files/Scan_Col%02d_WL0_NOpulse", col);
	scan(f_scan, 0, 100000.0);
//	MM34401A_MeasCurrent_Config(_MM34401A, 10, "IMM", 0.1, 1, 1);
	MM34410A_6_MeasCurrent_Config(_MM34410A_6, NPLCycles, "EXT", 0.0, 1, 1);

	for (row = 0; row < Num_of_row[col]; row++){
		MM34401A_MeasCurrent_Config(_MM34401A, 10, "IMM", 0.1, 1, 1);
		fprintf(f_ptr, "WL[%d]\n", row);
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VDD_typical); //VDD_DIG=VDD_typicalV
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical); // change VDD_WL => Vgs of WL selected transisor
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);
		DO_USB6008(MUX_Address_file_stress);
		scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
		E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
		Isense = MM34401A_MeasCurrent(_MM34401A); //measure IDSAT + leakage current through Current Meter
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);

		fprintf(f_ptr, "Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_stress, Isense);
		//debug: TODO these printf's waste resource/time???
		//printf("Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_stress, Isense);

		DO_USB6008(MUX_Address_file_mirror);
		scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
		E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
		Isense = MM34401A_MeasCurrent(_MM34401A); //measure IDSAT + leakage current through Current Meter
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);

		fprintf(f_ptr, "Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_mirror, Isense);
		//debug:
		//printf("Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_mirror, Isense);

		for (t = 1; t <= Num_of_Pulse; t++){

			E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
			scan("../Scan_files/MUX_OFF", 0, 100000.0);
			DO_USB6008(MUX_Address_file_stress);

			scan("../Scan_files/MUX_ON", 0, 100000.0);

			//multiple triggers, single sample
			MM34401A_MeasCurrent_Config(_MM34401A, NPLCycles, "EXT", 0.0, 1, 1);
			//char RdBuffer[12001];

			_ibwrt(_MM34401A, "INITiate");
			_ibwrt(_MM34410A_6, "INITiate");

			E3646A_SetVoltage(_VSPARE_VAB, 2, VDS[row]);     //VA=VDS
			E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VGS[t-1]); //VDD_DIG=VDD_WL=VGS, avoid any un-intentional crowbar current or turn-on diodes
			E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VGS[t-1]); //VDD_WL=VGS

			// 30ms initial delay is built into the beginning of the scan file to allow VDS/VDD_DIG/VDD_WL PSUs to transition->settle
			// before toggling PULSE and trigger Isub measurment
			//multiple triggers, single sample
			scan(f_scan_WLpulse_ExtTrig, 0, samp_rate);

			E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical); //VDD_WL=VDD_typicalV
			E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VDD_typical); //VDD_DIG=VDD_typicalV
			E3646A_SetVoltage(_VSPARE_VAB, 2, 0);
			scan("../Scan_files/MUX_OFF", 0, 100000.0);

			_ibwrt(_MM34401A, "FETCh?");
			_ibrd(_MM34401A, RdBuffer, 12000);
			RdBuffer[ibcntl] = '\0';         // Null terminate the ASCII string    
			//printf("%s\n", RdBuffer);

			for (j = 0; j < 1; j++){
				//output buffer of the DMM is formatted as CSV, of all the readings fetched from its internal memory 
				//each reading has 15 charaters, plus one comma
				strncpy(StrCurrent, RdBuffer + j * 16, 15);
				StrCurrent[15] = '\0';
				sscanf(StrCurrent, "%f", &Current[j]);
				fprintf(f_ptr, "Stress_%02dPULSE_WL[%d]_ID_program=%.12f\n", t, row, Current[j]);
				//debug:
				//printf("Time=%dms, Current=%.12f\n", 10 * j, Current[j]);
			}

			_ibwrt(_MM34410A_6, "FETCh?");
			_ibrd(_MM34410A_6, RdBuffer, 12000);
			RdBuffer[ibcntl] = '\0';         // Null terminate the ASCII string    
			//printf("%s\n", RdBuffer);

			for (j = 0; j < 1; j++){
				//output buffer of the DMM is formatted as CSV, of all the readings fetched from its internal memory 
				//each reading has 15 charaters, plus one comma
				strncpy(StrCurrent, RdBuffer + j * 16, 15);
				StrCurrent[15] = '\0';
				sscanf(StrCurrent, "%f", &Current[j]);
				fprintf(f_ptr, "Stress_%02dPULSE_WL[%d]_Isub=%.12f\n", t, row, Current[j]);
				//debug:
				//printf("Time=%dms, Current=%.12f\n", 10 * j, Current[j]);
			}

			MM34401A_MeasCurrent_Config(_MM34401A, 10, "IMM", 0.1, 1, 1);
			/*	E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VDD_typical); //VDD_DIG=VDD_typicalV
			E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical); // change VDD_WL => Vgs of WL selected transisor
			E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
			scan("../Scan_files/MUX_OFF", 0, 100000.0);
			DO_USB6008(MUX_Address_file_stress); */
			scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
			E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
			Isense = MM34401A_MeasCurrent(_MM34401A); //measure Ids + leakage current through Current Meter
			E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
			scan("../Scan_files/MUX_OFF", 0, 100000.0);

			fprintf(f_ptr, "Stress_%02dPULSE_IDSAT_WL[%d]_%s=%.12f\n", t, row, direction_char_stress, Isense);
			//debug:
			//printf("Stress_%02dPULSE_IDSAT_WL[%d]_%s=%.12f\n", t, row, direction_char_stress, Isense);

			DO_USB6008(MUX_Address_file_mirror);
			scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
			E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
			Isense = MM34401A_MeasCurrent(_MM34401A); //measure Ids + leakage current through Current Meter
			E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
			scan("../Scan_files/MUX_OFF", 0, 100000.0);

			fprintf(f_ptr, "Stress_%02dPULSE_IDSAT_WL[%d]_%s=%.12f\n", t, row, direction_char_mirror, Isense);
			//debug:
			//printf("Stress_%02dPULSE_IDSAT_WL[%d]_%s=%.12f\n", t, row, direction_char_mirror, Isense);
		}

		// shift down one row
		scan("../Scan_files/Scan_shift_1_NOpulse", 0, 100000.0);
	}
	E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
	scan("../Scan_files/MUX_OFF", 0, 100000.0);

	scan_equal = scan_selfcheck("../Scan_files/NIDAQ_test_data", 1);
	fprintf(f_ptr, "scan equal =%d\n", scan_equal);
	scan("../Scan_files/Scan_all_zero", 0, 100000.0);

	fclose(f_ptr);
	return 0;
}

int stress_Ext_Imeas_1by1(char* Measure_file, double VDS, double VGS, char* pulse_width_char, int Num_of_ExtTrig, int chip, int col, int direction, int Num_of_Pulse){

	char direction_char_stress[200], direction_char_mirror[200];
	char MUX_Address_file_stress[200], MUX_Address_file_mirror[200];

	if (direction == 0){
		sprintf(direction_char_stress, "VAsource_VBdrain");
		sprintf(MUX_Address_file_stress, "../Scan_files/MUX_Col%02d_VAsource_VBdrain", col);
		sprintf(direction_char_mirror, "VAdrain_VBsource");
		sprintf(MUX_Address_file_mirror, "../Scan_files/MUX_Col%02d_VAdrain_VBsource", col);
	}
	else{
		sprintf(direction_char_stress, "VAdrain_VBsource");
		sprintf(MUX_Address_file_stress, "../Scan_files/MUX_Col%02d_VAdrain_VBsource", col);
		sprintf(direction_char_mirror, "VAsource_VBdrain");
		sprintf(MUX_Address_file_mirror, "../Scan_files/MUX_Col%02d_VAsource_VBdrain", col);
	}


	double samp_rate = 1000.0; // sampling rate = 1000 is required to use these scan files!!!
	/*	"char* pulse_width_char" is used for naming the "WL PULSE scan file" 
	    all these "WL PULSE + ExtTrigger files" need to use 1000.0 sampling rate, to guarantee 10ms between ExtTrig;
		the "total number of triggers" = "WL PULSE width" / 10ms */
	char f_scan_WLpulse_ExtTrig[200];
	sprintf(f_scan_WLpulse_ExtTrig, "../Scan_files/%sPULSE_MUX_ON_%dExtTrig_1000SampRate", pulse_width_char, Num_of_ExtTrig);

	// scan in WL[0]=1 in column[col], pulse=0
	char f_scan[200];
	sprintf(f_scan, "../Scan_files/Scan_Col%02d_WL0_NOpulse", col);
	scan(f_scan, 0, 100000.0);

	FILE *f_ptr;
	if ((f_ptr = fopen(Measure_file, "w")) == NULL){
		printf("Cannot open%s.\n", Measure_file);
		return FAIL;
	}

	//row: WL[row] number (row=0~107), t: the number of WLpulse (t=1~Num_of_Pulse), j: number of ExtTrig measurement within one WLpulse
	int row, t, j; 
	float NPLCycles = 0.2;  //integration time for Multi-EstTrigger-ed measurements within a WLpulse
	float Isense;
	//the DMM's internal memory can hold 512 readings at most => the maximum number of (ExtTrig) measurements before fetching to output buffer
	float Current[512];
	//the output buffer is formatted as "SD.DDDDDDDDSEDD,SD.DDDDDDDDSEDD,SD.DDDDDDDDSEDD,....", so 15+1(comma) = 16 characters for each reading
	char StrCurrent[16];
	char RdBuffer[12001];

	for (row = 0; row < Num_of_row[col]; row++){
		MM34401A_MeasCurrent_Config(_MM34401A, 10, "IMM", 0.1, 1, 1);
		fprintf(f_ptr, "WL[%d]\n", row);
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VDD_typical); //VDD_DIG=VDD_typicalV
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical); // change VDD_WL => Vgs of WL selected transisor
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);
		DO_USB6008(MUX_Address_file_stress);
		scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
		E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
		Isense = MM34401A_MeasCurrent(_MM34401A); //measure IDSAT + leakage current through Current Meter
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);

		fprintf(f_ptr, "Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_stress, Isense);
		//debug: TODO these printf's waste resource/time???
		printf("Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_stress, Isense);

		DO_USB6008(MUX_Address_file_mirror);
		scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
		E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
		Isense = MM34401A_MeasCurrent(_MM34401A); //measure IDSAT + leakage current through Current Meter
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
		scan("../Scan_files/MUX_OFF", 0, 100000.0);

		fprintf(f_ptr, "Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_mirror, Isense);
		//debug:
		printf("Fresh_IDSAT_WL[%d]_%s=%.12f\n", row, direction_char_mirror, Isense);

		for (t = 1; t <= Num_of_Pulse; t++){

			E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
			scan("../Scan_files/MUX_OFF", 0, 100000.0);
			DO_USB6008(MUX_Address_file_stress);

			scan("../Scan_files/MUX_ON", 0, 100000.0);

			//multiple triggers, single sample
			MM34401A_MeasCurrent_Config(_MM34401A, NPLCycles, "EXT", 0.0, 1, Num_of_ExtTrig);
			//char RdBuffer[12001];

			_ibwrt(_MM34401A, "INITiate");

			E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VGS); //VDD_DIG=VDD_WL=VGS, avoid any un-intentional crowbar current or turn-on diodes
			E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VGS); //VDD_WL=VGS
			E3646A_SetVoltage(_VSPARE_VAB, 2, VDS);     //VA=VDS

			//multiple triggers, single sample
			scan(f_scan_WLpulse_ExtTrig, 0, samp_rate);

			E3646A_SetVoltage(_VSPARE_VAB, 2, 0);
			E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical); //VDD_WL=VDD_typicalV
			E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VDD_typical); //VDD_DIG=VDD_typicalV
			scan("../Scan_files/MUX_OFF", 0, 100000.0);

			_ibwrt(_MM34401A, "FETCh?");
			//_ibwrt(_MM34401A, "READ?");
			_ibrd(_MM34401A, RdBuffer, 12000);
			RdBuffer[ibcntl] = '\0';         // Null terminate the ASCII string    
			//printf("%s\n", RdBuffer);

			for (j = 0; j < Num_of_ExtTrig; j++){
				//output buffer of the DMM is formatted as CSV, of all the readings fetched from its internal memory 
				//each reading has 15 charaters, plus one comma
				strncpy(StrCurrent, RdBuffer + j * 16, 15); 
				StrCurrent[15] = '\0';
				sscanf(StrCurrent, "%f", &Current[j]);
				fprintf(f_ptr, "Stress_%02dPULSE_WL[%d]_ID_%03dms=%.12f\n", t, row, 10 * j, Current[j]);
				//debug:
				printf("Time=%dms, Current=%.12f\n", 10*j, Current[j]);
			}

			MM34401A_MeasCurrent_Config(_MM34401A, 10, "IMM", 0.1, 1, 1);
		/*	E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VDD_typical); //VDD_DIG=VDD_typicalV
			E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical); // change VDD_WL => Vgs of WL selected transisor
			E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
			scan("../Scan_files/MUX_OFF", 0, 100000.0);
			DO_USB6008(MUX_Address_file_stress); */
			scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
			E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
			Isense = MM34401A_MeasCurrent(_MM34401A); //measure Ids + leakage current through Current Meter
			E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
			scan("../Scan_files/MUX_OFF", 0, 100000.0);

			fprintf(f_ptr, "Stress_%02dPULSE_IDSAT_WL[%d]_%s=%.12f\n", t, row, direction_char_stress, Isense);
			//debug:
			printf("Stress_%02dPULSE_IDSAT_WL[%d]_%s=%.12f\n", t, row, direction_char_stress, Isense);

			DO_USB6008(MUX_Address_file_mirror);
			scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
			E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VAB = VDS = VDD_typicalV
			Isense = MM34401A_MeasCurrent(_MM34401A); //measure Ids + leakage current through Current Meter
			E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
			scan("../Scan_files/MUX_OFF", 0, 100000.0);

			fprintf(f_ptr, "Stress_%02dPULSE_IDSAT_WL[%d]_%s=%.12f\n", t, row, direction_char_mirror, Isense);
			//debug:
			printf("Stress_%02dPULSE_IDSAT_WL[%d]_%s=%.12f\n", t, row, direction_char_mirror, Isense);
		}

		// shift down one row
		scan("../Scan_files/Scan_shift_1_NOpulse", 0, 100000.0);
	}
	E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
	scan("../Scan_files/MUX_OFF", 0, 100000.0);
	fclose(f_ptr);
	return 0;
}

//TODO: fix the mistakes about MUX enable and pulse scan files!!!
int stress_checkerboard(int even_odd, double VDS, double VGS, double pulse_width, int chip, int col, int direction){
	/* stress half of the transistors in a certain column
	direction = 0: VAsource, VBdrain (VB=VDS=VD > VA=VS=0)
	direction = 1: VAdrain, VBsource (VA=VDS=VD > VB=VS=0) 
	*** Do full I-V curves characteriations in both directions EVERYTIME before using this function! ***
	even_odd = 0 select even number rows (0, 2, 4, ..., 106)
	even_odd = 1 select odd number rows (1, 3, 5, ..., 107)
	VDS = VA, VGS = VDD_WL = VDD_DIG, 
	pulse_width tougle WL within VDS adjustment window by PSU, in unit of SECOND.
	scan file "Singal_pulse" has 1000 lines of pulse=1, with a final line to reset pulse=0
	baseline default sampling rate=1000, which generates 1 second of pulse high.
	set pulse_width=10 will change sampling rate to 100, which generates 10 seconds of pulse high.
	TODO: double check data type of sampling rate!
	*/
	scan("../Scan_files/NIDAQ_test_data", 1, 100000.0);
	scan("../Scan_files/Scan_all_zero", 0, 100000.0);

	char direction_char[200];
	char MUX_Address_file[200];
	if (direction == 0){
		sprintf(direction_char, "VA = source, VB = drain");
		sprintf(MUX_Address_file, "../Scan_files/MUX_Col%02d_VAsource_VBdrain", col);
	}
	else{
		sprintf(direction_char, "VA = drain, VB = source");
		sprintf(MUX_Address_file, "../Scan_files/MUX_Col%02d_VAdrain_VBsource", col);
	}

	E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
	scan("../Scan_files/MUX_OFF", 0, 100000.0);
	DO_USB6008(MUX_Address_file);

	// scan in WL[0]=1 in column[col], pulse=0
	char f_scan[200];
	sprintf(f_scan, "../Scan_files/Scan_Col%02d_WL0_NOpulse", col);
	scan(f_scan, 0, 100000.0);
	//TODO: scan file error check!
	if (even_odd == 1){
		scan("../Scan_files/Scan_shift_1_NOpulse", 0, 100000.0);
	}
	double samp_rate = 1000.0; // sampling rate = 1000 generates 1 second pulse=1
	samp_rate = samp_rate / pulse_width; 
	// eg. samp_rate = 1000/5 = 200, to generate 5 seconds of pulse
	// eg. samp_rate = 1000/0.1 = 10000, to generate 0.1 seconds of pulse 

	scan("../Scan_files/MUX_ON", 0, 100000.0);
	for (int i = 0; i < 54; i++){
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VGS); //VDD_DIG=VDD_WL=VGS, avoid any un-intentional crowbar current or turn-on diodes
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VGS); //VDD_WL=VGS
		E3646A_SetVoltage(_VSPARE_VAB, 2, VDS);     //VA=VDS

		scan("../Scan_files/Singal_pulse_MUX_ON", 0, samp_rate);

		E3646A_SetVoltage(_VSPARE_VAB, 2, 0);
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical); //VDD_WL=VDD_typicalV
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VDD_typical); //VDD_DIG=VDD_typicalV

		// shift down two rows
		scan("../Scan_files/Scan_shift_1_NOpulse_MUX_ON", 0, 100000.0);
		scan("../Scan_files/Scan_shift_1_NOpulse_MUX_ON", 0, 100000.0);
	}
	E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
	scan("../Scan_files/MUX_OFF", 0, 100000.0);
	return 0;
}

int stress_sweep_VGS_VDS(double pulse_width, int chip, int col, int direction){
	/* stress the transistors in a certain column
	direction = 0: VAsource, VBdrain (VB=VDS=VD > VA=VS=0)
	direction = 1: VAdrain, VBsource (VA=VDS=VD > VB=VS=0)
	*** Do full I-V curves characteriations in both directions EVERYTIME before using this function! ***
	Monitor! VDS <= V(PSU+), VGS = VDD_WL = VDD_DIG,

	pulse_width tougle WL within VDS adjustment window by PSU, in unit of SECOND.
	scan file "Singal_pulse_MUX_ON" has 1000 lines of pulse=1, with a final line to reset pulse=0
	baseline default sampling rate=1000, which generates 1 second of pulse high.
	set pulse_width=10 will change sampling rate to 100, which generates 10 seconds of pulse high.
	TODO: double check data type of sampling rate!
	*/
	scan("../Scan_files/NIDAQ_test_data", 1, 100000.0);
	scan("../Scan_files/Scan_all_zero", 0, 100000.0);

	char direction_char[200];
	char MUX_Address_file[200];
	if (direction == 0){
		sprintf(direction_char, "VA = source, VB = drain");
		sprintf(MUX_Address_file, "../Scan_files/MUX_Col%02d_VAsource_VBdrain", col);
	}
	else{
		sprintf(direction_char, "VA = drain, VB = source");
		sprintf(MUX_Address_file, "../Scan_files/MUX_Col%02d_VAdrain_VBsource", col);
	}

	E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
	scan("../Scan_files/MUX_OFF", 0, 100000.0);
	DO_USB6008(MUX_Address_file);

	// scan in WL[0]=1 in column[col], pulse=0
	char f_scan[200];
	sprintf(f_scan, "../Scan_files/Scan_Col%02d_WL0_NOpulse", col);
	scan(f_scan, 0, 100000.0);
	//TODO: scan file error check!

	double samp_rate = 1000.0; // sampling rate = 1000 generates 1 second pulse=1
	samp_rate = samp_rate / pulse_width;
	// eg. samp_rate = 1000/5 = 200, to generate 5 seconds of WL pulse
	// eg. samp_rate = 1000/0.1 = 10000, to generate 0.1 seconds of WL pulse 

	scan("../Scan_files/MUX_ON", 0, 100000.0);
	for (int i = 0; i < 12; i++){
    /*	WL[9n]: don't stress, to check if {VG=0, VD high} causes degradation
		WL[9n+1]: VDS=0, VGS=1.5
		WL[9n+2]: VDS=0, VGS=1.6
		WL[9n+3]: VDS=0, VGS=1.7
		WL[9n+4]: VDS=0, VGS=1.8
		WL[9n+5]: VDS=0, VGS=1.9
		WL[9n+6]: VDS=0, VGS=2.0
		WL[9n+7]: VDS=0, VGS=2.1
		WL[9n+8]: VDS=0, VGS=2.2
	*/
		//WL[9n]
		scan("../Scan_files/Scan_shift_1_NOpulse_MUX_ON", 0, 100000.0);  // shift down 1 row
		//WL[9n+1]
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, 1.5); //VDD_DIG=VDD_WL=VGS, avoid any un-intentional crowbar current or turn-on diodes
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, 1.5); //VDD_WL=VGS
//		E3646A_SetVoltage(_VSPARE_VAB, 2, 1.8);     //VA=VDS
		scan("../Scan_files/Singal_pulse_MUX_ON", 0, samp_rate);
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0);
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical); //VDD_WL=VDD_typicalV
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VDD_typical); //VDD_DIG=VDD_typicalV
		scan("../Scan_files/Scan_shift_1_NOpulse_MUX_ON", 0, 100000.0);  // shift down 1 row
		//WL[9n+2]
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, 1.6); //VDD_DIG=VDD_WL=VGS, avoid any un-intentional crowbar current or turn-on diodes
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, 1.6); //VDD_WL=VGS
//		E3646A_SetVoltage(_VSPARE_VAB, 2, 1.8);     //VA=VDS
		scan("../Scan_files/Singal_pulse_MUX_ON", 0, samp_rate);
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0);
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical); //VDD_WL=VDD_typicalV
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VDD_typical); //VDD_DIG=VDD_typicalV
		scan("../Scan_files/Scan_shift_1_NOpulse_MUX_ON", 0, 100000.0);  // shift down 1 row
		//WL[9n+3]
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, 1.7); //VDD_DIG=VDD_WL=VGS, avoid any un-intentional crowbar current or turn-on diodes
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, 1.7); //VDD_WL=VGS
//		E3646A_SetVoltage(_VSPARE_VAB, 2, 2.0);     //VA=VDS
		scan("../Scan_files/Singal_pulse_MUX_ON", 0, samp_rate);
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0);
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical); //VDD_WL=VDD_typicalV
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VDD_typical); //VDD_DIG=VDD_typicalV
		scan("../Scan_files/Scan_shift_1_NOpulse_MUX_ON", 0, 100000.0);  // shift down 1 row
		//WL[9n+4]
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, 1.8); //VDD_DIG=VDD_WL=VGS, avoid any un-intentional crowbar current or turn-on diodes
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, 1.8); //VDD_WL=VGS
//		E3646A_SetVoltage(_VSPARE_VAB, 2, 2.0);     //VA=VDS
		scan("../Scan_files/Singal_pulse_MUX_ON", 0, samp_rate);
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0);
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical); //VDD_WL=VDD_typicalV
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VDD_typical); //VDD_DIG=VDD_typicalV
		scan("../Scan_files/Scan_shift_1_NOpulse_MUX_ON", 0, 100000.0);  // shift down 1 row
		//WL[9n+5]
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, 1.9); //VDD_DIG=VDD_WL=VGS, avoid any un-intentional crowbar current or turn-on diodes
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, 1.9); //VDD_WL=VGS
//		E3646A_SetVoltage(_VSPARE_VAB, 2, 2.0);     //VA=VDS
		scan("../Scan_files/Singal_pulse_MUX_ON", 0, samp_rate);
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0);
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical); //VDD_WL=VDD_typicalV
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VDD_typical); //VDD_DIG=VDD_typicalV
		scan("../Scan_files/Scan_shift_1_NOpulse_MUX_ON", 0, 100000.0);  // shift down 1 row
		//WL[9n+6]
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, 2.0); //VDD_DIG=VDD_WL=VGS, avoid any un-intentional crowbar current or turn-on diodes
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, 2.0); //VDD_WL=VGS
//		E3646A_SetVoltage(_VSPARE_VAB, 2, 2.2);     //VA=VDS
		scan("../Scan_files/Singal_pulse_MUX_ON", 0, samp_rate);
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0);
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical); //VDD_WL=VDD_typicalV
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VDD_typical); //VDD_DIG=VDD_typicalV
		scan("../Scan_files/Scan_shift_1_NOpulse_MUX_ON", 0, 100000.0);  // shift down 1 row
		//WL[9n+7]
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, 2.1); //VDD_DIG=VDD_WL=VGS, avoid any un-intentional crowbar current or turn-on diodes
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, 2.1); //VDD_WL=VGS
//		E3646A_SetVoltage(_VSPARE_VAB, 2, 2.2);     //VA=VDS
		scan("../Scan_files/Singal_pulse_MUX_ON", 0, samp_rate);
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0);
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical); //VDD_WL=VDD_typicalV
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VDD_typical); //VDD_DIG=VDD_typicalV
		scan("../Scan_files/Scan_shift_1_NOpulse_MUX_ON", 0, 100000.0);  // shift down 1 row
		//WL[9n+8]
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, 2.2); //VDD_DIG=VDD_WL=VGS, avoid any un-intentional crowbar current or turn-on diodes
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, 2.2); //VDD_WL=VGS
//		E3646A_SetVoltage(_VSPARE_VAB, 2, 2.2);     //VA=VDS
		scan("../Scan_files/Singal_pulse_MUX_ON", 0, samp_rate);
		E3646A_SetVoltage(_VSPARE_VAB, 2, 0);
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical); //VDD_WL=VDD_typicalV
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 1, VDD_typical); //VDD_DIG=VDD_typicalV

	}
	E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
	scan("../Scan_files/MUX_OFF", 0, 100000.0);
	return 0;
}

/******************ADAPTED from Siming_28nm******************/
int IDS_VGS(char *f_name, int col, int chip, int direction) 
//direction = 0: VAsource, VBdrain
//direction = 1: VAdrain, VBsource
{
//	MM34401A_MeasCurrent_Config(_MM34401A, 10, "IMM", 0.1, 1, 1);
	// 1) scan in all Zero's, measure total leakage current through a single column VA --> VB
	// sweep VDD_WL, total leakage should not make a difference!
	char direction_char[200];
	char MUX_Address_file[200];
	if (direction == 0){
		sprintf(direction_char, "VA = source, VB = drain");
		sprintf(MUX_Address_file, "../Scan_files/MUX_Col%02d_VAsource_VBdrain", col);
	}
	else{
		sprintf(direction_char, "VA = drain, VB = source");
		sprintf(MUX_Address_file, "../Scan_files/MUX_Col%02d_VAdrain_VBsource", col);
	}
	scan("../Scan_files/Scan_all_zero", 0, 100000.0);
	double Isense;
	FILE *f_ptr;
	if ((f_ptr = fopen(f_name, "w")) == NULL){
		printf("Cannot open%s.\n", f_name);
		return FAIL;
	}

	double leakage, VDD_WL;
	fprintf(f_ptr, "Total leakage current from column[%d], chip %d, VAB=VDS=%fV\n%s\n", col, chip, VDD_typical, direction_char);
	//scan("../Scan_files/MUX_OFF", 0, 100000.0);
	DO_USB6008("../Scan_files/MUX_OFF"); //all mux disabled
	E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
	DO_USB6008(MUX_Address_file); //MUX enable embedded into mux address file, all using USB-NIDAQ
	//scan("../Scan_files/MUX_ON", 0, 100000.0);

	E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VDS = |VB - VA|= VDD_typicalV
	MM34401A_MeasCurrent_Config(_MM34401A, 10, "IMM", 0.1, 1, 1);
	leakage = MM34401A_MeasCurrent(_MM34401A);
	if (leakage >= 0.00001){
//		printf("total subVt leakage too large! Check!\n");
		fprintf(f_ptr, "total subVt leakage too large! Check!\n");
//		fclose(f_ptr);
//		return FAIL;
	}
	//debug:
//	printf("Leakage = %.12f\n", leakage);
	for (VDD_WL = VDD_typical; VDD_WL >= 0.2 - 0.0001; VDD_WL -= 0.05){
		E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_WL); // change VDD_WL
		Isense = MM34401A_MeasCurrent(_MM34401A); //measure leakage current through Current Meter
		fprintf(f_ptr, "VDD_WL=%f  Isense=%.12f\n", VDD_WL, Isense);
		if (fabs(Isense - leakage) >= 0.000001){
//			printf("total subVt leakage should not change with VDD_WL! Check!\n");
			fprintf(f_ptr, "total subVt leakage should not change with VDD_WL! Check!\n");
//			fclose(f_ptr);
//			return FAIL;
		}
		//debug:
//		printf("VDD_WL=%f  Isense=%.12f\n", VDD_WL, Isense);
	}
	E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
	//scan("../Scan_files/MUX_OFF", 0, 100000.0);
	DO_USB6008("../Scan_files/MUX_OFF"); //all mux disabled
	E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical);

	fprintf(f_ptr, "\nIds-Vgs curves of each fresh transistor, from 0 to %d, in column[%d], chip %d\n%s\n", Num_of_row[col]-1, col, chip, direction_char);
	MM34401A_MeasCurrent_Config(_MM34401A, 10, "IMM", 0.1, 1, 1);
	//MM34401A_MeasCurrent_Config(1);
	// scan in WL[0]=1 in column[col], pulse=1
	char f_scan[200];
	sprintf(f_scan, "../Scan_files/Scan_Col%02d_WL0_pulse", col);
	scan(f_scan, 0, 100000.0);

	//scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
	DO_USB6008(MUX_Address_file); //MUX enable embedded into mux address file, all using USB-NIDAQ
	E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VDS = |VB - VA|= VDD_typicalV
	int WL;
	for (WL = 0; WL < Num_of_row[col]; WL++){
		fprintf(f_ptr, "WL[%d]\n", WL);
		for (VDD_WL = VDD_typical; VDD_WL >= 0.2-0.0001; VDD_WL -= 0.05){
			E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_WL); // change VDD_WL => Vgs of WL selected transisor
			Isense = MM34401A_MeasCurrent(_MM34401A); //measure Ids + leakage current through Current Meter
			fprintf(f_ptr, "VDD_WL=%f  Isense=%.12f  I(sense)-I(leakage)=%.12f\n", VDD_WL, Isense, Isense - leakage);
			//debug:
//			printf("VDD_WL=%f  Isense=%.12f  I(sense)-I(leakage)=%.12f\n", VDD_WL, Isense, Isense - leakage);
		}
		//shift scan bit to the next WL
		scan("../Scan_files/Scan_shift_1", 0, 100000.0); 
	}
	scan("../Scan_files/NOpulse", 0, 100000.0);
	fclose(f_ptr);
	E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical);
	E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
	//scan("../Scan_files/MUX_OFF", 0, 100000.0);
	DO_USB6008("../Scan_files/MUX_OFF"); //all mux disabled
	return 0;
}

//Cautious: out-of-date!!
int IDS_VDS(char* f_name, int col, int chip, int direction){

	// 1) scan in all Zero's, measure total leakage current through a single column VA --> VB
	// reduce VAB=VDS, total leakage should decrease
	char direction_char[200];
	char MUX_Address_file[200];
	if (direction == 0){
		sprintf(direction_char, "VA = source, VB = drain");
		sprintf(MUX_Address_file, "../Scan_files/MUX_Col%02d_VAsource_VBdrain", col);
	}
	else{
		sprintf(direction_char, "VA = drain, VB = source");
		sprintf(MUX_Address_file, "../Scan_files/MUX_Col%02d_VAdrain_VBsource", col);
	}

	E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical);
	scan("../Scan_files/Scan_all_zero", 0, 100000.0);
	E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
	scan("../Scan_files/MUX_OFF", 0, 100000.0);
	DO_USB6008(MUX_Address_file);
//DEBUG:
/*	float I_V_NIDAQ = 0.001*E3646A_MeasAvgCurrent(_VDD_IO_V_NIDAQ, 2);
	float I_VAB = 0.001*E3646A_MeasAvgCurrent(_VSPARE_VAB, 2);
	printf("Before MUX_ON: I(V_NIDAQ) = %f, I(VAB) = %f\n", I_V_NIDAQ, I_VAB);*/

	scan("../Scan_files/MUX_ON", 0, 100000.0);
//DEBUG:
/*	I_V_NIDAQ = 0.001*E3646A_MeasAvgCurrent(_VDD_IO_V_NIDAQ, 2);
	I_VAB = 0.001*E3646A_MeasAvgCurrent(_VSPARE_VAB, 2);
	printf("After MUX_ON: I(V_NIDAQ) = %f, I(VAB) = %f\n", I_V_NIDAQ, I_VAB);*/

//	E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VDS = |VB - VA|= VDD_typicalV
	double Isense;
	FILE *f_ptr;
	if ((f_ptr = fopen(f_name, "w")) == NULL){
		printf("Cannot open%s.\n", f_name);
		return FAIL;
	}
	//Vgs=0, total leakage vs Vds
	double leak_tot[19];
	fprintf(f_ptr, "Total leakage current from column[%d], chip %d, Vgs=0, sweep VAB=Vds\n%s\n", col, chip, direction_char);
	fprintf(f_ptr, "Total Leakage through column[%d]:\n", col);
	float VAB;
	int l = 18;
//	MM34401A_MeasCurrent_Config(100);
	MM34401A_MeasCurrent_Config(_MM34401A, 10, "IMM", 0.1, 1, 1);
	for (VAB = VDD_typical; VAB >= -0.00001; VAB -= 0.05){
		E3646A_SetVoltage(_VSPARE_VAB, 2, VAB);
		Isense = MM34401A_MeasCurrent(_MM34401A); //measure leakage current through Current Meter
		fprintf(f_ptr, "VAB=%f  Isense=%.12f\n", VAB, Isense);
		leak_tot[l] = Isense;
		l--;
		//debug:
		printf("VAB=%f  Isense=%.12f\n", VAB, Isense);
/*		I_V_NIDAQ = 0.001*E3646A_MeasAvgCurrent(_VDD_IO_V_NIDAQ, 2);
		I_VAB = 0.001*E3646A_MeasAvgCurrent(_VSPARE_VAB, 2);
		printf("I(V_NIDAQ) = %f, I(VAB) = %f\n", I_V_NIDAQ, I_VAB);*/
	}
	E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
	scan("../Scan_files/MUX_OFF", 0, 100000.0);
	
	fprintf(f_ptr, "\nIds-Vds curves of each fresh transistor, from 0 to 107, in column[%d]\n%s\n", col, direction_char);
	// scan in WL[0]=1 in column[col], pulse=1
	char f_scan[200];
	sprintf(f_scan, "../Scan_files/Scan_Col%02d_WL0_pulse", col);
	scan(f_scan, 0, 100000.0);
	int WL;
	MM34401A_MeasCurrent_Config(_MM34401A, 10, "IMM", 0.1, 1, 1);
	E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
	scan("../Scan_files/MUX_ON_pulse", 0, 100000.0);
	for (WL = 0; WL < Num_of_row[col]; WL++){
		fprintf(f_ptr, "WL[%d]\n", WL);
		l = 0;
		for (VAB = 0; VAB <= VDD_typical0001; VAB += 0.05){
			E3646A_SetVoltage(_VSPARE_VAB, 2, VAB); // change VAB => Vds of WL selected transistor
			Isense = MM34401A_MeasCurrent(_MM34401A); //measure Ids + leak_tot current through Current Meter
			fprintf(f_ptr, "VAB=%f  Isense=%.12f  I(sense)-I(leak_tot)=%.12f\n", VAB, Isense, Isense - leak_tot[l]);
			//debug:
			printf("VAB=%f  Vsense=%.12f  I(sense)-I(leak_tot)=%.12f\n", VAB, Isense, Isense - leak_tot[l]);
			l++;
		}
		scan("../Scan_files/Scan_shift_1_MUX_ON", 0, 100000.0);
	}
	scan("../Scan_files/NOpulse", 0, 100000.0);
	E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
	scan("../Scan_files/MUX_OFF", 0, 100000.0);
	fclose(f_ptr);
	return 0;
}

int Drain_leakage(char* f_name, int col, int chip, int direction){

	// 1) scan in all Zero's, measure total leakage current through a single column VA --> VB
	// reduce VAB=VDS, total leakage should decrease
	char direction_char[200];
	char MUX_Address_file[200];
	if (direction == 0){
		sprintf(direction_char, "VA = source, VB = drain");
		sprintf(MUX_Address_file, "../Scan_files/MUX_Col%02d_VAsource_VBdrain", col);
	}
	else{
		sprintf(direction_char, "VA = drain, VB = source");
		sprintf(MUX_Address_file, "../Scan_files/MUX_Col%02d_VAdrain_VBsource", col);
	}

	E3646A_SetVoltage(_VDD_DIG_VDD_WL, 2, VDD_typical);
	scan("../Scan_files/Scan_all_zero", 0, 100000.0);
	E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
	scan("../Scan_files/MUX_OFF", 0, 100000.0);
	DO_USB6008(MUX_Address_file);
	//DEBUG:
	/*	float I_V_NIDAQ = 0.001*E3646A_MeasAvgCurrent(_VDD_IO_V_NIDAQ, 2);
	float I_VAB = 0.001*E3646A_MeasAvgCurrent(_VSPARE_VAB, 2);
	printf("Before MUX_ON: I(V_NIDAQ) = %f, I(VAB) = %f\n", I_V_NIDAQ, I_VAB);*/

	scan("../Scan_files/MUX_ON", 0, 100000.0);
	//DEBUG:
	/*	I_V_NIDAQ = 0.001*E3646A_MeasAvgCurrent(_VDD_IO_V_NIDAQ, 2);
	I_VAB = 0.001*E3646A_MeasAvgCurrent(_VSPARE_VAB, 2);
	printf("After MUX_ON: I(V_NIDAQ) = %f, I(VAB) = %f\n", I_V_NIDAQ, I_VAB);*/

	//	E3646A_SetVoltage(_VSPARE_VAB, 2, VDD_typical); // VDS = |VB - VA|= VDD_typicalV
	double Isense_Isub, Isense_ID;
	FILE *f_ptr;
	if ((f_ptr = fopen(f_name, "w")) == NULL){
		printf("Cannot open%s.\n", f_name);
		return FAIL;
	}
	//Vgs=0, total leakage vs Vds
	//int N_VAB = 57;
	//double leak_tot[57];
	fprintf(f_ptr, "Total leakage current from column[%d], chip %d, Vgs=0, sweep VAB=Vds\n%s\n", col, chip, direction_char);
	fprintf(f_ptr, "Total Leakage through column[%d]:\n", col);
	float VAB;
	//int l = N_VAB - 1;
	//	MM34401A_MeasCurrent_Config(100);
	MM34401A_MeasCurrent_Config(_MM34401A, 10, "IMM", 0.1, 1, 1);
	MM34410A_6_MeasCurrent_Config(_MM34410A_6, 10, "IMM", 0.1, 1, 1);
	for (VAB = 0; VAB <= 3.20001; VAB += 0.05){
		E3646A_SetVoltage(_VSPARE_VAB, 2, VAB);
		Isense_Isub = MM34401A_MeasCurrent(_MM34410A_6); //measure substrate current through Current Meter
		Isense_ID = MM34401A_MeasCurrent(_MM34401A); //measure Drain side leakage current through Current Meter
		fprintf(f_ptr, "VAB=%f  Isub=%.12f\n", VAB, Isense_Isub);
		fprintf(f_ptr, "VAB=%f  ID=%.12f\n", VAB, Isense_ID);
		//leak_tot[l] = Isense;
		//l--;
		//debug:
		//printf("VAB=%f  Isense=%.12f\n", VAB, Isense);
		/*		I_V_NIDAQ = 0.001*E3646A_MeasAvgCurrent(_VDD_IO_V_NIDAQ, 2);
		I_VAB = 0.001*E3646A_MeasAvgCurrent(_VSPARE_VAB, 2);
		printf("I(V_NIDAQ) = %f, I(VAB) = %f\n", I_V_NIDAQ, I_VAB);*/
	}
	E3646A_SetVoltage(_VSPARE_VAB, 2, 0); // VDS = |VB - VA|= 0V
	scan("../Scan_files/MUX_OFF", 0, 100000.0);
	fclose(f_ptr);
	return 0;
}


/******************************
Scan Chain Load Function
******************************/

int scan(char *fn_scanin, int compareMode, float64 samp_freq)
{
	// NIDAQ-6115 (PCI card) 
	// digital output port for timing critical signals
	// "dev1/port0/line0:7": 
	// line0,   line1,   line2,   line3,   line4,                                  line5,        line6,       line7
	// S_IN,    PHI_1,   PHI_2,  PULSE_IN, unused (reserved for DMM ex-trigger),   /CS(unused), /WR(unused) /EN(unused).
	// analog input port "Dev1/ai0" monitor scan out from chip: S_OUT (VDD_IO=1.8V)

	#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

	int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData); 
	//This function is defined in the counter output example code

	// Variables for Error Buffer
	int32 error=0;
	char errBuff[2048]={'\0'};

	//NIDAQ ANSI C example code, counter/digital pulse train continuous generation
	//we use this counter output for the external clock source of digital output and analog input channel sampling clock
	TaskHandle  taskHandleCounter0 = 0;
	DAQmxErrChk(DAQmxCreateTask("", &taskHandleCounter0));
	//The output frequency is the target frequency for the sampling clock.
	DAQmxErrChk(DAQmxCreateCOPulseChanFreq(taskHandleCounter0, "Dev1/ctr0", "", DAQmx_Val_Hz, DAQmx_Val_Low, 0.0, samp_freq, 0.50));
	//continuous output, specify a large enough buffer size
	DAQmxErrChk(DAQmxCfgImplicitTiming(taskHandleCounter0, DAQmx_Val_ContSamps, 1000));
	DAQmxErrChk(DAQmxRegisterDoneEvent(taskHandleCounter0, 0, DoneCallback, NULL));

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/
	//We SHOULD NOT start the counter task now!!
//	DAQmxErrChk(DAQmxStartTask(taskHandleCounter0));
//	printf("Generating pulse train. Press Enter to interrupt\n");
//	getchar();

	//Variables for Processing of Input lines (output to chip)
	TaskHandle taskHandleScanin=0;
	TaskHandle taskHandleScanout=0;
	uInt8 scaninData[MAX_LINES][NUM_OUTPUT];

// bit-wise digital line format:
//	uInt8 scaninData[14] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	int scanoutData [MAX_LINES];
	
	int total_lines_read = 0;
	int SCAN_EQUAL = 1;
	
	float64 anaInputBuffer[MAX_LINES];
	int32 read,bytesPerSamp;

	int linecount = 0;

	/*********************************************/
	//  FILE I/O Intialization
	/*********************************************/

	//file IO variables
	FILE *fptr_scanin;

	if ((fptr_scanin = fopen(fn_scanin, "r")) == NULL){
		printf("Cannot open%s.\n", fn_scanin);
		return FAIL;
	}

	//printf("DEBUG:scanin file found\n");
		
	/*********************************************/
	// READ vectors from input file
	/*********************************************/
	int input_char;

	scanChainRead_Error = 0;

	//loop for each line of file I/O
	while (((input_char = fgetc(fptr_scanin)) != EOF) && (linecount <  MAX_LINES)) {
		//loop to read an input line
		int inputcnt = 0;
		while (input_char != '\n'){
			if (input_char == '1'){
				//for bit-wise digital format:
//				scaninData[linecount] = (scaninData[linecount]<<1) + 1;
				scaninData[linecount][inputcnt] = 1;
				inputcnt++;
			}
			else if ((input_char == '0') | (input_char == 'x') |
				(input_char == 'X') | (input_char == 'z') | 
				(input_char == 'Z')){
//				scaninData[linecount] = (scaninData[linecount]<<1) + 0;
				scaninData[linecount][inputcnt] = 0;
				inputcnt++;
			}

			if ((input_char=fgetc(fptr_scanin)) == EOF)
				break;
		}

		if (inputcnt != NUM_OUTPUT){
			printf ("Input line %d was not formatted correctly, received %d input characters\n", linecount, inputcnt);
			printf ("Data: ");
			for (int i=0; i<NUM_OUTPUT; i++)
//				printf ("%d ", scaninData[linecount][i]);
			printf("\n");
			scanChainRead_Error = 1;
		}
		linecount++;
		//printf("DEBUG: Read line %d\n", linecount);
	}
	fclose(fptr_scanin);
	total_lines_read = linecount;
	//printf("DEBUG: Finished Reading input file\n");

	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk (DAQmxCreateTask("OutputTask",&taskHandleScanin));
	DAQmxErrChk (DAQmxCreateDOChan(taskHandleScanin,outchannel_str,"outchannel",DAQmx_Val_ChanForAllLines));
    //implicit timing means software control, i.e. NIDAQ generate one sample output once it receives a software program command.
	//the timing is undeterministic because the variation in software runtime speed.
//	DAQmxCfgImplicitTiming (taskHandleScanin, DAQmx_Val_FiniteSamps,1);

	//Configure an output buffer equal or larger than the number of output lines
	DAQmxErrChk(DAQmxCfgOutputBuffer(taskHandleScanin, total_lines_read));

// For NI6115 device, "OnBoardClock", (MasterTimebase) and "" or NULL are not legal samplig clock source
// While "20MHzTimebase" and "100kHzTimebase" are fixed frequency clock source that are not impacted by the sampling rate settings
//	DAQmxErrChk(DAQmxCfgSampClkTiming(taskHandleScanin, "MasterTimebase", 1000, DAQmx_Val_Rising, DAQmx_Val_FiniteSamps, 14));

	//Using the counter ouput, set the sampling rate as the same as the counter task setting
	DAQmxErrChk(DAQmxCfgSampClkTiming(taskHandleScanin, "/dev1/ctr0Out", samp_freq, DAQmx_Val_Rising, DAQmx_Val_FiniteSamps, total_lines_read));

	DAQmxErrChk (DAQmxCreateTask("ReadTask",&taskHandleScanout));
	//TODO: adjust AI range to increase resolution!
	DAQmxErrChk(DAQmxCreateAIVoltageChan(taskHandleScanout, inchannel_str, "inchannel", DAQmx_Val_Cfg_Default, 0.0, VDD_NIDAQ + 0.1, DAQmx_Val_Volts, NULL));
	//Also use the counter ouput to synchronize with digital output generation, set the sampling rate as the same as the counter task setting
//	DAQmxErrChk(DAQmxCfgSampClkTiming(taskHandleScanin, "/dev1/ctr0out", 1000.0, DAQmx_Val_Rising, DAQmx_Val_FiniteSamps, 108 * 12));
	DAQmxErrChk(DAQmxCfgSampClkTiming(taskHandleScanout, "/dev1/ctr0out", samp_freq, DAQmx_Val_Rising, DAQmx_Val_FiniteSamps, total_lines_read));
	//the AI task should have an implicitly allocated buffer to hold all acquired data after StartTask before ReadAnalogF64.

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/
	// Do NOT start task here!
	// For buffered/hard-timed generation task, we need to first write all the data into buffer before starting the task
//	DAQmxErrChk (DAQmxStartTask(taskHandleScanin));
//	DAQmxErrChk (DAQmxStartTask(taskHandleScanout));

	/*********************************************/
	// DAQmx Read and Write Code - Loop
	/*********************************************/
	// if compareMode == 1, scan in twice, and compare the second-time scan out data with scan in data
	for (int scan_times = 0; scan_times <= compareMode; scan_times++){

		for (linecount = 0; linecount < total_lines_read; linecount++){
			//Write Code
			//Write digital data into output buffer, who holds the data before StartTask for real generating output
			// For this DO task, explicitly calling CfgOutputBuffer is necessary: 
			// otherwise the implicitly allocated buffer size defaults to 1,
			// because WriteDigitalLines is called one line at a time in a for loop!
			DAQmxErrChk(DAQmxWriteDigitalLines(taskHandleScanin, 1, 0, 20.0, DAQmx_Val_GroupByChannel, scaninData[linecount], NULL, NULL));
			// for bit-wise digital format
			//			DAQmxErrChk(DAQmxWriteDigitalU8(taskHandleScanin, 14, 0, 10.0, DAQmx_Val_GroupByChannel, scaninData, NULL, NULL));
			//			DAQmxErrChk(DAQmxStartTask(taskHandleScanin));
		}


		//first start the digital output and analog input tasks, 
		//but because the counter task has not started yet, no sampling clock ticks, these two tasks are ready and waiting
		DAQmxErrChk(DAQmxStartTask(taskHandleScanin));
		if (scan_times == 1){
			DAQmxErrChk(DAQmxStartTask(taskHandleScanout));
		}
		DAQmxErrChk(DAQmxStartTask(taskHandleCounter0));
		//		DAQmxErrChk(DAQmxReadAnalogF64(taskHandleScanout, -1, 10.0, DAQmx_Val_GroupByChannel, anaInputBuffer, total_lines_read, &read, NULL));
		//Now start the counter task, to guarantee input and output tasks start together and synchronized
		// may not start together if starting the counter task first!
		// Synchronised to counter0 sampling clock, DO generate samples from the previously filled output buffer, AI acquires samples into default internal buffer
		if (scan_times == 1){
			// DAQmxReadAnalogF64 read the acquired AI data from internal default buffer into the specified array "anaInputBuffer"
			DAQmxErrChk(DAQmxReadAnalogF64(taskHandleScanout, -1, 20.0, DAQmx_Val_GroupByChannel, anaInputBuffer, total_lines_read, &read, NULL));
			DAQmxStopTask(taskHandleScanout);
		}
		// CHECK: ANSI C code example (finite_AI_ExtClk)
		// The correct order: 
		//(1) StartTask: Scanin, Scanout 
		//(2) StartTask: Counter0
		//(3) ReadAnalogF64: Scanout
		// After (1) and (2), program proceeds without a waiting even if the generation and acquisation are still in progress.
		// Program wait at (3) for it to finish for a maximum duration of timeout

		// wait indefinitely until the task is done
		int taskDone;
		DAQmxErrChk(taskDone = DAQmxWaitUntilTaskDone(taskHandleScanin, -1));

		DAQmxStopTask(taskHandleScanin);
//		DAQmxStopTask(taskHandleScanout);
		DAQmxStopTask(taskHandleCounter0);
	}

	if (compareMode == 1){
		for (linecount = 0; linecount < total_lines_read; linecount++){
			// write scan out into scanoutData
			// S_OUT(VDD_IO) terminal from chip go through level shifter, shifted to VDD_NIDAQ = 5V
			if (anaInputBuffer[linecount] > VDD_NIDAQ * 0.5) {
				scanoutData[linecount] = 1;
			}
			else {
				scanoutData[linecount] = 0;
			}
		}
	}
		// Check if scan out is correct
		// use PHI_1 (scaninData[linecount][1]) to sample scanoutData

	// TODO: figure out why scan out samples are offset from scan in by about 21 sampling clock points!
	// DEFAULT DELAY cycles before the first AI sampling clock triggering? 
	// Understand how hard-wired AI (external/ NIDAQ board internal counter generated) sampling clock timing works!
	// route out and probe ai/SamplingClk from PFI7 (configure it as an output!)
	// Also refresh / double-check: chip internal circuit architecture -- scan chain arrangement / length??

	if (compareMode == 1) {
		for (linecount = 0; linecount < total_lines_read; linecount++){
			if (scaninData[linecount][1] == 1) {
				if (scanoutData[linecount] != scaninData[linecount][0]){
					SCAN_EQUAL = 0;
					printf("line=%d\n", linecount);
					printf("scanin=%d\n", scaninData[linecount][0]);
					printf("scanout=%d, AI=%f\n", scanoutData[linecount], anaInputBuffer[linecount]);
				}
			}
		}
	}

	/*******************************************/
	// Output
	/*******************************************/
	//printf("SCAN OKAY\n");
	if (compareMode == 1) {
		printf("SCAN_EQUAL: %d  \n", SCAN_EQUAL);
	}

Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	if( taskHandleScanin!=0 ) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandleScanin);
		DAQmxClearTask(taskHandleScanin);
	}
	// Andrew added following section for taskHandleScanout
	if( taskHandleScanout!=0 ) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandleScanout);
		DAQmxClearTask(taskHandleScanout);
	}
	// I added this for counter out
	if (taskHandleCounter0 != 0){
		DAQmxStopTask(taskHandleCounter0);
		DAQmxClearTask(taskHandleCounter0);
	}
	if( DAQmxFailed(error) )
		printf("DAQmx Error: %s\n",errBuff);
	fclose(fptr_scanin);
	return 0;
}


int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData)
{
	int32   error = 0;
	char    errBuff[2048] = { '\0' };

	// Check to see if an error stopped the task.
	DAQmxErrChk(status);

Error:
	if (DAQmxFailed(error)) {
		DAQmxGetExtendedErrorInfo(errBuff, 2048);
		DAQmxClearTask(taskHandle);
		printf("DAQmx Error: %s\n", errBuff);
	}
	return 0;
}

int DO_USB6008(char *fn_scanin)
{
	//NIDAQ-USB6008, digital output port: "dev2/port0/line0:7"
	//generate timing non-critical signals, including MUX addresses A[0:4]
	// line0, line1, line2, line3, line4,          line5,           line6,         line7,       line8,        line9
	//  A0,    A1,    A2,     A3,   A4,     /EN0=/WR0=/CS0, /EN1=/WR1=/CS1, /EN2=/WR2=/CS2, /EN3=/WR3=/CS3, POLARITY {=0 for write, =1 for erase)
	// (Due to NIDAQ hardware limitation?) use on-demand timing one-sample a time ("implicit timing", software controlled immediate output)

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else
	// Variables for Error Buffer
	int32 error = 0;
	char errBuff[2048] = { '\0' };


	//Variables for Processing of Input lines (output to chip)
	TaskHandle taskHandleScanin = 0;

	uInt8 scaninData[MAX_LINES][NUM_OUTPUT_USB6008];

	int total_lines_read = 0;

	int linecount = 0;

	/*********************************************/
	//  FILE I/O Intialization
	/*********************************************/

	//file IO variables
	FILE *fptr_scanin;

	if ((fptr_scanin = fopen(fn_scanin, "r")) == NULL){
		printf("Cannot open%s.\n", fn_scanin);
		return FAIL;
	}

	//printf("DEBUG:scanin file found\n");

	/*********************************************/
	// READ vectors from input file
	/*********************************************/
	int input_char;

	scanChainRead_Error = 0;

	//loop for each line of file I/O
	while (((input_char = fgetc(fptr_scanin)) != EOF) && (linecount < MAX_LINES)) {
		//loop to read an input line
		int inputcnt = 0;
		while (input_char != '\n'){
			if (input_char == '1'){
				scaninData[linecount][inputcnt] = 1;
				inputcnt++;
			}
			else if ((input_char == '0') | (input_char == 'x') |
				(input_char == 'X') | (input_char == 'z') |
				(input_char == 'Z')){
				scaninData[linecount][inputcnt] = 0;
				inputcnt++;
			}

			if ((input_char = fgetc(fptr_scanin)) == EOF)
				break;
		}

		if (inputcnt != NUM_OUTPUT_USB6008){
			printf("Input line %d was not formatted correctly, received %d input characters\n", linecount, inputcnt);
			printf("Data: ");
			for (int i = 0; i<NUM_OUTPUT_USB6008; i++)
				printf("%d ", scaninData[linecount][i]);
			printf("\n");
			scanChainRead_Error = 1;
		}
		linecount++;
		//printf("DEBUG: Read line %d\n", linecount);
	}
	fclose(fptr_scanin);
	total_lines_read = linecount;
	//printf("DEBUG: Finished Reading input file\n");

	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk(DAQmxCreateTask("OutputTask", &taskHandleScanin));
	DAQmxErrChk(DAQmxCreateDOChan(taskHandleScanin, outchannel_str_USB6008, "outchannel", DAQmx_Val_ChanForAllLines));
	DAQmxCfgImplicitTiming(taskHandleScanin, DAQmx_Val_FiniteSamps, 1);

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/
	//	DAQmxErrChk (DAQmxStartTask(taskHandleScanin));
	//	DAQmxErrChk (DAQmxStartTask(taskHandleScanout));

	/*********************************************/
	// DAQmx Read and Write Code - Loop
	/*********************************************/
	for (linecount = 0; linecount < total_lines_read; linecount++){
			//Write Code
		DAQmxErrChk(DAQmxWriteDigitalLines(taskHandleScanin, 1, 1, 10.0, DAQmx_Val_GroupByChannel, scaninData[linecount], NULL, NULL));
	}
	/*******************************************/
	// Output
	/*******************************************/
	//printf("SCAN OKAY\n");

Error:
	if (DAQmxFailed(error))
		DAQmxGetExtendedErrorInfo(errBuff, 2048);
	if (taskHandleScanin != 0) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandleScanin);
		DAQmxClearTask(taskHandleScanin);
	}
	// Andrew added following section for taskHandleScanout

	if (DAQmxFailed(error))
		printf("DAQmx Error: %s\n", errBuff);
	fclose(fptr_scanin);
	return 0;
}



int scanInternalRead(char *fn_scanin, int *RegsOut)
{
#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else
	
	// Variables for Error Buffer
	int32 error = 0;
	char errBuff[2048] = { '\0' };

	//Variables for Processing of Input lines (output to chip)
	TaskHandle taskHandleScanin = 0;
	TaskHandle taskHandleScanout = 0;
	uInt8 scaninData[MAX_LINES][NUM_OUTPUT];
	int scanoutData[MAX_LINES];
//	int internalRegs[NUM_SCAN_BITS];

	int total_lines_read = 0;
	int SCAN_EQUAL = 1;
	int ScanOutNumber = 0;

	float64 anaInputBuffer[MAX_LINES];
	int32 read, bytesPerSamp;

	int linecount = 0;

	/*********************************************/
	//  FILE I/O Intialization
	/*********************************************/

	//file IO variables
	FILE *fptr_scanin;

	if ((fptr_scanin = fopen(fn_scanin, "r")) == NULL){
		printf("Cannot open%s.\n", fn_scanin);
		return FAIL;
	}
	//printf("DEBUG:scanin file found\n");

	/*********************************************/
	// READ vectors from input file
	/*********************************************/
	int input_char;

	//loop for each line of file I/O
	while (((input_char = fgetc(fptr_scanin)) != EOF) && (linecount < MAX_LINES)) {
		//loop to read an input line
		int inputcnt = 0;
		while (input_char != '\n'){
			if (input_char == '1'){
				scaninData[linecount][inputcnt] = 1;
				inputcnt++;
			}
			else if ((input_char == '0') | (input_char == 'x') |
				(input_char == 'X') | (input_char == 'z') |
				(input_char == 'Z')){
				scaninData[linecount][inputcnt] = 0;
				inputcnt++;
			}

			if ((input_char = fgetc(fptr_scanin)) == EOF)
				break;
		}

		if (inputcnt != NUM_OUTPUT){
			printf("Input line %d was not formatted correctly, received %d input characters\n", linecount, inputcnt);
			printf("Data: ");
			for (int i = 0; i<NUM_OUTPUT; i++)
				printf("%d ", scaninData[linecount][i]);
			printf("\n");
		}
		linecount++;
		//printf("DEBUG: Read line %d\n", linecount);
	}

	fclose(fptr_scanin);
	total_lines_read = linecount;
	//printf("DEBUG: Finished Reading input file\n");

	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk(DAQmxCreateTask("OutputTask", &taskHandleScanin));
	DAQmxErrChk(DAQmxCreateDOChan(taskHandleScanin, outchannel_str, "outchannel", DAQmx_Val_ChanForAllLines));
	DAQmxCfgImplicitTiming(taskHandleScanin, DAQmx_Val_FiniteSamps, 1);

	DAQmxErrChk(DAQmxCreateTask("ReadTask", &taskHandleScanout));
	DAQmxErrChk(DAQmxCreateAIVoltageChan(taskHandleScanout, inchannel_str, "inchannel", DAQmx_Val_Cfg_Default, 0.0, 10.0, DAQmx_Val_Volts, NULL));

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/
	DAQmxErrChk(DAQmxStartTask(taskHandleScanin));
	DAQmxErrChk(DAQmxStartTask(taskHandleScanout));

	/*********************************************/
	// DAQmx Read and Write Code - Loop
	/*********************************************/

	for (linecount = 0; linecount<total_lines_read; linecount++){
		//Write Code
		DAQmxErrChk(DAQmxWriteDigitalLines(taskHandleScanin, 1, 1, 10.0, DAQmx_Val_GroupByChannel, scaninData[linecount], NULL, NULL));
		//Read Signals
		DAQmxErrChk(DAQmxReadAnalogF64(taskHandleScanout, 1, 10.0, DAQmx_Val_GroupByChannel, anaInputBuffer, 1, &read, NULL));

		// write scan out into scanoutData
		if (anaInputBuffer[0]>2) {
			scanoutData[linecount] = 1;
		}
		else {
			scanoutData[linecount] = 0;
		}

		// Use clk2 to Store Internal Register Values to Array
		if (scaninData[linecount][1] == 1) {
			RegsOut[9067 - ScanOutNumber] = scanoutData[linecount];
			ScanOutNumber = ScanOutNumber + 1;
		}
	}
	printf("Read Out Scan Bit %d\n", ScanOutNumber);
	//	printf("Reading Out Scan Done\n");
	/*******************************************/
	// Output
	/*******************************************/
	printf("SCAN OKAY\n");

Error:
	if (DAQmxFailed(error))
		DAQmxGetExtendedErrorInfo(errBuff, 2048);
	if (taskHandleScanin != 0) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandleScanin);
		DAQmxClearTask(taskHandleScanin);
	}
	// Andrew added following section for taskHandleScanout
	if (taskHandleScanout != 0) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandleScanout);
		DAQmxClearTask(taskHandleScanout);
	}

	if (DAQmxFailed(error))
		printf("DAQmx Error: %s\n", errBuff);
	fclose(fptr_scanin);
	return 0;
}

int scanFileChk(char *fn_scanin)
{
#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else
	// Variables for Error Buffer
	int32 error = 0;
	char errBuff[2048] = { '\0' };

	//Variables for Processing of Input lines (output to chip)
	uInt8 scaninData[MAX_LINES][NUM_OUTPUT];
	int scanoutData[MAX_LINES];

	int total_lines_read = 0;

	int linecount = 0;

	/*********************************************/
	//  FILE I/O Intialization
	/*********************************************/

	//file IO variables
	FILE *fptr_scanin;

	if ((fptr_scanin = fopen(fn_scanin, "r")) == NULL){
		printf("Cannot open%s.\n", fn_scanin);
		return FAIL;
	}

	//printf("DEBUG:scanin file found\n");

	/*********************************************/
	// READ vectors from input file
	/*********************************************/
	int input_char;

	scanChainRead_Error = 0;

	//loop for each line of file I/O
	while (((input_char = fgetc(fptr_scanin)) != EOF) && (linecount < MAX_LINES)) {
		//loop to read an input line
		int inputcnt = 0;
		while (input_char != '\n'){
			if (input_char == '1'){
				scaninData[linecount][inputcnt] = 1;
				inputcnt++;
			}
			else if ((input_char == '0') | (input_char == 'x') |
				(input_char == 'X') | (input_char == 'z') |
				(input_char == 'Z')){
				scaninData[linecount][inputcnt] = 0;
				inputcnt++;
			}

			if ((input_char = fgetc(fptr_scanin)) == EOF)
				break;
		}

		if (inputcnt != NUM_OUTPUT){
			printf("Input line %d was not formatted correctly, received %d input characters\n", linecount, inputcnt);
			printf("Data: ");
			for (int i = 0; i<NUM_OUTPUT; i++)
				printf("%d ", scaninData[linecount][i]);
			printf("\n");
			scanChainRead_Error = 1;
		}
		linecount++;
		//printf("DEBUG: Read line %d\n", linecount);
	}
	fclose(fptr_scanin);
	total_lines_read = linecount;

	if (total_lines_read != 36276) {
		scanChainRead_Error = 1;
	}
	//printf("DEBUG: Finished Reading input file\n");

	return 0;
}
