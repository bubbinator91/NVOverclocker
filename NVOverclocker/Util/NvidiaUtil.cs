using System;
using System.Runtime.InteropServices;

namespace NVOverclocker.Util
{
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    public struct GPUInfo
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public String name;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
        public String bios;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 6)]
        public String ramType;
        public Int32 ramSize;
        public UInt32 currentCoreClock;
        public UInt32 currentRamClock;
        public Int32 currentCoreOverclock;
        public Int32 currentRamOverclock;
    }

    public class NvidiaUtil
    {
        [DllImport("NVAPIWrapper.dll")]
        public static extern Int32 GetNumberOfGPUs();

        [DllImport("NVAPIWrapper.dll", CharSet = CharSet.Unicode)]
        public static extern bool GetGPUInfo([In] UInt32 gpuIndex, [In, Out] ref GPUInfo gpuInfo);

        [DllImport("NVAPIWrapper.dll")]
        public static extern bool SetOverclock([In] UInt32 gpuIndex, [In] Int32 coreOverclock, [In] Int32 ramOverclock);
    }
}
