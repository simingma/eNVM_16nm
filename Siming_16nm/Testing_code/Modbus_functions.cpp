/******************************
Functions for controlling the temperature chamber
*******************************/
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif


#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <conio.h>
#include <winbase.h>
#include "WAModbus.h"
#include "TestFunctions.h"


///////////////////////////////////////////////////////////
// The communication port must be referred to with a HANDLE
HANDLE portHandle;

////////////////////////////////////////////////////
// Variables necessary for launching separate thread
HANDLE hPollThread;
DWORD dwThreadID = 0;
DWORD passedParm = 13;
BOOL pollProcComplete = FALSE;


//////////////////////////////////////////////
// Interpret response from controller for user.
// Error code or valid data may be contained in ModBusBag struc
void printResponse(char *Measure_file, ModBusBag * mb)
{
    FILE *f_ptr;
    if ((f_ptr = fopen(Measure_file, "a")) == NULL){
    	printf("Cannot open%s.\n", Measure_file);
    	return;
    }

	int j;

	if (mb->errorCode!=0)
	{
		fprintf(f_ptr, " HOST RECEIVED MODBUS ERROR FROM SLAVE : ");
		switch(mb->errorCode)
		{
		case HOST_RECEIVED_IO_ERR_SENDING:
			fprintf(f_ptr, "HOST_RECEIVED_IO_ERR_SENDING\n");
			return;
		case ILLEGAL_FUNCTION:
			fprintf(f_ptr, "ILLEGAL_FUNCTION\n");
			return;;
		case ILLEGAL_DATA_ADDRESS:
			fprintf(f_ptr, "ILLEGAL_DATA_ADDRESS\n");
			return;
		case ILLEGAL_DATA_VALUE:
			fprintf(f_ptr, "ILLEGAL_DATA_VALUE\n");
			return;
		case SLAVE_DEVICE_FAILURE:
			fprintf(f_ptr, "SLAVE_DEVICE_FAILURE\n");
			return;
		case ACKNOWLEDGE:
			fprintf(f_ptr, "ACKNOWLEDGE\n");
			return;
		case SLAVE_DEVICE_BUSY:
			fprintf(f_ptr, "SLAVE_DEVICE_BUSY\n");
			return;
		case NEGATIVE_ACKNOWLEDGE:
			fprintf(f_ptr, "NEGATIVE_ACKNOWLEDGE\n");
			return;
		case MEMORY_PARITY_ERROR:
			fprintf(f_ptr, "MEMORY_PARITY_ERROR\n");
			return;
		case HOST_RECEIVED_TIMEOUT_ERROR:
			fprintf(f_ptr, "HOST RECEIVED TIMEOUT ERROR\n");
			return;
		case HOST_RECEIVED_CHECKSUM_ERROR:
			fprintf(f_ptr, "HOST RECEIVED CHECKSUM ERROR\n");
			return;
		case HOST_RECEIVED_MODBUS_ERROR:
			switch(mb->data[0])
			{
			case ILLEGAL_FUNCTION : fprintf(f_ptr, "ILLEGAL_FUNCTION\n");
				break;
			case ILLEGAL_DATA_ADDRESS : fprintf(f_ptr, "ILLEGAL_DATA_ADDRESS\n");
				break;
			case ILLEGAL_DATA_VALUE : fprintf(f_ptr, "ILLEGAL_DATA_VALUE\n");
				break;
			}
			return;
		}
	}
	if (mb->numPoints > 0)
	{
		for (j=0; j<mb->numPoints; j++)
			fprintf(f_ptr, " %d\n", mb->data[j]);
		//putch('\n');
	}

	fclose(f_ptr);
	return;
}



/*
////////////////////////////////////////////
// This thread reads an F4s Process Variable 
// and writes that value to the F4s Set Point register
BOOL pollProc(DWORD *parmPtr)
{
     ModBusBag mbRead;			// Read/Write data structures

	 pollProcComplete=FALSE;	// Keep polling until main() loop says stop

	 mbRead.slaveAddress = 1;	// Be sure to set the controller's communication address
	 mbRead.numPoints = 1;		// For multiloop controllers this may be > 1
     do
     {
		mbRead.startAddress = F4_PV;
		mbRead.errorCode = WA_readModbusDevice(portHandle, mbRead.slaveAddress, mbRead.startAddress, mbRead.data, mbRead.numPoints);      // Read from device
		printf("Thread #2: Reading Process Variable:");
		printResponse(&mbRead);

		mbRead.startAddress = F4_SETPOINT;
		mbRead.errorCode = WA_readModbusDevice(portHandle, mbRead.slaveAddress, mbRead.startAddress, mbRead.data, mbRead.numPoints);      // Read from device
		printf("Thread #2: Writing Set Point:");
		printResponse(&mbRead);
     }
     while(!kbhit());


	hPollThread=0;
	dwThreadID=0;
	return TRUE;
}
*/
/*
BOOL startThread(void)
{
     if (NULL == (hPollThread = CreateThread( (LPSECURITY_ATTRIBUTES) NULL, 0, (LPTHREAD_START_ROUTINE) pollProc,
                                    (LPVOID) &passedParm, 0, &dwThreadID )))
	 {
		 return FALSE;
	 }

	 return TRUE;
}
*/

// set the Set Point to SP=bake_temperature for bake_time, then lower back to SP=room_temperature 
// temperature in in short interger form, with one decimal infered: 
// 125C in ``short'' is bake_temperature = 1250
// 21C in ``short'' is room_temperature = 210
// bake_time is in DWORD representing milli-seconds
// 1 min (60 seconds) corresponds to bake_time = 60000
int write_SP(char *Measure_file, short room_temperature, short bake_temperature, DWORD bake_time)
{

    PortSetup ps;                // Comm port setup data struct
    ModBusBag mbRead;			  // Read/Write data structs


       // Set Modbus communicaitons Baudrate
    ps.baudRate = 9600;
        // Set the communication port to use.
    strcpy(ps.portStr, "COM1");
    ps.parity = 0;

        // Open COM port here
    if (WA_openModbusConnection(&portHandle, ps.portStr, ps.baudRate, ps.parity) != GOOD)
    {
         printf("\nCan not open port %s!\n", ps.portStr);
         return FAIL;
    }

    //startThread();		// illustrate DLL multi-threading capability by starting thread procedure

    
     ///////////////////////////////////////
    // Set the slave's communication address
    mbRead.slaveAddress = 1;
    /////////////////////////////////////////////////////////////////////////////////////
    // Set the number of points to up/download, for multiloop controllers this may be > 1
    mbRead.numPoints = 1;		

        // read the Process Variable value from the chamber
	// write a different temperature (bake temperature) to the Set Point parameter
	// soak (sleep/wait) for baking time
	// write a room temperature to the Set Point parameter
    
    
    SYSTEMTIME lt;
    FILE *f_ptr;

    if ((f_ptr = fopen(Measure_file, "a")) == NULL){
    	printf("Cannot open%s.\n", Measure_file);
    	return FAIL;
    }

    mbRead.startAddress = F4_PV;
    mbRead.errorCode = WA_readModbusDevice(portHandle, mbRead.slaveAddress, mbRead.startAddress, mbRead.data, mbRead.numPoints);      // Read from device
    fprintf(f_ptr, "Initial temperature: Reading Process Variable:");
    fclose(f_ptr);
    printResponse(Measure_file, &mbRead);
    if ((f_ptr = fopen(Measure_file, "a")) == NULL){
    	printf("Cannot open%s.\n", Measure_file);
    	return FAIL;
    }
    GetLocalTime(&lt);
    fprintf(f_ptr, "The local time is: %02d:%02d:%02d\n", lt.wHour, lt.wMinute, lt.wSecond);


    mbRead.startAddress = F4_SETPOINT;
    mbRead.data[0] = bake_temperature; //set the lowest index of data to the bake_temperature (only reading/writing one data)
    mbRead.errorCode = WA_writeModbusDevice(portHandle, mbRead.slaveAddress, mbRead.startAddress, mbRead.data, mbRead.numPoints);      // write to device
    fprintf(f_ptr, "Start heating up: Writing Set Point:");
    fclose(f_ptr);
    printResponse(Measure_file, &mbRead);
    if ((f_ptr = fopen(Measure_file, "a")) == NULL){
    	printf("Cannot open%s.\n", Measure_file);
    	return FAIL;
    }
    GetLocalTime(&lt);
    fprintf(f_ptr, "The local time is: %02d:%02d:%02d\n", lt.wHour, lt.wMinute, lt.wSecond);
    

    ::Sleep(bake_time);
    
    mbRead.startAddress = F4_PV;
    mbRead.errorCode = WA_readModbusDevice(portHandle, mbRead.slaveAddress, mbRead.startAddress, mbRead.data, mbRead.numPoints);      // Read from device
    fprintf(f_ptr, "After heating up and baking, right before cooling down: Reading Process Variable:");
    fclose(f_ptr);
    printResponse(Measure_file, &mbRead);
    if ((f_ptr = fopen(Measure_file, "a")) == NULL){
    	printf("Cannot open%s.\n", Measure_file);
    	return FAIL;
    }
    GetLocalTime(&lt);
    fprintf(f_ptr, "The local time is: %02d:%02d:%02d\n", lt.wHour, lt.wMinute, lt.wSecond);


    mbRead.startAddress = F4_SETPOINT;
    mbRead.data[0] = room_temperature; //set the lowest index of data (presumably back to before) to the room_temperature (only reading/writing one data)
    mbRead.errorCode = WA_writeModbusDevice(portHandle, mbRead.slaveAddress, mbRead.startAddress, mbRead.data, mbRead.numPoints);      // write to device
    fprintf(f_ptr, "Start cooling down: Writing Set Point:");
    fclose(f_ptr);
    printResponse(Measure_file, &mbRead);
    if ((f_ptr = fopen(Measure_file, "a")) == NULL){
    	printf("Cannot open%s.\n", Measure_file);
    	return FAIL;
    }
    GetLocalTime(&lt);
    fprintf(f_ptr, "The local time is: %02d:%02d:%02d\n", lt.wHour, lt.wMinute, lt.wSecond);


    DWORD cooldown_time = 1800000; //give the chamber 30 mins, to make sure to cool down from 125C to 21C
    ::Sleep(cooldown_time);

    mbRead.startAddress = F4_PV;
    mbRead.errorCode = WA_readModbusDevice(portHandle, mbRead.slaveAddress, mbRead.startAddress, mbRead.data, mbRead.numPoints);      // Read from device
    fprintf(f_ptr, "After cooling down: Reading Process Variable:");
    fclose(f_ptr);
    printResponse(Measure_file, &mbRead);
    if ((f_ptr = fopen(Measure_file, "a")) == NULL){
    	printf("Cannot open%s.\n", Measure_file);
    	return FAIL;
    }
    GetLocalTime(&lt);
    fprintf(f_ptr, "The local time is: %02d:%02d:%02d\n", lt.wHour, lt.wMinute, lt.wSecond);


    //pollProcComplete=TRUE;

    //while(dwThreadID);

        // Close COM port here
    WA_closeModbusConnection(portHandle);
    fclose(f_ptr);
    return 0;
}


