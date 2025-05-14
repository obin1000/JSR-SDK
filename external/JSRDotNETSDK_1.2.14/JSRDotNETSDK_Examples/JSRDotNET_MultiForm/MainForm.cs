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
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>This application is an example of an application which does not use the JSRDotNET_Manager
    /// class to access the devices.   The manager is only used to find and initialize the instrument
    /// plugins, and to receive instrument discovery events.   As the pulser receivers are discovered,
    /// IPulserReceiver objects are used to communicate with the devices.    I.E. there is no notion of
    /// the 'currently selected pulser receiver'. Each pulser form only communicates with the IPulserReceiver
    /// instance.   The pulser forms do not reference the JSRDotNET_Manager object.  Only the MainForm
    /// does that.</summary>
    ///
    /// <authorAndDate>kds, 5/12/2016.</authorAndDate>
    ///
    /// <seealso cref="T:System.Windows.Forms.Form"/>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    public partial class MainForm : Form
    {

        /// <summary>Name of the simulator plugin.</summary>
        const string DIAG_PLUGIN_NAME = "JSRDotNET_DiagLib";

        /// <summary>Name of the dpr plugin.</summary>
        const string DPR_PLUGIN_NAME = "JSRDotNET_DPR";

        /// <summary>The plugin name USB PureView Board.</summary>
        const string PLUGIN_NAME_USB_PUREVIEW_BOARD = "JSRDotNET_PureViewBrd";

        /// <summary>The path to the JSRDotNETSDK Instrument Plugins</summary>
        string PROGRAM_PLUGIN_PATH = System.IO.Path.GetDirectoryName( 
            System.Reflection.Assembly.GetExecutingAssembly().Location) + @"\..\..\..\..\Plugins";

        /// <summary>Queue of maximum toast.  More than this and some will not make it into the queue.</summary>
        const int MAX_TOAST_IN_QUEUE = 10;
        const int MAX_TOAST_DISPLAY_TIME_IN_MSEC = 3000;

        private delegate void del_oneBool(bool b);




        protected internal class MultiOptions
        {
            public const string CU_KEY = "Software";
            public const string CU_COMPANY_KEY = "JSR Ultrasonics";
            public const string CU_PRODUCT_KEY = "JSRDotNET_MultiForm";
            public const string SHOW_LOG_FORM = "ShowLogForm";
            public const string SCREEN_POS_X = "ScreenPositionX";
            public const string SCREEN_POS_Y = "ScreenPositionY";
            public const string NUM_SIM_INSTR = "NumSimulatedInstruments";
            public const string NUM_PULSERS = "NumPulsersPerSim";
            public const string LOG_WIN_BOUNDS = "LogWindowBounds";
            public const string ENABLE_SIM = "EnableSimulation";
            public const string INSTR_MODEL = "InstrumentModel";

            public bool LoggerFormVisible { get; set; }
            public Point ScreenPosition { get; set; }
            public string InstrumentModel { get; set; }
            public bool EnableSimulation { get; set; }
            public int SimulatedInstruments { get; set; }
            public int NumPulsersPerSim { get; set; }
            public Rectangle LoggerWindowBounds { get; set; }

            public void storeToRegistry()
            {
                RegistryKey key;
                JSRDotNET_Logger.logstrSignificant("Updating Registry");
                try
                {
                    key = Registry.CurrentUser.OpenSubKey(CU_KEY, true);
                    RegistryKey companyKey = key.CreateSubKey(CU_COMPANY_KEY);
                    RegistryKey productKey = companyKey.CreateSubKey(CU_PRODUCT_KEY);
                    productKey.SetValue(SHOW_LOG_FORM, LoggerFormVisible ? 1 : 0, RegistryValueKind.DWord);
                    productKey.SetValue(SCREEN_POS_X, ScreenPosition.X, RegistryValueKind.DWord);
                    productKey.SetValue(SCREEN_POS_Y, ScreenPosition.Y, RegistryValueKind.DWord);
                    productKey.SetValue(INSTR_MODEL, InstrumentModel, RegistryValueKind.String);
                    productKey.SetValue(ENABLE_SIM, (EnableSimulation ? 1 : 0), RegistryValueKind.DWord);
                    productKey.SetValue(NUM_SIM_INSTR, SimulatedInstruments, RegistryValueKind.DWord);
                    productKey.SetValue(NUM_PULSERS, NumPulsersPerSim, RegistryValueKind.DWord);
                    string[] strLogBound = new string[] { 
                        LoggerWindowBounds.X.ToString(),
                        LoggerWindowBounds.Y.ToString(),
                        LoggerWindowBounds.Width.ToString(),
                        LoggerWindowBounds.Height.ToString() };
                    productKey.SetValue(LOG_WIN_BOUNDS, string.Join(",", strLogBound), RegistryValueKind.String);
                }
                catch (Exception ee)
                {
                    JSRDotNET_Logger.logException(ee, "Exception while writing to registry.");
                }
            }

            public void readFromRegistry()
            {
                RegistryKey key;
                key = Registry.CurrentUser.OpenSubKey(CU_KEY, true);
                RegistryKey companyKey = key.CreateSubKey(CU_COMPANY_KEY);
                RegistryKey productKey = companyKey.CreateSubKey(CU_PRODUCT_KEY);
                object o, x, y;

                o = productKey.GetValue(SHOW_LOG_FORM);
                if (null != o)
                    LoggerFormVisible = (((int)o != 0) ? true : false);
                else
                    LoggerFormVisible = false;

                x = productKey.GetValue(SCREEN_POS_X);
                y = productKey.GetValue(SCREEN_POS_Y);
                if ((null != x) && (null != y))
                    ScreenPosition = new Point((int)x, (int)y);
                else
                    ScreenPosition = new Point(0, 0);

                o = productKey.GetValue(INSTR_MODEL);
                if (null != o)
                    InstrumentModel = (string)o;
                else
                    InstrumentModel = "";

                o = productKey.GetValue(ENABLE_SIM);
                if (null != o)
                    EnableSimulation = (((int)o != 0) ? true : false);
                else
                    EnableSimulation = false;

                o = productKey.GetValue(NUM_SIM_INSTR);
                if (null != o)
                    SimulatedInstruments = (int)o;
                else
                    SimulatedInstruments = 1;

                o = productKey.GetValue(NUM_PULSERS);
                if (null != o)
                    NumPulsersPerSim = (int)o;
                else
                    NumPulsersPerSim = 1;

                o = productKey.GetValue(LOG_WIN_BOUNDS);
                if (null != o)
                {
                    string sLoggerBounds = (string)o;
                    string[] saLoggerBounds = sLoggerBounds.Split(new char[] { ',' });
                    try
                    {
                        LoggerWindowBounds = new Rectangle(
                            int.Parse(saLoggerBounds[0]), int.Parse(saLoggerBounds[1]),
                            int.Parse(saLoggerBounds[2]), int.Parse(saLoggerBounds[3]));
                    }
                    catch (Exception)
                    {
                        LoggerWindowBounds = new Rectangle(0, 0, 0, 0);
                    }
                }
                else
                {
                    LoggerWindowBounds = new Rectangle(0, 0, 0, 0);
                }

            }

            public MultiOptions()
            {
                LoggerFormVisible = false;
                ScreenPosition = new Point(0, 0);
                InstrumentModel = "";
                EnableSimulation = false;
                SimulatedInstruments = 1;
                NumPulsersPerSim = 1;
                LoggerWindowBounds = new Rectangle(0, 0, 0, 0);
            }
        }



        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Values that represent toast message types.</summary>
        ///
        /// <authorAndDate>kds, 3/29/2016.</authorAndDate>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        protected internal enum ToastMessageType
        {
            NORMAL,
            WARNING
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>A toast message is a message that displays for a readable amound of time, then 
        /// disappears.</summary>
        ///
        /// <authorAndDate>kds, 3/29/2016.</authorAndDate>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        protected internal class ToastMessage
        {
            public const int DEFAULT_MIN_DISPLAY_MSEC = 1000;
            string m_msg;
            int m_timeRcvd;
            int m_tstampDisplayed;
            ToastMessageType m_msgType;
            int m_minDisplayMsec;

            public string Message { get { return m_msg; } }
            public int TimestampReceived { get { return m_timeRcvd; } }
            public ToastMessageType ToastType { get { return m_msgType; } }
            public int TimestampDisplayed { get { return m_tstampDisplayed; } set { m_tstampDisplayed = value; } }
            public int MinDisplayTime { get { return m_minDisplayMsec; } }

            public ToastMessage(string msg, ToastMessageType toastType = ToastMessageType.NORMAL, int minDisplayMsec = DEFAULT_MIN_DISPLAY_MSEC)
            {
                m_msg = msg;
                m_msgType = toastType;
                m_minDisplayMsec = minDisplayMsec;
                m_timeRcvd = Environment.TickCount;
                m_tstampDisplayed = 0;
            }
        }
        /// <summary>
        /// This example uses a helper class, JSRDotNETManager to simplify the application.
        /// </summary>
        JSRDotNETManager m_jsrManager;

        Dictionary<IPulserReceiverIdentity, FormPulser> m_dictPulserForms;

        MultiOptions m_options;

        /// <summary>To implement restart, the desired state causes code in the timer tick method to advance
        /// the state of the manager.    </summary>
        JSRDotNETManager.MANAGER_STATE m_desiredManagerState = JSRDotNETManager.MANAGER_STATE.NOT_STARTED;

        /// <summary>The timer tick lock.</summary>
        private int m_iInTimerTick = 0;



        /// <summary>   The logger form. </summary>
        LoggerForm m_loggerForm;

      //  ControlPanelOptions m_options;

     //   FormDevInfo m_formDevInfo = null;

     //   FormAbout m_formAbout = null;


        IJSRDotNET m_SimulatorLib = null;

        /// <summary>If the notify routine reports a manager exception, then is true.  If false, the
        /// timer routine can still detect an exception by looking at JSRDotNetManager.LastExceptionOrNull.</summary>
        bool m_bExceptionAlreadyReported = false;


        /// <summary>A queue of messages to be displayed to the user..</summary>
        Queue<ToastMessage> m_queueToast;

        /// <summary>The currently displayed toast.  Null if nothing is being displayed.</summary>
        ToastMessage m_currentlyDisplayedToast = null;

        Font m_toastNormalFont = null;
        Font m_toastWarningFont = null;
        bool m_bIsClosing = false;
        bool m_bStartupError = false;

        public void HideLogForm()
        {
            if (!m_bIsClosing)
            {
                logWindowToolStripMenuItem.Checked = false;
            }
        }

        public MainForm()
        {
            m_jsrManager = null;
            m_dictPulserForms = new Dictionary<IPulserReceiverIdentity, FormPulser>();
            JSRDotNET_Logger.LogLevel = JSRDotNET_Logger.LOGLEVEL.IMPORTANT;
            m_desiredManagerState = JSRDotNETManager.MANAGER_STATE.NOT_STARTED;  // Will get changed to RUNNING in form1.Load()
            m_queueToast = new Queue<ToastMessage>();
            InitializeComponent();
            m_options = new MultiOptions();
            m_options.readFromRegistry();
            logWindowToolStripMenuItem.Checked = m_options.LoggerFormVisible;
            simulatorWindowsToolStripMenuItem.Checked = m_options.EnableSimulation;
            m_loggerForm = new LoggerForm(this);
            m_loggerForm.MdiParent = this;
            string assemblyVersion = Assembly.GetExecutingAssembly().GetName().Version.ToString();
            string assemblyVerShorter = assemblyVersion;
            int idxLastDot = assemblyVersion.LastIndexOf('.');
            if (idxLastDot > 2)
            {
                assemblyVerShorter = assemblyVersion.Remove(idxLastDot);
            }
            Text = string.Format("  JSR .Net Multi Form Example  (Ver: {0})", assemblyVerShorter);
            JSRDotNET_Logger.logstrSignificant(string.Format("JSRDotNETControlPanel Ver {0} started at {1}", assemblyVerShorter,
                DateTime.Now.ToString()));

            Screen screen = Screen.FromControl(this);
            Rectangle rectscreen = screen.WorkingArea;
            Rectangle bounds = new Rectangle();
            int width = m_loggerForm.Bounds.Width;
            int height = m_loggerForm.Bounds.Height;
            bool bOffScreen = (m_options.LoggerWindowBounds.X > (rectscreen.Width - 50)) || (m_options.LoggerWindowBounds.Y > (rectscreen.Height - 30));

            if ((m_options.LoggerWindowBounds.Height == 0) || (bOffScreen))
            {
                bounds.X = rectscreen.Width - width - 2;
                bounds.Y = rectscreen.Height - height - 2;
                bounds.Width = m_loggerForm.Bounds.Width;
                bounds.Height = m_loggerForm.Bounds.Height;
                m_loggerForm.Bounds = bounds;
                m_options.LoggerWindowBounds = bounds;
            }
            else
            {
                bounds = m_options.LoggerWindowBounds;
                m_loggerForm.Bounds = bounds;
            }
        }

        public void ChildFormClosing(FormPulser childForm)
        {
            MakeToast("Pulser Receiver Disconnected: " + childForm.Item.ToString());
            if (m_dictPulserForms.Keys.Contains<IPulserReceiverIdentity>(childForm.PulserReceiverId))
            {
                m_dictPulserForms.Remove(childForm.PulserReceiverId);
            }
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            if(m_options.LoggerFormVisible) m_loggerForm.Show();
            m_desiredManagerState = JSRDotNETManager.MANAGER_STATE.RUNNING;
            timer1.Enabled = true;
        }

        private void MainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            m_loggerForm.CloseLogWindow();
            if (null != m_jsrManager)
            {
                m_jsrManager.Shutdown();
            }
            m_options.storeToRegistry();
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            timer1.Enabled = false;
            m_bIsClosing = true;
            JSRDotNET_Logger.logstrSignificant("Main Window Closing. Reason: " + e.CloseReason.ToString());
            m_options.ScreenPosition = Bounds.Location;
            m_options.LoggerWindowBounds = m_loggerForm.Bounds;
            m_options.LoggerFormVisible = logWindowToolStripMenuItem.CheckState == CheckState.Checked;
            if (m_loggerForm.Visible) m_loggerForm.Hide();
            foreach (FormPulser formPulser in m_dictPulserForms.Values)
            {
                formPulser.Close();
            }
            e.Cancel = false;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Queues a message to be displayed to the user in the status bar.</summary>
        ///
        /// <authorAndDate>kds, 3/29/2016.</authorAndDate>
        ///
        /// <param name="msg">The message.</param>
        /// <param name="toastType">Type of the toast.</param>
        /// <param name="minTimeDisplayedInMsec">The minimum time displayed in msec.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        internal void MakeToast(string msg, ToastMessageType toastType = ToastMessageType.NORMAL, int minTimeDisplayedInMsec = ToastMessage.DEFAULT_MIN_DISPLAY_MSEC)
        {
            lock (m_queueToast)
            {
                if (m_queueToast.Count < MAX_TOAST_IN_QUEUE)
                {
                    m_queueToast.Enqueue(new ToastMessage(msg, toastType, minTimeDisplayedInMsec));
                }
            }
            UpdateToast();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Updates the toast if there  are any in the queue and if the currently displayed toast
        /// is done with.</summary>
        ///
        /// <authorAndDate>kds, 3/29/2016.</authorAndDate>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        protected internal void UpdateToast()
        {
            if (this.InvokeRequired)
            {
                Action myself = UpdateToast;
                this.BeginInvoke(myself);
            }
            else
            {
                if (null == m_toastNormalFont)
                {
                    m_toastNormalFont = new Font(this.Font.FontFamily.Name, this.Font.Size);
                }
                if (null == m_toastWarningFont)
                {
                    m_toastWarningFont = new Font(this.Font.FontFamily.Name, this.Font.Size, FontStyle.Bold);
                }
                bool bOneQueued = false;
                lock (m_queueToast)
                {
                    bOneQueued = 0 < m_queueToast.Count;
                }
                if (m_currentlyDisplayedToast != null)
                {
                    int tElapsed = Environment.TickCount - m_currentlyDisplayedToast.TimestampDisplayed;
                    int tMaxToLive = MAX_TOAST_DISPLAY_TIME_IN_MSEC;
                    if (bOneQueued) tMaxToLive = m_currentlyDisplayedToast.MinDisplayTime;
                    if (tElapsed > tMaxToLive)
                    {
                        m_currentlyDisplayedToast = null;
                        if (!bOneQueued)
                        {
                            toolStripStatusLabel1.Font = m_toastNormalFont;
                            toolStripStatusLabel1.ForeColor = this.ForeColor;
                            toolStripStatusLabel1.Text = string.Empty;
                        }
                    }
                }
                if (bOneQueued && (null == m_currentlyDisplayedToast))
                {
                    lock (m_queueToast)
                    {
                        m_currentlyDisplayedToast = m_queueToast.Dequeue();
                    }
                    m_currentlyDisplayedToast.TimestampDisplayed = Environment.TickCount;
                    switch (m_currentlyDisplayedToast.ToastType)
                    {
                        case ToastMessageType.NORMAL:
                            toolStripStatusLabel1.Font = m_toastNormalFont;
                            toolStripStatusLabel1.ForeColor = this.ForeColor;
                            break;
                        case ToastMessageType.WARNING:
                            toolStripStatusLabel1.Font = m_toastWarningFont;
                            toolStripStatusLabel1.ForeColor = Color.Red;
                            break;
                    }
                    toolStripStatusLabel1.Text = m_currentlyDisplayedToast.Message;
                }
            }
        }


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Sets plugin open options.</summary>
        ///
        /// <authorAndDate>mwb, 5/2/2016.</authorAndDate>
        ///
        /// <param name="pluginName">Name of the plugin.</param>
        ///
        /// <returns>true to load the plugin, otherwise false.</returns>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        bool setPluginOpenOptions(string pluginName)
        {
            bool bRet = true;
            InstrumentOpenCriteria ioc = null;

            if (pluginName == DPR_PLUGIN_NAME)
            {
                IJSRDotNET lib = m_jsrManager.GetPluginLibraryInstance(pluginName);
                if (lib != null)
                {
                    ioc = lib.InstrumentOpenCriteria;
                    if (ioc != null)
                    {
                        JSRDotNET_Logger.logstrSignificant("Setting plugin open options for " + pluginName + " library.");
                        ioc.RemoveAllOpenOptions();
                        /* kds 11May2016 - removed HOTSWAP option, since we are going to make the implementation only do PROP_CHANGE */
                        //ioc.AddOpenOption("DPR500_HOTSWAP", "PROP_CHANGE");
                        //ioc.AddOpenOption("DPR500_HOTSWAP", "DETACH_REATTACH");
                        ioc.AddOpenOption("DPR500_POLL", "ENABLE");
                        ioc.AddOpenOption("SERIAL_PORT_LOG", "DISABLE");
                        bRet = true;
                    }
                }
            }
            else if (pluginName == DIAG_PLUGIN_NAME)
            {
                IJSRDotNET lib = m_jsrManager.GetPluginLibraryInstance(pluginName);
                if (lib != null)
                {
                    ioc = lib.InstrumentOpenCriteria;
                    if (ioc != null)
                    {
                        JSRDotNET_Logger.logstrSignificant("Setting plugin open options for " + pluginName + " library.");
                        ioc.RemoveAllOpenOptions();
                        ioc.AddOpenOption("NUM_PULSERS_PER_SIM", "1");
                        ioc.AddOpenOption("NUM_SIMULATED_INSTRUMENTS", "1");
                        ioc.AddOpenOption("SHOW_SIMULATOR_WINDOWS", "YES");
                        ioc.AddOpenOption("CONNECT_ON_OPEN", "NO");
                        bRet = false;
                    }
                }
            }
            else
            {
                JSRDotNET_Logger.logstrSignificant("No open options to set for " + pluginName + " library.");
            }

            return bRet;
        }



        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Configure simulator.</summary>
        ///
        /// <authorAndDate>mwb, 5/2/2016.</authorAndDate>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        protected void ConfigureSimulator()
        {
            string simPluginName = DIAG_PLUGIN_NAME;

            if (m_SimulatorLib == null)
            {
                m_SimulatorLib = m_jsrManager.GetPluginLibraryInstance(simPluginName);
            }

            if (m_SimulatorLib != null)
            {
                PropertyInfo piMdiParent = m_SimulatorLib.GetType().GetProperty("MdiParentForm");
                if (null != piMdiParent)
                {
                    piMdiParent.SetValue(m_SimulatorLib, this, null);
                }
                m_SimulatorLib.InstrumentDiscoveryEnable = false;
                m_SimulatorLib.Shutdown();

                if (m_options.EnableSimulation)
                {
                    try
                    {
                        setPluginOpenOptions(simPluginName);
                        m_jsrManager.AddManagedPlugin(simPluginName);

                        // Restart the instrument discovery process
                        JSRDotNET_Logger.logstrSignificant("Enabling Instrument Discovery for " + DIAG_PLUGIN_NAME + " lib.");
                        m_SimulatorLib.InstrumentDiscoveryEnable = true;
                    }
                    catch (Exception ee)
                    {
                        JSRDotNET_Logger.logException(ee,
                            "Exception while enabling instrument discovery for " + DIAG_PLUGIN_NAME + " lib");
                    }
                }
                else
                {
                    m_jsrManager.RemoveManagedPlugin(DIAG_PLUGIN_NAME);
                }
            }
        }

        void AddPulserReceiver(EventArgsManagerNotify args)
        {
            if (InvokeRequired)
            {
                BeginInvoke(new Action<EventArgsManagerNotify>(AddPulserReceiver), new object[] { args });
            }
            else
            {
                string strModel = args.Model;
                string strSerial = args.Serial;
                int prIndex = 0;
                prIndex = args.PRIndex;
                if (args.Model != string.Empty)
                {
                    string strConnectionInfo = null;
                    string strAddress = null;
                    string strPulserModel = null;
                    string strBandwidth = null;
                    {
                        string[] info = args.Info;
                        int i;
                        for (i = 0; i < info.Length; i += 2)
                        {
                            string name = info[i];
                            string value = info[i + 1];
                            if (name == "SerialPortName") strConnectionInfo = value;
                            if (name == "PulserModel") strPulserModel = value;
                            if (name == "InstrumentAddress") strAddress = value;
                        }
                        if (strAddress != null) strConnectionInfo += ",Addr " + strAddress;
                        switch (prIndex)
                        {
                            case 0:
                                strConnectionInfo += ",Ch A";
                                break;
                            case 1:
                                strConnectionInfo += ",Ch B";
                                break;
                            case 2:
                                strConnectionInfo += ",Ch C";
                                break;
                            case 3:
                                strConnectionInfo += ",Ch D";
                                break;
                            default:
                                break;
                        }
                        strBandwidth = args.MaxFrequency.ToString("###0.0") + "MHz";
                    }
                    PulserReceiverItem newItem = new PulserReceiverItem(strModel, strSerial, prIndex,
                        strConnectionInfo, strBandwidth, strPulserModel);
                    /* kds 5May2016 - taking out the changing of selection when a new device is discovered. */
                   // selectedIndex =
                   // listBoxPulserSelect.Items.Add(newItem);
                   // grpBoxInstrument.Text = "Instruments (" + listBoxPulserSelect.Items.Count + " connected)";

                    FormPulser newForm = new FormPulser();
                    m_dictPulserForms.Add(args.PulserReceiverId, newForm);
                    newForm.MdiParent = this;
                    newForm.Item = newItem;
                    newForm.PulserReceiverId = args.PulserReceiverId;



                    IPulserReceiver pulser = m_jsrManager.GetPulserReceiver(args.PulserReceiverId);

                    newForm.PulserReceiver = pulser;
                    newForm.Show();
                    MakeToast("Discovered " + newItem.ToString());
                    JSRDotNET_Logger.logstrSignificant("Form1.AddPulserReceiver: " + newItem.ToString());
                }
            }
           // PulserReceiverItem item = new PulserReceiverItem(args.Model, args.Serial, args.PRIndex, 

        }

        void RemovePulserReceiver(EventArgsManagerNotify args)
        {
        }

        void HandlePropertyChangeNotification(EventArgsManagerNotify args)
        {
        }


        private void HandleErrorNotification(EventArgsManagerNotify args)
        {
            if (InvokeRequired)
            {
                BeginInvoke(new Action<EventArgsManagerNotify>(HandleErrorNotification), new object[] { args });
            }
            else
            {
                m_bExceptionAlreadyReported = true;
                MakeToast(args.PluginName + " : " + args.ErrorMsg, ToastMessageType.WARNING, 4000);
                if (null == args.InstrumentId)
                {
                    JSRDotNET_Logger.logstrSignificant("Error in lib: " + args.PluginName + " Type: " + args.ExceptionTypeInfo);
                }
                else
                {
                    JSRDotNET_Logger.logstrSignificant("Error.  Device: " + args.InstrumentId.ToString() + " Type: " + args.ExceptionTypeInfo);
                }
                JSRDotNET_Logger.logstrSignificant("Error Message: " + args.ErrorMsg);
                JSRDotNET_Logger.logstrSignificant("Error detail: " + args.ErrorText);
                if (0 == (args.DiscoverState & DiscoveryStateFlags.PENDING))
                {
                    MessageBox.Show(args.ErrorMsg, args.ExceptionTypeInfo, MessageBoxButtons.OK, MessageBoxIcon.Error);
                    MessageBox.Show(args.ErrorText, "Stack Trace", MessageBoxButtons.OK);
                }
            }
        }


        /// <summary>
        /// The manager will call this when various events happen.   We are interested in when a device
        /// is discovered, and when it is selected.   
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        private void HandleManagerNotifyEvents(object sender, EventArgsManagerNotify args)
        {

            JSRDotNET_Logger.logstrSignificant(string.Format("Manager Notify: {0} Args:  {1}, {2}, {3}", args.NotifyType.ToString(), args.Model, args.Serial, args.PRIndex));
            switch (args.NotifyType)
            {
                case NOTIFY_TYPE.PULSER_RCVR_DISCOVERED:
                    AddPulserReceiver(args);
                    break;
                case NOTIFY_TYPE.CURRENT_PULSER_RCVR_CHANGED:
                    break;
                case NOTIFY_TYPE.PROPERTY_CHANGE:
                    HandlePropertyChangeNotification(args);
                    break;
                case NOTIFY_TYPE.PULSER_RCVR_DETACH:
                    RemovePulserReceiver(args);
                    break;
                case NOTIFY_TYPE.DISCOVERY_STARTED:
                    MakeToast("Turning on Device Discovery");
                    JSRDotNET_Logger.logstrSignificant(args.PluginName + " : NOTIFY DISC STARTED");
                    break;

                case NOTIFY_TYPE.DISCOVERY_COMPLETE:
                    MakeToast("Initial Device Discovery Complete");
                    JSRDotNET_Logger.logstrSignificant(args.PluginName + " : NOTIFY DISC COMPLETE");
                    break;

                case NOTIFY_TYPE.ERROR:
                    HandleErrorNotification(args);
                    break;
            }

        }


        void LoadPluginsAndStart()
        {


            List<string> listLibNames = null;
            bool bLoadPlugins = true;

            string fullpath_plugins_folder = PROGRAM_PLUGIN_PATH;

            if (!System.IO.Directory.Exists(fullpath_plugins_folder))
            {
                MessageBox.Show("Cannot find plugins folder '" + fullpath_plugins_folder + "'");
                m_bStartupError = true;
                return;
            }
            JSRDotNET_Logger.logstrSignificant("Looking for JSRDotNET library plugins in folder '" + fullpath_plugins_folder + "'");


            m_jsrManager = new JSRDotNETManager();


            if (bLoadPlugins)
            {

                try
                {
                    m_jsrManager.PluginPath = fullpath_plugins_folder;
                    m_jsrManager.LoadPlugins();
                }
                catch (Exception ea)
                {
                    string msg = string.Format("Exception type '{0}' occurred while loading plugins from folder '{1}'", ea.GetType().ToString(), fullpath_plugins_folder);
                    MessageBox.Show(msg, "Failure while loading plugins");
                    bLoadPlugins = false;
                    JSRDotNET_Logger.logException(ea, msg);
                    m_bStartupError = true;
                }
            }

            if (bLoadPlugins)
            {
                listLibNames = new List<string>(m_jsrManager.GetPluginNames());

                bLoadPlugins = 0 < listLibNames.Count;
                if (0 >= listLibNames.Count)
                {
                    MessageBox.Show(string.Format("The plugins folder '{0}' has no plugins.", fullpath_plugins_folder));
                }
                JSRDotNET_Logger.logstrSignificant(string.Format("Found {0} library plugins", listLibNames.Count));
                m_bStartupError = true;
            }
            if (bLoadPlugins)
            {
                List<string> listSupportedModels = new List<string>();
                foreach (string libname in listLibNames)
                {
                    JSRDotNET_Logger.logstrSignificant("Adding plugin library '" + libname + "'");

                    IJSRDotNETLibMetadata meta = m_jsrManager.GetPluginLibraryMetadata(libname);
                    if (null != meta)
                    {
                        JSRDotNET_Logger.logstrSignificant("Friendly Name: " + meta.FriendlyName);
                        JSRDotNET_Logger.logstrSignificant("Connection type: " + meta.ConnectionType.ToString());
                        MakeToast("Found plugin Library: " + meta.FriendlyName);
                        if (null != meta.SupportedModels)
                        {
                            JSRDotNET_Logger.logstrSignificant("Supported Models: " + string.Join(",", meta.SupportedModels));
                            listSupportedModels.AddRange(meta.SupportedModels);
                        }
                        Dictionary<string, List<string>> dictOpenOptions = m_jsrManager.GetPluginLibOpenOptions(libname);
                        if (null != dictOpenOptions)
                        {
                            foreach (string optionName in dictOpenOptions.Keys)
                            {
                                JSRDotNET_Logger.logstrSignificant(optionName + ":" + string.Join(",", dictOpenOptions[optionName].ToArray()));
                            }
                        }
                        else
                        {
                            JSRDotNET_Logger.logstrSignificant("OpenOptions was null");
                        }
                        if (setPluginOpenOptions(libname))
                        {
                            m_jsrManager.AddManagedPlugin(libname);
                        }
                    }
                }

                // Configure the simulator, if it's enabled
                ConfigureSimulator();

                toolStripStatusLabelRight.Text = string.Join(",", listSupportedModels.ToArray());
            }

            // Hook up our event handler so we can notifications when a device is discovered etc.
            m_jsrManager.NotifyEventHandler += HandleManagerNotifyEvents;

            // Tell the manager to start the discovery process. 
            try
            {
                MakeToast("Turning on Instrument Discovery");
                m_jsrManager.SetDiscoveryEnable(true);
            }
            catch (Exception eee)
            {
                JSRDotNET_Logger.logException(eee, "Exception Manager SetDiscoveryEnable(true)");
                string caption = string.Format("Got {0} while setting DiscoverEnable to true",
                    eee.GetType().ToString());
                string msg = eee.Message;
                if (null == msg) msg = "(null)";
                MessageBox.Show(msg, caption);
                m_bStartupError = true;
            }

            timer1.Enabled = true;
        }


        private void timer1_Tick(object sender, EventArgs e)
        {
            Interlocked.Increment(ref m_iInTimerTick);
            if (1 == m_iInTimerTick)
            {

                while (JSRDotNET_Logger.GetLogCount() > 0)
                {
                    string strLine = JSRDotNET_Logger.DequeueEarliestLogline();
                    if (null != strLine)
                    {

                        m_loggerForm.AddLine(strLine);
                    }
                }

                // Update the status strip text if need be.
                bool bToastInOven = false;
                lock (m_queueToast)
                {
                    bToastInOven = 0 < m_queueToast.Count;
                }
                if ((null != m_currentlyDisplayedToast) || bToastInOven)
                {
                    UpdateToast();
                }
                if (null != m_jsrManager)
                {
                    if (null != m_jsrManager.LastExceptionOrNull)
                    {
                        if (!m_bExceptionAlreadyReported)
                        {
                            string strErrMsg = m_jsrManager.LastExceptionContextMessage;
                            if (strErrMsg == null) strErrMsg = "(no context)";
                            if (null != m_jsrManager.LastExceptionOrNull.GetBaseException().Message)
                            {
                                strErrMsg += "\r\n Msg: " + m_jsrManager.LastExceptionOrNull.GetBaseException().Message;
                            }
                            string strErrType = m_jsrManager.LastExceptionOrNull.GetBaseException().GetType().ToString();
                            MessageBox.Show(strErrMsg, strErrType, MessageBoxButtons.OK, MessageBoxIcon.Error);
                            MessageBox.Show(m_jsrManager.LastExceptionOrNull.GetBaseException().ToString(), "Stack Trace", MessageBoxButtons.OK);
                            m_bExceptionAlreadyReported = true;
                        }
                    }
                }
                /* Handle desired stated for the Manager */
                if (m_desiredManagerState == JSRDotNETManager.MANAGER_STATE.RUNNING)
                {
                    if (!m_bStartupError && ( null == m_jsrManager))
                    {
                        JSRDotNET_Logger.logstrSignificant("TimerTick Desired state RUNNING but m_jsrManger null, so calling LoadPluginsAndStart()");
                        LoadPluginsAndStart();
                    }
                }
                else if (m_desiredManagerState == JSRDotNETManager.MANAGER_STATE.SHUTTING_DOWN)
                {
                    if (null != m_jsrManager)
                    {
                        if (m_jsrManager.ManagerState == JSRDotNETManager.MANAGER_STATE.RUNNING)
                        {
                            JSRDotNET_Logger.logstrSignificant("TimerTick Desired state SHUTTINGDOWN but is RUNNING, so setting Discover=false");
                            m_jsrManager.SetDiscoveryEnable(false);

                            IPulserReceiverIdentity[] prids = m_jsrManager.GetPulserReceivers();
                            if (0 < prids.Length)
                            {
                                if (m_jsrManager.IsPulserReceiverSelected && (m_jsrManager.Id.InstrumentId.ModelName == prids[0].InstrumentId.ModelName) &&
                                    (m_jsrManager.Id.InstrumentId.SerialNum == prids[0].InstrumentId.SerialNum) &&
                                    (m_jsrManager.Id.PulserReceiverIndex == prids[0].PulserReceiverIndex))
                                {
                                    JSRDotNET_Logger.logstrSignificant("TimerTick Desired SHUTTINGDOWN but is RUNNING.  Forcing Detach: " + m_jsrManager.Id.ToString());
                                    m_jsrManager.ForceDetach();
                                }
                                else
                                {
                                    JSRDotNET_Logger.logstrSignificant("TimerTick Desired SHUTTINGDOWN but is RUNNING.  Selecting pulser : " + prids[0].ToString());
                                    m_jsrManager.SetCurrentPulserReceiver(prids[0]);
                                }
                            }
                            else
                            {
                                JSRDotNET_Logger.logstrSignificant("TimerTick Desired SHUTTINGDOWN but is RUNNING. All pulsers detached, so calling shutdown()");
                                m_desiredManagerState = JSRDotNETManager.MANAGER_STATE.NOT_STARTED;
                                m_jsrManager.Shutdown();
                                m_jsrManager = null;
                                JSRDotNET_Logger.logstrSignificant("TimerTick shutdown complete and m_jsrManager set to null.  Setting desired state to RUNNING");
                                m_desiredManagerState = JSRDotNETManager.MANAGER_STATE.RUNNING;
                            }
                        }
                    }
                }
            }
            Interlocked.Decrement(ref m_iInTimerTick);
        }

        private void logWindowToolStripMenuItem_CheckStateChanged(object sender, EventArgs e)
        {
            /* don't do this during constructor. */
            if (null != m_loggerForm)
            {
                switch (logWindowToolStripMenuItem.CheckState)
                {
                    case CheckState.Checked:
                        m_loggerForm.Show();
                        m_options.LoggerFormVisible = true;
                        break;
                    case CheckState.Unchecked:
                        m_loggerForm.Hide();
                        m_options.LoggerFormVisible = false;
                        break;
                    default:
                        break;
                }
            }
        }

        private void simulatorWindowsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            /* Do do this during constructor. */
            if (null != m_loggerForm)
            {
                m_options.EnableSimulation = simulatorWindowsToolStripMenuItem.Checked;
                ConfigureSimulator();
            }
        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            FormAbout about = new FormAbout();
            about.MdiParent = this;
            about.PluginDataList = new List<IJSRDotNETLibMetadata>();
            foreach (string pluginName in m_jsrManager.GetPluginNames())
            {
                about.PluginDataList.Add(m_jsrManager.GetPluginLibraryMetadata(pluginName));
            }
            about.Show();
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }
}
