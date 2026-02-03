// JSRSDKManagerIntegrationTest.cpp
// Integration tests for JSRSDKManager requiring a physical device connection
#include "pch.h"
#include <gtest/gtest.h>

#include "JSR-SDK/JSRSDKManager.hpp"
#include <thread>
#include <chrono>
#include <vector>
#include <string>

//--------------------------------------------------------------------------
//  Helper Functions
//--------------------------------------------------------------------------
static std::string getStringFromBuffer(char *buffer, size_t size) {
  if (buffer == nullptr || size == 0) return "";
  return std::string(buffer);
}

static bool isCorrectDevice(const PulserReceiverID &prID) {
  std::string serial(prID.InstrumentId.SerialNum);
  
  // Also check if it's not a simulator
  std::string model(prID.InstrumentId.ModelName);
  if (model == "SIM") {
    return false;
  }
  
  return serial == "JB0104" || // HB3-2 (Loaner)
         serial == "JB0118" || // HB3-1
         serial == "JC0100";   // HB4
}

//--------------------------------------------------------------------------
//  Test Fixture for Integration Tests
//--------------------------------------------------------------------------
class JSRSDKManagerIntegrationTest : public ::testing::Test {
protected:
  JSRSDKManager *manager;
  bool deviceConnected;
  std::string testPluginName;
  PulserReceiverID connectedDeviceId;
  
  // Storage for callback messages
  std::vector<std::string> statusMessages;
  std::vector<std::string> notifyMessages;

  void SetUp() override {
    // Create the manager using the factory function
    manager = CreateJSRSDKManager();
    ASSERT_NE(manager, nullptr) << "Failed to create JSRSDKManager";

    deviceConnected = false;
    testPluginName = "";
    statusMessages.clear();
    notifyMessages.clear();

    // Try to load plugins from binary directory
    try {
      manager->loadPluginsFromBinaryDir();
    } catch (const std::exception &e) {
      FAIL() << "Failed to load plugins: " << e.what();
    }

    // Register callbacks to capture events
    try {
      manager->replaceStatusChangeEventHandler([this](const StatusChangedEvent &event) {
        std::string msg = "STATUS: ";
        msg += "ChangeType=" + std::to_string(static_cast<int>(event.changeType));
        msg += ", State=" + std::to_string(static_cast<int>(event.pulserState));
        msg += ", Property=" + std::string(event.pulserProperty);
        msg += ", Error=" + std::string(event.errorMessage);
        
        std::cout << "[CALLBACK] " << msg << std::endl;
        statusMessages.push_back(msg);
      });

      manager->replaceNotifyEventHandler([this](const NotifyEvent &event) {
        std::string msg = "NOTIFY: ";
        msg += "Type=" + std::to_string(static_cast<int>(event.notifyType));
        msg += ", Model=" + std::string(event.model);
        msg += ", Serial=" + std::string(event.serial);
        msg += ", State=" + std::to_string(static_cast<int>(event.state));
        msg += ", Property=" + std::string(event.propertyName);
        
        std::cout << "[CALLBACK] " << msg << std::endl;
        notifyMessages.push_back(msg);
        
        // Check for device discovered event
        if (event.notifyType == C_NOTIFY_TYPE::PULSER_RCVR_DISCOVERED) {
          std::cout << "[CALLBACK] Device discovered! Checking if correct device..." << std::endl;
          if (isCorrectDevice(event.pulserReceiverId)) {
            std::cout << "[CALLBACK] Correct device found! Serial: " 
                      << event.pulserReceiverId.InstrumentId.SerialNum << std::endl;
          } else {
            std::cout << "[CALLBACK] Device not in expected list (Serial: " 
                      << event.pulserReceiverId.InstrumentId.SerialNum << ")" << std::endl;
          }
        }
      });
      
      std::cout << "Callbacks registered successfully" << std::endl;
    } catch (const std::exception &e) {
      std::cout << "Failed to register callbacks: " << e.what() << std::endl;
    }

    // Check if plugins are loaded
    if (manager->getPluginsLoaded()) {
      // Get available plugins
      size_t pluginCount = manager->GetPluginNames(nullptr, 0, 0);
      
      std::cout << "\n=== Plugins Loaded ===" << std::endl;
      std::cout << "Total plugins: " << pluginCount << std::endl;
      
      if (pluginCount > 0) {
        std::vector<char*> pluginBuffers(pluginCount);
        std::vector<std::vector<char>> pluginStorage(pluginCount);
        
        for (size_t i = 0; i < pluginCount; ++i) {
          pluginStorage[i].resize(256);
          pluginBuffers[i] = pluginStorage[i].data();
        }
        
        manager->GetPluginNames(pluginBuffers.data(), pluginCount, 256);
        
        for (size_t i = 0; i < pluginCount; ++i) {
          std::string pluginName(pluginBuffers[i]);
          std::cout << "  [" << i << "] " << pluginName << std::endl;
        }
        std::cout << "=====================\n" << std::endl;
        
        testPluginName = pluginBuffers[0];
        
        // Try to discover devices
        try {
          std::cout << "Starting device discovery..." << std::endl;
          manager->SetDiscoveryEnable(true);
          
          // Poll for devices with progress updates
          const int maxAttempts = 20;  // 20 seconds total
          for (int attempt = 1; attempt <= maxAttempts && !deviceConnected; ++attempt) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            
            std::cout << "Discovery attempt " << attempt << "/" << maxAttempts << "..." << std::endl;
            
            size_t deviceCount = manager->GetPulserReceivers(nullptr, 0);
            std::cout << "  Found " << deviceCount << " device(s)" << std::endl;
            
            if (deviceCount > 0) {
              std::vector<PulserReceiverID> devices(deviceCount);
              manager->GetPulserReceivers(devices.data(), deviceCount);
              
              // List all found devices
              for (size_t i = 0; i < deviceCount; ++i) {
                std::string serial(devices[i].InstrumentId.SerialNum);
                std::string model(devices[i].InstrumentId.ModelName);
                std::string port(devices[i].InstrumentId.Port);
                
                std::cout << "    Device " << i << ": " << model 
                          << " (SN: " << serial << ") on " << port;
                
                // Check if this is a real device
                if (isCorrectDevice(devices[i])) {
                  std::cout << " --> MATCH!" << std::endl;
                  deviceConnected = true;
                  connectedDeviceId = devices[i];
                  
                  std::cout << "  --> Real device found! Using: " << serial << std::endl;
                  
                  // Set this device as current
                  manager->SetCurrentPulserReceiver(devices[i]);
                  break;
                } else {
                  std::cout << " (skipped)" << std::endl;
                }
              }
            }
            
            if (deviceConnected) {
              break;  // Exit the polling loop
            }
          }
          
          if (!deviceConnected) {
            std::cout << "No real device found after " << maxAttempts << " seconds" << std::endl;
            std::cout << "Expected devices: JB0104, JB0118, or JC0100" << std::endl;
            
            // Print summary of callbacks received
            std::cout << "\n=== Callback Summary ===" << std::endl;
            std::cout << "Status callbacks received: " << statusMessages.size() << std::endl;
            std::cout << "Notify callbacks received: " << notifyMessages.size() << std::endl;
            std::cout << "========================\n" << std::endl;
          }
        } catch (const std::exception &e) {
          std::cout << "Device discovery failed: " << e.what() << std::endl;
        }
      }
    }
  }

  void TearDown() override {
    if (manager) {
      try {
        // Remove callbacks before shutdown
        manager->removeStatusChangeEventHandler();
        manager->removeNotifyEventHandler();
        manager->Shutdown();
      } catch (...) {
        // Ignore shutdown errors in teardown
      }
      DestroyJSRSDKManager(manager);
      manager = nullptr;
    }
  }

  // Helper to fail tests if no device is connected
  void RequireDevice() {
    ASSERT_TRUE(deviceConnected) 
      << "No real device connected - test requires physical device (JB0104, JB0118, or JC0100)";
  }
  
  // Helper to check if device is required for test
  bool HasDevice() const {
    return deviceConnected;
  }
};

//--------------------------------------------------------------------------
//  Plugin Loading Tests
//--------------------------------------------------------------------------
TEST_F(JSRSDKManagerIntegrationTest, Callbacks_AreRegistered) {
  // Callbacks should be registered in SetUp
  // This test just verifies they don't crash
  SUCCEED();
  
  // Print callback statistics
  std::cout << "Callbacks registered. Status messages: " << statusMessages.size() 
            << ", Notify messages: " << notifyMessages.size() << std::endl;
}

TEST_F(JSRSDKManagerIntegrationTest, LoadPluginsFromBinaryDir_Success) {
  // Should already be loaded in SetUp
  EXPECT_TRUE(manager->getPluginsLoaded());
}

TEST_F(JSRSDKManagerIntegrationTest, GetPluginNames_ReturnsNonEmpty) {
  EXPECT_TRUE(manager->getPluginsLoaded());
  
  size_t pluginCount = manager->GetPluginNames(nullptr, 0, 0);
  
  EXPECT_EQ(pluginCount, 4u) << "Expected 4 plugins";
  
  // Verify we can read plugin names
  if (pluginCount > 0) {
    std::vector<char*> buffers(pluginCount);
    std::vector<std::vector<char>> storage(pluginCount);
    
    for (size_t i = 0; i < pluginCount; ++i) {
      storage[i].resize(256);
      buffers[i] = storage[i].data();
    }
    
    manager->GetPluginNames(buffers.data(), pluginCount, 256);
    
    EXPECT_FALSE(std::string(buffers[0]).empty());
  }
}

TEST_F(JSRSDKManagerIntegrationTest, GetPluginLibraryMetadata_ValidPlugin_ReturnsMetadata) {
  if (testPluginName.empty()) {
    FAIL() << "No plugin available for testing";
    return;
  }
  
  JSRLibMetadata metadata;
  bool success = manager->GetPluginLibraryMetadata(testPluginName.c_str(), &metadata);
  
  ASSERT_TRUE(success);
  EXPECT_FALSE(std::string(metadata.Name).empty());
  EXPECT_FALSE(std::string(metadata.GUID).empty());
  EXPECT_FALSE(std::string(metadata.FriendlyName).empty());
}

//--------------------------------------------------------------------------
//  Device Discovery Tests
//--------------------------------------------------------------------------
TEST_F(JSRSDKManagerIntegrationTest, SetDiscoveryEnable_True_EnablesDiscovery) {
  EXPECT_NO_THROW(manager->SetDiscoveryEnable(true));
  // Give time for discovery
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  EXPECT_NO_THROW(manager->SetDiscoveryEnable(false));
}

TEST_F(JSRSDKManagerIntegrationTest, GetPulserReceivers_ReturnsDevices) {
  RequireDevice();
  
  size_t deviceCount = manager->GetPulserReceivers(nullptr, 0);
  
  EXPECT_GT(deviceCount, 0u) << "Expected at least one device";
  
  std::vector<PulserReceiverID> devices(deviceCount);
  manager->GetPulserReceivers(devices.data(), deviceCount);
  
  // Verify at least one real device exists
  bool foundRealDevice = false;
  for (size_t i = 0; i < deviceCount; ++i) {
    if (isCorrectDevice(devices[i])) {
      foundRealDevice = true;
      EXPECT_FALSE(std::string(devices[i].InstrumentId.ModelName).empty());
      EXPECT_FALSE(std::string(devices[i].InstrumentId.SerialNum).empty());
    }
  }
  
  EXPECT_TRUE(foundRealDevice) << "No real device found in device list";
}

TEST_F(JSRSDKManagerIntegrationTest, SetCurrentPulserReceiver_ValidDevice_Success) {
  RequireDevice();
  
  size_t deviceCount = manager->GetPulserReceivers(nullptr, 0);
  ASSERT_GT(deviceCount, 0u);
  
  std::vector<PulserReceiverID> devices(deviceCount);
  manager->GetPulserReceivers(devices.data(), deviceCount);
  
  // Find the real device
  PulserReceiverID *realDevice = nullptr;
  for (size_t i = 0; i < deviceCount; ++i) {
    if (isCorrectDevice(devices[i])) {
      realDevice = &devices[i];
      break;
    }
  }
  
  ASSERT_NE(realDevice, nullptr) << "Real device not found in device list";
  
  EXPECT_NO_THROW(manager->SetCurrentPulserReceiver(*realDevice));
  
  // Verify it's selected
  EXPECT_TRUE(manager->getIsPulserReceiverSelected());
  
  PulserReceiverID currentId = manager->getId();
  EXPECT_EQ(currentId.PulserReceiverIndex, realDevice->PulserReceiverIndex);
}

TEST_F(JSRSDKManagerIntegrationTest, GetPulserReceiverInfo_ValidDevice_ReturnsInfo) {
  RequireDevice();
  
  size_t deviceCount = manager->GetPulserReceivers(nullptr, 0);
  ASSERT_GT(deviceCount, 0u);
  
  std::vector<PulserReceiverID> devices(deviceCount);
  manager->GetPulserReceivers(devices.data(), deviceCount);
  
  // Find the real device
  PulserReceiverID *realDevice = nullptr;
  for (size_t i = 0; i < deviceCount; ++i) {
    if (isCorrectDevice(devices[i])) {
      realDevice = &devices[i];
      break;
    }
  }
  
  ASSERT_NE(realDevice, nullptr) << "Real device not found in device list";
  
  size_t infoCount = manager->GetPulserReceiverInfo(*realDevice, nullptr, 0, 0);
  
  EXPECT_GT(infoCount, 0u) << "Expected device info";
}

TEST_F(JSRSDKManagerIntegrationTest, GetPluginsLoaded_AfterLoading_ReturnsTrue) {
  EXPECT_TRUE(manager->getPluginsLoaded());
}

//--------------------------------------------------------------------------
//  Device Property Tests (require connected device)
//--------------------------------------------------------------------------
TEST_F(JSRSDKManagerIntegrationTest, GetGainValues_ConnectedDevice_ReturnsValues) {
  RequireDevice();
  
  size_t gainCount = manager->getGainValues(nullptr, 0);
  
  if (gainCount > 0) {
    std::vector<double> gainValues(gainCount);
    manager->getGainValues(gainValues.data(), gainCount);
    
    // Verify values are reasonable (gain typically in dB)
    for (size_t i = 0; i < gainCount; ++i) {
      EXPECT_GE(gainValues[i], -100.0);
      EXPECT_LE(gainValues[i], 100.0);
    }
  }
}

TEST_F(JSRSDKManagerIntegrationTest, GetSetGainIndex_ValidIndex_Success) {
  RequireDevice();
  
  if (!manager->getGainIndexSupported()) {
    FAIL() << "Gain index not supported by device";
    return;
  }
  
  int maxIndex = manager->getGainIndexMax();
  EXPECT_GE(maxIndex, 0);
  
  // Try to set a valid index
  if (maxIndex > 0) {
    int testIndex = maxIndex / 2;  // Use middle value
    
    EXPECT_NO_THROW(manager->setGainIndex(testIndex));
    
    // Verify it was set
    int currentIndex = manager->getGainIndex();
    EXPECT_EQ(currentIndex, testIndex);
  }
}

TEST_F(JSRSDKManagerIntegrationTest, GetSetGain_ValidValue_Success) {
  RequireDevice();
  
  double minGain = manager->getGainMin();
  double maxGain = manager->getGainMax();
  
  EXPECT_LT(minGain, maxGain);
  
  // Set to middle value
  double testGain = (minGain + maxGain) / 2.0;
  
  EXPECT_NO_THROW(manager->setGain(testGain));
  
  // Read back and verify (may not be exact due to device limitations)
  double currentGain = manager->getGain();
  EXPECT_GE(currentGain, minGain);
  EXPECT_LE(currentGain, maxGain);
}

TEST_F(JSRSDKManagerIntegrationTest, GetSetReceiverMode_ValidMode_Success) {
  RequireDevice();
  
  bool echoSupported = manager->getReceiverModeEchoSupported();
  bool thruSupported = manager->getReceiverModeThruSupported();
  bool bothSupported = manager->getReceiverModeBothSupported();
  
  // At least one mode should be supported
  EXPECT_TRUE(echoSupported || thruSupported || bothSupported);
  
  // Try setting to ECHO if supported
  if (echoSupported) {
    EXPECT_NO_THROW(manager->setReceiverMode(C_RECEIVER_MODE::ECHO));
    C_RECEIVER_MODE mode = manager->getReceiverMode();
    EXPECT_EQ(mode, C_RECEIVER_MODE::ECHO);
  }
}

TEST_F(JSRSDKManagerIntegrationTest, GetPulseRepetitionFrequencyValues_ReturnsValidValues) {
  RequireDevice();
  
  size_t prfCount = manager->getPulseRepetitionFrequencyValues(nullptr, 0);
  
  if (prfCount > 0) {
    std::vector<double> prfValues(prfCount);
    manager->getPulseRepetitionFrequencyValues(prfValues.data(), prfCount);
    
    // PRF values should be positive and reasonable (Hz)
    for (size_t i = 0; i < prfCount; ++i) {
      EXPECT_GT(prfValues[i], 0.0);
      EXPECT_LT(prfValues[i], 1000000.0);  // Less than 1 MHz
    }
  }
}

TEST_F(JSRSDKManagerIntegrationTest, GetSetPulseRepetitionFrequency_ValidValue_Success) {
  RequireDevice();
  
  double minPRF = manager->getPulseRepetitionFrequencyMin();
  double maxPRF = manager->getPulseRepetitionFrequencyMax();
  
  EXPECT_GT(minPRF, 0.0);
  EXPECT_LT(minPRF, maxPRF);
  
  // Set to middle value
  double testPRF = (minPRF + maxPRF) / 2.0;
  
  EXPECT_NO_THROW(manager->setPulseRepetitionFrequency(testPRF));
  
  // Read back
  double currentPRF = manager->getPulseRepetitionFrequency();
  EXPECT_GE(currentPRF, minPRF);
  EXPECT_LE(currentPRF, maxPRF);
}

TEST_F(JSRSDKManagerIntegrationTest, GetSetHVSupply_ValidValue_Success) {
  RequireDevice();
  
  if (!manager->getHVSupplySupported()) {
    FAIL() << "HV Supply not supported by device";
    return;
  }
  
  double minHV = manager->getHVSupplyMin();
  double maxHV = manager->getHVSupplyMax();
  
  EXPECT_LT(minHV, maxHV);
  
  // Set to minimum (safest)
  EXPECT_NO_THROW(manager->setHVSupply(minHV));
  
  double currentHV = manager->getHVSupply();
  EXPECT_GE(currentHV, minHV);
  EXPECT_LE(currentHV, maxHV);
}

TEST_F(JSRSDKManagerIntegrationTest, GetHVMeasurement_ReturnsValidValue) {
  RequireDevice();
  
  if (!manager->getHVMeasurementSupported()) {
    FAIL() << "HV Measurement not supported by device";
    return;
  }
  
  double hvMeasurement = manager->getHVMeasurement();
  
  // Should be non-negative
  EXPECT_GE(hvMeasurement, 0.0);
  
  // Should be within reasonable range (0-1000V typical)
  EXPECT_LT(hvMeasurement, 2000.0);
}

//--------------------------------------------------------------------------
//  Trigger and Control Tests
//--------------------------------------------------------------------------
TEST_F(JSRSDKManagerIntegrationTest, GetSetTriggerEnable_Success) {
  RequireDevice();
  
  // Disable trigger
  EXPECT_NO_THROW(manager->setTriggerEnable(false));
  EXPECT_FALSE(manager->getTriggerEnable());
  
  // Enable trigger
  EXPECT_NO_THROW(manager->setTriggerEnable(true));
  EXPECT_TRUE(manager->getTriggerEnable());
  
  // Disable again for safety
  manager->setTriggerEnable(false);
}

TEST_F(JSRSDKManagerIntegrationTest, GetSetTriggerSource_ValidSource_Success) {
  RequireDevice();
  
  bool internalSupported = manager->getTriggerSourceInternalSupported();
  bool externalSupported = manager->getTriggerSourceExternalSupported();
  bool slaveSupported = manager->getTriggerSourceSlaveSupported();
  
  EXPECT_TRUE(internalSupported || externalSupported || slaveSupported);
  
  // Try setting to INTERNAL if supported
  if (internalSupported) {
    EXPECT_NO_THROW(manager->setTriggerSource(C_TRIGGER_SOURCE::INTERNAL));
    C_TRIGGER_SOURCE source = manager->getTriggerSource();
    EXPECT_EQ(source, C_TRIGGER_SOURCE::INTERNAL);
  }
}

TEST_F(JSRSDKManagerIntegrationTest, GetSetTriggerEdgePolarity_Success) {
  RequireDevice();
  
  if (!manager->getTriggerEdgePolaritySupported()) {
    FAIL() << "Trigger edge polarity not supported";
    return;
  }
  
  // Try setting to RISING
  EXPECT_NO_THROW(manager->setTriggerEdgePolarity(C_TRIGGER_POLARITY::RISING));
  C_TRIGGER_POLARITY polarity = manager->getTriggerEdgePolarity();
  EXPECT_EQ(polarity, C_TRIGGER_POLARITY::RISING);
  
  // Try setting to FALLING
  EXPECT_NO_THROW(manager->setTriggerEdgePolarity(C_TRIGGER_POLARITY::FALLING));
  polarity = manager->getTriggerEdgePolarity();
  EXPECT_EQ(polarity, C_TRIGGER_POLARITY::FALLING);
}

//--------------------------------------------------------------------------
//  Device Information Tests
//--------------------------------------------------------------------------
TEST_F(JSRSDKManagerIntegrationTest, GetInfo_ReturnsNonEmpty) {
  RequireDevice();
  
  size_t infoCount = manager->getInfo(nullptr, 0, 0);
  
  EXPECT_GT(infoCount, 0u) << "Expected device info";
  
  if (infoCount > 0) {
    std::vector<char*> buffers(infoCount);
    std::vector<std::vector<char>> storage(infoCount);
    
    for (size_t i = 0; i < infoCount; ++i) {
      storage[i].resize(256);
      buffers[i] = storage[i].data();
    }
    
    manager->getInfo(buffers.data(), infoCount, 256);
    
    // Verify info strings are not empty
    for (size_t i = 0; i < infoCount; ++i) {
      std::string infoStr(buffers[i]);
      EXPECT_FALSE(infoStr.empty()) << "Info string " << i << " is empty";
    }
  }
}

TEST_F(JSRSDKManagerIntegrationTest, GetUnitModelName_ConnectedDevice_ReturnsName) {
  RequireDevice();
  
  if (!manager->getUnitModelNameSupported()) {
    FAIL() << "Unit model name not supported";
    return;
  }
  
  char buffer[256];
  size_t size = manager->getUnitModelName(buffer, sizeof(buffer));
  
  EXPECT_GT(size, 0u);
  EXPECT_FALSE(std::string(buffer).empty());
}

TEST_F(JSRSDKManagerIntegrationTest, GetUnitSerialNum_ConnectedDevice_ReturnsSerialNum) {
  RequireDevice();
  
  if (!manager->getUnitSerialNumSupported()) {
    FAIL() << "Unit serial number not supported";
    return;
  }
  
  char buffer[256];
  size_t size = manager->getUnitSerialNum(buffer, sizeof(buffer));
  
  EXPECT_GT(size, 0u);
  std::string serial(buffer);
  EXPECT_FALSE(serial.empty());
  
  // Verify it's one of the expected devices
  EXPECT_TRUE(serial == "JB0104" || serial == "JB0118" || serial == "JC0100")
    << "Unexpected device serial: " << serial;
}

TEST_F(JSRSDKManagerIntegrationTest, GetPulserFirmwareVer_ReturnsVersion) {
  RequireDevice();
  
  if (!manager->getPulserFirmwareVerSupported()) {
    FAIL() << "Pulser firmware version not supported";
    return;
  }
  
  char buffer[256];
  size_t size = manager->getPulserFirmwareVer(buffer, sizeof(buffer));
  
  EXPECT_GT(size, 0u);
  EXPECT_FALSE(std::string(buffer).empty());
}

//--------------------------------------------------------------------------
//  Status Tests
//--------------------------------------------------------------------------
TEST_F(JSRSDKManagerIntegrationTest, GetPulserIsPulsing_ReturnsValidState) {
  RequireDevice();
  
  C_IS_PULSING pulsingState = manager->getPulserIsPulsing();
  
  // Should be a valid enum value
  EXPECT_TRUE(pulsingState == C_IS_PULSING::ACTIVE ||
              pulsingState == C_IS_PULSING::INACTIVE ||
              pulsingState == C_IS_PULSING::UNKNOWN);
}

TEST_F(JSRSDKManagerIntegrationTest, GetPulserPowerLimitStatus_ReturnsValidStatus) {
  RequireDevice();
  
  C_POWER_LIMIT powerLimit = manager->getPulserPowerLimitStatus();
  
  // Should be a valid enum value
  EXPECT_TRUE(powerLimit == C_POWER_LIMIT::WITHIN_LIMIT ||
              powerLimit == C_POWER_LIMIT::OVER_LIMIT);
}

TEST_F(JSRSDKManagerIntegrationTest, GetIsPulserPresent_ConnectedDevice_ReturnsTrue) {
  RequireDevice();
  
  if (!manager->getIsPulserPresentSupported()) {
    FAIL() << "Pulser presence detection not supported";
    return;
  }
  
  EXPECT_TRUE(manager->getIsPulserPresent());
}

//--------------------------------------------------------------------------
//  Error Handling Tests
//--------------------------------------------------------------------------
TEST_F(JSRSDKManagerIntegrationTest, GetLastExceptionOrNull_NoError_ReturnsZero) {
  char buffer[512];
  size_t size = manager->getLastExceptionOrNull(buffer, sizeof(buffer));
  
  // If size is 0, no exception occurred
  EXPECT_EQ(size, 0u);
}

//--------------------------------------------------------------------------
//  Cleanup Tests
//--------------------------------------------------------------------------
TEST_F(JSRSDKManagerIntegrationTest, Shutdown_Success) {
  C_MANAGER_STATE state = manager->getManagerState();
  EXPECT_EQ(state, C_MANAGER_STATE::PLUGINS_LOADED);

  EXPECT_NO_THROW(manager->Shutdown());
  state = manager->getManagerState();
  // After shutdown, manager state should change
  EXPECT_EQ(state, C_MANAGER_STATE::NOT_STARTED);
}

//--------------------------------------------------------------------------
//  main entry point
//--------------------------------------------------------------------------
#ifndef RUN_ALL_TESTS_WITH_MAIN
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  
  std::cout << "\n";
  std::cout << "========================================\n";
  std::cout << " JSRSDKManager Integration Tests\n";
  std::cout << "========================================\n";
  std::cout << "NOTE: These tests require a physical device to be connected.\n";
  std::cout << "      Expected devices: JB0104, JB0118, or JC0100\n";
  std::cout << "      Tests will FAIL if no real device is found.\n";
  std::cout << "========================================\n\n";
  
  return RUN_ALL_TESTS();
}
#endif
