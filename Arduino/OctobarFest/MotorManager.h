#include <AccelStepper.h>

#define NUM_POSITIONS 8

class MotorManager
{
  public:
    AccelStepper * stepper;
    int completeTurn = 21600;
    
    bool calibMode = false;
    int calibSpeed = 0;

    int maxAcceleration = 2000;
    int maxSpeed = 2000;

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
        stepper->runSpeed();
        return;
      }

      stepper->run();
    }

    void moveToPosition(int posIndex)
    {
      int targetPos = map(posIndex,1,9,0,completeTurn);
      #if DEBUG_MOTOR
      Serial.println("Move to Bar Position : "+String(posIndex)+", target position :"+String(targetPos));
      #endif
      if (posIndex < 1 || posIndex > 8) return;
      stepper->moveTo(targetPos);// positions[posIndex - 1]);
    }

    void moveToStepPosition(int stepPos)
    {
      #if DEBUG_MOTOR
      Serial.println("Move to STEP Position : "+String(stepPos));
      #endif
      
      stepper->moveTo(stepPos);
    }

    void setMaxValues(int maxAccel, int maxS)
    {
      maxAcceleration = maxAccel;
      maxSpeed = maxS;
      stepper->setAcceleration(maxAcceleration);
      stepper->setMaxSpeed(maxSpeed);

      //Save preferences
    }

    void setCalibSpeed(int speed)
    {
      #if DEBUG_MOTOR
      Serial.println("Set Speed : "+String(speed));
      #endif
      
      setCalibMode(true);
      calibSpeed = speed;
      stepper->setSpeed(calibSpeed);
    }

    void setCalibMode(bool val)
    {

      if(calibMode == val) return;

      #if DEBUG_MOTOR
      Serial.println("Set Calib : "+String(val));
      #endif


      calibMode = val;
      calibSpeed = 0;
    }

    void calibrateZero()
    {
      #if DEBUG_MOTOR
      Serial.println("Calibrate zero");
      #endif

      setCalibMode(false);
      stepper->setCurrentPosition(0);
    }

};
