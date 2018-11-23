#include<gLed.h>



/*-----------------------------_addressBook static member---------------------------------
||	Description: This is a vector that contains pointers to all gLed objects in the
||    sketch.
*/
gLed** gLed:: _addressBook = new gLed*[1];



/*--------------------------------_size static member-------------------------------------
||	Description: The size of the address book, this is the number of gLed objects in the
||	  sketch.
*/
uint8_t gLed:: _size = 0;



/*----------------------------------------Constructor-------------------------------------
||	Description: The constructor set the initial values of all variables.
||
||	Syntax: gLed myLed(pinNumber)
||
||	Params: pinNumber.- The Arduino pin in which the led will be connected.
||
||	Return: No return.
*/
gLed::gLed(uint8_t pinNumber)
{
	_pin = pinNumber;
	_status = LedOFF;
	_lastTime = 0;
	_timeAux = 0;
	_period = 0;
	_maxBright = 255;
	_minBright = 0;
	_lastMethod = M_restart;
	_blinks = 0;
	_counts = 0;
	
	_includeItem(this);       //Include this object in the address book
}



/*----------------------------------------Destructor--------------------------------------
||	Description: Return the memory used in the address book for this object.
||
||	Syntax: n/a
||
||	Params: No params.
||
||	Return: No return.
||
||	See: _includeItem()
*/
gLed::~gLed()
{
	gLed** temporalContainer = 0;          //Temporal container
	gLed::_size--;						   //Decrement the _size variable
	if(gLed::_size == 0)                   //If there is no objects
		gLed::_addressBook[0] = 0;		   //Set the address book to NULL
	else								   //Is there is more than one
	{
		temporalContainer = gLed::_addressBook;            //Copy the existent data in the container
		gLed::_addressBook = new gLed*[gLed::_size];       //Allocates new memory
		uint8_t auxIndex = 0;                              //Auxiliar variable
		for(uint8_t i=0;i<gLed::_size+1;i++)
		{
			if(temporalContainer[i] != this)              //Copy the existing data in the address book excluding this object
				gLed::_addressBook[auxIndex++] = temporalContainer[i];
		}
		delete[] temporalContainer;                        //Delete the old allocated memory
		temporalContainer = 0;                             //and just for safe set the container to 0
			
	}
	digitalWrite(_pin,LOW);                //Turn off the led
}



/*--------------------------------------begin method--------------------------------------
||	Description: Set the correct pin mode for the led. This method needs to be used at 
||	  least one time on the setup function.
||
||	Params: No params.
||
||	Return: No return.
||
||	See: constructor
||		 beginLeds()
*/
void gLed::begin()
{
	pinMode(_pin,OUTPUT);
}



/*-------------------------------------update method--------------------------------------
||	Description: Updates the timers, brightness and status of the led, this method needs
||	  to be called at least one time in the main loop function, after any ("long") delay 
||	  or at the top of any user loop (for, while, do while) if the led is used inside that
||	  loop.
||
||	Syntax: update()
||
||	Params: No params.
||
||	Return: No return.
||
||	See: updateLeds()
||	 	 ledOn()
||	     ledOff()
||		 blink()
||		 blinkFade()
||		 fadeIn()
||		 fadeOut()
*/
void gLed::update()
{
	switch(_status)                 //Check the status
	{
		case LedOFF:
			digitalWrite(_pin,LOW);        //Turn off the led
			break;
			
		case LedON:
			digitalWrite(_pin,HIGH);       //Turn on the led
			break;
			
		case LedPWM:
			analogWrite(_pin,_maxBright);  //Set the brightness of the led
			break;
			
		case LedBlinkON:   //High part of the blink method
			analogWrite(_pin,_maxBright);         //Set the brightness to _maxBright
			if(millis() - _lastTime >= _timeAux)  //If the time is over
			{
				_lastTime = millis();             //Update the _lastTime variable
				_status = LedBlinkOFF;            //Change the status to the next case
			}
			break;
			
		case LedBlinkOFF:  //Low part of the blink method
			analogWrite(_pin,_minBright);         //Set the brightness to _minBright
			if(millis() - _lastTime > _period - _timeAux) //If the time is over
			{
				_lastTime = millis();             //Update the _lastTime variable
				_status = LedBlinkON;             //Change the status to the previous case
			}
			break;
			
		case LedFadeBlinkIN:    //Rising part of the blinkFade method 
		{
			float _a = 1.0*(_maxBright - _minBright)/_timeAux;    //Slop of the brightness
			analogWrite(_pin,_a*(millis()-_lastTime)+_minBright); //Gradually increase the brightness from _minBright to _maxBright with the slope _a
			if(millis() - _lastTime >= _timeAux)                  //If the time is over
			{
				_lastTime = millis();                            //Update the _lastTime variable
				_status = LedFadeBlinkOUT;                       //Change the status to the next case
			}
		}
			break;
			
		case LedFadeBlinkOUT:   //Falling part of the fadeBlink method 
		{
			float _b = 1.0*(_minBright - _maxBright)/(_period-_timeAux); //Slope of the brightness
			analogWrite(_pin,_b*(millis() - _lastTime)+_maxBright);      //Gradually decrease the brightness from _maxBright to _minBright with the slope _b
			if(millis() - _lastTime > _period - _timeAux)                //If the time is over
			{
				_lastTime = millis();                                    //Update the _lastTime variable
				_status = LedFadeBlinkIN;                                //Change the status to the previous case
			}
		}
			break;
			
		case LedFadeIN:     //FadeIn method
		{
			float _c = 1.0*(_maxBright - _minBright)/_period;       //Slope of the brightness
			analogWrite(_pin,_c*(millis() - _lastTime)+_minBright); //Gradually increase the brightness from _minBright to _maxBright
			if(millis() - _lastTime >= _period)                     //If the time is over
			{
				_status = LedPWM;                                   //Set the brightness to _maxBright permanently
			}
 		}
			break;
			
		case LedFadeOUT:    //FadeOut method
		{
			float _d = 1.0*(_minBright - _maxBright)/_period;       //Slope of the brightness
			analogWrite(_pin,_d*(millis() - _lastTime)+_maxBright); //Gradually decrease the brightness from _maxBright to _minBright
			if(millis() - _lastTime >= _period)                     //If the time is over
			{ 
				analogWrite(_pin,_minBright);                       //Set the brightness to _minBright permanently
				_status = M_restart;                                //Restart the status 
			}
		}
			break;
			
		case LedTimeON:     //Turn on the led after after a period of time
			if(millis() - _lastTime >= _period) //If the time is over
				_status = LedPWM;               //Change the status to LedPWM
			break;
			
		case LedTimeOFF:    //Turn down the led
			if(millis() - _lastTime >= _period) //If the time is over
				_status = LedOFF;               //Change the status to LedOFF
			break;
			
		case LedMultiBlinkON:	//Blinking part of the multiBlink method
			if(millis() - _lastTime <= _timeAux/(2*_blinks-1))  //If the high part is active
				analogWrite(_pin,_maxBright);         //Set the brightness to _maxBright
			else                                      //Otherwise
			{            
				analogWrite(_pin,_minBright); 		  //Set the brightness to _minBright
				if(millis() - _lastTime >= 2*_timeAux/(2*_blinks-1)) //If the time is over
				{
					_lastTime = millis();	//Update the _lastTime variable
					_counts++;              //Increase the blink count 
				}
			}
				
			if(_counts == _blinks)          //If the blink part is over
			{
				_status = LedMultiBlinkOFF; //Change the status to LedMultiBlinkOFF
				_lastTime = millis();       //Update the _lastTime variable
			}
			break;
			
		case LedMultiBlinkOFF:	//Static part of the multiBlink method
			analogWrite(_pin,_minBright);         //Set the brightness to _minBright
			if(millis() - _lastTime > _period - _timeAux*2*_blinks/(2*_blinks-1)) //If the time is over
			{
				_lastTime = millis();             //Update the _lastTime variable
				_status = LedMultiBlinkON;        //Change the status to the previous case
				_counts = 0;
			}
			break;
			
		case LedPulseDELAY:		//Delay part of the pulse method
			analogWrite(_pin,_minBright);         //Set the brightness of the led to _minBright
			if(millis() - _lastTime >= _timeAux)  //If the delay is over
			{
				_status = LedPulseON;             //Change the status to LedPulseON
				_lastTime = millis();             //Update the _lastTime variable
			}
			break;
			
		case LedPulseON:		//Active part of the pulse method
			analogWrite(_pin,_maxBright);			//Set the brightness of the led to _maxBright
			if(millis() - _lastTime >= _period)     //If the pulse is over
			{
				analogWrite(_pin,_minBright);       //Set the brightness of the led to _minBright
				_status = M_restart;			    //Restart the status
				_lastMethod = M_restart;            //Restart the last method used
			}
			break;
			
		case LedMultiPulseDELAY:	//Delay part of the multiPulseMethod
			analogWrite(_pin,_minBright);			//Set the brightness of the led to _minBright
			if(millis() - _lastTime >= _timeAux)	//If the delay is over
			{
				_status = LedMultiPulseON;			//Change the status to LedMultiPulseON
				_lastTime = millis();				//Update the _lastTime variable
			}
			break;
			
		case LedMultiPulseON:		//Active part of the multiPulse method
			if(millis() - _lastTime <= _period/(_blinks+0.01*_counts2-1)*0.01*_counts2)	//If the high part is active 
				analogWrite(_pin,_maxBright);						 //Set the brightness to _maxBright
			else													 //Otherwise
			{
				analogWrite(_pin,_minBright);                        //Set the brightness to _minBright
				if(millis() - _lastTime >= _period/(_blinks+0.01*_counts2-1))  			//If the time is over
				{
					_counts++;                                       //Increment the _counts variable
					_lastTime = millis();                            //Update the _lastTime variable 
				}
			}
			if(_counts == _blinks)                  //If the number of pulses reach _blinks
			{
				_status = M_restart;                //Restart the status
				_lastMethod = M_restart;            //Restart the last method used
			}
			break;
			
		case LedPulseFadeDELAY:		//Delay part of the pulseFade method
			analogWrite(_pin,_minBright);			//Set the brightness of the led to _minBright
			if(millis() - _lastTime >= _timeAux)	//If the delay is over
			{
				_status = LedPulseFadeON;			//Change the status to LedPulseFadeON
				_lastTime = millis();				//Update the _lastTime variable
			}
			break;
		
		case LedPulseFadeON:		//Active part of the pulseFade method
			{
				float _e = 1.0*(_maxBright - _minBright)/(0.01*_period*_counts);	//Slope of the brightness
				if(millis() - _lastTime <= 0.01*_period*_counts)				 	//If the pulse is active
					analogWrite(_pin,_e*(millis() - _lastTime) + _minBright);	 	//Increase the brightness
				else																//otherwise 
				{
					_e = 1.0*(_minBright - _maxBright)/(_period*(1.0-0.01*_counts)); 			   //Change the slope
					analogWrite(_pin,_e*(millis() - _lastTime-0.01*_period*_counts) + _maxBright); //Decrease the brigness
					if(millis() - _lastTime >= _period)								//If the time is over
					{
						analogWrite(_pin,_minBright);								//Set the brightness to _minBright
						_status = M_restart;										//Restar the status
						_lastMethod = M_restart;									//Restart the last method used
					}
				}
			}			
			break;
			
		case LedMultiPulseFadeDELAY:	//Delay part of the multiPulseFade method
			analogWrite(_pin,_minBright);			//Set the brightness of the led to _minBright
			if(millis() - _lastTime >= _timeAux)	//If the delay is over
			{
				_status = LedMultiPulseFadeON;	    //Change the status to LedMultiPulseFadeON
				_lastTime = millis();				//Update the _lastTime variable
			}
			break;
		
		case LedMultiPulseFadeON:	//Active part of the multiPulseFade method
		{
			float _f = 1.0*(_maxBright - _minBright)/(0.01*_period/_blinks*_counts);	//Slope of the brightness
			if(millis() - _lastTime <= 0.01*_period/_blinks*_counts)		 			//If the high part is active 
				analogWrite(_pin,_f*(millis() - _lastTime) + _minBright);	 			//Increase the brightness
			else													 					//Otherwise
			{
				_f = 1.0*(_minBright - _maxBright)/(_period/_blinks*(1.0-0.01*_counts)); //Change the slope
				analogWrite(_pin,_f*(millis() - _lastTime - 0.01*_period/_blinks*_counts) + _maxBright); //Decrease the brigness
				if(millis() - _lastTime >= _period/_blinks)  		  //If the time is over
				{
					_counts2++;                                       //Increment the _counts variable
					_lastTime = millis();                             //Update the _lastTime variable 
				}
			}
			if(_counts2 == _blinks)                  				 //If the number of pulses reach _blinks
			{
				_status = M_restart;                				 //Restart the status
				_lastMethod = M_restart;            				 //Restart the last method used
			}
		}
			break;
			
		case LedMultiBlinkFadeON:	//Blinking part of the multiBlinkFade method
		{
			float _g=1.0*(_maxBright - _minBright)/_timeAux*_blinks*2;  //Slope of the brightness
			if(millis() - _lastTime <= _timeAux/(_blinks*2))  			//If the rising part is active
				analogWrite(_pin,_g*(millis()-_lastTime)+_minBright);   //Increase the brightness
			else                                      					//Otherwise
			{       
				_g*=-1;													//Change the slope  
				analogWrite(_pin,_g*(millis()-_lastTime)+_maxBright); 	//Set the brightness to _minBright
				if(millis() - _lastTime >= 2*_timeAux/(_blinks*2))      //If the time is over
				{
					_lastTime = millis();	    //Update the _lastTime variable
					_counts++;                  //Increase the blink count 
				}
			}
				
			if(_counts == _blinks)              //If the blink part is over
			{
				_status = LedMultiBlinkFadeOFF; //Change the status to LedMultiBlinkFadeOFF
				_lastTime = millis();           //Update the _lastTime variable
			}
		}
			break;
			
		case LedMultiBlinkFadeOFF:	//Static part of the multiBlinkFade method
			analogWrite(_pin,_minBright);                 //Set the brightness to _minBright
			if(millis() - _lastTime > _period - _timeAux) //If the time is over
			{
				_lastTime = millis();             //Update the _lastTime variable
				_status = LedMultiBlinkFadeON;    //Change the status to the previous case
				_counts = 0;
			}
			break;
	}
}



/*-------------------------------------ledOn method---------------------------------------
||	Description: Turn on the led after a short delay.
||
||	Syntax: ledOn(bright,duration)
||
||	Params: bright(optional).-  The brightness of the led, 0 set the led totally off and 
||								100 set the led totally on.
||								  Default value: 100.
||
||			delay(optional).-   The delay in milliseconds after which the led turn on.
||								  Default value: 0 (instantly)
||
||	Return: No return.
||
||	See: update()
||		 updateLeds()
*/
void gLed::ledOn(uint8_t bright, uint16_t delay)
{
	//Check for changes in the parameters.
	if(delay != _period || _maxBright != map(constrain(bright,0,100),0,100,0,255))
		_lastMethod = M_restart;    //Restar the las method.
		
	if(_lastMethod != M_ledOn)      //If the method was not called with the same parameters
	{
		_lastMethod = M_ledOn;                                  //Update the las method used 
		_maxBright = map(constrain(bright,0,100),0,100,0,255);  //Map bright to be used in analogWrite
		_status = LedTimeON;                                    //Update the status
		_period = delay;                                        //Save the duration of the animation
		_lastTime = millis();                                   //Update the _lastTime variable
	}
}



/*-------------------------------------ledOff method--------------------------------------
||	Description: Turn the led off after a short delay.
||
||	Syntax: ledOff(duration)
||
||	Params: delay(optional).-  The delay in milliseconds after which the led turn off.
||							     Default value: 0 (instantly)
||
||	Return: No return.
||
||	See: update()
||		 updateLeds()
*/
void gLed::ledOff(uint16_t delay)
{
	if(delay != _period)         //Check for changes in the parameters
		_lastTime = M_restart;   //Restart the last method
		
		
	if(_lastMethod != M_ledOff)  //If the method was not called with the same parameters
	{
		_lastMethod = M_ledOff;  //Update the last method used
		_status = LedTimeOFF;    //Update the status
		_period = delay;         //Save the duration of the animation
		_lastTime = millis();    //Update the _lastTime variable
	}
}



/*-------------------------------------blink method---------------------------------------
||	Description: Perform a blink animation without the use of the delay function.
||
||	Syntax: blink(duration,duty,maxBright,minBright)
||
||	Params: duration.-             The duration in milliseconds of one cycle of the
||								   animation, this is the sum of the time that the led
||								   will spend on and the time that the led will spend off.
||
||			duty(optional).-       The duty cycle of the animation, a number between 0 and
||								   100 representing the percentage of the time that the
||								   led will spend on with respect the entire duration of
||								   the animation.
||								   e.g. If duration = 1000 and duty = 50 the led will
||								   spend 500ms on and 500ms off.
||								   e.g. If duration = 1000 and duty = 20 the led will
||								   spend 200ms on and 800ms off.
||								     Default value: 50.
||
||			maxBright(optional).-  The maximum bright of the led in the high part, a 
||								   number between 0 and 100.
||								     Default value: 100.
||
||			minBright(optional).-  The minimum bright of the led in the low part, a number
||								   between 0 and 100.
||								     Default value: 0.
||
||	Return: No return.
||
||	See: update()
||		 updateLeds()
*/
void gLed::blink(uint16_t duration, uint8_t duty, uint8_t maxBright, uint8_t minBright)
{
	//Check for changes in the parameters 
	if(duration != _period || _timeAux != 0.01*duration*duty || _maxBright != map(constrain(maxBright,0,100),0,100,0,255) || _minBright != map(constrain(minBright,0,100),0,100,0,255))
		_lastMethod = M_restart;   //Restar the last method used

	if(_lastMethod != M_blink)     //If the method was not called with the same parameters
	{
		_lastMethod = M_blink;                                    //Update the last method used
		_maxBright = map(constrain(maxBright,0,100),0,100,0,255); //Map the maxBright to be used in analogWrite
		_minBright = map(constrain(minBright,0,100),0,100,0,255); //Map the minBright to be used in analogWrite
		duty = constrain(duty,0,100);                             //Make sura that the duty is between 0 and 100
		switch(duty)                           //Check the duty value
		{
			case 0:                            //If its 0
				_maxBright = _minBright;       //Exchange the max and min brightness
				_status = LedPWM;              //Change the status to LedPWM
				break;	
			case 100:                          //If its 100
				_status = LedPWM;              //Change the status to LedPWM
				break;
			default:                           //If its a value in between
				_status = LedBlinkON;          //Change the status to LedBlinkON
				_period = duration;            //Save the duration of the animation
				_lastTime = millis();          //Update the _lastTime variable
				_timeAux = 0.01*duration*duty; //Save the high part of the animation
				break;
		}
	}
}



/*-------------------------------------blinkFade method-----------------------------------
||	Description: Perform a fade in/out animation without the use of the delay function,
||	  only available for PWM pins.
||
||	Syntax: blinkFade(duration,duty,maxBright,minBright)
||
||	Params: duration.-             The duration in milliseconds of one cycle of the
||								   animation, this is the sum of the time that the led
||								   will spend increasing its brightness and the time that 
||								   the led will spend decreasing its brightness.
||
||			duty(optional).-       The duty cycle of the animation, a number between 0 and
||								   100 representing the percentage of the time that the
||								   led will spend increasing its brightness with respect 
||								   the entire duration of the animation.
||								   e.g. If duration = 1000 and duty = 50 the led will
||								   spend 500ms increasing its brightness and 500ms
||								   decreasing its brightness.
||								   e.g. If duration = 1000 and duty = 20 the led will
||								   spend 200ms increasing its brightness and 800ms
||								   decreasing its brightness.
||								     Default value: 50.
||
||			maxBright(optional).-  The maximum bright of the led, a number between 0 and 
||								   100.
||								     Default value: 100.
||
||			minBright(optional).-  The minimum bright of the led, a number between 0 and 
||								   100.
||								     Default value: 0.
||
||	Return: No return.
||
||	See: update()
||		 updateLeds()
*/
void gLed::blinkFade(uint16_t duration, uint8_t duty, uint8_t maxBright, uint8_t minBright)
{
	//Check for changes in the parameters 
	if(duration != _period || _timeAux != 0.01*duration*constrain(duty,0,100) || _maxBright != map(constrain(maxBright,0,100),0,100,0,255) || _minBright != map(constrain(minBright,0,100),0,100,0,255))
		_lastMethod = M_restart;   //Restart the last method used
		
	if(_lastMethod != M_fadeBlink) //If the method was not called with the same parameters 
	{
		_lastMethod = M_fadeBlink;								  //Update the las method used
		_maxBright = map(constrain(maxBright,0,100),0,100,0,255); //Map maxBright to be used in analogWrite
		_minBright = map(constrain(minBright,0,100),0,100,0,255); //Map minBright to be used in analogWrite
		duty = constrain(duty,0,100); 						      //Make sure that the duty is between 0 and 100
		_status = LedFadeBlinkIN;     //Set the status to LedFadeBlinkIN
		_period = duration;           //Save the duration of the animation
		_lastTime = millis();         //Update the _lastTime variable
		_timeAux = 0.01*duration*duty;//Save the increasing part of the animation
	}
}



/*---------------------------------------FadeIn method------------------------------------
||	Description: This method perform a fade in animation without the use of the delay 
||    function, only available for PWM pins.
||
||	Syntax: fadeIn(duration,maxBright,minBright)
||
||	Params: duration.-             The duration in milliseconds of the animation.
||
||			maxBright(optional).-  The final bright of the led at the end of the 
||								   animation, a number between 0 and 100.
||								     Default value: 100.
||
||			minBright(optional).-  The initial bright of the led at the start of the 
||								   animation, a number between 0 and 100.
||								     Default value: 0.
||
||	Return: No return.
||
||	See: update()
||		 updateLeds()  
*/
void gLed::fadeIn(uint16_t duration, uint8_t maxBright, uint8_t minBright)
{
	//Check for changes in the parameters
	if(duration != _period || _maxBright != map(constrain(maxBright,0,100),0,100,0,255) || _minBright != map(constrain(minBright,0,100),0,100,0,255))
		_lastMethod  = M_restart;	//Restar the last method used 
		
	if(_lastMethod != M_fadeIn)     //If the method was not called with the same parameters
	{
		_lastMethod = M_fadeIn;     							   //Update the last method used
		_maxBright = map(constrain(maxBright,0,100),0,100,0,255);  //Map maxBright to be used in analogWrite
		_minBright = map(constrain(minBright,0,100),0,100,0,255);  //Map minBright to be used in analogWrite
		_period = duration; 									   //Save the duration of the animation
		_status = LedFadeIN;									   //Update the status
		_lastTime = millis();									   //Update the _lastTime variables
	}
}



/*--------------------------------------FadeOut method------------------------------------
||	Description: This method perform a fade out animation without the use of the delay 
||    function, only available for PWM pins.
||
||	Syntax: fadeOut(duration,maxBright,minBright)
||
||	Params: duration.-             The duration in milliseconds of the animation.
||
||			maxBright(optional).-  The initial bright of the led at the start of the 
||								   animation, a number between 0 and 100.
||								     Default value: 100.
||
||			minBright(optional).-  The final bright of the led at the end of the 
||								   animation, a number between 0 and 100.
||								     Default value: 0.
||
||	Return: No return.
||
||	See: update()
||		 updateLeds() 
*/
void gLed::fadeOut(uint16_t duration, uint8_t maxBright, uint8_t minBright)
{
	//Check for changes in the parameters 
	if(duration != _period || _maxBright != map(constrain(maxBright,0,100),0,100,0,255) || _minBright != map(constrain(minBright,0,100),0,100,0,255))
		_lastMethod = M_restart;	//Restar the last method used 
		
	if(_lastMethod != M_fadeOut)    //If the method was not called with the same parameters 
	{
		_lastMethod = M_fadeOut;								   //Update the last method used
		_maxBright = map(constrain(maxBright,0,100),0,100,0,255);  //Map maxBright to be used in analogWrite
		_minBright = map(constrain(minBright,0,100),0,100,0,255);  //Map minBright to de used in analogWrite
		_period = duration;										   //Saves the duration of the animation
		_status = LedFadeOUT;									   //Update the status
		_lastTime = millis(); 									   //Update the _lastTime variable 
	}	
}



/*-------------------------------------multiBlink method----------------------------------
||	Description: Blink the led a number of times and wait a short delay.
||
||	Syntax: multiBlink(duration,blinks,duty,maxBright,minBright)
|| 
||	Params: duration.-             The duration in milliseconds of the animation, this is 
||								   the sum of the time that the led will be blinking and 
||								   the time that the led will be off.
||
||			blinks(optional).-     The number of blinks
||								     Default value: 2
||
||			duty(optional).-       The duty cycle of the animation, a number between 0 and
||								   100 representing the percentage of the time that the
||								   led will be blinking with respect the entire duration 
||								   of the animation.
||								   e.g. If duration = 1000 and duty = 50 the led will
||								   blink 500ms and be off 500ms.
||								   e.g. If duration = 1000 and duty = 20 the led will
||								   blink 200ms and be off 800ms.
||								     Default value: 50.       
||
||			maxBright(optional).-  Maximum bright of the led, a number between 0 and 100.
||								     Default value: 100.
||
||			minBright(optional).-  Minimun bright of the led, a number between 0 and 100.
||								     Default value: 0.
||
||	Return: No return
||
||	See: update()
||		 updateLeds()
*/
void gLed::multiBlink(uint16_t duration, uint8_t blinks, uint8_t duty, uint8_t maxBright, uint8_t minBright)
{
	//Check for changes in the parameters
	if(duration != _period || _timeAux != 0.01*duration*constrain(duty,0,100) || _maxBright != map(constrain(maxBright,0,100),0,100,0,255) || _minBright != map(constrain(minBright,0,100),0,100,0,255) || _blinks != blinks)
		_lastMethod = M_restart;	//Restar the last method used 
		
	if(_lastMethod != M_multiBlink)	//If the method was not called with the same parameters
	{
		_lastMethod = M_multiBlink; 							  //Update the last method used
		_maxBright = map(constrain(maxBright,0,100),0,100,0,255); //Map maxBright to be used in analogWrite
		_minBright = map(constrain(minBright,0,100),0,100,0,255); //Map minBright to be used in analogWrite
		duty = constrain(duty,0,100); 						      //Make sure that the duty is between 0 and 100
		switch(duty)
		{
			case 0:                           //If its 0
				_maxBright = _minBright;      //Exchange the min and max brightness
				_status = LedPWM;             //Set the status to LedPWM
				break;
			 
			case 100:                         //If its 100
				_status = LedPWM;			  //Set the status to LedPWM
				break;
			
			default: 
				_status = LedMultiBlinkON;     //Set the status to LedFadeBlinkIN
				_period = duration;            //Save the duration of the animation
				_lastTime = millis();          //Update the _lastTime variable
				_timeAux = 0.01*duration*duty; //Save the increasing part of the animation
				_blinks = blinks;              //Save the number of blinks
				_counts = 0;                   //Number of blinks already performed
				break;
		}
		
	}
}



/*------------------------------------pulse method----------------------------------------
||	Description: Cause a single pulse of certain duration after a short delay.
||
||	Syntax: pulse(duration,delay,maxBright,minBright)
||
||	Params: duration.-             The duration of the pulse in milliseconds.
||
||			delay(optional).-      The time in milliseconds after wich the pulse occur.
||								     Default value: 0 (instantly)
||
||			maxBright(optional).-  The maximum bright of the led in the pulse, a number 
||								   between 0 and 100.
||								     Default value: 100.
||
||			minBright(optional).-  The minimum bright of the led after the pulse, a number
||								   between 0 and 100.
||								     Default value: 0.
||
||	Return: No return.
||
||	See: update()
||		 updateLeds()
*/
void gLed::pulse(uint16_t duration, uint16_t delay, uint8_t maxBright, uint8_t minBright)
{
	//Check for changes in the parameters
	if(_period != duration || _timeAux != delay || _maxBright != map(constrain(maxBright,0,100),0,100,0,255) || _minBright != map(constrain(minBright,0,100),0,100,0,255))
		_lastMethod = M_restart;	//Restart the las method used
		
	if(_lastMethod != M_pulse)		//If the method was not called with the same parameters
	{
		_lastMethod = M_pulse;									  //Update the last method used
		_maxBright = map(constrain(maxBright,0,100),0,100,0,255); //Map maxBright to be used in analogWrite
		_minBright = map(constrain(minBright,0,100),0,100,0,255); //Map minBright to be used in analogWrite
		_period = duration;										  //Save the duration of the pulse
		_timeAux = delay;										  //Save the duration of the delay
		_lastTime = millis();                                     //Update the _lastMethod used
		if(delay > 0)                                             //If there is a delay
			_status = LedPulseDELAY;                              //Set the status to LedPulseDELAY
		else     												  //Otherwise 
			_status = LedPulseON;								  //Set the status to LedPulseON
	}
}



/*-------------------------------------multiPulse method----------------------------------
||	Description: Cause a multiple pulse of certain duration after a short delay.
||
||	Syntax: multiPulse(duration,pulses,delay,maxBright,minBright)
||
||	Params: duration.-             The duration of the pulses in milliseconds, this is not
||								   the duration of each pulse, is the sum of the time of 
||								   all pulses.
||
||			pulses(optional).-     The number of pulses that the led will make in 
||								   'duration' milliseconds.
||							         Default value: 2.
||
||			delay(optional).-      The time in milliseconds after wich the pulses occur.
||								     Default value: 0 (instantly)   
||
||
||			duty(optional).-       The duty cycle of the pulse, a number between 0 and 100
||								   representing the percentage of the time that the led 
||								   will spend on with respect the entire duration of the 
||								   pulse.
||								     Default value: 50.
||
||			maxBright(optional).-  The maximum bright of the led in the pulse, a number 
||								   between 0 and 100.
||								     Default value: 100.
||
||			minBright(optional).-  The minimum bright of the led after the pulse, a number
||								   between 0 and 100.
||								     Default value: 0.
||
||	Return: No return.
||
||	See: update()
||		 updateAll()
*/
void gLed::multiPulse(uint16_t duration, uint8_t pulses, uint16_t delay, uint8_t duty, uint8_t maxBright, uint8_t minBright)
{
	//Check for changes in the parameters
	if(_period != duration || _blinks != pulses || _counts2 != duty || _timeAux != delay || _maxBright != map(constrain(maxBright,0,100),0,100,0,255) || _minBright != map(constrain(minBright,0,100),0,100,0,255))
		_lastMethod = M_restart;		//Restart the las method used
	
	if(_lastMethod != M_multiPulse)		//If the method was not called with the same parameters 
	{
		_lastMethod = M_multiPulse;									//Update the last method used
		_period = duration; 										//Save the duration of the pulses
		_blinks = pulses;											//Save the number of pulses
		_timeAux = delay;											//Save the duration of the delay
		_maxBright = map(constrain(maxBright,0,100),0,100,0,255);   //Map maxBright to be used in analogWrite
		_minBright = map(constrain(minBright,0,100),0,100,0,255);   //Map minBright to be used in analogWrite
		_counts = 0;												//Number of pulses already performed
		_counts2 = duty;											//Here _counts2 is uses as an auxiliar variable to store the duty cycle
		_lastTime = millis(); 										//Update the _lastTime variable
		if(delay > 0)												//If there is a delay
			_status = LedMultiPulseDELAY;                           //Change the status to LedMultiPulseDELAY
		else														//Otherwise 
			_status = LedMultiPulseON;								//Change the status to LedMultiPulseON
	}
}



/*-----------------------------------pulseFade method-------------------------------------
||	Description: Cause a single pulse with a fade in and out effect after a short delay.
||
||	Syntax: pulseFade(duration,delay,maxBright,minBright)
||
||	Params: duration.-             The duration of the pulse in milliseconds.
||
||			delay(optional).-      The time in milliseconds after wich the pulse occur.
||								     Default value: 0 (instantly)
||
||			duty(optional).-       The duty cycle of the pulse, a number between 0 and 100
||								   representing the percentage of the time that the led 
||								   will spend increasing its brightness with respect 
||								   the entire duration of the pulse.
||								   e.g. If duration = 1000 and duty = 50 the led will
||								   spend 500ms increasing its brightness and 500ms
||								   decreasing its brightness.
||								   e.g. If duration = 1000 and duty = 20 the led will
||								   spend 200ms increasing its brightness and 800ms
||								   decreasing its brightness.
||								     Default value: 50.     
||
||			maxBright(optional).-  The maximum bright that the led will reach, a number 
||								   between 0 and 100.
||								     Default value: 100.
||
||			minBright(optional).-  The minimum bright of the led after the pulse, a number
||								   between 0 and 100.
||								     Default value: 0.
||
||	Return: No return.
||
||	See: update()
||		 updateLeds()
*/
void gLed::pulseFade(uint16_t duration, uint16_t delay, uint8_t duty, uint8_t maxBright, uint8_t minBright)
{	
	//Check for changes in the parameters
	if(_period != duration || _timeAux != delay || _counts != constrain(duty,0,100) || _maxBright != map(constrain(maxBright,0,100),0,100,0,255) || _minBright != map(constrain(minBright,0,100),0,100,0,255))
		_lastMethod = M_restart;		//Restart the last method used
	
	if(_lastMethod != M_pulseFade)      //If the method was not called with the same parameters
	{
		_lastMethod = M_pulseFade;          						//Update the last method used
		_period = duration;											//Save the duration of the pulse
		_timeAux = delay;											//Save the duration of the delay
		_counts = constrain(duty,0,100);							//_counts is used here as an auxiliar variable to save the duty cycle of the pulse
		_maxBright = map(constrain(maxBright,0,100),0,100,0,255);	//Map maxBright to be used in analogWrite
		_minBright = map(constrain(minBright,0,100),0,100,0,255);	//Map minBright to be used in analogWrite
		_lastTime = millis();										//Update the _lastTime variable
		if(delay > 0)												//If there is a delay
			_status = LedPulseFadeDELAY;							//Change the status to LedPulseFadeDELAY
		else														//Otherwise
			_status = LedPulseFadeON;								//Change the status to LedPulseFadeON
	}
}



/*-------------------------------------multiPulseFade method------------------------------
||	Description: Cause a multiple pulse with a fade in and out effect.
||
||	Syntax: multiPulseFade(duration,pulses,delay,duty,maxBright,minBright)
||
||	Params: duration.-             The duration of the pulses in milliseconds, this is not
||								   the duration of each pulse, is the sum of the time of 
||								   all pulses.
||
||			pulses(optional).-     The number of pulses that the led will make in 
||								   'duration' milliseconds.
||							         Default value: 2.
||
||			duty(optional).-       The duty cycle of the pulse, a number between 0 and 100
||								   representing the percentage of the time that the led 
||								   will spend increasing its brightness with respect 
||								   the entire duration of the pulse.
||								     Default value: 50.        
||
||			delay(optional).-      The time in milliseconds after wich the pulses occur.
||								     Default value: 0 (instantly)   
||
||			maxBright(optional).-  The maximum bright that the led will reach, a number 
||								   between 0 and 100.
||								     Default value: 100.
||
||			minBright(optional).-  The minimum bright of the led after the pulse, a number
||								   between 0 and 100.
||								     Default value: 0.
||
||	Return: No return.
||
||	See: update()
||		 updateAll()
*/
void gLed::multiPulseFade(uint16_t duration, uint8_t pulses, uint16_t delay, uint8_t duty, uint8_t maxBright, uint8_t minBright)
{
	//Check for changes in the parameters
	if(_period != duration || _blinks != pulses || _timeAux != delay || _counts != constrain(duty,0,100) || _maxBright != map(constrain(maxBright,0,100),0,100,0,255) || _minBright != map(constrain(minBright,0,100),0,100,0,255))
		_lastMethod = M_restart;		//Restart the last method used
		
	if(_lastMethod != M_multiPulseFade)
	{
		_lastMethod = M_multiPulseFade;          					//Update the last method used
		_period = duration;											//Save the duration of the pulse
		_blinks = pulses;											//Save the number of pulses
		_timeAux = delay;											//Save the duration of the delay
		_counts = constrain(duty,0,100);							//_counts is used here as an auxiliar variable to save the duty cycle of the pulse
		_maxBright = map(constrain(maxBright,0,100),0,100,0,255);	//Map maxBright to be used in analogWrite
		_minBright = map(constrain(minBright,0,100),0,100,0,255);	//Map minBright to be used in analogWrite
		_counts2 = 0;												//Number of pulses already performed
		_lastTime = millis();										//Update the _lastTime variable
		if(delay > 0)												//If there is a delay
			_status = LedMultiPulseFadeDELAY;						//Change the status to LedMultiPulseFadeDELAY
		else														//Otherwise
			_status = LedMultiPulseFadeON;							//Change the status to LedMultiPulseFadeON
	}
}



/*-----------------------------------multiBlinkFade method--------------------------------
||	Description: Blink the led a number of time with a fade in and out effect and wait a 
||	  short delay to start again.
||
||	Syntax: multiBlinkFade(duration,blinks,duty,maxBright,minBright)
|| 
||	Params: duration.-             The duration in milliseconds of the animation, this is 
||								   the sum of the time that the led will be blinking and 
||								   the time that the led will be off.
||
||			blinks(optional).-     The number of blinks
||								     Default value: 2
||
||			duty(optional).-       The duty cycle of the animation, a number between 0 and
||								   100 representing the percentage of the time that the
||								   led will be blinking with respect the entire duration 
||								   of the animation.
||								   e.g. If duration = 1000 and duty = 50 the led will
||								   blink 500ms and be off 500ms.
||								   e.g. If duration = 1000 and duty = 20 the led will
||								   blink 200ms and be off 800ms.
||								     Default value: 50.       
||
||			maxBright(optional).-  Maximum bright of the led, a number between 0 and 100.
||								     Default value: 100.
||
||			minBright(optional).-  Minimun bright of the led, a number between 0 and 100.
||								     Default value: 0.
||
||	Return: No return
||
||	See: update()
||		 updateLeds()
*/
void gLed::multiBlinkFade(uint16_t duration, uint8_t blinks, uint8_t duty, uint8_t maxBright, uint8_t minBright)
{
	if(duration != _period || _timeAux != 0.01*duration*constrain(duty,0,100) || _maxBright != map(constrain(maxBright,0,100),0,100,0,255) || _minBright != map(constrain(minBright,0,100),0,100,0,255) || _blinks != blinks)
	_lastMethod = M_restart;	//Restar the last method used 
		
	if(_lastMethod != M_multiBlinkFade)	//If the method was not called with the same parameters
	{
		_lastMethod = M_multiBlinkFade; 						  //Update the last method used
		_maxBright = map(constrain(maxBright,0,100),0,100,0,255); //Map maxBright to be used in analogWrite
		_minBright = map(constrain(minBright,0,100),0,100,0,255); //Map minBright to be used in analogWrite
		duty = constrain(duty,0,100); 						      //Make sure that the duty is between 0 and 100
		switch(duty)
		{
			case 0:                           //If its 0
				_maxBright = _minBright;      //Exchange the min and max brightness
				_status = LedPWM;             //Set the status to LedPWM
				break;
			 
			case 100:                         //If its 100
				_status = LedPWM;			  //Set the status to LedPWM
				break;
			
			default: 
				_status = LedMultiBlinkFadeON; //Set the status to LedFadeBlinkIN
				_period = duration;            //Save the duration of the animation
				_lastTime = millis();          //Update the _lastTime variable
				_timeAux = 0.01*duration*duty; //Save the increasing part of the animation
				_blinks = blinks;              //Save the number of blinks
				_counts = 0;                   //Number of blinks already performed
				break;
		}
		
	}
}



/*-----------------------------------beginLeds method-------------------------------------
||	Description: Call the begin method of all gLed objects in the sketch, when you have
||	  several leds you can use this single function to initialise all leds instead of 
||	  doing it one by one.
||
||	Syntax: beginLeds()
||
||	Params: No params.
||
||	Return: No return.
||
||	Notes: This is NOT a member method so you mut use this syntax
||		      beginLeds();
||		   instead of 
||			  myLed.beginLeds();
||
||	See: begin()
*/
void beginLeds()
{
	for(uint8_t i=0;i<gLed::_size;i++)
		gLed::_addressBook[i]->begin(); //Call the begin method of all gLed objects
}



/*----------------------------------updateLeds method-------------------------------------
||	Description: Call the update method of all gLed objects in the sketch, when you have
||	  several leds you can use this single function to update all leds instead of doing 
||	  it one by one.
||
||	Syntax: updateLeds()
||
||	Params: No params.
||
||	Return: No return.
||
||	Notes: This is NOT a member method so you mut use this syntax
||		      updateLeds();
||		   instead of 
||			  myLed.updateLeds();
||
||	See: update()
*/
void updateLeds()
{
	for(uint8_t i=0;i<gLed::_size;i++)
		gLed::_addressBook[i]->update();  //Call the update method of all gLed objects
}



/*---------------------------_includeItem private method----------------------------------
||	Description: Allocate new memory to store the pointer of all gLed objects in the 
||	  sketch.
||
||	Syntax: _includeItem(&newItem)
||
||	Params: newItem.- A pointer to a gLed object.
||
||	Return.- No return.
||
||	Note: Since this is a private method you cant use this method in you sketch.
||
||	See: constructor
*/
void gLed::_includeItem(gLed* newItem)
{
	gLed** temporalContainer = 0;          //Temporal container
	gLed::_size++;                         //Increment the _size variable
	if(gLed::_size == 1)                   //If there is only one object
		gLed::_addressBook[0] = newItem;   //Store that object (the direction in memory) in the address book
	else                                   //If there is more than one 
	{
		temporalContainer = gLed::_addressBook;            //Copy the existent data in the container
		
		gLed::_addressBook = new gLed*[gLed::_size];       //Allocates new memory
		for(uint8_t i=0;i<_size-1;i++)
			gLed::_addressBook[i] = temporalContainer[i];  //Copy the old data in the new allocated memory
		gLed::_addressBook[gLed::_size-1] = newItem;       //And add the new item at the end
		
		delete[] temporalContainer;                        //Delete the old allocated memory
		temporalContainer = 0;                             //and just for safe set the container to 0
	}
}