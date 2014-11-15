using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;
using System.Security.Permissions;
using System.Configuration;
using System.IO;
using System.Data.SQLite;
//using System.Data.SQLite.Linq;
using System.Data.Linq;
using System.Data.Linq.Mapping;
using System.Collections;
//using System.DirectoryServices;
//using System.Management;
using Newtonsoft.Json;
using Microsoft.Win32;
using System.Net;
using System.Security.Cryptography;
/***
 * 
 * TODO: Reflect changes made in config.html into this.persistant!!!
 * 
 * 
 * 
 * 
 * Consider optimising by storing all values from html in c# array?
 * this.folders: takes 0.0002ms - 0.0005ms
 * this.browser.Document.GetElementById(id).InnerHtml takes 55.1357ms first request and 0.2303ms - 0.3389ms consequtive requests
 * 
 ***/

namespace RPS {
    [PermissionSet(SecurityAction.Demand, Name = "FullTrust")]
    [System.Runtime.InteropServices.ComVisibleAttribute(true)]

    public partial class Config : Form {

        public enum Order { Random = 1, Sequential = 0 };

        private Dictionary<string, object> persistant;
        DBConnector dbConnector;
        //SQLiteConnection connection;

        private Screensaver screensaver;
        //private FolderBrowserDialog folderBrowserDialog1;

        private string folderChanged = null;
        private bool excludeAllSubfolders;

        public long maxMonitorDimension = 0;

        public jsonFolder effects;

        private bool checkUpdates = false;
        private bool downloadUpdates = false;
        private bool newVersionAvailable = false;
        //private bool installUpdates = false;

        //public WebBrowser browser;

        //private delegate void AddBrowser();

        public Config(Screensaver screensaver) {
            this.screensaver = screensaver;
            this.InitializeComponent();
            this.browser.ObjectForScripting = this;
            this.browser.AllowWebBrowserDrop = false;
            foreach (Screen screen in Screen.AllScreens) {
                this.maxMonitorDimension = Math.Max(Math.Max(this.maxMonitorDimension, screen.Bounds.Width), screen.Bounds.Height);
            }
        }

        public SQLiteConnection connectToDB() {
            this.dbConnector = new DBConnector(
                Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), Constants.AppFolderName, Constants.PersistantConfigFileName),
                Constants.SettingsDatabaseSQL,
                false
            );
            //return new SQLiteConnection("Data Source=" + path + ";Version=3;");
            return this.dbConnector.connection;
        }

        public string jsFileBrowserDialog(string filename, string filter) {
            OpenFileDialog dialog = new System.Windows.Forms.OpenFileDialog();
            if (filename == null) filename = "";
            else dialog.InitialDirectory = Path.GetFullPath(filename);
            dialog.FileName = Path.GetFileName(filename);
            if (filter != null && filter.Length > 0) {
                dialog.Filter = filter;
            }
            if (dialog.ShowDialog() == DialogResult.OK) {
                return dialog.FileName;
            } else {
                return filename;
            }
        }


        public string jsFolderBrowserDialog(string path) {
            FolderBrowserDialog dialog = new System.Windows.Forms.FolderBrowserDialog();
            dialog.SelectedPath = path;
            if (dialog.ShowDialog() == DialogResult.OK) {
                return dialog.SelectedPath;
            } else {
                return path;
            }
        }

        public string jsRawConverterAvailable(string path) {
            if (File.Exists(path)) return "true";
            else return "false";
        }

        public string jsGetUFRawLocation() {
            string path = (string) Registry.GetValue("HKEY_CURRENT_USER\\Software\\Microsoft\\IntelliPoint\\AppSpecific\\ufraw.exe", "Path", null);
            path = path.Replace("ufraw.exe", "ufraw-batch.exe");
            if (path != null && path.Length > 0) {
                if (File.Exists(path)) {
                    return path;
                }
            }
            return null;
        }

        public void jsSetSelectedEffects(string jsonEffects) {
            this.effects = JsonConvert.DeserializeObject<jsonFolder>(jsonEffects);
            this.persistant["effects"] = jsonEffects;
        }

        public string jsGetSelectedEffects() {
            return JsonConvert.SerializeObject(this.effects);
        }

        public void setBrowserBodyClasses(WebBrowser browser, Screensaver.Actions action) {
            setBrowserBodyClasses(browser, action, null);
        }

        public static void setBrowserBodyClasses(WebBrowser browser, Screensaver.Actions action, string classes) {
            HtmlElementCollection elems = browser.Document.GetElementsByTagName("body");
            foreach (HtmlElement elem in elems) {
                switch (action) {
                    case Screensaver.Actions.Preview: classes += " preview"; break;
                    case Screensaver.Actions.Config: classes += " config"; break;
                    case Screensaver.Actions.Screensaver: classes += " screensaver"; break;
                    case Screensaver.Actions.Test: classes += " test"; break;
                    case Screensaver.Actions.Slideshow: classes += " slideshow"; break;
                }
                classes += " IE" + browser.Version.Major;
                if (browser.Version.Major < 8) classes += " lowIE";
                elem.SetAttribute("className", elem.GetAttribute("className") + classes);
            }
        }

        public void loadPersistantConfig() {
            if (this.screensaver.monitors != null) this.loadPersistantConfig(this.screensaver.monitors.Length);
            else this.loadPersistantConfig(Screen.AllScreens.Length);
        }

        public void loadPersistantConfig(int nrMonitors) {
            this.browser.Document.InvokeScript("initMonitors", new string[] { Convert.ToString(Screen.AllScreens.Length) });
            //SQLiteConnection connection = 
            this.connectToDB();
            this.persistant = new Dictionary<string, object>();

            DataContext context = new DataContext(this.dbConnector.connection);
            var items = context.GetTable<Setting>();
            //MessageBox.Show("gettable");
            //var items = context.GetTable(typeof(Setting));
            foreach(Setting item in items) {
//                MessageBox.Show("item" + item.Key + " " + item.Value);
                this.persistant.Add(item.Key, item.Value);
            }
            if (!this.persistant.ContainsKey("folders") || this.persistant["folders"] == null || this.getPersistantString("folders").Trim().Length == 0) { 
                string path = Environment.GetFolderPath(Environment.SpecialFolder.MyPictures) + Environment.NewLine + 
                                                        Environment.GetFolderPath(Environment.SpecialFolder.MyVideos);
                if (!this.persistant.ContainsKey("folders")) {
                    this.persistant.Add("folders", path);
                } else {
                    this.persistant["folders"] = path;
                }
            }

            if (!this.persistant.ContainsKey("rawFolder") || this.persistant["rawFolder"] == null || Convert.ToString(this.persistant["rawFolder"]).Trim().Length == 0) {
                string path = Path.Combine(
                    Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), 
                    Constants.AppFolderName,
                    Constants.RawCacheFolder
                );
                    
                if (!this.persistant.ContainsKey("rawFolder")) {
                    this.persistant.Add("rawFolder", path);
                } else {
                    this.persistant["rawFolder"] = path;
                }
            }

            if (!this.persistant.ContainsKey("effects") || this.persistant["effects"] == null || Convert.ToString(this.persistant["effects"]) == "null" || Convert.ToString(this.persistant["effects"]).Trim().Length == 0) {
                // Load from effects.json
                string path = Constants.getDataFolder(Constants.EffectsJsonFile);
                if (File.Exists(path)) {
                    //this.effects = new jsonFolder();
                    JsonSerializer serializer = new JsonSerializer();
                    serializer.NullValueHandling = NullValueHandling.Ignore;
                    using (StreamReader sr = new StreamReader(path))
                    using (JsonReader reader = new JsonTextReader(sr)) {
                        this.effects = serializer.Deserialize<jsonFolder>(reader);
                    }
                }
            } else {
                this.effects = JsonConvert.DeserializeObject<jsonFolder>(Convert.ToString(this.persistant["effects"]));
            }

            HtmlElementCollection hec = this.GetElementsByTagName("input");
            foreach (HtmlElement e in hec) {
                if (this.persistant.ContainsKey(e.GetAttribute("id")) || (e.GetAttribute("type") == "radio" && this.persistant.ContainsKey(e.GetAttribute("name")))) {
                    switch (e.GetAttribute("type")) {
                        case "checkbox":
                            if (this.getPersistantBool(e.GetAttribute("id")) == true) {
                                e.SetAttribute("checked", "true");
                            } else {
                                e.SetAttribute("checked", "");
                            }
                            break;
                        case "radio":
                            if (this.getPersistantString(e.GetAttribute("name")) == e.GetAttribute("value")) {
                                e.SetAttribute("checked", "true");
                            }
                            break;
                        default:
                            e.SetAttribute("value", this.getPersistantString(e.GetAttribute("id")));
                            break;
                    }
                } else {
                    switch (e.GetAttribute("type")) {
                        case "checkbox":
                            this.persistant[e.GetAttribute("id")] =  this.getDomCheckboxValue(e.GetAttribute("id"));
                        break;
                        case "radio":
                            this.persistant[e.GetAttribute("name")] =  this.getDomRadioValue(e.GetAttribute("name"));
                        break;
                        default:
                            this.persistant[e.GetAttribute("id")] =  this.getDomValue(e.GetAttribute("id"));
                        break;
                    }
                    
                    // Set persistant value with default
                }
            }

            hec = this.browser.Document.GetElementsByTagName("textarea");
            foreach (HtmlElement e in hec) if (this.persistant.ContainsKey(e.GetAttribute("id"))) {
                e.SetAttribute("value", Convert.ToString(this.persistant[e.GetAttribute("id")]));
            }

            string classes= null;
            if (nrMonitors > 1) classes += " multi ";
            Config.setBrowserBodyClasses(this.browser, this.screensaver.action, classes);

            this.browser.Document.InvokeScript("persistantConfigLoaded", new string[] { Convert.ToString(Screen.AllScreens.Length) });

            if (this.screensaver.action == Screensaver.Actions.Preview && this.screensaver.monitors != null) {
                this.screensaver.monitors[0].defaultShowHide();
            }

        }

        public void safePersistantConfig() {
            this.persistant["effects"] = JsonConvert.SerializeObject(this.effects);
            if (this.screensaver.action != Screensaver.Actions.Config) {
                for (int i = 0; i < this.screensaver.monitors.Length; i++) {
                    this.persistant["historyM" + Convert.ToString(i)] = JsonConvert.SerializeObject(this.screensaver.monitors[i].historyLastEntries(Convert.ToInt32(this.getPersistant("rememberLast"))));
                    this.persistant["historyOffsetM" + Convert.ToString(i)] = Convert.ToString(this.screensaver.monitors[i].offset);
                }
            }
/*
            HtmlElementCollection hec = this.GetElementsByTagName("input");
            foreach (HtmlElement e in hec) {
                switch (e.GetAttribute("type")) {
                    case "checkbox":
                        this.persistant[e.GetAttribute("id")] = e.GetAttribute("checked").ToLower();
                    break;
                    case "radio":
                        if (e.GetAttribute("checked").ToLower() == "true") {
                            this.persistant[e.GetAttribute("name")] = e.GetAttribute("value");
                        }
                    break;
                    default:
                        this.persistant[e.GetAttribute("id")] = e.GetAttribute("value");
                    break;
                }
            }
               
            hec = this.browser.Document.GetElementsByTagName("textarea");
            foreach (HtmlElement e in hec) {
                this.persistant[e.GetAttribute("id")] = e.GetAttribute("value");
            }
*/
            this.connectToDB();
            SQLiteTransaction transaction = null;
            try {
                transaction = this.dbConnector.connection.BeginTransaction();
            } catch (System.Data.SQLite.SQLiteException se) {
                this.screensaver.showInfoOnMonitors("Error: " + se.Message, true);
            }
            foreach (var p in this.persistant) {
                SQLiteCommand insertSQL = new SQLiteCommand("INSERT OR REPLACE INTO Setting (key, value) VALUES (@key, @value);", this.dbConnector.connection);
                insertSQL.Parameters.AddWithValue("@key", p.Key);
                insertSQL.Parameters.AddWithValue("@value", p.Value);
                insertSQL.ExecuteNonQuery();
            }
            transaction.Commit();
            //connection.Close();
            //this.dbConnector.Close();
        }
 
/*
        public string getPersistant(string key) {
            if (!this.persistant.ContainsKey(key)) throw new KeyNotFoundException(key);
            return Convert.ToString(this.persistant[key]);
        }*/

        public void Message(string Text) {
//            Debug.WriteLine(this.Parent.ToString());
            MessageBox.Show(Text);
        }

        public HtmlElementCollection GetElementsByTagName(string name) {
            return (HtmlElementCollection)this.browser.Invoke(new Func<HtmlElementCollection>(() => this.browser.Document.GetElementsByTagName(name)));
        }

        /****
         * Called from config.js
         ****/
        public void applyFilter(string filter) {
            this.screensaver.fileNodes.setFilterSQL(filter);
        }

        public void clearFilter(string dummy) {
            this.screensaver.fileNodes.clearFilter();
        }

        public bool resetWallpaper() {
            Wallpaper wallpaper = new Wallpaper(this.screensaver);
            wallpaper.resetDefaultWallpaper();
            return true;
        }

        /****
         * Called from config.js
         * dumdum variable is used to avoid calling the function when testing with
         * if (typeof(window.external.getInitialFoldersJSON) !== "undefined") in JavaScript
         ****/
        public string getInitialFoldersJSON(bool dumdum) {
            List<jsonFolder> folders = new List<jsonFolder>();
            //MessageBox.Show("getInitialFoldersJSON()");
            //            folders.Add(new jsonFolder());
            //          folders.Add(new jsonFolder(Environment.GetFolderPath(Environment.SpecialFolder.MyVideos)));
            /*
                        jsonFolder network = new jsonFolder("Network");
                        network.key = "\\";
                        folders.Add(network);
                        DirectoryEntry root = new DirectoryEntry("WinNT:");
                        foreach (DirectoryEntry networkComputers in root.Children) {
                            foreach (DirectoryEntry networkComputer in networkComputers.Children) {
                                if (networkComputer.Name != "Schema") {
                                    jsonFolder networked = new jsonFolder(networkComputer.Name);
                                    networked.lazy = true;
                                    network.children.Add(networked);

                                    try {
                                        ManagementObjectSearcher searcher =
                                            new ManagementObjectSearcher("root\\CIMV2",
                                            "SELECT * FROM Win32_Share");

                                        foreach (ManagementObject queryObj in searcher.Get()) {
                                            Debug.WriteLine("-----------------------------------");
                                            Debug.WriteLine("Win32_Share instance");
                                            Debug.WriteLine("-----------------------------------");
                                            Debug.WriteLine("Name: {0}", queryObj["Name"]);
                                        }
                                    } catch (ManagementException e) {
                                        MessageBox.Show("An error occurred while querying for WMI data: " + e.Message);
                                    }
                                    //textBox1.Text += computer.Name + "\r\n";
                                }
                            }
                        }
                        */
            jsonFolder computer = new jsonFolder("Computer");
            computer.expanded = true;
            folders.Add(computer);
            DriveInfo[] info = DriveInfo.GetDrives();
            foreach (DriveInfo di in info) {
                string drive = "(" + di.Name.Replace("\\", "") + ")";
                jsonFolder f = new jsonFolder(drive);
                f.lazy = true;
                f.key = di.Name;

                string extraInfo = "";
                if (di.IsReady) {
                    extraInfo += di.VolumeLabel + " ";
                } else {
                    f.unselectable = true;
                    f.extraClasses = "dim";
                }
                switch (di.DriveType) {
                    case DriveType.Removable:
                        if (extraInfo == "") extraInfo += "Removable Disk ";
                        break;
                }
                if (extraInfo != "") f.title = extraInfo + f.title;
                computer.children.Add(f);
            }

            if (this.persistant == null || this.getPersistant("folders") == null) {
                this.loadPersistantConfig();
            }

            foreach (string folder in Convert.ToString(this.getPersistant("folders")).Split(new string[] { Environment.NewLine, "\r\n", "\n" }, StringSplitOptions.RemoveEmptyEntries)) {
                if (folder.Substring(0, 2) == "\\\\") {
                    string[] parts = folder.Split(new string[] { "\\" }, StringSplitOptions.RemoveEmptyEntries);
                    int i = 0;
                    jsonFolder network = new jsonFolder("Network");
                    folders.Add(network);
                    jsonFolder node = network;
                    while (i < parts.Length) {
                        jsonFolder newNode = new jsonFolder(parts[i]);
                        node.children.Add(newNode);
                        i++;
                        if (i == parts.Length) {
                            newNode.selected = true;
                            jsonFolder dummy = new jsonFolder("dummy");
                            dummy.selected = false;
                            //dummy.unselectable = false;
                            dummy.extraClasses = "hidden";
                            node.children.Add(dummy);
                        }
                        node = newNode;
                    }
                } else {
                    jsonFolder node = computer;
                    string basePath = "";
                    string[] parts = folder.Split(new string[] { "\\" }, StringSplitOptions.RemoveEmptyEntries);

                    int i = 0;
                    while (i < parts.Length) {
                        string key = parts[i].ToLower();
                        if (key.IndexOf(':') > -1) key = key.ToUpper() + "\\";
                        jsonFolder newNode = node.hasChild(key);

                        if (newNode == null) {
                            // Add children if not found
                            node.children.AddRange(this.getFolder(basePath));
                            node = node.hasChild(key);
                            if (node == null) break; // Escape while loop if still not found
                        } else {
                            node = newNode;
                        }
                        //node.expanded = true;
                        node.selected = true;
                        basePath += parts[i] + "\\";
                        i++;
                    }
                    if (node != null) node.selected = true;
                }
            }
            return JsonConvert.SerializeObject(folders);
        }

        public List<jsonFolder> getFolder(string folder) {
            List<jsonFolder> children = new List<jsonFolder>();
            //folder = folder.Replace("computer\\", "");
            string[] dirs = Directory.GetDirectories(folder);
            foreach (string dir in dirs) {
                try {
                    FileInfo fi = new FileInfo(dir);
                    if ((fi.Attributes & FileAttributes.Hidden) != FileAttributes.Hidden) {
                        jsonFolder d = new jsonFolder(fi.Name.ToLower());
                        d.lazy = (Directory.GetDirectories(dir).Length > 0);
                        children.Add(d);
                    }
                } catch (Exception e) {
                    Debug.WriteLine("getFolder " + e.Message);
                    // No access
                }
            }
            return children;
        }

        public string getFolderJSON(string folder) {
            return JsonConvert.SerializeObject(this.getFolder(folder));
        }

        public string getEffectsJSON() {
            return JsonConvert.SerializeObject(this.effects);

        }

        public string getRandomEffect() {
            string json = null;
            if (this.getPersistantBool("useTransitions") && this.effects != null) {
                jsonFolder effect = jsonFolder.getRandomSelected(this.effects.children);
                jsonFolder direction = null;
                if (effect == null) {
                    json = "{\"effect\":\"fade\", \"duration\":100}";
                } else { 
                    json += "{\"effect\":\"" + effect.key + "\"";
                    if (effect.children.Count > 0) {
                        direction = jsonFolder.getRandomSelected(effect.children);
                        if (direction != null) json += ", \"direction\":\"" + direction.title.ToLower() + "\"";
                    }
                    json += ", \"duration\":1000}";
                }
            }
            return json;
        }

        public void InputChanged(string id, string value) {
            this.setPersistant(id, value);
            /*            switch (type.ToLower()) {
                            case "checkbox":
                                this.setPersistant(id, this.getDomCheckboxValue(id));
                            break;
                            default:
                                this.setPersistant(id, this.getDomValue(id));
                            break;
                        }*/
        }

        public string InvokeScriptOnMonitor(int monitor, string script, string parameters) {
            string s = null;
            if (this.screensaver.monitors != null) for (int i = 0; i < this.screensaver.monitors.Length; i++) {
                if (monitor == Screensaver.CM_ALL || monitor == i) {
                    s += Convert.ToString(this.screensaver.monitors[i].InvokeScript(script, parameters.Split(';')));
                }
            }
            return s;
        }

        private HtmlElement getElementById(string id) {
            // Make thread safe 
            try {
                return (HtmlElement)this.browser.Invoke(new Func<HtmlElement>(() => this.browser.Document.GetElementById(id)));
            } catch (Exception e) {
                Debug.WriteLine("getElementById" + e.Message);
                return null;
            }
        }

        /***
         * This function assumes that <div id="#name#"> encapsulates
         *  <input type="radio" name="#name# value=...> lines
         ***/
        private string getDomRadioValue(string id) {
            HtmlElement he = (HtmlElement)this.browser.Invoke(new Func<HtmlElement>(() => this.browser.Document.GetElementById(id)));
            if (he != null) {
                HtmlElementCollection hec = he.GetElementsByTagName("input");
                foreach (HtmlElement e in hec) {
                    if (e.GetAttribute("type").ToLower() == "radio" && e.GetAttribute("checked").ToLower() == "true") {
                        return e.GetAttribute("value");
                    }
                }
            }
            return null;
        }

        private bool getDomCheckboxValue(string id) {
            HtmlElement he = this.getElementById(id);
            if (he == null) return false;
            switch (he.TagName.ToLower()) {
                case "input":
                    return (bool)he.GetAttribute("checked").ToLower().Equals("true");
                break;
                default:
                    Debug.WriteLine("getCheckboxValue called on non checkbox");
                break;
            }
            return false;
        }

        public bool persistantLoaded() {
            return this.persistant != null;
        }

        public bool hasPersistantKey(string key) {
            return this.persistant.ContainsKey(key);
        }

        public void setPersistant(string key, object value) {
            if (key == null) {
                this.screensaver.showInfoOnMonitors("Invalid configuration key: null", true);
            } else { 
                this.persistant[key] = value;
            }
        }

        public object getPersistant(string key) {
            if (!this.persistant.ContainsKey(key)) throw new KeyNotFoundException(key);
            return persistant[key];
        }

        public bool getPersistantBool(string key) {
            if (!this.persistant.ContainsKey(key)) throw new KeyNotFoundException(key);
            if (this.persistant[key].GetType() == typeof(bool)) {
                return (bool)this.persistant[key];
            }
            switch (Convert.ToString(this.persistant[key])) {
                case "True": case "true": case "1": 
                    return true; 
                break;
                case "False": case "false": case "0": 
                    return false;
                break;
            }
            throw new Exception("Can't cast keys '" + key + "' value " + this.persistant[key] + key + " to boolean");
        }

        public string getPersistantString(string key) {
            return Convert.ToString(this.getPersistant(key));
        }

        public bool syncMonitors() {
            if (this.persistant == null) return false;
            return this.getPersistantBool("syncScreens");
        }

        public Config.Order changeOrder() {
            if (this.getOrder() == Config.Order.Random) {
                //this.checkCheckbox("orderSequential");
                this.setPersistant("order", Config.Order.Sequential);
                return Config.Order.Sequential;
            } else {
                //this.checkCheckbox("orderRandom");
                this.setPersistant("order", Config.Order.Random);
                return Config.Order.Random;
            }
//            return null;
        }

        public Config.Order getOrder() {
            if (this.getPersistant("order").GetType() == typeof(string)) {
                switch (this.getPersistantString("order")) {
                    case "random": case "1":
                        return Config.Order.Random;
                    break;
                    default:
                        return Config.Order.Sequential;
                    break;
                }
            } else {
                if ((Config.Order)Enum.Parse(typeof(Config.Order), this.getPersistantString("order")) == Config.Order.Random) return Config.Order.Random;
                else return Config.Order.Sequential;
            }
        }

        public void setInnerHTML(string id, string html) {
            HtmlElement he = this.getElementById(id);
            if (he != null) {
                he.InnerHtml = html;
            }
        }

        public void setDomValue(string id, string value) {
            HtmlElement he = this.getElementById(id);
            if (he != null) {
                switch (he.TagName.ToLower()) {
                    case "textarea":
                        he.InnerHtml = value;
                    break;
                    default:
                        switch (he.GetAttribute("type").ToLower()) {
                            case "radio":
                                //he.SetAttribute("value", value);
                                if (value == "checked" || value == he.GetAttribute("value").ToLower()) {
                                    he.SetAttribute("checked", "true");
                                } else {
                                    he.SetAttribute("checked", "");
                                }
                            break;                            
                            case "checkbox":
                                if (value == "false") {
                                    he.SetAttribute("checked", "");
                                } else {
                                    he.SetAttribute("checked", "true");
                                }
                            break;
                            default:
                                he.SetAttribute("value", value);
                            break;
                        }
                    break;
                }
            }
        }

        public string getDomValue(string id) {
            HtmlElement he = this.getElementById(id);
            if (he == null) return null;
            try {
                switch (he.TagName.ToLower()) {
                    case "textarea":
                        return he.InnerHtml;
                        break;
                    default:
                        return he.GetAttribute("value");
                        break;
                }
            } catch (System.Runtime.InteropServices.COMException co) {
                //this.screensaver.showInfoOnMonitors("Error getPersistant(" + id + ")\n" + Convert.ToString(co.Message));
                return null;
            }
            return null;
        }

        
        public void Config_FormClosing(object sender, FormClosingEventArgs e) {            
            if (screensaver.action == Screensaver.Actions.Config) {
                this.safePersistantConfig();
                Application.Exit();
            } else if (!this.screensaver.applicationClosing) {
                if (!this.syncMonitors()) {
                    // restart timer in-case sync option has changed.
                    for (int i = 1; i < this.screensaver.monitors.Length; i++) {
                        this.screensaver.monitors[i].timer.Start();
                    }
                }
                //Console.Beep();
                this.Hide();
                e.Cancel = true;
            } 
        }

        private void Config_Shown(object sender, EventArgs e) {
            this.setInnerHTML("version", Application.ProductVersion.Replace(".0.", " Beta ").Replace(".0", ""));
            this.browser.Document.InvokeScript("initFancyTreeFolder");
            this.browser.Document.InvokeScript("initFancyTreeTransitions");
        }

        private void Config_VisibleChanged(object sender, EventArgs e) {
            if (this.Visible && this.screensaver.action != Screensaver.Actions.Config) {
                // Showing
                this.folderChanged = Convert.ToString(this.getPersistant("folders"));
                this.excludeAllSubfolders = this.getPersistantBool("excludeAllSubfolders");
            } else if (this.screensaver.action != Screensaver.Actions.Config) {
                // Hiding
                if (this.folderChanged != this.getPersistant("folders") || this.excludeAllSubfolders != this.getPersistantBool("excludeAllSubfolders")) {
                    //this.screensaver.fileNodes.purgeNotMatchingParentFolders(this.getPersistant("folders"));
                    this.screensaver.fileNodes.restartBackgroundWorkerImageFolder();
//                    MessageBox.Show("changed");
                }
            }
        }

        private void Config_Deactivate(object sender, EventArgs e) {
            if (this.screensaver.action != Screensaver.Actions.Config) {
                this.screensaver.configHidden = true;
                this.Hide();
            }
        }

        void DownloadFileCompleted(object sender, AsyncCompletedEventArgs e) {
            HtmlElement he = this.webUpdateCheck.Document.GetElementById("download");
            string updatePath = Path.Combine(Application.StartupPath, Constants.DownloadFolder, Path.GetFileName(he.GetAttribute("href")));
            if (!this.VerifyMD5(updatePath, he.GetAttribute("data-md5"))) {
                this.showUpdateInfo("Download " + he.GetAttribute("data-version") + " failed<br/>Please <a href='" + he.GetAttribute("href") + "'>download update manually</a>.");
            }
            this.screensaver.showUpdateInfo("RPS " + he.GetAttribute("data-version") + " downloaded<br/><a class='exit external' target='_blank' href='file://" + updatePath + "'>Click to install now</a>.");
        }

        bool VerifyMD5(string path, string md5verify) {
            using (var md5 = MD5.Create()) {
                using (var stream = File.OpenRead(path)) {
                    return BitConverter.ToString(md5.ComputeHash(stream)).Replace("-", string.Empty).ToLower() == md5verify.ToLower();
                }
            } 
        }

        private void notifyUpdateAvailable() {

        }

        private void showUpdateInfo(string info) {
            HtmlElement he = this.browser.Document.GetElementById("update");
            he.InnerHtml = info.Replace("<br/>", " ");
            this.screensaver.showUpdateInfo(info);
        }

        private Uri getUpdateUri() {
            string param = "";
            if (this.screensaver.config.getPersistantBool("disableGoAn")) param = "?track=no";
            return new Uri(Constants.UpdateCheckURL + param);
        }


        private void webUpdateCheck_DocumentCompleted(object sender, WebBrowserDocumentCompletedEventArgs e) {
            if (this.webUpdateCheck.Url.Equals(this.getUpdateUri())) {
                HtmlElement he = this.webUpdateCheck.Document.GetElementById("download");
                if (he != null) {
                    Version update = new Version(he.GetAttribute("data-version"));
                    this.newVersionAvailable = (this.screensaver.version.CompareTo(update) < 0);

                    if (this.newVersionAvailable) {
                        if (this.downloadUpdates) {
                            string updatePath = Path.Combine(Application.StartupPath, Constants.DownloadFolder, Path.GetFileName(he.GetAttribute("href")));
                            if (!File.Exists(updatePath) || !this.VerifyMD5(updatePath, he.GetAttribute("data-md5"))) {
                                this.showUpdateInfo("Downloading update: " + update.ToString());
                                Directory.CreateDirectory(Path.Combine(Application.StartupPath, Constants.DownloadFolder));
                                WebClient client = new WebClient();
                                client.DownloadFileCompleted += new AsyncCompletedEventHandler(DownloadFileCompleted);
                                client.DownloadFileAsync(new Uri(he.GetAttribute("href")), updatePath);
                                return;
                            } else {
                                this.DownloadFileCompleted(this, null);
                            }
                        } else {
                            this.showUpdateInfo("Update available<br/><a href='" + he.GetAttribute("href") + "'>Download RPS " + he.GetAttribute("data-version-text") + "</a>");
                        }
                    }
                }
            }
        }

        private void timerCheckUpdates_Tick(object sender, EventArgs e) {
            this.timerCheckUpdates.Enabled = false;
            switch (this.getPersistantString("checkUpdates")) {
                case "yes":
                    this.checkUpdates = true;
                    this.downloadUpdates = true;
                break;
                case "notify":
                    this.checkUpdates = true;
                break;
            }
            if (this.checkUpdates) {
                this.webUpdateCheck.Url = this.getUpdateUri();
            }

        }
    }
}
