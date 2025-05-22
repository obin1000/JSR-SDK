

using namespace JSRDotNETSDK;

/**
 * This wrapper holds the instance of JSRDotNETSDKManager and manages events and
 * callbacks.
 * This is the C# side of the wrapper.
 **/
public
ref class JSRSDKWrapper {
private:
  JSRDotNETManager ^ m_manager;

public:
  JSRSDKWrapper() { m_manager = gcnew JSRDotNETManager(""); }

  property JSRDotNETManager ^
      dotNETManager { JSRDotNETManager ^ get() { return m_manager; } }
};
