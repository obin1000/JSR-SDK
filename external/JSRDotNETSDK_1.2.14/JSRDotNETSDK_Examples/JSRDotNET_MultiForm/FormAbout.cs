using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Reflection;
using System.Windows.Forms;
using JSRDotNETSDK;

namespace JSRDotNET_MultiForm
{
    public partial class FormAbout : Form
    {
        public bool m_bPreventClose = true;

        public FormAbout()
        {
            InitializeComponent();
        }

        public List<IJSRDotNETLibMetadata> PluginDataList { get; set; }

        private void FormAbout_Load(object sender, EventArgs e)
        {
            string assemblyVersion = Assembly.GetExecutingAssembly().GetName().Version.ToString();
            string assemblyVerShorter = assemblyVersion;
            int idxLastDot = assemblyVersion.LastIndexOf('.');
            if (idxLastDot > 2)
            {
                assemblyVerShorter = assemblyVersion.Remove(idxLastDot);
            }
            lblVersion.Text = "Version   " + assemblyVerShorter;
            BindingSource source = new BindingSource();
            source.DataSource = PluginDataList;
            gridPlugins.DataSource = source;
        }

        private void FormAbout_FormClosing(object sender, FormClosingEventArgs e)
        {
            e.Cancel = m_bPreventClose;
            Hide();
        }

        private void btnAccept_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }
}
