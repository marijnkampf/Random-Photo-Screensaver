using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Linq;
using System.ComponentModel;
using System.Diagnostics;
using System.Threading;
using Microsoft.VisualBasic.FileIO;
using System.Drawing;
using Newtonsoft.Json;
using System.Management;
using Microsoft.Win32;
using System.Runtime.InteropServices;
//using System.Windows.Forms.HtmlElement;

namespace RPS {
    public class Screensaver : ApplicationContext {
        private int mouseX = -1, mouseY = -1;
        public static int CM_ALL = -1;

        private bool configInitialised = false;
        public bool applicationClosing = false;
        private IntPtr[] hwnds;
        private System.Windows.Forms.Keys previousKey;
        public bool configHidden = false;

        #if (DEBUG)
            public List<string> debugLog;
        #endif

        public int currentMonitor = CM_ALL;

        public enum Actions { Config, Preview, Screensaver, Slideshow, Test, Wallpaper };
        public Actions action;
        public Config config;
        public Monitor[] monitors;
        public Rectangle Desktop;
        public float desktopRatio;
        public FileNodes fileNodes;
        public bool readOnly;

        public Version version;

        private System.Windows.Forms.Timer mouseMoveTimer;

        #region Win32 API functions
        [DllImport("user32.dll")]
        public static extern void SwitchToThisWindow(IntPtr hWnd, bool turnon);
        #endregion

        private Screensaver(Actions action, bool readOnly, IntPtr[] hwnds) {
            #if (DEBUG)
                this.debugLog = new List<string>();
            #endif
            
            this.version = new Version(Application.ProductVersion);
            this.readOnly = readOnly;
            this.action = action;
            this.hwnds = hwnds;
            this.config = new Config(this);
            this.config.PreviewKeyDown += new System.Windows.Forms.PreviewKeyDownEventHandler(this.PreviewKeyDown);
            this.config.browser.PreviewKeyDown += new System.Windows.Forms.PreviewKeyDownEventHandler(this.PreviewKeyDown);
//            this.config.browser.OnMouseMove += new System.Windows.Forms.MouseEventHandler(this.MouseMove);

            this.config.browser.Navigate(new Uri(Constants.getDataFolder(Constants.ConfigHtmlFile)));
            this.config.browser.DocumentCompleted += new System.Windows.Forms.WebBrowserDocumentCompletedEventHandler(this.config.ConfigDocumentCompleted);
            if (this.action == Actions.Config) this.config.Show();
            else {
                if (this.action != Actions.Wallpaper) {
                    this.mouseMoveTimer = new System.Windows.Forms.Timer();
                    this.mouseMoveTimer.Interval = 1500;
                    this.mouseMoveTimer.Tick += mouseMoveTimer_Tick;
                }
            }
            // Wait for config document to load to complete initialisation
        }

        // Return the number of monitors pre initialising monitors array
        public int getNrMonitors() {
            int nrMonitors = 1;
            if (this.action == Actions.Test || this.action == Actions.Preview) {
                nrMonitors = hwnds.Length;
            } else {
                nrMonitors = Screen.AllScreens.Length;
            }
            return nrMonitors;
        }

        public void initForScreensaverAndWallpaper() {
            int nrMonitors = this.getNrMonitors();
            // Avoid double loading config from DB
            if (!config.persistantLoaded()) {
                this.config.loadPersistantConfig(nrMonitors);
            }
            this.configInitialised = true;
            this.fileNodes = new FileNodes(this.config, this);
            if (this.config.getPersistantBool("useFilter")) {
                this.fileNodes.setFilterSQL(this.config.getPersistantString("filter"));
            }
            this.Desktop = Constants.getDesktopBounds();
            this.desktopRatio = Desktop.Width / Desktop.Height;
        }

//        private void ConfigDocumentCompleted(object sender, System.Windows.Forms.WebBrowserDocumentCompletedEventArgs e) {
        public void initializeMonitors() {
            #if (DEBUG)
            this.debugLog.Add("initializeMonitors");
            #endif

            //MessageBox.Show("ConfigDocumentCompleted:" + this.action.ToString());
            if (this.action != Actions.Config && this.action != Actions.Wallpaper) {
                // Complete initialisation when config.html is loaded.
                if (!this.configInitialised && this.config.browser.Url.Segments.Last().Equals(Constants.ConfigHtmlFile)) {
                    this.initForScreensaverAndWallpaper();
                    System.Drawing.Color backgroundColour = System.Drawing.ColorTranslator.FromHtml(this.config.getPersistantString("backgroundColour"));
                    int i = 0;
                    if (this.action == Actions.Test || this.action == Actions.Preview) {
                        int start = 0;
                        // Skip first monitor for preview
                        if (this.action == Actions.Preview) {
                            start = 1;
                        } else {
                            this.monitors = new Monitor[hwnds.Length];
                        } 

                        for (i = start; i < hwnds.Length; i++) {
                            this.monitors[i] = new Monitor(hwnds[i], i, this);
                            this.monitors[i].FormClosed += new FormClosedEventHandler(this.OnFormClosed);
                            this.monitors[i].PreviewKeyDown += new System.Windows.Forms.PreviewKeyDownEventHandler(this.PreviewKeyDown);
                            this.monitors[i].browser.PreviewKeyDown += new System.Windows.Forms.PreviewKeyDownEventHandler(this.PreviewKeyDown);
                            this.monitors[i].Show();
                        }
                    } else {
                        this.monitors = new Monitor[Screen.AllScreens.Length];
                        foreach (Screen screen in Screen.AllScreens) {
                            this.monitors[i] = new Monitor(screen.Bounds, i, this);
                            this.monitors[i].browser.PreviewKeyDown += new System.Windows.Forms.PreviewKeyDownEventHandler(this.PreviewKeyDown);
                            this.monitors[i].PreviewKeyDown += new System.Windows.Forms.PreviewKeyDownEventHandler(this.PreviewKeyDown);
                            // Avoid white flash by hiding browser and setting form background colour. (Focus on browser on DocumentCompleted to process keystrokes)
                            this.monitors[i].browser.Hide();
                            try {
                                this.monitors[i].BackColor = backgroundColour;
                            } catch (System.ArgumentException ae) { }
                            this.monitors[i].Show();
                            i++;
                        }
                    }
                    this.MonitorsAndConfigReady();
                }
            }
        }

        private void CleanUpOnException(object sender, UnhandledExceptionEventArgs args) {
            this.fileNodes.OnExitCleanUp();
        }

        private void MonitorsAndConfigReady() {
            #if (DEBUG)
                this.debugLog.Add("MonitorsAndConfigReady");
            #endif

            if (this.action != Actions.Preview) {
                for (int i = 0; i < this.monitors.Length; i++) {
                    if (this.config.hasPersistantKey("historyM" + Convert.ToString(i)) && this.config.hasPersistantKey("historyOffsetM" + Convert.ToString(i))) {
                        this.monitors[i].setHistory(JsonConvert.DeserializeObject<List<long>>((string)this.config.getPersistant("historyM" + Convert.ToString(i))), Convert.ToInt32((string)this.config.getPersistant("historyOffsetM" + Convert.ToString(i))));
                    }
                }
            }
        }

        /***
         * Detect other RPS processes
         ***/
        private static bool singleProcess(Actions action) {
            Process currentProcess = Process.GetCurrentProcess();
            // Adapted from: http://stackoverflow.com/questions/504208/how-to-read-command-line-arguments-of-another-process-in-c
            string wmiQuery = string.Format("select Handle, ProcessId, CommandLine from Win32_Process where Name='{0}'", Path.GetFileName(currentProcess.MainModule.FileName));
            ManagementObjectSearcher searcher = new ManagementObjectSearcher(wmiQuery);
            ManagementObjectCollection retObjectCollection = searcher.Get();
            foreach (ManagementObject retObject in retObjectCollection) {
                if (currentProcess.Id != Convert.ToInt32(retObject["ProcessId"])) {
                    Console.WriteLine("[{0}]", retObject["CommandLine"] + " " + retObject["ProcessId"] + " ~ " + currentProcess.Id);
                    // If both self and process as screensaver /s switch to existing and exit
                    if (!System.Diagnostics.Debugger.IsAttached) {
                        if (action == Actions.Screensaver && !retObject["CommandLine"].ToString().Contains("/c") && !retObject["CommandLine"].ToString().Contains("/p")) {
                            try {
                                Process p = Process.GetProcessById(Convert.ToInt32(retObject["ProcessId"]));
                                SwitchToThisWindow(p.MainWindowHandle, false);
                                Environment.Exit(0);
                            } catch (System.ArgumentException ae) {

                            }
                        }
                        // Make this version read only
                        return true;
                    }
                }
            }       
            return false;
        }

        private void DoWorkDeleteFile(object sender, DoWorkEventArgs e) {
            //            Debug.WriteLine(this.config.getPersistant("folders"));
            BackgroundWorker worker = sender as BackgroundWorker;
            // Lower priority to ensure smooth working of main screensaver
            System.Diagnostics.Process.GetCurrentProcess().PriorityClass = System.Diagnostics.ProcessPriorityClass.BelowNormal;
            int monitorId = Convert.ToInt32(((object[])(e.Argument))[0]);
            string filename = Convert.ToString(((object[])(e.Argument))[1]);
            int i = 0;
            while (File.Exists(filename) && i < 100) {
                i++;
                try {
                    FileSystem.DeleteFile(filename, UIOption.OnlyErrorDialogs, RecycleOption.SendToRecycleBin, UICancelOption.DoNothing);
                    this.fileNodes.deleteFromDB(filename);
                } catch (ArgumentNullException ane) {
                    this.monitors[i].showInfoOnMonitor("Nothing to delete");
                } catch (Exception ex) {
                    if (this.monitors[i].imagePath() == filename) this.monitors[i].showInfoOnMonitor("Deleting\n"+Path.GetFileName(filename));
				    Thread.Sleep(1000);
                }
            }
        }

        public void showInfoOnMonitors(string info) {
            this.showInfoOnMonitors(info, false);
        }

        public void showInfoOnMonitors(string info, bool highPriority) {
            this.showInfoOnMonitors(info, highPriority, false);
        }

        public void showUpdateInfo(string info) {
            if (this.action == Actions.Config) {
                this.config.showUpdateInfo(info);
            } else {
                this.monitors[0].showUpdateInfo(info);
            }
        }


        public void showInfoOnMonitors(string info, bool highPriority, bool fade) {
            if (this.action == Actions.Config) {
                this.config.showUpdateInfo(info);
            } else {
                for (int i = 0; i < this.monitors.Length; i++) {
                    if ((this.currentMonitor == CM_ALL) || (this.currentMonitor == i)) {
                        this.monitors[i].showInfoOnMonitor(info, highPriority, fade);
                        //this.monitors[i].browser.Document.InvokeScript("showInfo", new String[] { info });
                    }
                }
            }
        }

        public void pauseAll(bool showInfo) {
            for (int i = 0; i < this.monitors.Length; i++) {
                this.monitors[i].timer.Enabled = false;
                if (showInfo) this.monitors[i].showInfoOnMonitor("||");
            }
        }

        public void resumeAll(bool showInfo) {
            for (int i = 0; i < this.monitors.Length; i++) {
                //this.monitors[i].timer.Enabled = true;
                this.monitors[i].startTimer();
                if (showInfo) this.monitors[i].showInfoOnMonitor("|>");
            }
        }

        public void startTimers() {
            //for (int i = (this.monitors.Length - 1); i >= 0 ; i--) {
            for (int i = 0; i < this.monitors.Length; i++) {
    //            if (this.currentMonitor == CM_ALL || this.currentMonitor == i) {
                    ///this.monitors[i].setTimerInterval();
                    this.monitors[i].startTimer();
                    //this.monitors[i].timer.Enabled = !this.monitors[i].paused;
                    /*if (!this.monitors[i].paused) {
                        this.monitors[i].timer.Start();
                    }*/
                    //
      //          }
            }
        }

        public void stopTimers() {
            for (int i = 0; i < this.monitors.Length; i++) {
//                if (this.currentMonitor == CM_ALL || this.currentMonitor == i) {
                    //this.monitors[i].timer.Stop();
                    this.monitors[i].timer.Enabled = false;
  //              }
            }
        }

        public void actionNext(int step) {
            this.stopTimers();
            for (int i = 0; i < this.monitors.Length; i++) {
                if (this.currentMonitor == CM_ALL || this.currentMonitor == i) {
                    this.monitors[i].timer.Stop();
                    string s = "";
                    if (step > 1) s = " x " + step;
                    this.monitors[i].showInfoOnMonitor(">>" + s);
                    this.monitors[i].nextImage(step);
                    this.monitors[i].showImage(this.config.getPersistantBool("useTransitionsOnInput"));
                    //this.monitors[i].startTimer();
                }
            }
            this.startTimers();
        }

        public void actionPrevious(int step) {
            this.stopTimers();
            for (int i = 0; i < this.monitors.Length; i++) {
                //for (int i = (this.monitors.Length - 1); i >= 0 ; i--) {
                if (this.currentMonitor == CM_ALL || this.currentMonitor == i) {
                    this.monitors[i].timer.Stop();
                    //if (i > 0) this.monitors[i].imageSettings["pano"] = false;
                    string s = "";
                    if (step > 1) s = " x " + step;
                    this.monitors[i].showInfoOnMonitor("<<" + s);
                    this.monitors[i].previousImage(step);
                    this.monitors[i].showImage(this.config.getPersistantBool("useTransitionsOnInput"));
                    //this.monitors[i].startTimer();
                }
            }
            this.startTimers();
        }

        public int getStep(PreviewKeyDownEventArgs e) {
            if (e.Shift) return 5;
            if (e.Control) return 25;
            if (e.Alt) return 100;
            return 1;
        }

        public void PreviewKeyDown(object sender, PreviewKeyDownEventArgs e) {
            // Ignore shortcut keys when Config screen is visible
            // Ignore repeated keys
            if (this.previousKey == e.KeyCode) {
                this.previousKey = 0;
            } else {
                if (this.config.Visible) {
                    switch (e.KeyCode) {
                        case Keys.Escape:
                            if (this.config.WindowState == FormWindowState.Minimized) {
                                this.config.WindowState = FormWindowState.Normal;
                            } else {
                                if (this.action == Actions.Config) {
                                    this.config.Config_FormClosing(this, null);
                                    //this.OnExit();
                                } else {
                                    this.configHidden = true;
                                    this.config.Hide();
                                }
                            }
                        break;
                        #if (DEBUG)
                        case Keys.F12:
                            string log = "";
                            foreach(string s in this.debugLog) {
                                log += s + Environment.NewLine;
                            }
                            MessageBox.Show(log, "Debug log:");
                        break;
                        #endif
                        case Keys.S:
                            if (this.config != Form.ActiveForm) {
                                this.config.Activate();
                            }
                        break;
                    }
                } else {
		            Keys KeyCode = e.KeyCode;
		            // fix German keyboard codes for [ ]
		            if (e.Alt && e.Control) {
			            switch (e.KeyCode) {
				            case Keys.D8: 
                                KeyCode = Keys.OemOpenBrackets;
				            break;
				            case Keys.D9:
					            KeyCode = Keys.OemCloseBrackets;
				            break;
			            }
		            }
                    switch (KeyCode) {
                        case Keys.Escape:
                            if (!this.configHidden) {
                                this.OnExit();
                            }
                            this.configHidden = false;
                        break;
                        case Keys.C:
                            this.showInfoOnMonitors("Calendar probably won' be implemented");
                        break;
                        case Keys.E:
                            for (int i = 0; i < this.monitors.Length; i++) {
                                if (this.currentMonitor == CM_ALL || this.currentMonitor == i) {
                                    if (this.monitors[i].imagePath() != null) {
                                        if (e.Control) {
                                            if (!File.Exists(Convert.ToString(this.config.getPersistant("externalEditor")))) {
                                                this.monitors[i].showInfoOnMonitor("External editor: '" + this.config.getPersistant("externalEditor") + "' not found.", true, true);
                                            } else {
                                                Process.Start(Convert.ToString(this.config.getPersistant("externalEditor")), this.monitors[i].imagePath());
                                            }
                                        } else {
                                            Process.Start("explorer.exe", "/e,/select," + this.monitors[i].imagePath());
                                        }
                                    }
                                }
                            }
                            if (Convert.ToBoolean(this.config.getPersistantBool("closeAfterImageLocate"))) this.OnExit();
                        break;
                        case Keys.F:
                        case Keys.NumPad7:
                            for (int i = 0; i < this.monitors.Length; i++) {
                                if (this.currentMonitor == CM_ALL || this.currentMonitor == i) {
                                    this.config.setPersistant("showFilenameM" + (i + 1), Convert.ToString(this.monitors[i].InvokeScript("toggle", new string[] { "#filename" })));
                                }
                            }
                        break;
                        case Keys.I:
                            for (int i = 0; i < this.monitors.Length; i++) {
                                if (this.currentMonitor == CM_ALL || this.currentMonitor == i) {
                                    this.monitors[i].browser.Document.InvokeScript("identify");
                                }
                            }
                        break;
                        case Keys.M:
                            this.showInfoOnMonitors("ToDo: Implement Metadata form");
                        break;
                        case Keys.N:
                            for (int i = 0; i < this.monitors.Length; i++) {
                                if (this.currentMonitor == CM_ALL || this.currentMonitor == i) {
                                    this.config.setPersistant("showQuickMetadataM" + (i + 1), Convert.ToString(this.monitors[i].InvokeScript("toggle", new string[] { "#quickMetadata" })));
                                }
                            }
                        break;
                        case Keys.P:
                            for (int i = 0; i < this.monitors.Length; i++) {
                                if (this.currentMonitor == CM_ALL || this.currentMonitor == i) {
                                    if (!this.config.syncMonitors() || i == 0) {
                                        this.monitors[i].paused = !this.monitors[i].paused;
                                        this.monitors[i].timer.Enabled = !this.monitors[i].paused;
                                    } else {
                                        this.monitors[i].paused = this.monitors[0].paused;
                                        this.monitors[i].timer.Enabled = !this.monitors[0].paused;
                                    }
                                    if (this.monitors[i].timer.Enabled) this.monitors[i].showInfoOnMonitor("|>");
                                    else this.monitors[i].showInfoOnMonitor("||");
                                }
                            }
                        break;
                        case Keys.R: case Keys.NumPad1:
                            if (this.config.changeOrder() == Config.Order.Random) {
                                this.showInfoOnMonitors("Randomising");
                            } else {
                                int monitor = this.currentMonitor;
                                if (this.currentMonitor == CM_ALL) monitor = 0;
                                if (this.monitors[monitor].currentImage != null) {
                                    this.fileNodes.currentSequentialSeedId = Convert.ToInt32(this.monitors[monitor].currentImage["id"]);
                                }
                                this.showInfoOnMonitors("Sequential");
                            };
                        break;
                        case Keys.S:
                            // Don't hide config screen if application is in Config mode
                            if (this.action != Actions.Config) {
                                if (this.config.Visible) this.config.Hide();
                                else {
                                    this.config.Activate();
                                    this.config.Show();
                                }
                            } else {

                            }
                        break;
                        case Keys.T: case Keys.NumPad5:
                            for (int i = 0; i < this.monitors.Length; i++) {
                                if (this.currentMonitor == CM_ALL || this.currentMonitor == i) {
                                    string display = "true";
                                    string clockType = "current";
                                    switch(Convert.ToString(this.config.getPersistant("clockM" + (i + 1)))) {
                                        case "none":
                                            this.config.setPersistant("currentClockM" + (i + 1), "checked");
                                            clockType = "current";
                                        break;
                                        case "current":
                                            this.config.setPersistant("elapsedClockM" + (i + 1), "checked");
                                            clockType = "elapsed";
                                        break;
                                        case "elapsed":
                                            this.config.setPersistant("noClockM" + (i + 1), "checked");
                                            clockType = "none";
                                            display = "false";
                                        break;
                                    }
                                    this.config.setPersistant("clockM" + (i + 1), clockType);
                                    this.monitors[i].InvokeScript("setClockType", new string[] { clockType  });
                                    this.monitors[i].InvokeScript("toggle", new string[] { "#clock", display });
                                }
                            }
                        break;
                        case Keys.U:
                            string updateFilename = this.config.updateFilename();
                            if (File.Exists(updateFilename)) {
                                Process.Start("explorer.exe", "/e,/select," + updateFilename);
                            }
                            //this.config.installUpdate();
                        break;
                        case Keys.W:
                            string[] paths = new string[this.monitors.Length];
                            for (int i = 0; i < this.monitors.Length; i++) {
                                paths[i] = Convert.ToString(this.monitors[i].imagePath());
                                if (this.currentMonitor == CM_ALL || this.currentMonitor == i) {
                                    this.monitors[i].showInfoOnMonitor("Setting as wallpaper");
                                }
                            }
                            Wallpaper wallpaper = new Wallpaper(this);
                            wallpaper.setWallpaper(this.currentMonitor, paths);
                        break;
                        case Keys.D0:
                            this.currentMonitor = CM_ALL;
                            for (int i = 0; i < this.monitors.Length; i++) {
                                this.monitors[i].browser.Document.InvokeScript("identify");
                            }
                            break;
                        case Keys.D1:case Keys.D2:case Keys.D3:
                        case Keys.D4:case Keys.D5:case Keys.D6:
                        case Keys.D7:case Keys.D8:case Keys.D9:
                            int monitorId = e.KeyValue-49;
                            if (monitorId < this.monitors.Length) {
                                this.currentMonitor = monitorId;
                                this.monitors[monitorId].browser.Document.InvokeScript("identify");
                            }
                        break;
                        case Keys.NumPad4: case Keys.Left:
                            this.actionPrevious(this.getStep(e));
                        break;
                        case Keys.NumPad6: case Keys.Right:
                            this.actionNext(this.getStep(e));
                        break;
                        case Keys.NumPad2: case Keys.Down:
                            this.stopTimers();
                            for (int i = 0; i < this.monitors.Length; i++) {
                                if (this.currentMonitor == CM_ALL || this.currentMonitor == i) {
                                    this.monitors[i].timer.Stop();
                                    this.monitors[i].offsetImage(1);
                                    this.monitors[i].showImage(this.config.getPersistantBool("useTransitionsOnInput"));
                                    this.monitors[i].showInfoOnMonitor("v (" + this.monitors[i].offset + ")");
                                    this.monitors[i].startTimer();
                                }
                            }
                            this.startTimers();
                        break;
                        case Keys.NumPad8: case Keys.Up:
                            this.stopTimers();
                            for (int i = 0; i < this.monitors.Length; i++) {
                                if (this.currentMonitor == CM_ALL || this.currentMonitor == i) {
                                    this.monitors[i].timer.Stop();
                                    this.monitors[i].offsetImage(-1);
                                    this.monitors[i].showImage(this.config.getPersistantBool("useTransitionsOnInput"));
                                    this.monitors[i].showInfoOnMonitor("^ (" + this.monitors[i].offset + ")");
                                    this.monitors[i].startTimer();
                                }
                            }
                            this.startTimers();
                        break;
                        case Keys.OemOpenBrackets:
                        case Keys.OemCloseBrackets: 
                        case Keys.Oemplus:
                            this.stopTimers();
                            int deg = 0;
                            string message = "";
                            switch (KeyCode) {
                                case Keys.OemOpenBrackets: 
                                    deg = 270;
                                    message = "Rotating 270° clock wise";
                                break;
                                case Keys.OemCloseBrackets: 
                                    deg = 90;
                                    message = "Rotating 90° clock wise";
                                break;
                                case Keys.Oemplus: 
                                    deg = 180;
                                    message = "Upside down you're turning me";
                                break;
                            }

                            for (int i = 0; i < this.monitors.Length; i++) {
                                if (this.currentMonitor == CM_ALL || this.currentMonitor == i) {
                                    this.monitors[i].info = message;
                                    this.monitors[i].showInfoOnMonitor(message, true, true);
                                    this.monitors[i].rotateImage(deg);
                                }
                            }
                            this.startTimers();
                        break;
                        case Keys.Delete:
                        if (this.config.getPersistantBool("deleteKey")) {
                                this.pauseAll(false);
                                for (int i = 0; i < this.monitors.Length; i++) {
                                    if (this.currentMonitor == CM_ALL || this.currentMonitor == i) {
                                        bool deleteFile = true;
                                        string filename = this.monitors[i].imagePath();
                                        if (filename != null && filename.Length > 0 && File.Exists(filename)) {
                                            Cursor.Show();
                                            this.monitors[i].Focus();
                                            if (DialogResult.Yes == MessageBox.Show("Are you sure you want to sent '" + Path.GetFileName(filename) + "' to the Recycle Bin?", "Confirm File Delete", MessageBoxButtons.YesNo, MessageBoxIcon.Exclamation)) {
                                                deleteFile = true;
                                            } else {
                                                deleteFile = false;
                                            }
                                            Cursor.Hide();
                                            if (deleteFile) {
                                                BackgroundWorker bgwDeleteFile = new BackgroundWorker();
                                                bgwDeleteFile.DoWork += new DoWorkEventHandler(DoWorkDeleteFile);
                                                bgwDeleteFile.RunWorkerAsync(new Object[] {i, filename});
                                            }
                                        }
                                    }
                                }
                                this.resumeAll(false);
                            }
                        break;
                        default:
        		            if (!e.Alt && !e.Control && !e.Shift) {
                                if (!this.config.getPersistantBool("onlyEscapeExits")) {
                                    this.OnExit();
                                }
                            }
                        break;
                    }
                    this.previousKey = e.KeyCode;
                }
            } 
        }

        public void OnExit() {
            Cursor.Show();
            if (this.fileNodes != null) {
                this.fileNodes.CancelBackgroundWorker();
                this.config.setPersistant("sequentialStartImageId", this.fileNodes.currentSequentialSeedId.ToString());
            }
            this.config.safePersistantConfig();
            if (this.fileNodes != null) this.fileNodes.OnExitCleanUp();
            // Manually call config close to ensure it will not cancel the close.
            this.applicationClosing = true;
            Application.Exit();
        }

        private void OnFormClosed(object sender, EventArgs e) {
            this.OnExit();
        }

        class MouseMessageFilter : IMessageFilter {
            public static event MouseEventHandler MouseMove = delegate { };
            public static event MouseEventHandler MouseClick = delegate { };
            const int WM_MOUSEMOVE = 0x0200;
            const int WM_LBUTTONDOWN = 0x0201;
            const int WM_LBUTTONUP = 0x0202;
            const int WM_MOUSEWHEEL = 0x020A;
            const int WM_RBUTTONDOWN = 0x0204;
            const int WM_RBUTTONUP = 0x0205;

            public bool PreFilterMessage(ref Message m) {
                switch (m.Msg) { 
                    case WM_MOUSEMOVE:
                        MouseMove(null, new MouseEventArgs(MouseButtons.None, 0, Control.MousePosition.X, Control.MousePosition.Y, 0));
                    break;
                    case WM_LBUTTONUP: case WM_RBUTTONUP:
                        MouseButtons mb = MouseButtons.Left;
                        switch(m.Msg) {
                            case WM_LBUTTONUP: 
                                mb = MouseButtons.Left;
                            break;
                            case WM_RBUTTONUP:
                                mb = MouseButtons.Right;
                            break;
                        }
                        MouseClick(null, new MouseEventArgs(mb, 1, Control.MousePosition.X, Control.MousePosition.Y, 0));
                    break;


                }
                return false;
            }
        }

        private void MouseClick(object sender, MouseEventArgs e) {
            if (!this.config.Visible) {
                //if (this.config.hiding) return null;
                if (this.config.getPersistantBool("browseMouse")) {
                    switch (e.Button) {
                        case MouseButtons.Left:
                            this.actionNext(1);
                            break;
                        case MouseButtons.Right:
                            this.actionPrevious(1);
                            break;
                    }
                } else {
                    this.OnExit();
                }
            }
        }

        public void resetMouseMove() {
            this.mouseX = -1;
            this.mouseY = -1;

        }

        public void MouseMove(object sender, MouseEventArgs e) {
            if (this.config.Visible) {
                this.resetMouseMove();
            } else {
                if (this.mouseX != e.X && this.mouseY != e.Y && this.mouseX != -1 && this.mouseY != -1) {
                    Cursor.Show();
                    this.mouseMoveTimer.Stop();
                    this.mouseMoveTimer.Start();
                    this.mouseMoveTimer.Enabled = true;
                }
                if (this.mouseX == -1) this.mouseX = e.X;
                if (this.mouseY == -1) this.mouseY = e.Y;
                int sensitivity = 0;
                switch (this.config.getPersistantString("mouseSensitivity")) {
                    case "high":
                        sensitivity = 0;
                    break;
                    case "medium":
                        sensitivity = 10;
                    break;
                    case "low":
                        sensitivity = 50;
                    break;
                    case "none":
                        this.mouseX = e.X;
                        this.mouseY = e.Y;
                    break;
                }
                if (e.X > (this.mouseX + sensitivity) || e.X < (this.mouseX - sensitivity) ||
                    e.Y > (this.mouseY + sensitivity) || e.Y < (this.mouseY - sensitivity)) {
                        this.OnExit();
                }
                this.mouseX = e.X;
                this.mouseY = e.Y;
            }
        }

        void mouseMoveTimer_Tick(object sender, EventArgs e) {
            Cursor.Hide();
            this.mouseMoveTimer.Enabled = false;
        }


   
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main(string[] args) {
            //MessageBox.Show(String.Join(" ", args));
            IntPtr previewHwnd = IntPtr.Zero;
            IntPtr[] hwnds;
            Actions action = Actions.Screensaver;
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            hwnds = null;
            if (args.Length > 0) {
                string arg1 = args[0].ToLower().Trim();
                string arg2 = null;
                if (arg1.Length > 2) {
                    arg2 = arg1.Substring(3).Trim();
                    arg1 = arg1.Substring(0, 2);
                } else if (args.Length > 1) {
                    arg2 = args[1];
                }
                switch (arg1[1]) {
                    case 'c':
                        action = Actions.Config;
                    break;
                    case 't':
                    case 'p':
                        if (arg1[1] == 't') action = Actions.Test;
                        else action = Actions.Preview;
                        //action = Actions.Test;
                        hwnds = new IntPtr[args.Length - 1];
                        for (int i = 1; i < args.Length; i++) {
                            hwnds[i - 1] = new IntPtr(long.Parse(args[i]));
                        }
                        //previewHwnd = new IntPtr(long.Parse(arg2));
                    break;
                    case 'o':
                        string setting = "0";
                        string value = args[0].Trim("-/\\".ToCharArray());
                        if (string.Compare(value, "on", true) == 0) setting = "1";
                        string oldValue = "on";
                        if (Convert.ToString(Registry.GetValue("HKEY_CURRENT_USER\\Control Panel\\Desktop", "ScreenSaveActive", null)) == "0") oldValue = "off";
                        Registry.SetValue("HKEY_CURRENT_USER\\Control Panel\\Desktop", "ScreenSaveActive", setting);
                        MessageBox.Show("Your screensaver has been truned " + value + "." + Environment.NewLine + "(It was " + oldValue + ")", "Random Photo Screensaver", MessageBoxButtons.OK, MessageBoxIcon.Information );
                        Application.Exit();
                        return;
                    break;
                    case 'w':
                        action = Actions.Wallpaper;
                    break;

                }
            }
            bool readOnly = Screensaver.singleProcess(action);

            Screensaver screensaver = new Screensaver(action, readOnly, hwnds);
            AppDomain.CurrentDomain.UnhandledException += new UnhandledExceptionEventHandler(screensaver.CleanUpOnException);
            switch (action) {
                case Actions.Config:
                    Application.Run(screensaver);
                break;
                case Actions.Preview:
    //MessageBox.Show(hwnds[0].ToString());
                    screensaver.monitors = new Monitor[hwnds.Length];
                    screensaver.monitors[0] = new Monitor(hwnds[0], 0, screensaver);
                    screensaver.monitors[0].FormClosed += new FormClosedEventHandler(screensaver.OnFormClosed);
                    screensaver.monitors[0].PreviewKeyDown += new System.Windows.Forms.PreviewKeyDownEventHandler(screensaver.PreviewKeyDown);
                    screensaver.monitors[0].browser.PreviewKeyDown += new System.Windows.Forms.PreviewKeyDownEventHandler(screensaver.PreviewKeyDown);
                    Application.Run(screensaver.monitors[0]);
                break;
                default:
                    Cursor.Hide();
                    Application.AddMessageFilter(new MouseMessageFilter());
                    MouseMessageFilter.MouseMove += new MouseEventHandler(screensaver.MouseMove);
                    MouseMessageFilter.MouseClick += new MouseEventHandler(screensaver.MouseClick);

                    Application.Run(screensaver);
                break;
            }
        }
    }
}
