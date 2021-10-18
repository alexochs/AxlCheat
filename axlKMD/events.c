#pragma warning (disable : 4047 4311)

#include "events.h"
#include "messages.h"
#include "data.h"

PLOAD_IMAGE_NOTIFY_ROUTINE ImageLoadCallback(PUNICODE_STRING FullImageName, HANDLE ProcessId, PIMAGE_INFO ImageInfo)
{
	if (wcsstr(FullImageName->Buffer, modulePathSubstring))
	{
		DebugMessage("Module found! \n");
		DebugMessage("Process ID: %d \n", ProcessId);

		processId = (ULONG)ProcessId;
		moduleAddress = ImageInfo->ImageBase;
	}

	return STATUS_SUCCESS;
}