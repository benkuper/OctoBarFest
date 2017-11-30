class PouringStep
{
public:
  int targetPosition;
  
  GlassIngredient  * ingredients[MAX_INGREDIENTS];
  int numIngredients;
  
  PouringStep()
  {
    reset();
  }

  void reset()
  {
    numIngredients = 0;
  }

  void addIngredient(GlassIngredient * gi)
  {
    ingredients[numIngredients] = gi;
    numIngredients++;
  }
};

