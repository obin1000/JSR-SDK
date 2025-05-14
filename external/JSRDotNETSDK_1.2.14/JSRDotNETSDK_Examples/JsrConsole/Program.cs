////////////////////////////////////////////////////////////////////////////////////////////////////
// JsrConsole
// 
// summary: A full-featured console program that demonstrates the ability to list the
// discovered plugins, instrument and Pulser-Receiver discovery, and displaying/configuring
// the different settings available for the Pulser-Receiver.
// 
// Copyright (C) 2016, Imaginant Inc.
// 
// See the file "LICENSE_EXAMPLES.TXT" for the full license governing this code. 
////////////////////////////////////////////////////////////////////////////////////////////////////
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Reflection;
using System.Threading;
using JSRDotNETSDK;

namespace JsrConsole
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// A full-featured console program that demonstrates the ability to list the discovered
    /// plugins, instrument and Pulser-Receiver discovery, and displaying/configuring the
    /// different settings available for the Pulser-Receiver.  Since this is a completely
    /// dynamically driven application, it demonstrates the ability to discovery the settings and
    /// values supported by the selected Pulser-Receiver.  The operation of this application is
    /// broken up into different methods that encapsulate the specific behavior of the current
    /// menu and operation being performed.
    /// </summary>
    ///
    /// <authorAndDate>Mike, 3/23/2016.</authorAndDate>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class Program
    {
        #region Local Variables
        /// <summary>Full pathname of the plugin file.</summary>
        internal string m_sPluginPath;

        /// <summary>The path to the JSRDotNETSDK Instrument Plugins</summary>
        string PROGRAM_PLUGIN_PATH = System.IO.Path.GetDirectoryName(
            System.Reflection.Assembly.GetExecutingAssembly().Location) + @"/../../../../Plugins";

        /// <summary>JSR Plugin helper</summary>
        JSRDotNETManager m_oJsrManager;

        /// <summary>The pulser receiver instances.</summary>
        List<IPulserReceiverIdentity> m_oPulserRcvrs;

        /// <summary>The state.</summary>
        ConsoleState m_nState;

        /// <summary>true to running.</summary>
        bool m_bRunning;

        /// <summary>true to error.</summary>
        bool m_bErrorOccurred;

        /// <summary>The discovery timeout in seconds.</summary>
        int m_nDiscoveryTimeout;

        /// <summary>The discovery event.</summary>
        AutoResetEvent m_DiscoveryEvent;

        /// <summary>A list of the PulserSettingInfo objects supported by the current device.</summary>
        SortedDictionary<string, PulserSettingInfo> m_oSettings;

        Thread m_logThread;

        #endregion

        #region Local Definitions and Consts
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Values that represent prompt results.</summary>
        ///
        /// <authorAndDate>Mike, 3/23/2016.</authorAndDate>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        internal enum PromptResult
        {
            /// <summary>
            /// Exit the application.
            /// </summary>
            EXIT = -99,
            /// <summary>
            /// Exit the current menu.
            /// </summary>
            BACK = -2,
            /// <summary>
            /// Display the help menu.
            /// </summary>
            HELP = -4,
            /// <summary>
            /// Internal value used until a value is determined,.
            /// </summary>
            PROMPT = -1,
            /// <summary>
            /// An enum constant representing the success option.
            /// </summary>
            SUCCESS = 0,
            /// <summary>
            /// An enum constant representing the success option.
            /// </summary>
            FALSE = 0,
            /// <summary>
            /// An enum constant representing the success option.
            /// </summary>
            TRUE = 1
        }

        /// <summary>The menu back.</summary>
        internal const char MENU_BACK = 'q';

        /// <summary>The menu help.</summary>
        internal const char MENU_HELP = 'h';

        /// <summary>The menu exit.</summary>
        internal const char MENU_EXIT = 'x';

        /// <summary>The menu idxprev.</summary>
        internal const char MENU_IDXPREV = 'p';

        /// <summary>The menu idxnext.</summary>
        internal const char MENU_IDXNEXT = 'n';

        /// <summary>The menu idxall.</summary>
        internal const char MENU_IDXALL = 'a';

        /// <summary>The discovery wait in seconds.</summary>
        internal const int DISCOVERY_WAIT_SECS = 30;

        /// <summary>A character abbreviation of the channel index</summary>
        internal char[] CHANNEL_NAME;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Values that represent console states.</summary>
        ///
        /// <authorAndDate>Mike, 3/23/2016.</authorAndDate>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        enum ConsoleState
        {
            /// <summary>An enum constant representing the state initialize option. </summary>
            STATE_INITIALIZE,
            /// <summary>An enum constant representing the state select plugin option. </summary>
            STATE_SELECT_PLUGIN,
            /// <summary>An enum constant representing the state open option. </summary>
            STATE_OPEN,
            /// <summary>An enum constant representing the state discovery option. </summary>
            STATE_DISCOVERY,
            /// <summary>An enum constant representing the state running option. </summary>
            STATE_RUNNING,
            /// <summary>An enum constant representing the state exit option.</summary>
            STATE_EXIT
        }
        #endregion

        #region Main() Entry Function
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Main entry-point for this application.</summary>
        ///
        /// <authorAndDate>Mike, 3/23/2016.</authorAndDate>
        ///
        /// <param name="args">An array of strings passed to the main() function.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static void Main(string[] args)
        {
            Program mainProg = new Program();

            // Parse any arguments provided on the command line
            mainProg.ParseArgs(args);
            mainProg.m_textWriterForLog = new StreamWriter(Path.GetDirectoryName(
            System.Reflection.Assembly.GetExecutingAssembly().Location)+"/log");
            mainProg.m_logThread.Start();
            JSRDotNET_Logger.logstrSignificant("Program Started");
            mainProg.Run();
        }
        #endregion

        #region Constructors
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Default constructor.</summary>
        ///
        /// <authorAndDate>Mike, 3/23/2016.</authorAndDate>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        internal Program()
        {
            CHANNEL_NAME = new char[] { 'A', 'B', 'C', 'D' };
            m_sPluginPath = "";
            m_oJsrManager = null;
            m_oPulserRcvrs = null;
            m_nState = ConsoleState.STATE_INITIALIZE;
            m_bRunning = true;
            m_bErrorOccurred = false;
            m_nDiscoveryTimeout = DISCOVERY_WAIT_SECS;
            m_oSettings = null;
            m_DiscoveryEvent = new AutoResetEvent(false);
            m_logThread = new Thread(new ThreadStart(LogThreadProc));

            m_logThread.Name = "LOG_THREAD";
            m_logThread.Priority = ThreadPriority.BelowNormal;
            JSRDotNET_Logger.LogLevel = JSRDotNET_Logger.LOGLEVEL.DEFAULT;
            m_bKillLogThread = false;

        }
        #endregion

        bool m_bKillLogThread;
        TextWriter m_textWriterForLog=null;

        private void LogThreadProc()
        {
            int i;
            Console.WriteLine("Log Thread Started");
            while(!m_bKillLogThread)
            {
                for(i=0; i<10; i++)
                {
                   if(m_bKillLogThread)
                   {break;}
                   Thread.Sleep(100);
                }
                if(m_bKillLogThread)
                {
                    break;
                }
                string line;
                do
                {
                    line = JSRDotNET_Logger.DequeueEarliestLogline();

                    if((null!=line) && (null!=m_textWriterForLog))
                    {


                        if (line.Length > 4)
                        {
                            Console.WriteLine(line);
                            m_textWriterForLog.WriteLine(line);
                        }

                    }
                } while(null!=line);
                if(null != m_textWriterForLog)
                {
                    m_textWriterForLog.Flush();
                }

            }
            Console.WriteLine("Log Thread Ended");
        }

        #region Main Application Method
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Runs this object.</summary>
        ///
        /// <authorAndDate>Mike, 3/23/2016.</authorAndDate>
        ///
        /// <returns>true if it succeeds, false if it fails.</returns>
        /// 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        internal bool Run()
        {
            while (m_bRunning)
            {
                switch (m_nState)
                {
                    case ConsoleState.STATE_INITIALIZE:
                        if (Initialize())
                        {
                            m_nState = ConsoleState.STATE_SELECT_PLUGIN;
                        }
                        else
                        {
                            m_nState = ConsoleState.STATE_EXIT;
                        }
                        break;
                    case ConsoleState.STATE_SELECT_PLUGIN:
                        switch (SelectPluginToConfigure())
                        {
                            case PromptResult.SUCCESS:
                                // Plugin was selected, now prompt for open
                                // options
                                m_nState = ConsoleState.STATE_DISCOVERY;
                                break;
                            default:
                                // Otherwise, only place to go is to exit
                                m_nState = ConsoleState.STATE_EXIT;
                                break;
                        }
                        break;

                    case ConsoleState.STATE_DISCOVERY:
                        switch (DiscoverDevices())
                        {
                            case PromptResult.SUCCESS:
                                m_nState = ConsoleState.STATE_RUNNING;
                                break;
                            default:
                            case PromptResult.BACK:
                                m_nState = ConsoleState.STATE_SELECT_PLUGIN;
                                break;
                            case PromptResult.EXIT:
                                m_nState = ConsoleState.STATE_EXIT;
                                break;
                        }
                        break;

                    case ConsoleState.STATE_RUNNING:
                        switch (ShowMainMenu())
                        {
                            case PromptResult.BACK:
                                m_nState = ConsoleState.STATE_DISCOVERY;
                                break;
                            case PromptResult.EXIT:
                                m_nState = ConsoleState.STATE_EXIT;
                                break;
                            default:
                                break;
                        }
                        break;

                    case ConsoleState.STATE_EXIT:
                        Stop();
                        break;

                    default:
                        Console.WriteLine("Unhandled state: {0}", m_nState.ToString());
                        m_nState = ConsoleState.STATE_EXIT;
                        break;
                }
            }



            if (m_bErrorOccurred)
            {
                // On error let the user see the error before exit
                PromptForKeyToContinue("Press any key to exit...");
            }

            Shutdown();

            return false;
        }
        #endregion

        #region JSRDotNETManager Event Handlers
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Event handler for new instruments that were discovered.</summary>
        ///
        /// <authorAndDate>Mike, 3/23/2016.</authorAndDate>
        ///
        /// <param name="sender">The object that sent this object.</param>
        /// <param name="args">Parameters about the discovered instrument.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        internal void OnDiscoveryEvent(object sender, EventArgsManagerNotify args)
        {
            if ((m_oPulserRcvrs != null) && (args != null))
            {
                // When a new instrument is discovered, this event will be called.
                // The pulserReceivers in the instrument will be added to the list
                // of available pulser receivers.
                switch (args.NotifyType)
                {
                    case NOTIFY_TYPE.PULSER_RCVR_DISCOVERED:
                        if (args.PulserReceiverId != null)
                        {
                            //save the discovered Pulser-Receiver
                            SaveFoundPulserRcvr(args.PulserReceiverId);
                        }
                        else
                        {
                            ReportError(string.Format("PulserReceiver ID is empty for a {0} event",
                                args.NotifyType.ToString()));
                        }
                        break;
                    case NOTIFY_TYPE.DISCOVERY_COMPLETE:
                        Console.WriteLine("Discovery complete (" + args.PluginName + ")");
                        m_DiscoveryEvent.Set();
                        break;
                    case NOTIFY_TYPE.DISCOVERY_STARTED:
                        Console.WriteLine("Discovery started (" + args.PluginName + ")");
                        break;
                    default:
                    case NOTIFY_TYPE.ERROR:
                        string errMsg = null;
                        if (!string.IsNullOrWhiteSpace(args.ErrorMsg)) errMsg = args.ErrorMsg;
                        else if (!string.IsNullOrWhiteSpace(args.ErrorText)) errMsg = args.ErrorText;

                        if (errMsg != null)
                            ReportError(errMsg,
                                (args.DiscoverState == DiscoveryStateFlags.STOPPED_ON_ERR));
                        else
                            ReportError("Error occurred during discovery",
                                (args.DiscoverState == DiscoveryStateFlags.STOPPED_ON_ERR));

                        Console.WriteLine();
                        PromptForKeyToContinue();
                        if (args.DiscoverState == DiscoveryStateFlags.STOPPED_ON_ERR)
                        {
                            m_DiscoveryEvent.Set();
                        }
                        break;
                }
            }
        }
        #endregion

        #region Plugin Methods
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// Initialize the Console program and attempt to load the plugins from the configured
        /// plugin directory.
        /// </summary>
        ///
        /// <authorAndDate>Mike, 3/23/2016.</authorAndDate>
        ///
        /// <returns>true if it succeeds, false if it fails.</returns>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        internal bool Initialize()
        {
            bool result = false;

            if (m_nState == ConsoleState.STATE_INITIALIZE)
            {
                // The path to the directory that contains the Plugin dll files
                m_sPluginPath = PROGRAM_PLUGIN_PATH;
                Console.WriteLine("Plugins Path: " + m_sPluginPath);

                try
                {
                    // Create the pluginhelper and load the plugins
                    m_oJsrManager = new JSRDotNETManager(m_sPluginPath);
                    m_oJsrManager.LoadPlugins();
                    // Add all the discovered plugins to the managed plugin list
                    foreach (string pluginName in m_oJsrManager.GetPluginNames())
                    {
                        m_oJsrManager.AddManagedPlugin(pluginName);
                        if (pluginName == "JSRDotNET_DiagLib")
                        {
                            // Configure default options for the simulator
                            m_oJsrManager.AddPluginOpenOption(pluginName, 
                                InstrumentOpenCriteria.SHOW_SIMULATOR_WINDOWS, "NO");
                            m_oJsrManager.AddPluginOpenOption(pluginName, 
                                InstrumentOpenCriteria.CONNECT_ON_OPEN, "YES");
                        }
                    }
                    result = true;
                }
                catch (Exception ex)
                {
                    ReportException(ex, "Loading plugins failed");
                    Console.WriteLine();
                    PromptForKeyToContinue();
                }

                if (result)
                {
                    m_nState = ConsoleState.STATE_SELECT_PLUGIN;
                }
            }
            else
            {
                result = true;
            }

            return result;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// If the plugins were loaded successfully, then prompt the user for the plugin to use,
        /// the open options, query for the attached devices and prompt the user to select the
        /// device to use.
        /// </summary>
        ///
        /// <authorAndDate>Mike, 3/23/2016.</authorAndDate>
        ///
        /// <returns>bool if a module was selected successfully.</returns>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        internal PromptResult SelectPluginToConfigure()
        {
            PromptResult result = PromptResult.EXIT;

            if (m_oJsrManager != null)
            {
                // Grab an array of the plugin names
                string[] pluginNames = m_oJsrManager.GetPluginNames().ToArray();

                // Were any plugins found?
                if ((pluginNames == null) || (pluginNames.Length == 0))
                {
                    ReportError("No plugins found in PluginPath");
                    result = PromptResult.EXIT;
                }
                else
                {
                    while (result != PromptResult.SUCCESS)
                    {
                        IJSRDotNETLibMetadata pluginInfo = null;
                        int stgIdx = 0;
                        string[] prompts = new string[pluginNames.Length + 1];
                        string[] managedPlugins = m_oJsrManager.GetManagedPluginNames();
                        prompts[stgIdx++] = "Start Pulser-Receiver discovery";
                        for (int i = 0; i < pluginNames.Length; i++)
                        {
                            string pluginName = pluginNames[i];
                            // Retrieve the metadata for each plugin so the models supported can be displayed
                            pluginInfo = m_oJsrManager.GetPluginLibraryMetadata(pluginName);
                            if (pluginInfo != null)
                            {
                                string ifcs = "";
                                bool usePlugin = managedPlugins.Contains(pluginName);
                                foreach (CONNECTION_TYPE conn in pluginInfo.ConnectionType)
                                {
                                    ifcs += conn.ToString() + "|";
                                }
                                prompts[stgIdx++] = string.Format( "[{0}] {1} ({2}) - Supports {3}", 
                                    (usePlugin ? "*" : " "), pluginName,
                                    ifcs.Substring(0, ifcs.Length - 1), 
                                    String.Join(", ", pluginInfo.SupportedModels));
                            }
                        }
                        int pluginIdx = Prompt("Configure Plugins or Start Discovery", prompts, 
                            "PLUGIN", true, false);

                        if (pluginIdx == 0)
                        {
                            // Start discovery
                            result = PromptResult.SUCCESS;
                        }
                        else if ((pluginIdx >= 0) && (pluginIdx <= pluginNames.Length))
                        {
                            ConfigurePlugin(pluginNames[pluginIdx - 1]);
                        }
                        else if (pluginIdx == (int)PromptResult.EXIT)
                        {
                            return PromptResult.EXIT;
                        }
                    }
                }
            }
            // Default case is to exit
            return result;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// If the plugins were loaded successfully, then prompt the user for the plugin to use,
        /// the open options, query for the attached devices and prompt the user to select the
        /// device to use.
        /// </summary>
        ///
        /// <authorAndDate>Mike, 3/23/2016.</authorAndDate>
        ///
        /// <param name="openCriteria">The open criteria.</param>
        ///
        /// <returns>bool if a module was selected successfully.</returns>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        internal PromptResult ConfigurePlugin(string pluginName)
        {
            PromptResult result = PromptResult.EXIT;
            
            if (m_oJsrManager != null)
            {
                result = PromptResult.SUCCESS;

                bool usePlugin = m_oJsrManager.GetManagedPluginNames().Contains(pluginName);
                IJSRDotNETLibMetadata pluginInfo = m_oJsrManager.GetPluginLibraryMetadata(pluginName);
                InstrumentOpenCriteria openCriteria = null;
                int promptVal = (int)PromptResult.PROMPT;
                Dictionary<string, string[]> options = new Dictionary<string, string[]>();
                List<string> menuOptions = new List<string>();

                if (pluginInfo == null)
                {
                    ReportError("Unable to retrieve Plugin information: " + pluginName, false);
                    return PromptResult.BACK;
                }

                int numOptions = pluginInfo.OpenOptions.Length;

                // Determine the names and valid values for each of the open options
                for (int i = 0; i < numOptions; i++)
                {
                    string[] option = pluginInfo.OpenOptions[i].Split(':');
                    options.Add(option[0], option[1].Split(','));
                }
                string[] keys = options.Keys.ToArray();
                
                // Menu to prompt for the open option to modify and display the current value
                while (promptVal != (int)PromptResult.BACK)
                {
                    openCriteria = m_oJsrManager.GetPluginOpenOptions(pluginName);

                    menuOptions.Clear();
                    menuOptions.Add(string.Format("{0} this plugin", (usePlugin ? "DISABLE" : "ENABLE")));
                    if (openCriteria != null)
                    {
                        for (int i = 0; i < numOptions; i++)
                        {
                            string optionValue = openCriteria.GetOpenOption(keys[i]);

                            menuOptions.Add(string.Format("{0} {1}", keys[i],
                                (string.IsNullOrWhiteSpace(optionValue) ? "" : " (" + optionValue + ")")));
                        }
                    }

                    promptVal = Prompt("Configure " + pluginName + " plugin", menuOptions.ToArray(), "PLUGIN");
                    // Toggling ENABLE/DISABLE plugin
                    if (promptVal == 0)
                    {
                        if (usePlugin)
                        {
                            m_oJsrManager.RemoveManagedPlugin(pluginName);
                            usePlugin = false;
                        }
                        else if (!usePlugin)
                        {
                            m_oJsrManager.AddManagedPlugin(pluginName);
                            usePlugin = true;
                        }
                    }
                    // An open option was specified, change the value
                    else if ((openCriteria != null) && (promptVal > 0) && (promptVal <= menuOptions.Count))
                    {
                        int optionNum = promptVal - 1;
                        if (optionNum < numOptions)
                        {
                            if ((options[keys[optionNum]].Length == 1) &&
                                (options[keys[optionNum]][0].Substring(0, 3).Equals("int")))
                            {
                                int[] minMax = null;
                                string[] intParams = options[keys[optionNum]][0].Split('|');
                                if (intParams.Length > 1)
                                {
                                    minMax = new int[intParams.Length - 1];
                                    try
                                    {
                                        minMax[0] = Int32.Parse(intParams[1]);
                                        if (intParams.Length > 2)
                                        {
                                            minMax[1] = Int32.Parse(intParams[2]);
                                        }
                                    }
                                    catch (Exception)
                                    {
                                        minMax = null;
                                    }
                                }

                                int optionVal = -1;

                                if (PromptForInt("Enter " + keys[optionNum] + " value", "INTVAL", out optionVal, minMax) == PromptResult.SUCCESS)
                                {
                                    openCriteria.AddOpenOption(keys[optionNum], optionVal.ToString());
                                }
                            }
                            else
                            {
                                int optionVal = Prompt("Select " + keys[optionNum] + " options", options[keys[optionNum]], "OPTION");
                                if ((optionVal >= 0) && (optionVal < options[keys[optionNum]].Length))
                                {
                                    openCriteria.AddOpenOption(keys[optionNum], options[keys[optionNum]][optionVal]);
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                ReportError("Attempted to configure OpenOptions while instances are NULL");
            }

            return result;
        }
        #endregion

        #region Pulser Receiver Methods
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// Begins instrument detection with the selected plugin and allows the user to select
        /// the desired instrument to use.  If any OpenOptions were previously set, then these
        /// will be taken into account during instrument detection.
        /// </summary>
        ///
        /// <authorAndDate>Mike, 3/23/2016.</authorAndDate>
        ///
        /// <returns>true if the select operation was successful.</returns>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        internal PromptResult DiscoverDevices()
        {
            PromptResult result = PromptResult.EXIT;
            int keyCheck = 250;
            int sleepWait = (m_nDiscoveryTimeout * 1000);

            if (m_oJsrManager != null)
            {
                if (m_oJsrManager.GetManagedPluginNames().Length == 0)
                {
                    ReportError("No Plugins selected", false);
                    return PromptResult.BACK;
                }

                if (m_oPulserRcvrs == null)
                {
                    // Create a new pulser receiver object if null
                    m_oPulserRcvrs = new List<IPulserReceiverIdentity>();
                }
                else
                {
                    // Otherwise clear the list
                    m_oPulserRcvrs.Clear();
                }

                // If any pulser receivers were previously detected, then add them to the
                // pulser receivers list.
                if ((m_oJsrManager.GetPulserReceivers() != null)&&(m_oJsrManager.GetPulserReceivers().Length > 0))
                {
                    Console.WriteLine("Previously discovered devices:");
                    foreach (IPulserReceiverIdentity prId in m_oJsrManager.GetPulserReceivers())
                    {
                        SaveFoundPulserRcvr(prId);
                    }
                }

                // If a new one pops up during discovery, then handle the discovery
                // event.
                m_oJsrManager.NotifyEventHandler += this.OnDiscoveryEvent;

                // Turn on discovery once for the plugin, and we'll let it continue to
                // discover new devices.  If more are added later, then we can pick
                // them up if the user goes into this menu again.
                Console.WriteLine("Starting device discovery...   Press any key to stop.");
                try
                {
                    //m_oPlugin.InstrumentDiscoveryEnable = true;
                    m_oJsrManager.SetDiscoveryEnable(true);
                }
                catch(Exception ex)
                {
                    ReportException(ex, "Exception during device discovery.");
                    Console.WriteLine();
                    PromptForKeyToContinue();
                }

                // Wait until the designated discovery timer expires or the user
                // presses a key.
                while (sleepWait > 0)
                {
                    if (Console.KeyAvailable)
                    {
                        Console.ReadKey();
                        Console.WriteLine("Stopped manually.");
                        sleepWait = 0;
                    }
                    else if (m_DiscoveryEvent.WaitOne(keyCheck))
                    {
                        sleepWait = 0;
                    }
                    else
                    {
                        sleepWait -= keyCheck;
                        if (sleepWait <= 0)
                        {
                            Console.WriteLine("Complete - Timeout");
                        }
                    }
                }

                m_oJsrManager.NotifyEventHandler -= this.OnDiscoveryEvent;
                // Instrument Discovery is still enabled at this point....

                // Did we find any pulser receivers during discovery?
                if (m_oPulserRcvrs.Count > 0)
                {
                    Console.WriteLine("Pulser Receivers found: {0}", m_oPulserRcvrs.Count);
                    result = SelectPulserReceiver();
                }
                else
                {
                    Console.WriteLine("No devices found...");
                    result = PromptResult.BACK;
                }
            }
            else
            {
                ReportError("Attempted to discover devices while instances are NULL");
            }

            return result;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// Add a new pulser receiver channel to the list of available pulser receivers that can
        /// be configured.
        /// </summary>
        ///
        /// <authorAndDate>Mike, 3/23/2016.</authorAndDate>
        ///
        /// <param name="prId">The pulser receiver identity object that describes this pulser
        /// receiver.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        internal void SaveFoundPulserRcvr(IPulserReceiverIdentity prId)
        {
            // If the pulser receiver hasn't been added to the pulser receivers list
            // yet, then add it now and display a message.
            if ((m_oPulserRcvrs != null) && (prId != null) && (!m_oPulserRcvrs.Contains(prId)))
            {
                Console.WriteLine(" - Found " + GetPulserRcvrDescription(prId));
                m_oPulserRcvrs.Add(prId);
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// Returns a descriptive string for the given pulser receiver identity object.
        /// </summary>
        ///
        /// <authorAndDate>Mike, 3/23/2016.</authorAndDate>
        ///
        /// <param name="prId">The pulser receiver identity object that describes this pulser
        /// receiver.</param>
        ///
        /// <returns>A string description of the pulser receiver represented by prId.</returns>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        internal string GetPulserRcvrDescription(IPulserReceiverIdentity prId)
        {
            string descStg = "";

            if (prId != null)
            {
                descStg = prId.InstrumentId.GetDescription() + " - CHANNEL " +
                    CHANNEL_NAME[prId.PulserReceiverIndex];
            }

            return descStg;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// If one or more pulser receiver channels were found, then either select the first one
        /// (if only 1 was found) or prompt the user to select the pulser receiver they want to configure.
        /// </summary>
        ///
        /// <authorAndDate>Mike, 3/23/2016.</authorAndDate>
        ///
        /// <returns>true - pulser receiver was selected successfully.</returns>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        internal PromptResult SelectPulserReceiver()
        {
            PromptResult result = PromptResult.EXIT;
            int selectedIndex = -1;

            if ((m_oJsrManager != null) && (m_oPulserRcvrs != null) && (m_oPulserRcvrs.Count > 0))
            {
                if (m_oPulserRcvrs.Count == 1)
                {
                    // One pulser receiver found, select it by default
                    selectedIndex = 0;
                    result = PromptResult.SUCCESS;
                }
                else if (m_oPulserRcvrs.Count > 1)
                {
                    string[] prNameList = new string[m_oPulserRcvrs.Count];

                    for (int i = 0; i < m_oPulserRcvrs.Count; i++)
                    {
                        prNameList[i] = GetPulserRcvrDescription(m_oPulserRcvrs[i]);
                    }

                    int promptVal = Prompt("Select Pulser-Receiver to configure", prNameList, "DEVICE", true);
                    if (promptVal >= 0)
                    {
                        selectedIndex = promptVal;
                        result = PromptResult.SUCCESS;
                    }
                    else
                    {
                        result = (PromptResult)promptVal;
                    }
                }
                if ((result == PromptResult.SUCCESS) && (selectedIndex >= 0))
                {
                    Console.WriteLine("Selected Pulser-Receiver: " +
                        GetPulserRcvrDescription(m_oPulserRcvrs[selectedIndex]));
                    m_oJsrManager.SetCurrentPulserReceiver(m_oPulserRcvrs[selectedIndex]);
                }
                else
                {
                    // Reset current Pulser-Receiver
                    m_oJsrManager.SetCurrentPulserReceiver();
                }
            }
            else
            {
                ReportError("Attempted to select a Pulser-Receiver while instances are NULL");
            }

            return result;
        }
        #endregion

        #region Pulser Receiver Settings Methods
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Shows the main menu.</summary>
        ///
        /// <authorAndDate>Mike, 3/23/2016.</authorAndDate>
        ///
        /// <returns>true if the main menu was successfully run.</returns>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        internal PromptResult ShowMainMenu()
        {
            PromptResult result = PromptResult.BACK;
            int listAllOption = -1;
            List<string> menuOptions = new List<string>();

            if ((m_oJsrManager != null) && (m_oPulserRcvrs != null) && (m_oPulserRcvrs.Count > 0))
            {
                if (BuildSettingsList())
                {
                    if (m_oSettings.Keys.Count == 0)
                    {
                        Console.WriteLine("NO Parameters are supported by this Pulser-Receiver");
                        PromptForKeyToContinue();
                    }
                    else
                    {
                        result = PromptResult.SUCCESS;
                        menuOptions.AddRange(m_oSettings.Keys);
                        listAllOption = menuOptions.Count;
                        menuOptions.Add("Read all parameters");
                    }

                    while (result == PromptResult.SUCCESS)
                    {
                        int promptVal = Prompt("Select the parameter to configure", menuOptions.ToArray(),
                            "PARAM", true);
                        if (promptVal == listAllOption)
                        {
                            DisplayAllSettings();
                        }
                        else if (promptVal >= 0)
                        {
                            if (SettingsSubMenu(m_oSettings[menuOptions[promptVal]]) == PromptResult.EXIT)
                            {
                                result = PromptResult.EXIT;
                            }
                        }
                        else
                        {
                            result = (PromptResult)promptVal;
                        }
                    }
                    ClearSettingsList();
                }
            }

            return result;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Settings sub menu.</summary>
        ///
        /// <authorAndDate>mwb, 4/14/2016.</authorAndDate>
        ///
        /// <param name="property">The property.</param>
        ///
        /// <returns>A PromptResult.</returns>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        internal PromptResult SettingsSubMenu(PulserSettingInfo property)
        {
            PromptResult result = PromptResult.EXIT;
            string promptStg = "Choose cofiguration method for " + property.SettingName;

            if (property != null)
            {
                result = PromptResult.SUCCESS;
                while (result == PromptResult.SUCCESS)
                {
                    if (DisplayValue(property))
                    {
                        if (property.IsReadOnly)
                        {
                            PromptForKeyToContinue();
                            result = PromptResult.BACK;
                        }
                        else
                        {
                            switch (property.AccessType)
                            {
                                case PulserSettingAccessType.INDEX_ONLY:
                                    result = SetIndexValue(property);
                                    break;
                                case PulserSettingAccessType.DIRECT_ONLY:
                                    result = SetDirectValue(property);
                                    break;
                                case PulserSettingAccessType.INDEX_AND_DIRECT:
                                    string[] values = new string[] { "DIRECT", "INDEXED" };
                                    int promptVal = Prompt(promptStg, values, "METHOD", true, true);
                                    if ((promptVal >= 0) && (promptVal < values.Length))
                                    {
                                        if (values[promptVal] == "INDEXED")
                                        {
                                            result = SetIndexValue(property);
                                        }
                                        else
                                        {
                                            result = SetDirectValue(property);
                                        }
                                    }
                                    else
                                    {
                                        result = (PromptResult)promptVal;
                                    }
                                    break;
                                default:
                                    PromptForKeyToContinue();
                                    result = PromptResult.BACK;
                                    break;
                            }
                        }
                    }
                }
            }

            return result;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Builds the list of settings supported by the current device.</summary>
        ///
        /// <authorAndDate>mwb, 4/13/2016.</authorAndDate>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        internal bool BuildSettingsList()
        {
            bool result = false;

            if (m_oSettings == null) m_oSettings = new SortedDictionary<string, PulserSettingInfo>();

            try
            {
                if (m_oJsrManager.Id != null)
                {
                    ClearSettingsList();

                    // Grab the fields from the device and build a list of properties...
                    // If a pulser receiver hasn't been selected, an exception will be
                    // thrown.
                    foreach (PulserSettingInfo setting in m_oJsrManager.PulserSettings)
                    {
                        if (setting.IsPropertySupported())
                        {
                            string settingName = setting.SettingName;
                            if (m_oSettings.ContainsKey(settingName))
                            {
                                ReportError(
                                    string.Format("Settings '{0}' appears twice in SettingNames.Names list",
                                    settingName));
                                result = false;
                            }
                            else
                            {
                                m_oSettings.Add(settingName, setting);
                                result = true;
                            }
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                ReportException(ex, "Error building settings list");
            }

            return result;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Clears the settings list.</summary>
        ///
        /// <authorAndDate>mwb, 4/13/2016.</authorAndDate>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        internal void ClearSettingsList()
        {
            if (m_oSettings != null) m_oSettings.Clear();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Prompts the user to set the index value for the provided property.</summary>
        ///
        /// <authorAndDate>mwb, 4/15/2016.</authorAndDate>
        ///
        /// <param name="property">The property.</param>
        ///
        /// <returns>A PromptResult indicating the result of the operation.</returns>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        internal PromptResult SetIndexValue(PulserSettingInfo property)
        {
            PromptResult result = PromptResult.EXIT;
            string[] values = new string[0];
            int promptVal;
            string promptStg = "Select the new value for " + property.SettingName;
            if (property.Units != PROPERTY_UNITS.UNIT_NONE)
            {
                promptStg += " (" + property.GetUnitsAsString() + ")";
            }

            if ((property != null) && (property.IsIndexedPresentAndSupported))
            {
                values = property.GetValuesAsStrings();
                if (values != null)
                {
                    promptVal = Prompt(promptStg, values, "SETTING", true, true, true);
                    if ((promptVal >= 0) && (promptVal < values.Length))
                    {
                        property.IndexValue = promptVal;
                        result = PromptResult.SUCCESS;
                    }
                    else
                    {
                        result = (PromptResult)promptVal;
                    }
                }
                else
                {
                    PromptForKeyToContinue(
                        "Unable to retrieve index values.  Press any key to continue...");
                    result = PromptResult.BACK;
                }
            }
            return result;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Prompts the user to set the direct value for the provided property.</summary>
        ///
        /// <authorAndDate>mwb, 4/15/2016.</authorAndDate>
        ///
        /// <exception cref="ExceptionJSRDotNET">Thrown when a JSR Dot NET error condition occurs.</exception>
        ///
        /// <param name="property">The property.</param>
        ///
        /// <returns>A PromptResult indicating the result of the operation.</returns>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        internal PromptResult SetDirectValue(PulserSettingInfo property)
        {
            PromptResult result = PromptResult.BACK;
            string[] values = new string[0];
            int promptVal;
            string promptStg = "Select the new value for " + property.SettingName;
            if (property.Units != PROPERTY_UNITS.UNIT_NONE)
            {
                promptStg += " (" + property.GetUnitsAsString() + ")";
            }

            if ((property != null) && (property.IsDirectPresentAndSupported))
            {
                try
                {
                    switch (property.DataType)
                    {
                        case PulserPropertyDataType.ENUM:
                            object[] enumVals = property.EnumValues;
                            if ((enumVals != null) && (enumVals.Length > 0))
                            {
                                values = new string[enumVals.Length];
                                for (int i = 0; i < enumVals.Length; i++)
                                {
                                    values[i] = enumVals[i].ToString();
                                }
                                promptVal = Prompt(promptStg, values, "SETTING", true, true);
                                if ((promptVal >= 0) && (promptVal < values.Length))
                                {
                                    property.DirectValue = enumVals[promptVal];
                                    result = PromptResult.SUCCESS;
                                }
                                else
                                {
                                    result = (PromptResult)promptVal;
                                }
                            }
                            else
                            {
                                PromptForKeyToContinue(
                                    "Unable to retrieve Enum values.  Press any key to continue...");
                                result = PromptResult.BACK;
                            }
                            break;
                        case PulserPropertyDataType.BOOLEAN:
                            values = new string[] { "ENABLE", "DISABLE" };
                            promptVal = Prompt(promptStg, values, "SETTING", true, true);
                            if ((promptVal >= 0) && (promptVal < values.Length))
                            {
                                property.DirectValue = (values[promptVal] == "ENABLE");
                                result = PromptResult.SUCCESS;
                            }
                            else
                            {
                                result = (PromptResult)promptVal;
                            }
                            break;
                        case PulserPropertyDataType.INTEGER:
                            int[] intMinMax = new int[2];
                            int intVal = 0;
                            if (property.GetReturnType() == typeof(int))
                            {
                                intMinMax[0] = (int)property.DirectValueMin;
                                intMinMax[1] = (int)property.DirectValueMax;
                                result = PromptForInt(promptStg, "SETTING", out intVal, intMinMax, true, true);
                                if (result == PromptResult.SUCCESS)
                                {
                                    property.DirectValue = intVal;
                                }
                            }
                            else
                            {
                                throw new ExceptionJSRDotNET(ERROR_CODE.INTERNAL_SOFTWARE_FAULT,
                                    "Return type mismatch for " + property.SettingName);
                            }
                            break;
                        case PulserPropertyDataType.DOUBLE:
                            double[] dMinMax = new double[2];
                            double dVal = 0;
                            if (property.GetReturnType() == typeof(double))
                            {
                                dMinMax[0] = (double)property.DirectValueMin;
                                dMinMax[1] = (double)property.DirectValueMax;
                                result = PromptForDouble(promptStg, "SETTING", out dVal, dMinMax, true, true);
                                if (result == PromptResult.SUCCESS)
                                {
                                    property.DirectValue = dVal;
                                }
                            }
                            else
                            {
                                throw new ExceptionJSRDotNET(ERROR_CODE.INTERNAL_SOFTWARE_FAULT,
                                    "Return type mismatch for " + property.SettingName);
                            }
                            break;

                        default:
                            PromptForKeyToContinue();
                            result = PromptResult.BACK;
                            break;
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine("ERROR: " + ex.Message);
                    result = PromptResult.SUCCESS;  // Stay in the loop
                }
            }
            return result;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Displays a value described by property.</summary>
        ///
        /// <authorAndDate>mwb, 4/14/2016.</authorAndDate>
        ///
        /// <param name="property">The property.</param>
        ///
        /// <returns>true if it succeeds, false if it fails.</returns>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        internal bool DisplayValue(PulserSettingInfo property)
        {
            bool result = false;

            if (property != null)
            {
                Console.WriteLine("* {0} => {1} {2}", property.SettingName, property.GetValueAsString(), 
                    property.GetUnitsAsString());
                result = true;
            }

            return result;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Displays all settings.</summary>
        ///
        /// <authorAndDate>mwb, 4/14/2016.</authorAndDate>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        internal void DisplayAllSettings()
        {
            foreach (string setting in m_oSettings.Keys)
            {
                if (!DisplayValue(m_oSettings[setting]))
                {
                    break;
                }
            }
            PromptForKeyToContinue();
        }
        #endregion

        #region Helper Methods
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// Parse any of the passed in arguments and translate them to options for this
        /// application.
        /// </summary>
        ///
        /// <authorAndDate>Mike, 3/23/2016.</authorAndDate>
        ///
        /// <param name="args">An array of strings passed to the main() function.</param>
        ///
        /// <returns>true if it succeeds, false if it fails.</returns>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        internal bool ParseArgs(string[] args)
        {
            return true;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Stops this object.</summary>
        ///
        /// <authorAndDate>Mike, 3/23/2016.</authorAndDate>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        internal void Stop()
        {
            m_bRunning = false;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Shuts down this object and frees any resources it is using.</summary>
        ///
        /// <authorAndDate>Mike, 3/23/2016.</authorAndDate>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        internal void Shutdown()
        {
             m_bKillLogThread=true;
             Thread.Sleep(200);
            if (null != m_oJsrManager)
            {
                m_oJsrManager.Shutdown();
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Prompt for key to continue.</summary>
        ///
        /// <authorAndDate>mwb, 4/14/2016.</authorAndDate>
        ///
        /// <param name="message">The message.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        internal void PromptForKeyToContinue(string message = "Press any key to continue...")
        {
            Console.WriteLine();
            Console.WriteLine(message);
            Console.ReadKey();
        }
        #endregion

        #region Console Prompt Methods
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// Generic function for writing a prompt and reading in the selection.
        /// </summary>
        ///
        /// <authorAndDate>Mike, 3/23/2016.</authorAndDate>
        ///
        /// <exception cref="Exception">Thrown when an exception error condition occurs.</exception>
        ///
        /// <param name="instructions">A string containing instructions for the user.</param>
        /// <param name="options">An array of strings that contains a list of options to select.</param>
        /// <param name="prompt">The value to use for the prompt.</param>
        /// <param name="includeExit">Should the 'Exit' menu option be included?</param>
        /// <param name="includeBack">Should the 'Back' menu option be included?</param>
        /// <param name="allowPaged">true to allow, false to deny paged.</param>
        ///
        /// <returns>
        /// The 0-based index of the option selected or a Prompt Result indicating the action
        /// taken.
        /// </returns>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        internal int Prompt(string instructions, string[] options, string prompt, bool includeExit = false,
            bool includeBack = true, bool allowPaged = false)
        {
            if (options == null)
                throw new Exception("Prompt - options list is NULL");

            const int PAGE_SIZE = 15;
            const int PAGE_START_SIZE = 20;
            int result = (int)PromptResult.HELP;
            bool cont = true;
            uint optionsIdx = 0;
            bool isPaged = ((options.Length > PAGE_START_SIZE) && (allowPaged));
            bool showNext = false;
            bool showPrev = false;

            while (cont)
            {
                // Display the instructions and list of options
                if (result == (int)PromptResult.HELP)
                {
                    showNext = false;
                    showPrev = false;
                    Console.WriteLine();
                    Console.WriteLine(instructions + ":");
                    if (isPaged)
                    {
                        for (int c = 0; ((c < (options.Length - optionsIdx)) && (c < PAGE_SIZE)); c++)
                        {
                            Console.WriteLine("{0,4:D}. " + options[c + optionsIdx], (c + 1));
                        }
                        if (optionsIdx > 0)
                        {
                            showPrev = true;
                            Console.WriteLine("   {0}. Previous page", MENU_IDXPREV);
                        }
                        if ((options.Length - optionsIdx) >= PAGE_SIZE)
                        {
                            showNext = true;
                            Console.WriteLine("   {0}. Next page", MENU_IDXNEXT);
                        }
                        if ((optionsIdx >= 0) && (options.Length > PAGE_SIZE))
                        {
                            Console.WriteLine("   {0}. Show all options", MENU_IDXALL);
                        }
                    }
                    else
                    {
                        for (int c = 0; (c < options.Length); c++)
                        {
                            Console.WriteLine("{0,4:D}. " + options[c], (c + 1));
                        }
                    }
                    if (includeBack)
                    {
                        Console.WriteLine("   {0}. Previous menu", MENU_BACK);
                    }
                    if (includeExit)
                    {
                        Console.WriteLine("   {0}. Exit application", MENU_EXIT);
                    }
                    Console.WriteLine();
                    result = (int)PromptResult.PROMPT;
                }

                // Read in the option selection and translate
                Console.Write(prompt + "> ");
                string resultStg = Console.ReadLine();
                if (resultStg.Length > 0)
                {
                    if ((showNext)&&(resultStg.ToLower()[0] == MENU_IDXNEXT))
                    {
                        if ((optionsIdx + PAGE_SIZE) < options.Length)
                        {
                            optionsIdx += PAGE_SIZE;
                        }
                        result = (int)PromptResult.HELP;
                    }
                    else if ((showPrev)&&(resultStg.ToLower()[0] == MENU_IDXPREV))
                    {
                        if (optionsIdx < PAGE_SIZE)
                        {
                            optionsIdx = 0;
                        }
                        else
                        {
                            optionsIdx -= PAGE_SIZE;
                        }
                        result = (int)PromptResult.HELP;
                    }
                    else if ((isPaged) && (resultStg.ToLower()[0] == MENU_IDXALL))
                    {
                        isPaged = false;
                        optionsIdx = 0;
                        result = (int)PromptResult.HELP;
                    }
                    else if ((includeBack) && (resultStg.ToLower()[0] == MENU_BACK))
                    {
                        result = (int)PromptResult.BACK;
                        cont = false;
                    }
                    else if (resultStg.ToLower()[0] == MENU_HELP)
                    {
                        result = (int)PromptResult.HELP;
                    }
                    else if ((includeExit) && (resultStg.ToLower()[0] == MENU_EXIT))
                    {
                        result = (int)PromptResult.EXIT;
                        cont = false;
                    }
                    else
                    {
                        int value = -1;
                        if (int.TryParse(resultStg, out value))
                        {
                            if (isPaged)
                            {
                                if ((value > 0) && (value <= PAGE_SIZE))
                                {
                                    result = (value + (int)optionsIdx - 1);
                                    cont = false;
                                }
                                else
                                {
                                    value = -1;
                                }
                            }
                            else if ((value > 0) && (value <= options.Length))
                            {
                                result = value - 1;
                                cont = false;
                            }
                            else
                            {
                                value = -1;
                            }
                        }
                        if (value <= 0)
                        {
                            Console.WriteLine(" - Invalid: " + resultStg);
                        }
                    }
                }
            }

            return result;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// Generic function for writing a prompt and reading in the selection.
        /// </summary>
        ///
        /// <authorAndDate>Mike, 3/23/2016.</authorAndDate>
        ///
        /// <param name="instructions">A string containing instructions for the user.</param>
        /// <param name="prompt">The value to use for the prompt.</param>
        /// <param name="includeExit">Should the 'Exit' menu option be included?</param>
        /// <param name="includeBack">Should the 'Back' menu option be included?</param>
        ///
        /// <returns>A Prompt Result indicating True/False or the action taken.</returns>
        ///
        /// ### <param name="options">An array of strings that contains a list of options to select.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        internal PromptResult PromptForYesNo(string instructions, string prompt, bool includeExit = false,
            bool includeBack = true)
        {
            const string YES_TEXT = "YES";
            const string NO_TEXT = "NO";

            string[] yesNoList = new string[] { NO_TEXT, YES_TEXT };

            PromptResult result = PromptResult.BACK;
            int promptVal = Prompt(instructions, yesNoList, prompt, includeExit, includeBack);

            if ((promptVal >= 0) && (promptVal < yesNoList.Length))
            {
                if (yesNoList[promptVal] == YES_TEXT)
                    result = PromptResult.TRUE;
                else
                    result = PromptResult.FALSE;
            }
            else
            {
                result = PromptResult.BACK;
            }

            return result;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// Generic function for writing a prompt and reading in the menu selection or prompting
        /// for an integer value.
        /// </summary>
        ///
        /// <authorAndDate>Mike, 3/23/2016.</authorAndDate>
        ///
        /// <param name="instructions">A string containing instructions for the user.</param>
        /// <param name="prompt">The value to use for the prompt.</param>
        /// <param name="newVal">[out] The integer value entered in the prompt.</param>
        /// <param name="minMax">An array containing the {min, max} or 0 to {max} values.</param>
        /// <param name="includeExit">Should the 'Exit' menu option be included?</param>
        /// <param name="includeBack">Should the 'Back' menu option be included?</param>
        ///
        /// <returns>A Prompt Result indicating the action taken.</returns>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        internal PromptResult PromptForInt(string instructions, string prompt, out int newVal,
            int[] minMax = null, bool includeExit = false, bool includeBack = true)
        {
            PromptResult result = PromptResult.HELP;
            bool cont = true;

            int value = 0;
            newVal = 0;

            while (cont)
            {
                // Display the instructions and list of options
                if (result == PromptResult.HELP)
                {
                    Console.WriteLine();
                    if (minMax == null)
                    {
                        Console.WriteLine(instructions + ":");
                    }
                    else if (minMax.Length > 1)
                    {
                        Console.WriteLine(instructions + " - Range [" + minMax[0] + " .. " + minMax[1] + "]:");
                        Console.WriteLine("   a. MIN ({0})", minMax[0]);
                        Console.WriteLine("   b. MAX ({0})", minMax[1]);
                    }
                    else
                    {
                        Console.WriteLine(instructions + " - Range [0 .. " + minMax[0] + "]:");
                        Console.WriteLine("   a. MAX ({0})", minMax[0]);
                    }
                    if (includeBack)
                    {
                        Console.WriteLine("   {0}. Previous menu", MENU_BACK);
                    }
                    if (includeExit)
                    {
                        Console.WriteLine("   {0}. Exit application", MENU_EXIT);
                    }
                    Console.WriteLine();
                    result = PromptResult.PROMPT;
                }

                // Read in the option selection and translate
                Console.Write(prompt + "> ");
                string resultStg = Console.ReadLine();
                if (resultStg.Length > 0)
                {
                    if ((includeBack) && (resultStg.ToLower()[0] == MENU_BACK))
                    {
                        result = PromptResult.BACK;
                        cont = false;
                    }
                    else if (resultStg.ToLower()[0] == MENU_HELP)
                    {
                        result = PromptResult.HELP;
                    }
                    else if ((minMax != null) && (minMax.Length > 0) && (resultStg.ToLower()[0] == 'a'))
                    {
                        // This is the MAX value if length=1 or MIN value if length=2
                        newVal = minMax[0];
                        cont = false;
                        result = PromptResult.SUCCESS;
                    }
                    else if ((minMax != null) && (minMax.Length > 1) && (resultStg.ToLower()[0] == 'b'))
                    {
                        newVal = minMax[1];
                        cont = false;
                        result = PromptResult.SUCCESS;
                    }
                    else if ((includeExit) && (resultStg.ToLower()[0] == MENU_EXIT))
                    {
                        result = PromptResult.EXIT;
                        cont = false;
                    }
                    else
                    {
                        if (Int32.TryParse(resultStg, out value))
                        {
                            if ((minMax == null) ||
                                ((minMax.Length == 1) && (value >= 0) && (value <= minMax[0])) ||
                                ((minMax.Length == 2) && (value >= minMax[0]) && (value <= minMax[1])))
                            {
                                newVal = value;
                                cont = false;
                                result = PromptResult.SUCCESS;
                            }
                            else
                            {
                                Console.WriteLine(" - Invalid: " + resultStg);
                            }
                        }
                        else
                        {
                            Console.WriteLine(" - Invalid: " + resultStg);
                        }
                    }
                }
            }

            return result;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// Generic function for writing a prompt and reading in the menu selection or prompting
        /// for a double value.
        /// </summary>
        ///
        /// <authorAndDate>Mike, 3/23/2016.</authorAndDate>
        ///
        /// <param name="instructions">A string containing instructions for the user.</param>
        /// <param name="prompt">The value to use for the prompt.</param>
        /// <param name="newVal">[out] The new value.</param>
        /// <param name="minMax">The minimum maximum.</param>
        /// <param name="includeExit">Should the 'Exit' menu option be included?</param>
        /// <param name="includeBack">Should the 'Back' menu option be included?</param>
        ///
        /// <returns>A Prompt Result indicating the action taken.</returns>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        internal PromptResult PromptForDouble(string instructions, string prompt, out double newVal,
            double[] minMax = null, bool includeExit = false, bool includeBack = true)
        {
            PromptResult result = PromptResult.HELP;
            bool cont = true;
            double value = 0D;
            newVal = 0D;

            while (cont)
            {
                // Display the instructions and list of options
                if (result == PromptResult.HELP)
                {
                    Console.WriteLine();
                    if (minMax == null)
                    {
                        Console.WriteLine(instructions + ":");
                    }
                    else if (minMax.Length > 1)
                    {
                        Console.WriteLine(instructions + " - Range [" + minMax[0] + " .. " + minMax[1] + "]:");
                        Console.WriteLine("   a. MIN ({0})", minMax[0]);
                        Console.WriteLine("   b. MAX ({0})", minMax[1]);
                    }
                    else
                    {
                        Console.WriteLine(instructions + " - Range [0 .. " + minMax[0] + "]:");
                        Console.WriteLine("   a. MAX ({0})", minMax[0]);
                    }
                    if (includeBack)
                    {
                        Console.WriteLine("   {0}. Previous menu", MENU_BACK);
                    }
                    if (includeExit)
                    {
                        Console.WriteLine("   {0}. Exit application", MENU_EXIT);
                    }
                    Console.WriteLine();
                    result = PromptResult.PROMPT;
                }

                // Read in the option selection and translate
                Console.Write(prompt + "> ");
                string resultStg = Console.ReadLine();
                if (resultStg.Length > 0)
                {
                    if ((includeBack) && (resultStg.ToLower()[0] == MENU_BACK))
                    {
                        result = PromptResult.BACK;
                        cont = false;
                    }
                    else if (resultStg.ToLower()[0] == MENU_HELP)
                    {
                        result = PromptResult.HELP;
                    }
                    else if ((minMax != null) && (minMax.Length > 0) && (resultStg.ToLower()[0] == 'a'))
                    {
                        newVal = minMax[0];
                        cont = false;
                        result = PromptResult.SUCCESS;
                    }
                    else if ((minMax != null) && (minMax.Length > 1) && (resultStg.ToLower()[0] == 'b'))
                    {
                        newVal = minMax[1];
                        cont = false;
                        result = PromptResult.SUCCESS;
                    }
                    else if ((includeExit) && (resultStg.ToLower()[0] == MENU_EXIT))
                    {
                        result = PromptResult.EXIT;
                        cont = false;
                    }
                    else
                    {
                        if (Double.TryParse(resultStg, out value))
                        {
                            if ((minMax == null) ||
                                ((minMax.Length == 1) && (value >= 0) && (value <= minMax[0])) ||
                                ((minMax.Length == 2) && (value >= minMax[0]) && (value <= minMax[1])))
                            {
                                newVal = value;
                                cont = false;
                                result = PromptResult.SUCCESS;
                            }
                            else
                            {
                                Console.WriteLine(" - Invalid: " + resultStg);
                            }
                        }
                        else
                        {
                            Console.WriteLine(" - Invalid: " + resultStg);
                        }
                    }
                }
            }

            return result;
        }
        #endregion

        #region Error Reporting Methods
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Reports an exception.</summary>
        ///
        /// <authorAndDate>mwb, 4/13/2016.</authorAndDate>
        ///
        /// <param name="ex">The exception that occurred.</param>
        /// <param name="prefix">An optional prefix string for the message.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        internal void ReportException(Exception ex, string prefix = "")
        {
            if (string.IsNullOrWhiteSpace(prefix))
            {
                ReportError(ex.Message);
            }
            else
            {
                ReportError(prefix + ": " + ex.Message);
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Reports an error to the console and sets the state to EXIT.</summary>
        ///
        /// <authorAndDate>mwb, 4/13/2016.</authorAndDate>
        ///
        /// <param name="message">The message.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        internal void ReportError(string message, bool exit = true)
        {
            Console.WriteLine("ERROR: " + message);
            if (exit)
            {
                m_nState = ConsoleState.STATE_EXIT;
                m_bErrorOccurred = true;
            }
        }
        #endregion
    }
}
