////////////////////////////////////////////////////////////////////////////////////////////////////
// JSRDotNET_FormExample
// 
// summary: An example of a Form-based application which can communicate with any combination
// of JSR Ultrasonics Pulser Receivers for which there is a plugin DLL for.  Most of the code
// in this example is needed to support the user interface with multiple simultaneously connected 
// instruments, some of which can be dynamically attached or removed while the application is running.
// In addition,  supporting the 'hot-swap' feature of the DPR500 added a significant amount of
// code.
// 
// Copyright (C) 2016, Imaginant Inc.
// 
// See the file "LICENSE_EXAMPLES.TXT" for the full license governing this code. 
////////////////////////////////////////////////////////////////////////////////////////////////////
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using JSRDotNETSDK;

namespace JSRDotNET_FormExample
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>An example of a Form-based application which can communicate with any combination of
    /// JSR Ultrasonics Pulser Receivers for which there is a plugin DLL for.  This application handles
    /// dynamic attach and re-attach of USB pulser receivers.</summary>
    ///
    /// <authorAndDate>kds, 4/15/2016.</authorAndDate>
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    public partial class FormExample : Form
    {

        #region String Constants

        /* The following string constants are the names of properties in the DPR plugin which are not 
         * in the IPulserReceiver interface */

        const string PRPropertyNamePulserModel = "PulserModel";
        const string PRPropertyNameReceiverModel = "ReceiverModel";
        const string PRPropertyNameIsPulserPresent = "IsPulserPresent";

        /// <summary>The name of the plugin which provides simulated instruments.</summary>
        const string PLUGIN_NAME_PRSIM = "JSRDotNET_DiagLib";

        /// <summary>The name of the plugin for DPR500 and DPR300.</summary>
        const string PLUGIN_NAME_DPR = "JSRDotNET_DPR";

        /// <summary>The plugin name USB PureView Board.</summary>
        const string PLUGIN_NAME_USB_PUREVIEW_BOARD = "JSRDotNET_PureViewBrd";


        #endregion

        #region Local Variables

        /// <summary>The path to the JSRDotNETSDK Instrument Plugins</summary>
        string PROGRAM_PLUGIN_PATH = System.IO.Path.GetDirectoryName(
            System.Reflection.Assembly.GetExecutingAssembly().Location) + @"\..\..\..\..\Plugins";


        /// <summary>The JSRDotNETManager object handles loading the plugins, as well as communicating
        /// with the instruments.</summary>
        JSRDotNETManager m_jsrManager;

        /// <summary>This is set to true when the controls are being updated, so we can tell the difference
        /// between a user manipulated a control verses normal programmatic changes.</summary>
        bool m_bLoadingControls;
        #endregion

        #region Local Definitions and Consts

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>A pr list box item.</summary>
        ///
        /// <authorAndDate>kds, 4/15/2016.</authorAndDate>
        ///
        /// <seealso cref="T:System.IEquatable{JSRDotNET_FormExample.FormExample.PRListBoxItem}"/>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class PRListBoxItem : IEquatable<PRListBoxItem>
        {
            public string ModelName { get; set; }
            public string SerialNum { get; set; }
            public int Index { get; set; }
            public string PulserModel { get; set; }

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// <summary>Constructor.</summary>
            ///
            /// <authorAndDate>kds, 4/15/2016.</authorAndDate>
            ///
            /// <param name="modelName">Name of the model.</param>
            /// <param name="serialNum">The serial number.</param>
            /// <param name="index">Zero-based index of the.</param>
            /// <param name="pulserModel">The pulser model.</param>
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            public PRListBoxItem(string modelName, string serialNum, int index = 0, string pulserModel = "")
            {
                ModelName = modelName;
                SerialNum = serialNum;
                Index = index;
                PulserModel = pulserModel;
            }

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// <summary>Returns a string that represents the current object.</summary>
            ///
            /// <authorAndDate>kds, 4/15/2016.</authorAndDate>
            ///
            /// <returns>A string that represents the current object.</returns>
            ///
            /// <seealso cref="M:System.Object.ToString()"/>
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            public override string ToString()
            {
                string strRet = ModelName + (Index == 0 ? " ChA " : " ChB ");
                if (ModelName.ToUpper().StartsWith("DPR500")) strRet += " " + PulserModel;
                strRet += " SN: " + SerialNum;
                return strRet;
            }

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// <summary>
            /// Determines whether the specified <see cref="T:System.Object" /> is equal to the
            /// current <see cref="T:System.Object" />.
            /// </summary>
            ///
            /// <authorAndDate>kds, 4/15/2016.</authorAndDate>
            ///
            /// <param name="obj">The object to compare with the current object.</param>
            ///
            /// <returns>
            /// true if the specified <see cref="T:System.Object" /> is equal to the current
            /// <see cref="T:System.Object" />; otherwise, false.
            /// </returns>
            ///
            /// <seealso cref="M:System.Object.Equals(object)"/>
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            public override bool Equals(object obj)
            {
                bool bRet = null != obj;
                if (bRet)
                {
                    bRet = obj is PRListBoxItem;
                }
                if (bRet)
                {
                    PRListBoxItem other = obj as PRListBoxItem;
                    bRet = Equals(other);
                }
                return bRet;
            }

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// <summary>
            /// Tests if this PRListBoxItem is considered equal to another.
            /// </summary>
            ///
            /// <authorAndDate>kds, 4/15/2016.</authorAndDate>
            ///
            /// <param name="other">The pr list box item to compare to this object.</param>
            ///
            /// <returns>
            /// true if the objects are considered equal, false if they are not.
            /// </returns>
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            public bool Equals(PRListBoxItem other)
            {
                bool bRet = null != other;
                if (bRet)
                {
                    bRet = (ModelName.ToUpperInvariant() == other.ModelName.ToUpperInvariant()) &&
                            (SerialNum.ToUpperInvariant() == other.SerialNum.ToUpperInvariant()) && (Index == other.Index);
                }
                return bRet;
            }

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// <summary>Query if this item has a matching model #, Serial #, and channel index.</summary>
            ///
            /// <authorAndDate>kds, 5/11/2016.</authorAndDate>
            ///
            /// <param name="modelName">Name of the model.</param>
            /// <param name="serialNumber">The serial number.</param>
            /// <param name="prIndex">Zero-based index of the pr.</param>
            ///
            /// <returns>true if match, false if not.</returns>
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            public bool IsMatch(string modelName, string serialNumber, int prIndex)
            {
                return (modelName == ModelName) && (SerialNum == serialNumber) && (Index == prIndex);
            }

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// <summary>Serves as a hash function for a particular type.</summary>
            ///
            /// <authorAndDate>kds, 4/15/2016.</authorAndDate>
            ///
            /// <returns>A hash code for the current <see cref="T:System.Object" />.</returns>
            ///
            /// <seealso cref="M:System.Object.GetHashCode()"/>
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            public override int GetHashCode()
            {
                return ModelName.GetHashCode() ^ SerialNum.GetHashCode() ^ Index;
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>A helper class to act as combo box items for floating point values.   </summary>
        ///
        /// <authorAndDate>kds, 5/5/2016.</authorAndDate>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        public class DoubleValue
        {
            /// <summary>The value.</summary>
            public double value;
            /// <summary>The value as a string.</summary>
            public string sValue;

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// <summary>Constructor.</summary>
            ///
            /// <authorAndDate>kds, 5/5/2016.</authorAndDate>
            ///
            /// <param name="_value">The value.</param>
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            public DoubleValue(double _value)
            {
                value = _value;
                sValue = value.ToString("F1");
            }


            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// <summary>Returns a string that represents the current object.</summary>
            ///
            /// <authorAndDate>kds, 5/5/2016.</authorAndDate>
            ///
            /// <returns>A string that represents the current object.</returns>
            ///
            /// <seealso cref="M:System.Object.ToString()"/>
            ////////////////////////////////////////////////////////////////////////////////////////////////////
            public override string ToString()
            {
                return sValue;
            }
        }
        #endregion

        #region Constructors
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Default constructor.</summary>
        ///
        /// <authorAndDate>kds, 5/5/2016.</authorAndDate>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        public FormExample()
        {
            m_bLoadingControls = false;
            m_jsrManager = new JSRDotNETManager();
            try
            {
                // Load the plugins, which means finding all the plugins which in the plugins folder, and reading
                // the metadata.  The classes in the plugin are not instantiated at this point, and no code from
                // the plugins is being executed.   This step is simply to find out more information about what we
                // can open.
                m_jsrManager.PluginPath = PROGRAM_PLUGIN_PATH;
                m_jsrManager.LoadPlugins();
            }
            catch (ExceptionJSRDotNET exJsr)
            {
                MessageBox.Show(this, exJsr.GetType().ToString() + " occurred: " + ((exJsr.Message != null) ? exJsr.Message : "(no msg)"));
                Application.Exit();
            }

            //  Now run through the list of plugin names, and tell the manager we want to use them with the AddManagedPlugin() method.
            // If we don't add the plugins to the Manager, then the classes in that plugin will never be instantiated, and none of
            // those devices will be discovered.
            // This is a good time to specify open options for each plugin also.   Below we look for a specific plugin name, and tell
            // to not automatically connect to the simulated pulser-receivers, and we do want to see the simulator windows displayed.
            foreach (string pluginName in m_jsrManager.GetPluginNames())
            {
                m_jsrManager.AddManagedPlugin(pluginName);
                switch(pluginName)
                {
                    /* For the simulator plugin, we indicate that it should not automatically connect upon startup.   The simulator(s)
                     * will exist, but will not be discovered by this application until the user clicks on the attach checkbox on 
                     * the simulator window.   In addition, the option is set that makes the simulator windows visible upon startup. */
                    case PLUGIN_NAME_PRSIM:
                      m_jsrManager.AddPluginOpenOption(PLUGIN_NAME_PRSIM, InstrumentOpenCriteria.CONNECT_ON_OPEN, "NO");
                      m_jsrManager.AddPluginOpenOption(PLUGIN_NAME_PRSIM, InstrumentOpenCriteria.SHOW_SIMULATOR_WINDOWS, "YES");
                      break;

                    /* For the DPR instrument plugin, we specify that the device should continuously query to see if a remote pulser
                     * has been removed or attached.  If so, property change notifications will be sent to notify this application. */
                    case PLUGIN_NAME_DPR:
                      m_jsrManager.AddPluginOpenOption(PLUGIN_NAME_DPR, "DPR500_POLL", "ENABLE");
                      m_jsrManager.AddPluginOpenOption(PLUGIN_NAME_DPR, "SERIAL_PORT_LOG", "DISABLE");
                      break;

                    case PLUGIN_NAME_USB_PUREVIEW_BOARD:
                        /* There presently no open options for this plugin. */
                      break;

                    default:
                      break;
                }
            }

            // Let the user know if there weren't any plugins.
            if(0 >= m_jsrManager.GetPluginNames().Length)
            {
                MessageBox.Show(this, "Plugin\nPath: " + 
                    m_jsrManager.PluginPath,
                    "No instrument plugins found. ", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            // Here we register to receiver Notification events from the Manager.    These events tell us when devices get discovered, 
            // when one is disconnected, or if a property has changed.  
            m_jsrManager.NotifyEventHandler += HandleManagerNotifyEvents;

            InitializeComponent();
        }
        #endregion

        #region Form Helper Methods

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// Gets a value indicating whether the currently selected pulser is ready.
        /// </summary>
        ///
        /// <value>true if currently selected pulser is ready, false if not.</value>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        bool CurrentlySelectedPulserIsReady
        {
            get
            {
                bool bRet = (null != m_jsrManager) && ( 0 <= listBoxPulserSelect.SelectedIndex);
                if (bRet)
                {
                    /* make sure manager thinks a pulser is selected. */
                    bRet = m_jsrManager.IsPulserReceiverSelected;
                }
                if (bRet)
                {
                    IPulserReceiverIdentity id = m_jsrManager.Id;
                    if (id.InstrumentId.ModelName.ToUpperInvariant().StartsWith("DPR500"))
                    {
                        /* If it is a DPR500, read the custom property "PulserPresent" to verify the remote pulser is still attached. */
                        bRet = (bool)m_jsrManager.GetPulserReceiver(id).GetPulserPropertyValue(PRPropertyNameIsPulserPresent);
                    }
                }
                return bRet;
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// When no pulser receiver is selected, the form's controls should be disabled.
        /// </summary>
        ///
        /// <authorAndDate>kds, 5/5/2016.</authorAndDate>
        ///
        /// <param name="bEnable">true to enable, false to disable.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void SetControlsEnable(bool bEnable)
        {
            cboxGain.Enabled = bEnable;
            cboxTrigSrc.Enabled = bEnable;
            ckboxTriggerEnable.Enabled = bEnable;
            cbLEDBlink.Enabled = bEnable;
            if (!bEnable)
            {
                cbLEDBlink.Items.Clear();
                cboxGain.Items.Clear();
                cboxTrigSrc.Items.Clear();
                ckboxTriggerEnable.Checked = false;
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Sets controls default content.</summary>
        ///
        /// <authorAndDate>kds, 5/5/2016.</authorAndDate>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void SetControlsDefaultContent()
        {
            cboxTrigSrc.Items.Clear();
            cboxTrigSrc.Items.Add("Internal");
            cboxTrigSrc.Items.Add("External");
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// Refreshes the controls on the form with values read from the pulser receiver.  This
        /// will occur if the currently selected pulser receiver changes.
        /// </summary>
        ///
        /// <authorAndDate>kds, 5/5/2016.</authorAndDate>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void setControlsFromPulser()
        {
            bool TrigSrcEnabled = false;
            bool bGainIndexSupported = true;
            SetControlsDefaultContent();

            SetModelAndSerial(m_jsrManager.Id.InstrumentId.ModelName,
                m_jsrManager.Id.InstrumentId.SerialNum);
            UpdateBlinkControl();

            if (CurrentlySelectedPulserIsReady)
            {
                // Lock out event handlers from responding while we programatically change controls.
                m_bLoadingControls = true;
                SetControlsEnable(true);
                try
                {
                    ckboxTriggerEnable.Enabled = true;
                    ckboxTriggerEnable.Checked = m_jsrManager.TriggerEnable;
                    if (m_jsrManager.TriggerSourceInternalSupported)
                    {
                        TrigSrcEnabled = m_jsrManager.TriggerSourceInternalSupported;
                        cboxTrigSrc.Enabled = TrigSrcEnabled;
                    }
                    int iTrigSrc = (int)m_jsrManager.TriggerSource;
                    cboxTrigSrc.SelectedIndex = iTrigSrc;

                    bGainIndexSupported = m_jsrManager.GainIndexSupported;
                    if (bGainIndexSupported)
                    {
                        cboxGain.Enabled = true;
                        cboxGain.Items.Clear();
                        double[] gains = m_jsrManager.GainValues;
                        foreach (double gain in gains)
                        {
                            cboxGain.Items.Add(new DoubleValue(gain));
                        }
                        cboxGain.SelectedIndex = m_jsrManager.GainIndex;
                    }
                    else if (m_jsrManager.GainStepSizeSupported)
                    {
                        double gmin = m_jsrManager.GainMin;
                        double gmax = m_jsrManager.GainMax;
                        double stepsz = m_jsrManager.GainStepSize;
                        int i = 0;
                        double gain = gmin;
                        while (gain <= gmax)
                        {
                            cboxGain.Items.Add(new DoubleValue(gain));
                            i++;
                            gain = gmin + (i * stepsz);
                        }
                        cboxGain.SelectedItem = findClosestItem(cboxGain, m_jsrManager.Gain);
                    }
                    else
                    {
                        // If stepsize not regular, and don't have a list of gains, we'd need to get more creative.
                        // but for this demo we will not handle it since all of the current products fit into one of the two cases.
                    }

                    ckboxTriggerEnable.Checked = m_jsrManager.TriggerEnable;
                }
                catch (ExceptionJSRDotNET eJSR)
                {
                    MessageBox.Show(eJSR.Message, "Error initing controls", MessageBoxButtons.OK);
                }
                m_bLoadingControls = false;
            }
            else
            {
                SetControlsEnable(false);
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Sets model and serial.</summary>
        ///
        /// <authorAndDate>mwb, 5/16/2016.</authorAndDate>
        ///
        /// <param name="strModel">The model.</param>
        /// <param name="strSerial">The serial.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void SetModelAndSerial(string strModel = "", string strSerial = "")
        {
            tbModel.Text = strModel;
            tbSerialNum.Text = strSerial;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Updates the blink control.</summary>
        ///
        /// <authorAndDate>mwb, 5/16/2016.</authorAndDate>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        private void UpdateBlinkControl()
        {
            cbLEDBlink.SelectedIndex = -1;
            cbLEDBlink.Items.Clear();

            if ((m_jsrManager.IsPulserReceiverSelected) &&
                (m_jsrManager.LEDBlinkModeIndexSupported))
            {
                cbLEDBlink.Enabled = true;
                foreach (string blinkVal in m_jsrManager.LEDBlinkModeValues)
                {
                    cbLEDBlink.Items.Add(blinkVal);
                }
                // Get a copy of the value to prevent it from being set
                cbLEDBlink.SelectedIndex = m_jsrManager.LEDBlinkModeIndex;
            }
            else
            {
                cbLEDBlink.Enabled = false;
            }
        }

        #endregion

        #region Helper Methods
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Searches for the nearest item.</summary>
        ///
        /// <authorAndDate>kds, 5/5/2016.</authorAndDate>
        ///
        /// <param name="cb">The cb.</param>
        /// <param name="enteredValue">The entered value.</param>
        ///
        /// <returns>The found item.</returns>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        private DoubleValue findClosestItem(ComboBox cb, double enteredValue)
        {
            DoubleValue selectedItem = null;
            double smallestDiff = double.MaxValue;
            foreach (DoubleValue item in cb.Items)
            {
                double diff = item.value - enteredValue;
                if (0.0 > diff) diff = -diff;
                if (diff < smallestDiff)
                {
                    selectedItem = item;
                    smallestDiff = diff;
                }
            }
            return selectedItem;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>For DPR500's, this helper method gets the remote pulser model, which is different from the instrument 
        /// model.</summary>
        ///
        /// <authorAndDate>kds, 5/5/2016.</authorAndDate>
        ///
        /// <param name="infoArray">Array of informations.</param>
        ///
        /// <returns>The pulser model.</returns>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        private string getPulserName(string[] infoArray)
        {
            string strRet = string.Empty;
            int i;
            for (i = 0; i < infoArray.Length; i += 2)
            {
                string name = infoArray[i];
                string value = infoArray[i + 1];
                if (name == "PulserModel")
                {
                    strRet = value;
                    break;
                }
            }

            return strRet;
        }
        #endregion

        #region JSRDotNETManager Event Handlers
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Handles the manager notify events.</summary>
        ///
        /// <authorAndDate>kds, 5/5/2016.</authorAndDate>
        ///
        /// <param name="sender">Source of the event.</param>
        /// <param name="args">The arguments.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        private void HandleManagerNotifyEvents(object sender, EventArgsManagerNotify args)
        {
            // This part is critical!   The manager will typically call your handler from another non-gui thread,
            // so you can't directly call most methods of controls.
            if (InvokeRequired)
            {
                BeginInvoke(new EventHandler<EventArgsManagerNotify>(HandleManagerNotifyEvents), new object[] { sender, args });
            }
            else
            {
                string model = string.Empty;
                string serialNum = string.Empty;
                int prevSelectedIndex;
                PRListBoxItem item;
                prevSelectedIndex = listBoxPulserSelect.SelectedIndex;
                switch (args.NotifyType)
                {
                    case NOTIFY_TYPE.PULSER_RCVR_DISCOVERED:
                        HandlePulserReceiverDiscoveryNotification(args);
                        break;
                    case NOTIFY_TYPE.CURRENT_PULSER_RCVR_CHANGED:
                        setControlsFromPulser();
                        break;
                    case NOTIFY_TYPE.PROPERTY_CHANGE:
                        HandlePropertyChangeNotifications(args);
                        break;
                    case NOTIFY_TYPE.PULSER_RCVR_DETACH:
                        item = new PRListBoxItem(args.Model, args.Serial, args.PRIndex);
                        // Search the items in the listBox for a pulser with the same model, serial num, and pulser index as the one that is now removed.
                        for (int i = 0; i < listBoxPulserSelect.Items.Count; i++)
                        {
                            if (listBoxPulserSelect.Items[i] is PRListBoxItem)
                            {
                                if (item.Equals(listBoxPulserSelect.Items[i] as PRListBoxItem))
                                {
                                    // Remove it from the list.
                                    listBoxPulserSelect.Items.Remove(listBoxPulserSelect.Items[i]);
                                    labelInstrumentList.Text = "Instruments ( " + listBoxPulserSelect.Items.Count + " connected )";
                                    // If the one we just removed was the currently selected one, select the first on the list or nothing if the list is empty.
                                    if (i == prevSelectedIndex)
                                    {
                                        listBoxPulserSelect.SelectedIndex = (0 < listBoxPulserSelect.Items.Count) ? 0 : -1;
                                    }
                                    break;
                                }
                            }
                        }
                        break;
                    case NOTIFY_TYPE.ERROR:
                        MessageBox.Show(args.ErrorMsg, args.ExceptionTypeInfo, MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                        break;
                }
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// Handles the pulser receiver discovery notification described by args.
        /// </summary>
        ///
        /// <authorAndDate>kds, 5/11/2016.</authorAndDate>
        ///
        /// <param name="args">The arguments.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void HandlePulserReceiverDiscoveryNotification(EventArgsManagerNotify args)
        {
            string model = string.Empty;
            string serialNum = string.Empty;
            int idxPR = 0;
            int prevSelectedIndex;
            int newListBoxIndex;
            PRListBoxItem item;
            bool isDPR500 = args.PulserReceiverId.InstrumentId.ModelName.ToUpperInvariant().StartsWith("DPR500");
            prevSelectedIndex = listBoxPulserSelect.SelectedIndex;

            model = args.Model;
            serialNum = args.Serial;
            idxPR = args.PRIndex;
            if (string.Empty != model)
            {
                // Here we are making a string to display in the pulser selection list.  Because
                // the DPR500 has multiple remote pulsers with many different models, we need to 
                // put the model number of the remote pulser in the string too, because it matters.
                if (model.ToUpper().StartsWith("DPR500"))
                {
                    item = new PRListBoxItem(model, serialNum, idxPR, getPulserName(args.Info));
                }
                else
                {
                    item = new PRListBoxItem(model, serialNum, idxPR);
                }

                // Now we add the item to the list box.   If it is the first pulser discovered, we 
                // make the current selection.
                newListBoxIndex = listBoxPulserSelect.Items.Add(item);
                labelInstrumentList.Text = "Instruments ( " + listBoxPulserSelect.Items.Count + " connected )";
                if (0 > prevSelectedIndex) listBoxPulserSelect.SelectedIndex = newListBoxIndex;


                
                    // To Support the DPR500's hotswap of remote pulser, we'll need to receive some notifications
                    // indicating when a pulser is removed or attached, and also get the pulser model name.  Therefore
                    // we create a filter.   This filter specifies which pulser receiver properties we wish to receive
                    // change notifications for.
                    IPulserReceiver pulser = m_jsrManager.GetPulserReceiver(args.PulserReceiverId);
                    if (null != pulser)
                    {
                        PropertyChangeEventCriteria pcec;
                        if (isDPR500)
                        {
                            pcec = new PropertyChangeEventCriteria(new string[] {
                              PRPropertyNameIsPulserPresent,
                              PRPropertyNamePulserModel
                           });
                        }
                        else
                        {
                            pcec = new PropertyChangeEventCriteria();
                        }
                        pulser.StatusChangePropertyCriteria = pcec;
                    }
                
            }
            else
            {
                string strPluginName = args.PluginName;
                string friendlyName = string.Empty;
                if ((strPluginName != null) && (0 < strPluginName.Length))
                {
                    friendlyName = m_jsrManager.GetPluginLibraryMetadata(strPluginName).FriendlyName;
                }
                MessageBox.Show(friendlyName + " : No instruments Found.");
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Query if 'modelName' is this the currently selected pulser.</summary>
        ///
        /// <authorAndDate>kds, 5/11/2016.</authorAndDate>
        ///
        /// <param name="modelName">Name of the model.</param>
        /// <param name="serialNum">The serial number.</param>
        /// <param name="prIndex">Zero-based index of the pulser receiver.</param>
        ///
        /// <returns>true if this the currently selected pulser, false if not.</returns>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        bool isThisTheCurrentlySelectedPulser(string modelName, string serialNum, int prIndex)
        {
            bool bRet = false;
            if (0 <= listBoxPulserSelect.SelectedIndex)
            {
                PRListBoxItem item = listBoxPulserSelect.SelectedItem as PRListBoxItem;
                if (null != item)
                {
                    bRet = (item.ModelName == modelName) && (item.SerialNum == serialNum) && (item.Index == prIndex);
                }
            }
            return bRet;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Handles the property change notifications.  This is to support DPR500 'Hot Swap' of 
        /// remote pulsers.</summary>
        ///
        /// <authorAndDate>kds, 5/11/2016.</authorAndDate>
        ///
        /// <param name="args">The arguments.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void HandlePropertyChangeNotifications(EventArgsManagerNotify args)
        {
            bool bRefreshControls = false;
            if ((null != m_jsrManager) && (null != args.PropertyName) && (null != args.NewValue))
            {
                /* If the DPR500 pulser model has changed because one was detached or attached, the
                 * pulser model name will change.   We need to update the controls on the form. */
                if (args.PropertyName == PRPropertyNamePulserModel)
                {
                    HandleDPR500_PulserModelNameChange(args, (string)args.NewValue);
                }
                else if (args.PropertyName == PRPropertyNameIsPulserPresent)
                {
                    /* If the DPR500 'PulserIsPresent' property changes, we need to refesh controls. */
                    bRefreshControls = isThisTheCurrentlySelectedPulser(args.Model, args.Serial, args.PRIndex);
                }
            }
            if (bRefreshControls)
            {
                setControlsFromPulser();
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>If a DPR500 remote pulser is attached or detached, the pulser model name will change,
        /// forcing us to update that line in the pulser select listbox.   </summary>
        ///
        /// <authorAndDate>kds, 5/11/2016.</authorAndDate>
        ///
        /// <param name="args">The arguments.</param>
        /// <param name="newPulserModelName">Name of the new pulser model.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void HandleDPR500_PulserModelNameChange(EventArgsManagerNotify args, string newPulserModelName)
        {
            string modelName = args.PulserReceiverId.InstrumentId.ModelName;
            string serialNum = args.PulserReceiverId.InstrumentId.SerialNum;
            int prIndex = args.PulserReceiverId.PulserReceiverIndex;
            if (null != newPulserModelName)
            {
                for (int i = 0; i < listBoxPulserSelect.Items.Count; i++)
                {
                    PRListBoxItem item = listBoxPulserSelect.Items[i] as PRListBoxItem;
                    if (null != item)
                    {
                        if (item.IsMatch(modelName, serialNum, prIndex))
                        {
                            item.PulserModel = newPulserModelName;
                            (listBoxPulserSelect as RefreshableListBox).RefreshOneItem(i);
                        }
                    }
                }
            }
        }
        #endregion

        #region Form Event Handlers
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Event handler. Called by FormExample for load events.</summary>
        ///
        /// <authorAndDate>kds, 5/5/2016.</authorAndDate>
        ///
        /// <param name="sender">Source of the event.</param>
        /// <param name="e">Event information.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        private void Form1_Load(object sender, EventArgs e)
        {
            string assemblyVersion = Assembly.GetExecutingAssembly().GetName().Version.ToString();
            string assemblyVerShorter = assemblyVersion;
            int idxLastDot = assemblyVersion.LastIndexOf('.');
            if (idxLastDot > 2)
            {
                assemblyVerShorter = assemblyVersion.Remove(idxLastDot);
            }
            Text = string.Format("  JSR .Net Simple Form Example  (Ver: {0})", assemblyVerShorter);
            MessageBox.Show("This simple example program will discover real hardware or simulated instruments.  " +
            "Click on the 'Attach Instrument' checkbox of any of the simulators to attach the pulser receiver to the application.", "FormExample Usage");
            SetControlsEnable(false);
            UpdateBlinkControl();
            SetModelAndSerial();
            m_jsrManager.SetDiscoveryEnable(true);
            Thread.Sleep(500);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Event handler. Called by FormExample for form closing events.</summary>
        ///
        /// <authorAndDate>kds, 5/5/2016.</authorAndDate>
        ///
        /// <param name="sender">Source of the event.</param>
        /// <param name="e">Form closing event information.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            SetControlsEnable(false);
            SetModelAndSerial();
            m_jsrManager.Shutdown();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// Event handler. Called by listBoxPulserSelect for selected index changed events.
        /// </summary>
        ///
        /// <authorAndDate>kds, 5/5/2016.</authorAndDate>
        ///
        /// <param name="sender">Source of the event.</param>
        /// <param name="e">Event information.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        private void listBoxPulserSelect_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!m_bLoadingControls)
            {
                if (0 > listBoxPulserSelect.SelectedIndex)
                {
                    SetControlsEnable(false);
                    SetModelAndSerial();
                }
                else
                {
                    SetControlsEnable(true);
                    PRListBoxItem item = listBoxPulserSelect.SelectedItem as PRListBoxItem;
                    if (null != item)
                    {
                        m_jsrManager.SetCurrentPulserReceiver(new PulserReceiverIdentity(new InstrumentIdentity(item.ModelName, item.SerialNum), item.Index));
                    }
                }
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// Event handler. Called by ckboxTriggerEnable for checked changed events.
        /// </summary>
        ///
        /// <authorAndDate>mwb, 5/6/2016.</authorAndDate>
        ///
        /// <param name="sender">Source of the event.</param>
        /// <param name="e">Event information.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        private void ckboxTriggerEnable_CheckedChanged(object sender, EventArgs e)
        {
            if (!m_bLoadingControls && m_jsrManager.IsPulserReceiverSelected)
            {
                try
                {
                    m_jsrManager.TriggerEnable = ckboxTriggerEnable.Checked;
                }
                catch (ExceptionJSRDotNET eJSR)
                {
                    MessageBox.Show(eJSR.Message);
                }
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// Event handler. Called by cboxTrigSrc for selected index changed events.
        /// </summary>
        ///
        /// <authorAndDate>mwb, 5/6/2016.</authorAndDate>
        ///
        /// <param name="sender">Source of the event.</param>
        /// <param name="e">Event information.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        private void cboxTrigSrc_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!m_bLoadingControls && m_jsrManager.IsPulserReceiverSelected)
            {
                try
                {
                    m_jsrManager.TriggerSource = (TRIGGER_SOURCE)cboxTrigSrc.SelectedIndex;
                }
                catch (ExceptionJSRDotNET eJSR)
                {
                    MessageBox.Show(eJSR.Message);
                }
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// Event handler. Called by cboxGain for selected index changed events.
        /// </summary>
        ///
        /// <authorAndDate>mwb, 5/6/2016.</authorAndDate>
        ///
        /// <param name="sender">Source of the event.</param>
        /// <param name="e">Event information.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        private void cboxGain_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!m_bLoadingControls && m_jsrManager.IsPulserReceiverSelected)
            {
                try
                {
                    if (m_jsrManager.GainIndexSupported)
                    {
                        m_jsrManager.GainIndex = cboxGain.SelectedIndex;
                    }
                    else
                    {
                        DoubleValue dvalue = cboxGain.SelectedValue as DoubleValue;
                        if (null != dvalue)
                        {
                            m_jsrManager.Gain = dvalue.value;
                        }

                        //m_jsrManager.Gain = cboxGain.SelectedValue;
                    }
                }
                catch (ExceptionJSRDotNET eJSR)
                {
                    MessageBox.Show(eJSR.Message);
                }
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// Event handler. Called by cbLEDBlink for selected index changed events.
        /// </summary>
        ///
        /// <authorAndDate>mwb, 5/6/2016.</authorAndDate>
        ///
        /// <param name="sender">Source of the event.</param>
        /// <param name="e">Event information.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        private void cbLEDBlink_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!m_bLoadingControls && m_jsrManager.IsPulserReceiverSelected)
            {
                if ((0 <= cbLEDBlink.SelectedIndex) &&
                    (cbLEDBlink.SelectedIndex != m_jsrManager.LEDBlinkModeIndex))
                {
                    m_jsrManager.LEDBlinkModeIndex = cbLEDBlink.SelectedIndex;
                }
            }
        }

        #endregion



    } // end class FormExample


}
