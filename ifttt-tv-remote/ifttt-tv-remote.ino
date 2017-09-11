#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ThingerWifi.h>

#define USERNAME ""
#define DEVICE_ID ""
#define DEVICE_CREDENTIAL "" 
#include <IRremoteESP8266.h>

// The pins on my board have different labels 
// than the official Arduino pins:
//D0   = 16;
//D1   = 5;
//D2   = 4;
//D3   = 0;
//D4   = 2;
//D5   = 14;
//D6   = 12;
//D7   = 13;
//D8   = 15;
//RX   = 3;
//TX   = 1;

IRsend irsend(12); //an IR led is connected to the pin
unsigned long startTime = 0;
// IR Remote Codes
#define CODE_ON       0x2FD48B7
#define CODE_CH_NEXT  0x2FDD827
#define CODE_CH_PREV  0x2FDF807

#define CODE_VOL_UP   0x2FD58A7
#define CODE_VOL_DOWN 0x2FD7887
#define CODE_SOURCE_CHANGE 0x2FDF00F

// Wifi SSID and Pass
#define SSID ""
#define SSID_PASSWORD ""

// Connect to Thinger.io
ThingerWifi thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

void setup() {

  // If we want to use the built in status LED
  pinMode(BUILTIN_LED, OUTPUT);

  // IR LED
  pinMode(5, OUTPUT);
  pinMode(12, OUTPUT);
  digitalWrite(5, LOW);
  irsend.begin();
  thing.add_wifi(SSID, SSID_PASSWORD);
  
  // If thinger.io response is 'led', send the Power On command to my TV
  thing["led"] << [](pson& in){ 
    digitalWrite(5, HIGH);
    delay(100);
    irsend.sendNEC(CODE_ON, 32);
    digitalWrite(5, LOW);
  };

}

void loop() {
 // Every two seconds, check the API for a command
 unsigned long loopTime = millis() - startTime;
 if (loopTime > 2000) //If time is over 2000 millis, set the startTime to millis so the loop time will be reset to zero
 {
    thing.handle();
    startTime = millis();
 }
}
