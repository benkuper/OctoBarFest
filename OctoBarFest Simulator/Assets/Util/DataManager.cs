using System;
using System.Text;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DataManager
{
    public static DataManager instance;

    static string basePath;
    static bool isInit;

    void Awake()
    {
        instance = this;
    }
    // Use this for initialization
    public static void init()
    {
#if UNITY_EDITOR
        basePath = Directory.GetCurrentDirectory().Replace('\\','/') + "/../data";
#else
        //basePath = "storage/emulated/0/PPP/"; //Android sync folder
#endif
        Debug.Log("Base Path :" + basePath + ": exists ? " + Directory.Exists(basePath));

        isInit = true;
    }

    public static string[] getFileList(string dir)
    {
        if (!isInit) init();
        return Directory.GetFiles(basePath + "/" + dir);
    }

    public static string getFileData(string filePath)
    {
        if (!isInit) init();
        string fullpath = basePath + "/" + filePath;
        if (!File.Exists(fullpath))
        {
            Debug.LogWarning("File not found : " + fullpath);
            return "";
        }
        StreamReader s = new StreamReader(fullpath, Encoding.Unicode);
        string data = s.ReadToEnd();
        s.Close();
        return data;
    }

    public static void writeFileData(string dir, string file, string data)
    {
        if (!isInit) init();

        Directory.CreateDirectory(basePath + dir);

        if (!File.Exists(basePath + "/" + dir + "/" + file))
        {
            File.Create(basePath + "/" + dir + "/" + file);
        }

        StreamWriter s = new StreamWriter(basePath + dir + "/" + file, false, Encoding.Unicode);
        s.Write(data);
        s.Close();
    }
}
