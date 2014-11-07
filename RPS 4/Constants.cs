using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Windows.Forms;
using System.Drawing;

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

        public const string ExifToolMetadataOptions = "\n-All\n-Orientation#";

        public enum Orientation {
            Horizontal = 1,
            Mirror_Horizontal = 2,
            Rotate_180 = 3,
            Mirror_Vertical = 4,
            Mirror_Horizontal_Rotate_270_CW = 5,
            Rotate_90_CW = 6,
            Mirror_Horizontal_Rotate_90_CW = 7,
            Rotate_270_CW = 8
        }

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

        public static Rectangle getDesktopBounds() {
            Rectangle r = Screen.AllScreens[0].Bounds;
            for (int i = 1; i < Screen.AllScreens.Length; i++) {
                r = Rectangle.Union(r, Screen.AllScreens[i].Bounds);
            }
            return r;
        }

        public static Rectangle FitIntoBounds(Rectangle image, Rectangle boundingBox, bool stretchSmall) {
            Rectangle r = boundingBox;
            double rw, rh;
            rw = (double)boundingBox.Width / (double)image.Width;
            rh = (double)boundingBox.Height / (double)image.Height;
            if (!stretchSmall && rw > 1 && rh > 1) {
                r.Width = image.Width;
                r.Height = image.Height;
            } else {
                if (rw < rh) {
                    r.Width = (int)Math.Round((double)image.Width * rw);
                    r.Height = (int)Math.Round((double)image.Height * rw);
                } else {
                    r.Width = (int)Math.Round((double)image.Width * rh);
                    r.Height = (int)Math.Round((double)image.Height * rh);
                }
            }
            r.X = boundingBox.X + (int)(boundingBox.Width - r.Width) / 2;
            r.Y = boundingBox.Y + (int)(boundingBox.Height - r.Height) / 2;
            return r;
        }



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

/*         	
1 = Horizontal (normal)
2 = Mirror horizontal
3 = Rotate 180
4 = Mirror vertical
5 = Mirror horizontal and rotate 270 CW
6 = Rotate 90 CW
7 = Mirror horizontal and rotate 90 CW
8 = Rotate 270 CW
    */
    }
}
