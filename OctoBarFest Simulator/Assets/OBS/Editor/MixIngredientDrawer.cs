using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

[CustomPropertyDrawer(typeof(MixIngredient))]
public class MixIngredientDrawer : PropertyDrawer
{
    public override void OnGUI(Rect p, SerializedProperty property, GUIContent label)
    {
        EditorGUI.BeginProperty(p, label, property);
        Rect nr = new Rect(p.x, p.y, p.width - 230, p.height);

        EditorGUIUtility.labelWidth = 60;
        EditorGUI.PropertyField(nr, property.FindPropertyRelative("ingredientName"), new GUIContent("Name"));
        EditorGUIUtility.labelWidth = 100;

        EditorGUI.PropertyField(new Rect(p.width - 230, p.y, 130, p.height), property.FindPropertyRelative("platePosition"), new GUIContent("Plate Position"));
        EditorGUIUtility.labelWidth = 80;

        EditorGUI.PropertyField(new Rect(p.width - 100, p.y, 100, p.height), property.FindPropertyRelative("isAlcohol"), new GUIContent("Is Alcohol"));

        EditorGUI.EndProperty();
    }
}

