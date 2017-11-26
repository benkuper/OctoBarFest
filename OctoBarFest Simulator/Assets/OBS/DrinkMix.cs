using System;
using UnityEngine;
using System.Collections.Generic;

public class DrinkMix : MonoBehaviour
{
    public string mixName;
    public int plugPosition;                                //Plug position on the drink patch bay
    [HideInInspector]
    public QuantifiedIngredient[] ingredients; 

    private void Start()
    {
        ingredients = GetComponents<QuantifiedIngredient>();
    }
}
