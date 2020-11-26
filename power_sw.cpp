#include "power_sw.h"
#include <stdlib.h>
#include <arduino.h>


#define PRINT(A) Serial.print(A);
//#define PRINT(A) 


#define EVENT(A) _eventfnc(A)

PowerSwitch::PowerSwitch()
{
    _t=0;
    _pin=0;
    _state = PRWSW_NONE;
    _eventfnc = NULL;
}


void PowerSwitch::Setup(int pinnumber,pwrsw_event_function* eventfnc)
{
    _eventfnc = eventfnc;
    _pin = pinnumber;
    pinMode(_pin, OUTPUT);
    _state = PRWSW_IDLE;
    digitalWrite(_pin, LOW);

    PRINT("PWRSW Ready at pin ");
    PRINT(_pin);
    PRINT("\r\n");

    //EVENT(EVENT_PSWIDLE);
}


void PowerSwitch::Loop(unsigned long* t)
{
    if (_state == PRWSW_IDLE)
        return;
        
    unsigned long now = t!=NULL ? *t : millis();

    if (_state == PRWSW_PRESS)
    {
        if ( now > _t)
        {   digitalWrite(_pin, LOW);
            PRINT("PWRSW Release\r\n");
            _state = PRWSW_REST;
            _t = now + PRWSW_REST_TIME;
            EVENT(EVENT_PSWREST);            
        }
        return;
    }

    if (_state == PRWSW_REST)
    {
        if ( now > _t)
        {
            _state = PRWSW_IDLE;
            PRINT("PWRSW Idle\r\n");
            EVENT(EVENT_PSWIDLE);
        }
        return;
    }
}


int PowerSwitch::Press(unsigned long* t,int presstype)
{
    if (_state != PRWSW_IDLE)
        return PRWSW_BUSY;

    unsigned long now = t!=NULL ? *t : millis();

    digitalWrite(_pin, HIGH);
    PRINT("PWRSW Press\r\n");
    EVENT(EVENT_PSWPRESS);    
    
    _state = PRWSW_PRESS;
    _t = now + ((presstype==PRWSW_LONGPRESS) ? PRWSW_LONGPRESS_TIME : PRWSW_SHORTPRESS_TIME);
    return PRWSW_OK;
}
