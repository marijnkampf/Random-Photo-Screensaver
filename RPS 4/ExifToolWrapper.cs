/***
 * Based on code by brain2cpu http://u88.n24.queensu.ca/exiftool/forum/index.php?topic=5262.0
 ***/

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using System.Management;

namespace ExifTool
{
    public class Wrapper : IDisposable
    {
        private readonly string exe;
        //private const string ExeName = "exiftool(-k).exe";
        private const string Arguments = "-fast -m -q -q -stay_open True -@ - -common_args -d \"%Y.%m.%d %H:%M:%S\" -t";   //-g for groups

        public enum Statuses { Stopped, Starting, Ready, Stopping };
        public Statuses Status { get; private set; }

        private int cmdCnt = 1;
        private readonly StringBuilder output = new StringBuilder();
        private readonly ProcessStartInfo psi;
        private Process proc = null;
        private readonly ManualResetEvent waitHandle = new ManualResetEvent(true);

        public string ExiftoolVersion { get; private set; }

        public Wrapper(string path = null)
        {
            if (path != null && Path.IsPathRooted(path)) {
                exe = path;
            } else {
                exe = Path.Combine(Path.GetDirectoryName(Assembly.GetEntryAssembly().Location), path);
            }
            //exe = string.IsNullOrEmpty(path) ? Path.Combine(Path.GetDirectoryName(Assembly.GetEntryAssembly().Location), ExeName) : path;
            //MessageBox.Show(exe);// + "\r\n\r\n" + Path.Combine(Path.GetDirectoryName(Assembly.GetEntryAssembly().Location), ExeName) + "\r\n" + path + "\r\n\r\n" + Assembly.GetEntryAssembly().Location);

            if (!File.Exists(exe)) {
                exe = "." + exe;
            }

            if (!File.Exists(exe))
                throw new FileNotFoundException(exe + " not found");

            psi = new ProcessStartInfo
            {
                FileName = exe,
                Arguments = Arguments,
                CreateNoWindow = true,
                UseShellExecute = false,
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                RedirectStandardInput = true
            };

            Status = Statuses.Stopped;
        }

        ~Wrapper() {
            this.Stop();
        }

        private void OutputDataReceived(object sender, DataReceivedEventArgs e)
        {
            if(string.IsNullOrEmpty(e.Data))
                return;

            if(Status == Statuses.Starting)
            {
                ExiftoolVersion = e.Data;
                waitHandle.Set();
            }
            else
            {
                if(e.Data.ToLower() == string.Format("{{ready{0}}}", cmdCnt))
                    waitHandle.Set();
                else
                    output.AppendLine(e.Data);
            }
        }

        public bool Started() {
            return (Status == Statuses.Ready || Status == Statuses.Starting);
        }

        // Start exiftool wrapper if not started alread.
        public void Starter() {
            if (!Started()) {
                this.Start();
            }
        }

        public void Start()
        {
            if(Status != Statuses.Stopped)
                throw new InvalidOperationException("Process is not stopped");

            Status = Statuses.Starting;

            this.proc = new Process { StartInfo = psi, EnableRaisingEvents = true };
            this.proc.OutputDataReceived += OutputDataReceived;
            this.proc.Exited += proc_Exited;
            this.proc.Start();

            this.proc.BeginOutputReadLine();

            waitHandle.Reset();
            this.proc.StandardInput.WriteLine("-ver\n-execute0000");
            waitHandle.WaitOne();

            Status = Statuses.Ready;
        }

        //detect if process is killed
        void proc_Exited(object sender, EventArgs e)
        {
            if (this.proc != null)
            {
                this.proc.Dispose();
                this.proc = null;
            }

            Status = Statuses.Stopped;

            waitHandle.Set();
        }

        /// <summary>
        /// Kill a process, and all of its children, grandchildren, etc.
        /// </summary>
        /// <param name="pid">Process ID.</param>
        private static void KillProcessAndChildren(Process p) {
            KillProcessAndChildren(p.Id);
        }
        private static void KillProcessAndChildren(int pid) {
            ManagementObjectSearcher searcher = new ManagementObjectSearcher
              ("Select * From Win32_Process Where ParentProcessID=" + pid);
            ManagementObjectCollection moc = searcher.Get();
            foreach (ManagementObject mo in moc) {
                KillProcessAndChildren(Convert.ToInt32(mo["ProcessID"]));
            }
            try {
                Process proc = Process.GetProcessById(pid);
                proc.Kill();
            } catch (ArgumentException) {
                // Process already exited.
            }
        }

        public void Stop()
        {
            //if (Status == Statuses.Stopped) return;
            if(Status != Statuses.Ready)
                throw new InvalidOperationException("Process must be ready");

            Status = Statuses.Stopping;

            waitHandle.Reset();
            try {
                if (this.proc != null) this.proc.StandardInput.WriteLine("-stay_open\nFalse\n");
            } catch (System.ObjectDisposedException ode) {
            }
            Wrapper.KillProcessAndChildren(this.proc);
            Status = Statuses.Stopped;
/*            if (!waitHandle.WaitOne(2500))
            {
                if (this.proc != null)
                {
                    //silently swallow an eventual exception
                    try
                    {
                        Wrapper.KillProcessAndChildren(this.proc);
                        //this.proc.CloseMainWindow();
                        //this.proc.Kill();
                        this.proc.WaitForExit(2000);
                        this.proc.Dispose();
                    }
                    catch { }

                    this.proc = null;
                }

                Status = Statuses.Stopped;
            }*/
        }

        public string SendCommand(string cmd)
        {
            if(Status != Statuses.Ready)
                throw new InvalidOperationException("Process must be ready");

            waitHandle.Reset();
            this.proc.StandardInput.WriteLine("{0}\n-execute{1}", cmd, cmdCnt);
            waitHandle.WaitOne();

            cmdCnt++;

            string r = output.ToString();
            output.Clear();

            return r;
        }

        public Dictionary<string, string> FetchExifFrom(string path)
        {
            Dictionary<string, string> res = new Dictionary<string, string>();

            string sRes = SendCommand(path);
            foreach(string s in sRes.Split(new [] { Environment.NewLine }, StringSplitOptions.RemoveEmptyEntries))
            {
                string[] kv = s.Split('\t');
                if(kv.Length == 2)
                    res[kv[0]] = kv[1];
                
                Debug.Assert(kv.Length == 2, "Can not parse line :'" + s + "'");
            }

            return res;
        }

        #region IDisposable Members

        public void Dispose()
        {
            Debug.Assert(Status == Statuses.Ready || Status == Statuses.Stopped, "Invalid state");

            if(proc != null && Status == Statuses.Ready)
                Stop();

            waitHandle.Dispose();
        }

        #endregion
    }
}
