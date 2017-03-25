using System;
using System.Runtime.InteropServices;

namespace NvidiaOverclockCSTest
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

    public class Program
    {
        [DllImport("NVAPIWrapper.dll")]
        static extern Int32 GetNumberOfGPUs();

        [DllImport("NVAPIWrapper.dll", CharSet = CharSet.Unicode)]
        static extern bool GetGPUInfo([In] UInt32 gpuIndex, [In, Out] ref GPUInfo gpuInfo);

        static void Main(string[] args)
        {
            var numGPUs = GetNumberOfGPUs();

            if (numGPUs <= 0)
            {
                Console.WriteLine("Could not get GPU data!");
            }
            else
            {
                Console.WriteLine("Num GPUs: " + numGPUs + "\n");
                for (UInt32 i = 1; i <= numGPUs; i++)
                {
                    var gpuInfo = new GPUInfo();
                    if (GetGPUInfo(i, ref gpuInfo))
                    {
                        Console.WriteLine("GPU " + i +  ":");
                        Console.WriteLine("\tName: " + gpuInfo.name);
                        Console.WriteLine("\tBIOS: " + gpuInfo.bios);
                        Console.WriteLine("\tvRam: " + (gpuInfo.ramSize / 1024) + "MB " + gpuInfo.ramType);
                        Console.WriteLine("\tCore Clock: " + (gpuInfo.currentCoreClock / 1000) + "MHz");
                        Console.WriteLine("\tvRam Clock: " + (gpuInfo.currentRamClock / 1000) + "MHz");
                        Console.WriteLine("\tCore Overclock: " + (gpuInfo.currentCoreOverclock / 1000) + "MHz");
                        Console.WriteLine("\tvRam Overclock: " + (gpuInfo.currentRamOverclock / 1000) + "MHz");
                        Console.WriteLine("");
                    }
                    else
                    {
                        Console.WriteLine("Could not get info for GPU " + i + "!\n");
                    }
                }
            }
        }
    }
}
