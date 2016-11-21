/*
 DayShift
 
 Uses a short 2ms pulse of light every 10s from 4am until wakeup to shift the circadian rythm.
 
 https://www.ncbi.nlm.nih.gov/pmc/articles/PMC4767340/
 http://blog.dansplan.com/a-new-product-to-significantly-reduce-jet-lag-interview-with-stanford-professor-jamie-zeitzer/
 
 November 2016, Peter Kuhar & Helena Jagodnik
*/

// Configure this if your board is not Arduino 101
// This are configurable from the app on arduino 101

double initialTimeOfDay = 23;   // the time of powering up the board
double flashStartTime   = 4.5;  // when in the morning you want the flashing start
double flashEndTime     = 8.0;    // when do you want the flashign end
double secondBetweenFlashes = 10.0;

// end of configuration section

#define LED_STATUS 13
#define LED_FLASH 12

// This is the current time when the sketch starts running.
// 0-24h

long dayStartMillis = 0;

void setTimeOfDay(double timeOfDay) {
  dayStartMillis = millis() - timeOfDay * 3600.0 * 1000.0;
}

// return time of day in 0 - 24.0 double format
double timeOfDay() {
  return ((double)((millis() - dayStartMillis) % (24 * 3600 * 1000))) / 1000.0 /
         3600.0;
}

void setup_ble();
void loop_ble();

void setup() {
  setTimeOfDay(initialTimeOfDay);

  setup_ble();

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_STATUS, OUTPUT);
  pinMode(LED_FLASH, OUTPUT);

  digitalWrite(LED_STATUS, LOW);
  digitalWrite(LED_FLASH, LOW);

  Serial.begin(9600);
}

void loop_flash() {
  if (timeOfDay() >= flashStartTime && timeOfDay() <= flashEndTime) {
    // in the flash zone.
    static long lastFlashMillis = 0;
    if (millis() > lastFlashMillis + secondBetweenFlashes * 1000) {
      lastFlashMillis = millis();
      digitalWrite(LED_STATUS, HIGH);
      digitalWrite(LED_FLASH, HIGH);
      delay(2);
      digitalWrite(LED_STATUS, LOW);
      digitalWrite(LED_FLASH, LOW);
      Serial.println("Flash");
    }
  }
}

// the loop function runs over and over again forever
void loop() {
  static bool serialInitialized = false;
  if (Serial && !serialInitialized) {
    serialInitialized = true;
    Serial.println("DayShift");
  }
  loop_flash();
  loop_ble();
}

#ifndef ARDUINO_ARCH_ARC32
void setup_ble() {}
void loop_ble() {}
#endif

/**
 Arduino 101
 Use Bluetooth Low energy to set settings
 */

#ifdef ARDUINO_ARCH_ARC32
#include <CurieBLE.h>

BLEPeripheral
    blePeripheral; // BLE Peripheral Device (the board you're programming)
BLEService
    ledService("19B10000-E8F2-537E-4F6C-D104768A1214"); // BLE LED Service

// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by
// central
BLEFloatCharacteristic
    startTimeCharacteristic("19B10002-E8F2-537E-4F6C-D104768A1214",
                            BLERead | BLEWrite);
BLEFloatCharacteristic
    endTimeCharacteristic("19B10003-E8F2-537E-4F6C-D104768A1214",
                          BLERead | BLEWrite);
BLEFloatCharacteristic
    intervalCharacteristic("19B10004-E8F2-537E-4F6C-D104768A1214",
                           BLERead | BLEWrite);
BLEFloatCharacteristic
    timeOfDayCharacteristic("19B10005-E8F2-537E-4F6C-D104768A1214",
                            BLERead | BLEWrite);

void setup_ble() {
  // set advertised local name and service UUID:
  blePeripheral.setLocalName("DayShift");
  blePeripheral.setAdvertisedServiceUuid(ledService.uuid());

  // add service and characteristic:
  blePeripheral.addAttribute(ledService);
  blePeripheral.addAttribute(startTimeCharacteristic);
  blePeripheral.addAttribute(endTimeCharacteristic);
  blePeripheral.addAttribute(intervalCharacteristic);
  blePeripheral.addAttribute(timeOfDayCharacteristic);

  // set the initial value for the characeristic:
  startTimeCharacteristic.setValue(flashStartTime);
  endTimeCharacteristic.setValue(flashEndTime);
  intervalCharacteristic.setValue(secondBetweenFlashes);
  timeOfDayCharacteristic.setValue(initialTimeOfDay);

  // begin advertising BLE service:
  blePeripheral.begin();
}

// BLECentral central = blePeripheral.central();

void loop_ble() {
  static bool wasConnected = false;
  static BLECentral central = blePeripheral.central();

  if (!central) {
    // pull for new connection
    central = blePeripheral.central();
  }

  if (central && wasConnected == false) {
    // new connection
    wasConnected = true;

    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());
  } else if (central && central.connected()) {
    // loop code

    if (startTimeCharacteristic.written()) {
      flashStartTime = startTimeCharacteristic.value();
      Serial.print("startTime:");
      Serial.println(flashStartTime);
    }

    if (endTimeCharacteristic.written()) {
      flashEndTime = endTimeCharacteristic.value();
      Serial.print("endTime:");
      Serial.println(flashEndTime);
    }

    if (intervalCharacteristic.written()) {
      secondBetweenFlashes = intervalCharacteristic.value();
      Serial.print("interval:");
      Serial.println(secondBetweenFlashes);
    }

    if (timeOfDayCharacteristic.written()) {
      double time = timeOfDayCharacteristic.value();
      setTimeOfDay(time);
      Serial.print("currentTime:");
      Serial.println(time);
    }
  } else if (central && !central.connected()) {
    // disconnected
    wasConnected = false;
    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());

    central = blePeripheral.central(); // prepare for new connection
  }
}

#endif //#ifdef ARDUINO_ARCH_ARC32
