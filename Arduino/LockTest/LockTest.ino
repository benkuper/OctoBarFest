long lastLockCheck;
bool locked;

void setup() {
  Serial.begin(9600);
  pinMode(34, INPUT);
}

void loop() {
  long curTime = millis();
  if (curTime > lastLockCheck + 20)
  {
    lastLockCheck = curTime;
    bool val = digitalRead(34);
    if (locked != val)
    {
      locked = val;
      Serial.println("Locked changed : " + String(locked));
    }
  }
}
