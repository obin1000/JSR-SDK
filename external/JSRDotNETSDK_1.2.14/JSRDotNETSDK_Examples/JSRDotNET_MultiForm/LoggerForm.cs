using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace JSRDotNET_MultiForm
{
    public partial class LoggerForm : Form
    {
        Queue<string> m_queuedLines;
        MainForm m_parentForm;
        string m_sLogFilePath;
        StreamWriter m_logWriter;
        bool m_bLogPaused;
        bool m_bIsClosing;
        //int m_nLabelPadding;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Constructor.</summary>
        ///
        /// <param name="parent">The parent form that owns this window.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        public LoggerForm(MainForm parent)
        {
            m_parentForm = parent;
            m_queuedLines = new Queue<string>();
            m_logWriter = null;
            m_sLogFilePath = "";
            m_bIsClosing = false;
            pauseLogUpdates();

            InitializeComponent();
            //m_nLabelPadding = (this.Width - lblLogFile.Width);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Closes log window.</summary>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        public void CloseLogWindow()
        {
            m_bIsClosing = true;
            pauseLogUpdates();
            lock (m_queuedLines)
            {
                // Flush the queue
                while (m_queuedLines.Count > 0)
                {
                    postLine(m_queuedLines.Dequeue());
                }
            }
            closeLogFile();
            this.Close();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Adds a line.</summary>
        ///
        /// <param name="line">The line to be added to the log output.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        public void AddLine(string line)
        {
            if (!m_bIsClosing)
            {
                string msg = line.TrimEnd();
                bool updateLog = false;
                lock (m_queuedLines)
                {
                    updateLog = (m_queuedLines.Count == 0);
                    m_queuedLines.Enqueue(msg);
                }
                if (updateLog)
                {
                    logUpdate();
                }
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Event handler. Called by LoggerForm for load events.</summary>
        ///
        /// <authorAndDate>mwb, 4/13/2016.</authorAndDate>
        ///
        /// <param name="sender">Source of the event.</param>
        /// <param name="e">Event information.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        private void LoggerForm_Load(object sender, EventArgs e)
        {
            dlgSaveLog.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
            resumeLogUpdates();
            updateLogStatus();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Event handler. Called by LoggerForm for resize events.</summary>
        ///
        /// <authorAndDate>mwb, 4/13/2016.</authorAndDate>
        ///
        /// <param name="sender">Source of the event.</param>
        /// <param name="e">Event information.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        private void LoggerForm_Resize(object sender, EventArgs e)
        {
            // Update the width of the filename label
          //  lblLogFile.Width = (this.Width - m_nLabelPadding);

            // Update the filename text
            updateLogStatus();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Event handler. Called by LoggerForm for form closing events.</summary>
        ///
        /// <authorAndDate>mwb, 4/13/2016.</authorAndDate>
        ///
        /// <param name="sender">Source of the event.</param>
        /// <param name="e">Form closing event information.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        private void LoggerForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if ((!m_bIsClosing) && (e.CloseReason == CloseReason.UserClosing))
            {
                e.Cancel = true;
                m_parentForm.HideLogForm();
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Event handler. Called by btnSaveLog for click events.</summary>
        ///
        /// <authorAndDate>mwb, 4/13/2016.</authorAndDate>
        ///
        /// <param name="sender">Source of the event.</param>
        /// <param name="e">Button Event information.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        private void btnSaveLog_Click(object sender, EventArgs e)
        {
            if (m_logWriter != null)
            {
                closeLogFile();
            }
            else
            {
                DateTime fileDate = DateTime.Now;
                dlgSaveLog.FileName = "jsrcp_" + fileDate.ToString("yyyyMMdd_HHmm");
                dlgSaveLog.DefaultExt = "log";
                dlgSaveLog.AddExtension = true;
                DialogResult result = dlgSaveLog.ShowDialog(this);
                if (result == DialogResult.OK)
                {
                    if (openLogFile(dlgSaveLog.FileName))
                    {
                        // Update the LogFileSave dialog to the last directory used.
                        dlgSaveLog.InitialDirectory = Path.GetDirectoryName(m_sLogFilePath);
                    }
                    saveAsToolStripMenuItem.Enabled = false;
                }
                
            }
            updateLogStatus();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Pause log updates.</summary>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        private void pauseLogUpdates()
        {
            m_bLogPaused = true;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Resume log updates.</summary>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        private void resumeLogUpdates()
        {
            m_bLogPaused = false;
            logUpdate();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Updates the log with the queued strings.</summary>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        private void logUpdate()
        {
            if (!m_bLogPaused)
            {
                if (this.InvokeRequired)
                {
                    this.Invoke(new Action(logUpdate));
                }
                else
                {
                    string nextLine;
                    do
                    {
                        nextLine = null;
                        lock (m_queuedLines)
                        {
                            if ((m_queuedLines.Count > 0) && (!m_bLogPaused))
                            {
                                nextLine = m_queuedLines.Dequeue();
                            }
                        }
                        postLine(nextLine);
                    }
                    while (nextLine != null);
                }
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Opens log file.</summary>
        ///
        /// <param name="path">Full pathname of the file.</param>
        ///
        /// <returns>true if it succeeds, false if it fails.</returns>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        private bool openLogFile(string path)
        {
            bool success = false;
            if (!string.IsNullOrWhiteSpace(path))
            {
                try
                {
                    m_sLogFilePath = path.Trim();
                    // Create the file from scratch for writing and allow others to read it
                    m_logWriter = new StreamWriter(File.Open(m_sLogFilePath, FileMode.Create, FileAccess.Write, FileShare.Read));
                    m_logWriter.AutoFlush = true;
                    // Pause the log udpates and save the current log to the file
                    pauseLogUpdates();
                    writeLogFileStatus("Log File Opened");
                    m_logWriter.Write(tbLogger.Text);
                    resumeLogUpdates();
                    success = true;
                }
                catch (Exception ex)
                {
                    closeLogFile();
                    MessageBox.Show(this, "Unable to open log file", "Log Save Failed",
                        MessageBoxButtons.OK, MessageBoxIcon.Error);
                    postLine(ex.Message);
                    resumeLogUpdates();
                }
            }
            return success;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Closes log file.</summary>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        private void closeLogFile()
        {
            if (m_logWriter != null)
            {
                writeLogFileStatus("Log File Closed");
                m_logWriter.Close();
            }
            m_logWriter = null;
            m_sLogFilePath = "";
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Updates the form fields with the current log file status.</summary>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        private void updateLogStatus()
        {
            if (m_logWriter != null)
            {
               // btnSaveLog.Text = "Close Log File";
                //lblLogFile.Text = leftTrimPath(m_sLogFilePath, lblLogFile);
                saveAsToolStripMenuItem.Enabled = false;
                stopSavingCloseLogFIleToolStripMenuItem.Enabled = true;
                toolStripStatusLabel1.Text = m_sLogFilePath;
            }
            else
            {
               // btnSaveLog.Text = "Save Log File";
                //lblLogFile.Text = "";
                saveAsToolStripMenuItem.Enabled = true;
                stopSavingCloseLogFIleToolStripMenuItem.Enabled = false;
                toolStripStatusLabel1.Text = string.Empty;
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// Trims the characters on the left of the string to ensure it will fit in the maximum
        /// boundaries of the provided label.
        /// </summary>
        ///
        /// <param name="text">The text to be displayed.</param>
        /// <param name="label">The label in which the text will be displayed (for max size and
        /// font)</param>
        ///
        /// <returns>A right-aligned trimmed string that will fit within the max size.</returns>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        private string leftTrimPath(string text, Label label)
        {
            // Get the maximum width of the label or the current size if max width is not set
            int maxWidth = (label.MaximumSize.Width == 0 ? label.Width : label.MaximumSize.Width);
            string trimmedText = text;
            bool wasTrimmed = false;

            while (TextRenderer.MeasureText(trimmedText, label.Font).Width > maxWidth)
            {
                trimmedText = trimmedText.Substring(1);
                if (wasTrimmed == false)
                {
                    // String has been trimmed, make room for the ellipses at the beginning
                    wasTrimmed = true;
                    maxWidth -= TextRenderer.MeasureText("...", label.Font).Width;
                }
            }
            if (wasTrimmed)
            {
                trimmedText = "..." + trimmedText;
            }
            return trimmedText;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Posts a line.</summary>
        ///
        /// <param name="line">The line to be added to the log output.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        private void postLine(string line)
        {
            if (line != null)
            {
                if (!line.EndsWith("\r\n"))
                {
                    line += "\r\n";
                }
                tbLogger.AppendText(line);
                if (m_logWriter != null)
                {
                    m_logWriter.Write(line);
                }
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Writes a log file status.</summary>
        ///
        /// <param name="message">The message to be included with the date line.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        private void writeLogFileStatus(string message)
        {
            if (m_logWriter != null)
            {
                writeLogFileSeparator();
                m_logWriter.WriteLine("* " + message + ": " + DateTime.Now.ToString());
                m_logWriter.WriteLine("* Log File Path: " + m_sLogFilePath);
                writeLogFileSeparator();
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>Writes the log file separator.</summary>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        private void writeLogFileSeparator()
        {
            if (m_logWriter != null)
            {
                m_logWriter.WriteLine("--------------------------------------------------------------------------------");
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// Event handler. Called by saveAsToolStripMenuItem for click events.
        /// </summary>
        ///
        /// <authorAndDate>kds, 5/13/2016.</authorAndDate>
        ///
        /// <param name="sender">Source of the event.</param>
        /// <param name="e">Event information.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            btnSaveLog_Click(sender, e);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// Event handler. Called by stopSavingCloseLogFIleToolStripMenuItem for click events.
        /// </summary>
        ///
        /// <authorAndDate>kds, 5/13/2016.</authorAndDate>
        ///
        /// <param name="sender">Source of the event.</param>
        /// <param name="e">Event information.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        private void stopSavingCloseLogFIleToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (m_logWriter != null)
            {
                string filename = Path.GetFileName(m_sLogFilePath);
                closeLogFile();
                MessageBox.Show("Log File '" + filename + "' saved.", "Log File Closed", MessageBoxButtons.OK, MessageBoxIcon.None);
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// <summary>
        /// Event handler. Called by closeToolStripMenuItem for click events.
        /// </summary>
        ///
        /// <authorAndDate>kds, 5/13/2016.</authorAndDate>
        ///
        /// <param name="sender">Source of the event.</param>
        /// <param name="e">Event information.</param>
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        private void closeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }
}
