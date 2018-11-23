/*
||	hardwareEnclosure base class
||
||	Author: Adrian Ramirez
||	Version:1.0.0
||	Contact: aredlabs@gmail.com
||
||	This is the base class of the Hardware Enclosure group.
||
||	Description: This is the base class of all Hardware Enclosure libraries group, it 
||	contains the basic declarations of the begin and update methods for their use in the 
||	more general beginAll and updateAll functions and contain the address of all objects 
||	that are part of the group.
||
||	The Hardware Enclosure is a group of libraries dedicated to simplify at the maximum
||	possible the use of various devices like buttons, leds, etc providing high 
||	level function easy to understand and implement in your projects.
||
||
||	Update History
||	--------------------------------------------------------------------------------------
||	Version 1.0.0 dd/mm/yyyy: First Release.
||	--------------------------------------------------------------------------------------
*/

#ifndef __HARDWARE_ENCLOSURE__
#define __HARDWARE_ENCLOSURE__

#include"dynamicVector.h"


void updateAll();
void beginAll();

class hardwareEnclosure
{
	friend void beginAll();
	friend void updateAll();
	
	public:
		virtual void begin() = 0;
		virtual void update() = 0;
	protected:
		static dynamicVector<hardwareEnclosure*> _addressBook; //Contains the address in 
															   //memory of all 
															   //hardwareEnclosure objects
															   //in you sketch
};

#endif