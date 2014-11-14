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
        static Random rnd = new Random();
        private int id;
        private Screensaver screensaver;

        public string info = "";
        private string priorityInfo = "";

        // Random photo history
        public List<long> history;
        public int historyPointer = -1;
        //public int historyOffset = 0;

        // Random image id of which offset is calculated
        public long seedImageId;

        public int offset = 0;

        public bool paused = false;

        public bool panoramaPart = false;

        public DataRow currentImage;
        //string metadata;
        MetadataTemplate quickMetadata;
        Hashtable imageSettings = new Hashtable();

        #region Win32 API functions

        [DllImport("user32.dll")]
        public static extern IntPtr SetParent(IntPtr hWndChild, IntPtr hWndNewParent);

        [DllImport("user32.dll")]
        public static extern int SetWindowLong(IntPtr hWnd, int nIndex, IntPtr dwNewLong);

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
            this.browser.Url = new Uri(Constants.getDataFolder(Constants.MonitorHtmlFile), System.UriKind.Absolute);
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
            SetParent(this.Handle, previewHwnd);
            SetWindowLong(this.Handle, -16, new IntPtr(GetWindowLong(this.Handle, 0) | 0x40000000));
            // TODO adjust size of window

            // Place our window inside the parent
            Rectangle ParentRect;
            GetClientRect(previewHwnd, out ParentRect);
            Size = ParentRect.Size;
            //Location = new Point(40, 40);
              //          this.Bounds = ParentRect;
            //this.browser.Scale(new SizeF((float)250, (float)250));
        }

        public void defaultShowHide() {
            this.InvokeScript("setBackgroundColour", new string[] { Convert.ToString(this.screensaver.config.getPersistant("backgroundColour")) });
            this.InvokeScript("toggle", new string[] { "#quickMetadata", Convert.ToString(this.screensaver.config.getPersistantBool("showQuickMetadataM" + (this.id + 1))) });
            this.InvokeScript("toggle", new string[] { "#filename", Convert.ToString(this.screensaver.config.getPersistantBool("showFilenameM" + (this.id + 1))) });

            string clockType = this.screensaver.config.getPersistantString("clockM" + (this.id + 1));
            if (clockType == null) clockType = "none";
            this.InvokeScript("toggle", new string[] { "#clock", Convert.ToString(!clockType.Equals("none")) });
            this.InvokeScript("setClockType", new string[] { clockType });
        }

        private void DocumentCompleted(object sender, System.Windows.Forms.WebBrowserDocumentCompletedEventArgs e) {
            // Correct document loaded (not about:blank)
            //MessageBox.Show("Monitor Document Completed: " + Constants.getDataFolder(Constants.ConfigHtmlFile));

            if (this.browser.Url.Segments.Last().Equals(Constants.MonitorHtmlFile)) {

                string classes = "monitor" + (this.id + 1);
                if (this.screensaver.monitors.Length > 1) classes += " multimonitor";
                Config.setBrowserBodyClasses(this.browser, this.screensaver.action, classes);
                
                this.browser.Document.GetElementById("identify").InnerText = Convert.ToString(this.id+1);
                this.browser.Show();
                // Focus browser to ensure key strokes are previewed
//                this.browser.PreviewKeyDown += this.screensaver.PreviewKeyDown;
                this.browser.Focus();
                
                // Remove form's PreviewKeyDown to avoid duplicated keys
                this.PreviewKeyDown -= this.screensaver.PreviewKeyDown;

                // Set defaults for monitor.html
                if (this.screensaver.config.persistantLoaded()) {
                    this.defaultShowHide();
                }
                
                this.showImage(true);
            }
        }

        private void Monitor_Load(object sender, EventArgs e) {
            this.browser.AllowWebBrowserDrop = false;
            this.browser.ObjectForScripting = this;

            this.timer.Interval = 1;
            this.startTimer();
        }

        public void startTimer() {
            if (this.id == 0 || !this.screensaver.config.syncMonitors()) this.timer.Start();
            this.paused = false;
        }

        public string showInfoOnMonitor(string info) {
            return this.showInfoOnMonitor(info, false, true);
        }

        public string showInfoOnMonitor(string info, bool highPriority) {
            return this.showInfoOnMonitor(info, highPriority, true);
        }

        public string showInfoOnMonitor(string info, bool highPriority, bool fade) {
            string previousInfo;
            if (highPriority) {
                previousInfo = this.priorityInfo;
                this.priorityInfo = info;
                try {
                    this.browser.Document.InvokeScript("showPriorityInfo", new String[] { info, Convert.ToString(fade) });
                } catch (Exception e) {
                    // TODO: log to debug file?
                    Debug.WriteLine(info);
                }
            } else {
                previousInfo = this.info;
                this.info = info;
                this.browser.Document.InvokeScript("showInfo", new String[] { info, Convert.ToString(fade) });
            }
            return previousInfo;
        }

        public void showUpdateInfo(string info) {
            this.browser.Document.InvokeScript("showUpdateInfo", new String[] { info });
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
            this.quickMetadata = new MetadataTemplate(metadata, Convert.ToString(this.screensaver.config.getPersistant("quickMetadata")));
            string md = this.quickMetadata.fillTemplate();
            if (md != null || md != "") {
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

        public void rotateImage(int deg) {
            //deg = ;
            this.imageSettings["exifRotate"] = (deg + Convert.ToInt32(this.imageSettings["exifRotate"])) % 360;
            this.resizeRatioRotate90(Convert.ToInt32(this.imageSettings["exifRotate"]));
            this.browser.Document.InvokeScript("setImageRotation", new Object[] { Convert.ToString(this.imageSettings["exifRotate"]), this.imageSettings["resizeRatio"] });

            if (this.screensaver.config.getPersistantBool("saveExifOnRotation")) {
                bool backedUp = true; // default to true to enable save without backup option
                if (this.screensaver.config.getPersistantBool("backupExifOnRotation")) {
                    if (!File.Exists(Convert.ToString(this.currentImage["path"]) + ".bak")) {
                        try {
                            // Copy if file doesn't exists
                            File.Copy(Convert.ToString(this.currentImage["path"]), Convert.ToString(this.currentImage["path"]) + ".bak");
                        } catch (Exception e) {
                            this.showInfoOnMonitor(e.Message, true);
                            backedUp = false;
                        }
                    }

                }
                if (backedUp) {
                    int[] rotate90 = {1, 8, 3, 6, 1, 0, 2, 7, 4, 5, 2};
			        int[] rotate180 = {1, 3, 1, 2, 4, 2, 5, 7, 5, 6, 8, 6};
                    int orientation = 1;
                    // Try reading orientation from file to ensure it's up to date
                    try {
                        orientation = Convert.ToByte(this.screensaver.fileNodes.exifToolCommand(Convert.ToString(this.currentImage["path"]) + "\n-Orientation#").Replace("Orientation\t", ""));
                    } catch (Exception e) {
                        orientation = 1;
                        if (this.quickMetadata.metadata.ContainsKey("orientation#")) {
                            orientation = Convert.ToByte(this.quickMetadata.metadata["orientation#"]);
                        }
                    }

                    int i = 0;
                    int newOrientation = orientation;
                    if (deg == 180) {
                        while (rotate180[i] != orientation) i++;
                        newOrientation = rotate180[i + 1];
                    } else {
                        while (rotate90[i] != orientation) i++;
                        if (deg == 90) i--; else i++;
                        if (i < 0) i = 3;
                        newOrientation = rotate90[i];
                    }
                    try { 
                        // Save orientation to file
                        this.screensaver.fileNodes.exifToolCommand(Convert.ToString(this.currentImage["path"]) + "\n-P\n-overwrite_original\n-n\n-Orientation=" + newOrientation);
                        if (this.imageSettings.ContainsKey("rawCached")) {
                            File.Delete(Convert.ToString(this.imageSettings["rawCached"]));
                        } else {
                            // Update metadata in DB
                            this.screensaver.fileNodes.exifToolGetMetadata(Convert.ToString(this.currentImage["path"]) + Constants.ExifToolMetadataOptions, Convert.ToInt32(this.currentImage["id"]));
                        }
                    } catch (Exception e) {
                        this.showInfoOnMonitor(e.Message, true);
                        backedUp = false;
                    }
                    this.showInfoOnMonitor(this.info + "\r\nFile saved", true);
                }
            }
        }

        public float resizeRatioRotate90(int rotate) {
            int width = Convert.ToInt32(this.quickMetadata.metadata["image width"]);
            int height = Convert.ToInt32(this.quickMetadata.metadata["image height"]);

            this.imageSettings["resizeRatio"] = 1;
            if (rotate == 90 || rotate == 270) {
                // Internet Explorer fits image into boundaries before rotating
                // Swap width, height values
                // Determine size ratio between un-rotated and rotated image
                if (width > this.Bounds.Width || height > this.Bounds.Height) {
                    Rectangle newR = Constants.FitIntoBounds(new Rectangle(this.Bounds.Location, new Size(width, height)), this.Bounds, false);
                    Rectangle oldR = Constants.FitIntoBounds(new Rectangle(this.Bounds.Location, new Size(height, width)), this.Bounds, false);
                    this.imageSettings["resizeRatio"] = (float)oldR.Height / newR.Width;
                }
                this.quickMetadata.metadata["image width"] = Convert.ToString(width);
                this.quickMetadata.metadata["image height"] = Convert.ToString(height);
            }
            return Convert.ToInt32(this.imageSettings["resizeRatio"]);
        }

        public void readMetadataImage() {
            if (this.currentImage != null) {
                //string metadata = "";
                string rawMetadata = null;

                this.imageSettings.Clear();
                this.imageSettings["metadata"] = "";

                if (this.currentImage.Table.Columns.Contains("all")) {
                    rawMetadata = Convert.ToString(this.currentImage["all"]);
                } else {
                    rawMetadata = this.screensaver.fileNodes.getMetadataById(Convert.ToInt32(this.currentImage["id"]));
                    if (rawMetadata == null) {
                        try {
                            rawMetadata = this.screensaver.fileNodes.exifToolGetMetadata(Convert.ToString(this.currentImage["path"]) + Constants.ExifToolMetadataOptions, Convert.ToInt32(this.currentImage["id"]));
                        } catch (System.IO.FileNotFoundException fnfe) {
                            this.imageSettings["metadata"] = "ExifTool not found: '" + this.screensaver.config.getPersistant("exifTool") + "'";
                        }
                    }
                }
                if (rawMetadata != null && rawMetadata != "") {
                    this.quickMetadata = new MetadataTemplate(rawMetadata, Convert.ToString(this.screensaver.config.getPersistant("quickMetadata")));
                    this.imageSettings["metadata"] = this.quickMetadata.fillTemplate();
                }
                this.imageSettings["mediatype"] = "image";
                if (Convert.ToString(this.screensaver.config.getPersistant("videoExtensions")).IndexOf(Path.GetExtension(Convert.ToString(this.currentImage["path"]).ToLower())) > -1) {
                    this.imageSettings["mediatype"] = "video";
                }
                switch(Path.GetExtension(Convert.ToString(this.currentImage["path"]).ToLower())) {
                    case ".wmv": case ".avi":
                        this.imageSettings["mediatype"] = "object";
                    break;
                }
                switch (Convert.ToString(this.imageSettings["mediatype"])) {
                    case "image":
                        this.imageSettings["stretchSmallImages"] = this.screensaver.config.getPersistantBool("stretchSmallImages");
                        if (this.screensaver.config.getPersistantBool("exifRotate")) {
                            if (this.quickMetadata != null && this.quickMetadata.metadata.ContainsKey("orientation#")) {
                                int rotate = 0;
                                switch (Convert.ToByte(this.quickMetadata.metadata["orientation#"])) {
                                    case (int)Constants.Orientation.Rotate_180: rotate = 180; break;
                                    case (int)Constants.Orientation.Rotate_90_CW: 
                                    case (int)Constants.Orientation.Mirror_Horizontal_Rotate_90_CW: 
                                        rotate = 90; 
                                    break;
                                    case (int)Constants.Orientation.Rotate_270_CW:
                                    case (int)Constants.Orientation.Mirror_Horizontal_Rotate_270_CW: 
                                        rotate = 270; 
                                    break;
                                }
                                this.resizeRatioRotate90(rotate);
                                if (rotate != 0) {
                                    this.imageSettings["exifRotate"] = rotate;
                                }
                            }
                        }
                        int width = Convert.ToInt32(this.quickMetadata.metadata["image width"]);
                        int height = Convert.ToInt32(this.quickMetadata.metadata["image height"]);
                        float imgRatio = (float)width / (float)height;
                        if (this.screensaver.config.getPersistantBool("stretchPanoramas") && imgRatio >= this.screensaver.desktopRatio) {
                            Rectangle pano = Constants.FitIntoBounds(new Rectangle(0, 0, width, height), this.screensaver.Desktop, false);
                            this.imageSettings["pano"] = true;
                            if (this.id != 0 && this.Bounds.Height != this.screensaver.monitors[0].Bounds.Height) {
                                this.imageSettings["pano.top"] = (this.Bounds.Height - pano.Height) / 2;//pano.Y - this.Bounds.Y;
                            } else {
                                this.imageSettings["pano.top"] = pano.Y - this.Bounds.Y;
                            }
                            this.imageSettings["pano.left"] = pano.X - this.Bounds.X;
                            this.imageSettings["pano.width"] = pano.Width;
                            this.imageSettings["pano.height"] = pano.Height;

                            if (this.id == 0) {
                                for (int i = 1; i < this.screensaver.monitors.Length; i++) {
                                    if (this.screensaver.monitors[i] != null) this.screensaver.monitors[i].trySetNextImage(Convert.ToInt32(this.currentImage["id"]));
                                }
                            }
                        } else {
                            for (int i = 0; i < this.screensaver.monitors.Length; i++) {
                                this.screensaver.monitors[i].panoramaPart = false;
                            }
                        }
                    break;
                    case "object": case "video":
                    this.imageSettings["stretchSmallVideos"] = this.screensaver.config.getPersistantBool("stretchSmallVideos");
                    this.imageSettings["showcontrols"] = this.screensaver.config.getPersistantBool("showControls");
                    this.imageSettings["loop"] = this.screensaver.config.getPersistantBool("videosLoop");
                    this.imageSettings["mute"] = this.screensaver.config.getPersistantBool("videosMute");
                    break;
                }
            }
        }

        public void showImage(bool animated) {
            if (this.currentImage != null) {
                this.imageSettings["animated"] = Convert.ToString(animated).ToLower();
                if (animated) {
                    this.imageSettings["effect"] = this.screensaver.config.getRandomEffect();
                }
                try {
                    var bgw = new BackgroundWorker();
                    bgw.DoWork += (object sender, DoWorkEventArgs e) => {
                        e.Result = this.screensaver.fileNodes.checkImageCache(Convert.ToString(this.currentImage["path"]), this.id, ref this.imageSettings);
                    };
                    bgw.RunWorkerCompleted += (object sender, RunWorkerCompletedEventArgs e) => {
                        if (this.imageSettings.ContainsKey("raw") && Convert.ToBoolean(this.imageSettings["raw"])) {
                            this.imageSettings["exifRotate"] = 0;
                            this.imageSettings["resizeRatio"] = 1;
                            this.imageSettings["rawCached"] = e.Result;
                        }
                        //metadata += " " + this.imageSettings["exifRotate"] + " " + this.Bounds.ToString();
                        //this.showInfoOnMonitor(this.info, false, true);
                        this.browser.Document.InvokeScript("showImage", new Object[] { e.Result, Convert.ToString(this.currentImage["path"]), JsonConvert.SerializeObject(this.imageSettings) });
                    };
                    string path = "";
                    object[] prms = new object[] { path };
                    if (this.screensaver.config.getPersistantBool("rawUseConverter") && (Convert.ToString(this.screensaver.config.getPersistant("rawExtensions")).IndexOf(Path.GetExtension(Convert.ToString(this.currentImage["path"])).ToLower()) > -1)) {
                        this.info = this.showInfoOnMonitor("Converting from RAW to JPEG <span class='wait'></span>", false, false);
                    }
                    bgw.RunWorkerAsync(prms);
                } catch (Exception e) {
                    this.showInfoOnMonitor(e.Message, true);
                }
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
                this.screensaver.fileNodes.deleteFromDB(Convert.ToInt32(dr["id"]));
                return false;
            }
            return true;
        }

        public DataRow offsetImage(int i) {
            if (this.screensaver.config.getOrder() == Config.Order.Random) {
                this.offset += i;
                //this.seedImage = this.currentImage;
                this.currentImage = this.screensaver.fileNodes.getImageById(Convert.ToInt32(this.seedImageId), this.offset);
                this.readMetadataImage();
                return this.currentImage;
            } else {
                // this.readMetadataImage() called in methods below;
                if (i > 0) return this.nextImage();
                else return this.previousImage();
            }
        }


        public void historyAdd(long id) {
            this.history.Add(Convert.ToInt32(this.currentImage["id"]));
            long max = Convert.ToInt32(this.screensaver.config.getPersistant("maxHistory"));
            if (max > 0) while (this.history.Count() > max) {
                    // Decrease historyPointer first to be on the safe side
                    this.historyPointer--;
                    this.history.RemoveAt(0);
                }
        }

        public void setHistory(List<long> history, int offset) {
            this.history = history;
            this.historyPointer = history.Count;
            this.offset = offset;
        }

        public List<long> historyLastEntries(int count) {
            int start;
            start = this.historyPointer - count +1;
            if (start < 0) start = 0;
            if (this.history.Count < start + count) count = this.history.Count - start;
            return this.history.GetRange(start, count);
        }

        public DataRow previousImage() {
            return this.previousImage(1);
        }

        /**
         * Note step parameter for prevoiusImage is always positive unlike offset which indicates direction.
         **/
        public DataRow previousImage(int step) {
            if (this.screensaver.config.getOrder() == Config.Order.Random) {
                this.historyPointer -= step;
                if (this.historyPointer < 0 || this.history.Count < this.historyPointer) {
                    this.historyPointer = 0;
                    this.showInfoOnMonitor("You've reached the first image");
                }
                long imageId = -1;
                if (this.history.Count > 0) 
                    imageId = this.history[this.historyPointer];
                this.currentImage = this.screensaver.fileNodes.getImageById(imageId, this.offset);
                this.seedImageId = imageId;
            } else {
                int offset = 1;
                // For first monitor skip back to photo on first monitor, than select going forward.
                if (this.screensaver.currentMonitor == Screensaver.CM_ALL && this.id == 0) {
                    offset = this.screensaver.monitors.Length*(step+1)*-1+1;
                }
                if (this.screensaver.currentMonitor == this.id) {
                    offset = -1;
                }

                this.currentImage = this.screensaver.fileNodes.getSequentialImage(this.id, offset);
            }
            if (!fileExistsOnDisk(this.currentImage)) return this.previousImage();
            this.readMetadataImage();
            return this.currentImage;
        }

        public DataRow nextImage() {
            return this.nextImage(1);
        }

        public DataRow nextImage(int step) {
            if (this.screensaver.fileNodes == null) return null;  
            if (this.screensaver.config.getOrder() == Config.Order.Random) {
                this.historyPointer += step;
                if (this.historyPointer < this.history.Count()) {
                    this.currentImage = this.screensaver.fileNodes.getImageById(this.history[this.historyPointer], this.offset);
                    this.seedImageId = Convert.ToInt32(this.currentImage["id"]);
                } else {
                    this.historyPointer = this.history.Count();
                    this.currentImage = this.screensaver.fileNodes.getRandomImage();
                    if (this.currentImage != null) {
                        this.historyAdd(Convert.ToInt32(this.currentImage["id"]));
                        //this.historyPointer += step;

                        this.seedImageId = Convert.ToInt32(this.currentImage["id"]);
                        if (this.offset != 0) {
                            this.currentImage = this.screensaver.fileNodes.getImageById(this.seedImageId, this.offset);
                        }
                    }
                }
            } else {
                //int offset = 1;
                if (this.panoramaPart) {
                    this.currentImage = this.screensaver.monitors[0].currentImage;
                } else {
                    this.currentImage = this.screensaver.fileNodes.getSequentialImage(this.id, step);
                }
            }
            if (!fileExistsOnDisk(this.currentImage)) return this.nextImage();
            this.readMetadataImage();
            return this.currentImage;
        }

        public bool trySetNextImage(long seedImageId) {
            if (this.screensaver.config.getOrder() == Config.Order.Random) {
                if (this.historyPointer == this.history.Count()-1) {
                    this.historyAdd(seedImageId);
                    return true;
                } else return false;
            } else {
                this.panoramaPart = true;
            }

            return false;
        }

        public void setTimerInterval() {
            if (this.id == 0 || !this.screensaver.config.syncMonitors()) {
                int timeout;
                try {
                    timeout = Convert.ToInt32(this.screensaver.config.getPersistant("timeout")) * 1000;
                } catch (Exception e) {
                    timeout = 10;
                }
                if (timeout < 1) timeout = 1000;
                if (this.screensaver.config.getPersistantBool("variableTimeout")) {
                    int min;
                    int max;
                    rnd.Next();
                    try {
                        min = Math.Min(timeout, Convert.ToInt32(this.screensaver.config.getPersistant("timeoutMax")) * 1000);
                    } catch (Exception e) {
                        min = 10;
                    }
                    try {
                        max = Math.Max(timeout, Convert.ToInt32(this.screensaver.config.getPersistant("timeoutMax")) * 1000);
                    } catch (Exception e) {
                        max = 20;
                    }
                    if (min < 1000) min = 1000;
                    if (max < min) max = min;
                    timeout = rnd.Next(min, max);
                }
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
                if (this.screensaver.config.persistantLoaded() && this.timer.Interval > Convert.ToInt32(this.screensaver.config.getPersistant("timeout")) * 1000) {
                    this.setTimerInterval();
                }
            } else {
                this.showImage(true);
                this.setTimerInterval();
                if (this.id == 0 && this.screensaver.config.syncMonitors()) {
                    for (int i = 1; i < this.screensaver.monitors.Length; i++) {
                        if (this.screensaver.currentMonitor == Screensaver.CM_ALL || this.screensaver.currentMonitor == i) {
                            this.screensaver.monitors[i].timer.Stop();
                            this.screensaver.monitors[i].nextImage();
                            this.screensaver.monitors[i].showImage(true);
                        }
                    }
                }
            }
        }
    }
}
