#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <Windows.h>

#include "Nvidia.h"

NvAPI_QueryInterface_t NvQueryInterface = 0;
NvAPI_Initialize_t NvInit = 0;
NvAPI_Unload_t NvUnload = 0;
NvAPI_EnumPhysicalGPUs_t NvEnumGPUs = 0;
NvAPI_GPU_GetSystemType_t NvGetSysType = 0;
NvAPI_GPU_GetFullName_t NvGetName = 0;
NvAPI_GPU_GetPhysicalFrameBufferSize_t NvGetMemSize = 0;
NvAPI_GPU_GetRamType_t NvGetMemType = 0;
NvAPI_GPU_GetVbiosVersionString_t NvGetBiosName = 0;
NvAPI_GPU_GetAllClockFrequencies_t NvGetFreq = 0;
NvAPI_GPU_GetPstates20_t NvGetPstates = 0;
NvAPI_GPU_SetPstates20_t NvSetPstates = 0;

int main(int argc, char **argv)
{
	int nGPU = 0, userfreq = 0, systype = 0, memsize = 0, memtype = 0;
	int *hdlGPU[64] = { 0 }, *buf = 0;
	char sysname[64] = { 0 }, biosname[64] = { 0 };
	NV_GPU_PERF_PSTATES20_INFO_V1 pstates_info;
	pstates_info.version = 0x11c94;

	NvQueryInterface = (NvAPI_QueryInterface_t)GetProcAddress(LoadLibrary(_T("nvapi.dll")), "nvapi_QueryInterface");

	if (NvQueryInterface == 0)
	{
        NvQueryInterface = (NvAPI_QueryInterface_t)GetProcAddress(LoadLibrary(_T("nvapi64.dll")), "nvapi_QueryInterface");
        if (NvQueryInterface == 0)
        {
            printf("\nnvapi.dll could not be loaded! This is probably because you have\nno nVidia hardware, and, therefore, no nVidia drivers installed.\n");
            return 0;
        }
	}

	NvInit = (NvAPI_Initialize_t)NvQueryInterface(0x0150E828);
	NvUnload = (NvAPI_Unload_t)NvQueryInterface(0xD22BDD7E);
	NvEnumGPUs = (NvAPI_EnumPhysicalGPUs_t)NvQueryInterface(0xE5AC921F);
	NvGetSysType = (NvAPI_GPU_GetSystemType_t)NvQueryInterface(0xBAAABFCC);
	NvGetName = (NvAPI_GPU_GetFullName_t)NvQueryInterface(0xCEEE8E9F);
	NvGetMemSize = (NvAPI_GPU_GetPhysicalFrameBufferSize_t)NvQueryInterface(0x46FBEB03);
	NvGetMemType = (NvAPI_GPU_GetRamType_t)NvQueryInterface(0x57F7CAAC);
	NvGetBiosName = (NvAPI_GPU_GetVbiosVersionString_t)NvQueryInterface(0xA561FD7D);
	NvGetFreq = (NvAPI_GPU_GetAllClockFrequencies_t)NvQueryInterface(0xDCB616C3);
	NvGetPstates = (NvAPI_GPU_GetPstates20_t)NvQueryInterface(0x6FF81213);
	NvSetPstates = (NvAPI_GPU_SetPstates20_t)NvQueryInterface(0x0F4DAE6B);

	NvInit();
	NvEnumGPUs(hdlGPU, &nGPU);
	NvGetSysType(hdlGPU[0], &systype);
	NvGetName(hdlGPU[0], sysname);
	NvGetMemSize(hdlGPU[0], &memsize);
	NvGetMemType(hdlGPU[0], &memtype);
	NvGetBiosName(hdlGPU[0], biosname);
	NvGetPstates(hdlGPU[0], &pstates_info);

	switch (systype)
	{
	case 1:
		printf("\nType: Laptop\n");
		break;
	case 2:
		printf("\nType: Desktop\n");
		break;
	default:
		printf("\nType: Unknown\n");
		break;
	}

	printf("Name: %s\n", sysname);
	printf("VRAM: %dMB GDDR%d\n", memsize / 1024, memtype <= 7 ? 3 : 5);
	printf("BIOS: %s\n", biosname);
	printf("\nGPU: %dMHz\n", (int)((pstates_info.pstates[0].clocks[0]).data.range.maxFreq_kHz) / 1000);
	printf("RAM: %dMHz\n", (int)((pstates_info.pstates[0].clocks[1]).data.single.freq_kHz) / 1000);
	printf("\nCurrent GPU OC: %dMHz\n", (int)((pstates_info.pstates[0].clocks[0]).freqDelta_kHz.value) / 1000);
	printf("Current RAM OC: %dMHz\n", (int)((pstates_info.pstates[0].clocks[1]).freqDelta_kHz.value) / 1000);

	if (argc > 1)
	{
		userfreq = atoi(argv[1]) * 1000;
		if (-250000 <= userfreq && userfreq <= 250000)
		{
			buf = (int*)malloc(0x1c94);
			memset(buf, 0, 0x1c94);
			buf[0] = 0x11c94; buf[2] = 1; buf[3] = 1;
			buf[10] = userfreq;
			NvSetPstates(hdlGPU[0], buf) ? printf("\nGPU OC failed!\n") : printf("\nGPU OC OK: %d MHz\n", userfreq / 1000);
			free(buf);
		}
		else
		{
			printf("\nGPU Frequency not in safe range (-250MHz to +250MHz).\n");
			return 1;
		}
	}
	if (argc > 2)
	{
		userfreq = atoi(argv[2]) * 1000;
		if (-250000 <= userfreq && userfreq <= 250000)
		{
			buf = (int*)malloc(0x1c94);
			memset(buf, 0, 0x1c94);
			buf[0] = 0x11c94; buf[2] = 1; buf[3] = 1;
			buf[7] = 4; buf[10] = memtype <= 7 ? userfreq : userfreq * 2;
			NvSetPstates(hdlGPU[0], buf) ? printf("VRAM OC failed!\n") : printf("RAM OC OK: %d MHz\n", userfreq / 1000);
			free(buf);
		}
		else
		{
			printf("\nRAM Frequency not in safe range (-250MHz to +250MHz).\n");
			return 1;
		}
	}

	NvUnload();
	return 0;
}