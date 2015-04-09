using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Windows.Forms;
using System.Drawing;
using System.Data.Linq;

namespace RPS {
    static class Constants {
        public const int DebugFileLogSize = 10;
        public const string DataFolder = "data";
        public const string RawCacheFolder = "rawCache.rps";
        public const string devDataFolder = "../../data";
        public const string ConfigHtmlFile = "config.html";
        public const string MonitorHtmlFile = "monitor.html";
        public const string EffectsJsonFile = "js/effects.json";
        
        public const int reloadFilters = 500;
        public const string AppName = "Random Photo Screensaver";
        public const string DbFileName = "store.sqlite";
        public const string MetadataFileName = "meta.sqlite";
        public const string PersistantConfigFileName = "settings.sqlite";
        public const string WallpaperFileName = "rpsBackground.bmp";

        public const string ExifToolMetadataOptions = "\n-All\n-Orientation#";

        public const string UpdateCheckURL = "http://www.abscreensavers.com/abdownloads/rps/latestversion.php";
        public const string DownloadFolder = "downloads";

        public const string NoImagesFound = "No images found in folder(s)\n\ror filter didn't return any results.\n\rPress 'S' key to enter setup";

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

        public static DBTableDefinition FileNodesDefinition = new DBTableDefinition(
            "FileNodes",
            new Dictionary<string, ColumnInfo> {
                {"id",              new ColumnInfo("INTEGER PRIMARY KEY AUTOINCREMENT", true, true) },
                {"path",            new ColumnInfo("TEXT UNIQUE", new FilterInfo("paths", true, FilterInfo.VarType.text), true, true)},
                {"parentpath",      new ColumnInfo("TEXT", new FilterInfo("parent paths", true, FilterInfo.VarType.text), true)},
                {"filename",        new ColumnInfo("TEXT", new FilterInfo("filenames", true, FilterInfo.VarType.text), true)},
                {"created",         new ColumnInfo("DATETIME", new FilterInfo("created date", true, FilterInfo.VarType.date), true)},
                {"modified",        new ColumnInfo("DATETIME", new FilterInfo("modified date", true, FilterInfo.VarType.date), true)},
                {"size",            new ColumnInfo("INTEGER", new FilterInfo("size", true, FilterInfo.VarType.numeral), true)},
                {"metainfoindexed", new ColumnInfo( "INTEGER DEFAULT 0", false, false)}
            }
        );

       
/*
        public static Dictionary<string, string> FileDatabaseColumns = 

/*
        public const string FileDatabaseColumns = @"
	`id`	            INTEGER PRIMARY KEY AUTOINCREMENT,
	`path`	            TEXT UNIQUE,
	`parentpath`	    TEXT,
	`filename`	        TEXT,
	`created`	        DATETIME,
	`modified`	        DATETIME,
	`size`	            INTEGER,
	`metainfoindexed`   INTEGER DEFAULT 0
        ";*/
/*
        public static string FileDatabaseSQL = @"
PRAGMA page_size=4096;
CREATE TABLE `FileNodes` (" + Constants.DictionaryToSQLColumnDefs(Constants.FileDatabaseColumns) + @"
        );";
        */
        public const string FileDatabaseIndexes = @"
CREATE UNIQUE INDEX FileNodes_Path ON FileNodes(path);
CREATE INDEX FileNodes_Parentpath ON FileNodes(parentpath);
CREATE INDEX FileNodes_Parentpath ON FileNodes(filename);
CREATE INDEX FileNodes_Parentpath ON FileNodes(size);
CREATE INDEX FileNodes_Created ON FileNodes(created);
CREATE INDEX FileNodes_Modified ON FileNodes(modified);
       ";

        public const string MetadataSQL = @"
PRAGMA page_size=4096;
CREATE TABLE `Metadata` (
	`id`	    INTEGER NOT NULL,
	`all`	    TEXT,
	`width`	    INTEGER,
	`height`    INTEGER,
	`area`	    INTEGER,
	PRIMARY KEY(id)
);
        ";

        public static DBTableDefinition MetadataDefinition = new DBTableDefinition(
            "Metadata",
            new Dictionary<string, ColumnInfo> {
                {"id",              new ColumnInfo("INTEGER PRIMARY KEY", false, false) },
                {"all",             new ColumnInfo("TEXT", new FilterInfo("metadata", true, FilterInfo.VarType.text))},
                {"width",           new ColumnInfo("INTEGER", new FilterInfo("width", true, FilterInfo.VarType.numeral))},
                {"height",          new ColumnInfo("INTEGER", new FilterInfo("height", true, FilterInfo.VarType.numeral))},
                {"area",            new ColumnInfo("INTEGER", new FilterInfo("area", true, FilterInfo.VarType.numeral))},
            }
        );



        public static DBTableDefinition SettingsDefinition = new DBTableDefinition(
            "Setting",
            new Dictionary<string, ColumnInfo> {
                {"id",              new ColumnInfo("INTEGER PRIMARY KEY AUTOINCREMENT") },
                {"key",             new ColumnInfo("TEXT UNIQUE")},
                {"value",           new ColumnInfo("TEXT")},
            }
        );

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

        public static double ratioFitIntoBounds(Rectangle image, Rectangle boundingBox, bool stretchSmall, bool cover) {
            double rw, rh;
            rw = (double)boundingBox.Width / (double)image.Width;
            rh = (double)boundingBox.Height / (double)image.Height;
            if (!stretchSmall && rw > 1 && rh > 1) {
                return 1;
            } else {
                if (cover) {
                    if ((double)boundingBox.Width / (double)boundingBox.Height < (double)image.Width / (double)image.Height) {
                        return rh;
                    } else {
                        return rw;
                    }
                } else {
                    if (rw < rh) {
                        return rw;
                    } else {
                        return rh;
                    }
                }
            }
            return 1;
        }

        public static Rectangle FitIntoBounds(Rectangle image, Rectangle boundingBox, bool stretchSmall, bool cover) {
            Rectangle r = boundingBox;
            double ratio = Constants.ratioFitIntoBounds(image, boundingBox, stretchSmall, cover);
            r.Width = (int)Math.Round((double)image.Width * ratio);
            r.Height = (int)Math.Round((double)image.Height * ratio);
            r.X = boundingBox.X + (int)(boundingBox.Width - r.Width) / 2;
            r.Y = boundingBox.Y + (int)(boundingBox.Height - r.Height) / 2;
            return r;
/*

            double rw, rh;
            rw = (double)boundingBox.Width / (double)image.Width;
            rh = (double)boundingBox.Height / (double)image.Height;
            if (!stretchSmall && rw > 1 && rh > 1) {
                r.Width = image.Width;
                r.Height = image.Height;
            } else {
                if (cover) {
                    if ((double)boundingBox.Width / (double)boundingBox.Height < (double)image.Width / (double)image.Height) {                        
                        r.Width = (int)Math.Round((double)image.Width * rh);
                        r.Height = (int)Math.Round((double)image.Height * rh);
                    } else {
                        r.Width = (int)Math.Round((double)image.Width * rw);
                        r.Height = (int)Math.Round((double)image.Height * rw);
                    }
                } else {
                    if (rw < rh) {
                        r.Width = (int)Math.Round((double)image.Width * rw);
                        r.Height = (int)Math.Round((double)image.Height * rw);
                    } else {
                        r.Width = (int)Math.Round((double)image.Width * rh);
                        r.Height = (int)Math.Round((double)image.Height * rh);
                    }
                }
            }
            r.X = boundingBox.X + (int)(boundingBox.Width - r.Width) / 2;
            r.Y = boundingBox.Y + (int)(boundingBox.Height - r.Height) / 2;
            return r;*/
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

        public static string getUpdateFolder() {
            return Path.Combine(Constants.getLocalAppDataFolder(), Constants.DownloadFolder);
        }

        public static string getLocalAppDataFolder() {
            return Path.Combine(
                Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData),
                Constants.AppName
            );
        }

        public static string getProgramDataFolder() {
            return Path.Combine(
                Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData),
                Constants.AppName
            );
        }

        public static string selectProgramAppDataFolder(string filename) {
            /**
             * 1. If filename exists in user folder (C:\Users\[user name]\AppData\Local\[Random Photo Screensaver]\filename) return full path
             * 2. Otherwise return full path to program data folder (C:\ProgramData\[Random Photo Screensaver]\filename)
             **/
            Console.WriteLine("findOrInitInDataFolder: " + filename);
            string fullPath;

            fullPath = Path.Combine(Constants.getLocalAppDataFolder(), filename);

            /* 1 */
            if (!File.Exists(fullPath)) {
                /* 2 */
                fullPath = Path.Combine(Constants.getProgramDataFolder(), filename);
            }
            return fullPath;
        }

        public static string getDataFolder(string filename) {
            /** 
             * 1. development folder 
             * 2. user folder (C:\Users\[user name]\AppData\Local\[Random Photo Screensaver])
             * 3. program data folder (C:\ProgramData\[Random Photo Screensaver])
             * 4. executable folder
             **/
            Console.WriteLine("getDataFolder: " + filename);
            string notFound = "";
            string fullPath;

            /* 1 */
            fullPath = Path.Combine(Application.StartupPath, Constants.devDataFolder, filename);

            /* 2 */
            if (!File.Exists(fullPath)) {
                notFound += fullPath + "\r\n";
                fullPath = Path.Combine(Constants.getLocalAppDataFolder(), Constants.DataFolder, filename);

                /* 3 */
                if (!File.Exists(fullPath)) {
                    notFound += fullPath + "\r\n";
                    fullPath = Path.Combine(Constants.getProgramDataFolder(), Constants.DataFolder, filename);

                    /* 4 */
                    if (!File.Exists(fullPath)) {
                        notFound += fullPath + "\r\n";
                        fullPath = Path.Combine(Application.StartupPath, Constants.DataFolder, filename);
                        requireFile(fullPath);
                    }
                }
            }

            return fullPath;
        }

        public static string getNiceVersion() {
            Version version = new Version(Application.ProductVersion);
            if (version.Revision == 0) {
                if (version.Build == 0) return new Version(version.Major, version.Minor).ToString();
                else return new Version(version.Major, version.Minor, version.Build).ToString();
            } else return Application.ProductVersion;
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
