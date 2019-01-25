#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUDP.h>
#include <OSCMessage.h>

class WifiManager
{
  public:

    String ssid = "OctoBarFest";
    String password = "pumpitup";

    const int maxTries = 50;

    WiFiUDP oscUDP;
    WiFiUDP streamingUDP;

    bool isConnected;


    WifiManager() {
      isConnected = false;
      addCallbackConnectingUpdate(&WifiManager::connectingUpdateDefaultCallback);
    }

    void init()
    {
#if SERIAL_DEBUG
      Serial.println("WifiManager init.");
#endif

      // Connect to WiFi network
      #if SERIAL_DEBUG
      Serial.println(String("WifiManager connecting to "+ssid+" : "+password));
      #endif
      
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid.c_str(),password.c_str());

      int tryIndex = 0;
      bool success = true;

#if SERIAL_DEBUG
      Serial.print("Connecting : ");
#endif

      // Wait for connection
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);

#if SERIAL_DEBUG
        Serial.print("*");
#endif

        if (tryIndex >= maxTries)
        {
          success = false;
          break;
        }
        onConnectingUpdate(tryIndex);
        tryIndex++;
      }

#if SERIAL_DEBUG
      if (success) 
      {
        Serial.println("");
        Serial.print("Connected to ");
      Serial.println(ssid);
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      }else 
      {
        Serial.print("Failed");
        Serial.println();
      }
#endif

      delay(500);

#if SERIAL_DEBUG
      Serial.print("Setting up UDP...");
#endif

      oscUDP.begin(9000);
      streamingUDP.begin(8888);

#if SERIAL_DEBUG
      Serial.println("OK");
#endif

/*
#if USE_BONJOUR
  #if SERIAL_DEBUG
        Serial.print("Registering mDNS...");
        Serial.print("...");
  #endif

      if (!mdns.begin("bento", apMode ? WiFi.softAPIP() : WiFi.localIP())) {
#if SERIAL_DEBUG
        Serial.println("Error setting up MDNS responder!");
#endif
      } else
      {
#if SERIAL_DEBUG
        Serial.print("OK : ");
        Serial.println("Registering Bonjour service...");
#endif

        mdns.addService("ledcontrol", "udp", 8888);
#endif
      }
      */
      isConnected = true;
      delay(500);
    }

    void setupLocalWifi()
    {
      //uint8 mac[6];
      //WiFi.softAPmacAddress(mac);
      //String macString((const char *)mac);
      String wifiString(String("BenTo v4.2 ") +  ESP.getChipId());

#if SERIAL_DEBUG
      Serial.print("Setting up AP Wifi : ");
      Serial.println(wifiString);
#endif


      WiFi.softAP(wifiString.c_str());

#if USE_SERVER
      wServer.init();
#endif //end USE_SERVER
    }

    void update()
    {
#if USE_SERVER
      if (apMode) 
      {
        wServer.update();
        if(wServer.ssid.length() > 0)
        {
          Serial.println("Wifi Manager saving config...");
          saveWifiConfig(wServer.ssid.c_str(),wServer.pass.c_str());
          
        }
      }
#endif
    }

#if USE_OSC
    boolean handleMessage(OSCMessage &msg)
    {

      if (msg.match("/wifiConfig"))
      {
        if (msg.size() >= 2)
        {
          char ssidData[32];
          char passData[32];
          msg.getString(0, ssidData, msg.getDataLength(0));
          msg.getString(1, passData, msg.getDataLength(1));

          //saveWifiConfig(ssidData, passData);
        }
        return true;
      }

      return false;
    }
#endif


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
