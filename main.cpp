// #include <Arduino.h>
// #include <BLEDevice.h>
// #include <BLEUtils.h>
// #include <BLEScan.h>
// #include <BLEAdvertisedDevice.h>

// int scanTime = 5; //In seconds
// BLEScan* pBLEScan;

// class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
//     void onResult(BLEAdvertisedDevice advertisedDevice) {
//       Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
//     }
// };

// void setup() {
//   Serial.begin(115200);
//   Serial.println("Scanning...");

//   BLEDevice::init("");
//   pBLEScan = BLEDevice::getScan(); //create new scan
//   pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
//   pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
//   pBLEScan->setInterval(100);
//   pBLEScan->setWindow(99);  // less or equal setInterval value
// }

// void loop() {
//   // put your main code here, to run repeatedly:
//   BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
//   Serial.print("Devices found: ");
//   Serial.println(foundDevices.getCount());
//   Serial.println("Scan done!");
//   pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
//   delay(10000);
// }


// #include <Arduino.h>
// #include <BLEDevice.h>
// #include <BLEUtils.h>
// #include <BLEScan.h>
// #include <BLEAdvertisedDevice.h>
// // Client Code
// #include "BLEDevice.h"
// //#include "BLEScan.h"

// // TODO: change the service UUID to the one you are using on the server side.
// // The remote service we wish to connect to.
// static BLEUUID serviceUUID("3a61d668-3a41-4ad9-bd56-c4940609ac0e");
// // The characteristic of the remote service we are interested in.
// static BLEUUID    charUUID("beb5483e-36e1-4688-b7f5-ea07361b26a8");

// static boolean doConnect = false;
// static boolean connected = false;
// static boolean doScan = false;
// static BLERemoteCharacteristic* pRemoteCharacteristic;
// static BLEAdvertisedDevice* myDevice;

// // TODO: define new global variables for data collection

// // TODO: define a new function for data aggregation

// static void notifyCallback(
//   BLERemoteCharacteristic* pBLERemoteCharacteristic,
//   uint8_t* pData,
//   size_t length,
//   bool isNotify) {
//     // TODO: add codes to handle the data received from the server, and call the data aggregation function to process the data

//     // TODO: change the following code to customize your own data format for printing
//     Serial.print("Notify callback for characteristic ");
//     Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
//     Serial.print(" of data length ");
//     Serial.println(length);
//     Serial.print("data: ");
//     Serial.write(pData, length);
//     Serial.println();
// }

// class MyClientCallback : public BLEClientCallbacks {
//   void onConnect(BLEClient* pclient) {
//   }

//   void onDisconnect(BLEClient* pclient) {
//     connected = false;
//     Serial.println("onDisconnect");
//   }
// };

// bool connectToServer() {
//     Serial.print("Forming a connection to ");
//     Serial.println(myDevice->getAddress().toString().c_str());

//     BLEClient*  pClient  = BLEDevice::createClient();
//     Serial.println(" - Created client");

//     pClient->setClientCallbacks(new MyClientCallback());

//     // Connect to the remove BLE Server.
//     pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
//     Serial.println(" - Connected to server");
//     pClient->setMTU(517); //set client to request maximum MTU from server (default is 23 otherwise)

//     // Obtain a reference to the service we are after in the remote BLE server.
//     BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
//     if (pRemoteService == nullptr) {
//       Serial.print("Failed to find our service UUID: ");
//       Serial.println(serviceUUID.toString().c_str());
//       pClient->disconnect();
//       return false;
//     }
//     Serial.println(" - Found our service");

//     // Obtain a reference to the characteristic in the service of the remote BLE server.
//     pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
//     if (pRemoteCharacteristic == nullptr) {
//       Serial.print("Failed to find our characteristic UUID: ");
//       Serial.println(charUUID.toString().c_str());
//       pClient->disconnect();
//       return false;
//     }
//     Serial.println(" - Found our characteristic");

//     // Read the value of the characteristic.
//     if(pRemoteCharacteristic->canRead()) {
//       std::string value = pRemoteCharacteristic->readValue();
//       Serial.print("The characteristic value was: ");
//       Serial.println(value.c_str());
//     }

//     if(pRemoteCharacteristic->canNotify())
//       pRemoteCharacteristic->registerForNotify(notifyCallback);

//     connected = true;
//     return true;
// }
// /**
//  * Scan for BLE servers and find the first one that advertises the service we are looking for.
//  */
// class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
//   /**
//    * Called for each advertising BLE server.
//    */
//   void onResult(BLEAdvertisedDevice advertisedDevice) {
//     Serial.print("BLE Advertised Device found: ");
//     Serial.println(advertisedDevice.toString().c_str());

//     // We have found a device, let us now see if it contains the service we are looking for.
//     if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

//       BLEDevice::getScan()->stop();
//       myDevice = new BLEAdvertisedDevice(advertisedDevice);
//       doConnect = true;
//       doScan = true;

//     } // Found our server
//   } // onResult
// }; // MyAdvertisedDeviceCallbacks

// void setup() {
//   Serial.begin(115200);
//   Serial.println("Starting Arduino BLE Client application...");
//   BLEDevice::init("");

//   // Retrieve a Scanner and set the callback we want to use to be informed when we
//   // have detected a new device.  Specify that we want active scanning and start the
//   // scan to run for 5 seconds.
//   BLEScan* pBLEScan = BLEDevice::getScan();
//   pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
//   pBLEScan->setInterval(1349);
//   pBLEScan->setWindow(449);
//   pBLEScan->setActiveScan(true);
//   pBLEScan->start(5, false);
// } // End of setup.

// // This is the Arduino main loop function.
// void loop() {
//   // If the flag "doConnect" is true then we have scanned for and found the desired
//   // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
//   // connected we set the connected flag to be true.
//   if (doConnect == true) {
//     if (connectToServer()) {
//       Serial.println("We are now connected to the BLE Server.");
//     } else {
//       Serial.println("We have failed to connect to the server; there is nothin more we will do.");
//     }
//     doConnect = false;
//   }
 
//   // If we are connected to a peer BLE Server, update the characteristic each time we are reached
//   // with the current time since boot.
//   if (connected) {
//     String newValue = "Time since boot: " + String(millis()/1000);
//     Serial.println("Setting new characteristic value to \"" + newValue  + "\"");

//     // Set the characteristic's value to be the array of bytes that is actually a string.
//     pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
//   }else if(doScan){
//     BLEDevice::getScan()->start(0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
//   }

//   delay(1000); // Delay a second between loops.
// } // End of loop


#include <Arduino.h>

#include <BLEDevice.h>

#include <BLEUtils.h>

#include <BLEScan.h>

#include <BLEClient.h>

#include <BLERemoteCharacteristic.h>

#include <BLEAdvertisedDevice.h>
 
// MUST match your server UUIDs exactly

#define SERVICE_UUID        "3a61d668-3a41-4ad9-bd56-c4940609ac0e"

#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
 
// Optional: give your client a name

static const char* CLIENT_NAME = "Coleman Client";
 
// Globals

static BLERemoteCharacteristic* pRemoteCharacteristic = nullptr;

static BLEAdvertisedDevice* myDevice = nullptr;
 
static bool doConnect = false;

static bool connected = false;
 
// Min/max tracking since connection

static float currentVal = NAN;

static float minVal = 1e9;

static float maxVal = -1e9;
 
// --- Notification callback: runs EVERY time server notifies ---

static void notifyCallback(

  BLERemoteCharacteristic*,

  uint8_t* pData,

  size_t length,

  bool

) {

  // Server sends ASCII like "17.6"

  char buf[32];

  size_t n = (length < sizeof(buf) - 1) ? length : (sizeof(buf) - 1);

  memcpy(buf, pData, n);

  buf[n] = '\0';
 
  float x = atof(buf);

  currentVal = x;
 
  if (x < minVal) minVal = x;

  if (x > maxVal) maxVal = x;
 
  // REQUIRED: print current, min, max EVERY time you receive new data

  Serial.print("Current(cm): ");

  Serial.print(currentVal, 1);

  Serial.print(" | Min(cm): ");

  Serial.print(minVal, 1);

  Serial.print(" | Max(cm): ");

  Serial.println(maxVal, 1);

}
 
class MyClientCallbacks : public BLEClientCallbacks {

  void onConnect(BLEClient*) override {}
 
  void onDisconnect(BLEClient*) override {

    connected = false;

    Serial.println("Disconnected. Restart scan to reconnect...");

    BLEDevice::getScan()->start(0, false);

  }

};
 
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {

  void onResult(BLEAdvertisedDevice advertisedDevice) override {

    // Look for the service UUID (strongest match)

    if (advertisedDevice.haveServiceUUID() &&

        advertisedDevice.isAdvertisingService(BLEUUID(SERVICE_UUID))) {

      Serial.println("Found server advertising our SERVICE_UUID.");

      myDevice = new BLEAdvertisedDevice(advertisedDevice);

      doConnect = true;

      advertisedDevice.getScan()->stop();

    }

  }

};
 
bool connectToServer() {

  Serial.println("Connecting to server...");
 
  BLEClient* pClient = BLEDevice::createClient();

  pClient->setClientCallbacks(new MyClientCallbacks());
 
  if (!pClient->connect(myDevice)) {

    Serial.println("FAILED to connect.");

    return false;

  }
 
  Serial.println("Connected! (take 4.2 screenshot on this client if needed)");
 
  BLERemoteService* pRemoteService = pClient->getService(BLEUUID(SERVICE_UUID));

  if (pRemoteService == nullptr) {

    Serial.println("Could not find service UUID on server.");

    pClient->disconnect();

    return false;

  }
 
  pRemoteCharacteristic = pRemoteService->getCharacteristic(BLEUUID(CHARACTERISTIC_UUID));

  if (pRemoteCharacteristic == nullptr) {

    Serial.println("Could not find characteristic UUID on server.");

    pClient->disconnect();

    return false;

  }
 
  // Subscribe to notifications

  if (pRemoteCharacteristic->canNotify()) {

    pRemoteCharacteristic->registerForNotify(notifyCallback);

    Serial.println("Subscribed to notifications.");

  } else {

    Serial.println("Characteristic cannot notify.");

  }
 
  // Reset min/max on new connection (rubric says “since connection”)

  minVal = 1e9;

  maxVal = -1e9;

  currentVal = NAN;
 
  connected = true;

  return true;

}
 
void setup() {

  Serial.begin(115200);

  delay(500);

  Serial.println("Starting BLE client...");
 
  BLEDevice::init(CLIENT_NAME);
 
  BLEScan* pScan = BLEDevice::getScan();

  pScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());

  pScan->setInterval(1349);

  pScan->setWindow(449);

  pScan->setActiveScan(true);
 
  Serial.println("Scanning for server...");

  pScan->start(0, false);  // continuous scan until found

}
 
void loop() {

  if (doConnect && !connected) {

    if (connectToServer()) {

      Serial.println("Ready to receive distance data.");

    } else {

      Serial.println("Retry scanning...");

      BLEDevice::getScan()->start(0, false);

    }

    doConnect = false;

  }
 
  delay(200);

}

 