// See AssemblyResolver.hpp. Compiled with /clr; it must not reference
// JSRDotNETSDK, or .NET would need that assembly to compile this code.
#include "AssemblyResolver.hpp"

using namespace System;
using namespace System::IO;
using namespace System::Reflection;
using namespace System::Threading;

namespace {

ref class OwnFolderResolver abstract sealed {
public:
  /// Loads the assembly .NET couldn't find from JSR-SDK.dll's folder, or
  /// returns nullptr so .NET reports it missing as before.
  static Assembly ^ Resolve(Object ^, ResolveEventArgs ^ args) {
    try {
      String ^ folder =
          Path::GetDirectoryName(OwnFolderResolver::typeid->Assembly->Location);
      String ^ name = (gcnew AssemblyName(args->Name))->Name;
      for each (String ^ extension in gcnew array<String ^>{".dll", ".exe"}) {
        String ^ path = Path::Combine(folder, name + extension);
        if (File::Exists(path)) {
          return Assembly::LoadFrom(path);
        }
      }
    } catch (Exception ^) {
      // An assembly that can't be resolved here stays missing.
    }
    return nullptr;
  }

  static int installed = 0;
};

} // namespace

void jsr::detail::installAssemblyResolver() {
  if (Interlocked::CompareExchange(OwnFolderResolver::installed, 1, 0) == 0) {
    AppDomain::CurrentDomain->AssemblyResolve +=
        gcnew ResolveEventHandler(&OwnFolderResolver::Resolve);
  }
}
