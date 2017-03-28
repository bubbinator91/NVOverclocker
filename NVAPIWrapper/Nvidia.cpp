#include <exception>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <tchar.h>

#include "Nvidia.h"
#include "Util.h"

extern "C"
{
    __declspec(dllexport) INT32 GetNumberOfGPUs()
    {
        NvAPI_QueryInterface_t NvQueryInterface = 0;
        NvAPI_Initialize_t NvInit = 0;
        NvAPI_Unload_t NvUnload = 0;
        NvAPI_EnumPhysicalGPUs_t NvEnumGPUs = 0;
        int *gpuHandles[64] = { 0 };
        INT32 numGPUs = -1;

        // Try loading the 32bit nvapi library
        NvQueryInterface = (NvAPI_QueryInterface_t)GetProcAddress(LoadLibrary(_T("nvapi.dll")), "nvapi_QueryInterface");

        // If the 32bit nvapi library didn't work, try the 64bit one
        if (NvQueryInterface == 0)
        {
            NvQueryInterface = (NvAPI_QueryInterface_t)GetProcAddress(LoadLibrary(_T("nvapi64.dll")), "nvapi_QueryInterface");
        }

        // Return false if neither worked, otherwise return true
        if (NvQueryInterface == 0)
        {
            return -1;
        }

        NvInit = (NvAPI_Initialize_t)NvQueryInterface(0x0150E828);
        NvUnload = (NvAPI_Unload_t)NvQueryInterface(0xD22BDD7E);
        NvEnumGPUs = (NvAPI_EnumPhysicalGPUs_t)NvQueryInterface(0xE5AC921F);

        NvInit();
        NvEnumGPUs(gpuHandles, &numGPUs);
        NvUnload();

        return numGPUs;
    }

    __declspec(dllexport) bool GetGPUInfo(_In_ UINT32 gpuNum, _Out_ GPUInfo* gpuInfo)
    {
        if (gpuNum <= 0)
        {
            return false;
        }

        UINT32 gpuIndex = gpuNum - 1;

        NvAPI_QueryInterface_t NvQueryInterface = 0;
        NvAPI_Initialize_t NvInit = 0;
        NvAPI_Unload_t NvUnload = 0;
        NvAPI_EnumPhysicalGPUs_t NvEnumGPUs = 0;
        NvAPI_GPU_GetFullName_t NvGetName = 0;
        NvAPI_GPU_GetPhysicalFrameBufferSize_t NvGetMemSize = 0;
        NvAPI_GPU_GetRamType_t NvGetMemType = 0;
        NvAPI_GPU_GetVbiosVersionString_t NvGetBiosName = 0;
        NvAPI_GPU_GetAllClockFrequencies_t NvGetFreq = 0;
        NvAPI_GPU_GetPstates20_t NvGetPstates = 0;
        int *gpuHandles[64] = { 0 };
        INT32 numGPUs = -1;

        // Try loading the 32bit nvapi library
        NvQueryInterface = (NvAPI_QueryInterface_t)GetProcAddress(LoadLibrary(_T("nvapi.dll")), "nvapi_QueryInterface");

        // If the 32bit nvapi library didn't work, try the 64bit one
        if (NvQueryInterface == 0)
        {
            NvQueryInterface = (NvAPI_QueryInterface_t)GetProcAddress(LoadLibrary(_T("nvapi64.dll")), "nvapi_QueryInterface");
        }

        // Return false if neither worked
        if (NvQueryInterface == 0)
        {
            return false;
        }

        NvInit = (NvAPI_Initialize_t)NvQueryInterface(0x0150E828);
        NvUnload = (NvAPI_Unload_t)NvQueryInterface(0xD22BDD7E);
        NvEnumGPUs = (NvAPI_EnumPhysicalGPUs_t)NvQueryInterface(0xE5AC921F);
        NvGetName = (NvAPI_GPU_GetFullName_t)NvQueryInterface(0xCEEE8E9F);
        NvGetMemSize = (NvAPI_GPU_GetPhysicalFrameBufferSize_t)NvQueryInterface(0x46FBEB03);
        NvGetMemType = (NvAPI_GPU_GetRamType_t)NvQueryInterface(0x57F7CAAC);
        NvGetBiosName = (NvAPI_GPU_GetVbiosVersionString_t)NvQueryInterface(0xA561FD7D);
        NvGetFreq = (NvAPI_GPU_GetAllClockFrequencies_t)NvQueryInterface(0xDCB616C3);
        NvGetPstates = (NvAPI_GPU_GetPstates20_t)NvQueryInterface(0x6FF81213);

        NvInit();
        NvEnumGPUs(gpuHandles, &numGPUs);

        if ((gpuIndex >= 0) && (static_cast<INT32>(gpuIndex) < numGPUs))
        {
            if (gpuInfo == nullptr)
            {
                gpuInfo = new GPUInfo();
            }

            size_t outSize = 0;
            NV_GPU_PERF_PSTATES20_INFO_V1 pstates_info;
            pstates_info.version = 0x11c94;
            NvGetPstates(gpuHandles[gpuIndex], &pstates_info);

            char name[64] = { 0 };
            NvGetName(gpuHandles[gpuIndex], name);
            mbstowcs_s(&outSize, gpuInfo->name, name, static_cast<size_t>(strlen(name) + 1));

            char bios[64] = { 0 };
            NvGetBiosName(gpuHandles[gpuIndex], bios);
            mbstowcs_s(&outSize, gpuInfo->bios, bios, static_cast<size_t>(strlen(bios) + 1));

            int memtype = 0;
            NvGetMemType(gpuHandles[gpuIndex], &memtype);
            std::string ramtype = std::string("GDDR");
            ramtype.append((memtype <= 7) ? "3" : "5");
            mbstowcs_s(&outSize, gpuInfo->ramType, ramtype.c_str(), static_cast<size_t>(ramtype.length()));

            NvGetMemSize(gpuHandles[gpuIndex], &gpuInfo->ramSize);
            gpuInfo->currentCoreClock = static_cast<UINT32>((pstates_info.pstates[0].clocks[0]).data.range.maxFreq_kHz);
            gpuInfo->currentRamClock = static_cast<UINT32>((pstates_info.pstates[0].clocks[1]).data.single.freq_kHz);
            gpuInfo->currentCoreOverclock = static_cast<INT32>((pstates_info.pstates[0].clocks[0]).freqDelta_kHz.value);
            gpuInfo->currentRamOverclock = static_cast<INT32>((pstates_info.pstates[0].clocks[1]).freqDelta_kHz.value);

            NvUnload();
            return true;
        }
        else
        {
            NvUnload();
            return false;
        }
    }

    // Sets the overclock for the given GPU.
    //
    // Parameters:
    //    gpuNum - The index of the GPU. Greater than or equal to 1.
    //    coreOverclock - The MHz value to add to the current core clock speed.
    //    ramOverclock - The MHz value to add to the current ram clock speed.
    //
    // Return:
    //    True upon success; false on failure or refusal to apply overclock.
    __declspec(dllexport) bool SetOverclock(_In_ UINT32 gpuNum, _In_ INT32 coreOverclock, _In_ INT32 ramOverclock)
    {
        if (gpuNum <= 0)
        {
            return false;
        }

        UINT32 gpuIndex = gpuNum - 1;

        NvAPI_QueryInterface_t NvQueryInterface = 0;
        NvAPI_Initialize_t NvInit = 0;
        NvAPI_Unload_t NvUnload = 0;
        NvAPI_EnumPhysicalGPUs_t NvEnumGPUs = 0;
        NvAPI_GPU_GetRamType_t NvGetMemType = 0;
        NvAPI_GPU_SetPstates20_t NvSetPstates = 0;
        int *gpuHandles[64] = { 0 };
        int memtype = 0;
        INT32 numGPUs = -1;

        // Try loading the 32bit nvapi library
        NvQueryInterface = (NvAPI_QueryInterface_t)GetProcAddress(LoadLibrary(_T("nvapi.dll")), "nvapi_QueryInterface");

        // If the 32bit nvapi library didn't work, try the 64bit one
        if (NvQueryInterface == 0)
        {
            NvQueryInterface = (NvAPI_QueryInterface_t)GetProcAddress(LoadLibrary(_T("nvapi64.dll")), "nvapi_QueryInterface");
        }

        // Return false if neither worked
        if (NvQueryInterface == 0)
        {
            return false;
        }

        NvInit = (NvAPI_Initialize_t)NvQueryInterface(0x0150E828);
        NvUnload = (NvAPI_Unload_t)NvQueryInterface(0xD22BDD7E);
        NvEnumGPUs = (NvAPI_EnumPhysicalGPUs_t)NvQueryInterface(0xE5AC921F);
        NvGetMemType = (NvAPI_GPU_GetRamType_t)NvQueryInterface(0x57F7CAAC);
        NvSetPstates = (NvAPI_GPU_SetPstates20_t)NvQueryInterface(0x0F4DAE6B);

        NvInit();
        NvEnumGPUs(gpuHandles, &numGPUs);

        if ((gpuIndex >= 0) && (static_cast<INT32>(gpuIndex) < numGPUs))
        {
            NvGetMemType(gpuHandles[gpuIndex], &memtype);

            if ((coreOverclock <= 250) && (coreOverclock >= -250))
            {
                int *buf = new int[0x1c94];
                std::fill(buf, buf + 0x1c94, 0);
                buf[0] = 0x11c94; buf[2] = 1; buf[3] = 1;
                buf[10] = static_cast<int>(coreOverclock * 1000);
                if (NvSetPstates(gpuHandles[gpuIndex], buf))
                {
                    delete[] buf;
                    return false;
                }
                delete[] buf;
            }
            else
            {
                return false;
            }

            if ((ramOverclock <= 250) && (ramOverclock >= -250))
            {
                int *buf = new int[0x1c94];
                std::fill(buf, buf + 0x1c94, 0);
                buf[0] = 0x11c94; buf[2] = 1; buf[3] = 1; buf[7] = 4;
                buf[10] = (memtype <= 7) ? static_cast<int>(ramOverclock * 1000) : static_cast<int>(ramOverclock * 1000 * 2);
                if (NvSetPstates(gpuHandles[gpuIndex], buf))
                {
                    delete[] buf;
                    return false;
                }
                delete[] buf;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }

        return true;
    }
}