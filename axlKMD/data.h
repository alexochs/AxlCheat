#pragma once
#include <ntdef.h>

PDEVICE_OBJECT pDeviceObject;
UNICODE_STRING dev, dos;

wchar_t* modulePathSubstring;
ULONG moduleAddress, processId;
