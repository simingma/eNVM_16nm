// MODULE : WAModbus.H
// Definitions required to use the Modbus DLL interface functions.


#ifndef __WAModbus_H__
#define __WAModbus_H__


#if defined(__cplusplus)
extern "C"
{
#endif

	// #include <windows.h>
typedef void * MODBUSHND_T;
typedef unsigned short * MODBUSBSTR;

#ifdef WAMODBUS_EXPORTS
 #define WAMODBUS_EXPORT __declspec(dllexport)
#else
 #define WAMODBUS_EXPORT 
#endif


#define MAX_MODBUS_DATA_LENGTH 250


// Possible return values for WA_readModbusDevice(), WA_writeModbusDevice(), WA_readModbusDiagnostics()
#define HOST_REJECTED_DATA_LENGTH    -6
#define HOST_REJECTED_START_ADDRESS  -5
#define HOST_RECEIVED_IO_ERR_SENDING -4
#define HOST_RECEIVED_MODBUS_ERROR   -3	// Error set from controller, the MODBUS ERROR CODE
										// will be stored in data[0]
#define HOST_RECEIVED_TIMEOUT_ERROR  -2 // Timeout waiting for return string from slave.
#define HOST_RECEIVED_CHECKSUM_ERROR -1	// Slave's return string had a bad checksum

#define COMM_OK					0
// MODBUS ERROR CODES
#define ILLEGAL_FUNCTION		1
#define ILLEGAL_DATA_ADDRESS	2
#define ILLEGAL_DATA_VALUE		3
#define SLAVE_DEVICE_FAILURE	4
#define ACKNOWLEDGE				5
#define SLAVE_DEVICE_BUSY		6
#define NEGATIVE_ACKNOWLEDGE	7
#define MEMORY_PARITY_ERROR		8


// Port Settings
#define NOPARITY            0
#define ODDPARITY           1
#define EVENPARITY          2
#define MARKPARITY          3
#define SPACEPARITY         4


// Returned values for WA_openModbusConnection(), WA_openVBModbusConnection(), WA_closeModbusConnection()
#define BAD 1
#define GOOD 0



extern WAMODBUS_EXPORT int _stdcall WA_openModbusConnection(MODBUSHND_T *hComm, char *portStr, long baudRate, short parity);
extern WAMODBUS_EXPORT int _stdcall WA_readModbusDevice(MODBUSHND_T hComm, short slaveAddress, long startAddress, short *data, short numPoints);
extern WAMODBUS_EXPORT int _stdcall WA_writeModbusDevice(MODBUSHND_T hComm, short slaveAddress, long startAddress, short *data, short numPoints);
extern WAMODBUS_EXPORT void _stdcall WA_closeModbusConnection(MODBUSHND_T hComm);

extern WAMODBUS_EXPORT int _stdcall WA_readModbusDiagnostics(MODBUSHND_T hComm, short slaveAddress, short subFunction, short *data);

extern WAMODBUS_EXPORT int _stdcall  WA_openVBModbusConnection(MODBUSHND_T *hComm, MODBUSBSTR bStr, long baudRate, short parity);

extern WAMODBUS_EXPORT int _stdcall  WA_getReadString(MODBUSHND_T *hComm, unsigned char *data);
extern WAMODBUS_EXPORT int _stdcall  WA_getWriteString(MODBUSHND_T *hComm, unsigned char *data);
extern WAMODBUS_EXPORT int _stdcall  WA_getReadResponse(MODBUSHND_T *hComm, unsigned char *data);
extern WAMODBUS_EXPORT int _stdcall  WA_getWriteResponse(MODBUSHND_T *hComm, unsigned char *data);

extern WAMODBUS_EXPORT int _stdcall WA_readAnafazeDevice(MODBUSHND_T hComm, short ctlrAddr, long dataAddr, short *dataArry, short numItems, short dataSize, short *statusByte);
extern WAMODBUS_EXPORT int _stdcall WA_writeAnafazeDevice(MODBUSHND_T hComm, short ctlrAddr, long dataAddr, short *dataArry, short numItems, short dataSize, short *statusByte);
extern WAMODBUS_EXPORT int _stdcall WA_getAnafazeReadString(MODBUSHND_T *hComm, unsigned char *data);
extern WAMODBUS_EXPORT int _stdcall WA_getAnafazeWriteString(MODBUSHND_T *hComm, unsigned char *data);
extern WAMODBUS_EXPORT int _stdcall WA_getAnafazeReadResponse(MODBUSHND_T *hComm, unsigned char *data);
extern WAMODBUS_EXPORT int _stdcall WA_getAnafazeWriteResponse(MODBUSHND_T *hComm, unsigned char *data);
extern WAMODBUS_EXPORT int _stdcall WA_readModbusDeviceNoWait(MODBUSHND_T hComm, short slaveAddress, long startAddress, short numPoints);
extern WAMODBUS_EXPORT int _stdcall WA_writeModbusDeviceNoWait(MODBUSHND_T hComm, short slaveAddress, long startAddress, short *data, short numPoints);
extern WAMODBUS_EXPORT int _stdcall WA_getCommChars(MODBUSHND_T hComm);
extern WAMODBUS_EXPORT int _stdcall WA_getSerialPortBytes(MODBUSHND_T hComm, short *data, short numPoints);
extern WAMODBUS_EXPORT int _stdcall WA_setTimeout(short milliSeconds);




#if defined(__cplusplus)
}
#endif

#endif /* __WAModbus_H__ */

