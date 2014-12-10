using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices; 

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


    }
}
