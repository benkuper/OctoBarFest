class SerialManager
{
  public:
    char buffer[32];
    int bufferIndex = 0;

    SerialManager()
    {
      addCallbackSerialEvent(&SerialManager::defaultCallback);
    }

    void init()
    {
      Serial.begin(9600);

#if DEBUG
      Serial.println("SerialManager init.");
#endif
    }

    void update()
    {
      while (Serial.available())
      {
        char c = Serial.read();
        if (c == '\n')
        {
          onSerialEvent(buffer,bufferIndex);
          bufferIndex = 0;
        } else if (bufferIndex < 32)
        {
          buffer[bufferIndex] = c;
          bufferIndex++;
        }
      }
    }

    //Events
    typedef void(*onEvent)(const char *, int);
    void (*onSerialEvent) (const char *, int);

    void addCallbackSerialEvent(onEvent func) {
      onSerialEvent = func;
    }

    static void defaultCallback(const char *, int) {
      /* default callback */
    }
};
