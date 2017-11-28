using System;
using UnityEngine;
using System.Collections.Generic;


[Serializable]
public class MixIngredient
{
    public int id;                  //Used for pin connection (which bottle is connected to which pin)
    public string ingredientName;         //Name of the ingredient
    public int platePosition;        //Pouring position on the plate, 0->7 (0 = position of first glass, 7=position of last glass, clockwise)
}

[Serializable]
public class QuantifiedIngredient
{
    public string ingredientName;
    public float quantity;

    public QuantifiedIngredient(string ingredientName, float quantity)
    {
        this.ingredientName = ingredientName;
        this.quantity = quantity;
    }

    internal QuantifiedIngredient Clone()
    {
        return new QuantifiedIngredient(ingredientName, quantity);
    }
}

[Serializable]
public class DrinkMix
{
    public int id;                                          //used for plug connection (which mix is connected to which plug)
    public string mixName;
    public QuantifiedIngredient[] ingredients; 
}

[Serializable]
public class PouringDrink
{
    public int glassPosition;
    public Dictionary<QuantifiedIngredient, bool> ingredientStates;    //State of each ingredient (has it been already poured ?)

    public PouringDrink()
    {
        ingredientStates = new Dictionary<QuantifiedIngredient, bool>();
    }

    public List<string> getIngredientsNotPouredYet()
    {
        List<string> result = new List<string>();
        foreach (KeyValuePair<QuantifiedIngredient, bool> iState in ingredientStates)
        {
            if (!iState.Value) result.Add(iState.Key.ingredientName);
        }

        return result;
    }

    public float getQuantityForIngredient(string ingredientName, bool return0IfAlreadyPoured = true)
    {
        foreach (KeyValuePair<QuantifiedIngredient,bool> qi in ingredientStates)
        {
            if (qi.Key.ingredientName == ingredientName)
            {
                if (qi.Value && return0IfAlreadyPoured) return 0;
                return qi.Key.quantity;
            }
        }

        Debug.LogWarning("Ingredient " + ingredientName + " not found");
        return 0;
    }

    public bool isFinished()
    {
        return getIngredientsNotPouredYet().Count == 0;
    }

    public PouringDrink Clone()
    {
        PouringDrink pd = new PouringDrink();
        pd.glassPosition = glassPosition;

        foreach (KeyValuePair<QuantifiedIngredient, bool> iState in ingredientStates) pd.ingredientStates.Add(iState.Key, iState.Value);
        return pd;
    }
}


