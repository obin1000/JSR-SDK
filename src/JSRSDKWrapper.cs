namespace JSRSDK;

using System.Runtime.InteropServices;

public static class NativeExports
{
    // Exposes: int Add(int a, int b);
    [UnmanagedCallersOnly(EntryPoint = "sdk_add")]
    public static int sdk_add(int a, int b) => a + b;
}
