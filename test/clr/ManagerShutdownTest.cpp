// ManagerShutdownTest.cpp
// #13: destroying a manager must shut the .NET manager down. Runs
// shutdown_check, a .NET program that creates, uses and destroys two managers
// on the simulator, and requires it to exit. Before the fix it hung: the
// destroyed managers' threads kept the .NET program alive.
#include <gtest/gtest.h>

#include <msclr/marshal_cppstd.h>

#include <string>

#ifndef JSR_SHUTDOWN_CHECK_EXE
#error "test/CMakeLists.txt defines JSR_SHUTDOWN_CHECK_EXE"
#endif

TEST(ManagerShutdown, NetProgramExitsAfterDestroyingManagers) {
  using System::Diagnostics::Process;
  using System::Diagnostics::ProcessStartInfo;

  ProcessStartInfo ^ info =
      gcnew ProcessStartInfo(gcnew System::String(JSR_SHUTDOWN_CHECK_EXE));
  info->UseShellExecute = false;
  info->RedirectStandardOutput = true;
  info->RedirectStandardError = true;
  info->WorkingDirectory = System::IO::Path::GetDirectoryName(info->FileName);

  Process ^ process = Process::Start(info);
  // Read both pipes asynchronously, so a full pipe can't block the program.
  System::Threading::Tasks::Task<System::String ^> ^ out =
      process->StandardOutput->ReadToEndAsync();
  System::Threading::Tasks::Task<System::String ^> ^ err =
      process->StandardError->ReadToEndAsync();

  if (!process->WaitForExit(60000)) {
    process->Kill();
    FAIL() << "shutdown_check didn't exit within 60 s: a destroyed manager's "
              "threads keep the .NET program alive (#13)";
  }
  process->WaitForExit(); // Let the output readers finish.
  const std::string output =
      msclr::interop::marshal_as<std::string>(out->Result + err->Result);
  EXPECT_EQ(process->ExitCode, 0) << output;
  EXPECT_NE(output.find("OK, returning from main"), std::string::npos)
      << output;
}
