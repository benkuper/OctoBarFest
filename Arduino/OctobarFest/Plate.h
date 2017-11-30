class Plate
{
public:

  const long timePerStep = 1000;//in ms
  float speedFactor = 1;
  
  int currentPosition;
  Plate()
  {
    
  }


  void update()
  {
    
  }
  
  int getMinDistanceBetween(int pos1, int pos2)
  {
    int dist = abs(pos1-pos2);
    if(dist > floor(NUM_POSITIONS/2.0f)) dist = NUM_POSITIONS-dist;
  }
  
  long goToPosition(int pos)
  {
    int dist = getMinDistanceBetween(pos,currentPosition);
    currentPosition = pos;
    return dist*timePerStep*speedFactor;
  }
};
