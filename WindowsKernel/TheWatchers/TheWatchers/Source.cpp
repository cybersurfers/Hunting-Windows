#include <ntddk.h>

#include "Common.h"
#include "UndocumentedHelpers.h"

UNICODE_STRING g_DeviceName = RTL_CONSTANT_STRING(L"\\Device\\ch3rn0byl");
UNICODE_STRING g_SymbolicName = RTL_CONSTANT_STRING(L"\\??\\ch3rn0byl");

NTSTATUS HookedDriverDispatch(PDRIVER_OBJECT DriverObject)
{
	UNREFERENCED_PARAMETER(DriverObject);
	DbgPrint("HookedDriverDispatch: If you see this, its hooked");
	return STATUS_SUCCESS;
}

void HookedDriverUnload(PDRIVER_OBJECT DriverObject)
{
	DbgPrint("HookedDriverUnload: If this is seen, this function is hooked\n");

	IoDeleteDevice(DriverObject->DeviceObject);
}

void DriverUnload(PDRIVER_OBJECT DriverObject)
{
	IoDeleteSymbolicLink(&g_SymbolicName);
	IoDeleteDevice(DriverObject->DeviceObject);

	DbgPrint("Driver unloaded completed successfully\n");
}

NTSTATUS DriverCreateClose(PDEVICE_OBJECT, PIRP Irp)
{
	NTSTATUS Status = STATUS_SUCCESS;

	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = Status;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	DbgPrint("DriverCreateClose completed successfully\n");
	return Status;
}

NTSTATUS DriverHook(const char* TargetModule)
{
	// This is the DriverObject of the module we're going to hook
	PDRIVER_OBJECT pHijackedDriverObject = NULL;

	ANSI_STRING AnsiName = { 0 };
	UNICODE_STRING ModuleName = { 0 };
	OBJECT_ATTRIBUTES ObjAttr = { 0 };

	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	RtlInitAnsiString(&AnsiName, TargetModule);

	Status = RtlAnsiStringToUnicodeString(&ModuleName, &AnsiName, TRUE);
	if (!NT_SUCCESS(Status))
	{
		DbgPrint("RtlAnsiStringToUnicodeString error: 0x%08x", Status);
		return Status;
	}
	InitializeObjectAttributes(&ObjAttr, &ModuleName, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);

	Status = ObReferenceObjectByName(&ModuleName, OBJ_CASE_INSENSITIVE, NULL, 0, *IoDriverObjectType, KernelMode, NULL, reinterpret_cast<PVOID*>(&pHijackedDriverObject));
	if (!NT_SUCCESS(Status))
	{
		DbgPrint("ObReferenceObjectByName error: 0x%08x", Status);
		if (pHijackedDriverObject)
		{
			ObDereferenceObject(pHijackedDriverObject);
		}
		RtlFreeUnicodeString(&ModuleName);
		return Status;
	}

	//DbgPrint("got this name: %wZ", pHijackedDriverObject->DriverName);

	// Hooks DriverUnload for now
	auto pDriverUnload = InterlockedExchangePointer(reinterpret_cast<PVOID*>(&pHijackedDriverObject->DriverUnload), HookedDriverUnload);
	auto pDriverDispatch = InterlockedExchangePointer(reinterpret_cast<PVOID*>(pHijackedDriverObject->MajorFunction), HookedDriverDispatch);
	DbgPrint("DriverDispatch is at: %p", pDriverDispatch);
	DbgPrint("DriverUnload is at: %p", pDriverUnload);

	RtlFreeUnicodeString(&ModuleName);
	ObReferenceObject(pHijackedDriverObject);
	return Status;
}

NTSTATUS DriverDeviceControl(PDEVICE_OBJECT, PIRP Irp)
{
	NTSTATUS Status = STATUS_NOT_SUPPORTED;
	
	PIO_STACK_LOCATION StackLocation = IoGetCurrentIrpStackLocation(Irp);
	DWORD32 dwIoControlCode = StackLocation->Parameters.DeviceIoControl.IoControlCode;

	auto UserBuffer = StackLocation->Parameters.DeviceIoControl.Type3InputBuffer;

	switch (dwIoControlCode)
	{
	case DRIVER_HOOK:
		Status = DriverHook(static_cast<const char*>(UserBuffer));
	default:
		break;
	}

	Irp->IoStatus.Status = Status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	DbgPrint("DriverDeviceControl completed successfully!\n");
	return Status;
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING)
{
	NTSTATUS Status = STATUS_UNSUCCESSFUL;
	PDEVICE_OBJECT DeviceObject = NULL;

	Status = IoCreateDevice(DriverObject, 0, &g_DeviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, TRUE, &DeviceObject);
	if (!NT_SUCCESS(Status))
	{
		DbgPrint("IoCreateDevice failed: 0x%08x\n", Status);
		if (DeviceObject)
		{
			IoDeleteDevice(DeviceObject);
		}
		return Status;
	}

	Status = IoCreateSymbolicLink(&g_SymbolicName, &g_DeviceName);
	if (!NT_SUCCESS(Status))
	{
		DbgPrint("IoCreateSymbolicLink failed: 0x%08x\n", Status);
		IoDeleteDevice(DeviceObject);
		return Status;
	}

	DriverObject->DriverUnload = DriverUnload;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = DriverCreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = DriverCreateClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DriverDeviceControl;

	DbgPrint("DriverEntry completed successfully\n");
	return Status;
}
