using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using JSRDotNETSDK;

namespace JSRDotNET_MultiForm
{
    public partial class FormPulser : Form
    {
        /* Constants relevant to custom DPR-specific properties. */
        const string PRPropertyNamePulserModel = "PulserModel";
        const string PRPropertyNameReceiverModel = "ReceiverModel";
        const string PRPropertyNameIsPulserPresent = "IsPulserPresent";

        static int[] ObjectArrayToIntArray(object[] oarr)
        {
            int[] iarr = new int[oarr.Length];
            int i;
            for (i = 0; i < oarr.Length; i++) { iarr[i] = (int)oarr[i]; }
            return iarr;
        }

        static string[] ObjectArrayToStringArray(object[] oarr)
        {
            string[] sarr = new string[oarr.Length];
            int i;
            for (i = 0; i < oarr.Length; i++) { sarr[i] = oarr[i].ToString(); }
            return sarr;
        }

        static double[] ObjectArrayToDoubleArray(object[] oarr)
        {
            double[] iarr = new double[oarr.Length];
            int i;
            for (i = 0; i < oarr.Length; i++) { iarr[i] = (double)oarr[i]; }
            return iarr;
        }

        public class DoubleCBItem
        {
            double m_value;
            public double Value { get { return m_value; } }
            int m_idx;
            public int Index { get { return m_idx; } set { m_idx = value; } }
            bool m_bSelected;
            public bool Selected { get { return m_bSelected; } set { m_bSelected = value; } }
            int m_precision;
            public int Precision { get { return m_precision; } set { m_precision = value; } }

            public DoubleCBItem(int idx, double value, int precision = 1)
            {
                m_idx = idx;
                m_value = value;
                m_precision = precision;
                m_bSelected = false;
            }
            public override string ToString()
            {
                string format = "F" + m_precision;
                return m_value.ToString(format);
            }
        }

        PulserReceiverItem m_pulserItem;
        IPulserReceiverIdentity m_pulserId;

        IPulserReceiver m_pulser;

        bool m_bPulserOffline = false;

        AutoResetEvent m_evtDetach;

        /// <summary>
        /// When true, disables the changing of pulser property values due to combobox
        /// changes. This is to stop unnecessary setting of properties during initialization
        /// of the comboboxes.
        /// </summary>
        bool m_bIgnoreSelChanges = false;

        bool m_bFormClosing = false;




        private bool m_bTbar_useN = false;
        private double m_cache_minprf = 0.0;
        private double m_cache_maxprf = 0.0;
        private int m_cache_prfNumerator = 0;
        private int m_cache_prf_nmin = 0;
        private int m_cache_prf_nmax = 0;
        private int m_cache_nPrf_values = 0;
        private double[] m_cache_fArr = null;
        private double m_lastKnownPRF = 0.0;
        private bool m_bLastKnownPRFValid = false;



        private void makeSurePRFTextboxIsUpToDateWithTrackbarPos()
        {
            // TODO:  kds 22Mar2016  This needs work.  we have to figure out if the value in the text box was put
            // there by the slider moving, or by the user typing in something.  Otherwise, the slider messages 
            // getting missed and the textbox and the slider are out of sync.
#if false 
            if (!tbTrackPRF.Focused)
            {
                double trackingPRF = 0.0;
                int value = trackBarPRF.Value;
                if ((value >= 0) && (value < m_cache_nPrf_values))
                {
                    trackingPRF = m_cache_fArr[value];
                    string strPRF = trackingPRF.ToString("0.0");
                    string lastKnownPRF = m_lastKnownPRF.ToString("0.0");
                    if (strPRF != lastKnownPRF)
                    {
                        int nLastKnown = (int) Math.Round(m_cache_prfNumerator / m_lastKnownPRF);
                        int nTracking = (int)Math.Round(m_cache_prfNumerator / trackingPRF);
                    }
                }
            }
#endif

        }



        /// <summary>
        /// Checks to see if the numeric value for the PRF in the text box matches the last known value
        /// sent to the device.  If not, sends the pulser a new value for PRF.
        /// </summary>
        private void updatePulserPRF()
        {
            if (!m_bIgnoreSelChanges)
            {
                double curPrfFromTextBox = 0.0;
                if (0 < tbTrackPRF.Text.Length) // To stop red warning toast on startup
                {
                    if (double.TryParse(tbTrackPRF.Text, out curPrfFromTextBox))
                    {
                        if (curPrfFromTextBox != m_lastKnownPRF)
                        {
                            if ((curPrfFromTextBox >= m_cache_minprf) && (curPrfFromTextBox <= m_cache_maxprf))
                            {
                                tbTrackPRF.ForeColor = Color.Black;
                                // m_pulser.PulseRepetitionFrequency = curPrfFromTextBox;
                                m_lastKnownPRF = curPrfFromTextBox;
                                m_bLastKnownPRFValid = true;
                            }
                            else
                            {
                                m_bLastKnownPRFValid = false;
                                tbTrackPRF.ForeColor = Color.Red;
                                if (curPrfFromTextBox < m_cache_minprf)
                                {
                                    MakeToast("PRF Value too low: Must be at least: " + m_cache_minprf.ToString("0.0"), MainForm.ToastMessageType.WARNING);
                                }
                                else if (curPrfFromTextBox > m_cache_maxprf)
                                {
                                    MakeToast("PRF Value too high: Must lower than: " + m_cache_maxprf.ToString("0.0"), MainForm.ToastMessageType.WARNING);
                                }
                            }
                        }
                        else
                        {
                            //makeSurePRFTextboxIsUpToDateWithTrackbarPos();
                        }
                    }
                    else
                    {
                        tbTrackPRF.ForeColor = Color.Red;
                        MakeToast("Illegal Text in PRF Text box", MainForm.ToastMessageType.WARNING);
                    }
                }
            }
        }

        public void SetUpPRFTrackbar()
        {
            m_cache_minprf = m_pulser.PulseRepetitionFrequencyMin;
            m_cache_maxprf = m_pulser.PulseRepetitionFrequencyMax;
            m_cache_prfNumerator = m_pulser.PulseRepetitionFrequencyNumerator;
            double curprf = m_pulser.PulseRepetitionFrequency;
#if false
            m_cache_prf_nmin = (int)Math.Ceiling((double)m_cache_prfNumerator / m_cache_maxprf);
            m_cache_prf_nmax = (int)Math.Floor((double)m_cache_prfNumerator / m_cache_minprf);
            m_cache_nPrf_values = 200;

            m_bTbar_useN = (m_cache_prf_nmax - m_cache_prf_nmin) < m_cache_nPrf_values;
            if (m_bTbar_useN) m_cache_nPrf_values = m_cache_nPrf_values - m_cache_prf_nmin;



            int nMinus1 = m_cache_nPrf_values - 1;
            double fLogScale = (double)nMinus1 / (Math.Log(m_cache_maxprf) - Math.Log(m_cache_minprf));
            int i;
            int iClosest = 0;
            double diffClosest = double.MaxValue;
            m_cache_fArr = new double[m_cache_nPrf_values];
            for (i = 0; i < m_cache_nPrf_values; i++)
            {
                double logF = Math.Log(m_cache_minprf) + (i * (Math.Log(m_cache_maxprf) - Math.Log(m_cache_minprf)) / nMinus1);
                double prf = Math.Exp(logF);
                double nFloat = m_cache_prfNumerator / prf;
                int n = (int)Math.Round(nFloat);
                double actualPRF = m_cache_prfNumerator / n;
                double diff = Math.Abs(actualPRF - curprf);
                if (diff < diffClosest)
                {
                    iClosest = i;
                    diffClosest = diff;
                }
                m_cache_fArr[i] = actualPRF;
            }
            tbTrackPRF.Text = m_cache_fArr[iClosest].ToString("0.0");
#endif
            tbTrackPRF.Text = ((int)curprf).ToString();
            double.TryParse(tbTrackPRF.Text, out m_lastKnownPRF);
            tbTrackPRF.ForeColor = Color.Black;
        }




        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Gets or sets the item.</summary>
        ///
        /// <value>The item.</value>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        public PulserReceiverItem Item
        {
            get { return m_pulserItem; }
            set { m_pulserItem = value; }
        }

        public IPulserReceiver PulserReceiver
        {
            get { return m_pulser; }
            set { m_pulser = value; }
        }

        public IPulserReceiverIdentity PulserReceiverId
        {
            get { return m_pulserId; }
            set { m_pulserId = value; }
        }

        public FormPulser()
        {
            m_evtDetach = new AutoResetEvent(false);
            m_bPulserOffline = false;
            m_pulser = null;
            m_pulserId = null;
            m_pulserItem = null;
            InitializeComponent();
        }

        private void tabPageFrontPanel_Click(object sender, EventArgs e)
        {

        }

        void HandlePulserStatusChange(object sender, EventArgsStatusChange args)
        {
            switch (args.ChangeType)
            {
                case STATUS_CHANGE.INSTRUMENT_DISCONNECT:
                    m_bPulserOffline = true;
                    m_evtDetach.Set();
                    HandleDisconnect(args);
                    break;
                case STATUS_CHANGE.OPERATIONAL_FAULT:
                    m_bPulserOffline = true;
                    HandleOperationalFault(args);
                    break;
                case STATUS_CHANGE.PROPERTY_CHANGE:
                    HandlePropertyChange(args);
                    break;
                default:
                    break;
            }
        }

        void HandleOperationalFault(EventArgsStatusChange args)
        {
            if (InvokeRequired)
            {
                BeginInvoke(new Action<EventArgsStatusChange>(HandleOperationalFault), new object[] { args });
            }
            else
            {
                RefreshControlsFromPulser();
                JSRDotNET_Logger.logstrSignificant("Error (" + Item.ToString() + ") : " + args.ErrorMessage);
                JSRDotNET_Logger.logstrSignificant("ErrorCode: " + args.ErrorCode);
                if (null != args.ThrownException) JSRDotNET_Logger.logException(args.ThrownException, Item.ToString());
                MessageBox.Show(args.ErrorMessage, "Error: " + args.ErrorCode);
            }
        }

        void HandleDisconnect(EventArgsStatusChange args)
        {
            if (InvokeRequired)
            {
                BeginInvoke(new Action<EventArgsStatusChange>(HandleDisconnect), new object[] { args });
            }
            else
            {
                Program.MainFormInstance.ChildFormClosing(this);
                Close();
            }
        }


        void setPulserModelLabel(string strPulserModel)
        {
            lblPulserModel.Text = "Type: " + strPulserModel;
        }

        void HandlePropertyChange(EventArgsStatusChange args)
        {
            if (InvokeRequired)
            {
                BeginInvoke(new Action<EventArgsStatusChange>(HandlePropertyChange), new object[] { args });
            }
            else
            {
                switch (args.PulserProperty)
                {
                    case PRPropertyNameIsPulserPresent:
                        m_bPulserOffline = !((bool) args.NewValue);
                        RefreshControlsFromPulser();
                        if (tabControl.SelectedIndex == 1)
                        {
                            FillListview();
                        }
                        break;
                    case PRPropertyNamePulserModel:
                        if (null != args.NewValue)
                        {
                            setPulserModelLabel(args.NewValue.ToString());
                            Item.PulserModel = args.NewValue.ToString();
                            Text = Item.ToString();
                        }
                        break;
                    default:
                        break;
                }
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Enable or Disable all the user-controllable controls on the form except for the pulser
        /// select listbox.</summary>
        ///
        /// <authorAndDate>kds, 5/11/2016.</authorAndDate>
        ///
        /// <param name="b">true to b.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void SetControlsEnable(bool b)
        {
            cbLEDBlink.Enabled = b;
            cbBoxRcvrMode.Enabled = b;
            cbBoxGain.Enabled = b;
            cbBoxHPFilter.Enabled = b;
            cbBoxLPFilter.Enabled = b;
            ckboxTrigEnable.Enabled = b;
            cbBoxTriggerSrc.Enabled = b;
            cbBoxPRF.Enabled = b;
            ckBoxHVEnable.Enabled = b;
            cbBoxHVSupply.Enabled = b;
            cbBoxEnergy.Enabled = b;
            cbBoxPulserZ.Enabled = b;
            cbBoxDamping.Enabled = b;
            cbBoxTrigZ.Enabled = b;
            cbBoxTriggerPolarity.Enabled = b;
        }


        void fillComboBoxUsingStepsize(ComboBox cb, double dmin, double dmax, double curValue, double stepsize=1.0)
        {
            double[] values;
            int idxSmallestDiff = -1;
            cb.SelectedIndex = -1;
            cb.Items.Clear();
            if (dmin != dmax)
            {
                /* First find rounded prf */
                int minprf = (int)(Math.Ceiling(dmin / stepsize));
                int maxprf = (int)(Math.Floor(dmax/stepsize));
                int i;
                List<double> list = new List<double>();

                if ((double)(stepsize*minprf) != dmin) list.Add(dmin);
                for (i = minprf; i <= maxprf; i++)
                {
                    list.Add((double)i*stepsize);
                }
                if ((double)(stepsize*maxprf) != dmax) list.Add(dmax);


                values = list.ToArray();
                idxSmallestDiff = -1;
                double smallestDiff = double.MaxValue;
                double prf;
                double diff;
                for (i = 0; i < values.Length; i++)
                {
                    prf = values[i];
                    diff = prf - curValue;
                    if (diff < 0.0) diff = -diff;
                    if (diff < smallestDiff)
                    {
                        idxSmallestDiff = i;
                        smallestDiff = diff;
                    }
                }
            }
            else
            {
                idxSmallestDiff = 0;
                values = new double[] { dmin };
            }
            fillComboBox(cb, values, true, idxSmallestDiff);
        }

        void fillComboBoxUsingStepsizeTen(ComboBox cb, double dmin, double dmax, double curValue)
        {
            double[] values;
            int idxSmallestDiff = -1;
            cb.SelectedIndex = -1;
            cb.Items.Clear();
            if (dmin != dmax)
            {
                /* First find rounded prf */
                int minprf = 10 * ((int)Math.Ceiling(dmin / 10.0));
                int maxprf = 10 * ((int)Math.Floor(dmax / 10.0));
                int i;
                List<double> list = new List<double>();

                if ((double)minprf != dmin) list.Add(dmin);
                for (i = minprf; i <= maxprf; i += 10)
                {
                    list.Add((double)i);
                }
                if ((double)maxprf != dmax) list.Add(dmax);


                values = list.ToArray();
                idxSmallestDiff = -1;
                double smallestDiff = double.MaxValue;
                double prf;
                double diff;
                for (i = 0; i < values.Length; i++)
                {
                    prf = values[i];
                    diff = prf - curValue;
                    if (diff < 0.0) diff = -diff;
                    if (diff < smallestDiff)
                    {
                        idxSmallestDiff = i;
                        smallestDiff = diff;
                    }
                }
            }
            else
            {
                idxSmallestDiff = 0;
                values = new double[] { dmin };
            }
            fillComboBox(cb, values, true, idxSmallestDiff);
        }

        void fillComboBox(ComboBox cb, double[] values, bool bReverse, int curIndex, int precision = 1)
        {
            int i;
            DoubleCBItem item;
            int idxCb = 0;
            int selectedIndex = -1;
            cb.Items.Clear();
            if (!bReverse)
            {
                for (i = 0; i < values.Length; i++)
                {
                    item = new DoubleCBItem(i, values[i], precision);
                    idxCb = cb.Items.Add(item);
                    if (i == curIndex) selectedIndex = idxCb;
                }
            }
            else
            {
                for (i = values.Length - 1; i >= 0; i--)
                {
                    item = new DoubleCBItem(i, values[i], precision);
                    idxCb = cb.Items.Add(item);
                    if (i == curIndex) selectedIndex = idxCb;
                }
            }
            if (selectedIndex > -1)
            {
                cb.SelectedIndex = selectedIndex;
                setSelectedFlagInItem(cb, cb.SelectedItem as DoubleCBItem);
            }
        }

        void clearSelectedFlagsInItems(ComboBox cb)
        {
            if (cb.Items.Count > 0)
            {
                foreach (object oItem in cb.Items)
                {
                    if (oItem is DoubleCBItem)
                    {
                        DoubleCBItem item = (DoubleCBItem)oItem;
                        item.Selected = false;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }

        void setSelectedFlagInItem(ComboBox cb, DoubleCBItem itemToSelect)
        {
            if (cb.Items.Count > 0)
            {
                foreach (object oItem in cb.Items)
                {
                    if (oItem is DoubleCBItem)
                    {
                        DoubleCBItem item = (DoubleCBItem)oItem;
                        item.Selected = item.Index == itemToSelect.Index;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }

        DoubleCBItem getLastSelectedItem(ComboBox cb)
        {
            DoubleCBItem itemRet = null;
            if (cb.Items.Count > 0)
            {
                foreach (object oItem in cb.Items)
                {
                    if (oItem is DoubleCBItem)
                    {
                        DoubleCBItem item = (DoubleCBItem)oItem;
                        if (item.Selected)
                        {
                            itemRet = item;
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }
            return itemRet;
        }

        void fillComboBoxUsingMinAndMax(ComboBox cb, double dmin, double dmax, double curValue, double stepsize=1.0)
        {
            double[] values;
            int idxSmallestDiff = -1;
            cb.SelectedIndex = -1;
            cb.Items.Clear();
            if (dmin != dmax)
            {
                if (300.0 >= (dmax - dmin))
                {
                    fillComboBoxUsingStepsize(cb, dmin, dmax, curValue, stepsize);
                }
                else if (3000.0 >= (dmax - dmin))
                {
                    fillComboBoxUsingStepsizeTen(cb, dmin, dmax, curValue);
                }
                else
                {
                    fillComboBoxUsingMinMaxWideRange(cb, dmin, dmax, curValue);
                }
            }
            else
            {
                idxSmallestDiff = 0;
                values = new double[] { dmin };
                fillComboBox(cb, values, true, idxSmallestDiff);
            }
        }



        public readonly double[] g_rangeMultiplier = { 1.0, 1.25, 1.5, 2.0, 2.5, 3.0, 4.0, 5.0, 6.5, 8.0 };

        void fillComboBoxUsingMinMaxWideRange(ComboBox cb, double dmin, double dmax, double curValue)
        {
            double[] values;
            int idxSmallestDiff = -1;
            cb.SelectedIndex = -1;
            cb.Items.Clear();
            if (dmin != dmax)
            {
                /* First find rounded prf */
                int minprf = (int)dmin;
                if (minprf < 1) minprf = 1;
                int nDiv = 0;
                while ((minprf / 10) > 1)
                {
                    nDiv++;
                    minprf = minprf / 10;
                }
                double dminprf = 1.0;
                int i;
                for (i = 0; i < nDiv; i++)
                {
                    minprf *= 10;
                }
                dminprf = (double)minprf;
                double diff;
                double smallestDiff = double.MaxValue;

                List<double> list = new List<double>();
                i = 1;
                int iMod10 = 1;
                int iDecadeMultiplier = 1;
                double prf = dminprf;
                double prevprf = dminprf;
                while (prf <= dmax)
                {
                    if (prf >= dmin)
                    {
                        if (prf > dmin && prevprf < dmin)
                        {
                            list.Add(dmin);
                        }
                        list.Add(prf);
                    }
                    prevprf = prf;
                    prf = dminprf * iDecadeMultiplier * g_rangeMultiplier[iMod10];


                    i++;
                    iMod10 = i % 10;
                    if (0 == iMod10)
                    {
                        iDecadeMultiplier *= 10;
                    }
                }
                if (prevprf < dmax)
                {
                    list.Add(dmax);
                }
                values = list.ToArray();
                idxSmallestDiff = -1;
                smallestDiff = double.MaxValue;
                for (i = 0; i < values.Length; i++)
                {
                    prf = values[i];
                    diff = prf - curValue;
                    if (diff < 0.0) diff = -diff;
                    if (diff < smallestDiff)
                    {
                        idxSmallestDiff = i;
                        smallestDiff = diff;
                    }
                }
            }
            else
            {
                idxSmallestDiff = 0;
                values = new double[] { dmin };
            }
            fillComboBox(cb, values, true, idxSmallestDiff);
        }


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>The PulserSettingInfo contains all the information about how a setting is supported, as
        /// well as its current state.   To fill in all the controls,  this method is called repeatedly, each
        /// time with a diffent PulserSettingInfo object until all the controls are filled in.</summary>
        ///
        /// <authorAndDate>kds, 5/10/2016.</authorAndDate>
        ///
        /// <param name="setting">The setting.</param>
        ///
        /// <returns>true if it succeeds, false if it fails.</returns>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        public bool SetControlsFromSettingValue(PulserSettingInfo setting)
        {
            bool bRet = true;
            if ((null != setting) && (null != setting.SettingName))
            {
                int itmp = 0;
                double dtmp = 0.0;
                string[] sarr = null;
                object[] oarr = null;
                string strerr = null;
                int i;
                try
                {
                    switch (setting.SettingName)
                    {
                        case PRPropertyNamePulserModel:
                            string strPulserModel = setting.DirectValue.ToString();
                            if (strPulserModel.Length > 0)
                            {
                                setPulserModelLabel(strPulserModel);
                            }
                            else
                            {
                                setPulserModelLabel(setting.PulserReceiverId.InstrumentId.ModelName);
                            }
                            break;
                        case SettingNames.MaxFrequency:
                            dtmp = (double)setting.DirectValue;
                            tbMaxFrequency.Text = dtmp.ToString();
                            break;
                        case SettingNames.LEDBlinkMode:
                            cbLEDBlink.Items.Clear();
                            if (setting.IsIndexedPresentAndSupported)
                            {
                                itmp = setting.IndexValue;
                                oarr = setting.IndexValues;
                                sarr = ObjectArrayToStringArray(oarr);
                                cbLEDBlink.Enabled = true;
                                for (i = 0; i < sarr.Length; i++)
                                {
                                    cbLEDBlink.Items.Add(sarr[i]);
                                }
                                cbLEDBlink.SelectedIndex = itmp;
                            }
                            else
                            {
                                cbLEDBlink.Enabled = false;
                            }
                            break;
                        case SettingNames.ReceiverMode:
                            cbBoxRcvrMode.Enabled = true;
                            cbBoxRcvrMode.SelectedIndex = -1;
                            cbBoxRcvrMode.Items.Clear();
                            if (setting.IsRoleSupported(PulserPropertyRole.SPECIFICVALUESUPPORTED, 0)) cbBoxRcvrMode.Items.Add(RECEIVER_MODE.ECHO);
                            if (setting.IsRoleSupported(PulserPropertyRole.SPECIFICVALUESUPPORTED, 1)) cbBoxRcvrMode.Items.Add(RECEIVER_MODE.THRU);
                            if (setting.IsRoleSupported(PulserPropertyRole.SPECIFICVALUESUPPORTED, 2)) cbBoxRcvrMode.Items.Add(RECEIVER_MODE.BOTH);
                            cbBoxRcvrMode.SelectedItem = (RECEIVER_MODE)(int)setting.DirectValue;
                            break;
                        case SettingNames.Gain:
                            cbBoxGain.Enabled = true;
                            if (setting.IsIndexedPresentAndSupported)
                            {
                                fillComboBox(cbBoxGain, ObjectArrayToDoubleArray(setting.IndexValues), true, setting.IndexValue);
                            }
                            else if ((bool)setting.GetPropertyValue(PulserPropertyRole.STEPSIZESUPPORTED))
                            {
                                object oStepsize =  setting.GetPropertyValue(PulserPropertyRole.STEPSIZE);
                                double stepsize = (double) oStepsize;
                                fillComboBoxUsingMinAndMax(cbBoxGain, (double)setting.DirectValueMin, (double)setting.DirectValueMax, (double)setting.DirectValue,
                                    stepsize);
                            } else
                            {
                                fillComboBoxUsingMinAndMax(cbBoxGain, (double)setting.DirectValueMin, (double)setting.DirectValueMax, (double)setting.DirectValue);
                            }

                            break;
                        case SettingNames.HighPassFilter:
                            cbBoxHPFilter.Enabled = true;
                            fillComboBox(cbBoxHPFilter, ObjectArrayToDoubleArray(setting.IndexValues), false, setting.IndexValue);
                            break;
                        case SettingNames.LowPassFilter:
                            cbBoxLPFilter.Enabled = true;
                            fillComboBox(cbBoxLPFilter, ObjectArrayToDoubleArray(setting.IndexValues), false, setting.IndexValue);
                            break;
                        case SettingNames.TriggerEnable:
                            ckboxTrigEnable.Enabled = true;
                            ckboxTrigEnable.Checked = (bool)setting.DirectValue;
                            break;
                        case SettingNames.TriggerSource:
                            cbBoxTriggerSrc.Enabled = true;
                            cbBoxTriggerSrc.SelectedIndex = -1;
                            cbBoxTriggerSrc.Items.Clear();
                            cbBoxTriggerSrc.Items.Add(TRIGGER_SOURCE.EXTERNAL);
                            if (setting.IsRoleSupported(PulserPropertyRole.SPECIFICVALUESUPPORTED, 1)) cbBoxTriggerSrc.Items.Add(TRIGGER_SOURCE.INTERNAL);
                            if (setting.IsRoleSupported(PulserPropertyRole.SPECIFICVALUESUPPORTED, 2)) cbBoxTriggerSrc.Items.Add(TRIGGER_SOURCE.SLAVE);
                            cbBoxTriggerSrc.SelectedItem = (TRIGGER_SOURCE)(int)setting.DirectValue;
                            break;
                        case SettingNames.PulseRepetitionFrequency:
                            if (setting.IsRoleSupported(PulserPropertyRole.SPECIFICVALUESUPPORTED, 1))
                            {
                                cbBoxPRF.Enabled = true;
                                if (setting.IsIndexedPresentAndSupported)
                                {
                                    tbTrackPRF.Visible = false;
                                    tbTrackPRF.Enabled = false;
                                    butSetPRF.Visible = butSetPRF.Enabled = false;
                                    cbBoxPRF.Visible = true;
                                    cbBoxPRF.Enabled = true;
                                    fillComboBox(cbBoxPRF, ObjectArrayToDoubleArray(setting.IndexValues), true, setting.IndexValue, 0);
                                }
                                else
                                {
                                    if (!setting.IsNumeratorSupported)
                                    {
                                        tbTrackPRF.Visible = false;
                                        tbTrackPRF.Enabled = false;
                                        butSetPRF.Visible = false;
                                        butSetPRF.Enabled = false;
                                        cbBoxPRF.Visible = true;
                                        cbBoxPRF.Enabled = true;
                                        fillComboBoxUsingMinAndMax(cbBoxPRF, (double)setting.DirectValueMin, (double)setting.DirectValueMax, (double)setting.DirectValue);
                                    }
                                    else
                                    {
                                        tbTrackPRF.Visible = true;
                                        tbTrackPRF.Enabled = true;
                                        butSetPRF.Visible = true;
                                        butSetPRF.Enabled = true;
                                        cbBoxPRF.Visible = false;
                                        cbBoxPRF.Enabled = false;
                                        m_cache_minprf = (double)setting.DirectValueMin;
                                        m_cache_maxprf = (double)setting.DirectValueMax;
                                        m_cache_prfNumerator = (int)setting.GetPropertyValue(PulserPropertyRole.NUMERATOR);
                                        double curprf = (double)setting.DirectValue;
#if false
                                        m_cache_prf_nmin = (int)Math.Ceiling((double)m_cache_prfNumerator / m_cache_maxprf);
                                        m_cache_prf_nmax = (int)Math.Floor((double)m_cache_prfNumerator / m_cache_minprf);
                                        m_cache_nPrf_values = 200;

                                        m_bTbar_useN = (m_cache_prf_nmax - m_cache_prf_nmin) < m_cache_nPrf_values;
                                        if (m_bTbar_useN) m_cache_nPrf_values = m_cache_nPrf_values - m_cache_prf_nmin;

                                        int nMinus1 = m_cache_nPrf_values - 1;
                                        double fLogScale = (double)nMinus1 / (Math.Log(m_cache_maxprf) - Math.Log(m_cache_minprf));
                                        int iClosest = 0;
                                        double diffClosest = double.MaxValue;
                                        m_cache_fArr = new double[m_cache_nPrf_values];
                                        for (i = 0; i < m_cache_nPrf_values; i++)
                                        {
                                            double logF = Math.Log(m_cache_minprf) + (i * (Math.Log(m_cache_maxprf) - Math.Log(m_cache_minprf)) / nMinus1);
                                            double prf = Math.Exp(logF);
                                            double nFloat = m_cache_prfNumerator / prf;
                                            int n = (int)Math.Round(nFloat);
                                            double actualPRF = m_cache_prfNumerator / n;
                                            double diff = Math.Abs(actualPRF - curprf);
                                            if (diff < diffClosest)
                                            {
                                                iClosest = i;
                                                diffClosest = diff;
                                            }
                                            m_cache_fArr[i] = actualPRF;
                                        }
                                        tbTrackPRF.Text = m_cache_fArr[iClosest].ToString("0.0");
#endif
                                        tbTrackPRF.Text = ((int)curprf).ToString();
                                        double.TryParse(tbTrackPRF.Text, out m_lastKnownPRF);
                                        tbTrackPRF.ForeColor = Color.Black;
                                    } // If PRF is not indexed and has a numerator 
                                }
                            }
                            else
                            {
                                cbBoxPRF.Enabled = false;
                            }
                            break;
                        case SettingNames.HVSupplyEnable:
                            ckBoxHVEnable.Checked = true;
                            ckBoxHVEnable.Enabled = setting.IsDirectPresentAndSupported;
                            if (ckBoxHVEnable.Enabled)
                            {
                                ckBoxHVEnable.Checked = (bool)setting.DirectValue;
                            }
                            break;
                        case SettingNames.HVSupply:
                            if (setting.IsDirectPresentAndSupported || setting.IsIndexedPresentAndSupported)
                            {
                                cbBoxHVSupply.Enabled = true;
                                if (setting.IsIndexedPresentAndSupported)
                                {
                                    fillComboBox(cbBoxHVSupply, ObjectArrayToDoubleArray(setting.IndexValues), true, setting.IndexValue, 0);
                                }
                                else
                                {
                                    fillComboBoxUsingMinAndMax(cbBoxHVSupply, (double)setting.DirectValueMin, (double)setting.DirectValueMax, (double)setting.DirectValue);
                                }

                            }
                            else
                            {
                                cbBoxHVSupply.Enabled = false;
                            }
                            break;
                        case SettingNames.PulseEnergy:
                            cbBoxEnergy.SelectedIndex = -1;
                            cbBoxEnergy.Items.Clear();
                            if (setting.IsIndexedPresentAndSupported)
                            {
                                string[] pulseValueNames = ObjectArrayToStringArray(setting.IndexValues);
                                int iPulseCurIndex = setting.IndexValue;
                                cbBoxEnergy.Enabled = true;
                                int selIndex = -1;
                                for (i = 0; i < pulseValueNames.Length; i++)
                                {
                                    int idx = cbBoxEnergy.Items.Add(pulseValueNames[i]);
                                    if (i == iPulseCurIndex)
                                    {
                                        selIndex = idx;
                                    }
                                }
                                cbBoxEnergy.SelectedIndex = selIndex;
                            }
                            else
                            {
                                cbBoxEnergy.Enabled = false;
                            }
                            break;
                        case SettingNames.PulserImpedance:
                            if (setting.IsRoleSupported(PulserPropertyRole.DIRECTSUPPORTED))
                            {
                                lblPulserZ.Visible = true;
                                cbBoxPulserZ.Enabled = true;
                                cbBoxPulserZ.Visible = true;
                                cbBoxPulserZ.SelectedIndex = (((int)setting.DirectValue) == ((int)PULSER_IMPEDANCE.HIGH_Z)) ? 0 : 1;
                            }
                            else
                            {
                                lblPulserZ.Visible = false;
                                cbBoxPulserZ.SelectedIndex = 0;
                                cbBoxPulserZ.Enabled = false;
                                cbBoxPulserZ.Visible = false;
                            }
                            break;
                        case SettingNames.EnergyPerPulse:
                            dtmp = (double)setting.DirectValue;
                            tbEnergyPerPulse.Text = dtmp.ToString("0.0");
                            break;
                        case SettingNames.Damping:
                            cbBoxDamping.SelectedIndex = -1;
                            cbBoxDamping.Items.Clear();
                            if (setting.IsIndexedPresentAndSupported)
                            {
                                double[] dampingValues = ObjectArrayToDoubleArray(setting.IndexValues);
                                int iDampingCurIndex = setting.IndexValue;
                                cbBoxDamping.Enabled = true;
                                int selIndex = -1;
                                for (i = 0; i < dampingValues.Length; i++)
                                {
                                    int idx = cbBoxDamping.Items.Add(dampingValues[i].ToString("F0"));
                                    if (i == iDampingCurIndex)
                                    {
                                        selIndex = idx;
                                    }
                                }
                                cbBoxDamping.SelectedIndex = selIndex;
                            }
                            else
                            {
                                cbBoxDamping.Enabled = false;
                            }
                            break;
                        case SettingNames.TriggerImpedance:
                            cbBoxTrigZ.SelectedIndex = -1;
                            cbBoxTrigZ.Items.Clear();
                            if (setting.IsDirectPresentAndSupported)
                            {
                                cbBoxTrigZ.Enabled = true;
                                cbBoxTrigZ.Items.Add(TRIGGER_IMPEDANCE.LOW_Z);
                                cbBoxTrigZ.Items.Add(TRIGGER_IMPEDANCE.HIGH_Z);
                                if ((int)setting.DirectValue == (int)TRIGGER_IMPEDANCE.LOW_Z) cbBoxTrigZ.SelectedIndex = 0;
                                if ((int)setting.DirectValue == (int)TRIGGER_IMPEDANCE.HIGH_Z) cbBoxTrigZ.SelectedIndex = 1;
                            }
                            else
                            {
                                cbBoxTrigZ.Enabled = false;
                                try
                                {
                                    TRIGGER_IMPEDANCE trigz = (TRIGGER_IMPEDANCE)(int)setting.DirectValue;
                                    cbBoxTrigZ.Items.Add(trigz);
                                }
                                catch (Exception) { }
                            }
                            break;
                        case SettingNames.TriggerEdgePolarity:
                            cbBoxTriggerPolarity.SelectedIndex = -1;
                            cbBoxTriggerPolarity.Items.Clear();
                            if (setting.IsDirectPresentAndSupported)
                            {
                                cbBoxTriggerPolarity.Enabled = true;
                                cbBoxTriggerPolarity.Items.Add(TRIGGER_POLARITY.RISING);
                                cbBoxTriggerPolarity.Items.Add(TRIGGER_POLARITY.FALLING);
                                cbBoxTriggerPolarity.SelectedItem = (TRIGGER_POLARITY)(int)setting.DirectValue;
                            }
                            else
                            {
                                cbBoxTriggerPolarity.Enabled = false;
                            }
                            break;
                        default:
                            /* DO nothing for a setting we know nothing about. */
                            break;
                    } // switch(SettingName)
                } // try
                catch (Exception eee)
                {
                    strerr = "Form1.SetControlsFromSettingValue() exc '" + eee.GetType().ToString() + "' on setting: '" + setting.SettingName + "'.";
                    JSRDotNET_Logger.logException(eee, strerr);
                    MessageBox.Show(strerr, "Error While Filling Controls", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                    bRet = false;  // Let caller know error occurred.
                }
            } // if stuff != null
            return bRet;
        } // end method SetControlsFromSettingValue()


        public void RefreshControlsFromPulser()
        {
            if (InvokeRequired)
            {
                BeginInvoke(new Action(RefreshControlsFromPulser));
            }
            else
            {
                if (!m_bPulserOffline)
                {
                    m_bIgnoreSelChanges = true; // Disable selected index changed from doing things.
                    IEnumerable<PulserSettingInfo> listSettings = m_pulser.PulserSettings;
                    foreach (PulserSettingInfo setting in listSettings)
                    {
                        SetControlsFromSettingValue(setting);
                    }

                    m_bIgnoreSelChanges = false; // Re-enable the selected index changed operation
                }
                else
                {
                    SetControlsEnable(false);
                }
            }
        }

        string ArrayOfIntsToString(int[] arr)
        {
            string strRet = string.Empty;
            int len = arr.Length;
            int i;
            if (len > 4) len = 4;
            if (len > 0) strRet += "[";
            for (i = 0; i < len; i++)
            {
                strRet += arr[i].ToString();
                if (i < (len - 1))
                {
                    strRet += ", ";
                }
                else
                {
                    if (arr.Length != len)
                    {
                        strRet += "...]";
                    }
                    else
                    {
                        strRet += "]";
                    }
                }
            }
            return strRet;
        }

        string ArrayOfDoublesToString(double[] arr)
        {
            string strRet = string.Empty;
            int len = arr.Length;
            int i;
            if (len > 4) len = 4;
            if (len > 0) strRet += "[";
            for (i = 0; i < len; i++)
            {
                strRet += arr[i].ToString();
                if (i < (len - 1))
                {
                    strRet += ", ";
                }
                else
                {
                    if (arr.Length != len)
                    {
                        strRet += "...]";
                    }
                    else
                    {
                        strRet += "]";
                    }
                }
            }
            return strRet;
        }

        string ArrayOfStringsToString(string[] arr)
        {
            string strRet = string.Empty;
            int len = arr.Length;
            int i;
            if (len > 4) len = 4;
            if (len > 0) strRet += "[";
            for (i = 0; i < len; i++)
            {
                strRet += arr[i].ToString();
                if (i < (len - 1))
                {
                    strRet += ", ";
                }
                else
                {
                    if (arr.Length != len)
                    {
                        strRet += "...]";
                    }
                    else
                    {
                        strRet += "]";
                    }
                }
            }
            return strRet;
        }

        string ArrayToString(object oArray)
        {
            string strRet = string.Empty;
            if (null != oArray)
            {
                if (!oArray.GetType().IsArray)
                {
                    strRet = oArray.ToString();
                }
                else
                {
                    string elemtype = oArray.GetType().GetElementType().Name;
                    switch (elemtype)
                    {
                        case "Int32":
                            strRet = ArrayOfIntsToString((int[])oArray);
                            break;
                        case "Double":
                            strRet = ArrayOfDoublesToString((double[])oArray);
                            break;
                        case "Boolean":
                            // there aren't any
                            break;
                        case "String":
                            strRet = ArrayOfStringsToString((string[])oArray);
                            break;
                        default:
                            break;
                    }
                }
            }
            return strRet;
        }

        void FillListview()
        {
            listViewProperties.Items.Clear();
            if ( null != m_pulser)
            {
                IEnumerable<PulserSettingInfo> pulserSettings = m_pulser.PulserSettings;
                string[] sarr;
                bool bNoDirect;
                bool bNoIndex;
                foreach (PulserSettingInfo setting in pulserSettings)
                {
                    if (setting.IsPropertySupported())
                    {
                        bNoDirect = !setting.IsDirectPresentAndSupported;
                        bNoIndex = !setting.IsIndexedPresentAndSupported;
                        IEnumerable<PulserSettingInfo.PropInfoRole> pirList = setting.PropInfoRoles;
                        foreach (PulserSettingInfo.PropInfoRole pir in pirList)
                        {
                            string name = string.Empty;
                            object value;
                            int index;
                            string strValue;
                            if (bNoDirect)
                            {
                                if (pir.Role == PulserPropertyRole.DIRECT) continue;
                            }
                            if (bNoIndex)
                            {
                                if (pir.Role == PulserPropertyRole.INDEX) continue;
                                if (pir.Role == PulserPropertyRole.INDEXVALUES) continue;
                                if (pir.Role == PulserPropertyRole.INDEXMAX) continue;
                            }
                            if (!pir.PropInfo.PropertyType.IsPrimitive && !pir.PropInfo.PropertyType.IsArray 
                                && !pir.PropInfo.PropertyType.IsEnum && (pir.PropInfo.PropertyType.Name != "String")) continue;
                            try
                            {
                                name = pir.PropInfo.Name;
                                value = m_pulser.GetPulserPropertyValue(name);
                                index = pir.IndexValue;
                                strValue = null;
                                if (null != value)
                                {
                                    if (pir.PropInfo.PropertyType.IsArray)
                                    {
                                        strValue = ArrayToString(value);
                                    }
                                    else
                                    {
                                        strValue = value.ToString();
                                    }
                                    sarr = new string[] { name, strValue, pir.IndexValue.ToString() };
                                    ListViewItem lvItem = new ListViewItem(sarr);
                                    listViewProperties.Items.Add(lvItem);
                                }
                            }
                            catch (Exception ee)
                            {
                                if (null == name) name = string.Empty;
                                JSRDotNET_Logger.logException(ee, "Exc in FormPulser.FillListview() propname '" + name + "'");
                            }
                        }
                    } // if is supported
                }
            }
        }

        private void FormPulser_Load(object sender, EventArgs e)
        {
            bool bOK = (m_pulser != null) && (m_pulserId != null) && (m_pulserItem != null);
            if (bOK)
            {
                // Let instrument know we are interested in the power limit status.
                PropertyChangeEventCriteria pcec = new PropertyChangeEventCriteria(new string[] { 
                        PRPropertyNamePulserModel,  /* Only relevant for DPR500 */
                        PRPropertyNameIsPulserPresent, /* DPR500 only. */
                        PRPropertyNames.PulserIsPulsing,
                        PRPropertyNames.PulserPowerLimitStatus, 
                        PRPropertyNames.EnergyPerPulse,
                        PRPropertyNames.PulseEnergyIndex, 
                        PRPropertyNames.PulseRepetitionFrequencyIndex,
                        PRPropertyNames.PulseRepetitionFrequency,
                        PRPropertyNames.HVSupplyIndex, 
                        PRPropertyNames.HVSupply });
                m_pulser.StatusChangePropertyCriteria = pcec;
                m_pulser.StatusChangeEventHandler += HandlePulserStatusChange;

                tbModel.Text = m_pulserId.InstrumentId.ModelName;
                tbSerial.Text = m_pulserId.InstrumentId.SerialNum;
                tbChannel.Text = m_pulserId.PulserReceiverIndex.ToString();
                Text = Item.ToString();
                RefreshControlsFromPulser();
            }
        }

        private void FormPulser_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (null != m_pulser)
            {
                m_pulser.StatusChangeEventHandler -= HandlePulserStatusChange;
            }
        }

        void MakeToast(string msg, MainForm.ToastMessageType toastType = MainForm.ToastMessageType.NORMAL, int minTimeDisplayedInMsec = MainForm.ToastMessage.DEFAULT_MIN_DISPLAY_MSEC)
        {
            Program.MainFormInstance.MakeToast(msg, toastType, minTimeDisplayedInMsec);
        }

        private void FormPulser_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (!m_bFormClosing)
            {
                m_bFormClosing = true;
                m_pulser.ForceDetach();
                m_evtDetach.WaitOne(1000);
                e.Cancel = false;
            }
        }

        private void forceDisconnectToolStripMenuItem_Click(object sender, EventArgs e)
        {
            m_pulser.ForceDetach();
        }

        private void cbBoxTriggerSrc_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!m_bIgnoreSelChanges)
            {
                try
                {
                    TRIGGER_SOURCE tsource = (TRIGGER_SOURCE)cbBoxTriggerSrc.SelectedItem;
                    m_pulser.TriggerSource = tsource;
                }
                catch (Exception)
                {
                }
            }
        }

        private void cbBoxPRF_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!m_bIgnoreSelChanges)
            {
                try
                {
                    DoubleCBItem item = cbBoxPRF.SelectedItem as DoubleCBItem;
                    if (null != item)
                    {
                        setSelectedFlagInItem(cbBoxPRF, item);
                        if (m_pulser.PulseRepetitionFrequencyIndexSupported)
                        {
                            m_pulser.PulseRepetitionFrequencyIndex = item.Index;
                        }
                        else
                        {
                            m_pulser.PulseRepetitionFrequency = item.Value;
                        }
                    }
                    else
                    {
                        clearSelectedFlagsInItems(cbBoxPRF);
                    }
                }
                catch (Exception)
                {
                }
            }
        }

        private void cbBoxHVSupply_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!m_bIgnoreSelChanges)
            {
                try
                {
                    if (0 <= cbBoxHVSupply.SelectedIndex)
                    {
                        DoubleCBItem item = (DoubleCBItem)cbBoxHVSupply.SelectedItem;
                        setSelectedFlagInItem(cbBoxHVSupply, item);
                        if (m_pulser.HVSupplyIndexSupported)
                        {
                            m_pulser.HVSupplyIndex = item.Index;
                        }
                        else if (m_pulser.HVSupplySupported)
                        {
                            m_pulser.HVSupply = item.Value;
                        }
                    }
                    else
                    {
                        clearSelectedFlagsInItems(cbBoxHVSupply);
                    }
                }
                catch (Exception)
                {
                }
            }
        }

        private void cbBoxEnergy_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!m_bIgnoreSelChanges)
            {
                int selIdx = cbBoxEnergy.SelectedIndex;
                if (0 <= selIdx)
                {
                    m_pulser.PulseEnergyIndex = selIdx;
                }
            }
        }

        private void cbBoxDamping_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!m_bIgnoreSelChanges)
            {
                int selIdx = cbBoxDamping.SelectedIndex;
                if (0 <= selIdx)
                {
                    m_pulser.DampingIndex = selIdx;
                }
            }
        }

        private void cbBoxTrigZ_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!m_bIgnoreSelChanges)
            {
                if (0 <= cbBoxTrigZ.SelectedIndex)
                {
                    if (m_pulser.TriggerImpedanceSupported)
                    {
                        if (null != cbBoxTrigZ.SelectedItem)
                        {
                            m_pulser.TriggerImpedance = (TRIGGER_IMPEDANCE)cbBoxTrigZ.SelectedItem;
                        }
                    }
                }
            }
        }

        private DoubleCBItem findClosestItem(ComboBox cb, double enteredValue)
        {
            DoubleCBItem selectedItem = null;
            double smallestDiff = double.MaxValue;
            foreach (DoubleCBItem item in cb.Items)
            {
                double diff = item.Value - enteredValue;
                if (0.0 > diff) diff = -diff;
                if (diff < smallestDiff)
                {
                    selectedItem = item;
                    smallestDiff = diff;
                }
            }
            return selectedItem;
        }


        bool cbBox_KeyPress(ComboBox cb, bool bIndexSupported, string paramName, KeyPressEventArgs e, double dmin, double dmax, out double valueToWrite)
        {
            bool bRet = false;
            valueToWrite = 0.0;
            DoubleCBItem itemLastSelected = getLastSelectedItem(cb);
            {
                if (e.KeyChar == '\r')
                {
                    if (cb.Text.Length > 0)
                    {

                        double enteredValue = 0.0;
                        string strmsg;
                        DoubleCBItem selectedItem = null;
                        bool bParseOK = false;
                        if (!m_bIgnoreSelChanges && (cb.Text.Length > 0))
                        {
                            bParseOK = double.TryParse(cb.Text, out enteredValue);
                            if (bParseOK)
                            {
                                if (bIndexSupported)
                                {
                                    selectedItem = findClosestItem(cb, enteredValue);
                                    cb.Text = string.Empty;
                                    cb.SelectedItem = selectedItem;
                                }
                                else
                                {
                                    double mingain = dmin;
                                    double maxgain = dmax;
                                    if (enteredValue < mingain)
                                    {
                                        strmsg = string.Format("The {2} value entered '{0}' is below the minimum value of '{1}'.   {2} will be set to '{1}'", enteredValue, mingain, paramName);
                                        MessageBox.Show(strmsg, "Entered value is out of range.", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                                        enteredValue = mingain;
                                        cb.SelectedItem = findClosestItem(cb, enteredValue);
                                    }
                                    else
                                        if (enteredValue > maxgain)
                                        {
                                            strmsg = string.Format("The {2} value entered '{0}' is above the maximum value of '{1}'.   {2} will be set to '{1}'", enteredValue, maxgain, paramName);
                                            MessageBox.Show(strmsg, "Entered value is out of range.", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                                            enteredValue = maxgain;
                                            cb.SelectedItem = findClosestItem(cb, enteredValue);
                                        }
                                        else
                                        {
                                            valueToWrite = enteredValue;
                                            bRet = true;   /* Signal that this value needs to be written. */
                                        }
                                }
                            }
                            else
                            {
                                strmsg = string.Format("'{0}' is not valid floating point number", cb.Text);
                                MessageBox.Show(strmsg, "Cannot set " + paramName, MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                                m_bIgnoreSelChanges = true;
                                cb.SelectedItem = itemLastSelected;  // Return to last selected item.  No need to write to the pulser
                                m_bIgnoreSelChanges = false;
                            }
                        }
                        Focus();
                        //cb.Text

                    }
                }
                else if (e.KeyChar == (char)0x1b)  // Process escape key
                {
                    m_bIgnoreSelChanges = true;
                    cb.SelectedItem = itemLastSelected;
                    m_bIgnoreSelChanges = false;
                    Focus();
                }
            }

            return bRet;
        }



        private void cbBoxGain_KeyPress(object sender, KeyPressEventArgs e)
        {
            double newGain = 0.0;
            bool bWriteGain = cbBox_KeyPress(cbBoxGain, m_pulser.GainIndexSupported, "Gain", e, m_pulser.GainMin, m_pulser.GainMax, out newGain);
            if (bWriteGain)
            {
                m_pulser.Gain = newGain;
            }
        }

        private void cbBoxPRF_KeyPress(object sender, KeyPressEventArgs e)
        {
            double newPRF = 1000.0;
            bool bWritePRF = cbBox_KeyPress(cbBoxPRF, m_pulser.PulseRepetitionFrequencyIndexSupported, "PRF",
                e, m_pulser.PulseRepetitionFrequencyMin, m_pulser.PulseRepetitionFrequencyMax, out newPRF);
            if (bWritePRF)
            {
                m_pulser.PulseRepetitionFrequency = newPRF;
            }
        }

        private void cbBoxHVSupply_KeyPress(object sender, KeyPressEventArgs e)
        {
            double newVoltage = 0.0;
            bool bWrite = cbBox_KeyPress(cbBoxHVSupply, m_pulser.HVSupplyIndexSupported, "HV Supply",
                e, m_pulser.HVSupplyMin, m_pulser.HVSupplyMax, out newVoltage);
            if (bWrite)
            {
                m_pulser.HVSupply = newVoltage;
            }
        }

        private void cbLEDBlink_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!m_bIgnoreSelChanges)
            {
                if (0 <= cbLEDBlink.SelectedIndex)
                {
                    m_pulser.LEDBlinkModeIndex = cbLEDBlink.SelectedIndex;
                }
            }
        }

        private void ckBoxHVEnable_CheckedChanged(object sender, EventArgs e)
        {
            if (!m_bIgnoreSelChanges)
            {
                m_pulser.HVSupplyEnable = ckBoxHVEnable.Checked;
            }
        }

        private void cbBoxTriggerPolarity_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!m_bIgnoreSelChanges && (cbBoxTriggerPolarity.SelectedItem != null))
            {
                TRIGGER_POLARITY polarity = (TRIGGER_POLARITY)cbBoxTriggerPolarity.SelectedItem;
                m_pulser.TriggerEdgePolarity = polarity;
            }
        }

        private void cbBoxRcvrMode_SelectedIndexChanged(object sender, EventArgs e)
        {
            RECEIVER_MODE rmode = RECEIVER_MODE.ECHO;
            if (!m_bIgnoreSelChanges)
            {
                try
                {
                    rmode = (RECEIVER_MODE)cbBoxRcvrMode.SelectedItem;
                    m_pulser.ReceiverMode = rmode;
                }
                catch (Exception)
                {
                }
            }
        }

        private void cbBoxGain_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!m_bIgnoreSelChanges)
            {
                DoubleCBItem selectedItem = cbBoxGain.SelectedItem as DoubleCBItem;
                if (null != selectedItem)
                {
                    setSelectedFlagInItem(cbBoxGain, selectedItem);
                    if (m_pulser.GainIndexSupported)
                    {
                        m_pulser.GainIndex = selectedItem.Index;
                    }
                    else
                    {
                        m_pulser.Gain = selectedItem.Value;
                    }
                }
                else
                {
                    clearSelectedFlagsInItems(cbBoxGain);
                }
            }
        }

        private void cbBoxLPFilter_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!m_bIgnoreSelChanges)
            {
                DoubleCBItem item = cbBoxLPFilter.SelectedItem as DoubleCBItem;
                if (null != item)
                {
                    setSelectedFlagInItem(cbBoxLPFilter, item);
                    m_pulser.LowPassFilterIndex = item.Index;
                }
                else
                {
                    clearSelectedFlagsInItems(cbBoxLPFilter);
                }
            }
        }

        private void cbBoxHPFilter_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!m_bIgnoreSelChanges)
            {
                DoubleCBItem item = cbBoxHPFilter.SelectedItem as DoubleCBItem;
                if (null != item)
                {
                    setSelectedFlagInItem(cbBoxHPFilter, item);
                    m_pulser.HighPassFilterIndex = item.Index;
                }
                else
                {
                    clearSelectedFlagsInItems(cbBoxLPFilter);
                }
            }
        }

        private void ckboxTrigEnable_CheckedChanged(object sender, EventArgs e)
        {
            if (!m_bIgnoreSelChanges)
            {
                m_pulser.TriggerEnable = ckboxTrigEnable.Checked;
            }
        }
        private void cbBoxPulserZ_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!m_bIgnoreSelChanges)
            {
                m_pulser.PulserImpedance = (cbBoxPulserZ.SelectedIndex == 1) ? PULSER_IMPEDANCE.LOW_Z : PULSER_IMPEDANCE.HIGH_Z;
            }
        }





        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// Event handler. Called by tabControl for selected index changed events.
        /// </summary>
        ///
        /// <authorAndDate>kds, 3/24/2016. </authorAndDate>
        ///
        /// <param name="sender">.</param>
        /// <param name="e">Event information.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        private void tabControl_SelectedIndexChanged(object sender, EventArgs e)
        {
            switch (tabControl.SelectedIndex)
            {
                case 1:
                    FillListview();
                    break;
                default:
                    break;
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// Event handler. Called by closeToolStripMenuItem for click events.
        /// </summary>
        ///
        /// <authorAndDate>kds, 3/24/2016. </authorAndDate>
        ///
        /// <param name="sender">.</param>
        /// <param name="e">Event information.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        private void closeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void tbTrackPRF_TextChanged(object sender, EventArgs e)
        {
            updatePulserPRF();
        }

        private void butSetPRF_Click(object sender, EventArgs e)
        {
            updatePulserPRF();
            try
            {
                if (!m_bPulserOffline && m_bLastKnownPRFValid)
                {
                    m_pulser.PulseRepetitionFrequency = m_lastKnownPRF;
                }
            }
            catch (Exception ee)
            {
                JSRDotNET_Logger.logException(ee, "Exception in ThreadProcSetPRF");
            }
        }


    }
}
