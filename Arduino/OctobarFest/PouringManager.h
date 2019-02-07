#ifndef NUM_POSITIONS
#define NUM_POSITIONS 8
#endif

#define NUM_LIQUIDS 8
#define NUM_FILL_PER_GLASS 2

#define LIQUID_POURING_TIME 2000 //account for liquid going into the tube 500ms at each liquid pouring action (pump on/off)
#define LIQUID_POURING_SAFETIME 2500 //waiyinh 1000ms of serve time after motion before next step

#define USE_ANTICIPATION 1


class PouringManager
{
  public:

    const long motionTimings[NUM_POSITIONS] = { 0, 3300, 4000, 5800, 7500, 9400, 11000, 12400 };
    
    enum LiquidType { VIRGIN, BLUE, RED, GREEN, NUM_LIQUID_TYPES };

    struct Liquid
    {
      const char * name;
      LiquidType type;
      int pumpID;
      int position;
      long pumpToGlassTime; //time between the activation of pump and liquid touches the glass. 0 = instant

      String toString() {
        return String(name) + ", pumpID : " + String(pumpID) + ", Position " + String(position);
      };
    };

    struct Mix
    {
      String name;
      LiquidType liquidType;
    };

    struct Glass
    {
      int id; //1-8
      bool isPresent = false;
      Mix * currentMix;
      int fillState = 0;

      bool isFilled() {
        return fillState == NUM_FILL_PER_GLASS;
      };
    };

    struct PouringStep
    {
      int position;
      long startTime;
      long stepTime;
      Liquid * liquidsToServe[NUM_LIQUIDS];
      int liquidsTimings[NUM_LIQUIDS]; //time at which to start each liquid after startTime
      bool liquidsStates[NUM_LIQUIDS];
      int numLiquidsToServe;

      long getEndTime() {
        return startTime + stepTime;
      };
    };


    //Variables
    Liquid liquids[NUM_LIQUIDS]
    {
      {"Virgin", VIRGIN, 1, 6, 50},
      {"Virgin", VIRGIN, 2, 3, 50},
      {"Red",    RED,    3, 4, 60},
      {"Red",    RED,    4, 1, 100},
      {"Green",  GREEN,  5, 7, 50},
      {"Green",  GREEN,  6, 5, 200},
      {"Blue",   BLUE,   7, 8, 50},
      {"Blue",   BLUE,   8, 2, 50}
    };

    Mix mixes[NUM_LIQUID_TYPES]
    {
      {"Virgin", VIRGIN},
      {"Red",    RED},
      {"Green",  GREEN},
      {"Blue",   BLUE},
    };

    Glass glasses[NUM_POSITIONS];

    int pouringPositions[NUM_POSITIONS] = {1, 2, 3, 4, 5, 6, 7, 8} ;
    int filteredPouringPositions[NUM_POSITIONS] = {1, 2, 3, 4, 5, 6, 7, 8} ;

    PouringStep pouringSteps[NUM_POSITIONS + 1]; //at max 8 pouring + 1 finish
    int numPouringSteps = 0; //When filtered, will be used to know how much of the filtered positions we use
    int currentPouringStep = 0;

    //Pouring sequence timings
    long totalPouringTime;
    long timeAtStartPouring;

    bool isPouring = false;

    PouringManager()
    {
      addCallbackAskPositionEvent(&PouringManager::defaultAskPositionCallback);
      addCallbackAskPumpEvent(&PouringManager::defaultAskPumpCallback);
    }

    void init()
    {

#if DEBUG
      Serial.println("PouringManager init");
#endif

      for (int i = 0; i < NUM_POSITIONS; i++)
      {
        glasses[i].id = i + 1; //1-8
        glasses[i].isPresent = false;
        glasses[i].fillState = 0;
      }
    }


    void update()
    {
      if (isPouring)
      {
        long timeSinceStart = millis() - timeAtStartPouring;

        if (timeSinceStart >= totalPouringTime)
        {
#if DEBUG_POURING
          Serial.println("POURING FINISH at time " + String(timeSinceStart));
#endif

          finish();
          return;
        }

        int curStep = 0;
        while (curStep <= NUM_POSITIONS && pouringSteps[curStep].startTime < timeSinceStart)
        {
          curStep++;
        }

        int targetStep = std::min(std::max(curStep - 1, 0), numPouringSteps - 1);

        if (currentPouringStep != targetStep)
        {
          if (currentPouringStep >= 0)
          {
#if DEBUG_POURING
            //Serial.println("End of step " + String(currentPouringStep));
#endif

            //deactivate pumps for this, safety
            for (int i = 0; i < pouringSteps[currentPouringStep].numLiquidsToServe; i++)
            {
              int pumpToStop = pouringSteps[currentPouringStep].liquidsToServe[i]->pumpID;


#if DEBUG_POURING
              //Serial.println("> Stop pump : " + String(pumpToStop));
#endif
              onAskPumpEvent(pumpToStop, false);
            }
          }

          currentPouringStep = targetStep;
#if DEBUG_POURING
          //Serial.println("New step : " + String(targetStep) + ", move to position : " +  String(pouringSteps[currentPouringStep].position));
#endif
          onAskPositionEvent(pouringSteps[currentPouringStep].position);
        }

        PouringStep * step = &pouringSteps[currentPouringStep];
        int numStepLiquids = step->numLiquidsToServe;


        long timeSinceStepStart = timeSinceStart - step->startTime;
        for (int i = 0; i < numStepLiquids; i++)
        {
          boolean liquidVal = timeSinceStepStart > step->liquidsTimings[i] && timeSinceStepStart < step->liquidsTimings[i] + LIQUID_POURING_TIME;
          if (step->liquidsStates[i] != liquidVal)
          {
            step->liquidsStates[i] = liquidVal;

#if DEBUG_POURING
            //Serial.println("Set Pump " + String(step->liquidsToServe[i]->pumpID) + " to " + String(liquidVal));
#endif
            onAskPumpEvent(step->liquidsToServe[i]->pumpID, liquidVal);
          }
        }
      }
    }


    //POURING AND SERVICE
    void assignRandomMixesToGlasses()
    {
      for (int i = 0; i < NUM_POSITIONS; i++)
      {
        glasses[i].currentMix = &mixes[random(NUM_LIQUID_TYPES)];

#if DEBUG_POURING
        Serial.println("Glass " + String(glasses[i].id) + " assigned mix " + glasses[i].currentMix->name);
#endif
      }
    }

    void reset()
    {
      isPouring = false;
      for (int i = 0; i < NUM_POSITIONS; i++) glasses[i].fillState = 0;
    }

    void start() //Call at start of pouring
    {
      reset();
      generateSteps();
    }

    void generateSteps()
    {
      //Phase 1 : randomize positions
      shuffleArray(pouringPositions, NUM_POSITIONS);

#if DEBUG_POURING
      String pString = "Pouring path (unfiltered) : ";
      for (int i = 0; i < NUM_POSITIONS; i++) pString += pouringPositions[i] + String(" ");
      Serial.println(pString);
#endif



      //Phase 2 : filter positions and generate steps
      int currentStep = 0;

      for (int i = 0; i < NUM_POSITIONS; i++)
      {
        //check if all glasses would be filled
        if (areAllGlassesFilled())  break;


        int pouringPos = pouringPositions[i];

        //generate timing for  this step
        int previousPos = currentStep == 0 ? 1 : pouringSteps[currentStep - 1].position;
        long stepMotionTime = getTimingForPosToPos(pouringPos, previousPos);
       
        pouringSteps[currentStep].startTime = currentStep == 0 ? 0 : pouringSteps[currentStep - 1].getEndTime();
        pouringSteps[currentStep].stepTime = stepMotionTime + LIQUID_POURING_SAFETIME;

        pouringSteps[currentStep].numLiquidsToServe = 0; //reset step
        pouringSteps[currentStep].position = pouringPos;

        for (int j = 0; j < NUM_POSITIONS; j++)
        {
          if (!glasses[j].isPresent) continue;

          int targetLiquidPos = pouringPos + glasses[j].id;
          if (targetLiquidPos > 8) targetLiquidPos -= 8;

          Liquid * liquidAtPos = getLiquidAtPosition(targetLiquidPos); //position 1 to 8
          if (liquidAtPos == NULL)
          {
            Serial.println("ERROR PUMP NOT FOUND FOR POSITION : " + targetLiquidPos);
            return;
          }

          if (glasses[j].currentMix->liquidType == liquidAtPos->type) //if liquid match the mix's liquid type
          {
            int curLiquidIndex = pouringSteps[currentStep].numLiquidsToServe;
            pouringSteps[currentStep].liquidsToServe[curLiquidIndex] = liquidAtPos;

#if USE_ANTICIPATION
            pouringSteps[currentStep].liquidsTimings[curLiquidIndex] = std::max(stepMotionTime - liquidAtPos->pumpToGlassTime, (long)0);
#else
            pouringSteps[currentStep].liquidsTimings[curLiquidIndex] = stepMotionTime;
#endif
            glasses[j].fillState++;
            pouringSteps[currentStep].numLiquidsToServe++;
          }
        }

        if (pouringSteps[currentStep].numLiquidsToServe > 0) currentStep++; //This is a valid step, increment currentStep;
      }

      if (areAllGlassesFilled())
      {
        //add finish step
        pouringSteps[currentStep].position = 1;
        pouringSteps[currentStep].numLiquidsToServe = 0;
        pouringSteps[currentStep].startTime = pouringSteps[currentStep - 1].getEndTime();
        pouringSteps[currentStep].stepTime = getTimingForPosToPos(1, pouringSteps[currentStep - 1].position);

        totalPouringTime = pouringSteps[currentStep].getEndTime();

        numPouringSteps = currentStep + 1;

#if DEBUG_POURING
        Serial.println("Generated " + String(numPouringSteps) + " steps (including finish)");
        for (int i = 0; i < numPouringSteps; i++)
        {
          Serial.println("\t> Time " + String(pouringSteps[i].startTime) + ", Position " + String(pouringSteps[i].position) + ", " + String(pouringSteps[i].numLiquidsToServe + " liquids"));

          for (int j = 0; j < pouringSteps[i].numLiquidsToServe; j++)
          {
            int glassPos = pouringSteps[i].liquidsToServe[j]->position - pouringSteps[i].position;
            if (glassPos <= 0) glassPos += 8;
            Serial.println("\t\t >> " + pouringSteps[i].liquidsToServe[j]->toString() + ", to Glass " + glassPos);
          }
        }

        startPouring();

#endif
      } else
      {
#if DEBUG_POURING
        Serial.println("PROBLEM, NOT ALL GLASSES ARE FILLED !");
#endif
      }
    }

    void startPouring()
    {
      timeAtStartPouring = millis();
      currentPouringStep = -1; // force change
      isPouring = true;
    }

    void finish() //Call at end of pouring
    {
      isPouring = false;
    }

    //Helpers
    bool areAllGlassesFilled()
    {
      for (int i = 0; i < NUM_POSITIONS; i++)
      {
        if (glasses[i].isPresent && !glasses[i].isFilled()) return false;
      }
      return true;
    }

    Liquid * getLiquidAtPosition(int position)
    {
      for (int i = 0; i < NUM_LIQUIDS; i++)
      {
        if (liquids[i].position == position) return &liquids[i];
      }

      return NULL;
    }

    long getTimingForPosToPos(int startPos, int endPos)
    {
      return motionTimings[abs(endPos - startPos)];
      //return 2000 + abs(endPos - startPos) * 600; // for now, 1000ms between each position
    }

    void shuffleArray(int * array, int size)
    {
      int last = 0;
      int temp = array[last];
      for (int i = 0; i < size; i++)
      {
        int index = random(size);
        array[last] = array[index];
        last = index;
      }
      array[last] = temp;
    }

    // EVENTS
    typedef void(*onPosEvent)(int);
    typedef void(*onPumpEvent)(int, bool);
    void (*onAskPositionEvent) (int);
    void (*onAskPumpEvent) (int, bool);

    void addCallbackAskPositionEvent(onPosEvent func) {
      onAskPositionEvent = func;
    }
    void addCallbackAskPumpEvent(onPumpEvent func) {
      onAskPumpEvent = func;
    }

    static void defaultAskPositionCallback(int) { }
    static void defaultAskPumpCallback(int, bool) { }
};
