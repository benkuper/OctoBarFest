#include "FastLED.h"
#define BUTTON_PIN 12
#define BUTTON_LED_PIN 13
#define LOCK_PIN 34

class InputManager
{
  public:
    CRGB buttonLeds[1];

    bool buttonState;
    bool lockState;

    long lastCheckTime = 0;

    InputManager()
    {
      addCallbackButtonEvent(&InputManager::defaultCallback);
      addCallbackLockEvent(&InputManager::defaultCallback);
    }


    void init() {

#if DEBUG
      Serial.println("InputManager init.");
#endif

      pinMode(LOCK_PIN, INPUT);

      pinMode(BUTTON_PIN, INPUT);
      FastLED.addLeds<WS2812B, BUTTON_LED_PIN, GRB>(buttonLeds, 1).setCorrection(TypicalLEDStrip);
    }

    void update()
    {
      long curTime = millis();
      if (curTime > lastCheckTime + 10) //100fps
      {
        lastCheckTime = curTime;

        //check button
        bool buttonVal = digitalRead(BUTTON_PIN);
        if (buttonVal != buttonState)
        {
          buttonState = buttonVal;
          onButtonEvent(buttonState);
        }

        //check lock
        bool lockVal = digitalRead(LOCK_PIN);
        if (lockVal != lockState)
        {
          lockState = lockVal;
          onLockEvent(lockState);
        }
      }
    }

    void setButtonColor(CRGB color)
    {
      buttonLeds[0] = color;
    }

    //Events
    typedef void(*onEvent)(bool);
    void (*onButtonEvent) (bool);
    void (*onLockEvent) (bool);

    void addCallbackButtonEvent(onEvent func) {
      onButtonEvent = func;
    }
    void addCallbackLockEvent(onEvent func) {
      onLockEvent = func;
    }

    static void defaultCallback(bool) {
      /* default callback */
    }
};
