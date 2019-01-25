#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H


#include <WiFi.h>
#include <WiFiUDP.h>
#include <OSCMessage.h>

class WifiManager
{
  public:

    String ssid = "OctoBarFest";
    String password = "pumpitup";


    WiFiUDP oscUDP;
    WiFiUDP streamingUDP;

    bool isConnected;

    bool apMode;

#if USE_SERVER
    WifiSettingsServer wServer;
#endif

    WifiManager() {

      apMode = true;
      isConnected = false;
      addCallbackConnectingUpdate(&WifiManager::connectingUpdateDefaultCallback);
    }

    void init()
    {
      #if DEBUG
      Serial.println("WifiManager init.");
      Serial.print("Setting up AP Wifi : OctoBarFest, pass : pumpitup");
      #endif

      WiFi.softAP("OctoBarFest", "pumpitup");

      #if USE_SERVER
            wServer.init();
      #endif //end USE_SERVER
      
#if DEBUG
      if (apMode)
      {
        Serial.println("");
        Serial.print("Wifi AP created, IP address: ");
        Serial.println(WiFi.softAPIP());
      } else
      {
        Serial.println("");
        Serial.print("Connected to ");
        Serial.println(ssid);
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
      }

#endif
      delay(500);

#if DEBUG
      Serial.print("Setting up UDP...");
#endif
      
      oscUDP.begin(9000);
      streamingUDP.begin(8888);

#if DEBUG
      Serial.println("OK");
#endif

      isConnected = true;
      delay(500);
    }

    

    //EVENTS

    typedef void(*onConnectingUpdateEvent)(int);
    void (*onConnectingUpdate) (int);


    void addCallbackConnectingUpdate (onConnectingUpdateEvent func) {
      onConnectingUpdate = func;
    }

    

    static void connectingUpdateDefaultCallback(int curTry)
    {
      //nothing
    }

};

#endif
