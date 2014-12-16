using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using Microsoft.Win32.TaskScheduler;
using System.Security.Cryptography;
using System.IO;
using System.Diagnostics;
using System.Windows.Forms;

namespace RPS {
    class Utils {
        public struct MSG {
            IntPtr hwnd;
            uint message;
            public IntPtr wParam;
            public IntPtr lParam;
            int time;
            int ptX;
            int ptY;
        }

        const int PM_NOREMOVE = 0x0000;
        const int PM_REMOVE = 0x0001;
        const int WM_KEYFIRST = 0x0100;
        const int WM_KEYLAST = 0x0109;
        
        #region Win32 API functions
        [DllImport("user32.dll")]
        static extern bool PeekMessage(out MSG lpMsg, int hWnd, uint wMsgFilterMin,uint wMsgFilterMax, uint wRemoveMsg);
        #endregion

        public static bool hasKeyMessage() {
            return hasKeyMessage(false);
        }

        public static bool hasKeyMessage(bool remove) {
            MSG msg;
            uint removeMsg = PM_NOREMOVE;
            if (remove) removeMsg = PM_REMOVE;
            bool r = PeekMessage(out msg, 0, WM_KEYFIRST, WM_KEYLAST, removeMsg);
            if (r) {
                Console.WriteLine(msg.wParam.ToString() + " " + msg.lParam.ToString());
            }
            return r;
            //return msg;
        }

        public static List<string> stringToList(string f) {
            string[] ff = new string[] { };
            if (f != null && f.Length > 0) {
                ff = f.Split(new string[] { ";", System.Environment.NewLine, "\n" }, StringSplitOptions.RemoveEmptyEntries);
            }
            return new List<string>(ff);
        }

        public static bool RunTaskScheduler(string taskName, string path) {
            return Utils.RunTaskScheduler(taskName, path, null);
        }

        public static bool RunTaskScheduler(string taskName, string path, string arguments) {
            try {
                // List of Windows versions at: http://msdn.microsoft.com/library/windows/desktop/ms724832.aspx
                // Windows XP
                if (Environment.OSVersion.Version.Major < 6) {
                    Process.Start(path, arguments);
                } else {
                    using (TaskService ts = new TaskService()) {
                        TaskDefinition td = ts.NewTask();
                        td.Actions.Add(new ExecAction(path, arguments, null));
                        ts.RootFolder.RegisterTaskDefinition(taskName, td);
                        Microsoft.Win32.TaskScheduler.Task t = ts.FindTask(taskName);
                        if (t != null) t.Run();
                        ts.RootFolder.DeleteTask(taskName);
                    }
                }
            } catch (Exception e) {
                return false;
            }
            return true;
        }

        public static bool VerifyMD5(string path, string md5verify) {
            if (!File.Exists(path)) return false;
            using (var md5 = MD5.Create()) {
                using (var stream = File.OpenRead(path)) {
                    return BitConverter.ToString(md5.ComputeHash(stream)).Replace("-", string.Empty).ToLower() == md5verify.ToLower();
                }
            }
        }
    }
}
