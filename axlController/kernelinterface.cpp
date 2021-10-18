#include "kernelinterface.hpp"

KernelInterface::KernelInterface(LPCSTR RegistryPath)
{
	hDriver = CreateFileA(RegistryPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
}

KernelInterface::KernelInterface(HANDLE Driver)
{
	hDriver = Driver;
}

DWORD KernelInterface::GetClientDll()
{
	if (hDriver == INVALID_HANDLE_VALUE)
		return 0;

	ULONG Address;
	DWORD Bytes;

	if (DeviceIoControl(hDriver, IO_GET_CLIENTDLL, &Address, sizeof(Address), &Address, sizeof(Address), &Bytes, NULL))
		return Address;

	return 0;
}

DWORD KernelInterface::GetEngineDll()
{
	if (hDriver == INVALID_HANDLE_VALUE)
		return 0;

	ULONG Address;
	DWORD Bytes;

	if (DeviceIoControl(hDriver, IO_GET_ENGINEDLL, &Address, sizeof(Address), &Address, sizeof(Address), &Bytes, NULL))
		return Address;

	return 0;
}

bool KernelInterface::SetModulePathSubstring(const wchar_t* modulePath)
{
	if (hDriver == INVALID_HANDLE_VALUE)
		return false;

	ULONG Output;
	DWORD Bytes;

	return DeviceIoControl(hDriver, IO_SET_MODULEPATH, &modulePath, sizeof(wchar_t*), &Output, sizeof(Output), &Bytes, NULL);
}

DWORD KernelInterface::GetProcessId()
{
	if (hDriver == INVALID_HANDLE_VALUE)
		return 0;

	ULONG ProcessId;
	DWORD Bytes;

	if (DeviceIoControl(hDriver, IO_REQUEST_PROCESSID, &ProcessId, sizeof(ProcessId), &ProcessId, sizeof(ProcessId), &Bytes, NULL))
		return ProcessId;

	return 0;
}