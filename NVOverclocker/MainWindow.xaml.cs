using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
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
using MahApps.Metro.Controls.Dialogs;

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

            UpdateGPUInfo();
        }

        private void TextBox_GPU_CoreOverclock_GotFocus(object sender, RoutedEventArgs e)
        {
            TextBox_GPU_CoreOverclock.Text = TextBox_GPU_CoreOverclock.Text.Substring(0, TextBox_GPU_CoreOverclock.Text.Length - 4);
        }

        private void TextBox_GPU_CoreOverclock_LostFocus(object sender, RoutedEventArgs e)
        {
            if (String.IsNullOrWhiteSpace(TextBox_GPU_CoreOverclock.Text))
            {
                TextBox_GPU_CoreOverclock.Text = "0";
            }
            TextBox_GPU_CoreOverclock.Text = TextBox_GPU_CoreOverclock.Text + " MHz";
        }

        private void TextBox_GPU_vRamOverclock_GotFocus(object sender, RoutedEventArgs e)
        {
            TextBox_GPU_vRamOverclock.Text = TextBox_GPU_vRamOverclock.Text.Substring(0, TextBox_GPU_vRamOverclock.Text.Length - 4);
        }

        private void TextBox_GPU_vRamOverclock_LostFocus(object sender, RoutedEventArgs e)
        {
            if (String.IsNullOrWhiteSpace(TextBox_GPU_vRamOverclock.Text))
            {
                TextBox_GPU_vRamOverclock.Text = "0";
            }
            TextBox_GPU_vRamOverclock.Text = TextBox_GPU_vRamOverclock.Text + " MHz";
        }

        private void TextBoxValidateInput(object sender, TextCompositionEventArgs e)
        {
            var regex = new Regex("[^0-9.-]+");
            e.Handled = regex.IsMatch(e.Text);
        }

        private async void Button_Apply_Overclock_Click(object sender, RoutedEventArgs e)
        {
            if (numGPUs > 0)
            {
                Int32.TryParse(TextBox_GPU_CoreOverclock.Text.Substring(0, TextBox_GPU_CoreOverclock.Text.Length - 4), out Int32 coreOverclock);
                Int32.TryParse(TextBox_GPU_vRamOverclock.Text.Substring(0, TextBox_GPU_vRamOverclock.Text.Length - 4), out Int32 ramOverclock);

                if (NvidiaUtil.SetOverclock((UInt32)numGPUs, coreOverclock, ramOverclock))
                {
                    await this.ShowMessageAsync("Success!", "Overclock applied successfully...", MessageDialogStyle.Affirmative, new MetroDialogSettings() { DialogMessageFontSize = 12, DialogTitleFontSize = 15 });
                }
                else
                {
                    await this.ShowMessageAsync("Failed!", "Failed to apply overclock...", MessageDialogStyle.Affirmative, new MetroDialogSettings() { DialogMessageFontSize = 12, DialogTitleFontSize = 15 });
                }

                UpdateGPUInfo();
            }
        }

        private void UpdateGPUInfo()
        {
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
    }
}
