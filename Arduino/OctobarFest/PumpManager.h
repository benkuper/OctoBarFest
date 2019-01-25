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
      for(int i=0;i<NUM_PUMPS;i++)
      {
        pinMode(pumpPins[i], OUTPUT);
        setPump(i+1, false);
      }
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
