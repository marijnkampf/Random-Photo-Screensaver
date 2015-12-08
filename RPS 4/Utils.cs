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
using System.Globalization;
using System.Collections;
using System.Collections.Concurrent;

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
                //Console.WriteLine(msg.wParam.ToString() + " " + msg.lParam.ToString());
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

        public static ConcurrentQueue<string> stringToConcurrentQueue(string f) {
            ConcurrentQueue<string> cq = new ConcurrentQueue<string>();

            string[] ff = new string[] { };
            if (f != null && f.Length > 0) {
                ff = f.Split(new string[] { ";", System.Environment.NewLine, "\n" }, StringSplitOptions.RemoveEmptyEntries);
                for (int i = 0; i < ff.Length; i++) {
                    cq.Enqueue(ff[i]);
                }
            }
            return cq;
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

        private static readonly char[] s_entityEndingChars = new char[] { ';', '&' };

        // Ensure string has trailing slash
        public static string addTrailingSlash(string s) {
            return s.TrimEnd(Path.DirectorySeparatorChar) + Path.DirectorySeparatorChar;
        }

        public static string HtmlDecode(string html) {
            if (html == null) {
                return null;
            }
            if (html.IndexOf('&') < 0) {
                return html;
            }
            StringBuilder sb = new StringBuilder();
            StringWriter writer = new StringWriter(sb, CultureInfo.InvariantCulture);
            int length = html.Length;
            for (int i = 0; i < length; i++) {
                char ch = html[i];
                if (ch == '&') {
                    int num3 = html.IndexOfAny(s_entityEndingChars, i + 1);
                    if ((num3 > 0) && (html[num3] == ';')) {
                        string entity = html.Substring(i + 1, (num3 - i) - 1);
                        if ((entity.Length > 1) && (entity[0] == '#')) {
                            try {
                                if ((entity[1] == 'x') || (entity[1] == 'X')) {
                                    ch = (char)int.Parse(entity.Substring(2), NumberStyles.AllowHexSpecifier, CultureInfo.InvariantCulture);
                                } else {
                                    ch = (char)int.Parse(entity.Substring(1), CultureInfo.InvariantCulture);
                                }
                                i = num3;
                            } catch (FormatException) {
                                i++;
                            } catch (ArgumentException) {
                                i++;
                            }
                        } else {
                            i = num3;
                            char ch2 = HtmlEntities.Lookup(entity);
                            if (ch2 != '\0') {
                                ch = ch2;
                            } else {
                                writer.Write('&');
                                writer.Write(entity);
                                writer.Write(';');
                                continue;
                            }
                        }
                    }
                }
                writer.Write(ch);
            }
            return sb.ToString();
        }

        public class HtmlEntities {
            private static readonly string[] _entitiesList = new string[] {
                "\"-quot", "&-amp", "<-lt", ">-gt", "\x00a0-nbsp", "\x00a1-iexcl", "\x00a2-cent", "\x00a3-pound", "\x00a4-curren", "\x00a5-yen", "\x00a6-brvbar", "\x00a7-sect", "\x00a8-uml", "\x00a9-copy", "\x00aa-ordf", "\x00ab-laquo",
                "\x00ac-not", "\x00ad-shy", "\x00ae-reg", "\x00af-macr", "\x00b0-deg", "\x00b1-plusmn", "\x00b2-sup2", "\x00b3-sup3", "\x00b4-acute", "\x00b5-micro", "\x00b6-para", "\x00b7-middot", "\x00b8-cedil", "\x00b9-sup1", "\x00ba-ordm", "\x00bb-raquo",
                "\x00bc-frac14", "\x00bd-frac12", "\x00be-frac34", "\x00bf-iquest", "\x00c0-Agrave", "\x00c1-Aacute", "\x00c2-Acirc", "\x00c3-Atilde", "\x00c4-Auml", "\x00c5-Aring", "\x00c6-AElig", "\x00c7-Ccedil", "\x00c8-Egrave", "\x00c9-Eacute", "\x00ca-Ecirc", "\x00cb-Euml",
                "\x00cc-Igrave", "\x00cd-Iacute", "\x00ce-Icirc", "\x00cf-Iuml", "\x00d0-ETH", "\x00d1-Ntilde", "\x00d2-Ograve", "\x00d3-Oacute", "\x00d4-Ocirc", "\x00d5-Otilde", "\x00d6-Ouml", "\x00d7-times", "\x00d8-Oslash", "\x00d9-Ugrave", "\x00da-Uacute", "\x00db-Ucirc",
                "\x00dc-Uuml", "\x00dd-Yacute", "\x00de-THORN", "\x00df-szlig", "\x00e0-agrave", "\x00e1-aacute", "\x00e2-acirc", "\x00e3-atilde", "\x00e4-auml", "\x00e5-aring", "\x00e6-aelig", "\x00e7-ccedil", "\x00e8-egrave", "\x00e9-eacute", "\x00ea-ecirc", "\x00eb-euml",
                "\x00ec-igrave", "\x00ed-iacute", "\x00ee-icirc", "\x00ef-iuml", "\x00f0-eth", "\x00f1-ntilde", "\x00f2-ograve", "\x00f3-oacute", "\x00f4-ocirc", "\x00f5-otilde", "\x00f6-ouml", "\x00f7-divide", "\x00f8-oslash", "\x00f9-ugrave", "\x00fa-uacute", "\x00fb-ucirc",
                "\x00fc-uuml", "\x00fd-yacute", "\x00fe-thorn", "\x00ff-yuml", "Œ-OElig", "œ-oelig", "Š-Scaron", "š-scaron", "Ÿ-Yuml", "ƒ-fnof", "ˆ-circ", "˜-tilde", "Α-Alpha", "Β-Beta", "Γ-Gamma", "Δ-Delta",
                "Ε-Epsilon", "Ζ-Zeta", "Η-Eta", "Θ-Theta", "Ι-Iota", "Κ-Kappa", "Λ-Lambda", "Μ-Mu", "Ν-Nu", "Ξ-Xi", "Ο-Omicron", "Π-Pi", "Ρ-Rho", "Σ-Sigma", "Τ-Tau", "Υ-Upsilon",
                "Φ-Phi", "Χ-Chi", "Ψ-Psi", "Ω-Omega", "α-alpha", "β-beta", "γ-gamma", "δ-delta", "ε-epsilon", "ζ-zeta", "η-eta", "θ-theta", "ι-iota", "κ-kappa", "λ-lambda", "μ-mu",
                "ν-nu", "ξ-xi", "ο-omicron", "π-pi", "ρ-rho", "ς-sigmaf", "σ-sigma", "τ-tau", "υ-upsilon", "φ-phi", "χ-chi", "ψ-psi", "ω-omega", "ϑ-thetasym", "ϒ-upsih", "ϖ-piv",
                " -ensp", " -emsp", " -thinsp", "‌-zwnj", "‍-zwj", "‎-lrm", "‏-rlm", "–-ndash", "—-mdash", "‘-lsquo", "’-rsquo", "‚-sbquo", "“-ldquo", "”-rdquo", "„-bdquo", "†-dagger",
                "‡-Dagger", "•-bull", "…-hellip", "‰-permil", "′-prime", "″-Prime", "‹-lsaquo", "›-rsaquo", "‾-oline", "⁄-frasl", "€-euro", "ℑ-image", "℘-weierp", "ℜ-real", "™-trade", "ℵ-alefsym",
                "←-larr", "↑-uarr", "→-rarr", "↓-darr", "↔-harr", "↵-crarr", "⇐-lArr", "⇑-uArr", "⇒-rArr", "⇓-dArr", "⇔-hArr", "∀-forall", "∂-part", "∃-exist", "∅-empty", "∇-nabla",
                "∈-isin", "∉-notin", "∋-ni", "∏-prod", "∑-sum", "−-minus", "∗-lowast", "√-radic", "∝-prop", "∞-infin", "∠-ang", "∧-and", "∨-or", "∩-cap", "∪-cup", "∫-int",
                "∴-there4", "∼-sim", "≅-cong", "≈-asymp", "≠-ne", "≡-equiv", "≤-le", "≥-ge", "⊂-sub", "⊃-sup", "⊄-nsub", "⊆-sube", "⊇-supe", "⊕-oplus", "⊗-otimes", "⊥-perp",
                "⋅-sdot", "⌈-lceil", "⌉-rceil", "⌊-lfloor", "⌋-rfloor", "〈-lang", "〉-rang", "◊-loz", "♠-spades", "♣-clubs", "♥-hearts", "♦-diams"
             };

            public static char Lookup(string entity) {
                Hashtable hashtable = new Hashtable();
                foreach (string str in _entitiesList) {
                    hashtable[str.Substring(2)] = str[0];
                }

                object obj2 = hashtable[entity];
                if (obj2 != null) {
                    return (char)obj2;
                }
                return '\0';
            }
        }
    }
}
