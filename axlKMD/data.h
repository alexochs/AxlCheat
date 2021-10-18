#pragma once
#include <ntdef.h>

PDEVICE_OBJECT pDeviceObject;
UNICODE_STRING dev, dos;

ULONG processId,
		client,
		engine;
