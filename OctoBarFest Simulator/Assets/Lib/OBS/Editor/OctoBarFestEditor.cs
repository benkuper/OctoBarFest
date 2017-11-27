using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using System.IO;

[CustomEditor(typeof(OctoBarFest))]
public class OctoBarFestEditor : Editor {

    int numGlasses;

    public override void OnInspectorGUI()
    {
        base.DrawDefaultInspector();
        OctoBarFest obs = (OctoBarFest)target;

        string[] files = DataManager.getFileList("sessions");
        string[] sessions = new string[files.Length];
        int index = -1;
        for (int i = 0; i < files.Length; i++)
        {
            sessions[i] = Path.GetFileName(files[i]);
            if (sessions[i] == obs.currentSession) index = i;
        }

        int result = EditorGUILayout.Popup(index, sessions);
        if (result != -1 && sessions[result] != obs.currentSession) obs.setCurrentSession(sessions[result]);

        if (GUILayout.Button("Load session")) obs.setCurrentSession(obs.currentSession);
        EditorGUILayout.BeginHorizontal();
        if (GUILayout.Button("Full different")) obs.setupGlasses(obs.numPositions,OctoBarFest.FillMode.Different);
        if (GUILayout.Button("Full same")) obs.setupGlasses(obs.numPositions,OctoBarFest.FillMode.Same);
        if (GUILayout.Button("Full random")) obs.setupGlasses(obs.numPositions,OctoBarFest.FillMode.Random);
        EditorGUILayout.EndHorizontal();
        EditorGUILayout.BeginHorizontal();
        if (GUILayout.Button("Random different")) obs.setupGlasses(Random.Range(1,obs.numPositions),OctoBarFest.FillMode.Different);
        if (GUILayout.Button("Random same")) obs.setupGlasses(Random.Range(1, obs.numPositions), OctoBarFest.FillMode.Same);
        if (GUILayout.Button("Random random")) obs.setupGlasses(Random.Range(1, obs.numPositions), OctoBarFest.FillMode.Random);
        EditorGUILayout.EndHorizontal();

        numGlasses = EditorGUILayout.IntField("Num Glasses : ",numGlasses);
        EditorGUILayout.BeginHorizontal();
        if (GUILayout.Button("Random")) obs.setupGlasses(numGlasses, OctoBarFest.FillMode.Random);
        if (GUILayout.Button("Same")) obs.setupGlasses(numGlasses, OctoBarFest.FillMode.Same,false,false);
        if (GUILayout.Button("Different")) obs.setupGlasses(numGlasses, OctoBarFest.FillMode.Different,false);
        EditorGUILayout.EndHorizontal();
        if (GUILayout.Button("Generate Pouring Map")) obs.generatePouringPath();
        if (GUILayout.Button("Start Pouring")) obs.startPouring();
        
    }
}

[CustomPropertyDrawer(typeof(MixIngredient))]
public class MixIngredientDrawer : PropertyDrawer
{
    public override void OnGUI(Rect position, SerializedProperty property, GUIContent label)
    {
        //base.OnGUI(position, property, label);
        
        EditorGUI.indentLevel++;

        GUIStyle s = new GUIStyle();
        s.alignment = TextAnchor.MiddleCenter;
         
        string name = property.FindPropertyRelative("ingredientName").stringValue;
        int pos = property.FindPropertyRelative("platePosition").intValue;
        int id = property.FindPropertyRelative("id").intValue;
        EditorGUI.LabelField(position, "["+id+"] : "+name + " pouring on position " + pos,s);
    }
}

[CustomPropertyDrawer(typeof(DrinkMix))]
public class DrinkMixDrawer : PropertyDrawer
{
    public override void OnGUI(Rect position, SerializedProperty property, GUIContent label)
    {
        //base.OnGUI(position, property, label);
        EditorGUI.indentLevel++;

        GUIStyle s = new GUIStyle();
        s.alignment = TextAnchor.MiddleCenter;

        GUIStyle s2 = new GUIStyle();
        s2.alignment = TextAnchor.MiddleCenter;
        s2.normal.textColor = Color.grey;

        string name = property.FindPropertyRelative("mixName").stringValue;
        int id = property.FindPropertyRelative("id").intValue;

        EditorGUI.LabelField(new Rect(position.x,position.y,position.width,16),name+" on plug "+id, s);

        int numIngredients = property.FindPropertyRelative("ingredients").arraySize;
        for(int i=0;i<numIngredients;i++)
        {
            string iName= property.FindPropertyRelative("ingredients").GetArrayElementAtIndex(i).FindPropertyRelative("ingredientName").stringValue;
            float iQuantity = property.FindPropertyRelative("ingredients").GetArrayElementAtIndex(i).FindPropertyRelative("quantity").floatValue;

            int ty = 16+ i * 16;
            EditorGUI.LabelField(new Rect(position.x, position.y + ty, position.width, 16), iQuantity + " ml of " + iName, s2);
        }
    }

    public override float GetPropertyHeight(SerializedProperty property, GUIContent label)
    {
        int numIngredients = property.FindPropertyRelative("ingredients").arraySize;
        return 16 + numIngredients * 16;
    }
}