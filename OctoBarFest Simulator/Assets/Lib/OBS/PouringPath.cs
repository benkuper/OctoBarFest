using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Linq;

public class PouringStep
{

    public int targetPosition;
    public List<QuantifiedIngredient> ingredients;

    public float getMaxQuantityPoured()
    {
        float result = 0;
        foreach (QuantifiedIngredient qi in ingredients) result += Mathf.Max(qi.quantity);
        return result;
    }
}
    
public class PouringPath
{
    public List<PouringStep> steps;
    public int numPositions;

    public PouringPath(int numPositions)
    {
        this.numPositions = numPositions;
        steps = new List<PouringStep>();
    }



    public int getTotalMotion()
    {
        int motionLength = 0;
        for(int i=0;i<steps.Count;i++)
        {
            int lastPosition = i > 0 ? steps[i - 1].targetPosition : 0;
            motionLength += getMinDistanceBetween(lastPosition, steps[i].targetPosition, numPositions);
        }

        return motionLength;
    }

    public int getMinDistanceBetween(int pos1, int pos2, int numPositions)
    {
        int dist = Mathf.Abs(pos1 - pos2);
        if (dist > numPositions / 2.0f) dist = numPositions - dist;
        return dist;
    }

    public float getMaxCoalescedPouringQuantity()
    {
        float result = 0;
        foreach (PouringStep p in steps) result += p.getMaxQuantityPoured();
        return result;
    }

    public int getMaxSimultaneousPouring(out int coutOfMaxPouring)
    {
        int maxPouring = 0;
        coutOfMaxPouring = 0;
        foreach (PouringStep p in steps)
        {
            int numP = p.ingredients.Count;
            if (numP > maxPouring)
            {
                maxPouring = numP;
                coutOfMaxPouring = 1;
            }
            else if (numP == maxPouring)
            {
                coutOfMaxPouring++;
            }
        }

        return maxPouring;
    }

    public float getPouringVariance()
    {

        float average = 0;
        foreach (PouringStep p in steps) average += p.ingredients.Count;
        average /= steps.Count;

        float averageDistToAverage = 0;
        foreach (PouringStep p in steps) averageDistToAverage += (p.ingredients.Count-average) * (p.ingredients.Count-average);
        averageDistToAverage /= steps.Count;

        return averageDistToAverage;
    }

    public void Shuffle()
    {
        Debug.Log("Shuffle !");
        System.Random rnd = new System.Random((int)Time.time);
        steps = steps.OrderBy(item => rnd.Next()).ToList();
        foreach (PouringStep s in steps) Debug.Log("Target position : "+s.targetPosition);
    }


    public override string ToString()
    {
        String pList = "";
        foreach (PouringStep s in steps) pList += s.ingredients.Count + ", ";
        int maxPouringCount;
        return "[Pouring Path : Steps : " + steps.Count + ", Total motion :" + getTotalMotion() + ", Max pouring  : " + getMaxCoalescedPouringQuantity() + ", Max simultaneous pouring : " + getMaxSimultaneousPouring(out maxPouringCount) + " (" + maxPouringCount + " times), Pouring count list : "+pList+"]";
    }
    
}