using System.Collections;
using System.Collections.Generic;
using UnityEngine;

#if UNITY_EDITOR
using UnityEditor;
#endif

public class OBSGlass : MonoBehaviour {

    public int id;

	// Use this for initialization
	void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {
		
	}

#if UNITY_EDITOR
    void OnDrawGizmos()
    {
        Handles.color = Color.HSVToRGB(id / 10f, 1, 1);
        Handles.Label(transform.position + Vector3.up * .5f, id.ToString());
    }
#endif
}
