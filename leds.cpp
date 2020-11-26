#include "leds.h"
#include <stdlib.h>
#include <arduino.h>


#define PRINT(A) Serial.print(A)
//#define PRINT(A) 

Leds::Leds()
{
    _ledpins=NULL;
    _ledhi=NULL;
    _ledlo=NULL;
    _ledmode=NULL;
    _count=0;
    _isnolight=false;
  
}

void Leds::Setup(int count, int* ledpins,int* ledhi,int* ledlo,int* ledmode)
{
    _ledpins=ledpins;
    _ledhi=ledhi;
    _ledlo=ledlo;
    _ledmode=ledmode;
    _count=count;

    PRINT("LEDS pins ");
    for(int i=0;i<_count;i++)
    {   PRINT(_ledpins[i]);
        PRINT(" ");
    }
    PRINT("\r\n");

    for(int i=0;i<_count;i++)
    {   pinMode(_ledpins[i], OUTPUT);
        Set(i,LED_OFF);
    }
    
}


void Leds::SetNoLight(bool isturnitoff)
{
    _isnolight =  isturnitoff;   
}


void Leds::Loop(unsigned long* t)
{
    unsigned long now = t!=NULL ? *t : millis();

    if (now-_flashtime>LED_FLASHPERIOD)
    {
        for(int i=0;i<_count;i++)
            Animation(i,_flashanistage);
        _flashanistage++;
        if (_flashanistage>=LED_FLASCYCLEMAX)
            _flashanistage=0;
        _flashtime=now;
    }   
}



void Leds::Set(int i,int ledmode)
{
    PRINT("LED #");
    PRINT(i);
    PRINT(" -> ");
    switch(ledmode)
    {
        case LED_OFF:  
            PRINT("OFF");
            break;
        case LED_ON:  
            PRINT("ON"); 
            break;
        case LED_BLINK:  
            PRINT("BLINK"); 
            break; 
        case LED_BLINKHI:  
            PRINT("BLINK HI"); 
            break;
        case LED_BLINKLO:  
            PRINT("BLINK LO"); 
            break;    
        default:  
            PRINT("Unknown"); 
            break;    
    }
    PRINT("\r\n");
    
     _ledmode[i] = ledmode;
    
    if (ledmode==LED_OFF)
    {   digitalWrite(_ledpins[i],_ledlo[i]);
        return;
    } 
    
    if (ledmode==LED_ON)
    {   digitalWrite(_ledpins[i],_ledhi[i]); 
        return;
    }
   
}



void Leds::Animation(int i,int a)
{
    if (_isnolight)
    {
        digitalWrite(_ledpins[i], _ledlo[i]);
        return;  
    }
  
    if (_ledmode[i]==LED_BLINK)
    {
        digitalWrite(_ledpins[i],((a==0)||(a==1)) ? _ledhi[i] : _ledlo[i]);
        return;
    }  

    if (_ledmode[i]==LED_BLINKLO)
    {
        digitalWrite(_ledpins[i],(a==0) ? _ledhi[i] : _ledlo[i]);
        return;
    }  

    if (_ledmode[i]==LED_BLINKHI)
    {
        digitalWrite(_ledpins[i],((a==0)||(a==2)) ? _ledhi[i] : _ledlo[i]);
        return;
    }  

}
