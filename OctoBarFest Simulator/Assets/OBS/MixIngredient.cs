using System;
using UnityEngine;
using System.Collections.Generic;

[Serializable]
public class MixIngredient
{
    public string ingredientName;         //Name of the ingredient
    public int platePosition;        //Pouring position on the plate, 0->7 (0 = position of first glass, 7=position of last glass, clockwise)
    public bool isAlcohol;    //Contains alcohol ?
}