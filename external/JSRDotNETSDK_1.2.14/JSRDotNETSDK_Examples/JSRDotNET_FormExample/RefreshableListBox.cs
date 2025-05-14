using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Windows.Forms;

namespace JSRDotNET_FormExample
{
    public class RefreshableListBox : ListBox
    {


        public void RefreshOneItem(int index)
        {
            RefreshItem(index);
        }
    }
}

