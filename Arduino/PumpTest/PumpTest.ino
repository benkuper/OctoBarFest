#define NUM_PUMPS 8

const int pumpPins[NUM_PUMPS] {4, 16, 17, 5, 18, 19, 21, 22};


char buffer[32];
int bufferIndex = 0;

long lastLockCheck;
bool locked;
int lockPump = 0;

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < NUM_PUMPS; i++)
  {
    pinMode(pumpPins[i], OUTPUT);
    setPump(i, false);
  }

  pinMode(34, INPUT);
  
  //pinMode(13, INPUT);
}

bool prevP = false;
void loop() {

  processSerial();
  checkLock();
}

void processSerial()
{
  while(Serial.available())
  {
    char c = Serial.read();
    if(c == '\n')
    {
      processBuffer();
      bufferIndex = 0;
    }else if(bufferIndex < 32)
    {
      buffer[bufferIndex] = c;
      bufferIndex++;
    }
  }
}

void checkLock()
{
  long curTime = millis();
  if (curTime > lastLockCheck + 20)
  {
    lastLockCheck = curTime;
    bool val = digitalRead(34);
    if (locked != val)
    {
      locked = val;
      Serial.println("Locked changed : " + String(locked));

      if(val)
      {
        lockPump = (lockPump+1)%NUM_PUMPS;
        Serial.println("Pump from lock "+String(lockPump));
        setPump(lockPump, true);
      }else
      {
        setPump(lockPump,false);
      }
    }
  }
}

void processBuffer()
{
  char cmd = buffer[0];
  switch(cmd)
  {
    case 'p':
      setPump(buffer[1]-48, buffer[2] > 48);
      break;
  }
}

void setPump(int id, bool value)
{
  Serial.println("Set Pump "+String(id)+" : "+String(value));
  digitalWrite(pumpPins[id], value ? HIGH : LOW);
}
