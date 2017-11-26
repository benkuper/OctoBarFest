using System;
using UnityEngine;
using System.Collections.Generic;

[Serializable]
public class PouringMix
{
    public DrinkMix mix;                                        //Reference mix
    public Dictionary<MixIngredient, bool> ingredientStates;    //State of each ingredient (has it been already poured ?)

    public PouringMix()
    {
        ingredientStates = new Dictionary<MixIngredient, bool>();
    }

    public List<MixIngredient> getIngredientsNotPouredYet()
    {
        List<MixIngredient> result = new List<MixIngredient>();
        foreach (QuantifiedIngredient qi in mix.ingredients)
        {
            if (!ingredientStates[qi.ingredient]) result.Add(qi.ingredient);
        }

        return result;
    }

    public int getQuantityForIngredient(MixIngredient mi)
    {
        foreach (QuantifiedIngredient qi in mix.ingredients)
        {
            if (qi.ingredient == mi) return qi.quantity;
        }

        Debug.LogWarning("Ingredient " + mi.ingredientName + " not found in mix " + mix.mixName);
        return 0;
    }

    public bool isFinished()
    {
        return getIngredientsNotPouredYet().Count == 0;
    }
}