#include <exception>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <Windows.h>

#include "InitException.h"
#include "Nvidia.h"

NvAPI_QueryInterface_t _NvQueryInterface = 0;
NvAPI_Initialize_t _NvInit = 0;
NvAPI_Unload_t _NvUnload = 0;
NvAPI_EnumPhysicalGPUs_t _NvEnumGPUs = 0;
NvAPI_GPU_GetSystemType_t _NvGetSysType = 0;
NvAPI_GPU_GetFullName_t _NvGetName = 0;
NvAPI_GPU_GetPhysicalFrameBufferSize_t _NvGetMemSize = 0;
NvAPI_GPU_GetRamType_t _NvGetMemType = 0;
NvAPI_GPU_GetVbiosVersionString_t _NvGetBiosName = 0;
NvAPI_GPU_GetAllClockFrequencies_t _NvGetFreq = 0;
NvAPI_GPU_GetPstates20_t _NvGetPstates = 0;
NvAPI_GPU_SetPstates20_t _NvSetPstates = 0;

void init()
{
	if (_NvQueryInterface == 0)
	{
		_NvQueryInterface = (NvAPI_QueryInterface_t)GetProcAddress(LoadLibrary(_T("nvapi.dll")), "nvapi_QueryInterface");
	}

	if (_NvQueryInterface == 0)
	{
		throw InitException();
	}
}

extern "C"
{
	__declspec(dllexport) void Nv_Init()
	{
		if (_NvQueryInterface == 0)
		{
			init();
		}

		_NvInit = (NvAPI_Initialize_t)_NvQueryInterface(0x0150E828);
	}
}