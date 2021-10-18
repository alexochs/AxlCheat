#pragma warning (disable : 4047 4311)

#include "events.h"
#include "messages.h"
#include "data.h"

PLOAD_IMAGE_NOTIFY_ROUTINE ImageLoadCallback(PUNICODE_STRING FullImageName, HANDLE ProcessId, PIMAGE_INFO ImageInfo)
{
	if (wcsstr(FullImageName->Buffer, L"\\Counter-Strike Global Offensive\\csgo\\bin\\client.dll"))
	{
		processId = (ULONG)ProcessId;
		client = ImageInfo->ImageBase;
	}
	else if (wcsstr(FullImageName->Buffer, L"\\Counter-Strike Global Offensive\\bin\\engine.dll"))
	{
		engine = ImageInfo->ImageBase;
	}

	return STATUS_SUCCESS;
}