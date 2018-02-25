/******************************
 Basic GPIB Functions
*******************************/

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

//****** Test Chip Number (for Socket testing) ********
int chipNum = 2;
//TODO: what does this mean?

//******* Test Chip Number (for soldered testing) *********

//***** GPIB Device Address Variables Definition *******
/* Primary address of the device           */
int   PrimaryAddress_VDD_DIG_VDD_WL = 3;	
int   PrimaryAddress_VSS_WL_VSS_PW = 5;	
int   PrimaryAddress_VDD_IO_V_NIDAQ = 4;
int   PrimaryAddress_VSPARE_VAB = 2;
int   PrimaryAddress_MM34401A = 1;   //The first DMM(34401A), measure Drain Current
int   PrimaryAddress_MM34410A_6 = 6; //a second DMM(34410A) for Isub
int   PrimaryAddress_MM34401A_7 = 7; //a third DMM(34401A) for Is
int   PrimaryAddress_ELTM6514 = 14;  //electrometer 6514 borrowed from Jim McArther

int   SecondaryAddress = 0;		/* Secondary address of the device */

/* Device unit descriptor */
int   _VDD_DIG_VDD_WL;					 
int   _VSS_WL_VSS_PW;					 
int   _VDD_IO_V_NIDAQ;
int   _VSPARE_VAB;
int   _MM34401A;
int   _MM34410A_6;
int   _MM34401A_7;
int   _ELTM6514;

int   _VDD_DIG = 1;
int   _VDD_WL = 2;
int   _VSS_WL = 1;
int   _VSS_PW = 2;
int   _VDD_IO = 1;
int   _V_NIDAQ = 2;
int   _VSPARE = 1;
int   _VAB = 2;

int   BoardIndex = 0;			/* Interface Index (GPIB0=0,GPIB1=1,etc.)  */


//****** MM34401A Enable *********
int   MM34401A_present = 1;

//****** ELTM6514 Enable *********
int   ELTM6514_present = 0; //until ask Jim for borrowing the elctrometer again :)

//****** Write to Device (Send Command)*********
void _ibwrt(int DeviceId, char *command) {
	size_t len;
	size_t maxsize = 100;
	len = strnlen(command, maxsize);
	ibwrt(DeviceId, command, len);

	if (ibsta & ERR) {
		GpibError(DeviceId, "ibwrt Error");
	}
}

//****** Read From Device *******************
void _ibrd (int DeviceId, char *command, long num_bytes) {
	ibrd(DeviceId, command, num_bytes);

	if (ibsta & ERR) {
		GpibError(DeviceId,"ibrd Error");	
	}	
}
	
/*****************************************************************************
 *                      Function GPIBERROR
 * This function will notify you that a NI-488 function failed by
 * printing an error message.  The status variable IBSTA will also be
 * printed in hexadecimal along with the mnemonic meaning of the bit
 * position. The status variable IBERR will be printed in decimal
 * along with the mnemonic meaning of the decimal value.  The status
 * variable IBCNTL will be printed in decimal.
 *
 * The NI-488 function IBONL is called to disable the hardware and
 * software.
 *
 * The EXIT function will terminate this program.
 *****************************************************************************/
void GpibError(int DeviceId, char *msg) {

	printf ("%s\n", msg);

	printf ("ibsta = &H%x  <", ibsta);
	if (ibsta & ERR )  printf (" ERR");
	if (ibsta & TIMO)  printf (" TIMO");
	if (ibsta & END )  printf (" END");
	if (ibsta & SRQI)  printf (" SRQI");
	if (ibsta & RQS )  printf (" RQS");
	if (ibsta & CMPL)  printf (" CMPL");
	if (ibsta & LOK )  printf (" LOK");
	if (ibsta & REM )  printf (" REM");
	if (ibsta & CIC )  printf (" CIC");
	if (ibsta & ATN )  printf (" ATN");
	if (ibsta & TACS)  printf (" TACS");
	if (ibsta & LACS)  printf (" LACS");
	if (ibsta & DTAS)  printf (" DTAS");
	if (ibsta & DCAS)  printf (" DCAS");
	printf (" >\n");

	printf ("iberr = %d", iberr);
	if (iberr == EDVR) printf (" EDVR <DOS Error>\n");
	if (iberr == ECIC) printf (" ECIC <Not Controller-In-Charge>\n");
	if (iberr == ENOL) printf (" ENOL <No Listener>\n");
	if (iberr == EADR) printf (" EADR <Address error>\n");
	if (iberr == EARG) printf (" EARG <Invalid argument>\n");
	if (iberr == ESAC) printf (" ESAC <Not System Controller>\n");
	if (iberr == EABO) printf (" EABO <Operation aborted>\n");
	if (iberr == ENEB) printf (" ENEB <No GPIB board>\n");
	if (iberr == EOIP) printf (" EOIP <Async I/O in progress>\n");
	if (iberr == ECAP) printf (" ECAP <No capability>\n");
	if (iberr == EFSO) printf (" EFSO <File system error>\n");
	if (iberr == EBUS) printf (" EBUS <Command error>\n");
	if (iberr == ESTB) printf (" ESTB <Status byte lost>\n");
	if (iberr == ESRQ) printf (" ESRQ <SRQ stuck on>\n");
	if (iberr == ETAB) printf (" ETAB <Table Overflow>\n");

	printf ("ibcntl = %ld\n", ibcntl);
	printf ("\n");

	/* Call ibonl to take the device and interface offline */
	ibonl (DeviceId,0);

	exit(1);
}

//****** Device Initialization Function ***********
int GpibInitDev(int PrimaryAddress) {
	int DeviceId;

	DeviceId = ibdev(            /* Create a unit descriptor handle         */
		BoardIndex,              /* Board Index (GPIB0 = 0, GPIB1 = 1, ...) */
		PrimaryAddress,          /* Device primary address                  */
		SecondaryAddress,        /* Device secondary address                */
		T10s,                    /* Timeout setting (T10s = 10 seconds)     */
		1,                       /* Assert EOI line at end of write         */
		0);                      /* EOS termination mode                    */
	if (ibsta & ERR) {             /* Check for GPIB Error                    */
		GpibError(DeviceId, "ibdev Error"); 
	}

	ibclr(DeviceId);                 /* Clear the device                        */
	if (ibsta & ERR) {
		GpibError(DeviceId, "ibclr Error");
	}

	return DeviceId;
}


//****** Device Uninitialization Function *******
void GpibUnInitDev(int DeviceId) {
	ibonl(DeviceId, 0);              /* Take the device offline                 */
	if (ibsta & ERR) {
		GpibError(DeviceId, "ibonl Error");	
	}
}

//****** Send Local Command to system ********
void GpibSetLocalDev(int DeviceId) {
	_ibwrt(DeviceId, "System:Local");     /* Send the system:local command   */
	if (ibsta & ERR) {
		GpibError(DeviceId, "ibwrt Error");
	}
}


//****** Get Device Identification **********
void GpibGetIDN(int DeviceId) {

	char Buffer[101];

	ibwrt(DeviceId, "*IDN?", 5);     /* Send the identification query command   */
	if (ibsta & ERR) {
		GpibError(DeviceId, "ibwrt Error");
	}

	ibrd(DeviceId, Buffer, 100);     /* Read up to 100 bytes from the device    */
	if (ibsta & ERR) {
		GpibError(DeviceId, "ibrd Error");	
	}
	Buffer[ibcntl] = '\0';         /* Null terminate the ASCII string         */
	printf("%s\n", Buffer);        /* Print the device identification         */

}


//****** Send Reset Command to Device *********
void GpibResetDev(int DeviceId) {

	ibwrt(DeviceId, "*RST", 4);     /* Send the reset command   */
	if (ibsta & ERR) {
		GpibError(DeviceId, "ibwrt Error");
	}

}


//****** Send clear state command to Device ********
void GpibClrStDev(int DeviceId) {

	ibwrt(DeviceId, "*CLS", 4);     /* Send the clear state command   */
	if (ibsta & ERR) {
		GpibError(DeviceId, "ibwrt Error");
	}

}

//****** Initialize GPIB Equipments *********
void GpibEquipmentInit() {
	/* Match the primary address number with the setting on the device's front panel */
	/* The initualization function returns a device unit descriptor */
	_VDD_DIG_VDD_WL = GpibInitDev(PrimaryAddress_VDD_DIG_VDD_WL);
	_VSS_WL_VSS_PW = GpibInitDev(PrimaryAddress_VSS_WL_VSS_PW);
	_VDD_IO_V_NIDAQ = GpibInitDev(PrimaryAddress_VDD_IO_V_NIDAQ);
	_VSPARE_VAB = GpibInitDev(PrimaryAddress_VSPARE_VAB);

	GpibGetIDN(_VDD_DIG_VDD_WL);
	GpibGetIDN(_VSS_WL_VSS_PW);
	GpibGetIDN(_VDD_IO_V_NIDAQ);
	GpibGetIDN(_VSPARE_VAB);

	if (MM34401A_present == 1) {
		_MM34401A = GpibInitDev(PrimaryAddress_MM34401A);
		GpibGetIDN(_MM34401A);
		_MM34410A_6 = GpibInitDev(PrimaryAddress_MM34410A_6);
		GpibGetIDN(_MM34410A_6);
		_MM34401A_7 = GpibInitDev(PrimaryAddress_MM34401A_7);
		GpibGetIDN(_MM34401A_7);
	}
	if (ELTM6514_present == 1) {
		_ELTM6514 = GpibInitDev(PrimaryAddress_ELTM6514);
		GpibGetIDN(_ELTM6514);
	}
	
}

//****** Uninitialize GPIB Equipments ********
void GpibEquipmentUnInit() {
	GpibUnInitDev(_VDD_DIG_VDD_WL);
	GpibUnInitDev(_VSS_WL_VSS_PW);
	GpibUnInitDev(_VDD_IO_V_NIDAQ);
	GpibUnInitDev(_VSPARE_VAB);

	if (MM34401A_present == 1) {
		GpibUnInitDev(_MM34401A);
		GpibUnInitDev(_MM34410A_6);
		GpibUnInitDev(_MM34401A_7);
	}

	if (ELTM6514_present == 1) {
		GpibUnInitDev(_ELTM6514);
	}
	
}

//****** Make GPIB Equipments Local *********
void GpibEquipmentSetLocal() {
	GpibSetLocalDev(_VDD_DIG_VDD_WL);
	GpibSetLocalDev(_VSS_WL_VSS_PW);
	GpibSetLocalDev(_VDD_IO_V_NIDAQ);
	GpibSetLocalDev(_VSPARE_VAB);

	if (MM34401A_present == 1) {
		GpibSetLocalDev(_MM34401A);
		GpibSetLocalDev(_MM34410A_6);
		GpibSetLocalDev(_MM34401A_7);
	}
	if (ELTM6514_present == 1){
		GpibSetLocalDev(_ELTM6514);
	}
}
