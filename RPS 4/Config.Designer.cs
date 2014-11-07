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
            this.browser = new System.Windows.Forms.WebBrowser();
            this.SuspendLayout();
            // 
            // browser
            // 
            this.browser.Dock = System.Windows.Forms.DockStyle.Fill;
            this.browser.Location = new System.Drawing.Point(0, 0);
            this.browser.MinimumSize = new System.Drawing.Size(20, 20);
            this.browser.Name = "browser";
            this.browser.Size = new System.Drawing.Size(837, 537);
            this.browser.TabIndex = 0;
            // 
            // Config
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(837, 537);
            this.Controls.Add(this.browser);
            this.Name = "Config";
            this.Text = "Configuration Random Photo Screensaver";
            this.Deactivate += new System.EventHandler(this.Config_Deactivate);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Config_FormClosing);
            this.Shown += new System.EventHandler(this.Config_Shown);
            this.VisibleChanged += new System.EventHandler(this.Config_VisibleChanged);
            this.ResumeLayout(false);

        }

        #endregion

        public System.Windows.Forms.WebBrowser browser;



    }
}