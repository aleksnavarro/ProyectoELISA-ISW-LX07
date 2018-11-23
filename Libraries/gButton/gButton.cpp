#include<Arduino.h>
#include"gButton.h"



/*------------------------------------Constructor-----------------------------------------
||  Description: The constructor set the initial values of all variables.
||
||  Syntax: eButton myButton(pin,pullup)
||
||	Arguments: pinNumber.-         The pin in which the button will be connected.
||			   pullUp(optional).-  This is an optional boolean argument, if is set as true
||								   the internal pull-up will be enabled, if is set as 
||								   false you must provide an external pull-up or pulldown 
||								   resistor, 10kOhm are suggested.
||								     Default value: false.
||
||	Return: No return.
||
||	See: setDebounceTime()
||		 getDebounceTime()
||		 setClickTime()
||		 getClickTime()
*/
gButton::gButton(uint8_t pinNumber, bool pullUp)
{
    _pin = pinNumber;
    _debounceTime = ButtonDEBOUNCE;
    _timeBetweenClicks = ButtonClickTime;
    _lastTime = 0;
    _lastPressTime = 0;
    _status = 0;
    _multiClickCount = 0;
    if(pullUp == true)
    {
    	bitSet(_status,ButtonPULLUP); 
    	bitSet(_status,ButtonLAST);
    	bitSet(_status,ButtonACTUAL);
    }
    
	_addressBook.pushIn((hardwareEnclosure*)this);  //Used to include this object in the 
}                                                   //hardwareEnclosure address book



/*--------------------------------------begin method--------------------------------------
||	Description: This method set the correct mode on the button pin. This method need to
||	  be called in the setup function.
||
||	Syntax: begin()
||
||	Arguments: No arguments.
||
||	Return: No return.
||
||	See: constructor 
||		 beginAll() (defined in hardwareEnclosure.cpp)
*/
void gButton::begin()
{
    if(bitRead(_status,ButtonPULLUP)==1)
    	pinMode(_pin,INPUT_PULLUP);
	else
		pinMode(_pin,INPUT);
}



/*--------------------------------------update method-------------------------------------
||	Description: This method update the timers and all status of the button. This method 
||	  need to be called at least one time on the main loop function, after any (long) 
||	  delay function or at the top of any user loop (for,while,do while) if the button 
||	  is used inside that loop.
||
||	Syntax: update()
||
||	Arguments: No arguments.
||
||	Return: No return.
||
||	See: updateAll() (defined in hardwareEnclosure.cpp)
||	     setDebounceTime()
||		 getDebounceTime()
*/
void gButton::update()
{
    unsigned long actualTime;

    actualTime = millis();

    if (actualTime - _lastTime > (unsigned long)_debounceTime)              //Debounce filter
    {
        bitWrite(_status,ButtonLAST,bitRead(_status,ButtonACTUAL));       //Update the last state of the button (presed or not)
        bitWrite(_status,ButtonACTUAL,digitalRead(_pin));                 //Update the actual state of the button
        _lastTime = actualTime;                                           //Update the time

        if (bitRead(_status,ButtonACTUAL) != bitRead(_status,ButtonLAST)) //If occurs a change in the state of the button
            bitSet(_status,ButtonCHANGE);         //Set Change to 1
        else                                      //otherwise
            bitClear(_status,ButtonCHANGE);       //set Change to 0
    }
    else
        bitClear(_status,ButtonCHANGE);           //Set Change to 0

    _clickCheck(actualTime);                      //Used to check if a multiple press occurs
}



/*-------------------------------------pressed method-------------------------------------
||  Description: This method return true if the button has been pressed since the last 
||	  time it was updated, otherwise return false.
||
||	Syntax: pressed()
||
||	Arguments: No arguments.
||
||	Return: True if the button was pressed, false otherwise.
||
||	See: update()
||		 updateAll() (defined in hardwareEnclosure.cpp)
*/
bool gButton::pressed()
{
    if(bitRead(_status,ButtonCHANGE))        //If a change occurs
    {
    	if(bitRead(_status,ButtonPULLUP)==0) //Check the button mode 
    	{
        	if(bitRead(_status,ButtonACTUAL) && !bitRead(_status,ButtonLAST)) //If the button goes from LOW to HIGH
            	return true;     //Return true
        	else
            	return false;    //Otherwise return false
        }
        else //If the internal pull-up are enabled the HIGH and low states are backwards 
        {
        	if(!bitRead(_status,ButtonACTUAL) && bitRead(_status,ButtonLAST)) //If the button goes from HIGH to LOW
            	return true;     //Return true
        	else
            	return false;    //Otherwise return false
        }
    }                            
    else
        return false;            //Otherwise return false
}



/*----------------------------------released method---------------------------------------
||  Description: This method return true if the button has been released since the last 
||	  time it was updated, otherwise return false.
||
||	Syntax: released()
||
||	Arguments: No arguments.
||
||	Return: True if the button was released, false otherwise.
||
||	See: update()
||		 updateAll() (defined in hardwareEnclosure.cpp)
*/
bool gButton::released()
{
    if(bitRead(_status,ButtonCHANGE))        //If a change occurs
    {
    	if(bitRead(_status,ButtonPULLUP)==0) //Check the button mode 
    	{
        	if(!bitRead(_status,ButtonACTUAL) && bitRead(_status,ButtonLAST)) //If the button goes from HIGH to LOW
           	 return true;        //Return true
        	else
            	return false;    //Otherwise return false
        }
        else   //If the internal pull-up are enabled the HIGH and low states are backwards
    	{
    		if(bitRead(_status,ButtonACTUAL) && !bitRead(_status,ButtonLAST)) //If the button goes from LOW to HIGH
            	return true;     //Return true
        	else
            	return false;    //Otherwise return false
    	}
    }                            
    else
        return false;            //Otherwise return false
}



/*------------------------------------sustained method------------------------------------
||  Description: This method return true is the button is being continually pressed 
||	  otherwise return false.
||
||	Syntax: sustained()
||
||	Arguments: No arguments.
||
||	Return: True until you release the button.
||
||	See: update()
||		 updateAll() (defined in hardwareEnclosure.cpp)
*/
bool gButton::sustained()
{
	if(bitRead(_status,ButtonPULLUP)==0)
    	return bitRead(_status,ButtonACTUAL);
	else
		return !bitRead(_status,ButtonACTUAL);
}



/*-------------------------------------change method--------------------------------------
||  Description: This method return true if the status of the button has been changed, 
||	  that is to say if it pass from HIHG to LOW or vice versa.
||
||	Syntax: change()
||
||	Arguments: No arguments.
||
||	Return: True if the button pass from HIHG to LOW or vice versa.
||
||	See: update()
||		 updateAll() (defined in hardwareEnclosure.cpp)
*/
bool gButton::change()
{
    return bitRead(_status,ButtonCHANGE);
}



/*-----------------------------------clickChecker method----------------------------------
||  Description: This is a private function that is used in update() to check and set the 
||	  number of clicks.
||
||	Syntax: _clickCheck(currentTime)
||
||	Arguments: currentTime.-  The current time (the return of the millis() function)
||
||	Return: No return.
||
||	See: update()
||		 updateAll() (defined in hardwareEnclosure.cpp)
||		 setClickTime()
||		 getClickTime()
*/
void gButton::_clickCheck(unsigned long currentTime)
{
    if(!pressed())
    {
        if(currentTime - _lastPressTime > _timeBetweenClicks)     //If the button has not been pressed and time is running out
            _multiClickCount = 0;                                 //Reset the click count
    }
    else
    {
        if(_multiClickCount == 0)   //If its the first click
            _multiClickCount++;     //increase the click count
        else
        {
            if(currentTime - _lastPressTime <= _timeBetweenClicks)   //If click on time
                _multiClickCount++;                                  //increase the click count
            else                                                     //Otherwise
                _multiClickCount = 1;                                //counts as the first click
        }
        _lastPressTime = currentTime;   //Update the last Press Time
    }
}



/*-------------------------------------click method---------------------------------------
||  Description: This method will switch between true and false each time you press the 
||	  button. In other words, if you pressed one time the function will return true and 
||	  continues returning true until you pressed again and so on.
||
||	Syntax: click()
||
||	Arguments: No arguments.
||
||	Return: Switch between true and false each time you press the button.
||
||	See: pressed()
*/
bool gButton::click()
{
    if(pressed())      //If a press occurs
        bitWrite(_status,ButtonCLICK,(bitRead(_status,ButtonCLICK)?0:1));  //Switch between 1 and 0

    return bitRead(_status,ButtonCLICK);
}



/*----------------------------------releaseClick method-----------------------------------
||  Description: This method switch between true and false each time you release the 
||	  button. In other words, if you released one time the function will return true and 
||	  continues returning true until you released again and so on.
||
||	Syntax: click()
||
||	Arguments: No arguments.
||
||	Return: Switch between true and false each time you release the button.
||
||	See: released()
*/
bool gButton::releaseClick()
{
    if(released())    //If a release occurs
        bitWrite(_status,ButtonReleaseCLICK,(bitRead(_status,ButtonReleaseCLICK)?0:1));  //Switch between 1 and 0

    return bitRead(_status,ButtonReleaseCLICK);
}



/*------------------------------------multiPress method-----------------------------------
||  Description: This method return true if you press the button 'numberOfClicks' times 
||	  consecutively with 'timeBetweenClicks' milliseconds between each press, 
||	  timeBetweenClicks has a default value of 300 ms.
||
||	Syntax: multiPress(numberOfClicks)
||
||	Arguments: numberOfClicks(optional).-  The number of times that you must press the 
||										   button until the method return true.
||										     Default value: 2
||
||	Return: True if you press the button 'numberOfClicks' times before 'timeBetweenClicks'
||			  between each press.
||	
||  Notes: Since the methods multiPress and multiClick use and modify the same member
||		     variables we recommend using only the multiplePress or the multipleClick for 
||			 each button.
||
||         If you call the multiPress method multiple times only the one with the smallest
||		     argument or the first one will be able to return true, the others will return
||			 false no matter what.
||
||	See: _clickCheck()
||		 multiClick()
||       setClickTime()
||		 getClickTime()
*/
bool gButton::multiPress(uint8_t numberOfClicks)
{
    if (_multiClickCount == numberOfClicks)  //If _multiClickCount reach the numberOfClicks
    {
        _multiClickCount = 0;                //Reset the counter  
        return true;                         //and retur ture
    }
    else                                     //Otherwise
        return false;                        //Return false
}



/*----------------------------------multiClick method-------------------------------------
||  Description: This method switch between true and false each time you make a multiple 
||	  press similarly to the click method.
||
||	Syntax: multiClick(numberOfClicks)
||
||	Arguments: numberOfClicks(optional).-  The number of times that you must press the 
||										   button until the method switch between true
||										   and false.
||										     Default value: 2
||
||	Return: Switch between true and false each time you press the button 'numberOfClicks'
||			  times with 'timeBetweenClicks' milliseconds between each press.
||
||  Notes: Since the methods multiPress and multipleClick use and modify the same 
||	         variables members we recommend using only the multiPress or the multipleClick
||           for each button.
||
||         If you call the multiClick method with different arguments will be equivalent 
||		     of calling them with the smallest argument.
||         For example
||         		if(myButton.multiClick(5))
||     			{...}
||     			if(myButton.multiClick(3))
||     			{...}
||  	   Will be equivalent to
||        		if(myButton.multiClick(3))
||     			{...}
||     			if(myButton.multiClick(3))
||     			{...}
||
||	See: multiPress()
||       setClickTime()
||		 getClickTime()
*/
bool gButton::multiClick(uint8_t numberOfClicks)
{
    if(multiPress(numberOfClicks))   //If a multiplePress occurs
        bitWrite(_status,ButtonMultiCLICK,(bitRead(_status,ButtonMultiCLICK)?0:1));  //Switch between 1 and 0

    return bitRead(_status,ButtonMultiCLICK);
}



/*-------------------------------setDebounceTime method-----------------------------------
||	Description: Change the default debounce time used in the filter (50 ms).
||
||	Syntax: setDebounceTime(debounceTime)
||
||	Arguments: debounceTime.- A number between 0 and 255
||
||	Return: No return.
||
||	See: update()
||	     updateAll() (defined in hardwareEnclosure.cpp)
||		 getDebounceTime()
*/
void gButton::setDebounceTime(uint8_t debounceTime)
{	
	debounceTime = constrain(debounceTime,0,255);
	_debounceTime = debounceTime;
}



/*-------------------------------getDebounceTime method-----------------------------------
||	Description: Return the current value of the debounce time used in the filter.
||
||	Syntax: getDebounceTime()
||
||	Argument: No arguments
||
||	Return: A number between 0 and 255 representing the time in milliseconds of the 
||			  debounce filter.
||
||	See: update()
||	     updateAll()
||		 setDebounceTime()
*/
uint8_t gButton::getDebounceTime()
{
	return _debounceTime;
}



/*--------------------------------setClickTime method-------------------------------------   
||	Description: Change the default time between clicks used in the multiPress and 
||	  multiClick methods (300)
||
||	Syntax: setClickTime(clickTime)
||
||	Arguments: clickTime.- A number between 0 and 65,535
||
||	Return: No return.
||
||	Notes: The value of clickTime must be bigger than the debounce time used in the 
||           filter.
||
||	See: update()
||	     updateAll() (defined in hardwareEnclosure.cpp)
||		 _clickCheck()
||		 multiPress()
||		 multiClick()
||		 getClickTime()
||		 setDebounceTime()
||		 getDebounceTime()
*/
void gButton::setClickTime(uint16_t clickTime)
{	
	clickTime = constrain(clickTime,0,65535);
	_timeBetweenClicks = clickTime;
}



/*---------------------------------getClickTime method------------------------------------
||	Description: Return the current value of the allowed time between clicks used in the 
||	  multiPress and multiClick methods.
||
||	Syntax: getClickTime()
||
||	Argument: No arguments
||
||	Return: A number between 0 and 65,535 representing the time in milliseconds of the 
||			  time between clicks allowed
||
||	See: update()
||	     updateAll() (defined in hardwareEnclosure.cpp)
||		 _clickCheck()
||		 multiPress()
||		 multiClick()
||		 setClickTime()
*/
uint16_t gButton::getClickTime()
{
	return _timeBetweenClicks;
}
