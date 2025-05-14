////////////////////////////////////////////////////////////////////////////////////////////////////
// JSRDotNET_ConsoleExample
// 
// summary: A simple program which opens the first DPR instrument found on a specified serial port
// and dumps info about the hardware for each pulser receiver on it.
// 
// Copyright (C) 2016, Imaginant Inc.
// 
// See the file "LICENSE_EXAMPLES.TXT" for the full license governing this code. 
////////////////////////////////////////////////////////////////////////////////////////////////////
using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Reflection;
using System.Threading;
using JSRDotNETSDK;


namespace JSRDotNET_ConsoleExample
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// A simple program which opens the first DPR instrument found on a specified serial port
    /// and dumps info about the hardware for each pulser receiver on it.   
    /// </summary>
    ///
    /// <authorAndDate>kds, 8/7/2015.</authorAndDate>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class Program
    {
        #region Local Variables
        const string DPR_PLUGIN_NAME = "JSRDotNET_DPR";

        /// <summary>The path to the JSRDotNETSDK Instrument Plugins</summary>
        string PROGRAM_PLUGIN_PATH = System.IO.Path.GetDirectoryName(
            System.Reflection.Assembly.GetExecutingAssembly().Location) + @"\..\..\..\..\Plugins";

        /// <summary>The name of serial port chosen by the user.</summary>
        string m_strSerialPort;

        /// <summary>JSRDotNETManager instance that manages the plugins.</summary>
        JSRDotNETManager m_jsrManager;

        /// <summary>Event for notifying when a device has been discovered.</summary>
        AutoResetEvent m_evtDiscoveredInstrument = new AutoResetEvent(false);

        /// <summary>The discovery result to hold the first discovered device.</summary>
        EventArgsManagerNotify m_discoveryResult = null;
        #endregion

        #region Main() Entry Function
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Main entry-point for this application.</summary>
        ///
        /// <authorAndDate>kds, 8/7/2015.</authorAndDate>
        ///
        /// <param name="args">The arguments - not used.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static void Main(string[] args)
        {
            Program program = new Program();
            // Initialize the JSRDotNETManager and load the plugins
            if (program.initializePlugins())
            {
                // This example is using a specific plugin, 
                program.run(DPR_PLUGIN_NAME);
            }
            // All done... time to exit
            program.Shutdown();
            Console.WriteLine();
            Console.WriteLine("Press any key to exit program.");
            Console.ReadKey();
        }
        #endregion

        #region Constructors
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// Default constructor.  This initializes some of the parameters. Call
        /// initalizePlugins() to initialize the manager and load the plugins.
        /// </summary>
        ///
        /// <authorAndDate>kds, 8/7/2015.</authorAndDate>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        protected Program()
        {
            m_strSerialPort = string.Empty;
            m_evtDiscoveredInstrument = new AutoResetEvent(false);
            m_discoveryResult = null;
        }
        #endregion

        #region Main Application Method
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>   After the user chooses a serial port, the DPR library is used to discover the
        /// first instrument found on a serial port.  Upon discovery, the info about the hardware of all
        /// pulser receivers on that instrument is dumped to the console. </summary>
        ///
        /// <created>   kds, 8/7/2015. </created>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void run(string strPlugin)
        {
            int nIter=0;
            IJSRDotNETLibMetadata libMeta = null;

            // Make sure the Manager has been initialized and plugins were loaded.  Noted that the term
            // 'loaded' here means only the that the plugins have been found and the metadata read.
            // No instances of the plugin classes have been instantiated, no code from the plugins 
            // has executed at this point.
            if ((m_jsrManager == null)||(!m_jsrManager.PluginsLoaded))
            {
                Console.WriteLine("ERROR: JSRDotNETManager has not been initialized");
                return;
            }

            // Manager is not null and plugins were loaded.  Try to get the Plugin 
            // Metadata structure that contains information about the desired plugin.
            // If this returns null, then either the plugin was not in the plugin
            // directory or was not successfully loaded.
            libMeta = m_jsrManager.GetPluginLibraryMetadata(strPlugin);
            if (libMeta == null)
            {
                Console.WriteLine("ERROR: '" + strPlugin + "' Instrument Plugin was not found or loaded");
                return;
            }

            // Assuming the DPR plugin is in this directory, try to add it to the manager's list of
            // Managed Plugins - any plugins you want to work with will need to be added
            // to this list.
            m_jsrManager.AddManagedPlugin(strPlugin);

            // If the desired plugin uses a serial port, then prompt for the serial port
            // to use when discovering devices - if not set, then all serial ports will
            // be used.
            if (m_jsrManager.GetPluginLibraryMetadata(strPlugin).ConnectionType.Contains(CONNECTION_TYPE.SERIALPORT))
            {
                // Prompt the user for a serial port to use
                m_strSerialPort = chooseSerialPort();

                // Exit if no serial port was selected
                if (string.IsNullOrWhiteSpace(m_strSerialPort)) return;

                Console.WriteLine();
                Console.WriteLine("Serial port chosen: " + m_strSerialPort);

                // Get the Plugin Open Options for the DPR (Serial) Plugin
                InstrumentOpenCriteria ioc = m_jsrManager.GetPluginOpenOptions(strPlugin);

                // Clear any existing items in the ports lists
                ioc.PortsToExclude.Clear();
                ioc.PortsToInclude.Clear();

                // Add the serial port to include
                ioc.AddPortToInclude(m_strSerialPort);
            }

            // Subscribe to discovery events
            Console.WriteLine("Setting up discovery event handler");
            m_jsrManager.NotifyEventHandler += ManagerNotifyHandler;
            Console.WriteLine();

            // Enable device discovery.  Once this is true, the library will begin searching.  When a device
            // is discovered, the ManagerNotifyHandler below will get called.  If multiple plugins were added
            // to the Managed Plugins list in the Manager, then each plugin will report any discovered devices
            // while Discovery Enable is set to true.
            Console.WriteLine("Searching for instruments on " + m_strSerialPort);
            m_jsrManager.SetDiscoveryEnable(true);

            // Wait for up to 30 seconds for handler to set the event.
            while (!m_evtDiscoveredInstrument.WaitOne(500) && (nIter < 60))
            {
                Console.Write(".");
                nIter++;
            }

            // We were notified that a device (or error) was found.  Stop the discovery process.
            Console.WriteLine();
            Console.WriteLine("Turning off discovery of instruments.");
            Console.WriteLine();
            m_jsrManager.SetDiscoveryEnable(false);
            m_jsrManager.NotifyEventHandler -= ManagerNotifyHandler;

            // If the result never got set, then we must have timed out without getting any discovery event.
            if (null == m_discoveryResult)
            {
                Console.WriteLine("Timed out waiting for an instrument to be discovered.");
            }
            // Check to see if an Error occurred during discovery
            else if (m_discoveryResult.NotifyType == NOTIFY_TYPE.ERROR)
            {
                // An exception happened, so send the error info to the console.
                ReportDiscoveryException(m_discoveryResult);
            }
            else
            {
                // Report the model and serial number of instrument and the slot number (index)
                // of the Pulser/Receiver in this device.
                Console.WriteLine(string.Format("Found device - Instrument: Model: '{0}' Serial: '{1}' - Index: {2}",
                    m_discoveryResult.Model, m_discoveryResult.Serial, m_discoveryResult.PRIndex));

                // Since the Manager reports a single event per pulser/receiver, there
                // is only one device to report.
                Console.WriteLine();
                Console.WriteLine("Pulser Receiver Information:");
                // Ask the library for a IPulserReceiver object for each pulser
                IPulserReceiver pulser = m_jsrManager.GetPulserReceiver(m_discoveryResult.PulserReceiverId);
                if (pulser != null)
                {
                    string[] infoStrings = pulser.Info;
                    // The Pulser Info is a series of strings that provides information about the 
                    // instrument and pulser.  The format of this array is:
                    //   info[idx] = field description
                    //   info[idx + 1] = field value
                    // where idx starts at 0 and is all the even indices up until the end of the array.
                    for (int i = 0; ((infoStrings != null)&&(i < infoStrings.Length)); i += 2)
                    {
                        Console.WriteLine(string.Format("  * {0, -25}- {1, -20}",
                            infoStrings[i], infoStrings[i + 1].Trim()));
                    }
                }
            }
        }
        #endregion

        #region JSRDotNETManager Event Handlers
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// Handler, called when the instrument discovered, or if an exception occurs during the
        /// discovery process.  This implementation is designed for the simplest possible
        /// application which expects just one device.  It sets the info to a field of the class
        /// then sets the event to let the rest of the program know that something was called.
        /// </summary>
        ///
        /// <authorAndDate>kds, 8/7/2015.</authorAndDate>
        ///
        /// <param name="sender">Source of the event.</param>
        /// <param name="args">The arguments.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void ManagerNotifyHandler(object sender, EventArgsManagerNotify args)
        {
            bool notifyMain = false;

            switch (args.NotifyType)
            {
                case NOTIFY_TYPE.DISCOVERY_COMPLETE:
                    notifyMain = true;              // Notify the main thread
                    Console.WriteLine("Event: " + args.NotifyType.ToString());
                    break;

                case NOTIFY_TYPE.DISCOVERY_STARTED:
                    Console.WriteLine("Event: " + args.NotifyType.ToString());
                    break;

                case NOTIFY_TYPE.PULSER_RCVR_DISCOVERED:
                    // Once we have the first discovered device, we don't care about any more
                    // reported devices.
                    if (m_discoveryResult == null)
                    {
                        m_discoveryResult = args;   // Save the event with device info
                        notifyMain = true;          // Notify the main thread
                    }
                    break;

                case NOTIFY_TYPE.ERROR:
                    // An error occurred - print a simple message to the console
                    Console.WriteLine("Event: " + args.NotifyType.ToString());
                    m_discoveryResult = args;       // Save the event with the error info
                    notifyMain = true;              // Notify the main thread
                    break;

                default:
                    // Do nothing
                    break;
            }

            // Notify the main thread that we have a device
            if (notifyMain) m_evtDiscoveredInstrument.Set();
        }
        #endregion

        #region Helper Functions
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Initializes the JSRDotNETManager and loads plugins.</summary>
        ///
        /// <authorAndDate>mwb, 5/3/2016.</authorAndDate>
        ///
        /// <returns>true if it succeeds, false if it fails.</returns>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        protected bool initializePlugins()
        {
            bool bRet = false;
            try
            {
                // Create the JSRDotNETManager instance - the constructor can optionally take the
                // plugin path as a parameter.  This example sets the Plugin Path after 
                // construction, but before LoadPlugins() is called.  The Plugin Path cannot be
                // set once LoadPlugins() is called.
                m_jsrManager = new JSRDotNETManager();

                // Create a path string to search for the Instrument Plugins.  This path is built using
                // a relative path from the location where the application is executing from: e.g. 
                // "JSRDotNET_ConsoleExample/bin/Debug"
                m_jsrManager.PluginPath = PROGRAM_PLUGIN_PATH;
                Console.WriteLine("Plugins Path: " + m_jsrManager.PluginPath);
                Console.WriteLine();

                // Discover the instrument plugins in the PluginPath directory
                m_jsrManager.LoadPlugins();
                bRet = true;
            }
            catch (Exception ex)
            {
                // An exception occurred, print a message and return false
                Console.WriteLine("ERROR: Manager/Plugin Initialization failed: " + ex.Message);
            }

            return bRet;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Prompts the user to Choose serial port.</summary>
        ///
        /// <authorAndDate>kds, 8/7/2015.</authorAndDate>
        ///
        /// <returns>A string.  Or empty string if none found.</returns>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        string chooseSerialPort()
        {
            string strRet = string.Empty;
            Console.WriteLine("This program tests basic communication with a DPR series instrument.");
            Console.WriteLine("Available Serial Ports:");
            Console.WriteLine();

            // Create a list of the available serial ports - we are using a list for sorting
            List<string> arrSerialPorts = new List<string>(SerialPort.GetPortNames());
            arrSerialPorts.Sort();                  // Sort the list
            if (arrSerialPorts.Count < 1)           // Were any ports found?
            {
                Console.WriteLine("No serial ports detected on this computer.");
                return string.Empty;
            }
            else if (arrSerialPorts.Count == 1)     // Select device if there's only 1
            {
                strRet = arrSerialPorts[0];
                Console.WriteLine(string.Format("1). {0}", strRet));
                return strRet;
            }
            else                                    // Need to prompt for specific port
            {
                // Display the list of available ports (with a number prefix)
                for (int i = 0; i < arrSerialPorts.Count; i++)
                {
                    Console.WriteLine(string.Format("{0}). {1}", i + 1, arrSerialPorts[i]));
                }

                bool complete = false;
                string strInput;
                int portId = -1;

                Console.WriteLine();
                Console.WriteLine("Select serial port to use: (or 'x' to exit)");
                // Prompt for the port to use - stay in loop until X or valid entry
                while (!complete)
                {
                    Console.Write("PORT> ");
                    strInput = Console.ReadLine().Trim().ToUpper();
                    // Did we receive a request to exit?
                    if (strInput[0] == 'X')
                    {
                        strRet = string.Empty;
                        complete = true;
                    }
                    // We are using TryParse so we don't have to work with Exceptions
                    else if (int.TryParse(strInput, out portId))
                    {
                        // Array bounds checking (remember, portId is 1-based)
                        if ((portId > 0) && (portId <= arrSerialPorts.Count))
                        {
                            // it's valid, save the selected serial port string
                            strRet = arrSerialPorts[portId - 1];
                            complete = true;
                        }
                    }
                    if (!complete) Console.WriteLine(">> Invalid Entry");
                }
            }

            return strRet;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Shuts down this object and frees any resources it is using.</summary>
        ///
        /// <authorAndDate>mwb, 5/4/2016.</authorAndDate>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        protected void Shutdown()
        {
            if (m_jsrManager != null)
            {
                Console.WriteLine();
                Console.WriteLine(string.Format("Shutting Down Library to free resources and stop any threads."));
                m_jsrManager.Shutdown();
                m_jsrManager = null;
            }
        }
        #endregion

        #region Error Reporting Methods
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// This gets called to report that an exception occurred during device discovery.
        /// </summary>
        ///
        /// <authorAndDate>kds, 8/7/2015.</authorAndDate>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void ReportDiscoveryException(EventArgsManagerNotify args)
        {
            if (args.NotifyType == NOTIFY_TYPE.ERROR)
            {
                // An exception happened, so print the error info in the console.
                Console.WriteLine("ERROR: Exception occurred during discovery: " + args.ErrorMsg);
                Console.WriteLine("       " + args.ErrorText);
            }
        }
        #endregion
    }
}
