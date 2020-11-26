#pragma once

#include <ESP8266WiFi.h>
#include <PubSubClient.h>





typedef void delay_function(unsigned long n);
typedef void mqtt_callback_function(char* topic, unsigned char* payload, unsigned int len);
typedef void mqtt_event_function(int eventid);

#define EVENT_NONE              0 
#define EVENT_WIFICONNECTING    1
#define EVENT_WIFICONNECTED     2
#define EVENT_MQTTCONNECTING    3
#define EVENT_MQTTCONNECTED     4
#define EVENT_MQTTCONNECT_ERROR 5
#define EVENT_ESPSTUCK          6

class MyESP
{
    public:
        MyESP();

        void Setup( char* wifi_ssid,
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
                    
                    );

        void Loop(unsigned long* t);

        void Publish(char* topic,char* text);
        void Publish(char* topic,int n);
        
        void Subscribe(char* topic);
        void SoftReset(); 



    private:

        char* _wifi_ssid;
        char* _wifi_pwd;
        char* _mqtt_server;
        char* _mqtt_user;
        char* _mqtt_pwd;
        char* _mqtt_id;   
        delay_function* _delayfnc;   
        mqtt_callback_function* _mqttfnc;
        WiFiClient* _wificlnt;
        PubSubClient* _mqttclnt;
        mqtt_event_function* _eventfnc;
        void MQTTReconnect() ;  
            

        


    
};
