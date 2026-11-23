#pragma once

namespace jsr::detail {

/**
 * @brief Lets .NET find the SDK's assemblies next to JSR-SDK.dll.
 *
 * .NET looks for an assembly such as JSRDotNETSDK.dll next to the program,
 * not next to the DLL that needs it. So JSR-SDK.dll only worked when all
 * DLLs sat next to the .exe. This registers an AssemblyResolve handler that
 * loads a missing assembly from JSR-SDK.dll's own folder, so the DLLs work
 * from any folder: found through PATH, as cargo test does, or loaded from
 * another language.
 *
 * The C API calls it in JSR_Create, before any code that uses JSRDotNETSDK
 * runs; that is why it lives in a file that only touches System types.
 * Calling it more than once is harmless.
 */
void installAssemblyResolver();

} // namespace jsr::detail
