#define DEBUG 1

#define MAX_INGREDIENTS 16
#define MAX_MIXES 16
#define MAX_INGREDIENTS_PER_MIX 5

#define NUM_POSITIONS 8

#include <ArduinoJson.h>
#include "QuantifiedIngredient.h"
#include "LiquidJet.h"
#include "Glass.h"
#include "Plate.h"
#include "Mix.h"
#include "PouringStep.h"
#include "WebServer.h"

//Global objects
Plate plate;
LiquidJet jets[MAX_INGREDIENTS];
Glass glasses[NUM_POSITIONS];
Mix mixes[MAX_MIXES];

//Session veriable
int numPositions;
int numJets;
int numMixes;

//Active glass setup
int numGlasses;

//Service data
int numSteps; //Number of real steps for this service
PouringStep steps[NUM_POSITIONS]; //maximum is as many as positions


//Service vars
bool isServing;
int currentStepID;
long nextStepTime;
long timeAtServeStart;

//Pouring setup
float jetAnticipation;
const float pouringQuantityFactor = 1; //ms factor for 1 ml

void setup() {

  //initial values
  jetAnticipation = 0; //no ancitipation

  //init with sanity vars before loading session
  numJets  = 0;
  numPositions = 0;
  numGlasses = 0;
  numMixes = 0;

  loadSession();
}

void loop()
{
  plate.update();
  if (isServing) updateServing();
}


void updateServing()
{
  for (int i = 0; i < numJets; i++)
  {
    jets[i].update();
  }

  if (millis() > nextStepTime)
  {
    nextStepTime = serveStep(currentStepID);
  }
}

long serveStep(int stepID)
{
  if (stepID >= numSteps)
  {
    float totalTime = (millis() - timeAtServeStart) / 1000.0f;
    Serial.print("Sequence finished in ");
    Serial.print(totalTime);
    Serial.println(" seconds !");
    isServing = false;
    plate.goToPosition(0);
    return 0;
  }

  PouringStep s = steps[currentStepID];

  float posDist = plate.getMinDistanceBetween(s.targetPosition, plate.currentPosition);
  long targetTransitionTime = plate.goToPosition(s.targetPosition);

  float maxTime = 0;

  for (int i = 0; i < s.numIngredients; i++)
  {
    GlassIngredient * gi = s.ingredients[i];
    QuantifiedIngredient * qi = gi->ingredient;
    LiquidJet j = jets[gi->jetID];

    long t = qi->quantity * pouringQuantityFactor; //serving time in ms

    long anticipation = min(jetAnticipation * j.jetTimeToGlass, targetTransitionTime);
    long inverseAnticipation = targetTransitionTime - anticipation;

    j.launch(inverseAnticipation, t); //launch with anticipation delay

    long overallPourTime = t + j .jetTimeToGlass - anticipation;
    if (overallPourTime > maxTime) maxTime = overallPourTime;
  }

  currentStepID++;

  return millis() + targetTransitionTime + maxTime;
}


// ---------------    SESSION   ------------------ //

void loadSession()
{
  //Load data from eeprom
  const char* json = "{\"i\":[{\"t\":0,\"p\":0},{\"t\":1,\"p\":1},{\"t\":2,\"p\":2},{\"t\":3,\"p\":3},{\"t\":4,\"p\":4},{\"t\":5,\"p\":5},{\"t\":6,\"p\":4},{\"t\":7,\"p\":6},{\"t\":8,\"p\":7},{\"t\":9,\"p\":3}],\"mixes\":[{\"3\":150,\"7\":15},{\"5\":30,\"7\":30,\"8\":30,\"9\":30},{\"5\":7.5,\"8\":15,\"9\":30},{\"4\":15,\"6\":1,\"7\":60},{\"4\":15,\"7\":22.5,\"8\":22.5,\"9\":22.5},{\"4\":30,\"8\":30,\"9\":30},{\"5\":2.5,\"8\":30,\"9\":30},{\"1\":60,\"8\":15},{\"0\":15,\"1\":15,\"2\":15,\"5\":15,\"6\":1,\"7\":15},{\"5\":15,\"8\":15,\"9\":60},{\"4\":120,\"5\":1,\"7\":60},{\"0\":44,\"3\":44,\"7\":88},{\"1\":30,\"9\":15},{\"0\":30,\"4\":30,\"8\":30},{\"0\":30,\"3\":30,\"8\":30},{\"5\":0.35,\"6\":100}]}";
  parseData(json);
}

void parseData(const char * data)
{

  const size_t bufferSize = JSON_ARRAY_SIZE(10) + JSON_ARRAY_SIZE(16) + 15 * JSON_OBJECT_SIZE(2) + 9 * JSON_OBJECT_SIZE(3) + 2 * JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(6) + 390;
  DynamicJsonBuffer jsonBuffer(bufferSize);
  JsonObject& root = jsonBuffer.parseObject(data);

  //Create objects here

  //numPositions = ..
  //numMixes = ..
  //numIngredients = ..

  /*
    JsonArray& i = root["i"];

    int i0_t = i[0]["t"]; // 0
    int i0_p = i[0]["p"]; // 0

    int i1_t = i[1]["t"]; // 1
    int i1_p = i[1]["p"]; // 1

    int i2_t = i[2]["t"]; // 2
    int i2_p = i[2]["p"]; // 2

    int i3_t = i[3]["t"]; // 3
    int i3_p = i[3]["p"]; // 3

    JsonArray& mixes = root["mixes"];

    int mixes0_3 = mixes[0]["3"]; // 150
    int mixes0_7 = mixes[0]["7"]; // 15

    JsonObject& mixes1 = mixes[1];
    int mixes1_5 = mixes1["5"]; // 30
    int mixes1_7 = mixes1["7"]; // 30
    int mixes1_8 = mixes1["8"]; // 30
    int mixes1_9 = mixes1["9"]; // 30

    JsonObject& mixes2 = mixes[2];
    float mixes2_5 = mixes2["5"]; // 7.5
    int mixes2_8 = mixes2["8"]; // 15
    int mixes2_9 = mixes2["9"]; // 30

    JsonObject& mixes3 = mixes[3];
    int mixes3_4 = mixes3["4"]; // 15
    int mixes3_6 = mixes3["6"]; // 1
    int mixes3_7 = mixes3["7"]; // 60
  */
}

// ---------------    SERVICE   ------------------ //

void startService()
{
  setupGlasses();
  generateSteps();

#if DEBUG
  Serial.println("------ POURING SETUP ---------");
  for (int i = 0; i < numSteps; i++)
  {
    Serial.print("* Step : Position ");
    Serial.println(steps[i].targetPosition);
    for (int j = 0; j < steps[i].numIngredients; j++)
    {
      GlassIngredient * gi = steps[i].ingredients[j];
      Serial.print("\tJet #");
      Serial.print(gi->jetID);
      Serial.print(" on pump pin ");
      Serial.print(jets[gi->jetID].pumpPin);
      Serial.print(" serving glass #");
      Serial.print(gi->glassPosition);
      Serial.print(" ");
      Serial.print(gi->ingredient->quantity);
      Serial.print(" ml of ingredient #");
      Serial.print(gi->ingredient->ingredientID);
      Serial.println();
    }
  }
#endif

  startPouring();
}

void setupGlasses()
{
  for (int i = 0; i < numGlasses; i++)
  {
    Mix m = mixes[glasses[i].mixID];
    glasses[i].reset();

    for (int j = 0; j < m.numIngredients; j++)
    {
      glasses[j].addIngredient(&m.ingredients[j]);
    }
  }
}

void generateSteps()
{
  numSteps = 0;
  for (int i = numPositions - 1; i >= 0; i--)
  {
    int numValidIngredients = 0;
    GlassIngredient * validIngredients[MAX_INGREDIENTS];
    for (int j = 0; j < numGlasses; j++)
    {
      Glass g = glasses[j];
      int absoluteGlassPos = (i + g.position) % numPositions;

      for (int k = 0; k < g.numIngredients; k++)
      {
        GlassIngredient gi = g.ingredients[k];
        if (gi.isProcessed) continue;

        for (int l = 0; l < numJets; l++)
        {
          LiquidJet jet = jets[l];
          if (jet.targetPosition == absoluteGlassPos && jet.ingredientID == gi.ingredient->ingredientID)
          {
            validIngredients[numValidIngredients] = &gi;
            gi.jetID = l;
            gi.isProcessed = true;
            numValidIngredients++;
          }
        }
      }
    }

    if (numValidIngredients > 0)
    {
      steps[numSteps].reset();
      steps[numSteps].targetPosition = i;
      for (int j = 0; j < numValidIngredients; j++) steps[numSteps].addIngredient(validIngredients[i]);
      numSteps++;
    }
  }
}

void startPouring()
{

  isServing = true;
  currentStepID = 0;
  timeAtServeStart  = 0;
  nextStepTime = millis();
}


