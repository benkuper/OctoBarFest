using System.Collections;
using System.Collections.Generic;
using UnityEngine;

#if UNITY_EDITOR
using UnityEditor;
#endif

public class OBSGlass : MonoBehaviour {

    public int pos;
    public int plug;

	// Use this for initialization
	void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {
		
	}

#if UNITY_EDITOR
    void OnDrawGizmos()
    {
        Handles.color = Color.HSVToRGB(pos / 10f, 1, 0);
        Handles.Label(transform.position + Vector3.up * 2f, pos + " > "+ plug);
    }
#endif
}
