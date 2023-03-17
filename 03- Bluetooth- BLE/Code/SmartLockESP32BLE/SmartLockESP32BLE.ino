#include "BluetoothSerial.h"

String device_name = "ESP32-SmartLock";

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;

void setup() {
  pinMode(2,OUTPUT);
  digitalWrite(2,HIGH);
  Serial.begin(115200);
  SerialBT.begin(device_name); //Bluetooth device name
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
  //Serial.printf("The device with name \"%s\" and MAC address %s is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str(), SerialBT.getMacString()); // Use this after the MAC method is implemented

}

void loop() {
  if (SerialBT.available()) {
    char value=SerialBT.read();
    if(value=='1'){
      digitalWrite(2,LOW);
    }else if (value=='0'){
      digitalWrite(2,HIGH);
    }
  }
  delay(20);
}
