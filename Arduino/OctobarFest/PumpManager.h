#define NUM_PUMPS 8

class PumpManager
{
  public:

    const int pumpPins[NUM_PUMPS] {4, 16, 17, 5, 18, 19, 21, 22};

    void init()
    {
#if DEBUG
      Serial.println("PumpManager init.");
#endif
    }

    void update()
    {
    }

    void setPump(int id, bool value)
    {
      if(id < 1 || id > NUM_PUMPS) return;
      
#if DEBUG_PUMP
      Serial.println("Set Pump " + String(id) + " : " + String(value));
#endif

      digitalWrite(pumpPins[id-1], value ? HIGH : LOW);
    }
    
};
