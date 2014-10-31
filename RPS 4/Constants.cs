using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Windows.Forms;

namespace RPS {
    static class Constants {
        public const string DataFolder = "data";
        public const string RawCacheFolder = "rawCache.rps";
        public const string devDataFolder = "../../data";
        public const string ConfigHtmlFile = "config.html";
        public const string MonitorHtmlFile = "monitor.html";

        public const int reloadFilters = 500;
        public const string AppFolderName = "Random Photo Screensaver";
        public const string DbFileName = "store.sqlite";
        public const string MetadataFileName = "meta.sqlite";
        public const string PersistantConfigFileName = "settings.sqlite";
        public const string WallpaperFileName = "rpsBackground.bmp";

        public const string rawFileConvertedExt = ".jpg";
        //        public readonly string[] imageExtensions = { ".gif", ".png", ".jpeg", ".jpg", ".pcx", ".bmp" };
  //      public readonly string[] movieExtensions = { ".avi", ".wmv", "mp4" };

        public const string FileDatabaseColumns = @"
	`id`	            INTEGER PRIMARY KEY AUTOINCREMENT,
	`path`	            TEXT UNIQUE,
	`parentpath`	    TEXT,
	`created`	        DATETIME,
	`modified`	        DATETIME,
	`size`	            INTEGER,
	`metainfoindexed`   INTEGER DEFAULT 0
        ";

        public const string FileDatabaseSQL = @"
PRAGMA page_size=4096;
CREATE TABLE `FileNodes` (" + Constants.FileDatabaseColumns + @"
        );";

        public const string FileDatabaseIndexes = @"
CREATE UNIQUE INDEX FileNodes_Path ON FileNodes(path);
CREATE INDEX FileNodes_Created ON FileNodes(created);
CREATE INDEX FileNodes_Modified ON FileNodes(modified);
CREATE INDEX FileNodes_Parentpath ON FileNodes(parentpath);
       ";

        public const string MetadataSQL = @"
PRAGMA page_size=4096;
CREATE TABLE `Metadata` (
	`id`	INTEGER NOT NULL,
	`all`	TEXT,
	PRIMARY KEY(id)
);
        ";

        public const string SettingsDatabaseSQL = @"
PRAGMA page_size=4096;
CREATE TABLE `Setting` (
	`id`	INTEGER PRIMARY KEY AUTOINCREMENT,
	`key`	TEXT UNIQUE,
	`value`	TEXT
);

CREATE UNIQUE INDEX `keys` ON `Setting` (`key` ASC);
        ";

        /**
         * path: should contain full path and filename
         **/
        public static bool requireFile(string path) {
            return Constants.requireFile(path, true);
        }

        public static bool requireFile(string path, bool exitOnNotFound) {
            if (!File.Exists(path)) {
                string filename = Path.GetFileName(path);
                MessageBox.Show("I tried looking in:\r\n" + path, "'" + filename + "' not found");
                if (exitOnNotFound) {
                    Environment.Exit(0);
                }
                return false;
            }
            return true;
        }

        public static string getDataFolder(string filename) {
            /* 1. user folder (C:\Users\[user name]\AppData\Local\Random Photo Screensaver)
             * 2. executable folder
             * 3. development folder */

            /* 1 */
            string notFound = "";
            string fullPath = Path.Combine(
                Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), 
                Constants.AppFolderName,
                Constants.DataFolder,
                filename
            );
            
            /* 2 */
            if (!File.Exists(fullPath)) {
                notFound += fullPath + "\r\n";
                //MessageBox.Show(fullPath + " not found");
                fullPath = Path.Combine(
                    Application.StartupPath,
                    Constants.DataFolder,
                    filename
                );
                /* 3 */
                if (!File.Exists(fullPath)) {
                    notFound += fullPath + "\r\n";
                    //MessageBox.Show(fullPath + " not found");
                    fullPath = Path.Combine(
                        Application.StartupPath,
                        Constants.devDataFolder,
                        filename
                    );
                    requireFile(fullPath);
/*                    if (!File.Exists(fullPath)) {
                        notFound += fullPath + "\r\n";
                        Constants.requiredFileMissing(filename, notFound);
                    }*/
                }
            }
            return fullPath;
        }
    }
}
