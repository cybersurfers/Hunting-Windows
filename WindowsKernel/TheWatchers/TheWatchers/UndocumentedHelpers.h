#pragma once
#include <ntddk.h>

extern "C" NTSYSAPI NTSTATUS NTAPI ObReferenceObjectByName(
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