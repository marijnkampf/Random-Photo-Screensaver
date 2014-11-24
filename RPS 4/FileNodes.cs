using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Data.SQLite;
using System.ComponentModel;
using System.IO;
using System.Windows.Forms;
using System.Data;
using System.Diagnostics;
using System.Collections;

namespace RPS {
    public class FileNodes {
        System.ComponentModel.BackgroundWorker backgroundWorker;
        AutoResetEvent cancelCompleteEvent = new AutoResetEvent(false);

        bool restartBackgroundWorker = false;
        FileDatabase fileDatabase;
        //FileDatabase metaDatabase;

        ExifTool.Wrapper exifToolWorker;          // Used in background worker
        ExifTool.Wrapper exifToolMain;  // Used in foreground

        Config config;
        Screensaver screensaver;

        int nrFolders = 0;
        int nrFiles = 0;

        int nrUnprocessedMetadata = -1;

        System.Diagnostics.Stopwatch swFileScan;
        System.Diagnostics.Stopwatch swMetadata;

        public long currentSequentialSeedId = -1;

        object bwSender;
        DoWorkEventArgs bwEvents;

        public FileNodes(Config config, Screensaver screensaver) {
            this.config = config;
            this.screensaver = screensaver;
            this.fileDatabase = new FileDatabase();
            //this.fileDatabase.MetadataReadEvent += new MetadataReadEventHandler(metadataShow);

            this.backgroundWorker = new System.ComponentModel.BackgroundWorker();
            this.backgroundWorker.WorkerReportsProgress = true;
            this.backgroundWorker.WorkerSupportsCancellation = true;
            this.backgroundWorker.DoWork += new DoWorkEventHandler(DoWorkImageFolder);
            this.backgroundWorker.ProgressChanged += new ProgressChangedEventHandler(progressChanged);
            this.backgroundWorker.RunWorkerCompleted += new RunWorkerCompletedEventHandler(runWorkerCompleted);

            this.backgroundWorker.RunWorkerAsync();
        }
        /*
        void metadataShow(ImageMetadataStatus imageMetadata) {
            this.screensaver.monitors[imageMetadata.monitorId].showMetadataOnMonitor(imageMetadata.metadata);
            //this.Invoke()
            //this.MetadataReadEvent(imageMetadata);
            //            this.screensaver.monitors[imageMetadata.monitorId].showMetadataOnMonitor(imageMetadata.metadata);
            //this.screensaver.monitors[imageMetadata.monitorId].showImage(false);
        }*/

        public void setFilterSQL(string sql) {
            this.config.setPersistant("useFilter", true);
            this.fileDatabase.setFilterSQL(sql);
            this.showProgress();
        }

        public void clearFilter() {
            this.config.setPersistant("useFilter", false);
            this.fileDatabase.clearFilter();
            this.showProgress();
        }

        public void exifToolWorkerStarted() {
            if (this.exifToolWorker == null) {
                this.exifToolWorker = new ExifTool.Wrapper(Convert.ToString(this.config.getPersistant("exifTool")));
                this.exifToolWorker.Starter();
            }
        }

        public void exifToolMainStarted() {
            if (this.exifToolMain == null) {
                this.exifToolMain = new ExifTool.Wrapper(Convert.ToString(this.config.getPersistant("exifTool")));
                this.exifToolMain.Starter();
            }
        }

        public void CancelBackgroundWorker() {
            this.backgroundWorker.CancelAsync();
        }

        private bool bwCancelled() {
            if ((backgroundWorker.CancellationPending == true)) {
                this.bwEvents.Cancel = true;
                return true;
            } else {
                BackgroundWorker worker = this.bwSender as BackgroundWorker;
                try {
                    worker.ReportProgress(10);
                } catch (System.InvalidOperationException ioe) {
                    // Thrown when out of sync on application exit
                }
                return false;
            }
        }

        public void restartBackgroundWorkerImageFolder() {
            if (this.backgroundWorker.IsBusy) {
                this.restartBackgroundWorker = true;
                this.backgroundWorker.CancelAsync();
            } else {
                this.restartBackgroundWorker = false;
                this.backgroundWorker.RunWorkerAsync();
            }
        }

        private void processFolders(List<string> folders) {
            while (folders.Count > 0) {
                //Debug.WriteLine(folders[0]);
                if (this.bwCancelled() == true) return;

                folders[0] = folders[0].Trim();

                if (Directory.Exists(folders[0])) {
                    this.nrFolders++;
                    //HtmlElement he;
                    string allowedExtensions;
                    bool ignoreHiddenFiles;
                    bool ignoreHiddenFolders;
                    bool excludeSubfolders;
                    string rawExtensions = null;

                    try {
                        //he = this.config.getElementById("imageExtensions");
                        allowedExtensions = this.config.getPersistantString("imageExtensions").ToLower() + " " + this.config.getPersistantString("videoExtensions").ToLower();
                        ignoreHiddenFiles = this.config.getPersistantBool("ignoreHiddenFiles");
                        ignoreHiddenFolders = this.config.getPersistantBool("ignoreHiddenFolders");
                        excludeSubfolders = this.config.getPersistantBool("excludeAllSubfolders");

                        if (this.config.getPersistantBool("rawUseConverter")) {
                            rawExtensions = Convert.ToString(this.config.getPersistant("rawExtensions")).ToLower();
                            allowedExtensions += " " + rawExtensions;
                        }
                    } catch (Exception e) {
                        Debug.WriteLine("processFolders " + e.Message);
                        // TODO: only catch:
                        // System.NullReferenceException
                        // System.Runtime.InteropServices.InvalidComObjectException icoe
                        // Occurs when shutting down, cancel thread
                        this.bwCancelled();
                        return;
                    }

                    string[] filenames = new string[] { };
                    try {
                        filenames = Directory.GetFiles(folders[0]);
                    } catch (System.UnauthorizedAccessException uae) {
                        folders.RemoveAt(0);
                    }
                    var i = 0;
                    foreach (string filename in filenames) {
                        i++;
                        FileInfo fi = new FileInfo(filename);
                        if (
                            // Image has to have extension
                            (fi.Extension.Length > 0) &&
                            // Check allowed file extensions
                            (allowedExtensions.IndexOf(fi.Extension.ToLower()) > -1) &&
                            // Ignore hidden files if option checked
                            (!ignoreHiddenFiles || (ignoreHiddenFiles && (fi.Attributes & FileAttributes.Hidden) != FileAttributes.Hidden))
                        ) {
                            this.nrFiles++;
                            if ((i % 10 == 0) && (this.bwCancelled() == true)) break;
                            try {
                                this.fileDatabase.addFileToDB(fi);
                            } catch (Exception e) {
                                this.screensaver.showInfoOnMonitors("Error: " + e.Message, true);
                            }
                        }
                    }
                    if (!excludeSubfolders) {
                        string[] subfolders = new string[] { };
                        try {
                            subfolders = Directory.GetDirectories(folders[0]);
                        } catch (System.UnauthorizedAccessException uae) { }
                        i = 0;
                        foreach (string subfolder in subfolders) {
                            FileAttributes fa = File.GetAttributes(subfolder);
                            // Ignore hidden folders if option checked
                            if (!ignoreHiddenFolders || (ignoreHiddenFolders && (fa & FileAttributes.Hidden) != FileAttributes.Hidden)) {
                                i++;
                                folders.Add(subfolder);
                            }
                            if ((i % 100 == 0) && (this.bwCancelled() == true)) break;
                        }
                    }
                }
                folders.RemoveAt(0);
            }
        }

        public string exifToolCommand(string command) {
            this.exifToolMainStarted();
            return this.exifToolMain.SendCommand(command);
        }

        public string exifToolGetMetadata(string command, long imageId) {
            this.exifToolMainStarted();
            string metadata = this.exifToolMain.SendCommand(command);
            if (imageId > 0 && metadata != null) this.fileDatabase.addMetadataToDB(imageId, metadata);
            return metadata;
        }

        public void processMetadata() {
            this.nrUnprocessedMetadata = this.fileDatabase.nrMetadataImagesToProcess();
            DataRow dr;
            dr = this.fileDatabase.nextMetadataLessImage();
            int i = 0;
            string meta = null;
            while (dr != null) {
                if (this.bwCancelled() == true) return;
                if (i % 50 == 0) this.fileDatabase.toggleMetadataTransaction(this.bwCancelled());
                this.exifToolWorkerStarted();
                meta = this.exifToolWorker.SendCommand(Convert.ToString(dr["path"]) + Constants.ExifToolMetadataOptions);
                /*              // Alternative using exiv2, slightly (10% - 15%) quicker but output needs more processing (not tab deliminated)
                                Process proc = new Process {
                                    StartInfo = new ProcessStartInfo {
                                        FileName = @"D:\programming\vc#\RPS 4\RPS 4\vendor\exiv2_32.exe",
                                        Arguments = "-PEIXkvt \"" + Convert.ToString(dr["path"]) + "\"",
                                        UseShellExecute = false,
                                        RedirectStandardOutput = true,
                                        CreateNoWindow = true
                                    }
                                };
                                proc.Start();
                                string meta = proc.StandardOutput.ReadToEnd();
                                proc.WaitForExit();
                */
                if (this.bwCancelled() == true) return;
                this.fileDatabase.addMetadataToDB(Convert.ToInt32(dr["id"]), meta);
                this.nrUnprocessedMetadata--;

                if (this.bwCancelled() == true) return;

                dr = this.fileDatabase.nextMetadataLessImage();
                i++;
            }
            this.fileDatabase.toggleMetadataTransaction(this.bwCancelled());
        }


        public void debugMonitorInfo(int m, SortOrder d, int o, DataRow dr, string s) {
            if (dr == null) this.screensaver.monitors[m].showInfoOnMonitor("getSequentialImage(monitor " + m + ", direction " + d.ToString() + ", offset "+o+") ["+s+"]: null");
            else this.screensaver.monitors[m].showInfoOnMonitor("getSequentialImage(monitor " + m + ", direction " + d.ToString() + ", offset " + o + ") [" + s + "]: " + dr["id"]);
        }

        public bool cacheRawImage(string rawSource, string jpgDest, bool hideFolder, bool hideFile) {
            if (File.Exists(jpgDest)) return true;

            if (!File.Exists(Convert.ToString(this.config.getPersistant("rawConverter")))) {
                throw new FileNotFoundException("Raw converter: '" + this.config.getPersistant("rawConverter") + "' not found."); 
            }

            if (!Directory.Exists(Path.GetDirectoryName(jpgDest))) {
                Directory.CreateDirectory(Path.GetDirectoryName(jpgDest));
                if (hideFolder) {
                    DirectoryInfo di = new DirectoryInfo(Path.GetDirectoryName(jpgDest));
                    di.Attributes |= FileAttributes.Hidden;
                }
            }

            // Convert raw
            // Hide raw file
            string size = "";
            if (this.config.getPersistantString("rawCacheSize") == "monitor") {
                size = "--size=" + this.config.maxMonitorDimension;// Get screen max dimensions!!!

            }
            System.Threading.Thread.Sleep(250);
            Process p = new Process();
            p.StartInfo.UseShellExecute = false;
            p.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            p.StartInfo.CreateNoWindow = true;
            p.StartInfo.FileName = Convert.ToString(this.config.getPersistant("rawConverter"));
            p.StartInfo.Arguments = size + " " + Convert.ToString(this.config.getPersistant("rawConverterParams")).Replace("#RAW#", rawSource).Replace("#JPG#", jpgDest);
            p.Start();
            p.PriorityClass = ProcessPriorityClass.BelowNormal;
            p.WaitForExit();
            
            if (File.Exists(jpgDest)) {
                if (hideFile) {
                    FileInfo fi = new FileInfo(jpgDest);
                    fi.Attributes |= FileAttributes.Hidden;
                }
            }
            return false;
        }

        public string checkImageCache(string filename, long monitor, ref Hashtable settings) {
            string rawExtensions = Convert.ToString(this.config.getPersistant("rawExtensions"));
            bool rawUseConverter = this.config.getPersistantBool("rawUseConverter");
            //if ((this.config.getPersistant("rawExtensions").IndexOf(Path.GetExtension(filename).ToLower()) > -1) && this.config.getCheckboxValue("rawUseConverter")) {
            if ((rawExtensions.IndexOf(Path.GetExtension(filename).ToLower()) > -1) && rawUseConverter) {
                string cachedFilename = null;
                bool hideFolder = true;
                bool hideFile = false;

                switch (this.config.getPersistantString("rawLocation")) {
                    case "same":
                        cachedFilename = Path.ChangeExtension(filename, Constants.rawFileConvertedExt);
                        hideFile = true;
                    break;
                    case "subfolder":
                        cachedFilename = Path.Combine(
                            Path.GetDirectoryName(filename),
                            Convert.ToString(this.config.getPersistant("rawSubfolder")),
                            Path.GetFileNameWithoutExtension(filename) + Constants.rawFileConvertedExt
                        );
                        hideFile = false;
                    break;
                    case "separate":
                        cachedFilename = Path.Combine(
                            Convert.ToString(this.config.getPersistant("rawFolder")),
                            Path.ChangeExtension(filename, Constants.rawFileConvertedExt).Replace(":", "")
                        );
                        hideFolder = false;
                        hideFile = false;
                    break;
                }
                if (!File.Exists(cachedFilename)) this.cacheRawImage(filename, cachedFilename, hideFolder, hideFile);
                if (settings.ContainsKey("raw")) settings["raw"] = true;
                else settings.Add("raw", true);
                //this.screensaver.monitors[monitor].showInfoOnMonitor("");
                return cachedFilename;
            }
            return filename;
        }
/*
        public DataRow getFirstImage(int monitor) {
            this.
            /*
            string[] imageIds;
            string s = this.config.getPersistant("randomStartImages");
            if (s != null && s.Length > 0) {
                imageIds = s.Split(';');
                long imageId;
                try {
                    imageId = Convert.ToInt32(imageIds[monitor]);
                } catch (Exception e) {
                    imageId = -1;
                }
                return this.fileDatabase.getImageById(imageId, 0);
            }
            return null;
        }*/

        public DataRow getSequentialImage(int monitor, int offset) {
            DataRow currentImage = null;

            string sortByColumn = this.screensaver.config.getPersistantString("sortBy");
            SortOrder sortDirection = new SortOrder(this.screensaver.config.getPersistantString("sortDirection"));

            if (this.currentSequentialSeedId == -1) {
                try {
                    long imageId;
                    imageId = Convert.ToInt32(this.config.getPersistant("sequentialStartImageId"));
                    currentImage = this.fileDatabase.getImageById(imageId, (this.screensaver.monitors.Length - 1) * -1, sortByColumn, sortDirection);
                } catch (Exception e) {
                    Debug.WriteLine("getSequentialImage " + e.Message); 
                    currentImage = this.fileDatabase.getFirstImage(sortByColumn, sortDirection);
                }
            } else {
                currentImage = fileDatabase.getImageById(this.currentSequentialSeedId, offset, sortByColumn, sortDirection);
            }
            if (currentImage != null) {
                this.currentSequentialSeedId = Convert.ToInt32(currentImage["id"]);
            }
            return currentImage;
        }

        public DataRow getImageById(long id, long offset) {
            if (offset == 0) {
                return fileDatabase.getImageById(id, offset);
            } else {
                string sortByColumn = this.screensaver.config.getPersistantString("sortBy");
                SortOrder sortDirection = new SortOrder(this.screensaver.config.getPersistantString("sortDirection"));
                return fileDatabase.getImageById(id, offset, sortByColumn, sortDirection);
            }
        }

        public DataRow getRandomImage() {
            return this.getRandomImage(0);
        }

        public DataRow getRandomImage(long offset) {
            return fileDatabase.getRandomImage(offset);
        }
        
        public string getMetadataById(long id) {
            return this.fileDatabase.getMetadataById(id);
        }

        public int deleteFromDB(string path) {
            return this.fileDatabase.deleteFromDB(path);
        }

        public int deleteFromDB(long id) {
            return this.fileDatabase.deleteFromDB(id);
        }

        public List<string> stringToListFolders(string f) {
            string[] ff = new string[] { };
            if (f != null && f.Length > 0) {
                ff = f.Split(new string[] { ";", System.Environment.NewLine, "\n" }, StringSplitOptions.None);
            }
            return new List<string>(ff);
        }

        public int purgeNotMatchingParentFolders(string folders) {
            return this.purgeNotMatchingParentFolders(stringToListFolders(folders));
        }

        public int purgeNotMatchingParentFolders(List<string> folders) {
            return this.fileDatabase.purgeNotMatchingParentFolders(folders, this.screensaver.config.getPersistantBool("excludeAllSubfolders"));
        }
/*
        public void addIdToMetadataQueue(long monitorId, DataRow image) {
            this.fileDatabase.addIdToMetadataQueue(monitorId, image);
        }
        */
        private void DoWorkImageFolder(object sender, DoWorkEventArgs e) {
            this.nrFiles = 0;
            this.nrFolders = 0;
            this.nrUnprocessedMetadata = -1;

//            Debug.WriteLine(this.config.getPersistant("folders"));
            BackgroundWorker worker = sender as BackgroundWorker;
            // Lower priority to ensure smooth working of main screensaver
            System.Diagnostics.Process.GetCurrentProcess().PriorityClass = System.Diagnostics.ProcessPriorityClass.BelowNormal;
            this.bwSender = sender;
            this.bwEvents = e;
            var folders = stringToListFolders(Convert.ToString(this.config.getPersistant("folders")));
            this.swFileScan = new System.Diagnostics.Stopwatch();
            this.swMetadata = new System.Diagnostics.Stopwatch();
            if (this.purgeNotMatchingParentFolders(folders) > 0) {
                this.clearFilter();
            }
            this.swFileScan.Start();
            this.processFolders(folders);
            this.swFileScan.Stop();
            this.swMetadata.Start();
            this.processMetadata();
            this.swMetadata.Stop();

            if (!this.backgroundWorker.CancellationPending) this.fileDatabase.purgeMetadata();
            //if (Convert.ToDateTime(this.config.setValue("wallpaperLastChange")).Equals(DateTime.Today));
            if (!this.backgroundWorker.CancellationPending) {
                Wallpaper wallpaper = new Wallpaper(this.screensaver);
                if (wallpaper.changeWallpaper()) wallpaper.setWallpaper();
            }

            this.cancelCompleteEvent.Set();

/*
            var command = new SQLiteCommand(conn);
            command.CommandText = @"SELECT COUNT(id) FROM `FileNodes`;";
            //SQLiteDataReader reader = command.ExecuteReader();
            //while (reader.Read()) {
            Debug.WriteLine("Rows in DB: " + Convert.ToInt32(command.ExecuteScalar()));
            */
        }

        private string showProgress() {
            string info = "";// = "No files found in folder(s) - Press 'S' key to show configuration screen";
            long nrImagesFiltered = this.fileDatabase.nrImagesFilter();
            long nrImagesInDb = this.fileDatabase.nrImagesInDB();
            if (nrImagesFiltered > 0 || nrImagesInDb > 0) {
                info += String.Format("DB {0:##,#} images; ", this.fileDatabase.nrImagesInDB());
            }
            if (this.screensaver.config.getPersistantBool("useFilter")) {
                info += String.Format("filtered {0:##,#} images; ", this.fileDatabase.nrImagesFilter());
            }
            if (this.nrUnprocessedMetadata > 0) {
                info += String.Format(" Metadata queue {0:##,#} files", this.nrUnprocessedMetadata);
            } else {
                if (this.nrFiles > 0 || this.nrFolders > 0) {
                    info += String.Format("Scanned {0:##,#} files in {1:##,#} folders", this.nrFiles, this.nrFolders);
                }
            }
            try {
                for (int i = 0; i < this.screensaver.monitors.Length; i++) {
                    this.screensaver.monitors[i].browser.Document.InvokeScript("showIndexProgress", new String[] { info });
                }
                // TODO: Limit exceptions?
            } catch (Exception ex) {
                Debug.WriteLine("progressChanged " + ex.Message);
                //NullReferenceException
                // System.ObjectDisposedException: thrown on change screen saver preview

            }
            return info;
        }

        private void progressChanged(object sender, ProgressChangedEventArgs e) {
            this.showProgress();
        }

        private void runWorkerCompleted(object sender, RunWorkerCompletedEventArgs e) {
            if ((e.Cancelled == true)) {
                if (this.restartBackgroundWorker) {
                    this.restartBackgroundWorker = false;
                    this.backgroundWorker.RunWorkerAsync();
                    Debug.WriteLine("BackgroundWorker Restarted");
                } else {
                    Debug.WriteLine("BackgroundWorker Canceled!");
                }
            } else if (!(e.Error == null)) {
                Debug.WriteLine("BackgroundWorker Error: " + e.Error.Message);
            } else {
                if (this.screensaver.fileNodes.fileDatabase.nrImagesFilter() == 0) {
                    this.screensaver.showInfoOnMonitors("No images found in folder(s)\n\ror filter didn't return any results.\n\rPress 'S' key to enter setup", true);
                }
                this.showProgress();
                //if (this.screensaver.monitors != null) this.screensaver.monitors[0].browser.Document.InvokeScript("dbInfo", new String[] { this.getProgress() });
                //Debug.WriteLine("BackgroundWork done!");
            }
        }

        public void OnExitCleanUp() {
            this.CancelBackgroundWorker();
            this.cancelCompleteEvent.WaitOne();
            this.fileDatabase.storePersistant();
            this.fileDatabase.CloseConnections();
        }
    }
}
