#include "my_esp.h"
#include <stdlib.h>
#include <arduino.h>


#define PRINT(A) Serial.print(A)
#define PRINT2(A,B) Serial.print(A,B)

//#define PRINT(A) 
//#define PRINT2(A) 

#define PRINTESPINFO(A)  PRINT(#A ": ");PRINT( ESP.get##A()  );PRINT( "\r\n" ) 
#define DELAY(A) _delayfnc(A);
#define EVENT(A) _eventfnc(A);

#define INTTOSTR_BUFSIZE 50

#define MQTT_PORT_NUM 1883
#define MQTT_RECONNECT_FUSE_MAX 60
#define WIFI_RECONNECT_FUSE_MAX 1200

MyESP::MyESP()
{
    _wifi_ssid=NULL;
    _wifi_pwd=NULL;
    _mqtt_server=NULL;
    _mqtt_user=NULL;
    _mqtt_pwd=NULL;
    _mqtt_id=NULL; 
    _delayfnc = NULL;   
    _mqttfnc = NULL;
    _wificlnt = NULL;
    _mqttclnt = NULL;
    _eventfnc = NULL;
}




void MyESP::Setup(  char* wifi_ssid,
                    char* wifi_pwd,
                    char* mqtt_server,
                    char* mqtt_user,
                    char* mqtt_pwd,
                    char* mqtt_id,
                    delay_function* delayfnc,
                    mqtt_callback_function* mqttfnc,
                    WiFiClient* wificlnt,
                    PubSubClient* mqttclnt,
                    mqtt_event_function* eventfnc
                    
                    )
{
    _wifi_ssid=wifi_ssid;
    _wifi_pwd=wifi_pwd;
    _mqtt_server=mqtt_server;
    _mqtt_user=mqtt_user;
    _mqtt_pwd=mqtt_pwd;
    _mqtt_id=mqtt_id;
    _delayfnc = delayfnc;
    _mqttfnc = mqttfnc;
    _wificlnt = wificlnt;
    _mqttclnt = mqttclnt;
    _eventfnc = eventfnc;

    PRINTESPINFO(ChipId);
    PRINTESPINFO(FlashChipSize);
    PRINTESPINFO(FreeHeap);
    PRINTESPINFO(FreeSketchSpace);
    PRINTESPINFO(SketchSize);
    PRINTESPINFO(FlashChipId);
    PRINTESPINFO(SdkVersion);
    PRINTESPINFO(FlashChipRealSize);
    PRINTESPINFO(CpuFreqMHz);
    PRINTESPINFO(FlashChipSpeed);  
    PRINT("\r\n");

    DELAY(3);

    WiFi.persistent(false);
    WiFi.softAPdisconnect (true);

    EVENT(EVENT_WIFICONNECTING);
    WiFi.begin(_wifi_ssid,_wifi_pwd);
    PRINT("Connecting to '");
    PRINT(_wifi_ssid);
    PRINT("' ");
    int fuse = 0;
    while (WiFi.status() != WL_CONNECTED) 
    {   DELAY(500);
        PRINT(".");
        fuse++;
        if (fuse>WIFI_RECONNECT_FUSE_MAX)
        {   EVENT(EVENT_ESPSTUCK);
            fuse=0;
        }
    }
    PRINT("\r\n");
    EVENT(EVENT_WIFICONNECTED);
    

    PRINT("WiFi connected\r\n");
    PRINT("IP address: ");
    PRINT(WiFi.localIP());
    PRINT("\r\n");
    
    byte mac[6];
    WiFi.macAddress(mac);
    PRINT("MAC: ");
    PRINT2(mac[0],HEX);
    PRINT(":");
    PRINT2(mac[1],HEX);
    PRINT(":");
    PRINT2(mac[2],HEX);
    PRINT(":");
    PRINT2(mac[3],HEX);
    PRINT(":");
    PRINT2(mac[4],HEX);
    PRINT(":");
    PRINT2(mac[5],HEX);
    PRINT("\r\n");


    long rssi = WiFi.RSSI();
    Serial.print("RSSI:");
    Serial.println(rssi);

    _mqttclnt->setServer(_mqtt_server, MQTT_PORT_NUM);
    _mqttclnt->setCallback(_mqttfnc);

    
}




void MyESP::Loop(unsigned long* t)
{
    
    //unsigned long now = t!=NULL ? *t : millis();

    if (!_mqttclnt->connected())
        MQTTReconnect();
    _mqttclnt->loop();

}


void MyESP::SoftReset()
{
    ESP.reset();
}


void MyESP::Publish(char* topic,char* text)
{   if (_mqttclnt!=NULL)
        _mqttclnt->publish(topic, text);
}



void MyESP::Publish(char* topic,int n)
{
    char text[INTTOSTR_BUFSIZE]={0};
    itoa(n,text,10);
    Publish(topic,text);
}

void MyESP::Subscribe(char* topic)
{
    _mqttclnt->subscribe(topic);
}


void MyESP::MQTTReconnect() 
{

    int fuse=0;
    while (!_mqttclnt->connected()) 
    {
        DELAY(1);
        EVENT(EVENT_MQTTCONNECTING);        
        
        PRINT("Connect attempt ");
        PRINT(fuse+1);
        PRINT(" ("); 
        PRINT(MQTT_RECONNECT_FUSE_MAX);
        PRINT(")");
        PRINT(" to ");
        PRINT(_mqtt_server);
        PRINT(" as ");
        PRINT(_mqtt_id);
        PRINT(" ... ");
        
        if (_mqttclnt->connect(_mqtt_id,_mqtt_user, _mqtt_pwd)) 
        {  
            PRINT("connected\r\n");
            EVENT(EVENT_MQTTCONNECTED);
        } else 
        {
            EVENT(EVENT_MQTTCONNECT_ERROR);
            PRINT("failed, rc=");
            PRINT(_mqttclnt->state());
            PRINT(" try again in 10 seconds\r\n");
            DELAY(10000);
            fuse++;
            if (fuse>60)
            {   EVENT(EVENT_ESPSTUCK);
                fuse=0;
            }
        }
    }
    

  
}
