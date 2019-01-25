#define SERIAl_DEBUG 1

#define FASTLED_ALLOW_INTERRUPTS 0 //if not working here, modify fastled_config.h to uncomment this same line
#include "FastLED.h"

#include "OSCManager.h"

CRGBArray<1> leds;

WifiManager wifiManager;
OSCManager oscManager(wifiManager.oscUDP);

#define BUTTON_PIN 16
bool buttonState;

void messageReceived(OSCMessage &msg)
{
  Serial.println("Message received");
  if(msg.match("/color"))
  {
    setColor(CRGB(msg.getFloat(0)*255, msg.getFloat(1)*255,msg.getFloat(2)*255)); 
  }
}


void setup() {
  Serial.begin(9600);

  FastLED.addLeds<WS2812B, 5, GRB>(leds, 1);
  FastLED.clear();

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  setColor(CRGB::Blue);
  
  wifiManager.init();
  if(wifiManager.isConnected)
  {
    setColor(CRGB::Green);
    delay(100);
    setColor(CRGB::Black);

    oscManager.init();
    oscManager.addCallbackMessageReceived(&messageReceived);
  }else
  {
    setColor(CRGB::Red);
  }
}

void loop() {
 oscManager.update();

 bool val = digitalRead(BUTTON_PIN);
 if(val != buttonState)
 {
  buttonState = val;
  Serial.println("Button "+String(buttonState));
  OSCMessage m("/button");
  m.add((int)buttonState);
  oscManager.sendMessage(m);
 }
 
 delay(10);
}


void setColor(CRGB color)
{
  Serial.println("Set color "+String(color.r)+", "+String(color.g)+", "+String(color.b));
  leds[0] = color;
  FastLED.show();
}
