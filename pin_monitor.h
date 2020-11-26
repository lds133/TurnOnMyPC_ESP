#pragma once


class PinMonitor
{
    public:
        PinMonitor();
        void Setup(int pinnumber,unsigned int period);
        void Loop(unsigned long* t);
        int Get();


    private:
        int _pin;
        unsigned long _t;
        unsigned int _period; //ms
        unsigned int _sum_on;
        unsigned int _sum_off;
        int _oldpinstate;
        int _persentvalue;
        
        

};
