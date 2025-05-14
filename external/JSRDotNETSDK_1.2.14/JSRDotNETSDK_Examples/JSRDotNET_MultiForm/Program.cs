using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

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
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    static class Program
    {
        static MainForm g_mainForm=null;

        public static MainForm MainFormInstance
        {
            get
            {
                if (null == g_mainForm) g_mainForm = new MainForm();
                return g_mainForm;
            }
        }

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(MainFormInstance);
        }
    }
}
