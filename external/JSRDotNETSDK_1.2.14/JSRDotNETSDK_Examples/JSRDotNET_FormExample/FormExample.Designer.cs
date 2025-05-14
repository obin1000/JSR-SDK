namespace JSRDotNET_FormExample
{
    partial class FormExample
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormExample));
            this.tbModel = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.tbSerialNum = new System.Windows.Forms.TextBox();
            this.ckboxTriggerEnable = new System.Windows.Forms.CheckBox();
            this.imageList1 = new System.Windows.Forms.ImageList(this.components);
            this.cboxGain = new System.Windows.Forms.ComboBox();
            this.label3 = new System.Windows.Forms.Label();
            this.cboxTrigSrc = new System.Windows.Forms.ComboBox();
            this.label4 = new System.Windows.Forms.Label();
            this.listBoxPulserSelect = new JSRDotNET_FormExample.RefreshableListBox();
            this.labelInstrumentList = new System.Windows.Forms.Label();
            this.cbLEDBlink = new System.Windows.Forms.ComboBox();
            this.label5 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // tbModel
            // 
            this.tbModel.Location = new System.Drawing.Point(92, 118);
            this.tbModel.Name = "tbModel";
            this.tbModel.ReadOnly = true;
            this.tbModel.Size = new System.Drawing.Size(112, 20);
            this.tbModel.TabIndex = 0;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(50, 121);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(36, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Model";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(18, 147);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(73, 13);
            this.label2.TabIndex = 3;
            this.label2.Text = "Serial Number";
            this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // tbSerialNum
            // 
            this.tbSerialNum.Location = new System.Drawing.Point(92, 144);
            this.tbSerialNum.Name = "tbSerialNum";
            this.tbSerialNum.ReadOnly = true;
            this.tbSerialNum.Size = new System.Drawing.Size(112, 20);
            this.tbSerialNum.TabIndex = 2;
            // 
            // ckboxTriggerEnable
            // 
            this.ckboxTriggerEnable.AutoSize = true;
            this.ckboxTriggerEnable.Location = new System.Drawing.Point(21, 187);
            this.ckboxTriggerEnable.Name = "ckboxTriggerEnable";
            this.ckboxTriggerEnable.Size = new System.Drawing.Size(95, 17);
            this.ckboxTriggerEnable.TabIndex = 4;
            this.ckboxTriggerEnable.Text = "Trigger Enable";
            this.ckboxTriggerEnable.UseVisualStyleBackColor = true;
            this.ckboxTriggerEnable.CheckedChanged += new System.EventHandler(this.ckboxTriggerEnable_CheckedChanged);
            // 
            // imageList1
            // 
            this.imageList1.ColorDepth = System.Windows.Forms.ColorDepth.Depth8Bit;
            this.imageList1.ImageSize = new System.Drawing.Size(16, 16);
            this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
            // 
            // cboxGain
            // 
            this.cboxGain.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboxGain.FormattingEnabled = true;
            this.cboxGain.Location = new System.Drawing.Point(93, 258);
            this.cboxGain.Name = "cboxGain";
            this.cboxGain.Size = new System.Drawing.Size(111, 21);
            this.cboxGain.TabIndex = 5;
            this.cboxGain.SelectedIndexChanged += new System.EventHandler(this.cboxGain_SelectedIndexChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(58, 261);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(29, 13);
            this.label3.TabIndex = 6;
            this.label3.Text = "Gain";
            // 
            // cboxTrigSrc
            // 
            this.cboxTrigSrc.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboxTrigSrc.FormattingEnabled = true;
            this.cboxTrigSrc.Location = new System.Drawing.Point(93, 222);
            this.cboxTrigSrc.Name = "cboxTrigSrc";
            this.cboxTrigSrc.Size = new System.Drawing.Size(111, 21);
            this.cboxTrigSrc.TabIndex = 7;
            this.cboxTrigSrc.SelectedIndexChanged += new System.EventHandler(this.cboxTrigSrc_SelectedIndexChanged);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(43, 225);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(44, 13);
            this.label4.TabIndex = 8;
            this.label4.Text = "Trig Src";
            // 
            // listBoxPulserSelect
            // 
            this.listBoxPulserSelect.FormattingEnabled = true;
            this.listBoxPulserSelect.Location = new System.Drawing.Point(16, 23);
            this.listBoxPulserSelect.Name = "listBoxPulserSelect";
            this.listBoxPulserSelect.Size = new System.Drawing.Size(258, 82);
            this.listBoxPulserSelect.TabIndex = 9;
            this.listBoxPulserSelect.SelectedIndexChanged += new System.EventHandler(this.listBoxPulserSelect_SelectedIndexChanged);
            // 
            // labelInstrumentList
            // 
            this.labelInstrumentList.AutoSize = true;
            this.labelInstrumentList.Location = new System.Drawing.Point(17, 7);
            this.labelInstrumentList.Name = "labelInstrumentList";
            this.labelInstrumentList.Size = new System.Drawing.Size(136, 13);
            this.labelInstrumentList.TabIndex = 10;
            this.labelInstrumentList.Text = "Instruments ( 0 connected )";
            // 
            // cbLEDBlink
            // 
            this.cbLEDBlink.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbLEDBlink.FormattingEnabled = true;
            this.cbLEDBlink.Location = new System.Drawing.Point(320, 117);
            this.cbLEDBlink.Name = "cbLEDBlink";
            this.cbLEDBlink.Size = new System.Drawing.Size(125, 21);
            this.cbLEDBlink.TabIndex = 11;
            this.cbLEDBlink.SelectedIndexChanged += new System.EventHandler(this.cbLEDBlink_SelectedIndexChanged);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(260, 120);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(54, 13);
            this.label5.TabIndex = 12;
            this.label5.Text = "LED Blink";
            this.label5.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // FormExample
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(494, 299);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.cbLEDBlink);
            this.Controls.Add(this.labelInstrumentList);
            this.Controls.Add(this.listBoxPulserSelect);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.cboxTrigSrc);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.cboxGain);
            this.Controls.Add(this.ckboxTriggerEnable);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.tbSerialNum);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.tbModel);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "FormExample";
            this.Text = "JSRDotNETSDK Simple Form Example";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox tbModel;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox tbSerialNum;
        private System.Windows.Forms.CheckBox ckboxTriggerEnable;
        private System.Windows.Forms.ImageList imageList1;
        private System.Windows.Forms.ComboBox cboxGain;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ComboBox cboxTrigSrc;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label labelInstrumentList;
        private System.Windows.Forms.ComboBox cbLEDBlink;
        private System.Windows.Forms.Label label5;
        private RefreshableListBox listBoxPulserSelect;
    }
}

