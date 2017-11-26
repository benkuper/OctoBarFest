using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

[CustomEditor(typeof(OctoBarFest))]
public class OctoBarFestEditor : Editor {

    public override void OnInspectorGUI()
    {
        base.DrawDefaultInspector();
        OctoBarFest obs = (OctoBarFest)target;

        if (GUILayout.Button("Start Pouring")) obs.startPouring();

    }
}
