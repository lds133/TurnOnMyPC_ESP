#include "pin_monitor.h"
#include <stdlib.h>
#include <arduino.h>


#define PRINT(A) Serial.print(A)
//#define PRINT(A) 



PinMonitor::PinMonitor()
{
    _pin=0;
    _t=0;
    _sum_on=0;
    _sum_off=0;
    _oldpinstate=0;
    _persentvalue =0;
}


void PinMonitor::Setup(int pinnumber,unsigned int period)
{
    _period = period;
    _pin = pinnumber;
    pinMode(_pin, INPUT);
    _t=millis();
    _sum_on=0;
    _sum_off=0;
    _oldpinstate = digitalRead(_pin);
    _persentvalue = _oldpinstate==HIGH ? 100 : 0;

    PRINT("PINMON at pin ");
    PRINT(_pin);
    PRINT(" (");
    PRINT(_oldpinstate);
    PRINT(")\r\n");
    
}


int PinMonitor::Get()
{   return _persentvalue;
}
        

void PinMonitor::Loop(unsigned long* t)
{
    
    unsigned long now = t!=NULL ? *t : millis();

    unsigned long dt = now -_t;

    int pinstate = digitalRead(_pin);
    if (pinstate==_oldpinstate)
    {   if (pinstate==LOW)
        {   _sum_on+=dt;  
        } else
        {  _sum_off+=dt;
        }
    } else
    {   _sum_on+=dt/2;
        _sum_off+=dt/2;
    }
    _oldpinstate = pinstate;

    unsigned int n = _sum_on+_sum_off;
    if (n>=_period)
    {
        _persentvalue = (_sum_on*100)/n;
        _sum_on=0;
        _sum_off=0;
    }

    _t=now;
    
}
