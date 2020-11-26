#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "leds.h"
#include "pin_monitor.h"
#include "power_sw.h"
#include "my_esp.h"




#define PIN_BLUETOPLED       LED_BUILTIN
#define PIN_YELLOWLED        13

#define PIN_POWERSW          12
#define PIN_SIGPOWERLED      16
#define PIN_SIGHDDLED        14

#define LED_BLUE      0
#define LED_YELLOW    1

#define PINMONITOR_PERIOD_MS   3000

#define GROUPID "mypc"

#define MQTTTOPIC_CTRL "/home/" GROUPID  "/ctrl" 
#define MQTTTOPIC_INFO "/home/" GROUPID  "/info" 
#define MQTTTOPIC_ONDUTY  "/home/on_duty"
#define CLIENT_ID  "ESP_" GROUPID 

#define CLIENT_LOGIN "sensor"
#define CLIENT_PASSWORD "yourpassword"

#define WIFISSID "cinco_hsh"
#define WIFIPASSWORD "yourpassword"
#define MQTTSERVERIP "192.168.1.1"

#define EVENT_PUBLISHLEDSTATUS  200
#define PUBLISHLEDSTATUSPERIOD  10000//ms






int  _ledpins[] =  {PIN_BLUETOPLED,PIN_YELLOWLED};
int  _ledhi[]   =  {LOW,HIGH};
int  _ledlo[]   =  {HIGH,LOW};
int _ledmode[] =  {LED_OFF,LED_OFF};

char ssid[] =  WIFISSID;
char password[] = WIFIPASSWORD;
char mqtt_server[] = MQTTSERVERIP;
char mqtt_user[] = CLIENT_LOGIN;
char mqtt_pass[] = CLIENT_PASSWORD;
char mqtt_client[] = CLIENT_ID;


WiFiClient _wificlient;
PubSubClient _mqttclient(_wificlient);

Leds _leds;
PinMonitor _hddled;
PinMonitor _pwrled;
PowerSwitch _pwrsw;
MyESP _esp;

unsigned long _publishtime;




void event(int eventid)
{

    switch(eventid)
    {   
        case EVENT_MQTTCONNECTING:
            _leds.Set(LED_YELLOW,LED_ON); 
            break;
            
        case EVENT_MQTTCONNECTED:
            _leds.Set(LED_YELLOW,LED_OFF);
            _esp.Publish(MQTTTOPIC_ONDUTY,mqtt_client);
            _esp.Subscribe(MQTTTOPIC_CTRL);
            break;
            
        case EVENT_WIFICONNECTING:
            _leds.Set(LED_BLUE,LED_ON); 
            break;
            
        case EVENT_WIFICONNECTED:
            _leds.Set(LED_BLUE,LED_OFF); 
            break;
            
        case EVENT_ESPSTUCK:
            _leds.Set(LED_BLUE,LED_BLINKHI);
            _leds.Set(LED_YELLOW,LED_BLINKHI);
            Serial.println("ESP Soft Reset in 10 sec...");
            safedelay(10000);            
            _esp.SoftReset();
            break;
            
        case EVENT_MQTTCONNECT_ERROR:
            break;

        case EVENT_PSWPRESS:
            _leds.Set(LED_YELLOW,LED_ON);
            _esp.Publish(MQTTTOPIC_INFO,"Press");
            break;
            
        case EVENT_PSWREST:
            _leds.Set(LED_YELLOW,LED_BLINKHI);
            _esp.Publish(MQTTTOPIC_INFO,"Resting");
            break;
        case EVENT_PSWIDLE:
            _leds.Set(LED_YELLOW,LED_OFF);
            _esp.Publish(MQTTTOPIC_INFO,"Ready");
            break;

        case EVENT_PUBLISHLEDSTATUS:
            {   char text[20]={0};
                itoa(_hddled.Get(),text+strlen(text),10);
                strcat(text,",");
                itoa(_pwrled.Get(),text+strlen(text),10);
                Serial.print("Leds: ");
                Serial.println(text);
                _esp.Publish(MQTTTOPIC_INFO,text);
            }
            break;
    }
}


void mqttcallback(char* topic, byte* payload, unsigned int len) 
{
    Serial.print("Message ");
    for (int i = 0; i < len; i++) 
    {   Serial.print((char)payload[i]);
    }
    Serial.println();


    int cmd = (len==0) ? PRWSW_NONE : ((unsigned char)payload[0] - '0');


    int rc = PRWSW_NONE;
    switch(cmd)
    {
        case PRWSW_SHORTPRESS:
            rc = _pwrsw.Press(NULL,PRWSW_SHORTPRESS);
            break;
        case PRWSW_LONGPRESS:
            rc = _pwrsw.Press(NULL,PRWSW_LONGPRESS);
            break;
    }

    switch(rc)
    {   case PRWSW_NONE:
            _esp.Publish(MQTTTOPIC_INFO,"None");
            break;
        case PRWSW_OK:
            _esp.Publish(MQTTTOPIC_INFO,"Ok");
            break;            
        case PRWSW_BUSY:
            _esp.Publish(MQTTTOPIC_INFO,"Busy");
            break;            
    }
   
}

void setup() 
{
    Serial.begin(115200);
    Serial.print("\r\n\r\n* Turn On My PC * (c) June 2019\r\n");

    _leds.Setup( sizeof(_ledpins)/sizeof(int), _ledpins, _ledhi, _ledlo, _ledmode);
    _hddled.Setup(PIN_SIGHDDLED,PINMONITOR_PERIOD_MS);
    _pwrled.Setup(PIN_SIGPOWERLED,PINMONITOR_PERIOD_MS);
    _pwrsw.Setup(PIN_POWERSW,&event);

    _esp.Setup(ssid,password,mqtt_server,mqtt_user,mqtt_pass,mqtt_client,&safedelay,&mqttcallback,&_wificlient,&_mqttclient,&event);

    _publishtime = 0;

    Serial.print("Ctrl topic: ");
    Serial.println(MQTTTOPIC_CTRL);
    Serial.print("Info topic: ");
    Serial.println(MQTTTOPIC_INFO);
    Serial.println();
}

void loop() 
{
    unsigned long t = millis();

    dostaff(t);
    
    _esp.Loop(&t);

    if (t>_publishtime)
    {   _publishtime = t+PUBLISHLEDSTATUSPERIOD;
        event(EVENT_PUBLISHLEDSTATUS);
    }

    delay(1);
}




void dostaff(unsigned long t)
{
    _leds.Loop(&t);
    _hddled.Loop(&t);
    _pwrled.Loop(&t);
    _pwrsw.Loop(&t); 
}


void safedelay(unsigned long n_ms)
{
    unsigned long t_stop = millis()+n_ms;
    unsigned long t;
    while(true)
    {
        t = millis();
        if (t>t_stop)
            break;
        dostaff(t);
        delay(1);
    }
}
