#include "FastLED.h"
FASTLED_USING_NAMESPACE

class LedStripManager
{
public:  
  #define LEDS_PIN 23
  #define NUM_LEDS 98

  CRGB leds[NUM_LEDS];
  
  void init() {
    Serial.println("LedStrip init.");
    FastLED.addLeds<WS2812B, LEDS_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    fullColor(CRGB(10,0,10)); 
  }

  void update()
  {
    
  }

  void clear()
  {
    fullColor(CRGB::Black);
  }

  void addPoint(CRGB color, float position, float size, float fade)
  {
    
  }
  
  void fullColor(CRGB color)
  {
    for(int i=0;i<NUM_LEDS;i++) leds[i] = color;  
  }

};
