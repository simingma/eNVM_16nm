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

	int chip = 7;
	int col = 21;
	char Measure_file[200];

/*	int Num_of_Pulse = 12;
	int PulseCycle;

	double VDS_L36[128] = { 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0,
		                    2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0,
							2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0,
							2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0 };
	//double VDS_L16[32] = { 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 0, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 0, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 0, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 1.7, 0 };
	double VGS[128] = { 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 
						1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 
						1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 
						1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8 };
*/

/*	sprintf(Measure_file, "../Data/Stress_Chip%02d_Col%02d_Ids_Vgs_VAsource_VBdrain_AfterChargePumping", chip, col);
	IDS_VGS(Measure_file, col, chip, 0);
	sprintf(Measure_file, "../Data/Stress_Chip%02d_Col%02d_Ids_Vgs_VAdrain_VBsource_AfterChargePumping", chip, col);
	IDS_VGS(Measure_file, col, chip, 1); 

/*	sprintf(Measure_file, "../Data/Fresh_Chip%02d_Col%02d_Ids_Vgs_VAsource_VBdrain_01", chip, col);
	IDS_VGS(Measure_file, col, chip, 0);
	sprintf(Measure_file, "../Data/Fresh_Chip%02d_Col%02d_Ids_Vgs_VAdrain_VBsource_01", chip, col);
	IDS_VGS(Measure_file, col, chip, 1);

/*	double CP_VS = 1.0;
	double CP_VD = 1.0;
	double CP_VB = 1.0;*/
/*	double CP_VS = 0.9;
	double CP_VD = 0.9;
	double CP_VB = 0.9;*/

//	double CP_VDD_DIG = 1.7;
	double CP_VDD_DIG = 1.6;
	double CP_VSS_WL = 0;
//	double CP_VDD_WL = 1.7;
	double CP_VDD_WL = 1.6;
	double Num_of_ExtTrig = 60;
//	double Num_of_Sample = 1;
//	double Trig_Delay = 0;
	

//	double samp_rate = 1000;

/*	sprintf(Measure_file, "../Data/HCI_Chip%02d_Col%02d_2Pumping_1kHz_VSVBVD1p0_VSS_WL_0_VDD_WL_1p7_1kRsense_InAmp0RG", chip, col);
	Charge_Pumping(Measure_file, CP_VD, CP_VB, CP_VS, CP_VDD_DIG, CP_VSS_WL, CP_VDD_WL, "ChargePumping_0Delay_1Samp_1msExtTrig_2Pumping_100PLC_1kHz", samp_rate, Num_of_ExtTrig, Num_of_Sample, Trig_Delay, chip, col, 0, 1);

	sprintf(Measure_file, "../Data/HCI_Chip%02d_Col%02d_2DC_1kHz_VSVBVD1p0_VSS_WL_0_VDD_WL_1p7_1kRsense_InAmp0RG", chip, col);
	Charge_Pumping(Measure_file, CP_VD, CP_VB, CP_VS, CP_VDD_DIG, CP_VSS_WL, CP_VDD_WL, "ChargePumping_0Delay_1Samp_1msExtTrig_2DC_100PLC_1kHz", samp_rate, Num_of_ExtTrig, Num_of_Sample, Trig_Delay, chip, col, 0, 1);

/*	Num_of_ExtTrig = 20;
	sprintf(Measure_file, "../Data/Fresh_Chip%02d_Col%02d_DC_1kHz_VSVBVD1p0_VSS_WL_0_VDD_WL_1p7", chip, col);
	Charge_Pumping(Measure_file, CP_VD, CP_VB, CP_VS, CP_VDD_DIG, CP_VSS_WL, CP_VDD_WL, "ChargePumping_0Delay_1Samp_1msExtTrig_20DC_100PLC_1kHz", samp_rate, Num_of_ExtTrig, Num_of_Sample, Trig_Delay, chip, col, 0);
*/

/*	sprintf(Measure_file, "../Data/HCI_Chip%02d_Col%02d_1DC_1Pumping_1kHz_VSVBVD1p0_VSS_WL_0_VDD_WL_1p7_1kRsense_InAmp0RG", chip, col);
	Charge_Pumping(Measure_file, CP_VD, CP_VB, CP_VS, CP_VDD_DIG, CP_VSS_WL, CP_VDD_WL, "ChargePumping_0Delay_1Samp_1msExtTrig_1DC_1Pumping_100PLC_1kHz", samp_rate, Num_of_ExtTrig, Num_of_Sample, Trig_Delay, chip, col, 0, 1);


	samp_rate = 10000;

	sprintf(Measure_file, "../Data/HCI_Chip%02d_Col%02d_2Pumping_10kHz_VSVBVD1p0_VSS_WL_0_VDD_WL_1p7_1kRsense_InAmp0RG", chip, col);
	Charge_Pumping(Measure_file, CP_VD, CP_VB, CP_VS, CP_VDD_DIG, CP_VSS_WL, CP_VDD_WL, "ChargePumping_0Delay_1Samp_100usExtTrig_2Pumping_100PLC_10kHz", samp_rate, Num_of_ExtTrig, Num_of_Sample, Trig_Delay, chip, col, 0, 1);

	sprintf(Measure_file, "../Data/HCI_Chip%02d_Col%02d_2DC_10kHz_VSVBVD1p0_VSS_WL_0_VDD_WL_1p7_1kRsense_InAmp0RG", chip, col);
	Charge_Pumping(Measure_file, CP_VD, CP_VB, CP_VS, CP_VDD_DIG, CP_VSS_WL, CP_VDD_WL, "ChargePumping_0Delay_1Samp_100usExtTrig_2DC_100PLC_10kHz", samp_rate, Num_of_ExtTrig, Num_of_Sample, Trig_Delay, chip, col, 0, 1);

	sprintf(Measure_file, "../Data/HCI_Chip%02d_Col%02d_1DC_1Pumping_10kHz_VSVBVD1p0_VSS_WL_0_VDD_WL_1p7_1kRsense_InAmp0RG", chip, col);
	Charge_Pumping(Measure_file, CP_VD, CP_VB, CP_VS, CP_VDD_DIG, CP_VSS_WL, CP_VDD_WL, "ChargePumping_0Delay_1Samp_100usExtTrig_1DC_1Pumping_100PLC_10kHz", samp_rate, Num_of_ExtTrig, Num_of_Sample, Trig_Delay, chip, col, 0, 1);


    samp_rate = 100000;

	sprintf(Measure_file, "../Data/HCI_Chip%02d_Col%02d_2Pumping_100kHz_VSVBVD1p0_VSS_WL_0_VDD_WL_1p7_1kRsense_InAmp0RG", chip, col);
	Charge_Pumping(Measure_file, CP_VD, CP_VB, CP_VS, CP_VDD_DIG, CP_VSS_WL, CP_VDD_WL, "ChargePumping_0Delay_1Samp_10usExtTrig_2Pumping_100PLC_100kHz", samp_rate, Num_of_ExtTrig, Num_of_Sample, Trig_Delay, chip, col, 0, 1);

	sprintf(Measure_file, "../Data/HCI_Chip%02d_Col%02d_2DC_100kHz_VSVBVD1p0_VSS_WL_0_VDD_WL_1p7_1kRsense_InAmp0RG", chip, col);
	Charge_Pumping(Measure_file, CP_VD, CP_VB, CP_VS, CP_VDD_DIG, CP_VSS_WL, CP_VDD_WL, "ChargePumping_0Delay_1Samp_10usExtTrig_2DC_100PLC_100kHz", samp_rate, Num_of_ExtTrig, Num_of_Sample, Trig_Delay, chip, col, 0, 1);

	sprintf(Measure_file, "../Data/HCI_Chip%02d_Col%02d_1DC_1Pumping_100kHz_VSVBVD1p0_VSS_WL_0_VDD_WL_1p7_1kRsense_InAmp0RG", chip, col);
	Charge_Pumping(Measure_file, CP_VD, CP_VB, CP_VS, CP_VDD_DIG, CP_VSS_WL, CP_VDD_WL, "ChargePumping_0Delay_1Samp_10usExtTrig_1DC_1Pumping_100PLC_100kHz", samp_rate, Num_of_ExtTrig, Num_of_Sample, Trig_Delay, chip, col, 0, 1);

    samp_rate = 1000000;

	sprintf(Measure_file, "../Data/HCI_Chip%02d_Col%02d_1DC_1Pumping_1MHz_VSVBVD1p0_VSS_WL_0_VDD_WL_1p7_1kRsense_InAmp0RG", chip, col);
	Charge_Pumping(Measure_file, CP_VD, CP_VB, CP_VS, CP_VDD_DIG, CP_VSS_WL, CP_VDD_WL, "ChargePumping_0Delay_1Samp_10usExtTrig_1DC_1Pumping_100PLC_1MHz", samp_rate, Num_of_ExtTrig, Num_of_Sample, Trig_Delay, chip, col, 0, 1);
	*/
	//samp_rate = 10000000;
	double samp_rate = 10.0;
//	double pumping_freq = 5000000.0;
	double pumping_freq[8] = { 5000000, 2500000, 1000000, 100000, 10000, 1000, 100, 10 };
	int Num_of_freq = 8;
	//Trig_Delay = 0.02;

/*	sprintf(Measure_file, "../Data/HCI_Chip%02d_Col%02d_60DC_60Pumping_5MHz_VSVBVD0p9_VSS_WL_0_VDD_WL_1p4_ELTM", chip, col);
	Charge_Pumping_ELTM(Measure_file, CP_VD, CP_VB, CP_VS, CP_VDD_DIG, CP_VSS_WL, CP_VDD_WL, "ExtTrig_60_0p1sWidth_1sInterval", samp_rate, pumping_freq, Num_of_ExtTrig, chip, col, 0);
*/

/*	sprintf(Measure_file, "../Data/HCI_Chip%02d_Col%02d_1DC_1Pumping_5MHz_VSVBVD1p0_VSS_WL_0_VDD_WL_1p7_ELTM", chip, col);
	Charge_Pumping(Measure_file, CP_VD, CP_VB, CP_VS, CP_VDD_DIG, CP_VSS_WL, CP_VDD_WL, "ChargePumping_20msDelay_1Samp_20usExtTrig_1DC_1Pumping_10PLC_5MHz", samp_rate, Num_of_ExtTrig, Num_of_Sample, Trig_Delay, chip, col, 0, 2);

	sprintf(Measure_file, "../Data/HCI_Chip%02d_Col%02d_2DC_5MHz_VSVBVD1p0_VSS_WL_0_VDD_WL_1p7_ELTM", chip, col);
	Charge_Pumping(Measure_file, CP_VD, CP_VB, CP_VS, CP_VDD_DIG, CP_VSS_WL, CP_VDD_WL, "ChargePumping_20msDelay_1Samp_20usExtTrig_2DC_10PLC_5MHz", samp_rate, Num_of_ExtTrig, Num_of_Sample, Trig_Delay, chip, col, 0, 2);

	sprintf(Measure_file, "../Data/HCI_Chip%02d_Col%02d_2Pumping_5MHz_VSVBVD1p0_VSS_WL_0_VDD_WL_1p7_ELTM", chip, col);
	Charge_Pumping(Measure_file, CP_VD, CP_VB, CP_VS, CP_VDD_DIG, CP_VSS_WL, CP_VDD_WL, "ChargePumping_20msDelay_1Samp_20usExtTrig_2Pumping_10PLC_5MHz", samp_rate, Num_of_ExtTrig, Num_of_Sample, Trig_Delay, chip, col, 0, 2);
*/
/*	sprintf(Measure_file, "../Data/HCI_Chip%02d_Col%02d_1DC_1Pumping_5MHz_VSVBVD1p0_VSS_WL_0_VDD_WL_1p7_1kRsense_InAmp0RG", chip, col);
	Charge_Pumping(Measure_file, CP_VD, CP_VB, CP_VS, CP_VDD_DIG, CP_VSS_WL, CP_VDD_WL, "ChargePumping_0Delay_1Samp_10usExtTrig_1DC_1Pumping_100PLC_10MHz", samp_rate, Num_of_ExtTrig, Num_of_Sample, Trig_Delay, chip, col, 0, 1);
	*/
//	for (int repeat = 0; repeat < 36; repeat++){
		for (double VDBS = 0; VDBS <= 2.0+0.0001; VDBS += 0.1){
			sprintf(Measure_file, "../Data/HCI_Chip%02d_Col%02d_60DC_60Pumping_MultiFreq_SweepVSVBVD_VSS_WL_0_VDD_WL_1p6_ELTM", chip, col);
			Charge_Pumping_ELTM(Measure_file, VDBS, VDBS, VDBS, CP_VDD_DIG, CP_VSS_WL, CP_VDD_WL, "ExtTrig_60_0p1sWidth_1sInterval", samp_rate, Num_of_freq, pumping_freq, Num_of_ExtTrig, chip, col, 0);

			//sprintf(Measure_file, "../Data/HCI_Chip%02d_Col%02d_1DC_1Pumping_5MHz_SweepVDBS_VSS_WL_0_VDD_WL_1p7_1kRsense_InAmp0RG", chip, col);
            //Charge_Pumping(Measure_file, VDBS, VDBS, VDBS, CP_VDD_DIG, CP_VSS_WL, CP_VDD_WL, "ChargePumping_0Delay_1Samp_10usExtTrig_1DC_1Pumping_100PLC_10MHz", samp_rate, Num_of_ExtTrig, Num_of_Sample, Trig_Delay, chip, col, 0, 1);
		}
//	}
/*	for (PulseCycle = 1; PulseCycle <= 1; PulseCycle++){
		sprintf(Measure_file, "../Data/Chip%02d_Col%02d_stress_VG_ConstPulse_VAsource_VBdrain_%02d", chip, col, PulseCycle);
		stress_VG_ConstPulse(Measure_file, VDS_L36, VGS, "200ms", chip, col, 0, Num_of_Pulse);

		sprintf(Measure_file, "../Data/Stress_Chip%02d_Col%02d_Ids_Vgs_VAsource_VBdrain_%02d", chip, col, PulseCycle);
		IDS_VGS(Measure_file, col, chip, 0);
		sprintf(Measure_file, "../Data/Stress_Chip%02d_Col%02d_Ids_Vgs_VAdrain_VBsource_%02d", chip, col, PulseCycle);
		IDS_VGS(Measure_file, col, chip, 1);
	}*/


/*	double VS = 0;
	double VB = 2.4;
	double VD = 0;
	double VDD_DIG_WL = 1.7;

	sprintf(Measure_file, "../Data/Block_BD_erase_Chip%02d_Col%02d_VDD_IO_2p4_VDD_DIG_WL_1p7_VB2p4_VS0_VD0_VG0_5sec_01", chip, col);
	Block_Erase(Measure_file, VD, VB, VS, VDD_DIG_WL, "5000ms", 500, chip, col, 0, 1);

	sprintf(Measure_file, "../Data/Block_BD_Erase_Chip%02d_Col%02d_Ids_Vgs_VAsource_VBdrain_01", chip, col);
	IDS_VGS(Measure_file, col, chip, 0);
	sprintf(Measure_file, "../Data/Block_BD_Erase_Chip%02d_Col%02d_Ids_Vgs_VAdrain_VBsource_01", chip, col);
	IDS_VGS(Measure_file, col, chip, 1); */


	// Turn off PSU outputs after tests are done!
	_ibwrt(_VDD_DIG_VDD_WL, "OUTP:STAT OFF");
	_ibwrt(_VSS_WL_VSS_PW, "OUTP:STAT OFF");
	_ibwrt(_VDD_IO_V_NIDAQ, "OUTP:STAT OFF");
	_ibwrt(_VSPARE_VAB, "OUTP:STAT OFF");

	GpibEquipmentUnInit();

	return 0;
}
