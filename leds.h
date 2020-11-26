#pragma once

#define LED_OFF        0
#define LED_ON         1
#define LED_BLINK      2 
#define LED_BLINKLO    3 
#define LED_BLINKHI    4 


#define LED_FLASHPERIOD   250
#define LED_FLASCYCLEMAX  4



class Leds
{
  public:
    Leds();
    void Setup(int count, int* ledpins,int* ledhi,int* ledlo,int* ledmode);
    void Loop(unsigned long* t);
    void Set(int i,int ledmode);
    void SetNoLight(bool isturnitoff);


    private:    
        int* _ledpins;
        int* _ledhi;
        int* _ledlo;
        int* _ledmode;
        int _count;

        unsigned long _flashtime=0;
        unsigned long _flashanistage=0;
        void Animation(int i,int a);
        bool _isnolight;
  
  
};
