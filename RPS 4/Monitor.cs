using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using Microsoft.Win32;
using System.Security.Permissions;
using System.Linq;
using System.Diagnostics;
using System.IO;
using System.Collections;
using Newtonsoft.Json;

namespace RPS {
    [PermissionSet(SecurityAction.Demand, Name = "FullTrust")]
    [System.Runtime.InteropServices.ComVisibleAttribute(true)]

    public partial class Monitor : Form {
        private int id;
        private Screensaver screensaver;

        // Random photo history
        public List<long> history;
        public int historyPointer = -1;
        public int historyOffset = 0;

        public int offset = 1;

        DataRow currentImage;
        MetadataTemplate quickMetadata;

        #region Win32 API functions

        [DllImport("user32.dll")]
        static extern IntPtr SetParent(IntPtr hWndChild, IntPtr hWndNewParent);

        [DllImport("user32.dll")]
        static extern int SetWindowLong(IntPtr hWnd, int nIndex, IntPtr dwNewLong);

        [DllImport("user32.dll", SetLastError = true)]
        static extern int GetWindowLong(IntPtr hWnd, int nIndex);

        [DllImport("user32.dll")]
        static extern bool GetClientRect(IntPtr hWnd, out Rectangle lpRect);

        #endregion

        public bool RunningFromRPS() {
            return true;
        }

        public Monitor(int id, Screensaver screensaver) {
            InitializeComponent();
//            MessageBox.Show("Monitor: " + id);
            this.screensaver = screensaver;
            this.id = id;
            this.Text = "Monitor " + Convert.ToString(this.id+1) + " Random Photo Screensaver";
            this.history = new List<long>();
            this.browser.DocumentCompleted += new System.Windows.Forms.WebBrowserDocumentCompletedEventHandler(this.DocumentCompleted);
        }

        public Monitor(Rectangle Bounds, int id, Screensaver screensaver): this(id, screensaver) {
            this.Bounds = Bounds;
        }

        public Monitor(IntPtr previewHwnd, int id, Screensaver screensaver): this(id, screensaver) {
            // Set the preview window as the parent of this window
            SetParent(this.Handle, previewHwnd);

            // Make this a child window so it will close when the parent dialog closes
            SetWindowLong(this.Handle, -16, new IntPtr(GetWindowLong(this.Handle, -16) | 0x40000000));

            // Place our window inside the parent
            //Rectangle ParentRect;
            //GetClientRect(previewHwnd, out ParentRect);
            //Size = ParentRect.Size;
            //Location = new Point(40, 40);
            //            this.Bounds = ParentRect;
            //this.browser.Scale(new SizeF((float)5, (float)5));
        }

        private void DocumentCompleted(object sender, System.Windows.Forms.WebBrowserDocumentCompletedEventArgs e) {
            // Correct document loaded (not about:blank)
            //MessageBox.Show("Monitor Document Completed: " + Constants.getDataFolder(Constants.ConfigHtmlFile));

            if (this.browser.Url.Segments.Last().Equals(Constants.MonitorHtmlFile)) {
                HtmlElementCollection elems = this.browser.Document.GetElementsByTagName("body");
                foreach (HtmlElement elem in elems) {
                    string classes = "monitor" + (this.id+1);
                    if (this.screensaver.monitors.Length > 1) classes += " multimonitor";
                    elem.SetAttribute("className", elem.GetAttribute("className") + classes);
                    classes = null;                    
                }
                this.browser.Document.GetElementById("identify").InnerText = Convert.ToString(this.id+1);
                this.browser.Show();
                // Focus browser to ensure key strokes are previewed
//                this.browser.PreviewKeyDown += this.screensaver.PreviewKeyDown;
                this.browser.Focus();
                
                // Remove form's PreviewKeyDown to avoid duplicated keys
                this.PreviewKeyDown -= this.screensaver.PreviewKeyDown;

                // Set defaults for monitor.html
                this.InvokeScript("setBackgroundColour", new string[] { this.screensaver.config.getValue("backgroundColour") });
                this.InvokeScript("toggle", new string[] { "#quickMetadata", Convert.ToString(this.screensaver.config.getCheckboxValue("showQuickMetadataM" + (this.id + 1))) });
                this.InvokeScript("toggle", new string[] { "#filename", Convert.ToString(this.screensaver.config.getCheckboxValue("showFilenameM" + (this.id + 1))) });
                
                string clockType = Convert.ToString(this.screensaver.config.getRadioValue("clockM" + (this.id + 1)));
                if (clockType == null) clockType = "none";
                this.InvokeScript("toggle", new string[] { "#clock", Convert.ToString(!clockType.Equals("none")) });
                this.InvokeScript("setClockType", new string[] { clockType });
                
                this.showImage(true);
            }
        }

        public void debug(string s) {
//            if (InvokeRequired) {
                //Invoke(new GetStringHandler(this.browser.Document.InvokeScript("test", new String[] { s }));
                this.browser.Invoke(new Action(() => {
                    //this.browser.Document.InvokeScript("test", new String[] { s });
                    this.browser.Document.Write(s + "<br/>");
                }));
  /*          } else {
                this.browser.Document.InvokeScript("test", new String[] { s });
            }*/

        }

        private void Monitor_Load(object sender, EventArgs e) {
            this.browser.AllowWebBrowserDrop = false;
            this.browser.ObjectForScripting = this;
//            this.timer.Interval = 1;
            this.nextImage();
            if (this.currentImage != null) this.setTimerInterval();
/*           
            if (this.id == 0) {
                this.setTimerInterval();
            } else {
                if (this.screensaver.config.syncMonitors()) {
                    this.timer.Interval = this.screensaver.monitors[0].timer.Interval;
                }
            }*/
            this.startTimer();
        }

        public void startTimer() {
            if (this.id == 0 || !this.screensaver.config.syncMonitors()) this.timer.Start();
        }

        public void showInfoOnMonitor(string info) {
            this.showInfoOnMonitor(info, false);
        }

        public void showInfoOnMonitor(string info, bool highPriority) {
            if (highPriority) {
                this.browser.Document.InvokeScript("showPriorityInfo", new String[] { info });
            } else {
                this.browser.Document.InvokeScript("showInfo", new String[] { info });
            }
        }

        public long imageId() {
            if (this.currentImage != null) {
                return Convert.ToInt32(this.currentImage["id"]);
            }
            return -1;
        }

        public string imagePath() {
            if (this.currentImage != null) {
                return Convert.ToString(this.currentImage["path"]);
            }
            return null;
        }

        public void showMetadataOnMonitor(string metadata) {
            this.quickMetadata = new MetadataTemplate(metadata, this.screensaver.config.getValue("quickMetadata"));
            string md = this.quickMetadata.fillTemplate();
            if (md != null || md != "") {
                //this.browser.Document.InvokeScript("showImage", new String[] { Convert.ToString(this.currentImage["path"]), "false", md });

//                this.browser.Document.InvokeScript("showMetadata", new String[] { "<<" });
                //this.browser.Document.InvokeScript("showInfo", new String[] { md });
                if (this.browser.InvokeRequired) {
                    
                    this.browser.Document.InvokeScript("showMetadata", new String[] { Convert.ToString(md) });
                } else {
                    this.browser.Document.InvokeScript("showMetadata", new String[] { Convert.ToString(md) });
                }
                
            }
        }

        public Object InvokeScript(string script, string[] parameters) {
            return this.browser.Document.InvokeScript(script, parameters);
        }

        public void showImage(bool animated) {
            if (this.currentImage != null) {
                string metadata = "";
                string rawMetadata = null;
                Hashtable settings = new Hashtable();

                if (this.currentImage.Table.Columns.Contains("all")) {
                    rawMetadata = Convert.ToString(this.currentImage["all"]);
                } else {
                    rawMetadata = this.screensaver.fileNodes.getMetadataById(Convert.ToInt32(this.currentImage["id"]));
                    if (rawMetadata == null) {
                        try {
                            rawMetadata = this.screensaver.fileNodes.exifToolCommand(Convert.ToString(this.currentImage["path"]), Convert.ToInt32(this.currentImage["id"]));
                        } catch (System.IO.FileNotFoundException fnfe) {
                            metadata = "ExifTool not found: '" + this.screensaver.config.getValue("exifTool") + "'";
                        }
                    }
                }
                if (rawMetadata != null && rawMetadata != "") {
                    this.quickMetadata = new MetadataTemplate(rawMetadata, this.screensaver.config.getValue("quickMetadata"));
                    metadata = this.quickMetadata.fillTemplate();
                }
                settings["mediatype"] = "image";
                if (this.screensaver.config.getValue("videoExtensions").IndexOf(Path.GetExtension(Convert.ToString(this.currentImage["path"]))) > -1) {
                    settings["mediatype"] = "video";
                }
                switch(Path.GetExtension(Convert.ToString(this.currentImage["path"]).ToLower())) {
                    case ".wmv": case ".avi":
                        settings["mediatype"] = "object";
                    break;
                }
                settings["animated"] = Convert.ToString(animated).ToLower();
                settings["stretchSmallImages"] = this.screensaver.config.getCheckboxValue("stretchSmallImages");
                settings["stretchSmallVideos"] = this.screensaver.config.getCheckboxValue("stretchSmallVideos");
                settings["showcontrols"] = this.screensaver.config.getCheckboxValue("showControls");
                settings["loop"] = this.screensaver.config.getCheckboxValue("videosLoop");
                settings["mute"] = this.screensaver.config.getCheckboxValue("videosMute");

                this.browser.Document.InvokeScript("showImage", new Object[] { Convert.ToString(this.currentImage["path"]), metadata, JsonConvert.SerializeObject(settings) });
            }
        }

        /**
         * Check whether file exists.
         * If it doesn't exists delete from DB.
         * Return true if file exists other wise false
         **/
        public bool fileExistsOnDisk(DataRow dr) {
            if (dr == null) return true;
            if (!File.Exists(Convert.ToString(dr["path"]))) {
                Console.Beep();
                this.screensaver.fileNodes.deleteFromDB(Convert.ToInt32(dr["id"]));
                return false;
            }
            return true;
        }

        public DataRow previousImage() {
            if (this.screensaver.config.getOrder() == Config.Order.Random) {
                this.historyPointer--;
                if (this.historyPointer < 0 || this.history.Count < this.historyPointer) {
                    this.historyPointer = 0;
                    this.showInfoOnMonitor("You've reached the first image");
                }
                long imageId = -1;
                if (this.history.Count > 0) 
                    imageId = this.history[this.historyPointer];
                this.currentImage = this.screensaver.fileNodes.getImageById(imageId, this.historyOffset);
            } else {
                int offset = 1;
                SortOrder direction = new SortOrder(SortOrder.Direction.ASC);
                // For first monitor skip back to photo on first monitor, than select going forward.
                if (this.id == 0) {
                    direction.setDESC();
                    offset = this.screensaver.monitors.Length+1;
                }
                this.currentImage = this.screensaver.fileNodes.getSequentialImage(this.id, direction, offset);
            }
            if (!fileExistsOnDisk(this.currentImage)) return this.previousImage();
            return this.currentImage;
        }

        public DataRow nextImage() {
            if (this.screensaver.fileNodes == null) return null;  
            if (this.screensaver.config.getOrder() == Config.Order.Random) {
                if (this.historyPointer + 1 < this.history.Count()) {
                    this.historyPointer++;
                    this.currentImage = this.screensaver.fileNodes.getImageById(this.history[this.historyPointer], this.historyOffset);
                } else {
                    this.currentImage = this.screensaver.fileNodes.getRandomImage();
                    if (this.currentImage != null) {
                        //            string filename = Convert.ToString(img["path"]);
                        this.history.Add(Convert.ToInt32(this.currentImage["id"]));
                        this.historyPointer++;
                    }
                }
            } else {
                int offset = 1;
                //int offset = this.offset;
                if (this.id == 0) {
//                    offset = this.screensaver.monitors.Length;
                }
                this.currentImage = this.screensaver.fileNodes.getSequentialImage(this.id, new SortOrder(SortOrder.Direction.ASC), offset);
            }
            if (!fileExistsOnDisk(this.currentImage)) return this.nextImage();
            return this.currentImage;
        }

        public DataRow offsetImage(int offset) {
            return null;
        }

        public void setTimerInterval() {
            if (this.id == 0 || !this.screensaver.config.syncMonitors()) {
                int timeout;
                try {
                    timeout = Convert.ToInt32(this.screensaver.config.getValue("timeout")) * 1000;
                } catch (Exception e) {
                    timeout = 10;
                }
                if (timeout < 1) timeout = 1000;
                if (this.screensaver.config.getCheckboxValue("variableTimeout")) {
                    int min;
                    int max;
                    Random rnd = new Random();
                    try {
                        min = Math.Min(timeout, Convert.ToInt32(this.screensaver.config.getValue("timeoutMax")) * 1000);
                    } catch (Exception e) {
                        min = 10;
                    }
                    try {
                        max = Math.Max(timeout, Convert.ToInt32(this.screensaver.config.getValue("timeoutMax")) * 1000);
                    } catch (Exception e) {
                        max = 20;
                    }
                    if (min < 1000) min = 1000;
                    if (max < min) max = min;
                    timeout = rnd.Next(min, max);
                }
                //if (this.id == 0) timeout = timeout / 3;
                this.timer.Interval = timeout;
            }
            if (this.screensaver.config.syncMonitors() && this.id == 0) {
                for (int i = 0; i < this.screensaver.monitors.Length; i++) {
                    if (this.screensaver.monitors[i] != null) {
                        this.screensaver.monitors[i].timer.Interval = this.screensaver.monitors[0].timer.Interval;
                    }
                }
            }
        }

        public void timer_Tick(object sender, EventArgs e) {
            this.nextImage();
            
            if (this.currentImage == null || Convert.ToString(this.currentImage["path"]) == "") {
                this.timer.Interval *= 2;
                if (this.timer.Interval > Convert.ToInt32(this.screensaver.config.getValue("timeout")) * 1000) {
                    this.setTimerInterval();
                }
            } else {
                this.showImage(true);
                this.setTimerInterval();
                if (this.id == 0 && this.screensaver.config.syncMonitors()) {
                    for (int i = 1; i < this.screensaver.monitors.Length; i++) {
                        this.screensaver.monitors[i].timer.Stop();
                        this.screensaver.monitors[i].nextImage();
                        this.screensaver.monitors[i].showImage(true);
                    }
                }
            }
        }
    }
}
