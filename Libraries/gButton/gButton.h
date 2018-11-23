/*
||	gButton class
||
||	Author: Adrian Ramirez
||	Version:1.0.0
||	Contact: aredlabs@gmail.com
||
||	This class is part of the Hardware Enclosure group, make no use of the delay
||	function and can be used with the general beginAll and updateAll functions, see
||	hardwareEnclosure.cpp
||
||	Description: This class provides an easy way to work with buttons and other momentary
||	contact switches. It has a debounce filter and can be used with internal pullup or
||	external pulldown or pullup resistor (10kOhm are suggested).
||
||	All times are in milliseconds.
||
||	The Hardware Enclosure is a group of libraries dedicated to simplify at the maximum
||	possible the use of various devices like buttons, leds, etc providing high level 
||	function easy to understand and implement in your projects.
||
||
||	Update History
||	--------------------------------------------------------------------------------------
||	Version 1.0.0 dd/mm/yyyy: First Release.
||	--------------------------------------------------------------------------------------
*/

#ifndef __gBUTTON__
#define __gBUTTON__

#include<Arduino.h>
#include"hardwareEnclosure.h"



#define ButtonLAST         0 
#define ButtonACTUAL       1
#define ButtonCHANGE       2
#define ButtonCLICK        3
#define ButtonReleaseCLICK 4
#define ButtonMultiCLICK   5
#define ButtonPULLUP       6

#define ButtonDEBOUNCE     50
#define ButtonClickTime    300


class gButton:public hardwareEnclosure
{
	public:
		gButton(uint8_t pinNumber, bool pullUp = false);
		virtual void begin();
		virtual void update();
		bool pressed();	
		bool released();
		bool sustained();
		bool change();
		bool click();
		bool releaseClick();
		bool multiPress(uint8_t = 2);
		bool multiClick(uint8_t = 2);
		void setDebounceTime(uint8_t debounceTime);
		uint8_t getDebounceTime();
		void setClickTime(uint16_t clickTime);
		uint16_t getClickTime();
	private:
		uint8_t _pin;                         //Pin on the Arduino board in which the button will be connected
		uint8_t _debounceTime;                //Time that takes the button to bounce
		uint8_t _status;                      //Status of the button
		uint8_t _multiClickCount;             //Counts the multiples clicks
		unsigned long _lastTime;              //Last time since the button was updated
		unsigned long _lastPressTime;         //Last time since the button was pressed
		uint16_t _timeBetweenClicks;          //Time between each click
		
		void _clickCheck(unsigned long);      //Utility function, used in the update method
};

#endif