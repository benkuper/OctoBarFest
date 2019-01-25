#include <AccelStepper.h>

#define NUM_POSITIONS 8

class MotorManager
{
  public:
    AccelStepper * stepper;
    int positions[NUM_POSITIONS] { 0, 1000, 2000, 3000, 4000, 5000, 6000, 7000 };

    bool calibMode = false;
    int calibSpeed = 0;

    int maxAcceleration = 5000;
    int maxSpeed = 8000;

    void init()
    {
      #if DEBUG
      Serial.println("MotorManager init.");
      #endif
      
      stepper = new AccelStepper(AccelStepper::DRIVER, 2, 15); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5

      stepper->setAcceleration(maxAcceleration);
      stepper->setMaxSpeed(maxSpeed);
    }

    void update()
    {
      if (calibMode)
      {
        //stepper->runSpeed();
        return;
      }

      //stepper->run();
    }

    void saveCurrentPosition(int pos)
    {
#if DEBUG_MOTOR
      Serial.println("Position " + String(pos) + " saved to " + String(stepper->currentPosition()));
#endif
      positions[pos] = stepper->currentPosition();
    }

    void moveToPosition(int posIndex)
    {
      if (posIndex < 1 || posIndex > 8) return;
      //stepper->moveToPosition(positions[posIndex - 1]);
    }

    void setMaxAcceleration(int maxAccel, int maxS)
    {
      maxAcceleration = maxAccel;
      maxSpeed = maxS;
      //stepper->setAcceleration(maxAcceleration);
      //stepper->setMaxSpeed(maxSpeed);

      //Save preferences
    }

    void setCalibMode(bool val)
    {
      calibMode = val;
      calibSpeed = 0;
    }

    void calibrateZero()
    {
      //stepper->setCurrentPosition(0);
    }

};
