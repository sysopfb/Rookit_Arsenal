//system includes
#include "ntddk.h"

//shared includes
#include "ctrlcode.h"
#include "datatype.h"
#include "device.h"

//local includes
#include "dbgmsg.h"

//globals

PDEVICE_OBJECT MSNetDiagDeviceObject;
PDEVICE_OBJECT DriverObjectRef;

//operation routines

void TestCommand
(
 PVOID inputBuffer;
 PVOID outputBuffer;
 ULONG inputerBufferLength;
 ULONG outputBufferLength;
 )
{
	char *ptrBuffer;

	DBG_TRACE("dispathIOControl","Displaying inputBuffer");
	ptrBuffer = (char*)inputBuffer;
	DBG_PRINT2("[dispatchIOControl]: inputBuffer=%s\n", ptrBuffer);
	DBG_TRACE("dispatchIOControl","Populating outputBuffer");
	ptrBuffer = (char*)outputBuffer;
	ptrBuffer[0] = '!';
	ptrBuffer[1] = '1';
	ptrBuffer[2] = '2';
	ptrBuffer[3] = '3';
	ptrBuffer[4] = '!';
	ptrBuffer[5] = '\0';
	DBG_PRINT2("[dispatchIOControl]:outputBuffer=%s\n", ptrBuffer);
	return;
}


//Dispatch Handlers

NTSTATUS defaultDispatch(IN PDEVICE_OBJECT DeviceObject, IN PIRP IRP)
{
	((*IRP).IoStatus).Status = STATUS_SUCCESS;
	((*IRP).IoStatus).Information = 0;
	/*
	 * 	 The IoCompleteRequest routine indicates that the caller has 
	 * 	 	 completed all processing for a given I/O request and is 
	 * 	 	 	 returning the given IRP to the I/O manager.
	 * 	 	 	 	 */
	IoCompleteRequest(IRP, IO_NO_INCREMENT);
	return (STATUS_SUCCESS);
}

NTSTATUS dispatchIOControl(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIRP)
{
	PIO_STACK_LOCATION	irpStack;
	PVOID				inputBuffer;
	PVOID				outputBuffer;
	ULONG				inputBufferLength;
	ULONG				outputBufferLength;
	ULONG				ioctrlcode;
	NTSTATUS			ntStatus;

	ntStatus = STATUS_SUCCESS;
	((*pIRP).IoStatus).Status = STATUS_SUCCESS;
	((*pIRP).IoStatus).Information = 0;

	inputBuffer = (*IRP).AssociatedIrp.SystemBuffer;
	outputBuffer = (*IRP).AssociatedIrp.SystemBuffer;

	//get a pointer to the caller's stack location in the given IRP
	//This is where the function codes and other parameters are
	irpStack = IoGetCurrentIrpStackLocation(IRP);
	inBufferLength = (*irpStack).Parameters.DeviceIoControl.InputBufferLength;
	outBufferLength = (*irpStack).Parameters.DeviceIoControl.OutputBufferLength;
	ioctrlcode = (*irpStack).Parameters.DeviceIoControl.IoControlCode;

	DBG_TRACE("dispatchIOControl","Received a command");
	
	switch(ioctrlcode)
	{
		case IOCTL_TEST_CMD:
			{
				TestCommand(inputBuffer, outputBuffer, inBufferLength, outBufferLength);
				((*IRP).IoStatus).Information = outBufferLength;
			}
			break;
		default:
			{
				DBG_TRACE("dispatchIOControl","control code not recognized");
			}
			break;
	}

	IoCompleteRequest(pIRP, IO_NO_INCREMENT);
	return(ntStatus);
}
//End dispatchIOControl()

//Device and Driver Naming Routines


