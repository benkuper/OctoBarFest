struct GlassIngredient
{
  int glassPosition;
  QuantifiedIngredient * ingredient;
  bool isProcessed; //for generation
  bool isServed;    //for service
  int jetID;        //if multiple jets have same ingredient
};

class Glass
{
public:
  int position;
  
  int plugPin;
  int mixID;

  int numIngredients;
  GlassIngredient ingredients[MAX_INGREDIENTS_PER_MIX]; //for computing steps
  
  Glass()
  {
  }

  void reset()
  {
    numIngredients = 0;
  }

  void addIngredient(QuantifiedIngredient * ingredient)
  {
    ingredients[numIngredients].glassPosition = position;
    ingredients[numIngredients].ingredient = ingredient;
    ingredients[numIngredients].isProcessed = false;
    ingredients[numIngredients].isServed = false;
    numIngredients++;
  }
};

