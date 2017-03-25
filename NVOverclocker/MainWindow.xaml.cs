using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

using MahApps.Metro.Controls;

using NVOverclocker.Util;

namespace NVOverclocker
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : MetroWindow
    {
        private int numGPUs;

        public MainWindow()
        {
            InitializeComponent();

            numGPUs = NvidiaUtil.GetNumberOfGPUs();

            if (numGPUs <= 0)
            {
                MessageBox.Show("Could not get GPU data!", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                Application.Current.Shutdown();
            }
            else
            {
                var gpuInfo = new GPUInfo();
                if (NvidiaUtil.GetGPUInfo((UInt32)numGPUs, ref gpuInfo))
                {
                    Label_GPU_Name_Text.Content = gpuInfo.name;
                    Label_GPU_Bios_Text.Content = gpuInfo.bios;
                    Label_GPU_vRam_Text.Content = (gpuInfo.ramSize / 1024) + "MB " + gpuInfo.ramType;
                    Label_GPU_CoreClock_Text.Content = (gpuInfo.currentCoreClock / 1000) + "MHz";
                    Label_GPU_vRamClock_Text.Content = (gpuInfo.currentRamClock / 1000) + "MHz";
                    Label_GPU_CoreOverclock_Text.Content = (gpuInfo.currentCoreOverclock / 1000) + "MHz";
                    Label_GPU_vRamOverclock_Text.Content = (gpuInfo.currentRamOverclock / 1000) + "MHz";
                }
                else
                {
                    MessageBox.Show("Could not get info for GPU!", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                    Application.Current.Shutdown();
                }
            }
        }
    }
}
