#include"hardwareEnclosure.h"



/*---------------------------_addressBook static member-----------------------------------
||	Description: This variable uses dynamical allocation of memory to store the address of
||	  all hardwareEnclosure derived objects in the sketch to be used in the beginAll and 
||	  updateAll functions.
*/
dynamicVector<hardwareEnclosure*> hardwareEnclosure::_addressBook;



/*----------------------------beginAll friend function------------------------------------
||	Description: This function is used to call the begin method of all hardwareEnclosure 
||	  derived objects in the sketch.
||
||	Syntax: beginAll();
||
||	Arguments: No arguments.
||
||	Return: No return.
||
||	Notes: This is NOT a member function so you must use this syntax
||		beginAll();
||	  instead of 
||		myVariable.beginAll();
*/
void beginAll()
{
	for(uint8_t i=0;i<hardwareEnclosure::_addressBook.getSize();i++)
		hardwareEnclosure::_addressBook[i]->begin();  //Call the begin method of all hardwareEnclosure
}                                                     //derived objects in sequence 



/*---------------------------updateAll friend function------------------------------------
||	Description: This function is used to call the update method of all hardwareEnclosure 
||    derived objects in the sketch.
||
||	Syntax: beginAll();
||
||	Arguments: No arguments.
||
||	Return: No return.
||
||	Notes: This is not a member function so you must use this syntax
||	  	updateAll();
||	  instead of 
||		myVariable.updateAll();
*/
void updateAll()
{
	for(uint8_t i=0;i<hardwareEnclosure::_addressBook.getSize();i++)
		hardwareEnclosure::_addressBook[i]->update();  //Call the update method of all hardwareEnclosure
}                                                      //derived objects in sequence