/******************************
 Basic User Test Functions
*******************************/
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

ViSession defaultRM, DSA91304A_vi;  // DSA91304A session ID.
ViStatus err;						// VISA function return value.
char DSA91304A_str_result[256] = { 0 };

//********************************************************//
//            Power Supply Functions                      //
//********************************************************//
void A6611C_SetVoltage(int DeviceId, double Voltage)
{
	char PwrVal[100];

	sprintf(PwrVal, "VOLT %f", Voltage);
	_ibwrt(DeviceId, PwrVal);

}

void A6611C_AdjVoltage(int DeviceId, double StartVal, double EndVal, int numSteps)
{
	int i;
	double CurrVoltage;

	CurrVoltage = StartVal;
	A6611C_SetVoltage(DeviceId, StartVal);

	if (StartVal < EndVal) {
		for (i = 0; i < numSteps; i++) {
			CurrVoltage = CurrVoltage + 0.1;
			A6611C_SetVoltage(DeviceId, CurrVoltage);
			printf("Current 6611C voltage is %7.4f V\n", CurrVoltage);
		}
	}
	else if (StartVal > EndVal) {
		for (i = 0; i < numSteps; i++) {
			CurrVoltage = CurrVoltage - 0.1;
			A6611C_SetVoltage(DeviceId, CurrVoltage);
			printf("Current 6611C voltage is %7.4f V\n", CurrVoltage);
			}
	}
}

float A6611C_QueryVoltage(int DeviceId) {
	char VolVal[101];

	float Voltage;

	_ibwrt(DeviceId, "MEASure:VOLTage:DC?");
	::Sleep(1);
	_ibrd(DeviceId, VolVal, 100);
	VolVal[ibcntl] = '\0';
	sscanf(VolVal, "%f", &Voltage);

	return Voltage;
}

void E3646A_SetVoltage(int DeviceId, int OutputNo, double Voltage)
{
	char PwrInst[100];
	char PwrVal[100];

	sprintf(PwrInst, "INST:SEL OUT%d", OutputNo);
	sprintf(PwrVal, "VOLT %.3f", Voltage);
	//print to screen waste time/resource?
//	printf("Adjusting to %s\n", PwrVal);
	_ibwrt(DeviceId, PwrInst);
	_ibwrt(DeviceId, PwrVal);
}

void E3646A_SetVoltage_CurrentLimit(int DeviceId, int OutputNo, double Voltage, double CurrentLimit)
{
	char PwrInst[100];
	char PwrVal[100];
	char CurrentVal[100];

	sprintf(PwrInst, "INST:SEL OUT%d", OutputNo);
	sprintf(PwrVal, "VOLT %.3f", Voltage);
	printf("Adjusting to %s\n", PwrVal);
	sprintf(CurrentVal, "CURR %.3f", CurrentLimit);
	printf("Current Limited to: %s\n", CurrentVal);
	_ibwrt(DeviceId, PwrInst);
	_ibwrt(DeviceId, PwrVal);
	_ibwrt(DeviceId, CurrentVal);
}

void E3646A_AdjVoltage(int DeviceId, int OutputNo, double StartVal, double EndVal, int numSteps)
{

	float CurrVoltage;

	int i;

	CurrVoltage = StartVal;
	E3646A_SetVoltage(DeviceId, OutputNo, StartVal);

	if (StartVal < EndVal) {
		for (i = 0; i < numSteps; i++) {
			CurrVoltage = CurrVoltage + 0.1;
			E3646A_SetVoltage(DeviceId, OutputNo, CurrVoltage);
			E3646A_PrintVoltage(DeviceId, OutputNo, CurrVoltage);
		}
	}
	else if (StartVal > EndVal) {
			for (i = 0; i < numSteps; i++) {
				CurrVoltage = CurrVoltage - 0.1;
				E3646A_SetVoltage(DeviceId, OutputNo, CurrVoltage);
				E3646A_PrintVoltage(DeviceId, OutputNo, CurrVoltage);
			}
	}
}

void E3646A_PrintVoltage(int DeviceId, int OutputNo, double Voltage)
{
	char DeviceName[100];

	if (DeviceId == _VDD_DIG_VDD_WL) {
		if (OutputNo == 1) {
				sprintf(DeviceName, "VDD_DIG");
			}
			else {
				sprintf(DeviceName, "VDD_WL");
			}
	}
	if (DeviceId == _VSS_WL_VSS_PW) {
		if (OutputNo == 1) {
			sprintf(DeviceName, "VSS_WL");
		}
		else {
			sprintf(DeviceName, "VSS_PW");
		}
	}
	else if (DeviceId == _VDD_IO_V_NIDAQ) {
		if (OutputNo == 1) {
				sprintf(DeviceName, "VDD_IO");
			}
			else {
				sprintf(DeviceName, "V_NIDAQ");
			}
	}

	printf ("Current %s voltage is %7.4f V\n", DeviceName, Voltage);
}

/*
void E3646A_Raise_StackVoltage(double StartStackVol, int numStep) {

	double vstep = 0.02;
	int i;

	double vin_plus_voltage;
	for (i = 0; i <= numStep ; i++) {
		printf("i = %d, voltage is %f\n", i, StartStackVol + vstep*i);

		vin_plus_voltage = (StartStackVol * 5 + 5 * vstep*i) / 1.057;
		printf("i = %d, VIN_PLUS voltage is %f\n", i, vin_plus_voltage);

		A6611C_SetVoltage(_VIN_PLUS, vin_plus_voltage);
		E3646A_SetVoltage(_VIN_VOUT3, _VIN, (StartStackVol*4 + 4 * vstep*i));
		E3646A_SetVoltage(_TEST_3P6_2P7, _TEST3P6, (StartStackVol*4 + 4 * vstep*i));
		E3646A_SetVoltage(_VIN_VOUT3, _VOUT3, (StartStackVol*3 + 3 * vstep*i));
		E3646A_SetVoltage(_TEST_3P6_2P7, _TEST2P7, (StartStackVol*3 + 3 * vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT2, (StartStackVol*2 + 2 * vstep*i));
		E3646A_SetVoltage(_TEST_1P8_0P9, _TEST1P8, (StartStackVol*2 + 2 * vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT1, (StartStackVol + vstep*i));
		E3646A_SetVoltage(_TEST_1P8_0P9, _TEST0P9, (StartStackVol + vstep*i));
	}
}

void E3646A_Lower_StackVoltage(double StartStackVol, int numStep) {

	float vstep = 0.02;
	int i;

	double vin_plus_voltage;

	for (i = 0; i <= numStep; i++) {
		printf("i = %d, voltage is %f\n", i, StartStackVol - vstep*i);

		vin_plus_voltage = (StartStackVol * 5 - 5 * vstep*i) / 1.057;
		printf("i = %d, VIN_PLUS voltage is %f\n", i, vin_plus_voltage);

		A6611C_SetVoltage(_VIN_PLUS, vin_plus_voltage);
		E3646A_SetVoltage(_VIN_VOUT3, _VIN, (StartStackVol * 4 - 4 * vstep*i));
		E3646A_SetVoltage(_TEST_3P6_2P7, _TEST3P6, (StartStackVol * 4 - 4 * vstep*i));
		E3646A_SetVoltage(_VIN_VOUT3, _VOUT3, (StartStackVol * 3 - 3 * vstep*i));
		E3646A_SetVoltage(_TEST_3P6_2P7, _TEST2P7, (StartStackVol * 3 - 3 * vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT2, (StartStackVol * 2 - 2 * vstep*i));
		E3646A_SetVoltage(_TEST_1P8_0P9, _TEST1P8, (StartStackVol * 2 - 2 * vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT1, (StartStackVol - vstep*i));
		E3646A_SetVoltage(_TEST_1P8_0P9, _TEST0P9, (StartStackVol - vstep*i));
	}
}

void E3646A_Raise_InternalVoltage(double StartStackVol, int numStep) {

	double vstep = 0.02;
	int i;

	double vin_plus_voltage;
	for (i = 0; i <= numStep; i++) {
		printf("i = %d, voltage is %f\n", i, StartStackVol + vstep*i);

		E3646A_SetVoltage(_VIN_VOUT3, _VIN, (StartStackVol * 4 + 4 * vstep*i));
		E3646A_SetVoltage(_VIN_VOUT3, _VOUT3, (StartStackVol * 3 + 3 * vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT2, (StartStackVol * 2 + 2 * vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT1, (StartStackVol + vstep*i));
	}
}

void E3646A_Lower_InternalVoltage(double StartStackVol, int numStep) {

	float vstep = 0.02;
	int i;

	double vin_plus_voltage;

	for (i = 0; i <= numStep; i++) {
		printf("i = %d, voltage is %f\n", i, StartStackVol - vstep*i);

		E3646A_SetVoltage(_VIN_VOUT3, _VIN, (StartStackVol * 4 - 4 * vstep*i));
		E3646A_SetVoltage(_VIN_VOUT3, _VOUT3, (StartStackVol * 3 - 3 * vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT2, (StartStackVol * 2 - 2 * vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT1, (StartStackVol - vstep*i));
	}
}

void E3646A_Raise_VinVout1Voltage(double StartStackVol, int numStep) {

	double vstep = 0.02;
	int i;

	double vin_plus_voltage;
	for (i = 0; i <= numStep; i++) {
		printf("i = %d, voltage is %f\n", i, StartStackVol + vstep*i);
		E3646A_SetVoltage(_VIN_VOUT3, _VIN, (StartStackVol * 4 + 4 * vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT1, (StartStackVol + vstep*i));
	}
}

void E3646A_Lower_VinVout1Voltage(double StartStackVol, int numStep) {

	float vstep = 0.02;
	int i;

	double vin_plus_voltage;

	for (i = 0; i <= numStep; i++) {
		printf("i = %d, voltage is %f\n", i, StartStackVol - vstep*i);
		E3646A_SetVoltage(_VIN_VOUT3, _VIN, (StartStackVol * 4 - 4 * vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT1, (StartStackVol - vstep*i));
	}
}

void E3646A_Raise_VinVoltage(double StartStackVol, int numStep) {

	double vstep = 0.02;
	int i;

	double vin_plus_voltage;
	for (i = 0; i <= numStep; i++) {
		printf("i = %d, voltage is %f\n", i, StartStackVol + vstep*i);
		E3646A_SetVoltage(_VIN_VOUT3, _VIN, (StartStackVol * 4 + 4 * vstep*i));
	}
}

void E3646A_Lower_VinVoltage(double StartStackVol, int numStep) {

	float vstep = 0.02;
	int i;

	double vin_plus_voltage;

	for (i = 0; i <= numStep; i++) {
		printf("i = %d, voltage is %f\n", i, StartStackVol - vstep*i);
		E3646A_SetVoltage(_VIN_VOUT3, _VIN, (StartStackVol * 4 - 4 * vstep*i));
	}
}

void E3646A_Raise_AllSameVoltage(double StartStackVol, int numStep) {

	double vstep = 0.02;
	int i;

	double vin_plus_voltage;
	for (i = 0; i <= numStep; i++) {
		printf("i = %d, voltage is %f\n", i, StartStackVol + vstep*i);

		vin_plus_voltage = (StartStackVol + vstep*i) / 1.057;
		printf("i = %d, VIN_PLUS voltage is %f\n", i, vin_plus_voltage);

		A6611C_SetVoltage(_VIN_PLUS, vin_plus_voltage);
		E3646A_SetVoltage(_VIN_VOUT3, _VIN, (StartStackVol + vstep*i));
		E3646A_SetVoltage(_TEST_3P6_2P7, _TEST3P6, (StartStackVol + vstep*i));
		E3646A_SetVoltage(_VIN_VOUT3, _VOUT3, (StartStackVol + vstep*i));
		E3646A_SetVoltage(_TEST_3P6_2P7, _TEST2P7, (StartStackVol + vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT2, (StartStackVol + vstep*i));
		E3646A_SetVoltage(_TEST_1P8_0P9, _TEST1P8, (StartStackVol + vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT1, (StartStackVol + vstep*i));
		E3646A_SetVoltage(_TEST_1P8_0P9, _TEST0P9, (StartStackVol + vstep*i));
	}
}

void E3646A_Lower_AllSameVoltage(double StartStackVol, int numStep) {

	float vstep = 0.02;
	int i;

	double vin_plus_voltage;

	for (i = 0; i <= numStep; i++) {
		printf("i = %d, voltage is %f\n", i, StartStackVol - vstep*i);

		vin_plus_voltage = (StartStackVol - vstep*i) / 1.057;
		printf("i = %d, VIN_PLUS voltage is %f\n", i, vin_plus_voltage);

		A6611C_SetVoltage(_VIN_PLUS, vin_plus_voltage);
		E3646A_SetVoltage(_VIN_VOUT3, _VIN, (StartStackVol - vstep*i));
		E3646A_SetVoltage(_TEST_3P6_2P7, _TEST3P6, (StartStackVol - vstep*i));
		E3646A_SetVoltage(_VIN_VOUT3, _VOUT3, (StartStackVol - vstep*i));
		E3646A_SetVoltage(_TEST_3P6_2P7, _TEST2P7, (StartStackVol - vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT2, (StartStackVol - vstep*i));
		E3646A_SetVoltage(_TEST_1P8_0P9, _TEST1P8, (StartStackVol - vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT1, (StartStackVol - vstep*i));
		E3646A_SetVoltage(_TEST_1P8_0P9, _TEST0P9, (StartStackVol - vstep*i));
	}
}
*/

float E3646A_MeasAvgCurrent(int DeviceId, int OutputNo) {
	char CurVal[101];
	char PwrInst[100];

	float DCCurrentSum=0;
	float Current;
	int vind;
	sprintf(PwrInst, "INST:SEL OUT%d", OutputNo);
	
	DCCurrentSum=0;
	for (vind=0; vind<=19; vind++) {
		_ibwrt(DeviceId, PwrInst);
//		_ibwrt(DeviceId, "MEASure:CURRent:DC?");
		_ibwrt(DeviceId, "MEASure:CURRent?");
		::Sleep(1); //windows function: tread wait for 1 millisecond, integration time
		_ibrd(DeviceId, CurVal, 100);
		CurVal[ibcntl]='\0';
		sscanf(CurVal, "%f", &Current);
		printf("Measured Current = %f\n", Current);
		DCCurrentSum=DCCurrentSum + Current * 1000;
}
		
	Current = DCCurrentSum / 20;

	return Current;
}

float E3646A_MeasAvgVoltage(int DeviceId, int OutputNo) {
	char VolVal[101];
	char PwrInst[100];

	float DCVoltageSum = 0;
	float Voltage;
	int vind;
	sprintf(PwrInst, "INST:SEL OUT%d", OutputNo);

	DCVoltageSum = 0;
	for (vind = 0; vind <= 19; vind++) {
		_ibwrt(DeviceId, PwrInst);
		_ibwrt(DeviceId, "MEASure:VOLTage:DC?");
		::Sleep(1);
		_ibrd(DeviceId, VolVal, 100);
		VolVal[ibcntl] = '\0';
		sscanf(VolVal, "%f", &Voltage);
		DCVoltageSum = DCVoltageSum + Voltage;
	}

	Voltage = DCVoltageSum / 20;

	return Voltage;
}

float E3646A_QueryVoltage(int DeviceId, int OutputNo) {
	char VolVal[101];
	char PwrInst[100];

	float Voltage;
	int vind;
	sprintf(PwrInst, "INST:SEL OUT%d", OutputNo);

	_ibwrt(DeviceId, PwrInst);
	_ibwrt(DeviceId, "MEASure:VOLTage:DC?");
	::Sleep(1);
	_ibrd(DeviceId, VolVal, 100);
	VolVal[ibcntl] = '\0';
	sscanf(VolVal, "%f", &Voltage);

	return Voltage;
}

/*
void E3646A_Report_AllVoltage() {
	printf("VIN_PLUS is %f\n", A6611C_QueryVoltage(_VIN_PLUS));
	printf("VIN is %f\n", E3646A_QueryVoltage(_VIN_VOUT3, _VIN));
	printf("VTEST3P6 is %f\n", E3646A_QueryVoltage(_TEST_3P6_2P7, _TEST3P6));
	printf("VOUT3 is %f\n", E3646A_QueryVoltage(_VIN_VOUT3, _VOUT3));
	printf("VTEST2P7 is %f\n", E3646A_QueryVoltage(_TEST_3P6_2P7, _TEST2P7));
	printf("VOUT2 is %f\n", E3646A_QueryVoltage(_VOUT2_VOUT1, _VOUT2));
	printf("VTEST1P8 is %f\n", E3646A_QueryVoltage(_TEST_1P8_0P9, _TEST1P8));
	printf("VOUT1 is %f\n", E3646A_QueryVoltage(_VOUT2_VOUT1, _VOUT1));
	printf("VTEST0P9 is %f\n", E3646A_QueryVoltage(_TEST_1P8_0P9, _TEST0P9));
}
*/

//**************************************************************//
//                  Pulse Generator Functions                   //
//**************************************************************//
/*
void DTG5334_SetClkMode() {
	_ibwrt(_DTG5334, "TBAS:OMODe PULse");
	_ibwrt(_DTG5334, "TBAS:MODE CONTinuous");
	_ibwrt(_DTG5334, "OUTPut:STATe:ALL ON");
}

void DTG5334_SetDataMode() {
	_ibwrt(_DTG5334, "TBAS:OMODe DATA");
	_ibwrt(_DTG5334, "TBAS:MODE CONTinuous");
	_ibwrt(_DTG5334, "OUTPut:STATe:ALL ON");
}

void DTG5334_TurnOn_Channel(int ChannelNum) {
	char Cmd[100];
	sprintf(Cmd, "PGENA:CH%d:OUTPut ON", ChannelNum);
	_ibwrt(_DTG5334, Cmd);
}

void DTG5334_TurnOff_Channel(int ChannelNum) {
	char Cmd[100];
	sprintf(Cmd, "PGENA:CH%d:OUTPut OFF", ChannelNum);
	_ibwrt(_DTG5334, Cmd);
}

void DTG5334_SetChannelLow(int ChannelNum, float Low) 
{
	char Cmd[100];
	sprintf(Cmd, "PGENA:CH%d:LOW %f", ChannelNum, Low);
	_ibwrt(_DTG5334, Cmd);
}

void DTG5334_SetChannelHigh(int ChannelNum, float High)
{
	char Cmd[100];
	sprintf(Cmd, "PGENA:CH%d:HIGH %f", ChannelNum, High);
	_ibwrt(_DTG5334, Cmd);
}

void DTG5334_ClockOn()
{
	_ibwrt(_DTG5334, "TBAS:RUN ON");
}

void DTG5334_StopClock() {
	_ibwrt(_DTG5334, "TBAS:RUN OFF");
}

void DTG5334_SetClkFreq(float ClockFreq) {
	char FreqCmd[100];

	sprintf(FreqCmd, "TBAS:FREQuency %f", ClockFreq);
	_ibwrt(_DTG5334, FreqCmd);
}

void DTG5334_IVRClkEn(float ClockFreq) {

	DTG5334_TurnOn_Channel(1);
	DTG5334_TurnOff_Channel(2);
	DTG5334_SetClkFreq(ClockFreq);
}

void DTG5334_SystemClkEn(float ClockFreq) {

	DTG5334_TurnOn_Channel(2);
	DTG5334_TurnOff_Channel(1);
	DTG5334_SetClkFreq(ClockFreq);
}
*/
//**************************************************************//
//       DSA91304A oscilloscope Functions (VISA Interface)      //
//**************************************************************//
/*
void DSA91304A_error_handler()
{
	char err_msg[1024] = { 0 };

	viStatusDesc(DSA91304A_vi, err, err_msg);
	printf("VISA Error: %s\n", err_msg);
	if (err < VI_SUCCESS) {
		exit(1);
	}
}

void DSA91304A_open_session()
{
	// Open the default resource manager session
	err = viOpenDefaultRM(&defaultRM);
	if (err != VI_SUCCESS) {
		DSA91304A_error_handler();
	}

	// Open the session using the oscilloscope's VISA address.
	err = viOpen(defaultRM, DSA91304A_VISA_ADDRESS, VI_NULL, VI_NULL, &DSA91304A_vi);
	if (err != VI_SUCCESS) {
		DSA91304A_error_handler();
	}

	// Set the I/O timeout to fifteen seconds.
	err = viSetAttribute(DSA91304A_vi, VI_ATTR_TMO_VALUE, 15000);
	if (err != VI_SUCCESS) {
		DSA91304A_error_handler();
	}

	// Clear the interface.
	err = viClear(DSA91304A_vi);
	if (err != VI_SUCCESS) {
		DSA91304A_error_handler();
	}

	DSA91304A_do_query_string("*IDN?");
	printf("Oscilloscope *IDN? string: %s\n", DSA91304A_str_result);
	printf("Oscilloscope DSA91304A Session Open\n");

}

void DSA91304A_close_session()
{
	// Close the vi session and the resource manager session.
	viClose(DSA91304A_vi);
	viClose(defaultRM);
	printf("Oscilloscope DSA91304A Session Closed\n");

}

void DSA91304A_doCommand(char *command)
{
	char message[400];

	strcpy(message, command);
	strcat(message, "\n");
	err = viPrintf(DSA91304A_vi, message);

	if (err != VI_SUCCESS) {
		DSA91304A_error_handler();
	}
}

void DSA91304A_do_query_string(char *query)
{
	char message[400];

	strcpy(message, query);
	strcat(message, "\n");
	
	err = viPrintf(DSA91304A_vi, message);
	if (err != VI_SUCCESS) {
		DSA91304A_error_handler();
	}

	err = viScanf(DSA91304A_vi, "%t", DSA91304A_str_result);
	if (err != VI_SUCCESS) {
		DSA91304A_error_handler();
	}

}

void DSA91304A_do_query_number(char *query, double num_result)
{
	char message[400];

	strcpy(message, query);
	strcat(message, "\n");

	err = viPrintf(DSA91304A_vi, message);
	if (err != VI_SUCCESS) {
		DSA91304A_error_handler();
	}

	err = viScanf(DSA91304A_vi, "%lf", &num_result);
	if (err != VI_SUCCESS) {
		DSA91304A_error_handler();
	}
}

void DSA91304A_read_number(double num_result)
{
	err = viScanf(DSA91304A_vi, "%lf", &num_result);
	if (err != VI_SUCCESS) {
		DSA91304A_error_handler();
	}
}

void DSA91304A_ScopeHeadOff()
{
	DSA91304A_doCommand(":SYSTem:HEADer OFF");
}

void DSA91304A_ScopeRun()
{
	DSA91304A_doCommand(":RUN");    
}

void DSA91304A_ScopeStop()
{
	DSA91304A_doCommand(":STOP");
}

void DSA91304A_ScopeSingle()
{
	DSA91304A_doCommand(":SINGle");
}

void DSA91304A_ClearDisplay()
{
	DSA91304A_doCommand(":CDISplay");
}

void DSA91304A_SetTrigAuto()
{
	DSA91304A_doCommand(":TRIGger:SWEep AUTO");
}

void DSA91304A_SetTrigTrig()
{
	DSA91304A_doCommand(":TRIGger:SWEep TRIGgered");
}

void DSA91304A_SetTrigMode(int channelNum, double trigLevel)
{
	char Cmd[400];

	DSA91304A_doCommand("TRIGger:SWEep TRIGgered");
	DSA91304A_doCommand("TRIGger:MODE:EDGE");
	sprintf(Cmd, "TRIGger:EDGE:SOURce CHANnel%d", channelNum);
	DSA91304A_doCommand(Cmd);
	DSA91304A_doCommand("TRIGger:EDGE:SLOPe POSitive");
	sprintf(Cmd, "TRIGger:LEVel CHANnel%d, %f", channelNum, trigLevel);
	DSA91304A_doCommand(Cmd);
	DSA91304A_doCommand("TRIGger:HYSTeresis HSENsitivity");
}

void DSA91304A_SetSingleCapture(int channelNum, double trigLevel)
{
	DSA91304A_SetTrigMode(channelNum, trigLevel);
	DSA91304A_SetTrigTrig();
	DSA91304A_ClearDisplay();
	DSA91304A_ScopeSingle();
}

void DSA91304A_SetAutoRun()
{
	DSA91304A_SetTrigAuto();
	DSA91304A_ClearDisplay();
	DSA91304A_ScopeRun();
}

void DSA91304A_SetTimeBase_Range(double time_range, double time_delay) // Range : Entire Display Range
{
	char timeBaseCmd[400];

	sprintf(timeBaseCmd, ":TIMebase:REFerence CENTer");
	DSA91304A_doCommand(timeBaseCmd);

	sprintf(timeBaseCmd, ":TIMebase:RANGe %7.12f", time_range);
	DSA91304A_doCommand(timeBaseCmd);

	sprintf(timeBaseCmd, ":TIMebase:POSition %7.12f", time_delay);
	DSA91304A_doCommand(timeBaseCmd);
}

void DSA91304A_SetTimeBase_Scale(double time_scale, double time_delay) // Scale : xx/div
{
	char timeBaseCmd[400];

	sprintf(timeBaseCmd, ":TIMebase:REFerence CENTer");
	DSA91304A_doCommand(timeBaseCmd);

	sprintf(timeBaseCmd, ":TIMebase:SCALe %7.12f", time_scale);
	DSA91304A_doCommand(timeBaseCmd);

	sprintf(timeBaseCmd, ":TIMebase:POSition %7.12f", time_delay);
	DSA91304A_doCommand(timeBaseCmd);
}

void DSA91304A_SetChannel(int channel_num, float volt_scale, float volt_offset)
{
	char ChannelCmd[400];

	sprintf(ChannelCmd, ":CHANnel%d:UNITS VOLT");
	DSA91304A_doCommand(ChannelCmd);
	
	sprintf(ChannelCmd, ":CHANnel%d:OFFSet %7.8f", channel_num, volt_offset);
	DSA91304A_doCommand(ChannelCmd);
	
	sprintf(ChannelCmd, ":CHANnel%d:SCALe %7.8f", channel_num, volt_scale);
	DSA91304A_doCommand(ChannelCmd);
}

double DSA91304A_Meas_VAVG(int channel_num)
{
	char AvgCmd[400];

	double Vaverage = 0;

	// Measure Average
	sprintf(AvgCmd, ":MEASure:VAVerage DISPlay, CHANnel%d", channel_num);
	DSA91304A_doCommand(AvgCmd);

	::Sleep(10);

	sprintf(AvgCmd, ":MEASure:VAVerage? DISPlay, CHANnel%d", channel_num);
	DSA91304A_do_query_string(AvgCmd);
	sscanf(DSA91304A_str_result, "%lf", &Vaverage);
	
	return Vaverage;
}

double DSA91304A_Meas_Freq(int channel_num)
{
	char FreqCmd[400];

	double FreqVal = 0;

	// Measure Frequency
	sprintf(FreqCmd, ":MEASure:FREQuency CHANnel%d, RISing", channel_num);
	DSA91304A_doCommand(FreqCmd);

	::Sleep(10);

	sprintf(FreqCmd, ":MEASure:FREQuency? CHANnel%d, RISing", channel_num);
	DSA91304A_do_query_string(FreqCmd);
	sscanf(DSA91304A_str_result, "%lf", &FreqVal);

	return FreqVal;
}

double DSA91304A_Meas_VMax(int channel_num)
{
	char AvgCmd[400];

	double Vmax = 0;

	// Measure Average
	sprintf(AvgCmd, ":MEASure:VMAX CHANnel%d", channel_num);
	DSA91304A_doCommand(AvgCmd);

	::Sleep(10);

	sprintf(AvgCmd, ":MEASure:VMAX? CHANnel%d", channel_num);
	DSA91304A_do_query_string(AvgCmd);
	sscanf(DSA91304A_str_result, "%lf", &Vmax);

	return Vmax;
}


void DSA91304A_Set_ACQ_AUTO_Modes() {

	DSA91304A_doCommand(":ACQuire:AVERage OFF");
	DSA91304A_doCommand(":ACQuire:MODE HRESolution");
	DSA91304A_doCommand(":ACQuire:BANDwidth AUTO");
	DSA91304A_doCommand(":ACQuire:INTerpolate ON");
	DSA91304A_doCommand(":ACQuire:HRESolution AUTO");
	DSA91304A_doCommand(":ACQuire:SRATe 5E9");
	DSA91304A_doCommand(":ACQuire:POINts:AUTO ON");
}

void DSA91304A_Set_ClkFreq_ACQ_AUTO_Modes() {

	DSA91304A_doCommand(":ACQuire:AVERage OFF");
	DSA91304A_doCommand(":ACQuire:MODE RTIMe");
	DSA91304A_doCommand(":ACQuire:BANDwidth AUTO");
	DSA91304A_doCommand(":ACQuire:INTerpolate ON");
	DSA91304A_doCommand(":ACQuire:HRESolution AUTO");
	DSA91304A_doCommand(":ACQuire:SRATe 40E9");
	DSA91304A_doCommand(":ACQuire:POINts:AUTO ON");
}

void DSA91304A_SaveWaveToDisk(char *FileName, int wait) {
	char Cmd[400];

	sprintf(Cmd, ":DISK:SAVE:WAVeform ALL, \"%s\", CSV", FileName);
	DSA91304A_doCommand(Cmd);

	::Sleep(wait);

	printf("Saving Waveform to Disk Done....\n");
}

void DSA91304A_SaveHistToDisk(char *FileName) {
	char Cmd[400];
	int Done;
//	sprintf(Cmd, ":DISK:SAVE:WAVeform HISTogram, \"%s\", CSV; *OPC", FileName);
	sprintf(Cmd, ":DISK:SAVE:WAVeform HISTogram, \"%s\", CSV", FileName);
	DSA91304A_doCommand(Cmd);

//	DSA91304A_do_query_string("*OPC?");
//	sscanf(DSA91304A_str_result, "%d", &Done);

//	if (Done == 1) {
	printf("Saving Histogram to Disk Done....\n");
//	}
}

void DSA91304A_SetHistMode(char *mode)
{
	char Cmd[400];
	sprintf(Cmd, ":HISTogram:MODE %s", mode);
	DSA91304A_doCommand(Cmd);
}

void DSA91304A_SetJitterMeasNum(int measNum)
{
	char Cmd[400];
	sprintf(Cmd, ":MEASure:JITTer:MEASurement MEASurement%d", measNum);
	DSA91304A_doCommand(Cmd);
}

void DSA91304A_MeasJitterEnable()
{
	char Cmd[400];
	sprintf(Cmd, ":MEASure:JITTer:HISTogram ON");
	DSA91304A_doCommand(Cmd);
}

void DSA91304A_MeasJitterDisable()
{
	char Cmd[400];
	sprintf(Cmd, ":MEASure:JITTer:HISTogram OFF");
	DSA91304A_doCommand(Cmd);
}

void DSA91304A_MeasJitterStats(double *stats)
{

	double Hits;
	double Mean;
	double Median;
	double Mode;
	double stddev;
	double m1s;
	double m2s;
	double m3s;
	double max;
	double min;
	double peak;
	double pp;

	DSA91304A_ScopeHeadOff();

	// Measure Hits
	DSA91304A_do_query_string(":MEASure:HISTogram:HITS? HISTogram");
	sscanf(DSA91304A_str_result, "%lf", &Hits);
	::Sleep(100);

	// Measure Mean
	DSA91304A_do_query_string(":MEASure:HISTogram:MEAN? HISTogram");
	sscanf(DSA91304A_str_result, "%lf", &Mean);
	::Sleep(100);

	// Measure Median
	DSA91304A_do_query_string(":MEASure:HISTogram:MEDian? HISTogram");
	sscanf(DSA91304A_str_result, "%lf", &Median);
	::Sleep(100);

	// Measure Mode
	DSA91304A_do_query_string(":MEASure:HISTogram:MODE? HISTogram");
	sscanf(DSA91304A_str_result, "%lf", &Mode);
	::Sleep(100);

	// Measure StdDev
	DSA91304A_do_query_string(":MEASure:HISTogram:STDDev? HISTogram");
	sscanf(DSA91304A_str_result, "%lf", &stddev);
	::Sleep(100);

	// Measure M1S
	DSA91304A_do_query_string(":MEASure:HISTogram:M1S? HISTogram");
	sscanf(DSA91304A_str_result, "%lf", &m1s);
	::Sleep(100);

	// Measure M2S
	DSA91304A_do_query_string(":MEASure:HISTogram:M2S? HISTogram");
	sscanf(DSA91304A_str_result, "%lf", &m2s);
	::Sleep(100);

	// Measure M3S
	DSA91304A_do_query_string(":MEASure:HISTogram:M3S? HISTogram");
	sscanf(DSA91304A_str_result, "%lf", &m3s);
	::Sleep(100);

	// Measure MAX
	DSA91304A_do_query_string(":MEASure:HISTogram:MAX? HISTogram");
	sscanf(DSA91304A_str_result, "%lf", &max);
	::Sleep(100);

	// Measure MIN
	DSA91304A_do_query_string(":MEASure:HISTogram:MIN? HISTogram");
	sscanf(DSA91304A_str_result, "%lf", &min);
	::Sleep(100);

	// Measure PEAK
	DSA91304A_do_query_string(":MEASure:HISTogram:PEAK? HISTogram");
	sscanf(DSA91304A_str_result, "%lf", &peak);
	::Sleep(100);

	// Measure PP
	DSA91304A_do_query_string(":MEASure:HISTogram:PP? HISTogram");
	sscanf(DSA91304A_str_result, "%lf", &pp);
	::Sleep(100);

	// Pack stats register
	stats[0] = Hits;
	stats[1] = Mean;
	stats[2] = Median;
	stats[3] = Mode;
	stats[4] = stddev;
	stats[5] = m1s;
	stats[6] = m2s;
	stats[7] = m3s;
	stats[8] = max;
	stats[9] = min;
	stats[10] = pp;
	stats[11] = peak;
}

void DSA91304A_TurnONAllChannel() {
	DSA91304A_doCommand(":CHANnel1:DISPlay ON");
	DSA91304A_doCommand(":CHANnel2:DISPlay ON");
	DSA91304A_doCommand(":CHANnel3:DISPlay ON");
	DSA91304A_doCommand(":CHANnel4:DISPlay ON");
}

void DSA91304A_TurnOFFAllChannel() {
	DSA91304A_doCommand(":CHANnel1:DISPlay OFF");
	DSA91304A_doCommand(":CHANnel2:DISPlay OFF");
	DSA91304A_doCommand(":CHANnel3:DISPlay OFF");
	DSA91304A_doCommand(":CHANnel4:DISPlay OFF");
}

void DSA91304A_TurnONOnlyChannel(int channelNum) {
	char Cmd[400];
	DSA91304A_doCommand(":CHANnel1:DISPlay OFF");
	DSA91304A_doCommand(":CHANnel2:DISPlay OFF");
	DSA91304A_doCommand(":CHANnel3:DISPlay OFF");
	DSA91304A_doCommand(":CHANnel4:DISPlay OFF");
	sprintf(Cmd, ":CHANnel%d:DISPlay ON", channelNum);
	DSA91304A_doCommand(Cmd);
}

void DSA91304A_SetMeasFreqAllChannel() {
	DSA91304A_doCommand(":MEASure:FREQuency CHANnel4, RISing");
	DSA91304A_doCommand(":MEASure:FREQuency CHANnel3, RISing");
	DSA91304A_doCommand(":MEASure:FREQuency CHANnel2, RISing");
	DSA91304A_doCommand(":MEASure:FREQuency CHANnel1, RISing");
}
*/
//**************************************************************//
//               MSO6104A oscilloscope Functions                //
//**************************************************************//
/*
void MSO6104A_ScopeHeadOff()
{
	_ibwrt(_MSO6104A, ":SYSTem:HEADer OFF");
}

void MSO6104A_ScopeRun()
{
	_ibwrt(_MSO6104A, ":RUN");
}

void MSO6104A_ScopeStop()
{
	_ibwrt(_MSO6104A, ":STOP");
}

void MSO6104A_ClearDisplay()
{
	_ibwrt(_MSO6104A, ":CDISplay");
}

void MSO6104A_SetTrigAuto()
{
	_ibwrt(_MSO6104A, ":TRIGger:SWEep AUTO");
}

void MSO6104A_SetTrigTrig()
{
	_ibwrt(_MSO6104A, ":TRIGger:SWEep NORMal");
}

void MSO6104A_SetAutoRun()
{
	MSO6104A_SetTrigAuto();
	MSO6104A_ClearDisplay();
	MSO6104A_ScopeRun();
}

void MSO6104A_SetTrigMode(int channelNum, double trigLevel)
{
	char Cmd[200];

	_ibwrt(_MSO6104A, "TRIGger:SWEep NORMal");
	_ibwrt(_MSO6104A, "TRIGger:MODE:EDGE");
	sprintf(Cmd, "TRIGger:EDGE:SOURce CHANnel%d", channelNum);
	_ibwrt(_MSO6104A, Cmd);
	_ibwrt(_MSO6104A, "TRIGger:EDGE:SLOPe POSitive");
	sprintf(Cmd, "TRIGger:EDGE:LEVel %f", trigLevel);
	_ibwrt(_MSO6104A, Cmd);
}

void MSO6104A_ScopeSingle()
{
	_ibwrt(_MSO6104A, ":SINGle");
}

void MSO6104A_SetSingleCapture(int channelNum, double trigLevel)
{
	MSO6104A_SetTrigMode(channelNum, trigLevel);
	MSO6104A_SetTrigTrig();
	MSO6104A_ClearDisplay();
	MSO6104A_ScopeSingle();
}

void MSO6104A_SetTimeBase_Range(double time_range, double time_delay) // Range : Entire Display Range
{
	char timeBaseCmd[100];

	sprintf(timeBaseCmd, ":TIMebase:REFerence CENTer");
	_ibwrt(_MSO6104A, timeBaseCmd);

	sprintf(timeBaseCmd, ":TIMebase:RANGe %7.12f", time_range);
	_ibwrt(_MSO6104A, timeBaseCmd);

	sprintf(timeBaseCmd, ":TIMebase:POSition %7.12f", time_delay);
	_ibwrt(_MSO6104A, timeBaseCmd);
}

void MSO6104A_SetTimeBase_Scale(double time_scale, double time_delay) // Scale : xx/div
{
	char timeBaseCmd[100];

	sprintf(timeBaseCmd, ":TIMebase:REFerence CENTer");
	_ibwrt(_MSO6104A, timeBaseCmd);

	sprintf(timeBaseCmd, ":TIMebase:SCALe %7.12f", time_scale);
	_ibwrt(_MSO6104A, timeBaseCmd);

	sprintf(timeBaseCmd, ":TIMebase:POSition %7.12f", time_delay);
	_ibwrt(_MSO6104A, timeBaseCmd);
}

void MSO6104A_SetChannel(int channel_num, float volt_scale, float volt_offset)
{
	char ChannelCmd[100];

	sprintf(ChannelCmd, ":CHANnel%d:UNITS VOLT");
	_ibwrt(_MSO6104A, ChannelCmd);

	sprintf(ChannelCmd, ":CHANnel%d:OFFSet %7.8f", channel_num, volt_offset);
	_ibwrt(_MSO6104A, ChannelCmd);

	sprintf(ChannelCmd, ":CHANnel%d:SCALe %7.8f", channel_num, volt_scale);
	_ibwrt(_MSO6104A, ChannelCmd);
}

float MSO6104A_Meas_VAVG(int channel_num)
{
	char RdBuffer[101];
	char AvgCmd[100];
	float Vaverage;

	// Measure Average
	sprintf(AvgCmd, ":MEASure:VAVerage DISPlay, CHANnel%d", channel_num);
	_ibwrt(_MSO6104A, AvgCmd);

	::Sleep(10);

	sprintf(AvgCmd, ":MEASure:VAVerage? DISPlay, CHANnel%d", channel_num);
	_ibwrt(_MSO6104A, AvgCmd);

	_ibrd(_MSO6104A, RdBuffer, 100);
	RdBuffer[ibcntl] = '\0';         // Null terminate the ASCII string    

	sscanf(RdBuffer, "%f", &Vaverage);

	return Vaverage;
}
*/
//**************************************************************//
//                     Test Functions                           //
//**************************************************************//
/*
void MemoryLoad(char *mem_scan_fn)
{
//	FILE *err_file;
	char scan_memload_fn[300];
	char memloaderr_filename[300];
	int i;
	int *internalRegs = new int[NUM_SCAN_BITS];
	printf("Starting Loading Memory\n");

//	sprintf(memloaderr_filename, "%s_err.txt", mem_scan_fn);
//	err_file = fopen(memloaderr_filename, "w");
	for (i = 0; i < CHIP_MEM_LINES; i++) {
		sprintf(scan_memload_fn, "%s%03x.txt", mem_scan_fn, i);
		printf("Loading Memory Line %03x\n", i);
		scan(scan_memload_fn, 0);
		DTG5334_ClockOn();
		DTG5334_StopClock();
		if (scanChainRead_Error == 1) {
			break;
		}
	}
//	fclose(err_file);
	printf("Finished Loading Memory\n", i);
}

void MemoryLoadScanFileChk(char *mem_scan_fn, char *err_filename)
{
	FILE *err_fn;
	char scan_memload_fn[300];
	int i;
	int *internalRegs = new int[NUM_SCAN_BITS];
	printf("Starting Checking Memory\n");

	err_fn = fopen(err_filename, "a");
	for (i = 0; i < CHIP_MEM_LINES; i++) {
		sprintf(scan_memload_fn, "%s%03x.txt", mem_scan_fn, i);
		printf("Checking Memory Line %03x\n", i);
		scanFileChk(scan_memload_fn);
		if (scanChainRead_Error == 1) {
			fprintf(err_fn, "%s\n", scan_memload_fn);
			break;
		}
	}
	fclose(err_fn);
	printf("Finished Checking Memory\n", i);
}

void MemoryRead(char *mem_scan_read_dir, int *CENYDump, int *GWENYDump, int *WENYDump, int *AYDump, int *DYDump, int *MemoryDump)
{
	char scan_memread_fn[300];
	int *internalRegs = new int[NUM_SCAN_BITS];
	int i, j, k;
	int QStartIndex = 766;
	int QEndIndex = 735;

	printf("Starting Memory Read\n");
	for (i = 0; i < CHIP_MEM_LINES; i++) {
		sprintf(scan_memread_fn, "%sscanin_mem_read_line%03x.txt", mem_scan_read_dir, i);
		scan(scan_memread_fn, 0);
		DTG5334_ClockOn();
//		::Sleep(5);
		DTG5334_StopClock();
		scanInternalRead("E:/test_data/scan_files/scan_read/scanin_read_in.txt", internalRegs);
		printf("Read A=%03x D=%08x Q=%08x\n", ConvertBinArraytoInt(internalRegs, 808, 799), ConvertBinArraytoInt(internalRegs, 798, 767), ConvertBinArraytoInt(internalRegs, 766, 735));
//		printScanInternal(internalRegs);
		// Grab Memory Values from Scan Output 
		// 12 13 14 15  // Core Organization
		// 8  9  10 11
		// 4  5  6  7
		// 0  1  2  3
		QStartIndex = 766;
		QEndIndex = 735;

	
		for (j = 0; j < 16; j++) {
//			printf("Mem Q StartIndex = %d, EndIndex = %d\n", QStartIndex, QEndIndex);
			MemoryDump[j*CHIP_MEM_LINES + i] = ConvertBinArraytoInt(internalRegs, QStartIndex, QEndIndex);
			DYDump[j*CHIP_MEM_LINES + i] = ConvertBinArraytoInt(internalRegs, QStartIndex + 32, QEndIndex + 32);
			AYDump[j*CHIP_MEM_LINES + i] = ConvertBinArraytoInt(internalRegs, QStartIndex + 42, QEndIndex + 64);
			WENYDump[j*CHIP_MEM_LINES + i] = ConvertBinArraytoInt(internalRegs, QStartIndex + 46, QEndIndex + 74);
			GWENYDump[j*CHIP_MEM_LINES + i] = ConvertBinArraytoInt(internalRegs, QStartIndex + 47, QEndIndex + 78);
			CENYDump[j*CHIP_MEM_LINES + i] = ConvertBinArraytoInt(internalRegs, QStartIndex + 48, QEndIndex + 79);

			if ((j % 4) == 3) {
				QStartIndex = QStartIndex + 383 + 735;
				QEndIndex = QEndIndex + 383 + 735;
			}
			else {
				QStartIndex = QStartIndex + 383;
				QEndIndex = QEndIndex + 383;
			}
		}

//		if (i == 2) {
//			break;
//		}
	}
	printf("Finished Reading Memory\n", i);
}

void ReadInternalRegs(int *internalRegs){

	scanInternalRead("E:/test_data/scan_files/scan_read/scanin_read_in.txt", internalRegs);
	printf("Finished Reading Internal Registers\n");
}

void ReadMPRFRegs(int *RegsDump) 
{
	int i, j;
	int StartIndex, EndIndex;
	int *internalRegs = new int[NUM_SCAN_BITS];

	StartIndex = 890;
	EndIndex = 859;

	scanInternalRead("E:/test_data/scan_files/scan_read/scanin_read_in.txt", internalRegs);

	for (j = 0; j < 16; j++){
		for (i = 0; i < 8; i++){
			RegsDump[j * 8 + (7-i)] = ConvertBinArraytoInt(internalRegs, StartIndex, EndIndex);
			StartIndex = StartIndex + 32;
			EndIndex = EndIndex + 32;	
		}
		if ((j % 4) == 3) {
			StartIndex = StartIndex + 862;
			EndIndex = EndIndex + 862;
		}
		else {
			StartIndex = StartIndex + 159-32;
			EndIndex = EndIndex + 159-32;
		}
	}
}

void ReadBISTRegs(int *BISTDump) 
{
	int i, j;
	int StartIndex, EndIndex;
	int *internalRegs = new int[NUM_SCAN_BITS];

	StartIndex = 846;
	EndIndex = 815;

	scanInternalRead("E:/test_data/scan_files/scan_read/scanin_read_in.txt", internalRegs);

	for (j = 0; j < 16; j++){
		BISTDump[j * 4] = ConvertBinArraytoInt(internalRegs, StartIndex, EndIndex);
		BISTDump[j * 4 + 1] = ConvertBinArraytoInt(internalRegs, StartIndex + 10, EndIndex + 32);
		BISTDump[j * 4 + 2] = ConvertBinArraytoInt(internalRegs, StartIndex + 11, EndIndex + 42);
		BISTDump[j * 4 + 3] = ConvertBinArraytoInt(internalRegs, StartIndex + 12, EndIndex + 43);

		if ((j % 4) == 3) {
			StartIndex = StartIndex + 383 + 735;
			EndIndex = EndIndex + 383 + 735;
		}
		else {
			StartIndex = StartIndex + 383;
			EndIndex = EndIndex + 383;
		}
	}
}

void ReadEMIRegs(int *EMIDump)
{
	int i, j;
	int IndexOffset;
	int *internalRegs = new int[NUM_SCAN_BITS];

	IndexOffset = 0;

	scanInternalRead("E:/test_data/scan_files/scan_read/scanin_read_in.txt", internalRegs);

	for (j = 0; j < 4; j++){
		EMIDump[j * 23] = ConvertBinArraytoInt(internalRegs, IndexOffset + 35, IndexOffset + 27);			// PState Pulse Width
		EMIDump[j * 23 + 1] = ConvertBinArraytoInt(internalRegs, IndexOffset + 44, IndexOffset + 36);		// PState Extend Duration
		EMIDump[j * 23 + 2] = ConvertBinArraytoInt(internalRegs, IndexOffset + 74, IndexOffset + 45);		// PState Config
		EMIDump[j * 23 + 3] = ConvertBinArraytoInt(internalRegs, IndexOffset + 106, IndexOffset + 75);		// PState Mag 0
		EMIDump[j * 23 + 4] = ConvertBinArraytoInt(internalRegs, IndexOffset + 138, IndexOffset + 107);		// PState Mag 1
		EMIDump[j * 23 + 5] = ConvertBinArraytoInt(internalRegs, IndexOffset + 168, IndexOffset + 139);		// Reg SC Config
		EMIDump[j * 23 + 6] = ConvertBinArraytoInt(internalRegs, IndexOffset + 200, IndexOffset + 169);		// Reg SC Mag 0
		EMIDump[j * 23 + 7] = ConvertBinArraytoInt(internalRegs, IndexOffset + 232, IndexOffset + 201);		// Reg SC Mag 1
		EMIDump[j * 23 + 8] = ConvertBinArraytoInt(internalRegs, IndexOffset + 264, IndexOffset + 233);		// Reg SC Mag 2
		EMIDump[j * 23 + 9] = ConvertBinArraytoInt(internalRegs, IndexOffset + 296, IndexOffset + 265);		// Reg SC Mag 3
		EMIDump[j * 23 + 10] = ConvertBinArraytoInt(internalRegs, IndexOffset + 326, IndexOffset + 297);	// Clock Gate Ctrl 3 Config
		EMIDump[j * 23 + 11] = ConvertBinArraytoInt(internalRegs, IndexOffset + 358, IndexOffset + 327);	// Clock Gate Ctrl 3 Mag
		EMIDump[j * 23 + 12] = ConvertBinArraytoInt(internalRegs, IndexOffset + 388, IndexOffset + 359);	// Clock Gate Ctrl 2 Config
		EMIDump[j * 23 + 13] = ConvertBinArraytoInt(internalRegs, IndexOffset + 420, IndexOffset + 389);	// Clock Gate Ctrl 2 Mag
		EMIDump[j * 23 + 14] = ConvertBinArraytoInt(internalRegs, IndexOffset + 450, IndexOffset + 421);	// Clock Gate Ctrl 1 Config
		EMIDump[j * 23 + 15] = ConvertBinArraytoInt(internalRegs, IndexOffset + 482, IndexOffset + 451);	// Clock Gate Ctrl 1 Mag
		EMIDump[j * 23 + 16] = ConvertBinArraytoInt(internalRegs, IndexOffset + 512, IndexOffset + 483);	// Clock Gate Ctrl 0 Config
		EMIDump[j * 23 + 17] = ConvertBinArraytoInt(internalRegs, IndexOffset + 544, IndexOffset + 513);	// Clock Gate Ctrl 0 Mag
		EMIDump[j * 23 + 18] = ConvertBinArraytoInt(internalRegs, IndexOffset + 576, IndexOffset + 545);	// Reg NMOS Config
		EMIDump[j * 23 + 19] = ConvertBinArraytoInt(internalRegs, IndexOffset + 608, IndexOffset + 577);	// Reg NMOS Mag 0
		EMIDump[j * 23 + 20] = ConvertBinArraytoInt(internalRegs, IndexOffset + 640, IndexOffset + 609);	// Reg NMOS Mag 1
		EMIDump[j * 23 + 21] = ConvertBinArraytoInt(internalRegs, IndexOffset + 672, IndexOffset + 641);	// Reg NMOS Mag 2
		EMIDump[j * 23 + 22] = ConvertBinArraytoInt(internalRegs, IndexOffset + 704, IndexOffset + 673);	// Reg NMOS Mag 3

		IndexOffset = IndexOffset + 2267;
	}
}

int ConvertBinArraytoInt(int *InputArray, int startIndex, int EndIndex)
{
	int i;
	int IntOut = 0;
	int shift = 0;

	for (i = EndIndex; i <= startIndex; i++) {
//		printf("StartIndex = %d, EndIndex = %d\n", startIndex, EndIndex);
		IntOut = IntOut + (InputArray[i] << shift);
		shift = shift + 1;
	}
	return IntOut;
}

int ConvertHexArraytoInt(int *InputArray) 
{
	int i;
	int IntOut = 0;
	int shift = 28;

	for (i = 0; i < 8; i++) {
		IntOut = IntOut + (InputArray[i] << shift);
		shift = shift - 4;
	}
	return IntOut;
}

void ConvertIntto32bBinArray(int input, int *binArray){
	int BIT_COMPARE = 0x00000001;
	int i;
	for (i = 0; i < 32; i++){
//		printf("%08x\n", BIT_COMPARE);
		if (BIT_COMPARE & input) {
			binArray[31-i] = 1;
		}
		else {
			binArray[31-i] = 0;
		}
		BIT_COMPARE = BIT_COMPARE << 1;
	}
}
*/

/*int CompareMemFile(char *fn_memfile, int *MemDump, int CoreNum)
{
	//Variables for Processing of Input lines (Read from file)
	uInt8 fileData[CHIP_MEM_LINES][32];
	int memLine[32];

	int total_lines_read = 0;
	int linecount = 0;
	int i, j;
	int MEM_EQUAL = 1;

	/*********************************************/
	//  FILE I/O Intialization
	/*********************************************/

	//file IO variables
/*	FILE *fptr_memfile;

	if ((fptr_memfile = fopen(fn_memfile, "r")) == NULL){
		printf("Cannot open%s.\n", fn_memfile);
//		return FAIL;
	}
	//printf("DEBUG:scanin file found\n");

	/*********************************************/
	// READ vectors from input file
	/*********************************************/
/*	int input_char;

	//loop for each line of file I/O
	while (((input_char = fgetc(fptr_memfile)) != EOF) && (linecount < MAX_LINES)) {
		//loop to read an input line
		int inputcnt = 0;
		while (input_char != '\n'){
			if (input_char == '1'){
				fileData[linecount][inputcnt] = 1;
				inputcnt++;
			}
			else if ((input_char == '0') | (input_char == 'x') |
				(input_char == 'X') | (input_char == 'z') |
				(input_char == 'Z')){
				fileData[linecount][inputcnt] = 0;
				inputcnt++;
			}

			if ((input_char = fgetc(fptr_memfile)) == EOF)
				break;
		}

		if (inputcnt != 32){
			printf("Input line %d was not formatted correctly, received %d input characters\n", linecount, inputcnt);
			printf("Data: ");
			for (int i = 0; i<32; i++)
				printf("%d ", fileData[linecount][i]);
			printf("\n");
		}
		linecount++;
		//printf("DEBUG: Read line %d\n", linecount);
	}

	fclose(fptr_memfile);
	total_lines_read = linecount;
	//printf("DEBUG: Finished Reading input file\n");

	for (i = 0; i < 1024; i++){
		ConvertIntto32bBinArray(MemDump[CoreNum*CHIP_MEM_LINES+i], memLine);
		for (j = 0; j < 32; j++){
			if (fileData[i][j] != memLine[j]) {
				MEM_EQUAL = 0;
			}
		}
	}
/*	if (MEM_EQUAL == 1) {
		printf("Memory Content for Core%d Matches File\n", CoreNum);
	}
	else {
		printf("Memory Content for Core%d did NOT match File\n", CoreNum);
	}
*/
/*	return MEM_EQUAL;
}
*/

//void ReadMemFile(char *fn_memfile, int *MemFile)
//{
//	//Variables for Processing of Input lines (Read from file)
//	uInt8 fileData[CHIP_MEM_LINES][32];
//	int memLine[32];
//
//	int *MemFileBits = new int[CHIP_MEM_LINES * 32];
//	int total_lines_read = 0;
//	int linecount = 0;
//	int i, j;
//	int MEM_EQUAL = 1;
//
//	/*********************************************/
//	//  FILE I/O Intialization
//	/*********************************************/
//
//	//file IO variables
//	FILE *fptr_memfile;
//
//	if ((fptr_memfile = fopen(fn_memfile, "r")) == NULL){
//		printf("Cannot open%s.\n", fn_memfile);
//		//		return FAIL;
//	}
//	//printf("DEBUG:scanin file found\n");
//
//	/*********************************************/
//	// READ vectors from input file
//	/*********************************************/
//	int input_char;
//
//	//loop for each line of file I/O
//	while (((input_char = fgetc(fptr_memfile)) != EOF) && (linecount < MAX_LINES)) {
//		//loop to read an input line
//		int inputcnt = 0;
//		while (input_char != '\n'){
//			if (input_char == '1'){
//				fileData[linecount][inputcnt] = 1;
//				inputcnt++;
//			}
//			else if ((input_char == '0') | (input_char == 'x') |
//				(input_char == 'X') | (input_char == 'z') |
//				(input_char == 'Z')){
//				fileData[linecount][inputcnt] = 0;
//				inputcnt++;
//			}
//
//			if ((input_char = fgetc(fptr_memfile)) == EOF)
//				break;
//		}
//
//		if (inputcnt != 32){
//			printf("Input line %d was not formatted correctly, received %d input characters\n", linecount, inputcnt);
//			printf("Data: ");
//			for (int i = 0; i<inputcnt; i++)
//				printf("%d ", fileData[linecount][i]);
//			printf("\n");
//		}
//		linecount++;
//		//printf("DEBUG: Read line %d\n", linecount);
//	}
//
//	fclose(fptr_memfile);
//	total_lines_read = linecount;
//	//printf("DEBUG: Finished Reading input file\n");
//
//	for (i = 0; i < 1024; i++){
//		for (j = 0; j < 32; j++){
//			MemFileBits[i * 32 + j] = fileData[i][31-j];
//		}
//	}
//	
//	for (i = 0; i < 1024; i++){
//		MemFile[i] = ConvertBinArraytoInt(MemFileBits, i * 32 + 31, i * 32);
//	}
//
////	for (i = 0; i < 16; i++) {
////		printf("%8x\n", ConvertBinArraytoInt(MemFileBits, i * 32 + 31, i * 32));
////	}
//	/*	if (MEM_EQUAL == 1) {
//	printf("Memory Content for Core%d Matches File\n", CoreNum);
//	}
//	else {
//	printf("Memory Content for Core%d did NOT match File\n", CoreNum);
//	}
//	*/
////	return MEM_EQUAL;
//}
//
//int CompareMemFile(int *MemFile, int *MemDump, int CoreNum)
//{
//	int i;
//	int MEM_EQUAL = 1;
//
//	for (i = 0; i < CHIP_MEM_LINES; i++){
//		//printf("HexNum from file is = %d\n", fileHexNum);
//		//printf("HexNum from Reg is = %08x\n", MemDump[CoreNum * 8 + i]);
//		if (MemDump[CoreNum * CHIP_MEM_LINES + i] != MemFile[i]) {
//			MEM_EQUAL = 0;
//		}
//	}
//
//	return MEM_EQUAL;
//}
//
//void ReadVmonFile(char *fn_vmonfile, double *vmonVal)
//{
//	int lineCount;
//	int colCount;
//	int HEIGHT = 901;
//	int WIDTH = 2;
//	double vmon_variable;
//	
//	//file IO variables
//	FILE *fptr_vmonfile;
//
//	if ((fptr_vmonfile = fopen(fn_vmonfile, "r")) == NULL){
//	printf("Cannot open%s.\n", fn_vmonfile);
//	}
//
//	for (lineCount = 0; lineCount < HEIGHT; lineCount++) {
//		for (colCount = 0; colCount < WIDTH; colCount++) {
//			fscanf(fptr_vmonfile, "%lf", &vmon_variable);
//			vmonVal[lineCount * 2 + colCount] = vmon_variable;
//		}
//	}
//	
//	fclose(fptr_vmonfile);
//}
//
//double convertVmonVal(double vmon_in, double *vmonVal)
//{
//	int i;
//	double temp_diff;
//	double min_diff;
//	int min_index;
//	double vmon_diff;
//	double vmon_index_diff;
//	double convertVal;
//
//	for (i = 0; i < 901; i++) {
//		if (i == 0) {
//			min_diff = fabs(vmon_in - vmonVal[i * 2 + 1]);
////			printf("min_diff = %f\n", min_diff);
//			min_index = i * 2;
//		}
//		else {
//			temp_diff = fabs(vmon_in - vmonVal[i * 2 + 1]);
//			if (temp_diff < min_diff) {
//				min_diff = temp_diff;
//				min_index = i * 2;
//			}
//		}
//	}
//
//	if (vmonVal[min_index + 1] < vmon_in) {
//		vmon_diff = vmonVal[min_index - 1] - vmonVal[min_index + 1];
//		vmon_index_diff = vmonVal[min_index - 2] - vmonVal[min_index];
//		convertVal = vmonVal[min_index] + vmon_index_diff * (min_diff / vmon_diff);
//	}
//	else if (vmonVal[min_index + 1] > vmon_in) {
//		vmon_diff = vmonVal[min_index + 1] - vmonVal[min_index + 3];
//		vmon_index_diff = vmonVal[min_index] - vmonVal[min_index + 2];
//		convertVal = vmonVal[min_index] - vmon_index_diff * (min_diff / vmon_diff);
//	}
//	else if (vmonVal[min_index + 1] == vmon_in) {
//		convertVal = vmonVal[min_index];
//	}
//
//	return convertVal;
//}
//
//void ReadRegFile(char *fn_regfile, int *RegFile)
//{
//	//Variables for Processing of Input lines (Read from file)
//	int fileData[8][8];
//
//	int fileHexNum;
//	int total_lines_read = 0;
//	int linecount = 0;
//	int i, j;
//	int REG_EQUAL = 1;
//
//	/*********************************************/
//	//  FILE I/O Intialization
//	/*********************************************/
//
//	//file IO variables
//	FILE *fptr_regfile;
//
//	if ((fptr_regfile = fopen(fn_regfile, "r")) == NULL){
//		printf("Cannot open%s.\n", fn_regfile);
//		//	return FAIL;
//	}
//	//printf("DEBUG:scanin file found\n");
//
//	/*********************************************/
//	// READ vectors from input file
//	/*********************************************/
//	int input_char;
//
//	//loop for each line of file I/O
//	while (((input_char = fgetc(fptr_regfile)) != EOF) && (linecount < MAX_LINES)) {
//		//loop to read an input line
//		int inputcnt = 0;
//		while (input_char != '\n'){
//			if (input_char < 'a') {
//				//				printf("input_char = %d\n", input_char);
//				fileData[linecount][inputcnt] = input_char - '0';
//				//				printf("input_char = %x\n", fileData[linecount][inputcnt]);
//				inputcnt++;
//			}
//			else  {
//				//				printf("input_char = %d\n", input_char);
//				fileData[linecount][inputcnt] = input_char - 'W';
//				//				printf("input_char = %x\n", fileData[linecount][inputcnt]);
//				inputcnt++;
//			}
//
//			if ((input_char = fgetc(fptr_regfile)) == EOF)
//				break;
//		}
//
//		if (inputcnt != 8){
//			printf("Input line %d was not formatted correctly, received %d input characters\n", linecount, inputcnt);
//			printf("Data: ");
//			for (int i = 0; i<inputcnt; i++)
//				printf("%d ", fileData[linecount][i]);
//			printf("\n");
//		}
//		linecount++;
//		//printf("DEBUG: Read line %d\n", linecount);
//	}
//
//	fclose(fptr_regfile);
//
//	total_lines_read = linecount;
//	//printf("DEBUG: Finished Reading input file\n");
//
//	for (i = 0; i < 8; i++){
//		RegFile[i] = ConvertHexArraytoInt(fileData[i]);
//	}
//	
//}
//
//int CompareRegFile(int *RegFile, int *RegDump, int CoreNum)
//{
//	int i;
//	int REG_EQUAL = 1;
//
//	for (i = 0; i < 8; i++){
////		printf("HexNum from file is = %d\n", fileHexNum);
//		printf("HexNum from Reg is = %08x\n", RegDump[CoreNum * 8 + i]);
//		if (RegDump[CoreNum * 8 + i] != RegFile[i]) {
//			REG_EQUAL = 0;
//		}
//	}
//
//	return REG_EQUAL;
//}
//
//void ReadEMIRegFile(char *fn_regfile, int *RegFile)
//{
//	//Variables for Processing of Input lines (Read from file)
//	int fileData[23][8];
//
//	int fileHexNum;
//	int total_lines_read = 0;
//	int linecount = 0;
//	int i, j;
//	int REG_EQUAL = 1;
//
//	/*********************************************/
//	//  FILE I/O Intialization
//	/*********************************************/
//
//	//file IO variables
//	FILE *fptr_regfile;
//
//	if ((fptr_regfile = fopen(fn_regfile, "r")) == NULL){
//		printf("Cannot open%s.\n", fn_regfile);
//		//	return FAIL;
//	}
//	//printf("DEBUG:scanin file found\n");
//
//	/*********************************************/
//	// READ vectors from input file
//	/*********************************************/
//	int input_char;
//
//	//loop for each line of file I/O
//	while (((input_char = fgetc(fptr_regfile)) != EOF) && (linecount < MAX_LINES)) {
//		//loop to read an input line
//		int inputcnt = 0;
//		while (input_char != '\n'){
//			if (input_char < 'a') {
//				//				printf("input_char = %d\n", input_char);
//				fileData[linecount][inputcnt] = input_char - '0';
//				//				printf("input_char = %x\n", fileData[linecount][inputcnt]);
//				inputcnt++;
//			}
//			else  {
//				//				printf("input_char = %d\n", input_char);
//				fileData[linecount][inputcnt] = input_char - 'W';
//				//				printf("input_char = %x\n", fileData[linecount][inputcnt]);
//				inputcnt++;
//			}
//
//			if ((input_char = fgetc(fptr_regfile)) == EOF)
//				break;
//		}
//
//		if (inputcnt != 8){
//			printf("Input line %d was not formatted correctly, received %d input characters\n", linecount, inputcnt);
//			printf("Data: ");
//			for (int i = 0; i<inputcnt; i++)
//				printf("%d ", fileData[linecount][i]);
//			printf("\n");
//		}
//		linecount++;
//		//printf("DEBUG: Read line %d\n", linecount);
//	}
//
//	fclose(fptr_regfile);
//
//	total_lines_read = linecount;
//	//printf("DEBUG: Finished Reading input file\n");
//
//	for (i = 0; i < 23; i++){
//		RegFile[i] = ConvertHexArraytoInt(fileData[i]);
//	}
//
//}
//
//int CompareEMIRegFile(int *RegFile, int *RegDump, int LyrNum)
//{
//	int i;
//	int REG_EQUAL = 1;
//
//	for (i = 0; i < 23; i++){
//		//		printf("HexNum from file is = %d\n", fileHexNum);
//		printf("HexNum from EMIReg is = %08x\n", RegDump[LyrNum * 23 + i]);
//		if (RegDump[LyrNum * 23 + i] != RegFile[i]) {
//			REG_EQUAL = 0;
//		}
//	}
//
//	return REG_EQUAL;
//}


//**************************************************************//
//               FVS Comm Chip Test Functions				    //
//**************************************************************//
/*
void FVS_Comm_PowerUp_BotIn(int numStep)
{
	double vstep = 0.02;
	int i;

	DTG5334_SetChannelHigh(1, 0.9);
	DTG5334_SetChannelHigh(2, 0.9);

	DTG5334_SetChannelLow(1, 0.0);
	DTG5334_SetChannelLow(2, 0.0);

	for (i = 0; i <= numStep; i++) {
		printf("i = %d, voltage is %f\n", i, vstep*i);
		E3646A_SetVoltage(_VIN_VOUT3, _VOUT3, (3 * vstep*i));
		E3646A_SetVoltage(_VIN_VOUT3, _VIN, (3 * vstep*i));
		E3646A_SetVoltage(_TEST_3P6_2P7, _TEST2P7, (3 * vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT2, (2 * vstep*i));
		E3646A_SetVoltage(_TEST_1P8_0P9, _TEST1P8, (2 * vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT1, (vstep*i));
		E3646A_SetVoltage(_TEST_1P8_0P9, _TEST0P9, (vstep*i));
	}
}

void FVS_Comm_PowerDn_BotIn(int numStep)
{
	double vstep = 0.02;
	int i;

	for (i = 0; i <= numStep; i++) {
		printf("i = %d, voltage is %f\n", i, 0.9 - vstep*i);
		E3646A_SetVoltage(_VIN_VOUT3, _VOUT3, 2.7 - (3 * vstep*i));
		E3646A_SetVoltage(_VIN_VOUT3, _VIN, 2.7 - (3 * vstep*i));
		E3646A_SetVoltage(_TEST_3P6_2P7, _TEST2P7, 2.7 - (3 * vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT2, 1.8 - (2 * vstep*i));
		E3646A_SetVoltage(_TEST_1P8_0P9, _TEST1P8, 1.8 - (2 * vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT1, 0.9 - (vstep*i));
		E3646A_SetVoltage(_TEST_1P8_0P9, _TEST0P9, 0.9 - (vstep*i));
	}
}

void FVS_Comm_PowerUp_MidIn(int numStep)
{
	double vstep = 0.02;
	double in_low;
	double in_high;
	int i;

	DTG5334_SetChannelHigh(1, 0.1);
	DTG5334_SetChannelHigh(2, 0.1);

	DTG5334_SetChannelLow(1, 0.0);
	DTG5334_SetChannelLow(2, 0.0);

	for (i = 0; i <= numStep; i++) {
		printf("i = %d, voltage is %f\n", i, vstep*i);
		E3646A_SetVoltage(_VIN_VOUT3, _VOUT3, (3 * vstep*i));
		E3646A_SetVoltage(_VIN_VOUT3, _VIN, (3 * vstep*i));
		E3646A_SetVoltage(_TEST_3P6_2P7, _TEST2P7, (3 * vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT2, (2 * vstep*i));
		E3646A_SetVoltage(_TEST_1P8_0P9, _TEST1P8, (2 * vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT1, (vstep*i));
		E3646A_SetVoltage(_TEST_1P8_0P9, _TEST0P9, (vstep*i));

		if (i > 0) {
			if ((i % 5) == 0) {

				in_low = vstep * i;
				in_high = in_low + 0.1;

				DTG5334_SetChannelHigh(1, in_high);
				DTG5334_SetChannelHigh(2, in_high);

				DTG5334_SetChannelLow(1, in_low);
				DTG5334_SetChannelLow(2, in_low);
				::Sleep(500);
			}
		}
	}
	DTG5334_SetChannelHigh(1, 1.7);
	DTG5334_SetChannelHigh(2, 1.7);
}

void FVS_Comm_PowerDn_MidIn(int numStep)
{
	double vstep = 0.02;
	double in_low;
	double in_high;
	int i;

	DTG5334_SetChannelHigh(1, 1.0);
	DTG5334_SetChannelHigh(2, 1.0);

	for (i = 0; i <= numStep; i++) {
		printf("i = %d, voltage is %f\n", i, 0.9 - vstep*i);
		E3646A_SetVoltage(_VIN_VOUT3, _VOUT3, 2.7 - (3 * vstep*i));
		E3646A_SetVoltage(_VIN_VOUT3, _VIN, 2.7 - (3 * vstep*i));
		E3646A_SetVoltage(_TEST_3P6_2P7, _TEST2P7, 2.7 - (3 * vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT2, 1.8 - (2 * vstep*i));
		E3646A_SetVoltage(_TEST_1P8_0P9, _TEST1P8, 1.8 - (2 * vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT1, 0.9 - (vstep*i));
		E3646A_SetVoltage(_TEST_1P8_0P9, _TEST0P9, 0.9 - (vstep*i));

		if (i > 0) {
			if ((i % 5) == 0) {

				in_low = 0.9 - (vstep * i);
				in_high = in_low + 0.1;

				DTG5334_SetChannelLow(1, in_low);
				DTG5334_SetChannelLow(2, in_low);

				DTG5334_SetChannelHigh(1, in_high);
				DTG5334_SetChannelHigh(2, in_high);

				::Sleep(500);
			}
		}
	}
}

void FVS_Comm_PowerUp_TopIn(int numStep)
{
	double vstep = 0.02;
	double in_low;
	double in_high;
	int i;

	DTG5334_SetChannelHigh(1, 0.9);
	DTG5334_SetChannelHigh(2, 0.9);
	
	DTG5334_SetChannelLow(1, 0.0);
	DTG5334_SetChannelLow(2, 0.0);
	
	for (i = 0; i <= numStep; i++) {
		printf("i = %d, voltage is %f\n", i, vstep*i);
		E3646A_SetVoltage(_VIN_VOUT3, _VOUT3, (3 * vstep*i));
		E3646A_SetVoltage(_VIN_VOUT3, _VIN, (3 * vstep*i));
		E3646A_SetVoltage(_TEST_3P6_2P7, _TEST2P7, (3 * vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT2, (2 * vstep*i));
		E3646A_SetVoltage(_TEST_1P8_0P9, _TEST1P8, (2 * vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT1, (vstep*i));
		E3646A_SetVoltage(_TEST_1P8_0P9, _TEST0P9, (vstep*i));
	}
}

void FVS_Comm_PowerDn_TopIn(int numStep)
{
	double vstep = 0.02;
	double in_low;
	double in_high;
	int i;

	for (i = 0; i <= numStep; i++) {
		printf("i = %d, voltage is %f\n", i, 0.9 - vstep*i);
		E3646A_SetVoltage(_VIN_VOUT3, _VOUT3, 2.7 - (3 * vstep*i));
		E3646A_SetVoltage(_VIN_VOUT3, _VIN, 2.7 - (3 * vstep*i));
		E3646A_SetVoltage(_TEST_3P6_2P7, _TEST2P7, 2.7 - (3 * vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT2, 1.8 - (2 * vstep*i));
		E3646A_SetVoltage(_TEST_1P8_0P9, _TEST1P8, 1.8 - (2 * vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT1, 0.9 - (vstep*i));
		E3646A_SetVoltage(_TEST_1P8_0P9, _TEST0P9, 0.9 - (vstep*i));
	}
}

void FVS_Comm_PowerUp_Unified(int inputLayer, int numStep) 
{
	if (inputLayer == 1) {
		FVS_Comm_PowerUp_BotIn(numStep);
	}
	else if (inputLayer == 2) {
		FVS_Comm_PowerUp_MidIn(numStep);
	}
	else if (inputLayer == 3) {
		FVS_Comm_PowerUp_TopIn(numStep);
	}
}

void FVS_Comm_PowerDn_Unified(int inputLayer, int numStep)
{
	if (inputLayer == 1) {
		FVS_Comm_PowerDn_BotIn(numStep);
	}
	else if (inputLayer == 2) {
		FVS_Comm_PowerDn_MidIn(numStep);
	}
	else if (inputLayer == 3) {
		FVS_Comm_PowerDn_TopIn(numStep);
	}
}

void FVS_Comm_Raise_StackVoltage(double StartStackVol, double vstep, int numStep) {

	int i;

	double vin_plus_voltage;
	for (i = 0; i <= numStep; i++) {
		printf("i = %d, voltage is %f\n", i, StartStackVol + vstep*i);

		E3646A_SetVoltage(_VIN_VOUT3, _VIN, (StartStackVol * 3 + 3 * vstep*i));
		E3646A_SetVoltage(_VIN_VOUT3, _VOUT3, (StartStackVol * 3 + 3 * vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT2, (StartStackVol * 2 + 2 * vstep*i));
		E3646A_SetVoltage(_TEST_1P8_0P9, _TEST1P8, (StartStackVol * 2 + 2 * vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT1, (StartStackVol + vstep*i));
		E3646A_SetVoltage(_TEST_1P8_0P9, _TEST0P9, (StartStackVol + vstep*i));
	}
}

void FVS_Comm_Lower_StackVoltage(double StartStackVol, double vstep, int numStep) {

	int i;

	double vin_plus_voltage;

	for (i = 0; i <= numStep; i++) {
		printf("i = %d, voltage is %f\n", i, StartStackVol - vstep*i);

		E3646A_SetVoltage(_VIN_VOUT3, _VIN, (StartStackVol * 3 - 3 * vstep*i));
		E3646A_SetVoltage(_VIN_VOUT3, _VOUT3, (StartStackVol * 3 - 3 * vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT2, (StartStackVol * 2 - 2 * vstep*i));
		E3646A_SetVoltage(_TEST_1P8_0P9, _TEST1P8, (StartStackVol * 2 - 2 * vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT1, (StartStackVol - vstep*i));
		E3646A_SetVoltage(_TEST_1P8_0P9, _TEST0P9, (StartStackVol - vstep*i));
	}
}


void FVS_Comm_Raise_VOUT3_VOUT2(double vout1, double StartStackVol, double vstep, int numStep) {

	int i;

	for (i = 0; i <= numStep; i++) {
		printf("i = %d, Rx voltage is %f\n", i, StartStackVol + vstep*i);

		E3646A_SetVoltage(_VIN_VOUT3, _VIN, (vout1 + StartStackVol * 2 + 2 * vstep*i));
		E3646A_SetVoltage(_VIN_VOUT3, _VOUT3, (vout1 + StartStackVol * 2 + 2 * vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT2, (vout1 + StartStackVol + vstep*i));
		E3646A_SetVoltage(_TEST_1P8_0P9, _TEST1P8, (vout1 + StartStackVol + vstep*i));
	}
}

void FVS_Comm_Lower_VOUT3_VOUT2(double vout1, double StartStackVol, double vstep, int numStep) {

	int i;

	double vin_plus_voltage;

	for (i = 0; i <= numStep; i++) {
		printf("i = %d, Rx voltage is %f\n", i, StartStackVol - vstep*i);

		E3646A_SetVoltage(_VIN_VOUT3, _VIN, (vout1 + StartStackVol * 2 - 2 * vstep*i));
		E3646A_SetVoltage(_VIN_VOUT3, _VOUT3, (vout1 + StartStackVol * 2 - 2 * vstep*i));
		E3646A_SetVoltage(_VOUT2_VOUT1, _VOUT2, (vout1 + StartStackVol - vstep*i));
		E3646A_SetVoltage(_TEST_1P8_0P9, _TEST1P8, (vout1 + StartStackVol - vstep*i));
	}
}
*/

//**************************************************************//
//        MM34401A Multimeter Functions (GPIB Interface)        //
//**************************************************************//

//TODO: set up integration time: tradeoff between measurement time and precision
//TODO: external trigger (up to N), each initiates M samples with initial delay Td, interval deltaT, integration time Tint.
//TODO: configure buffer/internal memory to hold read data before fetching them back

//Device is the "device unit descriptor" integer to address a certain DMM
float MM34401A_MeasResistance(int Device) {

	char RdBuffer[101];
	float Resistance;

//	_ibwrt(_MM34401A, "MEASure:RESistance");
//	::Sleep(500);
//
	_ibwrt(Device, "MEASure:RESistance?");

	// Measure Average
	_ibrd(Device, RdBuffer, 100);
	RdBuffer[ibcntl] = '\0';         // Null terminate the ASCII string    

//	printf("%s\n", RdBuffer);
	sscanf(RdBuffer, "%f", &Resistance);

//	printf("%f\n", Resistance);
	return Resistance;
}

float MM34401A_MeasVoltage(int Device) {

	char RdBuffer[101];
	float Voltage;

	//	_ibwrt(_MM34401A, "MEASure:RESistance");
	//	::Sleep(500);
	//
	_ibwrt(Device, "MEASure:VOLtage?");

	// Measure Average
	_ibrd(Device, RdBuffer, 100);
	RdBuffer[ibcntl] = '\0';         // Null terminate the ASCII string    

	//	printf("%s\n", RdBuffer);
	sscanf(RdBuffer, "%f", &Voltage);

	//	printf("%f\n", Resistance);
	return Voltage;
}

void MM34410A_6_MeasVoltage_Config(int Device, float NPLCycles, char* TrigSource = "IMM", float TrigDelay = 0.1, int SampCount = 1, int TrigCount = 1) {
	/*********Configure the DMM for voltage measurement, adapted from MM34410A_6_MeasCurrent_Config********/
	/*** ADDITION: use Hi-Z input IMPedance to minimize DMM loading during small voltage measurement ******/

	_ibwrt(Device, "CONFigure:VOLtage:DC MIN, MIN");
        _ibwrt(Device, "VOLtage:DC:RANGe 0.1"); //100mV minimum range
	_ibwrt(Device, "VOLtage:DC:RESolution MINimum"); //highest resolution
	char Command_NPLC[100];
	sprintf(Command_NPLC, "VOLtage:DC:NPLCycles %f", NPLCycles);
	_ibwrt(Device, Command_NPLC); //MAXiumn = 100PLC integration time ~ 1.67 seconds per sample measurement
	char Command_TrigSource[100];
	sprintf(Command_TrigSource, "TRIGger:SOURce %s", TrigSource);
	_ibwrt(Device, Command_TrigSource);
	//	_ibwrt(_MM34401A, "TRIGger:SOURce IMMediate"); //default trigger source is an immediate internal trigger
	char Command_TrigDelay[100];
	sprintf(Command_TrigDelay, "TRIGger:DELay %f", TrigDelay);
	_ibwrt(Device, Command_TrigDelay);
	////	_ibwrt(_MM34401A, "TRIGger:DELay MIN"); //NO delay!
	//	_ibwrt(_MM34401A, "TRIGger:DELay 0.1"); //PSU transition/settle time
	char Command_SampCount[100];
	sprintf(Command_SampCount, "SAMPle:COUNT %d", SampCount);
	_ibwrt(Device, Command_SampCount);
	char Command_TrigCount[100];
	sprintf(Command_TrigCount, "TRIGger:COUNT %d", TrigCount);
	_ibwrt(Device, Command_TrigCount); 

	_ibwrt(Device, "ZERO:AUTO ON");

	//Specific to DC volage measurement!
	_ibwrt(Device, "VOLtage:DC:IMPedance:AUTO ON"); //AUTO function for DC input IMPedance
													//uses 10Mohm for 100V and 1000V ranges; >10Gohm for the 100mV, 1V and 10V ranges
	//Scrutinize if DMM is measuring the output from In-Amp(low Rout): should I choose 10Mohm to reduce noise? 

	//CAUTIOUS: quick and dirty! temporary! 
	_ibwrt(Device, "VOLtage:DC:RANGe:AUTO ON"); 
	//at least I can know the values of currents! Cautious about the increase in measurement time?!

}

void MM34401A_MeasCurrent_Config(int Device, float NPLCycles, char* TrigSource="IMM", float TrigDelay=0.1, int SampCount=1, int TrigCount=1) {
	/*********Configure the DMM for current measurement********/
	// Only need to call this function ONCE before all subsequent MM34401A_MeasCurrent() calls. (save time, faster)
	// TODO: try INITiate for faster multiple measurments (eg. external trigger) stored to internal memory 
	// and use FETCh? transfer to output buffer for bus controller to read from.

	//TODO: understand resolution vs accuracy vs integration time!!!
//	char RdBuffer[101];
//	float Current;

	_ibwrt(Device, "CONFigure:CURRent:DC MIN, MIN");
	//_ibwrt(_MM34401A, "FUNCtion \"CURRent:DC\"");
	_ibwrt(Device, "CURRent:DC:RANGe MINimum"); //10mA range
	//_ibwrt(Device, "CURRent:DC:RANGe 0.1"); //100mA range
	_ibwrt(Device, "CURRent:DC:RESolution MINimum"); //10nA resolution
	char Command_NPLC[100];
	sprintf(Command_NPLC, "CURRent:DC:NPLCycles %f", NPLCycles);
	_ibwrt(Device, Command_NPLC); //MAXiumn = 100PLC integration time ~ 1.67 seconds per sample measurement
	char Command_TrigSource[100];
	sprintf(Command_TrigSource, "TRIGger:SOURce %s", TrigSource);
	_ibwrt(Device, Command_TrigSource);
//	_ibwrt(_MM34401A, "TRIGger:SOURce IMMediate"); //default trigger source is an immediate internal trigger
	char Command_TrigDelay[100];
	sprintf(Command_TrigDelay, "TRIGger:DELay %f", TrigDelay);
	_ibwrt(Device, Command_TrigDelay);
////	_ibwrt(_MM34401A, "TRIGger:DELay MIN"); //NO delay!
//	_ibwrt(_MM34401A, "TRIGger:DELay 0.1"); //PSU transition/settle time
	char Command_SampCount[100];
	sprintf(Command_SampCount, "SAMPle:COUNT %d", SampCount);
	_ibwrt(Device, Command_SampCount);
//	_ibwrt(_MM34401A, "SAMPle:COUNT 1"); //1 sample for a trigger
	char Command_TrigCount[100];
	sprintf(Command_TrigCount, "TRIGger:COUNT %d", TrigCount);
	_ibwrt(Device, Command_TrigCount); //1 trigger

	_ibwrt(Device, "ZERO:AUTO ON");

	//CAUTIOUS: quick and dirty! temporary! 
//	_ibwrt(Device, "CURRent:DC:RANGe:AUTO ON"); 

//	_ibwrt(_MM34401A, "READ?"); 

	// Measure Average
//	_ibrd(_MM34401A, RdBuffer, 100);
//	RdBuffer[ibcntl] = '\0';         // Null terminate the ASCII string    

	//	printf("%s\n", RdBuffer);
//	sscanf(RdBuffer, "%f", &Current);

}

//TODO: the second DMM is different from the 1st one!!! different Min Max etc!!!
void MM34410A_6_MeasCurrent_Config(int Device, float NPLCycles, char* TrigSource = "IMM", float TrigDelay = 0.1, int SampCount = 1, int TrigCount = 1) {
	/*********Configure the DMM for current measurement********/
	// Only need to call this function ONCE before all subsequent MM34401A_MeasCurrent() calls. (save time, faster)
	// TODO: try INITiate for faster multiple measurments (eg. external trigger) stored to internal memory 
	// and use FETCh? transfer to output buffer for bus controller to read from.

	//TODO: understand resolution vs accuracy vs integration time!!!
	//	char RdBuffer[101];
	//	float Current;

	_ibwrt(Device, "CONFigure:CURRent:DC MIN, MIN");
	//_ibwrt(_MM34401A, "FUNCtion \"CURRent:DC\"");
//	_ibwrt(Device, "CURRent:DC:RANGe 0.1"); //100mA range
//	_ibwrt(Device, "CURRent:DC:RANGe 0.01"); //10mA range
//	_ibwrt(Device, "CURRent:DC:RANGe 0.001"); //1mA range (larger sense resistor (thus burden voltage) than  100uA and 10mA ranges!!!)
	_ibwrt(Device, "CURRent:DC:RANGe 0.0001"); //100uA range
	_ibwrt(Device, "CURRent:DC:RESolution MINimum"); //100pA resolution
	char Command_NPLC[100];
	sprintf(Command_NPLC, "CURRent:DC:NPLCycles %f", NPLCycles);
	_ibwrt(Device, Command_NPLC); //MAXiumn = 100PLC integration time ~ 1.67 seconds per sample measurement
	char Command_TrigSource[100];
	sprintf(Command_TrigSource, "TRIGger:SOURce %s", TrigSource);
	_ibwrt(Device, Command_TrigSource);
	//	_ibwrt(_MM34401A, "TRIGger:SOURce IMMediate"); //default trigger source is an immediate internal trigger
	char Command_TrigDelay[100];
	sprintf(Command_TrigDelay, "TRIGger:DELay %f", TrigDelay);
	_ibwrt(Device, Command_TrigDelay);
	////	_ibwrt(_MM34401A, "TRIGger:DELay MIN"); //NO delay!
	//	_ibwrt(_MM34401A, "TRIGger:DELay 0.1"); //PSU transition/settle time
	char Command_SampCount[100];
	sprintf(Command_SampCount, "SAMPle:COUNT %d", SampCount);
	_ibwrt(Device, Command_SampCount);
	//	_ibwrt(_MM34401A, "SAMPle:COUNT 1"); //1 sample for a trigger
	char Command_TrigCount[100];
	sprintf(Command_TrigCount, "TRIGger:COUNT %d", TrigCount);
	_ibwrt(Device, Command_TrigCount); //1 trigger

	_ibwrt(Device, "ZERO:AUTO ON");

	//CAUTIOUS: quick and dirty! temporary! 
//	_ibwrt(Device, "CURRent:DC:RANGe:AUTO ON"); 

}

float MM34401A_MeasCurrent(int Device) {

	char RdBuffer[101];
	float Current;

	_ibwrt(Device, "READ?");

	// Measure Average
	_ibrd(Device, RdBuffer, 100);
	RdBuffer[ibcntl] = '\0';         // Null terminate the ASCII string    

	//	printf("%s\n", RdBuffer);
	sscanf(RdBuffer, "%f", &Current);

	//	printf("%f\n", Resistance);
	return Current;
}
/*
//wrong sequence!!!
char* MM34401A_MeasCurrent_MultiSample() {

	char RdBuffer[12001];
	//float Current;

	_ibwrt(_MM34401A, "INITiate");
	_ibwrt(_MM34401A, "FETCh?");
	//_ibwrt(_MM34401A, "READ?");

	// Measure Average
	_ibrd(_MM34401A, RdBuffer, 12000);
	RdBuffer[ibcntl] = '\0';         // Null terminate the ASCII string    

	printf("%s\n", RdBuffer);
	//sscanf(RdBuffer, "%f", &Current);

	//return Current;
	return RdBuffer;
}
*/

void ELTM6514_MeasCurrent_Config(int Device, int TrigCount, float NPLCycles = 10, char* CURRentRANGe = "0.0000002") {
	/*********Configure the electrometer for current measurement********/

	_ibwrt(Device, "SENSe:FUNCtion 'CURRent'");
	_ibwrt(Device, "CURRent:DAMPing ON"); //turn on Damping
//	_ibwrt(Device, "CURRent:RANGe 0.0000002"); //200nA range
	char Command_CURRentRANGe[100];
	sprintf(Command_CURRentRANGe, "CURRent:RANGe %s", CURRentRANGe);
	_ibwrt(Device, Command_CURRentRANGe); 

	_ibwrt(Device, "DISPlay:DIGITs 7"); //6+1/2 digit resolution display
	//_ibwrt(Device, "CURRent:DC:RESolution MINimum"); //100pA resolution
	char Command_NPLC[100];
	sprintf(Command_NPLC, "CURRent:NPLCycles %f", NPLCycles);
	_ibwrt(Device, Command_NPLC); //ADC integration time for the best accuracy: 1PLC(0.0167s) to 10PLC (0.167s) 
//TODO: figure out the trigger model! arm layer vs trigger layer, arm count vs trigger count vs sample count
	_ibwrt(Device, "ARM:SOURce IMMediate");
	_ibwrt(Device, "ARM:COUNt 1");
	_ibwrt(Device, "ARM:DIRection ACCeptor");
//	_ibwrt(Device, "ARM:ILINe 1");            //input trigger line
//	_ibwrt(Device, "ARM:OLINe 2");            //output trigger line
//	_ibwrt(Device, "ARM:OUTPut TRIGger");     //output a trigger signal once measurement is done          
	_ibwrt(Device, "TRIGger:SOURce TLINk");   //Trigger link (TLINk) means the external trigger
	char Command_TrigCount[100];
	sprintf(Command_TrigCount, "TRIGger:COUNt %d", TrigCount);
	_ibwrt(Device, Command_TrigCount); 
	//_ibwrt(Device, "TRIGger:COUNt 2");        // one trigger for DC one trigger for pumping
//	_ibwrt(Device, "TRIGger:DELay 0.02");     //for 200nA range, AUTO delay=10ms, preamp settling time (to 10% of final value)=15ms
	_ibwrt(Device, "TRIGger:DELay 0.2");     //for 200nA range, AUTO delay=10ms, preamp settling time (to 10% of final value)=15ms
	_ibwrt(Device, "TRIGger:DIRection ACCeptor"); 
	_ibwrt(Device, "TRIGger:ILINe 1");        //input trigger line 
	_ibwrt(Device, "TRIGger:OLINe 2");        //output trigger line
	_ibwrt(Device, "TRIGger:OUTPut SENSe");  //output a trigger signal once measurement is done      
	//	_ibwrt(_MM34401A, "TRIGger:SOURce IMMediate"); //default trigger source is an immediate internal trigger
    
	_ibwrt(Device, "SYSTem:AZERo ON");
	//_ibwrt(Device, "ZERO:AUTO ON");

	_ibwrt(Device, "FORMat:ELEMents READing"); //default: "READing, TIME, STATus" all three elements

	_ibwrt(Device, "SYSTem:ZCHeck ON");
	_ibwrt(Device, "SYSTem:ZCORrect ON");
	_ibwrt(Device, "SYSTem:ZCHeck OFF");
	//CAUTIOUS: quick and dirty! temporary! 
//	_ibwrt(Device, "CURRent:DC:RANGe:AUTO ON"); 

}

//**************************************************************//
//       DSA91304A oscilloscope Functions (GPIB Interface)      //
//**************************************************************//
/*
void DSA91304A_ScopeHeadOff()
{
	_ibwrt(_DSA91304A, ":SYSTem:HEADer OFF");
}

void DSA91304A_ScopeRun()
{
	_ibwrt(_DSA91304A, ":RUN");
}

void DSA91304A_ScopeStop()
{
	_ibwrt(_DSA91304A, ":STOP");
}

void DSA91304A_ScopeSingle()
{
	_ibwrt(_DSA91304A, ":SINGle");
}

void DSA91304A_ClearDisplay()
{
	_ibwrt(_DSA91304A, ":CDISplay");
}

void DSA91304A_SetTrigAuto()
{
	_ibwrt(_DSA91304A, ":TRIGger:SWEep AUTO");
}

void DSA91304A_SetTrigTrig()
{
	_ibwrt(_DSA91304A, ":TRIGger:SWEep TRIGgered");
}

void DSA91304A_SetTrigMode(int channelNum, double trigLevel)
{
	char Cmd[200];

	_ibwrt(_DSA91304A, "TRIGger:SWEep TRIGgered");
	_ibwrt(_DSA91304A, "TRIGger:MODE:EDGE");
	sprintf(Cmd, "TRIGger:EDGE:SOURce CHANnel%d", channelNum);
	_ibwrt(_DSA91304A, Cmd);
	_ibwrt(_DSA91304A, "TRIGger:EDGE:SLOPe POSitive");
	sprintf(Cmd, "TRIGger:LEVel CHANnel%d, %f", channelNum, trigLevel);
	_ibwrt(_DSA91304A, Cmd);
	_ibwrt(_DSA91304A, "TRIGger:HYSTeresis HSENsitivity");
}

void DSA91304A_SetSingleCapture(int channelNum, double trigLevel)
{
	DSA91304A_SetTrigMode(channelNum, trigLevel);
	DSA91304A_SetTrigTrig();
	DSA91304A_ClearDisplay();
	DSA91304A_ScopeSingle();
}

void DSA91304A_SetAutoRun()
{
	DSA91304A_SetTrigAuto();
	DSA91304A_ClearDisplay();
	DSA91304A_ScopeRun();
}

void DSA91304A_SetTimeBase_Range(double time_range, double time_delay) // Range : Entire Display Range
{
	char timeBaseCmd[100];

	sprintf(timeBaseCmd, ":TIMebase:REFerence CENTer");
	_ibwrt(_DSA91304A, timeBaseCmd);

	sprintf(timeBaseCmd, ":TIMebase:RANGe %7.12f", time_range);
	_ibwrt(_DSA91304A, timeBaseCmd);

	sprintf(timeBaseCmd, ":TIMebase:POSition %7.12f", time_delay);
	_ibwrt(_DSA91304A, timeBaseCmd);
}

void DSA91304A_SetTimeBase_Scale(double time_scale, double time_delay) // Scale : xx/div
{
	char timeBaseCmd[100];

	sprintf(timeBaseCmd, ":TIMebase:REFerence CENTer");
	_ibwrt(_DSA91304A, timeBaseCmd);

	sprintf(timeBaseCmd, ":TIMebase:SCALe %7.12f", time_scale);
	_ibwrt(_DSA91304A, timeBaseCmd);

	sprintf(timeBaseCmd, ":TIMebase:POSition %7.12f", time_delay);
	_ibwrt(_DSA91304A, timeBaseCmd);
}

void DSA91304A_SetChannel(int channel_num, float volt_scale, float volt_offset)
{
	char ChannelCmd[100];

	sprintf(ChannelCmd, ":CHANnel%d:UNITS VOLT");
	_ibwrt(_DSA91304A, ChannelCmd);

	sprintf(ChannelCmd, ":CHANnel%d:OFFSet %7.8f", channel_num, volt_offset);
	_ibwrt(_DSA91304A, ChannelCmd);

	sprintf(ChannelCmd, ":CHANnel%d:SCALe %7.8f", channel_num, volt_scale);
	_ibwrt(_DSA91304A, ChannelCmd);
}

float DSA91304A_Meas_VAVG(int channel_num)
{
	char RdBuffer[101];
	char AvgCmd[100];

	float Vaverage;

	// Measure Average
	sprintf(AvgCmd, ":MEASure:VAVerage DISPlay, CHANnel%d", channel_num);
	_ibwrt(_DSA91304A, AvgCmd);

	::Sleep(10);

	sprintf(AvgCmd, ":MEASure:VAVerage? DISPlay, CHANnel%d", channel_num);
	_ibwrt(_DSA91304A, AvgCmd);

	_ibrd(_DSA91304A, RdBuffer, 100);
	RdBuffer[ibcntl] = '\0';         // Null terminate the ASCII string    

	//	strcpy(arg,RdBuffer);

	sscanf(RdBuffer, "%f", &Vaverage);

	return Vaverage;
}

void DSA91304A_Meas_Freq(char *arg, int channel_num)
{
	char RdBuffer[101];
	char FreqCmd[100];

	// Measure Frequency
	sprintf(FreqCmd, ":MEASure:FREQuency CHANnel%d, RISing", channel_num);
	_ibwrt(_DSA91304A, FreqCmd);

	::Sleep(10);

	sprintf(FreqCmd, ":MEASure:FREQuency? CHANnel%d, RISing", channel_num);
	_ibwrt(_DSA91304A, FreqCmd);

	_ibrd(_DSA91304A, RdBuffer, 100);
	RdBuffer[ibcntl] = '\0';         // Null terminate the ASCII string    

	strcpy(arg, RdBuffer);
}

void DSA91304A_Set_ACQ_Modes(int numPoints) {
	char Cmd[100];
	sprintf(Cmd, ":ACQuire:POINts %d", numPoints);

	_ibwrt(_DSA91304A, ":ACQuire:AVERage OFF");
	_ibwrt(_DSA91304A, ":ACQuire:MODE HRESolution");
	_ibwrt(_DSA91304A, ":ACQuire:BANDwidth AUTO");
	_ibwrt(_DSA91304A, ":ACQuire:INTerpolate ON");
	_ibwrt(_DSA91304A, ":ACQuire:HRESolution AUTO");
	_ibwrt(_DSA91304A, ":ACQuire:SRATe 5E9");
	_ibwrt(_DSA91304A, Cmd);
}

void DSA91304A_Set_ACQ_AUTO_Modes() {

	_ibwrt(_DSA91304A, ":ACQuire:AVERage OFF");
	_ibwrt(_DSA91304A, ":ACQuire:MODE HRESolution");
	_ibwrt(_DSA91304A, ":ACQuire:BANDwidth AUTO");
	_ibwrt(_DSA91304A, ":ACQuire:INTerpolate ON");
	_ibwrt(_DSA91304A, ":ACQuire:HRESolution AUTO");
	_ibwrt(_DSA91304A, ":ACQuire:SRATe 5E9");
	_ibwrt(_DSA91304A, ":ACQuire:POINts:AUTO ON");
}

void DSA91304A_Set_ClkFreq_ACQ_Modes(int numPoints) {
	char Cmd[100];
	sprintf(Cmd, ":ACQuire:POINts %d", numPoints);

	_ibwrt(_DSA91304A, ":ACQuire:AVERage OFF");
	_ibwrt(_DSA91304A, ":ACQuire:MODE RTIMe");
	_ibwrt(_DSA91304A, ":ACQuire:BANDwidth AUTO");
	_ibwrt(_DSA91304A, ":ACQuire:INTerpolate ON");
	_ibwrt(_DSA91304A, ":ACQuire:HRESolution AUTO");
	_ibwrt(_DSA91304A, ":ACQuire:SRATe 40E9");
	_ibwrt(_DSA91304A, Cmd);
}

void DSA91304A_Set_ClkFreq_ACQ_AUTO_Modes() {

	_ibwrt(_DSA91304A, ":ACQuire:AVERage OFF");
	_ibwrt(_DSA91304A, ":ACQuire:MODE RTIMe");
	_ibwrt(_DSA91304A, ":ACQuire:BANDwidth AUTO");
	_ibwrt(_DSA91304A, ":ACQuire:INTerpolate ON");
	_ibwrt(_DSA91304A, ":ACQuire:HRESolution AUTO");
	_ibwrt(_DSA91304A, ":ACQuire:SRATe 20E9");
	_ibwrt(_DSA91304A, ":ACQuire:POINts:AUTO ON");
}

void DSA91304A_Digitize(int numPoints) {
	char Cmd[101];
	sprintf(Cmd, ":ACQuire:POINts %d", numPoints);
	// Capture Data
	_ibwrt(_DSA91304A, ":ACQuire:COMPLete 100");
	_ibwrt(_DSA91304A, Cmd);
	_ibwrt(_DSA91304A, ":WAVeform:FORMat ASCii");
	_ibwrt(_DSA91304A, ":DIGITIZE");
	printf("DIGITIZE Done\n");

}

void DSA91304A_SaveWaveToDisk(char *FileName, int wait) {
	char Cmd[200];
	char RdBuffer[2];
	int Done;
	sprintf(Cmd, ":DISK:SAVE:WAVeform ALL, \"%s\", CSV; *OPC", FileName);
	_ibwrt(_DSA91304A, Cmd);

	::Sleep(wait);

	//	_ibwrt(_DSA91304A, "*OPC?");
	//	_ibrd(_DSA91304A, RdBuffer, 1);
	//	printf("OPC is %s\n", RdBuffer[0]);
	//	RdBuffer[1] = '\0';

	//	sscanf(RdBuffer, "%d", &Done);

	//	if (Done == 1) {
	printf("Saving Waveform to Disk Done....\n");
	//	}
}

void DSA91304A_SaveHistToDisk(char *FileName) {
	char Cmd[300];
	char RdBuffer[2];
	int Done;
	sprintf(Cmd, ":DISK:SAVE:WAVeform HISTogram, \"%s\", CSV; *OPC", FileName);
	_ibwrt(_DSA91304A, Cmd);

	_ibwrt(_DSA91304A, "*OPC?");
	_ibrd(_DSA91304A, RdBuffer, 1);
	RdBuffer[1] = '\0';

	sscanf(RdBuffer, "%d", &Done);

	if (Done == 1) {
		printf("Saving Histogram to Disk Done....\n");
	}
}
void DSA91304A_CaptureWave(char *Vout1Dump, char *Vout2Dump, char *Vout3Dump) {
	char Cmd[101];
	char *RdBuffer = new char[5000001];

	// Capture VOUT1
	_ibwrt(_DSA91304A, ":WAVeform:SOURce CHANnel1");
	_ibwrt(_DSA91304A, ":WAVeform:DATA?");
	printf("Waveform:DATA Done\n");
	_ibrd(_DSA91304A, RdBuffer, 5000000);
	printf("Reading into Buffer Done\n");
	RdBuffer[ibcntl] = '\0';
	printf("ibcntl = %d\n", ibcntl);
	strncpy(Vout1Dump, RdBuffer, 5000001);
	printf("Reading VOUT1 Done\n");
	::Sleep(2500);

	// Capture VOUT2
	_ibwrt(_DSA91304A, ":WAVeform:SOURce CHANnel2");
	_ibwrt(_DSA91304A, ":WAVeform:DATA?");
	printf("Waveform:DATA Done\n");
	_ibrd(_DSA91304A, RdBuffer, 5000000);
	printf("Reading into Buffer Done\n");
	RdBuffer[ibcntl] = '\0';
	printf("ibcntl = %d\n", ibcntl);
	strncpy(Vout2Dump, RdBuffer, 5000001);
	printf("Reading VOUT2 Done\n");
	::Sleep(2500);

	// Capture VOUT3
	_ibwrt(_DSA91304A, ":WAVeform:SOURce CHANnel3");
	_ibwrt(_DSA91304A, ":WAVeform:DATA?");
	printf("Waveform:DATA Done\n");
	_ibrd(_DSA91304A, RdBuffer, 5000000);
	printf("Reading into Buffer Done\n");
	RdBuffer[ibcntl] = '\0';
	printf("ibcntl = %d\n", ibcntl);
	strncpy(Vout3Dump, RdBuffer, 5000001);
	printf("Reading VOUT3 Done\n");
	::Sleep(2500);
}

void DSA91304A_CaptureClkWave(char *ClkBotDump, char *ClkMBotDump, char *ClkMTopDump, char *ClkTopDump) {
	char Cmd[101];
	char *RdBuffer = new char[10000001];

	// Capture BotClk
	_ibwrt(_DSA91304A, ":WAVeform:SOURce CHANnel1");
	_ibwrt(_DSA91304A, ":WAVeform:DATA?");
	printf("Waveform:DATA Done\n");
	//	::Sleep(5000);
	_ibrd(_DSA91304A, RdBuffer, 10000000);
	printf("Reading into Buffer Done\n");
	RdBuffer[ibcntl] = '\0';
	printf("ibcntl = %d\n", ibcntl);
	strncpy(ClkBotDump, RdBuffer, 10000001);
	printf("Reading BOTCLK Done\n");

	// Capture MBotClk
	_ibwrt(_DSA91304A, ":WAVeform:SOURce CHANnel2");
	_ibwrt(_DSA91304A, ":WAVeform:DATA?");
	printf("Waveform:DATA Done\n");
	//	::Sleep(5000);
	_ibrd(_DSA91304A, RdBuffer, 10000000);
	printf("Reading into Buffer Done\n");
	RdBuffer[ibcntl] = '\0';
	printf("ibcntl = %d\n", ibcntl);
	strncpy(ClkMBotDump, RdBuffer, 10000001);
	printf("Reading MBOTCLK Done\n");

	// Capture MTopClk
	_ibwrt(_DSA91304A, ":WAVeform:SOURce CHANnel3");
	_ibwrt(_DSA91304A, ":WAVeform:DATA?");
	printf("Waveform:DATA Done\n");
	//	::Sleep(5000);
	_ibrd(_DSA91304A, RdBuffer, 10000000);
	printf("Reading into Buffer Done\n");
	RdBuffer[ibcntl] = '\0';
	printf("ibcntl = %d\n", ibcntl);
	strncpy(ClkMTopDump, RdBuffer, 10000001);
	printf("Reading MTOPCLK Done\n");

	// Capture TopClk
	_ibwrt(_DSA91304A, ":WAVeform:SOURce CHANnel4");
	_ibwrt(_DSA91304A, ":WAVeform:DATA?");
	printf("Waveform:DATA Done\n");
	//	::Sleep(5000);
	_ibrd(_DSA91304A, RdBuffer, 10000000);
	printf("Reading into Buffer Done\n");
	RdBuffer[ibcntl] = '\0';
	printf("ibcntl = %d\n", ibcntl);
	strncpy(ClkTopDump, RdBuffer, 10000001);
	printf("Reading TOPCLK Done\n");

	delete(RdBuffer);

}

void DSA91304A_gen_csv(char *Vout1Dump, char *Vout2Dump, char *Vout3Dump, char *csv_fn, int numPoints) {

	int i, j;

	FILE *fc;

	int v1_index = 0;
	int v2_index = 0;
	int v3_index = 0;

	fc = fopen(csv_fn, "w");

	for (i = 0; i < numPoints; i++) {

		while (Vout1Dump[v1_index] != ',') {
			fprintf(fc, "%c", Vout1Dump[v1_index]);
			v1_index++;
		}
		fprintf(fc, ",");
		v1_index++;
		while (Vout2Dump[v2_index] != ',') {
			fprintf(fc, "%c", Vout2Dump[v2_index]);
			v2_index++;
		}
		fprintf(fc, ",");
		v2_index++;
		while (Vout3Dump[v3_index] != ',') {
			fprintf(fc, "%c", Vout3Dump[v3_index]);
			v3_index++;
		}
		fprintf(fc, "\n");
		v3_index++;
	}
	fclose(fc);
	printf("Writing to CSV Done\n");
}

void DSA91304A_gen_clk_csv(char *ClkBotDump, char *ClkMBotDump, char *ClkMTopDump, char *ClkTopDump, char *csv_fn, int numPoints) {

	int i, j;

	FILE *fc;

	int index1 = 0;
	int index2 = 0;
	int index3 = 0;
	int index4 = 0;

	fc = fopen(csv_fn, "w");

	for (i = 0; i < numPoints; i++) {
		while (ClkBotDump[index1] != ',') {
			fprintf(fc, "%c", ClkBotDump[index1]);
			index1++;
		}
		fprintf(fc, ",");
		index1++;
		while (ClkMBotDump[index2] != ',') {
			fprintf(fc, "%c", ClkMBotDump[index2]);
			index2++;
		}
		fprintf(fc, ",");
		index2++;
		while (ClkMTopDump[index3] != ',') {
			fprintf(fc, "%c", ClkMTopDump[index3]);
			index3++;
		}
		fprintf(fc, ",");
		index3++;
		while (ClkTopDump[index4] != ',') {
			fprintf(fc, "%c", ClkTopDump[index4]);
			index4++;
		}
		fprintf(fc, "\n");
		index4++;
	}
	fclose(fc);
	printf("Writing to CSV Done\n");
}

void DSA91304A_SetHistMode(char *mode)
{
	char Cmd[200];
	sprintf(Cmd, ":HISTogram:MODE %s", mode);
	_ibwrt(_DSA91304A, Cmd);
}

void DSA91304A_SetJitterMeasNum(int measNum)
{
	char Cmd[200];
	sprintf(Cmd, ":MEASure:JITTer:MEASurement MEASurement%d", measNum);
	_ibwrt(_DSA91304A, Cmd);
}

void DSA91304A_MeasJitterEnable()
{
	char Cmd[200];
	sprintf(Cmd, ":MEASure:JITTer:HISTogram ON");
	_ibwrt(_DSA91304A, Cmd);
}

void DSA91304A_MeasJitterDisable()
{
	char Cmd[200];
	sprintf(Cmd, ":MEASure:JITTer:HISTogram OFF");
	_ibwrt(_DSA91304A, Cmd);
}

void DSA91304A_MeasJitterStats(double *stats)
{
	char RdBuffer[100];

	double Hits;
	double Mean;
	double Median;
	double Mode;
	double stddev;
	double m1s;
	double m2s;
	double m3s;
	double max;
	double min;
	double peak;
	double pp;

	DSA91304A_ScopeHeadOff();

	// Measure Hits
	_ibwrt(_DSA91304A, ":MEASure:HISTogram:HITS? HISTogram; *OPC");

	_ibrd(_DSA91304A, RdBuffer, 100);
	RdBuffer[ibcntl] = '\0';         // Null terminate the ASCII string    

	sscanf(RdBuffer, "%lf", &Hits);

	::Sleep(100);

	// Measure Mean
	_ibwrt(_DSA91304A, ":MEASure:HISTogram:MEAN? HISTogram; *OPC");

	_ibrd(_DSA91304A, RdBuffer, 100);
	RdBuffer[ibcntl] = '\0';         // Null terminate the ASCII string    

	sscanf(RdBuffer, "%lf", &Mean);
	::Sleep(100);

	// Measure Median
	_ibwrt(_DSA91304A, ":MEASure:HISTogram:MEDian? HISTogram; *OPC");

	_ibrd(_DSA91304A, RdBuffer, 100);
	RdBuffer[ibcntl] = '\0';         // Null terminate the ASCII string    

	sscanf(RdBuffer, "%lf", &Median);
	::Sleep(100);

	// Measure Mode
	_ibwrt(_DSA91304A, ":MEASure:HISTogram:MODE? HISTogram; *OPC");

	_ibrd(_DSA91304A, RdBuffer, 100);
	RdBuffer[ibcntl] = '\0';         // Null terminate the ASCII string    

	sscanf(RdBuffer, "%lf", &Mode);
	::Sleep(100);

	// Measure StdDev
	_ibwrt(_DSA91304A, ":MEASure:HISTogram:STDDev? HISTogram; *OPC");

	_ibrd(_DSA91304A, RdBuffer, 100);
	RdBuffer[ibcntl] = '\0';         // Null terminate the ASCII string    

	sscanf(RdBuffer, "%lf", &stddev);
	::Sleep(100);

	// Measure M1S
	_ibwrt(_DSA91304A, ":MEASure:HISTogram:M1S? HISTogram; *OPC");

	_ibrd(_DSA91304A, RdBuffer, 100);
	RdBuffer[ibcntl] = '\0';         // Null terminate the ASCII string    

	sscanf(RdBuffer, "%lf", &m1s);
	::Sleep(100);

	// Measure M2S
	_ibwrt(_DSA91304A, ":MEASure:HISTogram:M2S? HISTogram; *OPC");

	_ibrd(_DSA91304A, RdBuffer, 100);
	RdBuffer[ibcntl] = '\0';         // Null terminate the ASCII string    

	sscanf(RdBuffer, "%lf", &m2s);
	::Sleep(100);

	// Measure M3S
	_ibwrt(_DSA91304A, ":MEASure:HISTogram:M3S? HISTogram; *OPC");

	_ibrd(_DSA91304A, RdBuffer, 100);
	RdBuffer[ibcntl] = '\0';         // Null terminate the ASCII string    

	sscanf(RdBuffer, "%lf", &m3s);
	::Sleep(100);

	// Measure MAX
	_ibwrt(_DSA91304A, ":MEASure:HISTogram:MAX? HISTogram; *OPC");

	_ibrd(_DSA91304A, RdBuffer, 100);
	RdBuffer[ibcntl] = '\0';         // Null terminate the ASCII string    

	sscanf(RdBuffer, "%lf", &max);
	::Sleep(100);

	// Measure MIN
	_ibwrt(_DSA91304A, ":MEASure:HISTogram:MIN? HISTogram; *OPC");

	_ibrd(_DSA91304A, RdBuffer, 100);
	RdBuffer[ibcntl] = '\0';         // Null terminate the ASCII string    

	sscanf(RdBuffer, "%lf", &min);
	::Sleep(100);

	// Measure PEAK
	_ibwrt(_DSA91304A, ":MEASure:HISTogram:PEAK? HISTogram; *OPC");

	_ibrd(_DSA91304A, RdBuffer, 100);
	RdBuffer[ibcntl] = '\0';         // Null terminate the ASCII string    

	sscanf(RdBuffer, "%lf", &peak);
	::Sleep(100);

	// Measure PP
	_ibwrt(_DSA91304A, ":MEASure:HISTogram:PP? HISTogram; *OPC");

	_ibrd(_DSA91304A, RdBuffer, 100);
	RdBuffer[ibcntl] = '\0';         // Null terminate the ASCII string    

	sscanf(RdBuffer, "%lf", &pp);
	::Sleep(100);

	// Pack stats register
	stats[0] = Hits;
	stats[1] = Mean;
	stats[2] = Median;
	stats[3] = Mode;
	stats[4] = stddev;
	stats[5] = m1s;
	stats[6] = m2s;
	stats[7] = m3s;
	stats[8] = max;
	stats[9] = min;
	stats[10] = pp;
	stats[11] = peak;
}

void DSA91304A_TurnONAllChannel() {
	_ibwrt(_DSA91304A, ":CHANnel1:DISPlay ON");
	_ibwrt(_DSA91304A, ":CHANnel2:DISPlay ON");
	_ibwrt(_DSA91304A, ":CHANnel3:DISPlay ON");
	_ibwrt(_DSA91304A, ":CHANnel4:DISPlay ON");
}

void DSA91304A_TurnOFFAllChannel() {
	_ibwrt(_DSA91304A, ":CHANnel1:DISPlay OFF");
	_ibwrt(_DSA91304A, ":CHANnel2:DISPlay OFF");
	_ibwrt(_DSA91304A, ":CHANnel3:DISPlay OFF");
	_ibwrt(_DSA91304A, ":CHANnel4:DISPlay OFF");
}

void DSA91304A_TurnONOnlyChannel(int channelNum) {
	char Cmd[200];
	_ibwrt(_DSA91304A, ":CHANnel1:DISPlay OFF");
	_ibwrt(_DSA91304A, ":CHANnel2:DISPlay OFF");
	_ibwrt(_DSA91304A, ":CHANnel3:DISPlay OFF");
	_ibwrt(_DSA91304A, ":CHANnel4:DISPlay OFF");
	sprintf(Cmd, ":CHANnel%d:DISPlay ON", channelNum);
	_ibwrt(_DSA91304A, Cmd);
}

*/
