namespace JSRDotNET_MultiForm
{
    partial class FormAbout
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
            this.components = new System.ComponentModel.Container();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle2 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle3 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle4 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle5 = new System.Windows.Forms.DataGridViewCellStyle();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormAbout));
            this.lblVersion = new System.Windows.Forms.Label();
            this.btnAccept = new System.Windows.Forms.Button();
            this.gridPlugins = new System.Windows.Forms.DataGridView();
            this.nameDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.friendlyNameDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.versionDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.InterfaceVersion = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.iJSRDotNETLibMetadataBindingSource1 = new System.Windows.Forms.BindingSource(this.components);
            this.connectionTypeBindingSource = new System.Windows.Forms.BindingSource(this.components);
            this.iJSRDotNETLibMetadataBindingSource = new System.Windows.Forms.BindingSource(this.components);
            this.lblPlugins = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.gridPlugins)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.iJSRDotNETLibMetadataBindingSource1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.connectionTypeBindingSource)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.iJSRDotNETLibMetadataBindingSource)).BeginInit();
            this.SuspendLayout();
            // 
            // lblVersion
            // 
            this.lblVersion.AutoSize = true;
            this.lblVersion.Location = new System.Drawing.Point(9, 9);
            this.lblVersion.Name = "lblVersion";
            this.lblVersion.Size = new System.Drawing.Size(48, 13);
            this.lblVersion.TabIndex = 2;
            this.lblVersion.Text = "Version: ";
            // 
            // btnAccept
            // 
            this.btnAccept.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnAccept.Location = new System.Drawing.Point(137, 263);
            this.btnAccept.Name = "btnAccept";
            this.btnAccept.Size = new System.Drawing.Size(75, 23);
            this.btnAccept.TabIndex = 0;
            this.btnAccept.Text = "OK";
            this.btnAccept.UseVisualStyleBackColor = true;
            this.btnAccept.Click += new System.EventHandler(this.btnAccept_Click);
            // 
            // gridPlugins
            // 
            this.gridPlugins.AllowUserToAddRows = false;
            this.gridPlugins.AllowUserToDeleteRows = false;
            this.gridPlugins.AllowUserToResizeRows = false;
            this.gridPlugins.AutoGenerateColumns = false;
            this.gridPlugins.AutoSizeRowsMode = System.Windows.Forms.DataGridViewAutoSizeRowsMode.DisplayedCells;
            dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle1.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle1.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.gridPlugins.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle1;
            this.gridPlugins.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.gridPlugins.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.nameDataGridViewTextBoxColumn,
            this.friendlyNameDataGridViewTextBoxColumn,
            this.versionDataGridViewTextBoxColumn,
            this.InterfaceVersion});
            this.gridPlugins.DataSource = this.iJSRDotNETLibMetadataBindingSource1;
            this.gridPlugins.Location = new System.Drawing.Point(12, 69);
            this.gridPlugins.MultiSelect = false;
            this.gridPlugins.Name = "gridPlugins";
            this.gridPlugins.ReadOnly = true;
            this.gridPlugins.RowHeadersVisible = false;
            this.gridPlugins.RowTemplate.ReadOnly = true;
            this.gridPlugins.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.gridPlugins.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.gridPlugins.ShowEditingIcon = false;
            this.gridPlugins.Size = new System.Drawing.Size(325, 184);
            this.gridPlugins.TabIndex = 6;
            this.gridPlugins.TabStop = false;
            // 
            // nameDataGridViewTextBoxColumn
            // 
            this.nameDataGridViewTextBoxColumn.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.nameDataGridViewTextBoxColumn.DataPropertyName = "Name";
            dataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            this.nameDataGridViewTextBoxColumn.DefaultCellStyle = dataGridViewCellStyle2;
            this.nameDataGridViewTextBoxColumn.FillWeight = 175F;
            this.nameDataGridViewTextBoxColumn.HeaderText = "Name";
            this.nameDataGridViewTextBoxColumn.Name = "nameDataGridViewTextBoxColumn";
            this.nameDataGridViewTextBoxColumn.ReadOnly = true;
            this.nameDataGridViewTextBoxColumn.ToolTipText = "Plugin Name";
            // 
            // friendlyNameDataGridViewTextBoxColumn
            // 
            this.friendlyNameDataGridViewTextBoxColumn.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.friendlyNameDataGridViewTextBoxColumn.DataPropertyName = "FriendlyName";
            dataGridViewCellStyle3.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            this.friendlyNameDataGridViewTextBoxColumn.DefaultCellStyle = dataGridViewCellStyle3;
            this.friendlyNameDataGridViewTextBoxColumn.FillWeight = 200F;
            this.friendlyNameDataGridViewTextBoxColumn.HeaderText = "Description";
            this.friendlyNameDataGridViewTextBoxColumn.Name = "friendlyNameDataGridViewTextBoxColumn";
            this.friendlyNameDataGridViewTextBoxColumn.ReadOnly = true;
            this.friendlyNameDataGridViewTextBoxColumn.ToolTipText = "Plugin Description";
            // 
            // versionDataGridViewTextBoxColumn
            // 
            this.versionDataGridViewTextBoxColumn.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.versionDataGridViewTextBoxColumn.DataPropertyName = "Version";
            dataGridViewCellStyle4.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            this.versionDataGridViewTextBoxColumn.DefaultCellStyle = dataGridViewCellStyle4;
            this.versionDataGridViewTextBoxColumn.FillWeight = 75F;
            this.versionDataGridViewTextBoxColumn.HeaderText = "Version";
            this.versionDataGridViewTextBoxColumn.Name = "versionDataGridViewTextBoxColumn";
            this.versionDataGridViewTextBoxColumn.ReadOnly = true;
            this.versionDataGridViewTextBoxColumn.ToolTipText = "Plugin Assembly Version";
            // 
            // InterfaceVersion
            // 
            this.InterfaceVersion.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.InterfaceVersion.DataPropertyName = "InterfaceVersion";
            dataGridViewCellStyle5.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            this.InterfaceVersion.DefaultCellStyle = dataGridViewCellStyle5;
            this.InterfaceVersion.FillWeight = 40F;
            this.InterfaceVersion.HeaderText = "IF";
            this.InterfaceVersion.Name = "InterfaceVersion";
            this.InterfaceVersion.ReadOnly = true;
            this.InterfaceVersion.ToolTipText = "Plugin Interface Version";
            // 
            // iJSRDotNETLibMetadataBindingSource1
            // 
            this.iJSRDotNETLibMetadataBindingSource1.DataSource = typeof(JSRDotNETSDK.IJSRDotNETLibMetadata);
            // 
            // connectionTypeBindingSource
            // 
            this.connectionTypeBindingSource.DataMember = "ConnectionType";
            this.connectionTypeBindingSource.DataSource = this.iJSRDotNETLibMetadataBindingSource;
            // 
            // iJSRDotNETLibMetadataBindingSource
            // 
            this.iJSRDotNETLibMetadataBindingSource.DataSource = typeof(JSRDotNETSDK.IJSRDotNETLibMetadata);
            // 
            // lblPlugins
            // 
            this.lblPlugins.AutoSize = true;
            this.lblPlugins.Location = new System.Drawing.Point(9, 53);
            this.lblPlugins.Name = "lblPlugins";
            this.lblPlugins.Size = new System.Drawing.Size(83, 13);
            this.lblPlugins.TabIndex = 7;
            this.lblPlugins.Text = "Loaded Plugins:";
            // 
            // FormAbout
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnAccept;
            this.ClientSize = new System.Drawing.Size(349, 298);
            this.Controls.Add(this.lblPlugins);
            this.Controls.Add(this.gridPlugins);
            this.Controls.Add(this.btnAccept);
            this.Controls.Add(this.lblVersion);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "FormAbout";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "About JSR .Net Multi Form Example";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.FormAbout_FormClosing);
            this.Load += new System.EventHandler(this.FormAbout_Load);
            ((System.ComponentModel.ISupportInitialize)(this.gridPlugins)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.iJSRDotNETLibMetadataBindingSource1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.connectionTypeBindingSource)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.iJSRDotNETLibMetadataBindingSource)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label lblVersion;
        private System.Windows.Forms.Button btnAccept;
        private System.Windows.Forms.DataGridView gridPlugins;
        private System.Windows.Forms.BindingSource iJSRDotNETLibMetadataBindingSource;
        private System.Windows.Forms.BindingSource connectionTypeBindingSource;
        private System.Windows.Forms.Label lblPlugins;
        private System.Windows.Forms.BindingSource iJSRDotNETLibMetadataBindingSource1;
        private System.Windows.Forms.DataGridViewTextBoxColumn nameDataGridViewTextBoxColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn friendlyNameDataGridViewTextBoxColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn versionDataGridViewTextBoxColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn InterfaceVersion;
    }
}