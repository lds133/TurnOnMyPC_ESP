#pragma once


#define PRWSW_NONE    0
#define PRWSW_OK      1
#define PRWSW_BUSY    2


#define PRWSW_IDLE    1
#define PRWSW_PRESS   2
#define PRWSW_REST    3

#define PRWSW_SHORTPRESS   1
#define PRWSW_LONGPRESS    2


#define PRWSW_SHORTPRESS_TIME 1500 //ms
#define PRWSW_LONGPRESS_TIME 15000 //ms
#define PRWSW_REST_TIME 15000 //ms


#define EVENT_PSWPRESS   101
#define EVENT_PSWREST    102
#define EVENT_PSWIDLE    103

typedef void pwrsw_event_function(int eventid);

class PowerSwitch
{
    public:
        PowerSwitch();
        void Setup(int pinnumber,pwrsw_event_function* eventfnc);
        void Loop(unsigned long* t);

        int Press(unsigned long* t,int presstype);
        

    
    private:
        unsigned long _t;
        int _pin;
        int _state;
        pwrsw_event_function* _eventfnc;
   
    
    
};
