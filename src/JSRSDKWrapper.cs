namespace JSRSDK;

using JSRDotNETSDK;
using System.Runtime.InteropServices;

public static class NativeExports
{
    private readonly static JSRDotNETManager jsrManager = new();
    // Exposes: int Add(int a, int b);
    [UnmanagedCallersOnly(EntryPoint = "sdk_add")]
    public static int sdk_add(int a, int b) => a + b;


    [UnmanagedCallersOnly(EntryPoint = "LoadPlugins")]
    public static int LoadPlugins()
    {
        jsrManager.PluginPath = "C:\\Users\\micro\\Documents\\Projects\\kaminarimedical\\JSR-SDK\\external\\JSRDotNETSDK_1.2.14\\Plugins";
        jsrManager.LoadPlugins();
        return 0;
    }

}
