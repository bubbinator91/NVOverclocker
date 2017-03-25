#ifndef UTIL_H
#define UTIL_H

#include <Windows.h>

struct GPUInfo
{
    WCHAR name[64] = { 0 };
    WCHAR bios[64] = { 0 };
    WCHAR ramType[6] = { 0 };
    INT32 ramSize;
    UINT32 currentCoreClock;
    UINT32 currentRamClock;
    INT32 currentCoreOverclock;
    INT32 currentRamOverclock;
};

#endif
