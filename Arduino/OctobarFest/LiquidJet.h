class LiquidJet
{
public:
    int ingredientID;
    int pumpPin;
    int targetPosition;
    int jetTimeToGlass;

    long timeAtArm;
    long timeAtOpen;
    bool isArmed;
    bool isOpen;

    long pourDelay; //
    long pourTime; //when serving
    
    LiquidJet()
    {
      pinMode(pumpPin,OUTPUT);
    }

    void launch(long _pourDelay, long _pourTime)
    {
      setOpen(false);
      
      timeAtArm = millis();
      isArmed = true;
      
      pourDelay = _pourDelay;
      pourTime = _pourTime;
    }

    void update()
    {
      if(isOpen)
      {
        if(millis() > timeAtOpen+pourTime)
        {
          setOpen(false);
        }
      }else if(isArmed)
      {
        if(millis() > timeAtArm+pourDelay)
        {
          setOpen(true);
        }
      }
    }

    void setOpen(bool value)
    {
      isOpen = value;
      if(isOpen) timeAtOpen = millis();
      isArmed = false; // reset always arm state when opening or closing
      digitalWrite(pumpPin,value?HIGH:LOW);
    }
};

