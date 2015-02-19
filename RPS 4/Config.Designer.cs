namespace RPS {
    partial class Config {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing) {
            if (disposing && (components != null)) {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent() {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Config));
            this.browser = new System.Windows.Forms.WebBrowser();
            this.webUpdateCheck = new System.Windows.Forms.WebBrowser();
            this.timerCheckUpdates = new System.Windows.Forms.Timer(this.components);
            this.SuspendLayout();
            // 
            // browser
            // 
            this.browser.Dock = System.Windows.Forms.DockStyle.Fill;
            this.browser.Location = new System.Drawing.Point(0, 0);
            this.browser.MinimumSize = new System.Drawing.Size(20, 20);
            this.browser.Name = "browser";
            this.browser.Size = new System.Drawing.Size(784, 682);
            this.browser.TabIndex = 0;
            // 
            // webUpdateCheck
            // 
            this.webUpdateCheck.Location = new System.Drawing.Point(0, 0);
            this.webUpdateCheck.MinimumSize = new System.Drawing.Size(20, 20);
            this.webUpdateCheck.Name = "webUpdateCheck";
            this.webUpdateCheck.ScriptErrorsSuppressed = true;
            this.webUpdateCheck.Size = new System.Drawing.Size(250, 250);
            this.webUpdateCheck.TabIndex = 1;
            this.webUpdateCheck.Visible = false;
            this.webUpdateCheck.DocumentCompleted += new System.Windows.Forms.WebBrowserDocumentCompletedEventHandler(this.webUpdateCheck_DocumentCompleted);
            // 
            // timerCheckUpdates
            // 
            this.timerCheckUpdates.Enabled = true;
            this.timerCheckUpdates.Interval = 10000;
            this.timerCheckUpdates.Tick += new System.EventHandler(this.timerCheckUpdates_Tick);
            // 
            // Config
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(784, 682);
            this.Controls.Add(this.webUpdateCheck);
            this.Controls.Add(this.browser);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "Config";
            this.Text = "Configuration " + Constants.AppName;
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Config_FormClosing);
            this.VisibleChanged += new System.EventHandler(this.Config_VisibleChanged);
            this.ResumeLayout(false);

        }

        #endregion

        public System.Windows.Forms.WebBrowser browser;
        private System.Windows.Forms.WebBrowser webUpdateCheck;
        private System.Windows.Forms.Timer timerCheckUpdates;
//        private System.ComponentModel.BackgroundWorker bgwCheckUpdate;
    }
}