using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.Drawing;
using System.Data;
//using System.Windows.Media;
using System.IO;

namespace RPS {
    class Wallpaper {
        [DllImport("user32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool SystemParametersInfo(uint uiAction, uint uiParam, string pvParam, uint fWinIni);
        
        private const uint SPI_SETDESKWALLPAPER = 20;
        private const uint SPIF_UPDATEINIFILE = 0x01;
        private const uint SPIF_SENDWININICHANGE = 0x02;

        private Screensaver screensaver;
        public Wallpaper(Screensaver screensaver) {
            this.screensaver = screensaver;
        }

        public void resetDefaultWallpaper() {
            SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, "", SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE);
//            this.setWallpaper();
        }

        public Rectangle getDesktopBounds() {
            Rectangle r = Screen.AllScreens[0].Bounds;
            for(int i = 1; i < Screen.AllScreens.Length; i++) {
                r = Rectangle.Union(r, Screen.AllScreens[i].Bounds);
            }
            return r;
        }

        public bool changeWallpaper() {
            switch(this.screensaver.config.getRadioValue("wallpaperChange")) {
                case "never": 
                    return false;
                break;
                case "daily":
                    if (Convert.ToDateTime(this.screensaver.config.getValue("wallpaperLastChange")).Equals(DateTime.Today)) return false;
                break;
            }
            return true;
        }

        public Rectangle FitIntoBounds(Rectangle image, Rectangle boundingBox, bool stretchSmall) {
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

        public void setWallpaper() {
            string[] paths = new string[Screen.AllScreens.Length];
            for (int i = 0; i < Screen.AllScreens.Length; i++) {
                int j = 0;
                DataRow dr;
                do {
                    dr = this.screensaver.fileNodes.getRandomImage();
                    paths[i] = Convert.ToString(dr["path"]);
                    j++;
                } while (paths[i] == null && j < 10);
            }
            this.setWallpaper(paths);
        }

        /**
         * Set single screen wallpaper path
         **/
        public void setWallpaper(long monitor, string path) {
            string[] paths = new string[Screen.AllScreens.Length];
            for (int i = 0; i < Screen.AllScreens.Length; i++) {
                if (monitor == Screensaver.CM_ALL || monitor == i) {
                    paths[i] = path;
                }
            }
            this.setWallpaper(monitor, paths);
        }

        public void setWallpaper(string[] paths) {
            this.setWallpaper(Screensaver.CM_ALL, paths);
        }

        public void setWallpaper(long monitor, string[] paths) {
            // Wallpaper will have exact size of monitors so wallpaper style (Tile, Center, Stretch, Fit, Fill) shouldn't matter
            string wallpaperPath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), Constants.AppFolderName, Constants.WallpaperFileName);
            Rectangle r = this.getDesktopBounds();
            Bitmap wallpaper  = new Bitmap(r.Width, r.Height);
            Graphics g = Graphics.FromImage(wallpaper);
            GraphicsUnit units = GraphicsUnit.Pixel;
            if (monitor != Screensaver.CM_ALL) {
                if (File.Exists(wallpaperPath)) {
                    try {
                        Image old = Image.FromFile(wallpaperPath);
                        g.DrawImage(old, old.GetBounds(ref units));
                        old.Dispose();
                    } catch (Exception ex) {

                    }
                }
            }
            Color c = (Color)new ColorConverter().ConvertFromString(Convert.ToString(this.screensaver.config.getValue("wallpaperBackgroundColour")));
            System.Drawing.Brush fill = new System.Drawing.SolidBrush(c);

            for(int i = 0; i < Screen.AllScreens.Length; i++) {
                if (monitor == Screensaver.CM_ALL || monitor == i) {
                    bool readSuccess = false;
                    Image image = null;
                    try {
                        image = Image.FromFile(paths[i]);
                        readSuccess = true;
                    } catch (OutOfMemoryException ex) {
                        this.screensaver.monitors[0].showInfoOnMonitor("Out of memory reading '" + paths[i] + "' for wallpaper");
                    } catch (FileNotFoundException ex) {
                        this.screensaver.monitors[0].showInfoOnMonitor("File not found '" + paths[i] + "' for wallpaper");
                    }
                    if (readSuccess) {
                        g.FillRectangle(fill, Screen.AllScreens[i].Bounds);
                        g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.HighQuality;
                        g.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.HighQualityBicubic;
                        g.DrawImage(image, this.FitIntoBounds(Rectangle.Round(image.GetBounds(ref units)), Screen.AllScreens[i].Bounds, this.screensaver.config.getCheckboxValue("wallpaperStretchSmall")));
                        if (this.screensaver.config.getCheckboxValue("wallpaperFilenames")) {
                            // ToDo: Get font settings from config.html
                            Font font = new Font("Arial", 10);
                            g.DrawString(paths[i], font, new SolidBrush(Color.Black), Screen.AllScreens[i].Bounds.Left + 1, Screen.AllScreens[i].Bounds.Top + 1);
                            g.DrawString(paths[i], font, new SolidBrush(Color.White), Screen.AllScreens[i].Bounds.Left, Screen.AllScreens[i].Bounds.Top);
                        }
                    }
                }
                //this.screensaver.config.getCheckboxValue("wallpaperStretchSmall")
            }
            if (!Directory.Exists(Path.GetDirectoryName(wallpaperPath))) {
                if (DialogResult.OK != MessageBox.Show("Create folder '"+Path.GetDirectoryName(wallpaperPath)+"'>\n\nOk: Creates folder for backgrounds\nCancel doesn't change background image.", "Installation folder for background not found!", MessageBoxButtons.OKCancel, MessageBoxIcon.Exclamation)) {
                    return;
                }
                Directory.CreateDirectory(Path.GetDirectoryName(wallpaperPath));
            }
            //try {
                wallpaper.Save(wallpaperPath, System.Drawing.Imaging.ImageFormat.Bmp);
            //} catch(Exception ex) {}
            if (File.Exists(wallpaperPath)) {
                SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, wallpaperPath, SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE);
                this.screensaver.config.setValue("wallpaperLastChange", Convert.ToString(DateTime.Today));
                Console.Beep();
            }
        }
    }
}
