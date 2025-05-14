namespace JSRDotNET_MultiForm
{
    partial class FormPulser
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormPulser));
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.pulserToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.forceDisconnectToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.closeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.tabControl = new System.Windows.Forms.TabControl();
            this.tabPageFrontPanel = new System.Windows.Forms.TabPage();
            this.label18 = new System.Windows.Forms.Label();
            this.lblPulserModel = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.cbBoxTriggerSrc = new System.Windows.Forms.ComboBox();
            this.ckboxTrigEnable = new System.Windows.Forms.CheckBox();
            this.label15 = new System.Windows.Forms.Label();
            this.lblPulserZ = new System.Windows.Forms.Label();
            this.cbBoxTriggerPolarity = new System.Windows.Forms.ComboBox();
            this.label17 = new System.Windows.Forms.Label();
            this.label14 = new System.Windows.Forms.Label();
            this.cbBoxPulserZ = new System.Windows.Forms.ComboBox();
            this.cbBoxTrigZ = new System.Windows.Forms.ComboBox();
            this.tbTrackPRF = new System.Windows.Forms.TextBox();
            this.label13 = new System.Windows.Forms.Label();
            this.cbBoxPRF = new System.Windows.Forms.ComboBox();
            this.cbBoxDamping = new System.Windows.Forms.ComboBox();
            this.label16 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.cbBoxHVSupply = new System.Windows.Forms.ComboBox();
            this.tbEnergyPerPulse = new System.Windows.Forms.TextBox();
            this.ckBoxHVEnable = new System.Windows.Forms.CheckBox();
            this.label10 = new System.Windows.Forms.Label();
            this.cbBoxEnergy = new System.Windows.Forms.ComboBox();
            this.label11 = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.tbMaxFrequency = new System.Windows.Forms.TextBox();
            this.label9 = new System.Windows.Forms.Label();
            this.cbBoxRcvrMode = new System.Windows.Forms.ComboBox();
            this.label8 = new System.Windows.Forms.Label();
            this.cbBoxGain = new System.Windows.Forms.ComboBox();
            this.label7 = new System.Windows.Forms.Label();
            this.cbBoxLPFilter = new System.Windows.Forms.ComboBox();
            this.cbBoxHPFilter = new System.Windows.Forms.ComboBox();
            this.label6 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.cbLEDBlink = new System.Windows.Forms.ComboBox();
            this.tbChannel = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.tbSerial = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.tbModel = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.tabPagePropList = new System.Windows.Forms.TabPage();
            this.listViewProperties = new System.Windows.Forms.ListView();
            this.colPropName = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.colValue = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.colIndex = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.butSetPRF = new System.Windows.Forms.Button();
            this.menuStrip1.SuspendLayout();
            this.tabControl.SuspendLayout();
            this.tabPageFrontPanel.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.tabPagePropList.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.pulserToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(374, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            this.menuStrip1.Visible = false;
            // 
            // pulserToolStripMenuItem
            // 
            this.pulserToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.forceDisconnectToolStripMenuItem,
            this.closeToolStripMenuItem});
            this.pulserToolStripMenuItem.MergeAction = System.Windows.Forms.MergeAction.Insert;
            this.pulserToolStripMenuItem.MergeIndex = 1;
            this.pulserToolStripMenuItem.Name = "pulserToolStripMenuItem";
            this.pulserToolStripMenuItem.Size = new System.Drawing.Size(51, 20);
            this.pulserToolStripMenuItem.Text = "&Pulser";
            // 
            // forceDisconnectToolStripMenuItem
            // 
            this.forceDisconnectToolStripMenuItem.Name = "forceDisconnectToolStripMenuItem";
            this.forceDisconnectToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.D)));
            this.forceDisconnectToolStripMenuItem.Size = new System.Drawing.Size(207, 22);
            this.forceDisconnectToolStripMenuItem.Text = "Force &Disconnect";
            this.forceDisconnectToolStripMenuItem.Click += new System.EventHandler(this.forceDisconnectToolStripMenuItem_Click);
            // 
            // closeToolStripMenuItem
            // 
            this.closeToolStripMenuItem.Name = "closeToolStripMenuItem";
            this.closeToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.W)));
            this.closeToolStripMenuItem.Size = new System.Drawing.Size(207, 22);
            this.closeToolStripMenuItem.Text = "&Close";
            this.closeToolStripMenuItem.Click += new System.EventHandler(this.closeToolStripMenuItem_Click);
            // 
            // statusStrip1
            // 
            this.statusStrip1.Location = new System.Drawing.Point(0, 403);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(374, 22);
            this.statusStrip1.TabIndex = 1;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // tabControl
            // 
            this.tabControl.Controls.Add(this.tabPageFrontPanel);
            this.tabControl.Controls.Add(this.tabPagePropList);
            this.tabControl.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl.Location = new System.Drawing.Point(0, 0);
            this.tabControl.Name = "tabControl";
            this.tabControl.SelectedIndex = 0;
            this.tabControl.Size = new System.Drawing.Size(374, 403);
            this.tabControl.TabIndex = 2;
            this.tabControl.SelectedIndexChanged += new System.EventHandler(this.tabControl_SelectedIndexChanged);
            // 
            // tabPageFrontPanel
            // 
            this.tabPageFrontPanel.Controls.Add(this.label18);
            this.tabPageFrontPanel.Controls.Add(this.lblPulserModel);
            this.tabPageFrontPanel.Controls.Add(this.groupBox2);
            this.tabPageFrontPanel.Controls.Add(this.groupBox1);
            this.tabPageFrontPanel.Controls.Add(this.label4);
            this.tabPageFrontPanel.Controls.Add(this.cbLEDBlink);
            this.tabPageFrontPanel.Controls.Add(this.tbChannel);
            this.tabPageFrontPanel.Controls.Add(this.label3);
            this.tabPageFrontPanel.Controls.Add(this.tbSerial);
            this.tabPageFrontPanel.Controls.Add(this.label2);
            this.tabPageFrontPanel.Controls.Add(this.tbModel);
            this.tabPageFrontPanel.Controls.Add(this.label1);
            this.tabPageFrontPanel.Location = new System.Drawing.Point(4, 22);
            this.tabPageFrontPanel.Name = "tabPageFrontPanel";
            this.tabPageFrontPanel.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageFrontPanel.Size = new System.Drawing.Size(366, 377);
            this.tabPageFrontPanel.TabIndex = 0;
            this.tabPageFrontPanel.Text = "FrontPanel";
            this.tabPageFrontPanel.UseVisualStyleBackColor = true;
            this.tabPageFrontPanel.Click += new System.EventHandler(this.tabPageFrontPanel_Click);
            // 
            // label18
            // 
            this.label18.AutoSize = true;
            this.label18.Location = new System.Drawing.Point(5, 113);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(68, 13);
            this.label18.TabIndex = 61;
            this.label18.Text = "Pulser Model";
            // 
            // lblPulserModel
            // 
            this.lblPulserModel.AutoSize = true;
            this.lblPulserModel.Location = new System.Drawing.Point(85, 113);
            this.lblPulserModel.Name = "lblPulserModel";
            this.lblPulserModel.Size = new System.Drawing.Size(67, 13);
            this.lblPulserModel.TabIndex = 60;
            this.lblPulserModel.Text = "Type: RP-00";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.butSetPRF);
            this.groupBox2.Controls.Add(this.cbBoxTriggerSrc);
            this.groupBox2.Controls.Add(this.ckboxTrigEnable);
            this.groupBox2.Controls.Add(this.label15);
            this.groupBox2.Controls.Add(this.lblPulserZ);
            this.groupBox2.Controls.Add(this.cbBoxTriggerPolarity);
            this.groupBox2.Controls.Add(this.label17);
            this.groupBox2.Controls.Add(this.label14);
            this.groupBox2.Controls.Add(this.cbBoxPulserZ);
            this.groupBox2.Controls.Add(this.cbBoxTrigZ);
            this.groupBox2.Controls.Add(this.tbTrackPRF);
            this.groupBox2.Controls.Add(this.label13);
            this.groupBox2.Controls.Add(this.cbBoxPRF);
            this.groupBox2.Controls.Add(this.cbBoxDamping);
            this.groupBox2.Controls.Add(this.label16);
            this.groupBox2.Controls.Add(this.label12);
            this.groupBox2.Controls.Add(this.cbBoxHVSupply);
            this.groupBox2.Controls.Add(this.tbEnergyPerPulse);
            this.groupBox2.Controls.Add(this.ckBoxHVEnable);
            this.groupBox2.Controls.Add(this.label10);
            this.groupBox2.Controls.Add(this.cbBoxEnergy);
            this.groupBox2.Controls.Add(this.label11);
            this.groupBox2.Location = new System.Drawing.Point(173, 6);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(190, 367);
            this.groupBox2.TabIndex = 59;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Pulser";
            // 
            // cbBoxTriggerSrc
            // 
            this.cbBoxTriggerSrc.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbBoxTriggerSrc.FormattingEnabled = true;
            this.cbBoxTriggerSrc.Location = new System.Drawing.Point(83, 41);
            this.cbBoxTriggerSrc.Name = "cbBoxTriggerSrc";
            this.cbBoxTriggerSrc.Size = new System.Drawing.Size(99, 21);
            this.cbBoxTriggerSrc.TabIndex = 37;
            this.cbBoxTriggerSrc.SelectedIndexChanged += new System.EventHandler(this.cbBoxTriggerSrc_SelectedIndexChanged);
            // 
            // ckboxTrigEnable
            // 
            this.ckboxTrigEnable.AutoSize = true;
            this.ckboxTrigEnable.Location = new System.Drawing.Point(83, 18);
            this.ckboxTrigEnable.Name = "ckboxTrigEnable";
            this.ckboxTrigEnable.Size = new System.Drawing.Size(95, 17);
            this.ckboxTrigEnable.TabIndex = 36;
            this.ckboxTrigEnable.Text = "Trigger Enable";
            this.ckboxTrigEnable.UseVisualStyleBackColor = true;
            this.ckboxTrigEnable.CheckedChanged += new System.EventHandler(this.ckboxTrigEnable_CheckedChanged);
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(28, 341);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(53, 13);
            this.label15.TabIndex = 53;
            this.label15.Text = "Trig Edge";
            // 
            // lblPulserZ
            // 
            this.lblPulserZ.AutoSize = true;
            this.lblPulserZ.Location = new System.Drawing.Point(34, 230);
            this.lblPulserZ.Name = "lblPulserZ";
            this.lblPulserZ.Size = new System.Drawing.Size(43, 13);
            this.lblPulserZ.TabIndex = 57;
            this.lblPulserZ.Text = "Pulse Z";
            this.lblPulserZ.Visible = false;
            // 
            // cbBoxTriggerPolarity
            // 
            this.cbBoxTriggerPolarity.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbBoxTriggerPolarity.FormattingEnabled = true;
            this.cbBoxTriggerPolarity.Location = new System.Drawing.Point(83, 338);
            this.cbBoxTriggerPolarity.Name = "cbBoxTriggerPolarity";
            this.cbBoxTriggerPolarity.Size = new System.Drawing.Size(99, 21);
            this.cbBoxTriggerPolarity.TabIndex = 52;
            this.cbBoxTriggerPolarity.SelectedIndexChanged += new System.EventHandler(this.cbBoxTriggerPolarity_SelectedIndexChanged);
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Location = new System.Drawing.Point(33, 44);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(44, 13);
            this.label17.TabIndex = 38;
            this.label17.Text = "Trig Src";
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(42, 313);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(35, 13);
            this.label14.TabIndex = 50;
            this.label14.Text = "Trig Z";
            // 
            // cbBoxPulserZ
            // 
            this.cbBoxPulserZ.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbBoxPulserZ.FormattingEnabled = true;
            this.cbBoxPulserZ.Items.AddRange(new object[] {
            "High-Z",
            "Low-Z"});
            this.cbBoxPulserZ.Location = new System.Drawing.Point(83, 227);
            this.cbBoxPulserZ.Name = "cbBoxPulserZ";
            this.cbBoxPulserZ.Size = new System.Drawing.Size(99, 21);
            this.cbBoxPulserZ.TabIndex = 56;
            this.cbBoxPulserZ.Visible = false;
            this.cbBoxPulserZ.SelectedIndexChanged += new System.EventHandler(this.cbBoxPulserZ_SelectedIndexChanged);
            // 
            // cbBoxTrigZ
            // 
            this.cbBoxTrigZ.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbBoxTrigZ.FormattingEnabled = true;
            this.cbBoxTrigZ.Location = new System.Drawing.Point(83, 310);
            this.cbBoxTrigZ.Name = "cbBoxTrigZ";
            this.cbBoxTrigZ.Size = new System.Drawing.Size(99, 21);
            this.cbBoxTrigZ.TabIndex = 49;
            this.cbBoxTrigZ.SelectedIndexChanged += new System.EventHandler(this.cbBoxTrigZ_SelectedIndexChanged);
            // 
            // tbTrackPRF
            // 
            this.tbTrackPRF.Location = new System.Drawing.Point(8, 109);
            this.tbTrackPRF.Name = "tbTrackPRF";
            this.tbTrackPRF.Size = new System.Drawing.Size(94, 20);
            this.tbTrackPRF.TabIndex = 55;
            this.tbTrackPRF.TextChanged += new System.EventHandler(this.tbTrackPRF_TextChanged);
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(28, 285);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(49, 13);
            this.label13.TabIndex = 48;
            this.label13.Text = "Damping";
            // 
            // cbBoxPRF
            // 
            this.cbBoxPRF.FormattingEnabled = true;
            this.cbBoxPRF.Location = new System.Drawing.Point(83, 69);
            this.cbBoxPRF.Name = "cbBoxPRF";
            this.cbBoxPRF.Size = new System.Drawing.Size(99, 21);
            this.cbBoxPRF.TabIndex = 39;
            this.cbBoxPRF.SelectedIndexChanged += new System.EventHandler(this.cbBoxPRF_SelectedIndexChanged);
            this.cbBoxPRF.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.cbBoxPRF_KeyPress);
            // 
            // cbBoxDamping
            // 
            this.cbBoxDamping.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbBoxDamping.FormattingEnabled = true;
            this.cbBoxDamping.Location = new System.Drawing.Point(83, 282);
            this.cbBoxDamping.Name = "cbBoxDamping";
            this.cbBoxDamping.Size = new System.Drawing.Size(99, 21);
            this.cbBoxDamping.TabIndex = 47;
            this.cbBoxDamping.SelectedIndexChanged += new System.EventHandler(this.cbBoxDamping_SelectedIndexChanged);
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Location = new System.Drawing.Point(49, 72);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(28, 13);
            this.label16.TabIndex = 40;
            this.label16.Text = "PRF";
            this.label16.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(5, 258);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(72, 13);
            this.label12.TabIndex = 46;
            this.label12.Text = "Calc\'d Energy";
            this.label12.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // cbBoxHVSupply
            // 
            this.cbBoxHVSupply.FormattingEnabled = true;
            this.cbBoxHVSupply.Location = new System.Drawing.Point(84, 171);
            this.cbBoxHVSupply.Name = "cbBoxHVSupply";
            this.cbBoxHVSupply.Size = new System.Drawing.Size(98, 21);
            this.cbBoxHVSupply.TabIndex = 41;
            this.cbBoxHVSupply.SelectedIndexChanged += new System.EventHandler(this.cbBoxHVSupply_SelectedIndexChanged);
            this.cbBoxHVSupply.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.cbBoxHVSupply_KeyPress);
            // 
            // tbEnergyPerPulse
            // 
            this.tbEnergyPerPulse.Location = new System.Drawing.Point(83, 255);
            this.tbEnergyPerPulse.Name = "tbEnergyPerPulse";
            this.tbEnergyPerPulse.ReadOnly = true;
            this.tbEnergyPerPulse.Size = new System.Drawing.Size(99, 20);
            this.tbEnergyPerPulse.TabIndex = 45;
            // 
            // ckBoxHVEnable
            // 
            this.ckBoxHVEnable.AutoSize = true;
            this.ckBoxHVEnable.Location = new System.Drawing.Point(105, 148);
            this.ckBoxHVEnable.Name = "ckBoxHVEnable";
            this.ckBoxHVEnable.Size = new System.Drawing.Size(77, 17);
            this.ckBoxHVEnable.TabIndex = 51;
            this.ckBoxHVEnable.Text = "HV Enable";
            this.ckBoxHVEnable.UseVisualStyleBackColor = true;
            this.ckBoxHVEnable.CheckedChanged += new System.EventHandler(this.ckBoxHVEnable_CheckedChanged);
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(18, 174);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(59, 13);
            this.label10.TabIndex = 42;
            this.label10.Text = "Pulse Volts";
            this.label10.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // cbBoxEnergy
            // 
            this.cbBoxEnergy.FormattingEnabled = true;
            this.cbBoxEnergy.Location = new System.Drawing.Point(83, 199);
            this.cbBoxEnergy.Name = "cbBoxEnergy";
            this.cbBoxEnergy.Size = new System.Drawing.Size(99, 21);
            this.cbBoxEnergy.TabIndex = 43;
            this.cbBoxEnergy.SelectedIndexChanged += new System.EventHandler(this.cbBoxEnergy_SelectedIndexChanged);
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(34, 202);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(40, 13);
            this.label11.TabIndex = 44;
            this.label11.Text = "Energy";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.tbMaxFrequency);
            this.groupBox1.Controls.Add(this.label9);
            this.groupBox1.Controls.Add(this.cbBoxRcvrMode);
            this.groupBox1.Controls.Add(this.label8);
            this.groupBox1.Controls.Add(this.cbBoxGain);
            this.groupBox1.Controls.Add(this.label7);
            this.groupBox1.Controls.Add(this.cbBoxLPFilter);
            this.groupBox1.Controls.Add(this.cbBoxHPFilter);
            this.groupBox1.Controls.Add(this.label6);
            this.groupBox1.Controls.Add(this.label5);
            this.groupBox1.Location = new System.Drawing.Point(8, 199);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(159, 173);
            this.groupBox1.TabIndex = 58;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Receiver";
            // 
            // tbMaxFrequency
            // 
            this.tbMaxFrequency.Location = new System.Drawing.Point(71, 19);
            this.tbMaxFrequency.Name = "tbMaxFrequency";
            this.tbMaxFrequency.ReadOnly = true;
            this.tbMaxFrequency.Size = new System.Drawing.Size(80, 20);
            this.tbMaxFrequency.TabIndex = 10;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(7, 19);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(51, 13);
            this.label9.TabIndex = 11;
            this.label9.Text = "Max Freq";
            // 
            // cbBoxRcvrMode
            // 
            this.cbBoxRcvrMode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbBoxRcvrMode.FormattingEnabled = true;
            this.cbBoxRcvrMode.Location = new System.Drawing.Point(73, 48);
            this.cbBoxRcvrMode.Name = "cbBoxRcvrMode";
            this.cbBoxRcvrMode.Size = new System.Drawing.Size(78, 21);
            this.cbBoxRcvrMode.TabIndex = 12;
            this.cbBoxRcvrMode.SelectedIndexChanged += new System.EventHandler(this.cbBoxRcvrMode_SelectedIndexChanged);
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(7, 48);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(60, 13);
            this.label8.TabIndex = 13;
            this.label8.Text = "Rcvr Mode";
            // 
            // cbBoxGain
            // 
            this.cbBoxGain.FormattingEnabled = true;
            this.cbBoxGain.Location = new System.Drawing.Point(73, 75);
            this.cbBoxGain.Name = "cbBoxGain";
            this.cbBoxGain.Size = new System.Drawing.Size(78, 21);
            this.cbBoxGain.TabIndex = 14;
            this.cbBoxGain.SelectedIndexChanged += new System.EventHandler(this.cbBoxGain_SelectedIndexChanged);
            this.cbBoxGain.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.cbBoxGain_KeyPress);
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(7, 75);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(29, 13);
            this.label7.TabIndex = 15;
            this.label7.Text = "Gain";
            // 
            // cbBoxLPFilter
            // 
            this.cbBoxLPFilter.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbBoxLPFilter.FormattingEnabled = true;
            this.cbBoxLPFilter.Location = new System.Drawing.Point(73, 102);
            this.cbBoxLPFilter.Name = "cbBoxLPFilter";
            this.cbBoxLPFilter.Size = new System.Drawing.Size(78, 21);
            this.cbBoxLPFilter.TabIndex = 16;
            this.cbBoxLPFilter.SelectedIndexChanged += new System.EventHandler(this.cbBoxLPFilter_SelectedIndexChanged);
            // 
            // cbBoxHPFilter
            // 
            this.cbBoxHPFilter.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbBoxHPFilter.FormattingEnabled = true;
            this.cbBoxHPFilter.Location = new System.Drawing.Point(73, 129);
            this.cbBoxHPFilter.Name = "cbBoxHPFilter";
            this.cbBoxHPFilter.Size = new System.Drawing.Size(78, 21);
            this.cbBoxHPFilter.TabIndex = 17;
            this.cbBoxHPFilter.SelectedIndexChanged += new System.EventHandler(this.cbBoxHPFilter_SelectedIndexChanged);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(7, 102);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(53, 13);
            this.label6.TabIndex = 18;
            this.label6.Text = "Low Pass";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(7, 129);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(55, 13);
            this.label5.TabIndex = 19;
            this.label5.Text = "High Pass";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(15, 144);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(30, 13);
            this.label4.TabIndex = 7;
            this.label4.Text = "Blink";
            // 
            // cbLEDBlink
            // 
            this.cbLEDBlink.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbLEDBlink.FormattingEnabled = true;
            this.cbLEDBlink.Location = new System.Drawing.Point(53, 141);
            this.cbLEDBlink.Name = "cbLEDBlink";
            this.cbLEDBlink.Size = new System.Drawing.Size(114, 21);
            this.cbLEDBlink.TabIndex = 6;
            this.cbLEDBlink.SelectedIndexChanged += new System.EventHandler(this.cbLEDBlink_SelectedIndexChanged);
            // 
            // tbChannel
            // 
            this.tbChannel.Location = new System.Drawing.Point(45, 63);
            this.tbChannel.Name = "tbChannel";
            this.tbChannel.ReadOnly = true;
            this.tbChannel.Size = new System.Drawing.Size(122, 20);
            this.tbChannel.TabIndex = 5;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(7, 66);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(32, 13);
            this.label3.TabIndex = 4;
            this.label3.Text = "Chan";
            // 
            // tbSerial
            // 
            this.tbSerial.Location = new System.Drawing.Point(45, 37);
            this.tbSerial.Name = "tbSerial";
            this.tbSerial.ReadOnly = true;
            this.tbSerial.Size = new System.Drawing.Size(122, 20);
            this.tbSerial.TabIndex = 3;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(6, 40);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(33, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Serial";
            // 
            // tbModel
            // 
            this.tbModel.Location = new System.Drawing.Point(45, 11);
            this.tbModel.Name = "tbModel";
            this.tbModel.ReadOnly = true;
            this.tbModel.Size = new System.Drawing.Size(122, 20);
            this.tbModel.TabIndex = 1;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(3, 14);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(36, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Model";
            // 
            // tabPagePropList
            // 
            this.tabPagePropList.Controls.Add(this.listViewProperties);
            this.tabPagePropList.Location = new System.Drawing.Point(4, 22);
            this.tabPagePropList.Name = "tabPagePropList";
            this.tabPagePropList.Padding = new System.Windows.Forms.Padding(3);
            this.tabPagePropList.Size = new System.Drawing.Size(366, 377);
            this.tabPagePropList.TabIndex = 1;
            this.tabPagePropList.Text = "Properties";
            this.tabPagePropList.UseVisualStyleBackColor = true;
            // 
            // listViewProperties
            // 
            this.listViewProperties.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.colPropName,
            this.colValue,
            this.colIndex});
            this.listViewProperties.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listViewProperties.Location = new System.Drawing.Point(3, 3);
            this.listViewProperties.Name = "listViewProperties";
            this.listViewProperties.Size = new System.Drawing.Size(360, 371);
            this.listViewProperties.TabIndex = 0;
            this.listViewProperties.UseCompatibleStateImageBehavior = false;
            this.listViewProperties.View = System.Windows.Forms.View.Details;
            // 
            // colPropName
            // 
            this.colPropName.Text = "Property Name";
            this.colPropName.Width = 172;
            // 
            // colValue
            // 
            this.colValue.Text = "Value";
            this.colValue.Width = 120;
            // 
            // colIndex
            // 
            this.colIndex.Text = "Index";
            // 
            // butSetPRF
            // 
            this.butSetPRF.Location = new System.Drawing.Point(119, 107);
            this.butSetPRF.Name = "butSetPRF";
            this.butSetPRF.Size = new System.Drawing.Size(63, 23);
            this.butSetPRF.TabIndex = 58;
            this.butSetPRF.Text = "Set PRF";
            this.butSetPRF.UseVisualStyleBackColor = true;
            this.butSetPRF.Click += new System.EventHandler(this.butSetPRF_Click);
            // 
            // FormPulser
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(374, 425);
            this.Controls.Add(this.tabControl);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.menuStrip1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "FormPulser";
            this.Text = "FormPulser";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.FormPulser_FormClosing);
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.FormPulser_FormClosed);
            this.Load += new System.EventHandler(this.FormPulser_Load);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.tabControl.ResumeLayout(false);
            this.tabPageFrontPanel.ResumeLayout(false);
            this.tabPageFrontPanel.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.tabPagePropList.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem pulserToolStripMenuItem;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.TabControl tabControl;
        private System.Windows.Forms.TabPage tabPageFrontPanel;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ComboBox cbLEDBlink;
        private System.Windows.Forms.TextBox tbChannel;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox tbSerial;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox tbModel;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TabPage tabPagePropList;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.ComboBox cbBoxHPFilter;
        private System.Windows.Forms.ComboBox cbBoxLPFilter;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.ComboBox cbBoxGain;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.ComboBox cbBoxRcvrMode;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.TextBox tbMaxFrequency;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label lblPulserZ;
        private System.Windows.Forms.ComboBox cbBoxPulserZ;
        private System.Windows.Forms.TextBox tbTrackPRF;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.ComboBox cbBoxTriggerPolarity;
        private System.Windows.Forms.CheckBox ckBoxHVEnable;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.ComboBox cbBoxTrigZ;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.ComboBox cbBoxDamping;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.TextBox tbEnergyPerPulse;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.ComboBox cbBoxEnergy;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.ComboBox cbBoxHVSupply;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.ComboBox cbBoxPRF;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.ComboBox cbBoxTriggerSrc;
        private System.Windows.Forms.CheckBox ckboxTrigEnable;
        private System.Windows.Forms.Label label18;
        private System.Windows.Forms.Label lblPulserModel;
        private System.Windows.Forms.ToolStripMenuItem forceDisconnectToolStripMenuItem;
        private System.Windows.Forms.ListView listViewProperties;
        private System.Windows.Forms.ColumnHeader colPropName;
        private System.Windows.Forms.ColumnHeader colValue;
        private System.Windows.Forms.ColumnHeader colIndex;
        private System.Windows.Forms.ToolStripMenuItem closeToolStripMenuItem;
        private System.Windows.Forms.Button butSetPRF;
    }
}