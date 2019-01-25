#define NUM_LASERS 8

class LaserManager
{
  public:

    const int laserPins[NUM_LASERS]{ 27,26,25,33,32,35,39,36 };
    bool laserStates[NUM_LASERS];
    
    long lastLaserCheck = 0;

    int laserThreshold = 4000;

    LaserManager()
    {
        addCallbackLaserEvent(&LaserManager::defaultCallback);
    }
    
    void init()
    {

#if DEBUG
      Serial.println("LaserManager init.");
#endif

      for (int i = 0; i < NUM_LASERS; i++)
      {
        pinMode(laserPins[i], INPUT);
        laserStates[i] = false;
      }
    }

    void update()
    {
      long curTime = millis();
      if(curTime > lastLaserCheck + 100)
      {
        lastLaserCheck = curTime;
        
        for(int i=0;i<NUM_LASERS;i++)
        {
          int val = analogRead(laserPins[i]);
          bool laserVal = val < laserThreshold;
          if(laserStates[i] != val)
          {
            laserStates[i] = val;
            onLaserEvent(i+1, val); //id starts at 1
          }
        }
      }
    }

    bool getLaserState(int id)
    {
      if(id < 1 || id > NUM_LASERS) return false;
      return laserStates[id-1];
    }

    typedef void(*onEvent)(int, bool);
    void (*onLaserEvent) (int, bool);

    void addCallbackLaserEvent(onEvent func) {
      onLaserEvent = func;
    }

    static void defaultCallback(int, bool) {
      /* default callback */
    }
};
