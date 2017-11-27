using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class OctoBarFest : MonoBehaviour
{
    public enum FillMode { Same, Different, Random };

    [Header("Setup")]
    [Tooltip("Number of positions available for glasses")]
    public int numPositions;

    [Header("Pouring")]
    [Tooltip("Time to got from one place to another")]
    public float stepTransitionTime;
    public float pouringTimeFactor;
    public float minServingTime;

    [Range(0, 1)]
    [Tooltip("Anticipation of the jet before the glass is in position. 1 = first drop will arrive as the same time as the glass, 0 = first drop will launch when the glass is in position")]
    public float jetAnticipation;

    [Header("Session")]
    public string currentSession;
    public List<MixIngredient> ingredients;
    public DrinkMix[] mixes;

    //public List<PouringDrink> pouringMixes;

    //private
    OBSPlate plate;
    Coroutine pourCoroutine;


    // Use this for initialization
    void Start()
    {
        plate = GetComponentInChildren<OBSPlate>();
    }


    // Update is called once per frame
    void Update()
    {
    }

    public void clearSession()
    {
        //
    }

    public void setCurrentSession(string file)
    {
        clearSession();
        currentSession = file;
        string data = DataManager.getFileData("sessions/" + currentSession);
        JSONObject o = new JSONObject(data);

        //Load ingredients
        ingredients = new List<MixIngredient>();
        for (int i = 0; i < o["ingredients"].Count; i++)
        {
            MixIngredient mi = new MixIngredient();
            mi.id = i;

            JSONObject io = o["ingredients"][i];
            io.GetField(ref mi.ingredientName, "ingredientName");
            io.GetField(ref mi.platePosition, "platePosition");

            ingredients.Add(mi);
        }

        mixes = new DrinkMix[o["mixes"].Count];

        for (int i = 0; i < mixes.Length; i++)
        {
            DrinkMix m = new DrinkMix();
            m.id = i;

            JSONObject mo = o["mixes"][i];
            mo.GetField(ref m.mixName, "mixName");

            JSONObject moi = mo["ingredients"];
            m.ingredients = new QuantifiedIngredient[moi.Count];
            for (int j = 0; j < moi.Count; j++)
            {
                MixIngredient mi = getMixIngredientWithName(moi.keys[j]);
                m.ingredients[j] = new QuantifiedIngredient(mi.ingredientName, moi[j].f);
            }

            mixes[i] = m;
        }
    }


    public void setupGlasses(int numGlasses, FillMode fillMode, bool randomizePositions = true, bool randomizeSameGlasses = true)
    {
        if(plate == null) plate = GetComponentInChildren<OBSPlate>();
        plate.clear();

        int[] positions = new int[numPositions];
        for (int i = 0; i < positions.Length; i++) positions[i] = i;
        if(randomizePositions) Shuffle(positions);

        int rPlug = randomizeSameGlasses ? UnityEngine.Random.Range(0, mixes.Length) : 0;

        for (int i = 0; i < numGlasses;i++)
        {
            int plug = -1;
            switch (fillMode)
            {
                case FillMode.Same:
                    plug = rPlug;
                    break;

                case FillMode.Different:
                    plug = i % mixes.Length;
                    break;

                case FillMode.Random:
                    plug = UnityEngine.Random.Range(0, mixes.Length);
                    break;
            }

            plate.addGlass(positions[i], plug);
        }
    }

    public void startPouring()
    {
        PouringPath p = generatePouringPath();

        p.Shuffle();
        //

        if (pourCoroutine != null) StopCoroutine(pourCoroutine);
        pourCoroutine = StartCoroutine(pourSequence(p));
    }

    public PouringPath generatePouringPath()
    {
        //Generate here
        List<PouringDrink> pdList = new List<PouringDrink>();
        foreach (OBSGlass g in plate.glasses)
        {
            PouringDrink pd = new PouringDrink();
            pd.glassPosition = g.pos;
            DrinkMix mix = mixes[g.plug];

            //Debug.Log("Glass at pos " + g.pos + " will get mix " + mix.mixName);
            foreach (QuantifiedIngredient qi in mix.ingredients)
            {
                //Debug.Log(" > Ingredient " + qi.ingredientName);
                pd.ingredientStates.Add(qi.Clone(), false);
            }

            pdList.Add(pd);
        }

        PouringPath p = getPathForDrinks(pdList);

        return p;
    }

    IEnumerator pourSequence(PouringPath path)
    {
        Debug.Log("Pour sequence "+path);

        //plate.goHome();
        //yield return new WaitForSeconds(stepTransitionTime + .5f);

        foreach (PouringStep s in path.steps)
        {
            plate.goToPosition(s.targetPosition, stepTransitionTime);
            yield return new WaitForSeconds(stepTransitionTime);

            foreach (QuantifiedIngredient qi in s.ingredients)
            {
                int targetGlass = (getMixIngredientWithName(qi.ingredientName).platePosition + s.targetPosition) % numPositions;
                Debug.Log("Serving " + qi.quantity + " ml of " + qi.ingredientName + " to position " + targetGlass);
            }

            float servingTime = s.getMaxQuantityPoured() * pouringTimeFactor / 100.0f;
            yield return new WaitForSeconds(Mathf.Max(servingTime,minServingTime));
        }

        Debug.Log("Sequence finished !");
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

    void Shuffle(int[] a)
    {
        // Loops through array
        for (int i = a.Length - 1; i > 0; i--)
        {
            // Randomize a number between 0 and i (so that the range decreases each time)
            int rnd = UnityEngine.Random.Range(0, i);

            // Save the value of the current i, otherwise it'll overright when we swap the values
            int temp = a[i];

            // Swap the new and old values
            a[i] = a[rnd];
            a[rnd] = temp;
        }
        
    }


    public PouringPath getPathForDrinks(List<PouringDrink> drinks)
    {
        PouringPath result = new PouringPath(numPositions);

        
        for(int i=0;i<numPositions;i++)
        {
            List<QuantifiedIngredient> validIngredients = new List<QuantifiedIngredient>();
            foreach (PouringDrink d in drinks)
            {
                int absoluteGlassPos = (i + d.glassPosition) % numPositions;

                List<QuantifiedIngredient> iList = getMixIngredientWithPositionForDrink(absoluteGlassPos, d); 

                foreach (QuantifiedIngredient qi in iList)
                {
                    d.ingredientStates[qi] = true;
                    validIngredients.Add(qi);
                }
            }

            if(validIngredients.Count > 0)
            {
                PouringStep s = new PouringStep();
                s.targetPosition = i;
                s.ingredients = validIngredients;
                result.steps.Add(s);
            }
        }
        
        return result;
    }


    private List<QuantifiedIngredient> getMixIngredientWithPositionForDrink(int absolutePosition, PouringDrink d)
    {
        return getMixIngredientWithPositionForDrink(absolutePosition, d, ingredients);
    }

    private List<QuantifiedIngredient> getMixIngredientWithPositionForDrink(int absolutePosition, PouringDrink d, List<MixIngredient> iList)
    {
        List<QuantifiedIngredient> result = new List<QuantifiedIngredient>();
        foreach (MixIngredient mi in ingredients)
        {
            if (mi.platePosition == absolutePosition)
            {
                foreach (KeyValuePair<QuantifiedIngredient,bool> qi in d.ingredientStates)
                {
                    if (qi.Key.ingredientName == mi.ingredientName)
                    {
                        result.Add(qi.Key);
                        break;
                    }
                }
            }
        }

        return result;
    }

    public List<PouringDrink> getDrinksClones(List<PouringDrink> drinks)
    {
        List<PouringDrink> result = new List<PouringDrink>();
        foreach (PouringDrink pd in drinks) result.Add(pd.Clone());
        return result;
    }

    //Compute all maps
    /*
    public List<PouringMap> getAllPouringMapsDrinkState(PouringMap sourceMap, List<PouringDrink> drinks, int currentPosition)
    {
        List<PouringMap> result = new List<PouringMap>();
        

        return result;
    }
    */
}
