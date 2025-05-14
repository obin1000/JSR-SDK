namespace JSRDotNET_MultiForm
{
    partial class LoggerForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(LoggerForm));
            this.tbLogger = new System.Windows.Forms.TextBox();
            this.dlgSaveLog = new System.Windows.Forms.SaveFileDialog();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.logFileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveAsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.stopSavingCloseLogFIleToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.closeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.toolStripStatusLabel1 = new System.Windows.Forms.ToolStripStatusLabel();
            this.menuStrip1.SuspendLayout();
            this.statusStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // tbLogger
            // 
            this.tbLogger.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbLogger.Font = new System.Drawing.Font("Courier New", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tbLogger.Location = new System.Drawing.Point(0, 0);
            this.tbLogger.Multiline = true;
            this.tbLogger.Name = "tbLogger";
            this.tbLogger.ReadOnly = true;
            this.tbLogger.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.tbLogger.Size = new System.Drawing.Size(597, 562);
            this.tbLogger.TabIndex = 0;
            // 
            // dlgSaveLog
            // 
            this.dlgSaveLog.DefaultExt = "log";
            this.dlgSaveLog.FileName = "jsrcp";
            this.dlgSaveLog.Filter = "Log Files|*.log|All Files|*.*";
            this.dlgSaveLog.RestoreDirectory = true;
            this.dlgSaveLog.Title = "Save Log";
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.logFileToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(597, 24);
            this.menuStrip1.TabIndex = 4;
            this.menuStrip1.Text = "menuStrip1";
            this.menuStrip1.Visible = false;
            // 
            // logFileToolStripMenuItem
            // 
            this.logFileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.saveAsToolStripMenuItem,
            this.stopSavingCloseLogFIleToolStripMenuItem,
            this.closeToolStripMenuItem});
            this.logFileToolStripMenuItem.MergeAction = System.Windows.Forms.MergeAction.Insert;
            this.logFileToolStripMenuItem.MergeIndex = 1;
            this.logFileToolStripMenuItem.Name = "logFileToolStripMenuItem";
            this.logFileToolStripMenuItem.Size = new System.Drawing.Size(60, 20);
            this.logFileToolStripMenuItem.Text = "&Log File";
            // 
            // saveAsToolStripMenuItem
            // 
            this.saveAsToolStripMenuItem.Name = "saveAsToolStripMenuItem";
            this.saveAsToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.S)));
            this.saveAsToolStripMenuItem.Size = new System.Drawing.Size(204, 22);
            this.saveAsToolStripMenuItem.Text = "&Start Save To File";
            this.saveAsToolStripMenuItem.Click += new System.EventHandler(this.saveAsToolStripMenuItem_Click);
            // 
            // stopSavingCloseLogFIleToolStripMenuItem
            // 
            this.stopSavingCloseLogFIleToolStripMenuItem.Enabled = false;
            this.stopSavingCloseLogFIleToolStripMenuItem.Name = "stopSavingCloseLogFIleToolStripMenuItem";
            this.stopSavingCloseLogFIleToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.T)));
            this.stopSavingCloseLogFIleToolStripMenuItem.Size = new System.Drawing.Size(204, 22);
            this.stopSavingCloseLogFIleToolStripMenuItem.Text = "S&top Save To File";
            this.stopSavingCloseLogFIleToolStripMenuItem.Click += new System.EventHandler(this.stopSavingCloseLogFIleToolStripMenuItem_Click);
            // 
            // closeToolStripMenuItem
            // 
            this.closeToolStripMenuItem.Name = "closeToolStripMenuItem";
            this.closeToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.W)));
            this.closeToolStripMenuItem.Size = new System.Drawing.Size(204, 22);
            this.closeToolStripMenuItem.Text = "&Close";
            this.closeToolStripMenuItem.Click += new System.EventHandler(this.closeToolStripMenuItem_Click);
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripStatusLabel1});
            this.statusStrip1.Location = new System.Drawing.Point(0, 562);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(597, 22);
            this.statusStrip1.TabIndex = 5;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // toolStripStatusLabel1
            // 
            this.toolStripStatusLabel1.Name = "toolStripStatusLabel1";
            this.toolStripStatusLabel1.Size = new System.Drawing.Size(0, 17);
            // 
            // LoggerForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(597, 584);
            this.Controls.Add(this.tbLogger);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.menuStrip1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip1;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "LoggerForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "Control Panel Log";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.LoggerForm_FormClosing);
            this.Load += new System.EventHandler(this.LoggerForm_Load);
            this.Resize += new System.EventHandler(this.LoggerForm_Resize);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox tbLogger;
        private System.Windows.Forms.SaveFileDialog dlgSaveLog;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem logFileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveAsToolStripMenuItem;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel1;
        private System.Windows.Forms.ToolStripMenuItem stopSavingCloseLogFIleToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem closeToolStripMenuItem;
    }
}