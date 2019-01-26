#include "FastLED.h"

#define NUM_POSITIONS 8
#define POURING_NEEDS_LOCK 0

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

  pumpManager.init();
  motorManager.init();

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
  else if(msg.match("/phone/pump/1")) pumpManager.setPump(1,msg.getFloat(0) > 0);
  else if(msg.match("/phone/pump/2")) pumpManager.setPump(2,msg.getFloat(0) > 0);
  else if(msg.match("/phone/pump/3")) pumpManager.setPump(3,msg.getFloat(0) > 0);
  else if(msg.match("/phone/pump/4")) pumpManager.setPump(4,msg.getFloat(0) > 0);
  else if(msg.match("/phone/pump/5")) pumpManager.setPump(5,msg.getFloat(0) > 0);
  else if(msg.match("/phone/pump/6")) pumpManager.setPump(6,msg.getFloat(0) > 0);
  else if(msg.match("/phone/pump/7")) pumpManager.setPump(7,msg.getFloat(0) > 0);
  else if(msg.match("/phone/pump/8")) pumpManager.setPump(8,msg.getFloat(0) > 0);
  else if(msg.match("/phone/start")) pouringManager.start();
  else if(msg.match("/phone/finish")) pouringManager.finish();
  else if(msg.match("/phone/calibrateMotor")) motorManager.calibrateZero();
  else if(msg.match("/phone/calib")) motorManager.setCalibMode(msg.getInt(0) > 0);
  else if(msg.match("/phone/speed"))
  {
    motorManager.setCalibSpeed(msg.getFloat(0) * 1000);
  }
  else if(msg.match("/motor/position")) motorManager.moveToPosition(msg.getInt(0));
  else if(msg.match("/phone/position")) motorManager.moveToPosition((int)msg.getFloat(0));
  else if(msg.match("/motor/stepPos")) motorManager.moveToStepPosition(msg.getInt(0));
  else if(msg.match("/phone/zero") || msg.match("/motor/zero")) motorManager.calibrateZero();
}

void buttonEvent(bool pressed)
{
#if DEBUG_INPUT
  Serial.println("Button change : " + String(pressed));
#endif

  //temp
  if (pressed)
  {

#if POURING_NEEDS_LOCK
  bool lockOk = !inputManager.lockState;
#else
  bool lockOk = true;
#endif

    if (lockOk)
    {
      for (int i = 0; i < NUM_POSITIONS; i++)
      {
        pouringManager.glasses[i].isPresent = true;
      }

      
      pouringManager.assignRandomMixesToGlasses();
      pouringManager.start();
    } else
    {
#if DEBUG_INPUT
      Serial.println("Must unlock before starting !");
#endif
    }
  }
}

void lockEvent(bool locked)
{
#if DEBUG_INPUT
  Serial.println("Lock change : " + String(locked));
#endif

#if POURING_NEEDS_LOCK
  if (locked) pouringManager.finish();
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
  motorManager.moveToPosition(pos);
}

void pouringAskPumpEvent(int pumpID, bool value)
{
#if DEBUG_POURING
  Serial.println("Pouring ask pump " + String(pumpID) + " : " + String(value));
#endif
  pumpManager.setPump(pumpID, value);
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
