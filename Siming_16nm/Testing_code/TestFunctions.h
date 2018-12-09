/******************************************
Header File for all Basic Test Functions
*******************************************/

#ifndef TESTFUNCTIONS_H
#define TESTFUNCTIONS_H
#include <ostream>
#include <conio.h>
#include <strstream>
#include <assert.h>
#include <comdef.h>
#include <math.h>
#include <time.h>
#include <visa.h>

#include <stdio.h>
#include <windows.h>
#include <winbase.h>
#include "WAModbus.h"


//***** DSA91304 VISA address and Global Variables *******
#define DSA91304A_VISA_ADDRESS "USB0::0x0957::0x9001::MY48130009::0::INSTR"
#define IEEEBLOCK_SPACE 5000000

extern ViSession defaultRM, DSA91304A_vi;  // DSA91304A session ID.
extern ViStatus err;						// VISA function return value.
extern char DSA91304A_str_result[256];

//****** Test Chip Number (for Socket testing) ********
extern int	 chipNum;

//***** GPIB Device Address Variables *******
extern int   PrimaryAddress_VDD_DIG_VDD_WL;
extern int   PrimaryAddress_VSS_WL_VSS_PW;
extern int   PrimaryAddress_VDD_IO_V_NIDAQ;	
extern int   PrimaryAddress_VSPARE_VAB;
extern int   PrimaryAddress_MM34401A;
extern int   PrimaryAddress_MM34410A_6;
extern int   PrimaryAddress_MM34401A_7;
extern int   PrimaryAddress_ELTM6514; //electrometer 6514 borrowed from Jim McArther

extern int   SecondaryAddress;       

//extern int   _DSA91304A;			
extern int   _VDD_DIG_VDD_WL;	
extern int   _VSS_WL_VSS_PW;
extern int   _VDD_IO_V_NIDAQ;
extern int   _VSPARE_VAB;     
extern int   _MM34401A;     // the first DMM measuring ID is 34401A, GPIB 0
extern int   _MM34410A_6;  // the second DMM has better resolutions, measuring Isub: 34410A, GPIB 6
extern int   _MM34401A_7;  // the third DMM is borrowed from Xuan, measuring Is, GPIB 7
extern int   _ELTM6514;    // the electrometer measures the substrate current during charge pumping measurement


/***parameters specific to 16FF+ eNVM******/
extern int Num_of_row[36];
extern double VDD_typical;
/***parameters specific to 16FF+ eNVM******/

extern int   _VDD_DIG;
extern int   _VDD_WL; 
extern int   _VSS_WL; 
extern int   _VSS_PW;
extern int   _VDD_IO;
extern int   _V_NIDAQ;
extern int   _VSPARE;
extern int   _VAB;

extern int   BoardIndex;               		/* Interface Index (GPIB0=0,GPIB1=1,etc.)  */

//****** MSO6104A enable *********
extern int	  MSO6104A_present;

//****** MM34401A enable *********
extern int	  MM34401A_present;

//****** ELTM6514 enable *********
extern int    ELTM6514_present;

//****** ScanChainRead Error *********
extern int scanChainRead_Error;

//***** Scan Functions and Attributes *******
#define NUM_OUTPUT 8        // using PCI-NIDAQ 8 DO lines
#define NUM_OUTPUT_USB6008 10        // using USB-NIDAQ 8 DO lines
#define MAX_LINES 20000      // each scan bit needs NIDAQ DO lines to scan in 4 times (non-overlapping PHI_1, PHI_2)
//#define MAX_LINES 1000020
#define NUM_SCAN_BITS 4608  // not used, 36 Column's * 128 WL's
/*
#define MAX_LINES 50000 
#define NUM_SCAN_BITS 9068
#define CHIP_MEM_LINES 1024
#define CORE_COUNT 16
*/
#define FAIL 1

#define VDD_NIDAQ 5.0 //NIDAQ and VH of the level shifter is always 5V => SOUT after level shifter

extern char outchannel_str[];  //PCI-NIDAQ
extern char inchannel_str[];   //PCI-NIDAQ
extern char outchannel_str_USB6008[];  //USB-NIDAQ


//***************** Temperature chamber extra Modbus_functions.cpp *************
//***************** majorities declared in WAModbus.H *************

// Structure to store setting relevant to Reading or Writing daa
typedef struct tagModBusTag
{
	short slaveAddress;		// comm line address, range: 1 to 247
	long startAddress;		// Memory address of parameter in device range:1-499999
	short numPoints;		// # consequetive items to read or write from startAddress
	short data[MAX_MODBUS_DATA_LENGTH - 9];	// array containing data read or data to write
	short errorCode;		// MODBUS return error code
	short function;			// For DLL internal use only. Will be set to appropriate MODBUS function code
}ModBusBag;



//////////////////////////////
// Communication port settings
typedef struct tagPortSetup
{
	char portStr[80];		// "COM1", "COM2", or other device name.
	long baudRate;			// 2400, 9600, 19200, etc.
	short parity;			// 0=None, 1=Odd, 2=Even
}PortSetup;


// Here are some possible internal data addresses you may want to use.
// These correspond to memory locations inside the MODBUS device.
//
// WATLOW F4 parameter addresses
#define F4_PV		 40101		// F4 Input Reading MODBUS address
#define F4_SETPOINT  40301		// F4 Set Point MODBUS address

void printResponse(char *Measure_file, ModBusBag * mb);
//BOOL pollProc(DWORD *parmPtr);
//BOOL startThread(void);
int write_SP(char *Measure_file, short room_temperature, short bake_temperature, DWORD bake_time);


//***************** Nit, Nox characterizations *************
int Charge_Pumping(char* Measure_file, double VD, double VB, double VS, double VDD_DIG, double VSS_WL, double VDD_WL, char* scan_file, double samp_rate, int Num_of_ExtTrig, int Num_of_Sample, double Trig_Delay, int chip, int col, int direction, int Isub_Rsense);
int Charge_Pumping_ELTM(char* Measure_file, double* VDBS_list_Vr0, int Num_of_VDBS, double VDD_DIG, double VSS_WL, double VDD_WL, char* scan_file, double samp_rate, int Num_of_freq, double* pumping_freq, int Num_of_ExtTrig, int chip, int col, int direction);
int RTN_ID_MM34410(char* Measure_file, double VDS, double VGS, char* scan_file_name, int chip, int col, int direction, int Num_of_ExtTrig, float NPLCycles);

//************ I-V curve measurement ***************
int IDS_sweepVG(char *f_name, int col, int chip, int direction, float VD, float VS, float VB, float VGmin, float VGmax);
int IDS_sweepVD(char *f_name, int col, int chip, int direction, float VG, float VS, float VB, float VDmin, float VDmax);
int IDS_diode(char *f_name, int col, int chip, int direction);
int IDS_VGS(char *f_name, int col, int chip, int direction);
int IDS_VDS(char* f_name, int col, int chip, int direction);
int Col_punchthrough(char* f_name, double VS, double VB, double VG, int col, int chip, int direction, int MUX_ON);
int Drain_leakage(char* f_name, double VS, double VB, double VG, int col, int chip, int direction, int MUX_ON);
int FN_Erase_leakage(char* f_name, double VSVBVD_max, double VG, double VDD_WL, int col, int chip, int direction, int MUX_ON);
int VG_realvalue(char* f_name, double VDD_WL_max, int col, int chip, int direction);
int ALL_IDSAT(char* Measure_file, int chip, int col, int direction);

//************ MUX address selection ***************
int DO_USB6008(char *fn_scanin);
int MUX_Delay_DO_USB6008(char *fn_scanin, int Delay_ms);

//************ Carrier injection ***************
int stress_checkerboard(int even_odd, double VDS, double VGS, double pulse_width, int chip, int col, int direction);
int stress_sweep_VGS_VDS(double pulse_width, int chip, int col, int direction);
int stress_Ext_Imeas_1by1(char* Measure_file, double VDS, double VGS, char* pulse_width_char, int Num_of_ExtTrig, int chip, int col, int direction, int Num_of_Pulse);
int stress_VG_RampPulse_Isub(char* Measure_file, double* VDS, double* VGS, char* pulse_width_char, int chip, int col, int direction, int Num_of_Pulse);
int stress_VG_ConstPulse(char* Measure_file, double* VDS, double* VGS, char* pulse_width_char, int chip, int col, int direction, int Num_of_Pulse);
int us_stress_VG_ConstPulse(char* Measure_file, double* VDS, double* VGS, char* pulse_width_char, int is_us, int ExtTrig, int chip, int col, int direction, int Num_of_Pulse);
int Block_FN_tunnel(char* Measure_file, double VDD_DIG, double VSS_WL, double VDD_WL, char* pulse_width_char, int Num_of_ExtTrig, int chip, int col, int direction);
int PBTI_VG_ConstPulse(char* Measure_file, double VDD_WL, double VSS_WL, double VDD_DIG, char* pulse_width_char, int chip, int col, int direction, int Num_of_Pulse, int Num_of_trigger);
int Stacked_VG_ConstPulse(char* Measure_file, double VDD_WL, double VSS_WL, double VDD_DIG, double* VDS, char* pulse_width_char, int chip, int col, int direction, int Num_of_Pulse, int Num_of_trigger);

int MLC_programming(char* Measure_file, double VDS, double VGS, char* pulse_width_char, int chip, int col, int direction, int Max_Num_of_Pulse, int Max_Num_of_Pulse_round2, double IDSAT_threshold);

//************ Erase ***************
int Block_GateErase(char* Measure_file, double VDD_DIG, double VSS_WL, double VDD_WL, char* pulse_width_char, int Num_of_ExtTrig, int chip, int col, int direction);
int erase_VSDB_tunneling(char* Measure_file, double VSDB, double VDD_DIG_WL, char* pulse_width_char, int chip, int col, int direction, int Num_of_Pulse);
int Block_Erase(char* Measure_file, double VD, double VB, double VS, double VDD_DIG, double VSS_WL, double VDD_WL, char* pulse_scan_file, int Num_of_ExtTrig, int chip, int col, int direction, int Erase_Cycle);
//int Block_Erase(char* Measure_file, double VD, double VB, double VS, double VDD_DIG_WL, char* pulse_width_char, int Num_of_ExtTrig, int chip, int col, int direction, int Erase_Cycle);
//int Block_Erase(char* Measure_file, double VSDB, double VDD_DIG_WL, char* pulse_width_char, int Num_of_ExtTrig, int chip, int col, int direction);
//int Block_Erase(char* Measure_file, double VS_D, double VG, double VPW_ERASE, char* pulse_width_char, int Num_of_ExtTrig, int chip, int col, int direction);
int Erase_VG_ConstPulse(char* Measure_file, double VDS, double VGS, char* pulse_width_char, int chip, int col, int direction, int Num_of_Pulse, int Num_of_Trigger);

//************ Scan Functions *****************
int long_scan(char *fn_scanin, float samp_freq);
int scan_selfcheck(char *fn_scanin, int compareMode);
int scan(char *fn_scanin, int compareMode, float samp_freq);
int scanInternalRead(char *fn_scanin, int *RegsOut);
int scanFileChk(char *fn_scanin);

//***** GPIB Functions and Attributes ********
void _ibwrt(int DeviceId, char *command);
void _ibrd (int DeviceId, char *command, long num_bytes);
void GpibError(int DeviceId, char *msg);
int GpibInitDev(int PrimaryAddress);
void GpibUnInitDev(int DeviceId);
void GpibSetLocalDev(int DeviceId);
void GpibGetIDN(int DeviceId);
void GpibResetDev(int DeviceId);
void GpibClrStDev(int DeviceId);

void GpibEquipmentInit();
void GpibEquipmentSetLocal();
void GpibEquipmentUnInit();

//************ Power Supply Functions *****************
void A6611C_SetVoltage(int DeviceId, double Voltage);
void A6611C_AdjVoltage(int DeviceId, double StartVal, double EndVal, int numSteps);
float A6611C_QueryVoltage(int DeviceId);
void DevicePowerDn (int DeviceId, int OutputNo, double EndVal);
void E3646A_SetVoltage(int DeviceId, int OutputNo, double Voltage);
void E3646A_SetVoltage_CurrentLimit(int DeviceId, int OutputNo, double Voltage, double CurrentLimit);
void E3646A_AdjVoltage(int DeviceId, int OutputNo, double StartVal, double EndVal, int numSteps);
void E3646A_PrintVoltage (int DeviceId, int OutputNo, double Voltage);
void E3646A_Raise_StackVoltage(double StartStackVol, int numStep);
void E3646A_Lower_StackVoltage(double StartStackVol, int numStep);
void E3646A_Raise_InternalVoltage(double StartStackVol, int numStep);
void E3646A_Lower_InternalVoltage(double StartStackVol, int numStep);
void E3646A_Raise_VinVout1Voltage(double StartStackVol, int numStep);
void E3646A_Lower_VinVout1Voltage(double StartStackVol, int numStep);
void E3646A_Raise_VinVoltage(double StartStackVol, int numStep);
void E3646A_Lower_VinVoltage(double StartStackVol, int numStep);
void E3646A_Raise_AllSameVoltage(double StartStackVol, int numStep);
void E3646A_Lower_AllSameVoltage(double StartStackVol, int numStep);
float E3646A_MeasAvgCurrent(int DeviceId, int OutputNo);
float E3646A_MeasAvgVoltage(int DeviceId, int OutputNo);
float E3646A_QueryVoltage(int DeviceId, int OutputNo);
void E3646A_Report_AllVoltage();

//************ Pulse Generator Functions ****************
void DTG5334_SetClkMode();
void DTG5334_SetDataMode();
void DTG5334_TurnOn_Channel(int ChannelNum);
void DTG5334_TurnOff_Channel(int ChannelNum);
void DTG5334_SetChannelLow(int ChannelNum, float Low);
void DTG5334_SetChannelHigh(int ChannelNum, float High);
void DTG5334_ClockOn();
void DTG5334_StopClock();
void DTG5334_SetClkFreq(float ClockFreq);
void DTG5334_IVRClkEn(float ClockFreq);
void DTG5334_SystemClkEn(float ClockFreq);

//************ DSA91034A Oscilloscope Functions (VISA Interface) *******************
void DSA91304A_error_handler();
void DSA91304A_open_session();
void DSA91304A_close_session();
void DSA91304A_doCommand(char *command);
void DSA91304A_do_query_string(char *query);
void DSA91304A_do_query_number(char *query, double num_result);
void DSA91304A_read_number(double num_result);
void DSA91304A_ScopeHeadOff();
void DSA91304A_ScopeRun();
void DSA91304A_ScopeStop();
void DSA91304A_ScopeSingle();
void DSA91304A_SetTrigAuto();
void DSA91304A_SetTrigTrig();
void DSA91304A_SetTrigMode(int  channelNum, double trigLevel);
void DSA91304A_SetSingleCapture(int channelNum, double trigLevel);
void DSA91304A_SetAutoRun();
void DSA91304A_SetTimeBase_Range(double time_range, double time_delay);
void DSA91304A_SetTimeBase_Scale(double time_scale, double time_delay);
void DSA91304A_SetChannel(int channel_num, float volt_scale, float volt_offset);
void DSA91304A_ClearDisplay();
double DSA91304A_Meas_VAVG(int channel_num);
double DSA91304A_Meas_Freq(int channel_num);
double DSA91304A_Meas_VMax(int channel_num);
//void DSA91304A_Set_ACQ_Modes(int numPoints);
void DSA91304A_Set_ACQ_AUTO_Modes();
//void DSA91304A_Set_ClkFreq_ACQ_Modes(int numPoints);
void DSA91304A_Set_ClkFreq_ACQ_AUTO_Modes();
//void DSA91304A_Digitize(int numPoints);
void DSA91304A_SaveWaveToDisk(char *FileName, int wait);
void DSA91304A_SaveHistToDisk(char *FileName);
//void DSA91304A_CaptureWave(char *Vout1Dump, char *Vout2Dump, char *Vout3Dump);
//void DSA91304A_CaptureClkWave(char *ClkBotDump, char *ClkMBotDump, char *ClkMTopDump, char *ClkTopDump);
//void DSA91304A_gen_csv(char *Vout1Dump, char *Vout2Dump, char *Vout3Dump, char *csv_fn, int numPoints);
//void DSA91304A_gen_clk_csv(char *ClkBotDump, char *ClkMBotDump, char *ClkMTopDump, char *ClkTopDump, char *csv_fn, int numPoints);
void DSA91304A_SetHistMode(char *mode);
void DSA91304A_SetJitterMeasNum(int measNum);
void DSA91304A_MeasJitterEnable();
void DSA91304A_MeasJitterDisable();
void DSA91304A_MeasJitterStats(double *stats);
void DSA91304A_TurnONAllChannel();
void DSA91304A_TurnOFFAllChannel();
void DSA91304A_TurnONOnlyChannel(int channelNum);
void DSA91304A_SetMeasFreqAllChannel();

//************ MSO6104A Oscilloscope Functions *******************
void MSO6104A_ScopeHeadOff();
void MSO6104A_ScopeRun();
void MSO6104A_ScopeStop();
void MSO6104A_ClearDisplay();
void MSO6104A_SetTrigAuto();
void MSO6104A_SetTrigTrig();
void MSO6104A_SetAutoRun();
void MSO6104A_SetTrigMode(int channelNum, double trigLevel);
void MSO6104A_ScopeSingle();
void MSO6104A_SetSingleCapture(int channelNum, double trigLevel);
void MSO6104A_SetTimeBase_Range(double time_range, double time_delay);
void MSO6104A_SetTimeBase_Scale(double time_scale, double time_delay);
void MSO6104A_SetChannel(int channel_num, float volt_scale, float volt_offset);
float MSO6104A_Meas_VAVG(int channel_num);


void MemoryLoad(char *mem_scan_fn);
void MemoryLoadScanFileChk(char *mem_scan_fn, char *err_filename);
void MemoryRead(char *mem_scan_read_dir, int *CENYDump, int *GWENYDump, int *WENYDump, int *AYDump, int *DYDump, int *MemoryDump);
int ConvertBinArraytoInt(int *InputArray, int startIndex, int EndIndex);
void ConvertIntto32bBinArray(int input, int *binArray);
int ConvertHexArraytoInt(int *InputArray);
//int CompareMemFile(char *fn_memfile, int *MemDump, int CoreNum);
void ReadMemFile(char *fn_memfile, int *MemFile);
int CompareMemFile(int *MemFile, int *MemDump, int CoreNum);
void ReadRegFile(char *fn_regfile, int *RegFile);
//int CompareRegFile(char *fn_regfile, int *RegDump, int CoreNum);
int CompareRegFile(int *RegFile, int *RegDump, int CoreNum);
void ReadInternalRegs(int *internalRegs);
void ReadMPRFRegs(int *RegsDump);
void ReadBISTRegs(int *BISTDump);
void ReadEMIRegs(int *EMIDump);
void ReadEMIRegFile(char *fn_regfile, int *RegFile); 
int CompareEMIRegFile(int *RegFile, int *RegDump, int LyrNum);


void ReadVmonFile(char *fn_vmonfile, double *vmonVal);
double convertVmonVal(double vmon_in, double *vmonVal);

//************ FVS Comm Chip Functions *******************
void FVS_Comm_PowerUp_BotIn(int numStep);
void FVS_Comm_PowerDn_BotIn(int numStep);
void FVS_Comm_PowerUp_MidIn(int numStep);
void FVS_Comm_PowerDn_MidIn(int numStep);
void FVS_Comm_PowerUp_TopIn(int numStep);
void FVS_Comm_PowerDn_TopIn(int numStep);
void FVS_Comm_PowerUp_Unified(int inputLayer, int numStep);
void FVS_Comm_PowerDn_Unified(int inputLayer, int numStep);
void FVS_Comm_Raise_StackVoltage(double StartStackVol, double vstep, int numStep);
void FVS_Comm_Lower_StackVoltage(double StartStackVol, double vstep, int numStep);
void FVS_Comm_Raise_VOUT3_VOUT2(double vout1, double StartStackVol, double vstep, int numStep);
void FVS_Comm_Lower_VOUT3_VOUT2(double vout1, double StartStackVol, double vstep, int numStep);

//************ MM34401A Functions *******************
//void MM34401A_MeasCurrent_Config(int NPLCycles);
//void MM34401A_MeasCurrent_Config(float NPLCycles, char* TrigSource = "IMM", float TrigDelay = 0.1, int SampCount = 1, int TrigCount=1);
void MM34401A_MeasCurrent_Config(int Device, float NPLCycles, char* TrigSource, float TrigDelay, int SampCount, int TrigCount);
void MM34410A_6_MeasCurrent_Config(int Device, float NPLCycles, char* TrigSource, float TrigDelay, int SampCount, int TrigCount);
float MM34401A_MeasCurrent(int Device);
////wrong sequence!!!
//char* MM34401A_MeasCurrent_MultiSample();
float MM34401A_MeasResistance(int Device);
float MM34401A_MeasVoltage(int Device);
void MM34410A_6_MeasVoltage_Config(int Device, float NPLCycles, char* TrigSource, float TrigDelay, int SampCount, int TrigCount);
void ELTM6514_MeasCurrent_Config(int Device, int TrigCount, float NPLCycles, char* CURRentRANGe);

#endif


