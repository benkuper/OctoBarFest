#ifndef OSC_MANAGER_H
#define OSC_MANAGER_H

#include "WifiManager.h"

#include <OSCMessage.h>
#include <OSCBoards.h>

class OSCManager
{
public:
  WiFiUDP &udp;

  char remoteHost[32];
  const int remotePort = 10000;

  bool pingEnabled;
  long lastPingTime;
  long pingTime;
  
  OSCManager(WiFiUDP &udp): udp(udp)
  {
    
    sprintf(remoteHost,"192.168.4.2");
    
    addCallbackMessageReceived(&OSCManager::defaultCallback);
    
    pingEnabled = false;
    lastPingTime = 0;
    pingTime = 1000;
  }
  
  void init()
  {
    #if DEBUG
    Serial.println("OCSManager init."); 
    #endif 
  }

  void update()
  {
    if(pingEnabled) ping();
    receiveOSC();
  }

  void receiveOSC()
  {
     OSCMessage msgIN;
     int size;
    
     if( (size = udp.parsePacket())>0)
     {
       while(size--)
       {
         msgIN.fill(udp.read());
       }
       
       if(!msgIN.hasError()) 
       {

        #if DEBUG
         Serial.println("Got message");
        #endif
        
        if(msgIN.match("/yo"))
        {
         
          msgIN.getString(0,remoteHost,32);

          #if DEBUG
          Serial.print("Got yo request from : ");
          Serial.println(remoteHost);
          #endif

          OSCMessage msg("/wassup");
          msg.add(ipToString());
          
          sendMessage(msg);
          
        }else
        {   
          onMessageReceived(msgIN);
        }
       }else
       {
        #if DEBUG
        Serial.println("Msg got error");
        #endif
       }
     }
  }
  
  
  void sendMessageTo(OSCMessage & msg, const char * host, int port)
  {
    udp.beginPacket(host,port);
    msg.send(udp);
    udp.endPacket();
    msg.empty();
  }

  void sendMessage(OSCMessage &msg)
  {
    sendMessageTo(msg,remoteHost,remotePort);
  }
  
  void ping()
  {
    if(millis() - lastPingTime < pingTime) return;
    
    OSCMessage msg("/ping");
    sendMessage(msg);

    lastPingTime = millis();
  }


  //Helpers
  char * ipToString()
  {
    char myIpString[24];
    IPAddress myIp = WiFi.localIP();
    sprintf(myIpString, "%d.%d.%d.%d", myIp[0], myIp[1], myIp[2], myIp[3]);
    return myIpString;
  }

  // Events
  typedef void(*onEvent)(OSCMessage &);
  void (*onMessageReceived) (OSCMessage &);
  
  void addCallbackMessageReceived (onEvent func) {
    onMessageReceived = func;
  }

  static void defaultCallback(OSCMessage &msg)
  {
    //nothing
  }
  
};

#endif
