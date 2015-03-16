using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Text.RegularExpressions;
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
using System.Security.Principal;
using Microsoft.VisualBasic;

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
        private int resetFilterCount = 0;
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
            this.Text = "Monitor " + Convert.ToString(this.id+1) + " " + Constants.AppName;
            this.history = new List<long>();
            this.browser.DocumentCompleted += new System.Windows.Forms.WebBrowserDocumentCompletedEventHandler(this.DocumentCompleted);
        }

        public Monitor(Rectangle Bounds, int id, Screensaver screensaver): this(id, screensaver) {
            this.Bounds = Bounds;
        }

        public Monitor(IntPtr previewHwnd, int id, Screensaver screensaver): this(id, screensaver) {
            SetParent(this.Handle, previewHwnd);
            SetWindowLong(this.Handle, -16, new IntPtr(GetWindowLong(this.Handle, 0) | 0x40000000));

            // Place our window inside the parent
            Rectangle ParentRect;
            GetClientRect(previewHwnd, out ParentRect);
            Size = ParentRect.Size;
        }

        public void defaultShowHide() {
            this.InvokeScript("setBackgroundColour", new string[] { Convert.ToString(this.screensaver.config.getPersistant("backgroundColour")) });
            this.InvokeScript("toggle", new string[] { "#quickMetadata", Convert.ToString(this.screensaver.config.getPersistantBool("showQuickMetadataM" + (this.id + 1))) });
            this.InvokeScript("toggle", new string[] { "#filename", Convert.ToString(this.screensaver.config.getPersistantBool("showFilenameM" + (this.id + 1))) });
            this.InvokeScript("toggle", new string[] { "#filename .root", Convert.ToString(this.screensaver.config.getPersistantBool("showPathRoot")) });
            this.InvokeScript("toggle", new string[] { "#filename .subfolders", Convert.ToString(this.screensaver.config.getPersistantBool("showPathSubfolders")) });
            this.InvokeScript("toggle", new string[] { "#filename .filename", Convert.ToString(this.screensaver.config.getPersistantBool("showPathFilename")) });
            this.InvokeScript("toggle", new string[] { "#filename .extension", Convert.ToString(this.screensaver.config.getPersistantBool("showPathExtension")) });
            this.InvokeScript("toggle", new string[] { "#indexprogress", Convert.ToString(this.screensaver.config.getPersistantBool("showIndexProgressM" + (this.id + 1))) });

            string clockType = this.screensaver.config.getPersistantString("clockM" + (this.id + 1));
            if (clockType == null) clockType = "none";
            this.InvokeScript("toggle", new string[] { "#clock", Convert.ToString(!clockType.Equals("none")) });
            this.InvokeScript("setClockType", new string[] { clockType });
            this.InvokeScript("setClockFormat", new string[] { this.screensaver.config.getPersistantString("clockFormatM" + (this.id + 1)) });
        }

        private void DocumentCompleted(object sender, System.Windows.Forms.WebBrowserDocumentCompletedEventArgs e) {
            // Correct document loaded (not about:blank)
            //MessageBox.Show("Monitor Document Completed: " + Constants.getDataFolder(Constants.ConfigHtmlFile));

            if (this.browser.Url.Segments.Last().Equals(Constants.MonitorHtmlFile)) {

                string classes = "monitor" + (this.id + 1);
                if (this.screensaver.monitors.Length > 1) classes += " multimonitor";
                if (this.screensaver.readOnly) classes += " readonly";
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
                    Debug.WriteLine("showInfoOnMonitor " + e.Message);
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

        public void downloadProgressIndicator(int i) {
            this.browser.Document.InvokeScript("downloadProgressIndicator", new String[] { Convert.ToString(i) });
        }

        public void hideUpdateInfo() {
            this.browser.Document.InvokeScript("hideUpdateInfo", new String[] { });
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
/*
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
        }*/

        public Object InvokeScript(string script, string[] parameters) {
            return this.browser.Document.InvokeScript(script, parameters);
        }

        public bool backupImageCheck(bool backItUp) {
            bool backedUp = true; // default to true to enable save without backup option
            if (backItUp) {
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
            return backedUp;
        }

        public void rotateImage(int deg) {
            //deg = ;
            this.imageSettings["exifRotate"] = (deg + Convert.ToInt32(this.imageSettings["exifRotate"])) % 360;
            this.resizeRatio(Convert.ToInt32(this.imageSettings["exifRotate"]));
            this.browser.Document.InvokeScript("setImageRotation", new Object[] { Convert.ToString(this.imageSettings["exifRotate"]), this.imageSettings["resizeRatio"] });

            if (this.screensaver.config.getPersistantBool("saveExifOnRotation")) {
                bool backedUp = backupImageCheck(this.screensaver.config.getPersistantBool("backupExifOnRotation"));

                if (backedUp) {
                    int[] rotate90 = { 1, 8, 3, 6, 1, 0, 2, 7, 4, 5, 2 };
                    int[] rotate180 = { 1, 3, 1, 2, 4, 2, 5, 7, 5, 6, 8, 6 };
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
                            string meta = this.screensaver.fileNodes.exifToolGetMetadata(Convert.ToString(this.currentImage["path"]) + Constants.ExifToolMetadataOptions, Convert.ToInt32(this.currentImage["id"]));
                            this.screensaver.fileNodes.addMetadataToDB(Convert.ToInt32(this.currentImage["id"]), meta);
                        }
                    } catch (Exception e) {
                        this.showInfoOnMonitor(e.Message, true);
                        backedUp = false;
                    }
                    this.showInfoOnMonitor(this.info + "<br/>File saved", true);
                }
            }
        }

        public void rateImage(int rating) {
            //deg = ;
            if (this.screensaver.config.getPersistantBool("saveExifRating")) {
                bool backedUp = backupImageCheck(this.screensaver.config.getPersistantBool("backupOnExifRating"));

                if (backedUp) {
                    try {
                        int ratingPercent = 0;
                        switch (rating) {
                            case 1: ratingPercent = 1; break;
                            case 2: ratingPercent = 25; break;
                            case 3: ratingPercent = 50; break;
                            case 4: ratingPercent = 75; break;
                            case 5: ratingPercent = 99; break;
                        }
                        // Save orientation to file
                        this.screensaver.fileNodes.exifToolCommand(Convert.ToString(this.currentImage["path"]) + "\n-P\n-overwrite_original\n-n\n-RatingPercent=" + ratingPercent + "\n-Rating=" + rating);
                        if (this.imageSettings.ContainsKey("rawCached")) {
                            File.Delete(Convert.ToString(this.imageSettings["rawCached"]));
                        } else {
                            // Update metadata in DB
                            string meta = this.screensaver.fileNodes.exifToolGetMetadata(Convert.ToString(this.currentImage["path"]) + Constants.ExifToolMetadataOptions, Convert.ToInt32(this.currentImage["id"]));
                            this.screensaver.fileNodes.addMetadataToDB(Convert.ToInt32(this.currentImage["id"]), meta);
                        }
                        this.showInfoOnMonitor("Star rating set to<br/><div class='rating r" + rating + "'>" + rating + " stars</div>", true);
                    } catch (Exception e) {
                        this.showInfoOnMonitor(e.Message, true);
                        backedUp = false;
                    }
                } else {
                    this.showInfoOnMonitor("Backup failed<br/>Star rating not set", true);
                }
            } else {
                this.showInfoOnMonitor("Ratings not enabled in settings<br/>[S > Metadata > Image rating]", true);
            }
        }

        public double resizeRatio(int rotate) {
            this.imageSettings["resizeRatio"] = 1;
            if (this.quickMetadata != null && this.quickMetadata.metadata.ContainsKey("imagewidth") && this.quickMetadata.metadata.ContainsKey("imageheight")) {
                int width;
                int height;
                try {
                    width = Convert.ToInt32(Regex.Match(this.quickMetadata.metadata["imagewidth"], @"\d+").Value);
                    height = Convert.ToInt32(Regex.Match(this.quickMetadata.metadata["imageheight"], @"\d+").Value);
                } catch (Exception e) {
                    this.showInfoOnMonitor("Error determining width/height of media:" + Environment.NewLine + e.Message);
                    return 1;
                }

                Rectangle newR = Constants.FitIntoBounds(new Rectangle(this.Bounds.Location, new Size(width, height)), this.Bounds, false, false);
                Rectangle oldR;
                Rectangle coverR;
                if (rotate == 90 || rotate == 270) {
                    // Internet Explorer fits image into boundaries before rotating
                    // Swap width, height values
                    // Determine size ratio between un-rotated and rotated image
                    this.imageSettings["resizeRatioCover"] = null;

                    // Set reseizeRatioCover for non stretched images
                    //if (!this.screensaver.config.getPersistantBool("stretchSmallImages")) {
                        oldR = Constants.FitIntoBounds(new Rectangle(this.Bounds.Location, new Size(height, width)), this.Bounds, true, false);
                        coverR = Constants.FitIntoBounds(new Rectangle(this.Bounds.Location, new Size(width, height)), this.Bounds, true, true);
                        this.imageSettings["resizeRatioCover"] = (float)coverR.Width / oldR.Height;
                    //} 

                    //oldR = Constants.FitIntoBounds(new Rectangle(this.Bounds.Location, new Size(height, width)), this.Bounds, false, false);
                    //coverR = Constants.FitIntoBounds(new Rectangle(this.Bounds.Location, new Size(width, height)), this.Bounds, false, true);
                    // Set reseizeRatioCover for stretched images
                    //if (this.imageSettings["resizeRatioCover"] == null) this.imageSettings["resizeRatioCover"] = (float)coverR.Width / oldR.Height;

                    this.quickMetadata.metadata["imagewidth"] = Convert.ToString(height);
                    this.quickMetadata.metadata["imageheight"] = Convert.ToString(width);
                    int temp = height;
                    height = width;
                    width = temp;
                    if (width > this.Bounds.Width || height > this.Bounds.Height) {
                        if (this.screensaver.config.getPersistantString("fitTo") == "cover") this.imageSettings["resizeRatio"] = this.imageSettings["resizeRatioCover"];
                        else this.imageSettings["resizeRatio"] = (float)oldR.Height / (float)newR.Width;
                    }
                } else {
                }
                if (this.screensaver.config.getPersistantBool("stretchSmallImages") && width < this.Bounds.Width && height < this.Bounds.Height) {
                    // Check if IE will resize the image before rotation
                    float ratio = 1;
                    if ((rotate == 90 || rotate == 270) && (height < this.Bounds.Width || width < this.Bounds.Height)) {
                        newR = Constants.FitIntoBounds(new Rectangle(this.Bounds.Location, new Size(height, width)), this.Bounds, false, false);
                        if (newR.Width != height) {
                            ratio = ((float)newR.Width / height);
                        }
                    }
                    //oldR = Constants.FitIntoBounds(new Rectangle(this.Bounds.Location, new Size(width, height)), this.Bounds, true, false);
                    //newR = Constants.FitIntoBounds(new Rectangle(this.Bounds.Location, new Size(width, height)), this.Bounds, true, true);
                    //this.imageSettings["resizeRatio"] = (float)newR.Width / width;
                    //   oldR = Constants.FitIntoBounds(new Rectangle(this.Bounds.Location, new Size(width, height)), this.Bounds, true, false);
                    newR = Constants.FitIntoBounds(new Rectangle(this.Bounds.Location, new Size(width, height)), this.Bounds, true, false);
                    this.imageSettings["resizeRatio"] = ((float)newR.Width / width) / ratio;
                }
            }
            return Convert.ToDouble(this.imageSettings["resizeRatio"]);
        }

        public void readMetadataImage() {
            if (this.currentImage != null) {
                //string metadata = "";
                string rawMetadata = null;

                this.imageSettings.Clear();
                this.imageSettings["metadata"] = "";

                string path = Convert.ToString(this.currentImage["path"]);
                string root = "";
                string filename = Path.GetFileNameWithoutExtension(path);
                string extension = Path.GetExtension(path);
                List<string> folders = Utils.stringToList(Convert.ToString(this.screensaver.config.getPersistant("folders")));
                foreach (string folder in folders) {
                    if (path.StartsWith(folder)) {
                        root = folder;
                        break;
                    }
                }
                // Whatever is leftover are the subfolders
                string subfolders = path.Substring(root.Length, path.Length - root.Length - filename.Length - extension.Length);

                this.imageSettings["path.root"] = root;
                this.imageSettings["path.subfolders"] = subfolders;
                this.imageSettings["path.filename"] = filename;
                this.imageSettings["path.extension"] = extension;

                if (this.currentImage.Table.Columns.Contains("all")) {
                    rawMetadata = Convert.ToString(this.currentImage["all"]);
                }
                if (rawMetadata == null || rawMetadata == "") {
                    rawMetadata = this.screensaver.fileNodes.getMetadataById(Convert.ToInt32(this.currentImage["id"]));
                    if (rawMetadata == null) {
                        try {
                            rawMetadata = this.screensaver.fileNodes.exifToolGetMetadata(Convert.ToString(this.currentImage["path"]) + Constants.ExifToolMetadataOptions, Convert.ToInt32(this.currentImage["id"]));
                        } catch (System.IO.FileNotFoundException fnfe) {
                            this.imageSettings["metadata"] = "ExifTool not found: '" + this.screensaver.config.getPersistant("exifTool") + "'";
                        } catch (Exception e) {
                            this.imageSettings["metadata"] = e.Message;
                        }
                    }
                }
                if (rawMetadata != null && rawMetadata != "") {
                    this.quickMetadata = new MetadataTemplate(rawMetadata, Utils.HtmlDecode(this.screensaver.config.getPersistantString("quickMetadata")));
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
                        int rotate = 0;
                        if (this.screensaver.config.getPersistantBool("exifRotate")) {
                            if (this.quickMetadata != null && this.quickMetadata.metadata.ContainsKey("orientation#")) {
                                switch (Convert.ToInt32(this.quickMetadata.metadata["orientation#"])) {
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
                            }
                        }
                        this.resizeRatio(rotate);
                        if (rotate != 0) {
                            this.imageSettings["exifRotate"] = rotate;
                        }
                        if (this.quickMetadata != null && this.quickMetadata.metadata.ContainsKey("imagewidth") && this.quickMetadata.metadata.ContainsKey("imageheight")) {
                            int width;
                            int height;
                            try { 
                                width = Convert.ToInt32(Regex.Match(this.quickMetadata.metadata["imagewidth"], @"\d+").Value);
                                height = Convert.ToInt32(Regex.Match(this.quickMetadata.metadata["imageheight"], @"\d+").Value);
                            } catch (Exception e) {
                                this.showInfoOnMonitor("Error determining width/height of media:" + Environment.NewLine + e.Message);
                                return;
                            }

                            float imgRatio = (float)width / (float)height;
                            this.imageSettings["width"] = width;
                            this.imageSettings["height"] = height;
                            this.imageSettings["ratio"] = imgRatio;
                            this.imageSettings["backgroundImage"] = this.screensaver.config.getPersistantBool("backgroundImage");
                            this.imageSettings["imageShadow"] = this.screensaver.config.getPersistantBool("imageShadow");
                            this.imageSettings["fitTo"] = this.screensaver.config.getPersistantString("fitTo");

                            bool fitToDimensionHeight;
                            if ((double)this.Bounds.Width / (double)this.Bounds.Height < (double)width / (double)height) {
                                //this.imageSettings["fitToDimension"] = "height";
                                fitToDimensionHeight = true;
                            } else {
                                //this.imageSettings["fitToDimension"] = "width";
                                fitToDimensionHeight = false;
                            }

                            if (fitToDimensionHeight) this.imageSettings["fitToDimension"] = "height";
                            else this.imageSettings["fitToDimension"] = "width";
/*
                            this.imageSettings["smallUnstretchedImage"] = false;
                            if (!this.screensaver.config.getPersistantBool("stretchSmallImages") && width < this.Bounds.Width && height < this.Bounds.Height) {
                                this.imageSettings["smallUnstretchedImage"] = true;

                            }*/

                            if (rotate != 90 && rotate != 270 && this.screensaver.getNrMonitors() > 1 && this.screensaver.config.getPersistantBool("stretchPanoramas") && imgRatio >= this.screensaver.desktopRatio) {
                                if (this.screensaver.config.getPersistantBool("stretchSmallImages") || width > this.Bounds.Width || height > this.Bounds.Height) {
                                    //this.imageSettings["fitToDimension"] = "width";
                                    this.imageSettings["pano"] = true;
                                    Rectangle pano, cover;
                                    pano = Constants.FitIntoBounds(new Rectangle(0, 0, width, height), this.screensaver.Desktop, this.screensaver.config.getPersistantBool("stretchSmallImages"), this.screensaver.config.getPersistantString("fitTo") == "cover");
                                    cover = Constants.FitIntoBounds(new Rectangle(0, 0, width, height), this.screensaver.Desktop, this.screensaver.config.getPersistantBool("stretchSmallImages"), true);
                                    if (this.id != 0 && this.Bounds.Height != this.screensaver.monitors[0].Bounds.Height) {
                                        this.imageSettings["pano.top"] = (this.Bounds.Height - pano.Height) / 2;
                                        this.imageSettings["pano.cover.top"] = (this.Bounds.Height - cover.Height) / 2;
                                    } else {
                                        this.imageSettings["pano.top"] = pano.Y - this.Bounds.Y;
                                        this.imageSettings["pano.cover.top"] = cover.Y - this.Bounds.Y;
                                    }
                                    this.imageSettings["pano.width"] = pano.Width;
                                    this.imageSettings["pano.left"] = pano.X - this.Bounds.X;
                                    this.imageSettings["pano.height"] = pano.Height;
                                    this.imageSettings["pano.cover.width"] = cover.Width;
                                    this.imageSettings["pano.cover.left"] = cover.X - this.Bounds.X;
                                    this.imageSettings["pano.cover.height"] = cover.Height;
                                }
                            } else {
                                for (int i = 0; i < this.screensaver.monitors.Length; i++) {
                                    this.screensaver.monitors[i].panoramaPart = false;
                                }
                            }
                        }
                    break;
                    case "object": 
                    case "video":
                        this.imageSettings["stretchSmallVideos"] = this.screensaver.config.getPersistantBool("stretchSmallVideos");
                        this.imageSettings["showcontrols"] = this.screensaver.config.getPersistantBool("showControls");
                        this.imageSettings["loop"] = this.screensaver.config.getPersistantBool("videosLoop");
                        this.imageSettings["mute"] = this.screensaver.config.getPersistantBool("videosMute");
                        this.imageSettings["videosPlay"] = this.screensaver.config.getPersistantString("videosPlay");
                    break;
                }
            }
        }

        public void showImage(bool animated) {
            if (this.currentImage != null) {
                this.imageSettings["play.interval"] = this.timer.Interval;
                this.imageSettings["animated"] = Convert.ToString(animated).ToLower();
                if (animated) {
                    this.imageSettings["effect"] = this.screensaver.config.getRandomEffect();
                }
                try {
                    var bgw = new BackgroundWorker();
                    if (this.screensaver.config.getPersistantBool("rawUseConverter") && (Convert.ToString(this.screensaver.config.getPersistant("rawExtensions")).IndexOf(Path.GetExtension(Convert.ToString(this.currentImage["path"])).ToLower()) > -1)) {
                        this.info = this.showInfoOnMonitor("Converting from RAW to JPEG <span class='wait'></span>", false, false);
                    }
                    bgw.DoWork += (object sender, DoWorkEventArgs e) => {
                        try {
                            e.Result = this.screensaver.fileNodes.checkImageCache(Convert.ToString(this.currentImage["path"]), this.id, ref this.imageSettings);
                        } catch (Exception f) {
                            // Error message is never displayed?!?
                            this.screensaver.monitors[this.id].showInfoOnMonitor(f.Message, true);
                        }
                    };
                    bgw.RunWorkerCompleted += (object sender, RunWorkerCompletedEventArgs e) => {
                        if (this.imageSettings.ContainsKey("raw") && Convert.ToBoolean(this.imageSettings["raw"])) {
                            this.imageSettings["exifRotate"] = 0;
                            this.imageSettings["resizeRatio"] = 1;
                            this.imageSettings["rawCached"] = e.Result;
                        }
                        this.showInfoOnMonitor(this.info, false, true);
                        try {
                            this.browser.Document.InvokeScript("showImage", new Object[] { e.Result, "<span class='folder'>" + Convert.ToString(this.currentImage["path"]) + "</span>", JsonConvert.SerializeObject(this.imageSettings) });
                        } catch (Exception f) {
                            this.showInfoOnMonitor(f.Message, true);
                        }
                    };
                    string path = "";
                    object[] prms = new object[] { path };
                    bgw.RunWorkerAsync(prms);
                } catch (Exception e) {
                    this.showInfoOnMonitor(e.Message, true);
                }
            }
        }

        public void saveDebug() {
            string filename = "_M" + (this.id + 1) + "_" + DateTime.Now.ToString("yyyyMMddhhmmss") + ".html";
            string path = this.browser.Url.LocalPath.Replace(Constants.MonitorHtmlFile, filename);
            try {
                string log;
                log = "<!--" + Environment.NewLine + JsonConvert.SerializeObject(this.currentImage) + Environment.NewLine + "-->";
                File.WriteAllText(path, this.browser.Document.GetElementsByTagName("HTML")[0].OuterHtml + Environment.NewLine + log);
            } catch (Exception) {
                path = Path.Combine(Constants.getProgramDataFolder(), Constants.DataFolder, filename);
                try {
                    File.WriteAllText(path, this.browser.Document.GetElementsByTagName("HTML")[0].OuterHtml);
                } catch (Exception e) {
                    this.showInfoOnMonitor("Error exporting HTML to '" + path + "'" + Environment.NewLine + e.Message, false, true);
                    return;
                }
            } 
            this.info = this.showInfoOnMonitor("HTML exported to " + path, false, false);
        }

        public void renameFile() {
            this.Focus();
            string input = Microsoft.VisualBasic.Interaction.InputBox("New name", "Rename", Path.GetFileName(Convert.ToString(this.currentImage["path"])));
            if (input != "" && input != Path.GetFileName(Convert.ToString(this.currentImage["path"]))) {
                string target = Path.Combine(Path.GetDirectoryName(Convert.ToString(this.currentImage["path"])), input);
                if (File.Exists(target)) {
                    MessageBox.Show("Can't rename file to" + Environment.NewLine + "'" + target + "'" + Environment.NewLine + "Already exists!", "File exists", MessageBoxButtons.OK, MessageBoxIcon.Error);
                } else {
                    try {
                        System.IO.File.Move(Convert.ToString(this.currentImage["path"]), target);
                        this.screensaver.fileNodes.updatePath(Convert.ToInt32(this.currentImage["id"]), target);
                    } catch (Exception e) {
                        MessageBox.Show(e.Message, "Error renaming file", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }
            }
        }

        /**
         * Check whether 
         *      file exists and 
         *      is not hidden if hidden files are ignore
         * If it doesn't exists 
         *      delete from DB and 
         *      set currentImage to null
         * Return true if file exists otherwise false
         **/
        public bool fileExistsOnDisk(DataRow dr) {
            if (dr == null) return true;

            bool delete = false;
            if (!File.Exists(Convert.ToString(dr["path"]))) delete = true;
            else {
                FileInfo fi = new FileInfo(Convert.ToString(dr["path"]));
                delete = !(!this.screensaver.config.getPersistantBool("ignoreHiddenFiles") || (this.screensaver.config.getPersistantBool("ignoreHiddenFiles") && (fi.Attributes & FileAttributes.Hidden) != FileAttributes.Hidden));
            }
            if (delete) {
                this.screensaver.fileNodes.deleteFromDB(Convert.ToInt32(dr["id"]));

                this.currentImage = null;
                this.resetFilterCount++;
                if (this.resetFilterCount > 10) {
                    this.resetFilterCount = 0;
                    this.screensaver.fileNodes.resetFilter();
                }
                if (this.screensaver.fileNodes.getNrImagesFilter() == 0) {
                    this.screensaver.showInfoOnMonitors(Constants.NoImagesFound, true);
                }
                return false;
            }
            return true;
        }

        public DataRow offsetImage(int i) {
            if (this.screensaver.config.getOrder() == Config.Order.Random) {
                if (this.id == 0 || !this.isMonitor0PanoramaImage(true)) {
                    this.offset += i;
                    //this.seedImage = this.currentImage;
                    this.currentImage = this.screensaver.fileNodes.getImageById(Convert.ToInt32(this.seedImageId), this.offset);
                }
                this.readMetadataImage();
                return this.currentImage;
            } else {
                // this.readMetadataImage() called in methods below;
                if (i > 0) return this.nextImage(1, false);
                // ToDo check for panoramaShown bugs
                else return this.previousImage(1, false);
            }
        }


        public void historyAdd(long id) {
            this.history.Add(id);
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
            if (count < 0) return new List<long>();
            return this.history.GetRange(start, count);
        }
/*
        public int getPanoramaOffset() {
            return getPanoramaOffset(false);
        }

        public int getPanoramaOffset(bool isInitial) {
            if (this.id > 0 && this.screensaver.monitors[0].imageSettings != null && Convert.ToBoolean(this.screensaver.monitors[0].imageSettings["pano"])) {
                //if (!isInitial) return 0;//
                return this.screensaver.monitors[0].offset;
                //else return this.screensaver.monitors[0].offset*-1;
                //return 0;
                //this.showInfoOnMonitor("Panorama not stretched as offset not 0<br/>Offset monitor 1: " + this.screensaver.monitors[0].offset + "; Offset monitor " + (this.id + 1) + ": " + this.offset);
            }
            return this.offset;
        }
        */
/*        public DataRow previousImage() {
            return this.previousImage(1);
        }*/

        public bool isMonitor0PanoramaImage(bool setAsCurrent) {
            if (this.screensaver.monitors[0].imageSettings != null && Convert.ToBoolean(this.screensaver.monitors[0].imageSettings["pano"])) {
                this.panoramaPart = true;
                if (setAsCurrent) this.currentImage = this.screensaver.monitors[0].currentImage;
                return true;
            }
            return false;
        }

        /**
         * Note step parameter for prevoiusImage is always positive unlike offset which indicates direction.
         **/
        public DataRow previousImage(int step, bool panoramaShownPreviously) { 
            return this.previousImage(step, panoramaShownPreviously, true); 
        }
        public DataRow previousImage(int step, bool panoramaShownPreviously, bool setCurrentImage) {
            DataRow currentImage = this.currentImage;
            long seedImageId = this.seedImageId;
            int historyPointer = this.historyPointer;

            if (this.screensaver.config.getOrder() == Config.Order.Random) {
                if (this.id == 0 || this.id > 0 && !panoramaShownPreviously) {
                    historyPointer -= step;
                    if (historyPointer < 0) {
                        historyPointer = 0;
                        this.showInfoOnMonitor("You've reached the first image");
                    }
                    if (!setCurrentImage && this.history.Count <= historyPointer) {
                        historyPointer = this.history.Count-1;
                        this.showInfoOnMonitor("You've reached the last image");
                    }
                    long imageId = -1;
                    if (this.history.Count > 0)
                        imageId = this.history[historyPointer];
                    currentImage = this.screensaver.fileNodes.getImageById(imageId, this.offset);
                    seedImageId = imageId;
                }
                isMonitor0PanoramaImage(true);
            } else {
                if (this.id == 0 && currentImage != null) {
                    // Previous navigation is flawwed only possible fix is keep record of which images shown
                    // If panorama found, there is no way to tell whether it was shown on screen 2 only or across screens 1 + 2

                    // Optimised panoramas 
                    //this.currentImage = this.screensaver.fileNodes.getSequentialImage(Convert.ToInt32(this.currentImage["id"]), (step * -1));
                    // Optimised for displaying regular images (skips some panoramas)
                    int stepBy = (step + this.screensaver.monitors.Length - 1) * -1;
                    if (this.screensaver.config.getPersistantString("mmImages") == "same" || (setCurrentImage && this.screensaver.config.getPersistantString("mmImages") == "slide")) {
                        stepBy = step * -1;
                    }
                    currentImage = this.screensaver.fileNodes.getSequentialImage(Convert.ToInt32(currentImage["id"]), stepBy, !setCurrentImage);
                } else {
                    if (!isMonitor0PanoramaImage(true)) {
                        currentImage = this.screensaver.fileNodes.getSequentialImage(1, !setCurrentImage);
                    }
                }
            }
            if (!fileExistsOnDisk(currentImage)) return this.previousImage(1, panoramaShownPreviously, setCurrentImage);
            if (setCurrentImage) {
                this.currentImage = currentImage;
                this.seedImageId = seedImageId;
                this.historyPointer = historyPointer;
                this.readMetadataImage();
                return this.currentImage;
            } else {
                return currentImage;
            }
            
        }

        public DataRow nextImage(int step, bool panoramaShownPreviously) {
            if (this.id != 0) {
                // Note check for same / slide previousImage is performed in RPS.cs
                if (this.screensaver.config.getPersistantString("mmImages") == "same") {
                    this.currentImage = this.screensaver.monitors[0].currentImage;
                    this.readMetadataImage();
                    return this.currentImage;
                }
                if (this.screensaver.config.getPersistantString("mmImages") == "slide") {
                    int extra;
                    if (this.screensaver.config.getOrder() == Config.Order.Random) extra = 0; else extra = -1;
                    this.currentImage = this.screensaver.monitors[0].previousImage(this.id + extra, panoramaShownPreviously, false);
                    this.readMetadataImage();
                    return this.currentImage;
                }
            }
            if (this.id == 0 || !this.isMonitor0PanoramaImage(true)) {
                if (this.screensaver.fileNodes == null) return null;
                if (this.screensaver.config.getOrder() == Config.Order.Random) {
                    this.historyPointer += step;
                    if (this.historyPointer < this.history.Count()) {
                        this.currentImage = this.screensaver.fileNodes.getImageById(this.history[this.historyPointer], this.offset);
                        if (this.currentImage != null) this.seedImageId = Convert.ToInt32(this.currentImage["id"]);
                    } else {
                        this.historyPointer = this.history.Count();
                        this.currentImage = this.screensaver.fileNodes.getRandomImage();
                        if (this.currentImage != null) {
                            this.historyAdd(Convert.ToInt32(this.currentImage["id"]));
                            this.seedImageId = Convert.ToInt32(this.currentImage["id"]);
                            if (this.offset != 0) {
                                this.currentImage = this.screensaver.fileNodes.getImageById(this.seedImageId, this.offset);
                            }
                        }
                    }
                } else {
                    this.currentImage = this.screensaver.fileNodes.getSequentialImage(step);
                }
                if (!fileExistsOnDisk(this.currentImage)) return this.nextImage(step, panoramaShownPreviously);
            }
            if (this.currentImage != null) { 
                this.screensaver.appendDebugFile(this.id, Convert.ToString(this.currentImage["path"]));
                this.readMetadataImage();
            }
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

        public void jsOverrideTimerInterval(float duration) {
            duration *= 1000;
            if (duration > this.timer.Interval && this.screensaver.config.getPersistantString("videosPlay") == "entire") {
                int monitorID = this.id;
                if (this.screensaver.config.syncMonitors()) monitorID = Screensaver.CM_ALL;
                for (int i = 0; i < this.screensaver.monitors.Length; i++) {
                    if (this.screensaver.monitors[i] != null) {
                        if (monitorID == Screensaver.CM_ALL || monitorID == i) {
                            this.screensaver.monitors[i].timer.Stop();
                            if (i <= this.id || this.screensaver.monitors[i].timer.Interval < Convert.ToInt32(duration)) this.screensaver.monitors[i].timer.Interval = Convert.ToInt32(duration);
                            this.screensaver.monitors[i].timer.Start();
                        }
                    }
                }
            }
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
            bool panoramaShownPreviously = this.isMonitor0PanoramaImage(true);
            this.nextImage(1, panoramaShownPreviously);
            if (this.currentImage == null || Convert.ToString(this.currentImage["path"]) == "") {
                this.timer.Interval *= 2;
                if (this.screensaver.config.persistantLoaded() && this.timer.Interval > Convert.ToInt32(this.screensaver.config.getPersistant("timeout")) * 1000) {
                    this.setTimerInterval();
                }
            } else {
                this.setTimerInterval();
                this.showImage(true);
                if (this.id == 0 && this.screensaver.config.syncMonitors()) {
                    for (int i = 1; i < this.screensaver.monitors.Length; i++) {
                        if (this.screensaver.currentMonitor == Screensaver.CM_ALL || this.screensaver.currentMonitor == i) {
                            this.screensaver.monitors[i].timer.Stop();
                            this.screensaver.monitors[i].nextImage(1, panoramaShownPreviously);
                            this.screensaver.monitors[i].showImage(true);
                        }
                    }
                }
            }
        }
    }
}
