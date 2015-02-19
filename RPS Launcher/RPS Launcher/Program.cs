using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using Microsoft.Win32;
using System.IO;
using System.Diagnostics;

namespace RPS_Launcher {
    static class Program {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static int Main(string[] args) {
            const string HKLM = "HKEY_LOCAL_MACHINE\\";
            const string HKCU = "HKEY_LOCAL_USER\\";
            string installOrDownload = "Please download the latest verson from www.abscreensavers.com and try re-installing the screensaver.";
            string appName = "Random Photo Screensaver";
            string regSubKey = "Software\\abScreensavers.com\\" + appName;
            string regName = "installFolder";
            string installDir = (string)Registry.GetValue(HKLM + regSubKey, regName, null);
            if (installDir == null) {
                installDir = (string)Registry.GetValue(HKCU + regSubKey, regName, null);
            }
            if (installDir == null) {
                MessageBox.Show("No registry entries found for " + appName + "." + Environment.NewLine +
                    "(Missing key: " + HKLM + regSubKey + "\\" + regName + Environment.NewLine + 
                    "or " + HKCU + regSubKey + "\\" + regName + ")" + Environment.NewLine + Environment.NewLine +
                    installOrDownload, "No Registry Entry Found",
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Error);
                return 3;
            }
            if (!Directory.Exists(installDir)) {
                MessageBox.Show("The " + appName + " installation folder does not exists:" + Environment.NewLine +
                    "(" + installDir + ")" + Environment.NewLine + Environment.NewLine +
                    installOrDownload, "Installation Folder Not Found",
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Error);
                return 3;
            }
            // Find file with latest creation date
            List<FileInfo> files = Directory.GetFiles(installDir, "RPS*.exe").Select(fn => new FileInfo(fn)).OrderByDescending(f => f.CreationTimeUtc).ToList();
            if (files.Count > 0) {
                Process p = Process.Start(Convert.ToString(files[0]), String.Join(" ", args));
                p.WaitForExit();
                // Check for updated version
            } else {
                MessageBox.Show("No " + appName + " executable found in:" + Environment.NewLine +
                    "(" + installDir + ")" + Environment.NewLine + Environment.NewLine +
                    installOrDownload, appName + " Not Found",
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Error);
                return 2;

            }
            Console.WriteLine(installDir);
            return 0;
        }
    }
}
