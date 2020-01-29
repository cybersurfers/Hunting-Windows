#include <ntddk.h>

#include "UndocumentedHelpers.h"

void HookedDriverUnload(PDRIVER_OBJECT DriverObject)
{
	DbgPrint("If this is seen, this function is hooked\n");

	IoDeleteDevice(DriverObject->DeviceObject);
}

void DriverUnload(PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING SymbolicName = RTL_CONSTANT_STRING(L"\\??\\ch3rn0byl");

	IoDeleteSymbolicLink(&SymbolicName);
	IoDeleteDevice(DriverObject->DeviceObject);

	DbgPrint("Driver unloaded completed successfully\n");
}

NTSTATUS DriverCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	NTSTATUS status = STATUS_SUCCESS;

	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = status;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	DbgPrint("DriverCreateClose completed successfully\n");
	return status;
}

NTSTATUS DriverDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	NTSTATUS status = STATUS_NOT_SUPPORTED;

	PIO_STACK_LOCATION StackLocation = IoGetCurrentIrpStackLocation(Irp);
	DWORD32 dwIoControlCode = StackLocation->Parameters.DeviceIoControl.IoControlCode;

	switch (dwIoControlCode)
	{
	case 12:
		break;
	default:
		status = STATUS_INVALID_DEVICE_REQUEST;
		break;
	}

	Irp->IoStatus.Status = status;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	DbgPrint("DriverDeviceControl completed successfully!\n");
	return status;
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);

	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\ch3rn0byl");
	UNICODE_STRING SymbolicName = RTL_CONSTANT_STRING(L"\\??\\ch3rn0byl");

	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PDEVICE_OBJECT DeviceObject = NULL;

	status = IoCreateDevice(DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, TRUE, &DeviceObject);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("IoCreateDevice failed: 0x%08x\n", status);
		if (DeviceObject)
		{
			IoDeleteDevice(DeviceObject);
		}
		return status;
	}

	status = IoCreateSymbolicLink(&SymbolicName, &DeviceName);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("IoCreateSymbolicLink failed; 0x%08x\n", status);

		// DeviceObject shouldn't be NULL at this stage
		IoDeleteDevice(DeviceObject);
		return status;
	}

	DriverObject->DriverUnload = DriverUnload;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = DriverCreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = DriverCreateClose;

	// This will get implemented soon
	// DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = 

	// Making sure everything below is function before I over engineer it
	UNICODE_STRING TargetedDriverName = RTL_CONSTANT_STRING(L"\\driver\\aksfridge");
	PDRIVER_OBJECT TargetedDriver = NULL;

	__try
	{
		DbgPrint("Checking against: %wZ\n", TargetedDriverName);
		status = ObReferenceObjectByName(
			&TargetedDriverName,
			OBJ_CASE_INSENSITIVE,
			NULL,
			0,
			*IoDriverObjectType,
			KernelMode,
			NULL,
			reinterpret_cast<PVOID*>(&TargetedDriver)
		);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DbgPrint("Failed at ObReferenceObjectByName: 0x%08x\n", GetExceptionCode());
	}

	DbgPrint("Its name: %wZ", TargetedDriver->DriverName);
	DbgPrint("The size: %d", TargetedDriver->Size);

	// Save the original pointer to switch back
	auto wtf = InterlockedExchangePointer((PVOID*)&TargetedDriver->DriverUnload, HookedDriverUnload);
	DbgPrint("wtheck: %p", wtf);


	// Close out
	ObDereferenceObject(TargetedDriver);

	DbgPrint("DriverEntry completed successfully\n");
	return status;
}
