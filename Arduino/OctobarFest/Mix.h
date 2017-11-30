class Mix
{
public:
  int mixPin;
  
  QuantifiedIngredient ingredients[MAX_INGREDIENTS_PER_MIX];
  int numIngredients;
  
  Mix()
  { 
  }

  void reset()
  {
    numIngredients = 0;
  }

  void addIngredient(int ingredientID, float quantity)
  {
    ingredients[numIngredients].ingredientID = ingredientID;
    ingredients[numIngredients].quantity = quantity;
    numIngredients++;
  }
};

