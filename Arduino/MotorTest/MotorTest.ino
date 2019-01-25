#include <AccelStepper.h>

#define TEST_SPEED 1

// Define a stepper and the pins it will use
AccelStepper stepper(AccelStepper::DRIVER,2,15); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5

bool faultVal;

bool active = true;

#if TEST_SPEED
int curSpeed = 0;
#endif

void setup()
{  
  Serial.begin(9600);
  // Change these to suit your stepper if you want
 
  stepper.setAcceleration(5000);
  stepper.setMaxSpeed(8000);
  stepper.moveTo(12000);

  
  #if TEST_SPEED
  stepper.setSpeed(curSpeed);
  return;
  #endif
}

void loop()
{
  #if TEST_SPEED
   while(Serial.available() > 0)
   {
      char c = Serial.read();
      switch(c)
      {
        case '+': curSpeed += 10; break;
        case '-': curSpeed -= 10; break;
        case '*': curSpeed += 100; break;
        case '/': curSpeed -= 100; break;
        case '0': curSpeed = 0; break;
        case '1': curSpeed = 1000; break;
        case '2': curSpeed = 2000; break;
        case '3': curSpeed = 3000; break;
        case '4': curSpeed = 4000; break;
        case 'a': 
        case '.':
        {
          active = !active;
          Serial.println("active "+String(active));
          if(active)stepper.moveTo(-stepper.currentPosition());
        }
         break;
      }

      stepper.setSpeed(curSpeed);
      Serial.println("Cur speed : "+String(curSpeed));
   }

    stepper.runSpeed();
    return;
    #endif
    
    // If at the end of travel go to the other end
    
    if (active && stepper.distanceToGo() == 0)
    {
      Serial.println("Move to "+ String(-stepper.currentPosition()));
      stepper.moveTo(-stepper.currentPosition());
    }  

    stepper.run();
   
}
