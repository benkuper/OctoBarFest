using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LiquidJet : MonoBehaviour {

    public string ingredientName;
    public float jetTimeToGlass;
    ParticleSystem p;

	// Use this for initialization
	void Start () {
        p = GetComponent<ParticleSystem>();

    }
	
	// Update is called once per frame
	void Update () {
		
	}

    public void launch(float delay, float time)
    {
        StartCoroutine(play(delay,time));
    }

    IEnumerator play(float delay, float time)
    {
        yield return new WaitForSeconds(delay);
        //set time
        ParticleSystem.Burst b = p.emission.GetBurst(0);
        b.cycleCount = Mathf.Max((int)(time / p.emission.GetBurst(0).repeatInterval), 2);
        p.emission.SetBurst(0, b);
        p.Play();
    }
}
