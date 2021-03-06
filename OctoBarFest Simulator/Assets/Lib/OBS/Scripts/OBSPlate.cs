﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;

#if UNITY_EDITOR
using UnityEditor;
#endif

using DG.Tweening;

public class OBSPlate : MonoBehaviour {

    [Header("Setup")]
    public int numGlasses;
    public float radius;
    public GameObject glassPrefab;

    public List<OBSGlass> glasses;

    // Use this for initialization
    void Start()
    {
        clear();
    }
	
	// Update is called once per frame
	void Update () {
		
	}

    public void clear()
    {
        if (glasses != null)
        {
            foreach (OBSGlass g in glasses)
            {
                if (g != null) DestroyImmediate(g.gameObject);
            }
        }

        OBSGlass[] tmpGlasses = GetComponentsInChildren<OBSGlass>();
        foreach (OBSGlass g in tmpGlasses) DestroyImmediate(g.gameObject);
        glasses = new List<OBSGlass>();
    }

    public void addGlass(int pos, int plug)
    {
        OBSGlass g = Instantiate(glassPrefab).GetComponent<OBSGlass>();
        glasses.Add(g);


        g.pos = pos;
        g.plug = plug;


        g.transform.SetParent(transform);
        float angle = pos * 45 * Mathf.PI / 180.0f;  
        g.transform.localPosition = new Vector3(Mathf.Cos(angle) * radius, 0, -Mathf.Sin(angle) * radius);
    }

    public void goHome()
    {
        goToPosition(0, 1f);
    }

    public void goToPosition(int position, float time)
    {
        float step = 360.0f / GetComponentInParent<OctoBarFest>().numPositions;
        float targetRotY = position * step;

        transform.DORotate(new Vector3(0, targetRotY, 0), time).SetEase(Ease.InOutSine);
    }

#if UNITY_EDITOR
    void OnDrawGizmos()
    {
        Handles.color = new Color(.3f, .5f, 1f, .2f);
        Handles.DrawSolidArc(transform.position, Vector3.up, Vector3.right, transform.rotation.eulerAngles.y, radius / 2);
    }
#endif
}
