/*
||	gLed class
||
||	Author: Adrian Ramirez
||	Version: 1.0.0
||	Contact: aredlabs@gmail.com	
||
||	Description: This class provides a simple way to use and implement leds in your
||	  projects, it includes some effects like fade in/out and blinking without using the 
||	  delay function which allows you to use those effects without affecting the program
||	  flow.
||
||	If you call one method consecutively with the same parameters only the first one will
||	  take effect.
||
||	Change Log.
||	**************************************************************************************
||	Version: 1.0.0 dd/mm/yyyy	First Release.
||	**************************************************************************************
*/



#ifndef __gLed__
#define __gLed__

#include<Arduino.h>

	//Different led status
#define LedOFF                 0
#define LedON                  1
#define LedPWM                 2
#define LedBlinkON             3 
#define LedBlinkOFF            4
#define LedFadeBlinkIN         5
#define LedFadeBlinkOUT        6
#define LedFadeIN              7
#define LedFadeOUT             8
#define LedTimeON              9
#define LedTimeOFF             10
#define LedMultiBlinkON        11
#define LedMultiBlinkOFF       12
#define LedPulseDELAY          13
#define LedPulseON             14
#define LedMultiPulseDELAY     15
#define LedMultiPulseON        16
#define LedPulseFadeDELAY      17
#define LedPulseFadeON         18
#define LedMultiPulseFadeDELAY 19
#define LedMultiPulseFadeON    20
#define LedMultiBlinkFadeON    21
#define LedMultiBlinkFadeOFF   22

	//Last method used
#define M_restart        255
#define M_ledOff         0
#define M_ledOn          1
#define M_fadeIn         2
#define M_fadeOut        3
#define M_blink          4
#define M_fadeBlink      5
#define M_multiBlink     6
#define M_pulse          7
#define M_pulseFade      8
#define M_multiPulse     9
#define M_multiPulseFade 10
#define M_multiBlinkFade 11

void beginLeds();
void updateLeds();

class gLed
{
	friend void beginLeds();
	friend void updateLeds();
	public:
		gLed(uint8_t pinNumber);
		~gLed();
		void begin();
		void update();
		void ledOn(uint8_t bright = 100, uint16_t delay = 0);
		void ledOff(uint16_t delay = 0);
		void fadeIn(uint16_t duration, uint8_t maxBright = 100, uint8_t minBright = 0);
		void fadeOut(uint16_t duration, uint8_t maxBright = 100, uint8_t minBright = 0);
		void blink(uint16_t duration, uint8_t duty = 50, uint8_t maxBright = 100, uint8_t minBright = 0);
		void blinkFade(uint16_t duration, uint8_t duty = 50, uint8_t maxBright = 100, uint8_t minBright = 0);
		void multiBlink(uint16_t duration, uint8_t blinks = 2, uint8_t duty = 50, uint8_t maxBright = 100, uint8_t minBright = 0);
		void multiBlinkFade(uint16_t duration, uint8_t blinks = 2, uint8_t duty = 50, uint8_t maxBright = 100, uint8_t minBright = 0);
		void pulse(uint16_t duration, uint16_t delay = 0, uint8_t maxBright = 100, uint8_t minBright = 0);
		void pulseFade(uint16_t duration, uint16_t delay = 0,uint8_t duty = 50, uint8_t maxBright = 100, uint8_t minBright = 0);
		void multiPulse(uint16_t duration, uint8_t pulses = 2, uint16_t delay = 0, uint8_t duty = 50, uint8_t maxBright = 100, uint8_t minBright = 0);
		void multiPulseFade(uint16_t duration, uint8_t pulses = 2, uint16_t delay = 0, uint8_t duty = 50, uint8_t maxBright = 100, uint8_t minBright = 0);
	private:
		uint8_t  _pin;            //Pin on the Arduino board in which the led will be connected
		uint8_t  _status;         //Status of the led
		uint8_t  _maxBright;      //Maximum brightness of the led 
		uint8_t  _minBright;      //Minimum brightness of the led 
		uint8_t  _lastMethod;     //Last method used
		uint16_t _period;         //Period of the blink/fade effect
		uint16_t _timeAux;        //Auxiliar time variable used in the blink and blinkFade effects
		uint8_t  _blinks;         //Number of blinks if the multiBlink effect
		uint8_t  _counts;         //Auxiliar variable to count the number of blinks
		uint8_t  _counts2;        //Auxiliar variable to count the number of blinks
		unsigned long _lastTime;  //Time variable used in various effects
		
			//Variables and methods used to implement beginLeds() and updateLeds()
		static gLed **_addressBook;                //Contains the address of all gLed objects in the sketch
		static uint8_t _size;                      //The number of gLeds objects in the sketch
		static void   _includeItem(gLed* newItem); //Function used to include a new object in the address Book
};

#endif