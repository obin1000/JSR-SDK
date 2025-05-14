////////////////////////////////////////////////////////////////////////////////////////////////////
// JSRDotNET_Sample
// 
// summary: The source of the sample application that is described in the 
// JSRDotNETSDK_QuickStart documentation.
// 
// Copyright (C) 2016, Imaginant Inc.
// 
// See the file "LICENSE_EXAMPLES.TXT" for the full license governing this code. 
////////////////////////////////////////////////////////////////////////////////////////////////////
using System;
using System.Reflection;
using System.Threading;
using JSRDotNETSDK;

namespace JSRDotNET_Sample
{
    class SampleProgram
    {
        #region Local Variables
        /// <summary>true if error occurred.</summary>
        private bool m_bErrorOccurred = false;

        /// <summary>The wait event used for device discovery</summary>
        private AutoResetEvent m_WaitEvent = new AutoResetEvent(false);
        #endregion

        #region Main() Entry Function

        ///////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Main entry-point for this application.</summary>
        ///
        /// <authorAndDate>mwb, 5/6/2016.</authorAndDate>
        ///
        /// <param name="args">Array of command-line argument strings.</param>
        ///////////////////////////////////////////////////////////////////////////////////////////////
        static void Main(string[] args)
        {
            SampleProgram myProgram = new SampleProgram();
            // Start the program execution
            myProgram.Run();
        }
        #endregion

        #region Main Application Method

        ///////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Runs this object.</summary>
        ///
        /// <authorAndDate>mwb, 5/6/2016.</authorAndDate>
        ///////////////////////////////////////////////////////////////////////////////////////////////
        internal void Run()
        {
            // We need the path to the plugins directory that contains the JSRDotNET
            // plugins. In this case, the plugins are in a subdirectory where the
            // executable is located. Use the Assembly functions to get the application
            // directory.
            string pluginPath = System.IO.Path.GetDirectoryName(
               Assembly.GetExecutingAssembly().Location) + @"\plugins";
            // Create a new JSRDotNETManager using the default constructor
            JSRDotNETManager jsrManager = new JSRDotNETManager(pluginPath);

            // OR

            // Create a new JSRDotNETManager using the default constructor
            //            JSRDotNETManager jsrManager_alt = new JSRDotNETManager();
            //            jsrManager_alt.PluginPath = pluginPath;

            try
            {
                // Load all of the Plugin dlls - this may throw an exception
                // if there is an error with the path.
                jsrManager.LoadPlugins();
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error loading plugins: " + ex.Message);
            }
            // At this point the jsrManager has a reference to the plugins that were
            // discovered in the given directory.

            // Get the list of plugins found (these are just the names)
            foreach (string name in jsrManager.GetPluginNames())
            {
                Console.WriteLine("Found plugin: '" + name + "'");
                // Get the metadata (info) about the plugin
                IJSRDotNETLibMetadata pluginInfo = jsrManager.GetPluginLibraryMetadata(name);
                // Print some info about the plugin
                Console.WriteLine(" - " + pluginInfo.FriendlyName + " - " +
                   string.Join(", ", pluginInfo.SupportedModels) +
                   " (" + string.Join(", ", pluginInfo.ConnectionType) + ")");

                jsrManager.AddManagedPlugin(name);
            }

            // Add our NotifyEventHander method to the jsrManager
            jsrManager.NotifyEventHandler += this.OnJsrManagerNotify;

            // Start the device discovery
            jsrManager.SetDiscoveryEnable(true);

            Console.WriteLine();
            Console.WriteLine("Waiting for devices...  Press any key to stop");
            Console.WriteLine();

            int sleepWait = 20000;

            while (sleepWait > 0)
            {
                sleepWait -= 100;
                if ((m_WaitEvent.WaitOne(100)) || (Console.KeyAvailable))
                    sleepWait = 0;
            }
            while (Console.KeyAvailable)
                Console.ReadKey(true);


            if (!m_bErrorOccurred)
            {
                IInstrumentIdentity[] instrList = jsrManager.GetInstruments();

                if ((instrList != null)&&(0 < instrList.Length))
                {
                    foreach (IInstrumentIdentity instrId in instrList)
                    {
                        Console.WriteLine(
                            "=============================================================");
                        Console.WriteLine("    INSTRUMENT: {0}[{1}] - Port: {2} ({3})", 
                            instrId.ModelName, instrId.SerialNum, instrId.Port, instrId.PluginName);
                        Console.WriteLine(
                            "=============================================================");
                        Console.WriteLine();

                        // Get the list of the discovered pulser/receivers, select each one a print
                        // out some parameters for each.
                        foreach (IPulserReceiverIdentity prID in jsrManager.GetPulserReceivers(instrId))
                        {
                            Console.WriteLine(
                                "-------------------------------------------------------------");
                            Console.WriteLine("    P/R: {0}[{1}] - IDX {2}", 
                                prID.InstrumentId.ModelName, prID.InstrumentId.SerialNum, 
                                prID.PulserReceiverIndex);
                            Console.WriteLine(
                                "-------------------------------------------------------------");
                            try
                            {
                                // Set this pulser/receiver as the current device - this function will
                                // throw the CURRENT_PULSER_RCVR_CHANGED event.
                                jsrManager.SetCurrentPulserReceiver(prID);

                                // We can now call all the functions available in the IPulserReceiver
                                // interface on the selected pulser/receiver by using the
                                // JSRDotNETManager
                                Console.WriteLine("         BANDWIDTH: {0:F}", jsrManager.MaxFrequency);
                                Console.WriteLine("     SIGNAL_SELECT: {0}", 
                                    jsrManager.ReceiverMode.ToString());
                                Console.WriteLine("              GAIN: {0:F2}", jsrManager.Gain);
                                Console.WriteLine("          LOW_PASS: {0:F0}",
                                    jsrManager.LowPassFilterValues[jsrManager.LowPassFilterIndex]);
                                Console.WriteLine("         HIGH_PASS: {0:F0}",
                                    jsrManager.HighPassFilterValues[jsrManager.HighPassFilterIndex]);
                                Console.WriteLine("               PRF: {0:F0}", 
                                    jsrManager.PulseRepetitionFrequency);
                                Console.WriteLine("  ENERGY_PER_PULSE: {0:F2}", 
                                    jsrManager.EnergyPerPulse);
                                Console.WriteLine("           VOLTAGE: {0:F0}", jsrManager.HVSupply);
                                if (jsrManager.HVSupplyEnableSupported)
                                    Console.WriteLine("         HV_SUPPLY: {0:F0}",
                                       (jsrManager.HVSupplyEnable ? "ENABLED" : "DISABLED"));
                                if (jsrManager.PulseEnergyIndexSupported)
                                    Console.WriteLine("    ENERGY_CONTROL: {0}",
                                       jsrManager.PulseEnergyValueNames[jsrManager.PulseEnergyIndex]);
                                if (jsrManager.DampingIndexSupported)
                                    Console.WriteLine("           DAMPING: {0}",
                                       jsrManager.DampingValues[jsrManager.DampingIndex]);
                                if (jsrManager.PulserImpedanceSupported)
                                    Console.WriteLine("  PULSER_IMPEDANCE: {0}",
                                       jsrManager.PulserImpedance.ToString());

                                Console.WriteLine("    TRIGGER_SOURCE: {0}",
                                    jsrManager.TriggerSource.ToString());
                                if (jsrManager.TriggerEdgePolaritySupported)
                                    Console.WriteLine("  TRIGGER_POLARITY: {0}",
                                       jsrManager.TriggerEdgePolarity.ToString());
                                if (jsrManager.TriggerImpedanceSupported)
                                    Console.WriteLine(" TRIGGER_IMPEDANCE: {0}",
                                       jsrManager.TriggerImpedance.ToString());

                                Console.WriteLine("      IS_TRIGGERED: {0}",
                                    (jsrManager.TriggerEnable ? "YES" : "NO"));
                                Console.WriteLine("        IS_PULSING: {0}", 
                                    jsrManager.PulserIsPulsing.ToString());
                                if (jsrManager.PulserTriggerCountSupported)
                                    Console.WriteLine("     TRIGGER_COUNT: {0:D}", 
                                        jsrManager.PulserTriggerCount);
                                Console.WriteLine("       POWER_LIMIT: {0}",
                                    jsrManager.PulserPowerLimitStatus.ToString());
                                if (jsrManager.LEDBlinkModeIndexSupported)
                                    Console.WriteLine("        BLINK_MODE: {0}",
                                       jsrManager.LEDBlinkModeValues[jsrManager.LEDBlinkModeIndex]);
                            }
                            catch (Exception ex)
                            {
                                Console.WriteLine("Exception occurred during processing: " + 
                                    ex.Message);
                            }
                            Console.WriteLine();
                        } // for(pulserReceivers... )
                    }// for(instruments... )
                } // If count > 0
                else
                {
                    Console.WriteLine();
                    Console.WriteLine(" No Pulser Receivers found.");
                    Console.WriteLine();
                }
            } // if !m_bErrorOccurred

            // Make sure device discovery is turned off
            jsrManager.SetDiscoveryEnable(false);

            // Unregister our NotifyEventHander
            jsrManager.NotifyEventHandler -= this.OnJsrManagerNotify;

            Console.WriteLine("Press any key to exit...");
            Console.ReadKey(true);
            // Make sure any local references to pulser/receivers have been freed...

            // Application is finished with the JSRDotNETManager; call the Shutdown() function
            jsrManager.Shutdown();
        }
        #endregion

        #region JSRDotNETManager Event Handlers

        ///////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Event handler for notify events from the JSRDotNETManager.</summary>
        ///
        /// <authorAndDate>mwb, 4/1/2016.</authorAndDate>
        ///
        /// <param name="sender">Source of the event.</param>
        /// <param name="e">The EventArgsManagerNotify to process.</param>
        ///////////////////////////////////////////////////////////////////////////////////////////////
        internal void OnJsrManagerNotify(object sender, EventArgsManagerNotify e)
        {
            // We don't care about this notification
            Console.WriteLine();
            Console.WriteLine("EVENT{0}: {1}", 
                (string.IsNullOrWhiteSpace(e.PluginName) ? "" : "[" + e.PluginName + "]"),
                e.NotifyType);
            if (e.PulserReceiverId != null)
            {
                Console.WriteLine("DEVICE: {0}", e.PulserReceiverId.ToString());
            }
            if (e.NotifyType == NOTIFY_TYPE.PULSER_RCVR_DETACH)
            {
                if (e.WasSelected)
                    Console.WriteLine("DETACHED: Device was the selected device");
            }
            else if (e.NotifyType == NOTIFY_TYPE.DISCOVERY_COMPLETE)
            {
                m_WaitEvent.Set();
            }
            else if (e.NotifyType == NOTIFY_TYPE.ERROR)
            {
                if (0 == (e.DiscoverState & DiscoveryStateFlags.PENDING))
                {
                    Console.WriteLine(" ERROR MSG: " + e.ErrorMsg);
                    Console.WriteLine("ERROR TEXT: " + e.ErrorText);
                    m_bErrorOccurred = true;
                    m_WaitEvent.Set();
                }
                else
                {
                    Console.WriteLine();
                    Console.WriteLine(e.ErrorMsg);
                    Console.WriteLine();
                }
            }
        }
        #endregion
    }
}
