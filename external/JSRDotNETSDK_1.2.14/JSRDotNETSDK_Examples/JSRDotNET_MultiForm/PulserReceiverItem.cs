using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Printing;
using System.Linq;
using System.Text;
using System.IO;
using System.Windows.Forms;
using System.Reflection;
using System.Threading;
using Microsoft.Win32;
using JSRDotNETSDK;

namespace JSRDotNET_MultiForm
{

    /// <summary>
    /// A class for the objects that populate the pulser receiver select box.
    /// </summary>
    public class PulserReceiverItem
    {
        string m_model;
        string m_serial;
        int m_prIndex;
        string m_connectionInfo;
        string m_strBandwidth;
        string m_pulserModel;

        public string Model { get { return m_model; } }
        public string Serial { get { return m_serial; } }
        public int PRIndex { get { return m_prIndex; } }
        public string ConnectionInfo
        {
            get { return m_connectionInfo; }
            set { if (null == value) value = string.Empty; m_connectionInfo = value; }
        }
        public string Bandwidth
        {
            get { return m_strBandwidth; }
            set { if (null == value) value = string.Empty; m_strBandwidth = value; }
        }
        public string PulserModel
        {
            get { return m_pulserModel; }
            set { if (null == value) value = string.Empty; m_pulserModel = value; }
        }

        public bool isMatch(string model, string serial, int prindex)
        {
            return (model == Model) && (serial == Serial) && (prindex == PRIndex);
        }

        public PulserReceiverItem(
            string model, string serial, int prIndex,
            string connectionInfo, string strBandwidth, string pulserModel)
        {
            m_model = model;
            m_serial = serial;
            m_prIndex = prIndex;
            if (null == connectionInfo) connectionInfo = string.Empty;
            if (null == strBandwidth) strBandwidth = string.Empty;
            if (null == pulserModel) pulserModel = string.Empty;
            m_connectionInfo = connectionInfo;
            m_strBandwidth = strBandwidth;
            m_pulserModel = pulserModel;
        }

        public override string ToString()
        {
            string sret;
            if ((0 < m_connectionInfo.Length) && (0 < m_strBandwidth.Length) && (0 < m_pulserModel.Length))
            {
                sret = string.Format("{0}, {1}, {2}, {3}", m_model, m_connectionInfo, m_strBandwidth, m_pulserModel);
            }
            else
            {
                sret = string.Format("{0}, {1}, {2}", m_model, m_serial, m_prIndex);
            }

            return sret;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Gets information describing the model, if it is a DPR500, the pulser model is appended.</summary>
        ///
        /// <value>Information describing the model.</value>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        public string ModelDescription
        {
            get
            {
                string strret = m_model;
                if (m_model.ToUpper().StartsWith("DPR500"))
                {
                    strret += " " + m_pulserModel;
                }
                return strret;
            }
        }

    }

}
