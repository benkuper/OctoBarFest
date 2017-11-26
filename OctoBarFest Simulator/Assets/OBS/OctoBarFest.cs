using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class OctoBarFest : MonoBehaviour
{
    [Header("Setup")]
    [Tooltip("Number of positions available for glasses")]
    public int numPositions;
    [Tooltip("Number of available mixes")]
    public int numMixes;


    [Header("Pouring")]
    [Tooltip("Time to got from one place to another")]
    public float stepTransitionTime;
    [Range(0, 1)]
    [Tooltip("Anticipation of the jet before the glass is in position. 1 = first drop will arrive as the same time as the glass, 0 = first drop will launch when the glass is in position")]
    public float jetAnticipation;

    [Header("Drinks")]
    public MixIngredient[] ingredients;
    DrinkMix[] mixes;
    public List<PouringMix> pouringMixes;

    //private
    OBSPlate plate;
    Coroutine pourCoroutine;


    // Use this for initialization
    void OnEnable()
    {
        plate = GetComponentInChildren<OBSPlate>();
        plate.numPositions = numPositions;

        mixes = GetComponentsInChildren<DrinkMix>();

        Debug.Log("Got " + mixes.Length + " mixes");
    }



    // Update is called once per frame
    void Update()
    {
    }


    public void startPouring()
    {
        if (pourCoroutine != null) StopCoroutine(pourCoroutine);
        pourCoroutine = StartCoroutine(pourSequence());
    }

    IEnumerator pourSequence()
    {
        plate.goHome();
        yield return new WaitForSeconds(stepTransitionTime + .5f);
        plate.goToPosition(2, stepTransitionTime);
        yield return new WaitForSeconds(stepTransitionTime + .5f);
        plate.goToPosition(6, stepTransitionTime);
        yield return new WaitForSeconds(stepTransitionTime + .3f);
        plate.goToPosition(4, stepTransitionTime);
        yield return new WaitForSeconds(stepTransitionTime + 1f);
        plate.goHome();
    }

    public void endPouring()
    {

    }


    //Util
    public MixIngredient getMixIngredientWithName(string ingredientName)
    {
        foreach (MixIngredient mi in ingredients) if (mi.ingredientName == ingredientName) return mi;
        return null;
    }
}
