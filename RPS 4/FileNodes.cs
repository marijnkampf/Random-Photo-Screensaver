using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Collections.Concurrent;
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

        public List<System.IO.FileSystemWatcher> watchers = new List<FileSystemWatcher>();

        //AutoResetEvent cancelCompleteEvent = new AutoResetEvent(false);

        bool restartBackgroundWorker = false;
        bool onExit = false;
        FileDatabase fileDatabase;
        //FileDatabase metaDatabase;

        ExifTool.Wrapper exifToolWorker;          // Used in background worker
        ExifTool.Wrapper exifToolMain;  // Used in foreground

        Config config;
        Screensaver screensaver;

        int nrFolders = 0;
        int nrFiles = 0;
        //List<string> folders;
        ConcurrentQueue<string> folders = new ConcurrentQueue<string>();
        //ConcurrentQueue<string> files = new ConcurrentQueue<string>();

        string allowedExtensions;
        bool ignoreHiddenFiles;
        bool ignoreHiddenFolders;
        bool excludeSubfolders;
        List<string> excludedSubfolders;
        string rawExtensions = null;

        int nrUnprocessedMetadata = -1;

        System.Diagnostics.Stopwatch swFileScan;
        System.Diagnostics.Stopwatch swMetadata;

        public long currentSequentialSeedId = -1;

        object bwSender;
        DoWorkEventArgs bwEvents;

        public FileNodes(Config config, Screensaver screensaver) {
            this.config = config;
            this.screensaver = screensaver;
            this.fileDatabase = new FileDatabase(this.screensaver.readOnly);
            //this.fileDatabase.MetadataReadEvent += new MetadataReadEventHandler(metadataShow);

            if (screensaver.action != Screensaver.Actions.Wallpaper) {

                this.resetFoldersQueue();
                
                this.backgroundWorker = new System.ComponentModel.BackgroundWorker();
                this.backgroundWorker.WorkerReportsProgress = true;
                this.backgroundWorker.WorkerSupportsCancellation = true;
                this.backgroundWorker.DoWork += new DoWorkEventHandler(DoWorkImageFolder);
                this.backgroundWorker.ProgressChanged += new ProgressChangedEventHandler(progressChanged);
                this.backgroundWorker.RunWorkerCompleted += new RunWorkerCompletedEventHandler(runWorkerCompleted);

                // Use local folders as this.folders is used in backgroundWorker
                //var folders = Utils.stringToConcurrentQueue(Convert.ToString(this.config.getPersistant("folders")));             
                // Purge database in main thread rather, to avoid having to run database filter twice
                this.purgeNotMatchingParentFolders(this.folders);

                this.backgroundWorker.RunWorkerAsync();
            }
        }

        public void resetFoldersQueue() {
            this.nrFiles = 0;
            this.nrFolders = 0;
            this.nrUnprocessedMetadata = -1;

            this.watchers.Clear();

            this.folders = Utils.stringToConcurrentQueue(this.config.getPersistantString("folders"));
            //this.screensaver.fileNodes.resetFoldersQueue();
            if (this.screensaver.fileNodes != null) this.screensaver.fileNodes.purgeNotMatchingParentFolders(this.folders);
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

        public void resetFilter() {
            if (this.config.getPersistantBool("useFilter")) {
                this.fileDatabase.resetFilter();
            } else {
                this.fileDatabase.clearFilter();
            }
            this.showProgress();
        }

        public void exifToolWorkerStarted() {
            if (this.exifToolWorker == null) {
                try {
                    this.exifToolWorker = new ExifTool.Wrapper(Convert.ToString(this.config.getPersistant("exifTool")));
                    this.exifToolWorker.Starter();
                } catch (Exception e) {
                    this.screensaver.showInfoOnMonitors(e.Message, true, true);
                }
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
            if (this.backgroundWorker != null) {
                if (this.backgroundWorker.IsBusy) {
                    this.restartBackgroundWorker = true;
                    this.backgroundWorker.CancelAsync();
                } else {
                    this.restartBackgroundWorker = false;
                    this.screensaver.fileNodes.resetFoldersQueue();
                    this.backgroundWorker.RunWorkerAsync();
                }
            }
        }

        private void OnFolderCreated(object source, FileSystemEventArgs e) {
            if (this.folders.Count > 0 && this.backgroundWorker.IsBusy) {
                this.folders.Enqueue(e.FullPath);
            } else {
                this.folders.Enqueue(e.FullPath);
                this.showProgress();
            }
        }

        private void OnFolderDeleted(object source, FileSystemEventArgs e) {
            this.fileDatabase.purgeMatchingParentPaths(Utils.addTrailingSlash(e.FullPath));
            this.showProgress();
        }

        private void OnFolderRenamed(object source, RenamedEventArgs e) {
            // TODO Fix rename in database
            this.fileDatabase.renameFolderPaths(Utils.addTrailingSlash(e.OldFullPath), Utils.addTrailingSlash(e.FullPath));

            Debug.WriteLine("Rename folder: " + e.FullPath + " " + e.ChangeType);
            // TODO
        }

        private void OnFileCreated(object source, FileSystemEventArgs e) {
            Debug.WriteLine("New file: " + e.FullPath + " " + e.ChangeType);
            this.addImage(e.FullPath);
            this.showProgress();
        }


        private void OnFileDeleted(object source, FileSystemEventArgs e) {
            this.deleteFromDB(e.FullPath);
            this.showProgress();
        }

        private void OnFileRenamed(object source, RenamedEventArgs e) {
            this.screensaver.fileNodes.updatePath(fileDatabase.getIdFromPath(e.OldFullPath), e.FullPath);
        }

        private void addImage(string filename) {
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
                try {
                    this.fileDatabase.addFileToDB(fi);
                } catch (Exception e) {
                    this.screensaver.showInfoOnMonitors("Error: " + e.Message, true);
                }
            }
        }

        private void processFolders() {
            try {
                this.allowedExtensions = this.config.getPersistantString("imageExtensions").ToLower() + " " + this.config.getPersistantString("videoExtensions").ToLower();
                this.ignoreHiddenFiles = this.config.getPersistantBool("ignoreHiddenFiles");
                this.ignoreHiddenFolders = this.config.getPersistantBool("ignoreHiddenFolders");
                this.excludedSubfolders = Utils.stringToList(Convert.ToString(this.config.getPersistant("excludedSubfolders")).ToLower());
                this.excludeSubfolders = this.config.getPersistantBool("excludeAllSubfolders");

                if (this.config.getPersistantBool("rawUseConverter")) {
                    this.rawExtensions = Convert.ToString(this.config.getPersistant("rawExtensions")).ToLower();
                    this.allowedExtensions += " " + rawExtensions;
                }            
            } catch (Exception e) {
                Debug.WriteLine("processFolders #1" + e.Message);
                // TODO: only catch:
                // System.NullReferenceException
                // System.Runtime.InteropServices.InvalidComObjectException icoe
                // Occurs when shutting down, cancel thread
                this.bwCancelled();
                return;
            }
           
            while (this.folders.Count > 0) {
                //Debug.WriteLine(folders[0]);
                if (this.bwCancelled() == true) return;

                //this.folders[0] = this.folders[0].Trim();

                // Get first value from folders queue.
                string folder;
                if (!this.folders.TryPeek(out folder)) {
                    Debug.WriteLine("folders queue trypeek failed when it should have succeeded.");
                } else {
                    if (!Directory.Exists(folder)) {
                        this.fileDatabase.purgeMatchingParentPaths(folder);
                    } else {
                        this.nrFolders++;
                        //HtmlElement he;

                        //                    try {
                        if (this.screensaver.action != Screensaver.Actions.Preview && this.config.getPersistantBool("watchFolders")) {
                            System.IO.FileSystemWatcher fileWatcher = new System.IO.FileSystemWatcher();
                            //watcher.SynchronizingObject = this;
                            fileWatcher.NotifyFilter = NotifyFilters.FileName;
                            fileWatcher.Created += new FileSystemEventHandler(OnFileCreated);
                            fileWatcher.Deleted += new FileSystemEventHandler(OnFileDeleted);
                            fileWatcher.Renamed += new RenamedEventHandler(OnFileRenamed);
                            fileWatcher.Path = folder;
                            fileWatcher.EnableRaisingEvents = true;
                            fileWatcher.IncludeSubdirectories = true;
                            this.watchers.Add(fileWatcher);

                            System.IO.FileSystemWatcher folderWatcher = new System.IO.FileSystemWatcher();
                            //watcher.SynchronizingObject = this;
                            folderWatcher.NotifyFilter = NotifyFilters.DirectoryName;
                            folderWatcher.Created += new FileSystemEventHandler(OnFolderCreated);
                            folderWatcher.Deleted += new FileSystemEventHandler(OnFolderDeleted);
                            folderWatcher.Renamed += new RenamedEventHandler(OnFolderRenamed);
                            folderWatcher.Path = folder;
                            folderWatcher.EnableRaisingEvents = true;
                            folderWatcher.IncludeSubdirectories = true;

                            this.watchers.Add(folderWatcher);
                        }
                        /*                  } catch (Exception e) {
                                              Debug.WriteLine("processFolders #2" + e.Message);
                                              // TODO: only catch:
                                              // System.NullReferenceException
                                              // System.Runtime.InteropServices.InvalidComObjectException icoe
                                              // Occurs when shutting down, cancel thread
                                              this.bwCancelled();
                                              return;
                                          }*/

                        string[] filenames = new string[] { };
                        try {
                            filenames = Directory.GetFiles(folder);
                        } catch (System.UnauthorizedAccessException uae) {
                            // Remove from folders queue
                            this.folders.TryDequeue(out folder);
                        }
                        var i = 0;
                        foreach (string filename in filenames) {
                            i++;
                            this.addImage(filename);
                            if ((i % 10 == 0) && (this.bwCancelled() == true)) break;
                        }
                        if (!excludeSubfolders) {
                            string[] subfolders = new string[] { };
                            try {
                                subfolders = Directory.GetDirectories(folder);
                            } catch (System.UnauthorizedAccessException uae) { }
                            i = 0;
                            foreach (string subfolder in subfolders) {
                                FileAttributes fa = File.GetAttributes(subfolder);
                                // Ignore hidden folders if option checked
                                if (!ignoreHiddenFolders || (ignoreHiddenFolders && (fa & FileAttributes.Hidden) != FileAttributes.Hidden)) {
                                    if (excludedSubfolders.IndexOf(Path.GetFileName(subfolder.ToLower())) == -1) {
                                        i++;
                                        this.folders.Enqueue(subfolder);
                                        //this.folders.Add(subfolder);
                                    }
                                } else {
                                    this.fileDatabase.purgeMatchingParentPaths(subfolder);
                                }
                                if ((i % 100 == 0) && (this.bwCancelled() == true)) break;
                            }
                        }
                    }
                }
                // Remove from folders queue
                this.folders.TryDequeue(out folder);
            }
            this.fileDatabase.resetIfChangedFilter();
        }

        public string exifToolCommand(string command) {
            this.exifToolMainStarted();
            return this.exifToolMain.SendCommand(command);
        }

        public string exifToolGetMetadata(string command, long imageId) {
            this.exifToolMainStarted();
            string metadata = this.exifToolMain.SendCommand(command);
            // Hangs screensaver when database is locked
            //if (imageId > 0 && metadata != null) this.fileDatabase.addMetadataToDB(imageId, metadata);
            return metadata;
        }

        public bool addMetadataToDB(long imageId, string meta) {
            return this.fileDatabase.addMetadataToDB(imageId, meta);
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
                if (this.exifToolWorker != null) {
                    meta = this.exifToolWorker.SendCommand(Convert.ToString(dr["path"]) + Constants.ExifToolMetadataOptions);
                    if (this.bwCancelled() == true) return;
                    this.fileDatabase.addMetadataToDB(Convert.ToInt32(dr["id"]), meta);
                }
                if (!this.fileDatabase.readOnly) {
                    this.nrUnprocessedMetadata--;
                    if (this.bwCancelled() == true) return;
                    dr = this.fileDatabase.nextMetadataLessImage();
                    i++;
                } else {
                    this.screensaver.showInfoOnMonitors("Database locked, read only");
                    dr = null;
                }
            }
            this.fileDatabase.toggleMetadataTransaction(this.bwCancelled());
            this.fileDatabase.resetIfChangedFilter();
        }


        public void debugMonitorInfo(int m, SortOrder d, int o, DataRow dr, string s) {
            if (dr == null) this.screensaver.monitors[m].showInfoOnMonitor("getSequentialImage(monitor " + m + ", direction " + d.ToString() + ", offset "+o+") ["+s+"]: null");
            else this.screensaver.monitors[m].showInfoOnMonitor("getSequentialImage(monitor " + m + ", direction " + d.ToString() + ", offset " + o + ") [" + s + "]: " + dr["id"]);
        }

        public bool cacheRawImage(string rawSource, string jpgDest, bool hideFolder, bool hideFile) {
            if (File.Exists(jpgDest)) return true;

            if (!File.Exists(Convert.ToString(this.config.getPersistant("rawConverter")))) {
                throw new FileNotFoundException("Raw converter: '" + this.config.getPersistant("rawConverter") + "' not found.");
                return false;
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
            return true;
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
                    default:
                        cachedFilename = Path.ChangeExtension(filename, Constants.rawFileConvertedExt);
                        hideFile = true;
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

        public int getNrImagesFilter() {
            return this.screensaver.fileNodes.fileDatabase.nrImagesFilter();
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

/*        public DataRow getSequentialImageFromId(int monitor, int offset) {

        }*/

        public DataRow getSequentialImage(int offset) {
            return this.getSequentialImage(offset, false);
        }

        public DataRow getSequentialImage(int offset, bool readOnly) {
            return this.getSequentialImage(this.currentSequentialSeedId, offset, readOnly);
        }

        public DataRow getSequentialImage(long seedId, int offset) {
            return this.getSequentialImage(seedId, offset, false);
        }

        public DataRow getSequentialImage(long seedId, int offset, bool readOnly) {
            DataRow currentImage = null;

            string sortByColumn = this.screensaver.config.getPersistantString("sortBy");
            SortOrder sortDirection = new SortOrder(this.screensaver.config.getPersistantString("sortDirection"));

            if (seedId == -1) {
                try {
                    long imageId;
                    imageId = Convert.ToInt32(this.config.getPersistant("sequentialStartImageId"));
                    currentImage = this.fileDatabase.getImageById(imageId, (this.screensaver.monitors.Length - 1) * -1, sortByColumn, sortDirection);
                } catch (Exception e) {
                    Debug.WriteLine("getSequentialImage " + e.Message); 
                    currentImage = this.fileDatabase.getFirstImage(sortByColumn, sortDirection);
                }
            } else {
                currentImage = fileDatabase.getImageById(seedId, offset, sortByColumn, sortDirection);
            }
            if (!readOnly && currentImage != null) {
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

        public DataTable runSQLByPassFilter(string subQuery) {
            return this.fileDatabase.runSQLByPassFilter(subQuery);
        }

        public int deleteFromDB(string path) {
            return this.fileDatabase.deleteFromDB(path);
        }

        public int deleteFromDB(long id) {
            return this.fileDatabase.deleteFromDB(id);
        }

        public int purgeNotMatchingParentFolders(string folders) {
            return this.purgeNotMatchingParentFolders(Utils.stringToConcurrentQueue(folders));
        }

        public int purgeNotMatchingParentFolders(ConcurrentQueue<string> folders) {
            return this.fileDatabase.purgeNotMatchingParentFolders(folders, this.screensaver.config.getPersistantBool("excludeAllSubfolders"), Utils.stringToList(this.screensaver.config.getPersistantString("excludedSubfolders")));
        }

        public int purgeMediaDatabase() {
            return this.fileDatabase.purgeMediaDatabase();
        }

        public void updatePath(long id, string path) {
            this.fileDatabase.updateFileNodesPath(id, path);
        }
        
/*
        public void addIdToMetadataQueue(long monitorId, DataRow image) {
            this.fileDatabase.addIdToMetadataQueue(monitorId, image);
        }
        */
        private void DoWorkImageFolder(object sender, DoWorkEventArgs e) {
//            Debug.WriteLine(this.config.getPersistant("folders"));
            BackgroundWorker worker = sender as BackgroundWorker;
            // Lower priority to ensure smooth working of main screensaver
            System.Diagnostics.Process.GetCurrentProcess().PriorityClass = System.Diagnostics.ProcessPriorityClass.BelowNormal;
            this.bwSender = sender;
            this.bwEvents = e;
            if (!this.screensaver.readOnly) {
                this.swFileScan = new System.Diagnostics.Stopwatch();
                this.swMetadata = new System.Diagnostics.Stopwatch();
                // Folder purge done in main thread
                this.swFileScan.Start();
                this.processFolders();
                this.swFileScan.Stop();
                Debug.WriteLine("FileScan: " + swFileScan.ElapsedMilliseconds + "ms");
                this.swMetadata.Start();
                this.processMetadata();
                this.swMetadata.Stop();
                Debug.WriteLine("Metadata: " + swMetadata.ElapsedMilliseconds + "ms");
            }

            if (!this.backgroundWorker.CancellationPending) this.fileDatabase.purgeMetadata();
            //if (Convert.ToDateTime(this.config.setValue("wallpaperLastChange")).Equals(DateTime.Today));
            if (!this.backgroundWorker.CancellationPending) {

                Wallpaper wallpaper = new Wallpaper(this.screensaver);
                if (wallpaper.changeWallpaper()) {
                    switch (this.config.getPersistantString("wallpaperSource")) {
                        case "on": // Backwards compatability old settings
                        case "current":
                            wallpaper.generateWallpaper();
                        break;
                        case "filter":
                            string sql = this.config.getPersistantString("wallpaperSourceFilterSQL");
                            if (sql != "") wallpaper.generateWallpaperFromSQL(sql);
                        break;
                    }
                }
            }

            //this.cancelCompleteEvent.Set();

/*
            var command = new SQLiteCommand(conn);
            command.CommandText = @"SELECT COUNT(id) FROM `FileNodes`;";
            //SQLiteDataReader reader = command.ExecuteReader();
            //while (reader.Read()) {
            Debug.WriteLine("Rows in DB: " + Convert.ToInt32(command.ExecuteScalar()));
            */
        }

        private string showProgress() {
            String info = "";// = "No files found in folder(s) - Press 'S' key to show configuration screen";
            long nrImagesFiltered = this.fileDatabase.nrImagesFilter();
            long nrImagesInDb = this.fileDatabase.nrImagesInDB();
            if (nrImagesFiltered > 0 || nrImagesInDb > 0) {
                info += String.Format("DB {0:##,#} images; ", nrImagesInDb);
            }
            if (this.screensaver.config.getPersistantBool("useFilter")) {
                if (nrImagesFiltered == 0) info += " filter: no images found; ";
                else info += String.Format("filtered {0:##,#} images; ", nrImagesFiltered);
            }
            if (this.nrUnprocessedMetadata > 0) {
                info += String.Format(" Metadata queue {0:##,#} files", this.nrUnprocessedMetadata);
            } else {
                if (this.nrFiles > 0 || this.nrFolders > 0) {
                    info += String.Format("Scanned {0:##,#} files in {1:##,#} folders", this.nrFiles, this.nrFolders);
                }
            }
            if (this.screensaver.readOnly) {
                info += " Read Only - RPS is already running";
            }
            try {
                for (int i = 0; i < this.screensaver.monitors.Length; i++) {

                    if (this.screensaver.monitors[i].InvokeRequired) {
                        MethodInvoker del = delegate { this.screensaver.monitors[i].InvokeScript("showIndexProgress", new String[] { info }); };
                        this.screensaver.monitors[i].Invoke(del);

//                        this.screensaver.monitors[i].browser.Invoke(new MethodInvoker(() => this.InvokeScript("showIndexProgress", new String[] { info })));
//                        this.screensaver.monitors[i].InvokeScript("showIndexProgress", new String[] { info });
                    } else {
                        this.screensaver.monitors[i].InvokeScript("showIndexProgress", new String[] { info });
                    }
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
                    this.screensaver.fileNodes.resetFoldersQueue();
                    this.backgroundWorker.RunWorkerAsync();
                    Debug.WriteLine("BackgroundWorker Restarted");
                } else {
                    Debug.WriteLine("BackgroundWorker Canceled!");
                }
            } else if (!(e.Error == null)) {
                Debug.WriteLine("BackgroundWorker Error: " + e.Error.Message);
            } else {
                if (this.screensaver.fileNodes.fileDatabase.nrImagesFilter() == 0) {
                    this.screensaver.showInfoOnMonitors(Monitor.NoImagesFoundMessage(this.screensaver.readOnly), true);
                }
                this.showProgress();
                //if (this.screensaver.monitors != null) this.screensaver.monitors[0].browser.Document.InvokeScript("dbInfo", new String[] { this.getProgress() });
                //Debug.WriteLine("BackgroundWork done!");
            }
        }

        public void OnExitCleanUp() {
            if (!this.onExit) {
                this.onExit = true;
                this.CancelBackgroundWorker();
                //this.cancelCompleteEvent.WaitOne();
                this.fileDatabase.storePersistant();
                this.fileDatabase.CloseConnections();
            }
        }

    }
}
