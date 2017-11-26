using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

[CustomEditor(typeof(QuantifiedIngredient))]
public class QuantifiedIngredientEditor : Editor {

	// Use this for initialization
	public override void OnInspectorGUI()
    {
        base.DrawDefaultInspector();

        QuantifiedIngredient qi = (QuantifiedIngredient)target;
        OctoBarFest obs = qi.GetComponentInParent<OctoBarFest>();

        string[] options = new string[obs.ingredients.Length];
        int index = -1;
        for (int i = 0; i < obs.ingredients.Length; i++)
        {
            options[i] = obs.ingredients[i].ingredientName;
            //Debug.Log(qi.ingredient + " / " + obs.ingredients[i] + " / " + (qi.ingredient == obs.ingredients[i]));
            if (qi.ingredient == obs.ingredients[i])
            {
                index = i;
            }
        }

        int newIndex = EditorGUILayout.Popup(index, options);
        if (newIndex != index)
        {
            Debug.Log("Set :" + newIndex + " > " + obs.ingredients[newIndex]);
            qi.ingredient = obs.ingredients[newIndex];
            Debug.Log("check " + qi.ingredient.ingredientName);
        }
    }
}
