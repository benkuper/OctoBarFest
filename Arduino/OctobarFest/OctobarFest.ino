#include "FastLED.h"

#define DEBUG 1

#if DEBUG
#define DEBUG_PUMP 1
#define DEBUG_MOTOR 1
#define DEBUG_POURING 1
#define DEBUG_INPUT 1
#define DEBUG_SERIAL 1
#define DEBUG_OSC 1
#endif

#include "SerialManager.h"
#include "InputManager.h"
#include "LaserManager.h"
#include "MotorManager.h"
#include "PumpManager.h"
#include "LedStripManager.h"
#include "OSCManager.h"
//#include "WebServer.h"
#include "PouringManager.h"

////// MANAGERS

SerialManager serialManager;

WifiManager wifiManager;
OSCManager oscManager(wifiManager.oscUDP);

InputManager inputManager; //button & lock
LaserManager laserManager;
LedStripManager ledStripManager;

PumpManager pumpManager;
MotorManager motorManager;

PouringManager pouringManager;

//FastLED update
long lastLedUpdate = 0;




/////////////   SETUP

void setup() {

  serialManager.init();
  serialManager.addCallbackSerialEvent(&serialEvent);

  ledStripManager.init();

  inputManager.init();
  inputManager.addCallbackButtonEvent(&buttonEvent);
  inputManager.addCallbackLockEvent(&lockEvent);

  laserManager.init();
  laserManager.addCallbackLaserEvent(&laserEvent);

  wifiManager.init();

  oscManager.init();
  oscManager.addCallbackMessageReceived(&oscEvent);

  pouringManager.init();
  pouringManager.addCallbackAskPositionEvent(&pouringAskPositionEvent);
  pouringManager.addCallbackAskPumpEvent(&pouringAskPumpEvent);

  Serial.println("OctobarFest 1.0 Ready.");
}


////// UPDATE

void loop()
{
  serialManager.update();
  oscManager.update();

  inputManager.update();
  laserManager.update();
  motorManager.update();

  pouringManager.update();

  updateLeds();
 
}

/////// EVENTS

void serialEvent(const char * buffer, int length)
{
#if DEBUG_SERIAL
  Serial.println("Serial Event : " + String(length) + " bytes");
#endif

  if (length == 0) return;

  char cmd = buffer[0];

  switch (cmd)
  {

  }
}

void oscEvent(OSCMessage &msg)
{
#if DEBUG_OSC
  char address[32];
  msg.getAddress(address);
  Serial.println("Message received : " + String(address));
#endif

  if (msg.match("/button"))
  {
    bool val = msg.size() > 0 ? msg.getInt(0) > 0 : 0;

#if DEBUG_OSC
    Serial.println("Got button ! " + String(val));
#endif

    inputManager.setButtonColor(CRGB(val ? 1 : 0, 1, 0));
  }
}

void buttonEvent(bool pressed)
{
#if DEBUG_INPUT
  Serial.println("Button change : " + String(pressed));
#endif
}

void lockEvent(bool locked)
{
#if DEBUG_INPUT
  Serial.println("Lock change : " + String(locked));
#endif
}

void laserEvent(int id, bool value)
{
#if DEBUG_LASER
  Serial.println("Laser change : " + String(id) + " : " + String(value));
#endif
}

void pouringAskPositionEvent(int pos)
{
#if DEBUG_POURING
  Serial.println("Pouring ask positition  " + String(pos));
#endif
}

void pouringAskPumpEvent(int pumpID, bool value)
{
#if DEBUG_POURING
  Serial.println("Pouring ask pump " + String(pumpID) + " : " + String(value));
#endif
}



//FastLED unified
void updateLeds()
{
  long curTime = millis();
  if (curTime > lastLedUpdate + 20) //50fps
  {
    FastLED.show();
    lastLedUpdate = curTime;
  }
}
