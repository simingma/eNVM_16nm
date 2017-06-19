

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


//char outchannel_str[] = "Dev1/port0/line0:7";
//char inchannel_str[] = "Dev1/ai0";
//char outchannel_str_USB6008[] = "Dev2/port0/line0:7";

//int scanChainRead_Error = 0;



int scan_selfcheck(char *fn_scanin, int compareMode)
{
	// NIDAQ-6115 (PCI card) 
	// digital output port for timing critical signals
	// "dev1/port0/line0:7": 
	// line0,   line1,   line2,   line3,   line4,                                  line5,       line6,      line7
	// S_IN,    PHI_1,   PHI_2,  PULSE_IN, unused (reserved for DMM ex-trigger),   /CS(unused), /WR(unused) /EN(unused).
	// analog input port "Dev1/ai0" monitor scan out from chip: S_OUT (VDD_IO=1.8V)

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

	// Variables for Error Buffer
	int32 error = 0;
	char errBuff[2048] = { '\0' };

	//Variables for Processing of Input lines (output to chip)
	TaskHandle taskHandleScanin = 0;
	TaskHandle taskHandleScanout = 0;
	uInt8 scaninData[MAX_LINES][NUM_OUTPUT];

	// bit-wise digital line format:
	//	uInt8 scaninData[14] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	int scanoutData[MAX_LINES];

	int total_lines_read = 0;
	int SCAN_EQUAL = 1;

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

			if ((input_char = fgetc(fptr_scanin)) == EOF)
				break;
		}

		if (inputcnt != NUM_OUTPUT){
			printf("Input line %d was not formatted correctly, received %d input characters\n", linecount, inputcnt);
			printf("Data: ");
			for (int i = 0; i<NUM_OUTPUT; i++)
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
	DAQmxErrChk(DAQmxCreateTask("OutputTask", &taskHandleScanin));
	DAQmxErrChk(DAQmxCreateDOChan(taskHandleScanin, outchannel_str, "outchannel", DAQmx_Val_ChanForAllLines));
	//implicit timing means software control, i.e. NIDAQ generate one sample output once it receives a software program command.
	//the timing is undeterministic because the variation in software runtime speed.
	DAQmxCfgImplicitTiming (taskHandleScanin, DAQmx_Val_FiniteSamps, 1);

	DAQmxErrChk(DAQmxCreateTask("ReadTask", &taskHandleScanout));
	//TODO: adjust AI range to increase resolution!
	DAQmxErrChk(DAQmxCreateAIVoltageChan(taskHandleScanout, inchannel_str, "inchannel", DAQmx_Val_Cfg_Default, 0.0, VDD_NIDAQ + 0.1, DAQmx_Val_Volts, NULL));
	//the AI task should have an implicitly allocated buffer to hold all acquired data after StartTask before ReadAnalogF64.

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/
	DAQmxErrChk (DAQmxStartTask(taskHandleScanin));
	DAQmxErrChk (DAQmxStartTask(taskHandleScanout));

	/*********************************************/
	// DAQmx Read and Write Code - Loop
	/*********************************************/
	// if compareMode == 1, scan in twice, and compare the second-time scan out data with scan in data
	for (int scan_times = 0; scan_times <= compareMode; scan_times++){

		for (linecount = 0; linecount < total_lines_read; linecount++){

			//using auto start here:
			DAQmxErrChk(DAQmxWriteDigitalLines(taskHandleScanin, 1, 1, 10.0, DAQmx_Val_GroupByChannel, scaninData[linecount], NULL, NULL));
			// for bit-wise digital format
			//			DAQmxErrChk(DAQmxWriteDigitalU8(taskHandleScanin, 14, 0, 10.0, DAQmx_Val_GroupByChannel, scaninData, NULL, NULL));
			//			DAQmxErrChk(DAQmxStartTask(taskHandleScanin));

			//for (int loop = 0; loop < 100000; loop++);

			if (scan_times == 1){
				// DAQmxReadAnalogF64 read the acquired AI data from internal default buffer into the specified array "anaInputBuffer"
				DAQmxErrChk(DAQmxReadAnalogF64(taskHandleScanout, 1, 10.0, DAQmx_Val_GroupByChannel, anaInputBuffer, 1, &read, NULL));

				if (anaInputBuffer[0] > VDD_NIDAQ * 0.85) {
					scanoutData[linecount] = 1;
				}
				else {
					scanoutData[linecount] = 0;
				}
			}
		}
	}

	// Check if scan out is correct
	// use PHI_1 (scaninData[linecount][1]) to sample scanoutData

	if (compareMode == 1) {
		for (linecount = 0; linecount < total_lines_read; linecount++){
			if (scaninData[linecount][1] == 1) {
				if (scanoutData[linecount] != scaninData[linecount][0]){
					SCAN_EQUAL = 0;
					printf("line=%d\n", linecount);
					printf("scanin=%d\n", scaninData[linecount][0]);
					printf("scanout=%d\n", scanoutData[linecount]);
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

	return SCAN_EQUAL; //SCAN_EQUAL= 1: pass scan equal selfcheck
}
