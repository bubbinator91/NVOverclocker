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
                    TextBox_GPU_Name.Text = gpuInfo.name;
                    TextBox_GPU_Bios.Text = gpuInfo.bios;
                    TextBox_GPU_vRam_Type.Text = gpuInfo.ramType;
                    TextBox_GPU_vRam_Size.Text = (gpuInfo.ramSize / 1024) + " MB";
                    TextBox_GPU_CoreClock.Text = (gpuInfo.currentCoreClock / 1000) + " MHz";
                    TextBox_GPU_vRamClock.Text = (gpuInfo.currentRamClock / 1000) + " MHz";
                    TextBox_GPU_CoreOverclock.Text = (gpuInfo.currentCoreOverclock / 1000) + " MHz";
                    TextBox_GPU_vRamOverclock.Text = (gpuInfo.currentRamOverclock / 1000) + " MHz";
                }
                else
                {
                    MessageBox.Show("Could not get info for GPU!", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                    Application.Current.Shutdown();
                }
            }
        }

        private void Button_Apply_Overclock_Click(object sender, RoutedEventArgs e)
        {

        }
    }
}
