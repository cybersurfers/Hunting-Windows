#pragma once
#include <ntddk.h>

typedef struct _SYSTEM_MODULE_ENTRY
{
	HANDLE Section;
	PVOID MappedBase;
	PVOID ImageBase;
	ULONG ImageSize;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT OffsetToFileName;
	UCHAR FullPathName[256];
} SYSTEM_MODULE_ENTRY, * PSYSTEM_MODULE_ENTRY;

typedef struct _SYSTEM_MODULE_INFORMATION
{
	ULONG Count;
	SYSTEM_MODULE_ENTRY Module[1];
} SYSTEM_MODULE_INFORMATION, * PSYSTEM_MODULE_INFORMATION;

typedef enum _SYSTEM_INFORMATION_CLASS 
{
	// This is the only one we need
	SystemModuleInformation = 11
} SYSTEM_INFORMATION_CLASS;

// Already exported
extern "C" NTSYSAPI NTSTATUS NTAPI ZwQuerySystemInformation
(
	SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	PULONG ReturnLength 
);

// Already exported
extern "C" NTSYSAPI NTSTATUS NTAPI ObReferenceObjectByName
(
	PUNICODE_STRING ObjectName,
	ULONG Attributes,
	PACCESS_STATE AccessState,
	ACCESS_MASK DesiredAccess,
	POBJECT_TYPE ObjectType,
	KPROCESSOR_MODE AccessMode,
	PVOID ParseContext OPTIONAL,
	PVOID * Object
);
// Not sure why it works importing it rather than using it through the below line
// POBJECT_TYPE* IoDriverObjectType = NULL;
extern "C" __declspec(dllimport) POBJECT_TYPE * IoDriverObjectType;