////////////////////////////////////////////////////////////////////////////////////////////////////
// JSRDotNET_FormExample
// 
// summary: An example of a Form-based application which can communicate with any combination
// of JSR Ultrasonics Pulser Receivers for which there is a plugin DLL for.
// 
// Copyright (C) 2016, Imaginant Inc.
// 
// See the file "LICENSE_EXAMPLES.TXT" for the full license governing this code. 
////////////////////////////////////////////////////////////////////////////////////////////////////
using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace JSRDotNET_FormExample
{
    static class Program
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>The main entry point for the application.</summary>
        ///
        /// <authorAndDate>kds, 4/15/2016.</authorAndDate>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new FormExample());
        }
    }
}
