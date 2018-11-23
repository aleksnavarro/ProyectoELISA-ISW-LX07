/*
||	dynamicVector template
||
||	Author: Adrian Ramirez
||  Version: 1.0.1
||  Contact: aredlabs@gmail.com
||
||	Description: This template defines a simple dynamic container class that can be used 
||	  to store up to 255 elements of any data type (you can increase the maximum number of
||	  elements by changing the _size private member type and the operator[] index
||	  argument).
||
||	The template uses dynamic memory allocation to store the new incoming data.
||
||
||	Update History
||	--------------------------------------------------------------------------------------
||	Version 1.0.0     dd/mm/yyyy: First Release.
||	Version 1.0.1	  18/11/2018: New remove and resize methods are added.
||	--------------------------------------------------------------------------------------
*/



#ifndef __dynamicVector__
#define __dynamicVector__

#include<Arduino.h>

template<typename V>
class dynamicVector{
    public:
        dynamicVector();
        ~dynamicVector();
        void pushIn(const V);
        void remove();
        void resize(uint8_t newSize);
        void resize(uint8_t newSize, const V);
        uint8_t getSize();
        V &operator[](uint8_t index);
	private:
        uint8_t _size;           //Number of elements in the container
        V *_data;	             //Data stored in the container
};



/*------------------------------------Constructor-----------------------------------------
||	Description: Creates a dynamicVector container with one slot 
||
||	Syntax: dynamicVector<dataTipe> myVector
||
||	Arguments: No arguments.
||
||	Return: No return.
*/

template<typename V>
dynamicVector<V>::dynamicVector(){
    _size = 0;			//Set the initial value of the _size variable
    _data = new V[1];	//Allocates the first memory space
}



/*-------------------------------------Destructor-----------------------------------------
||	Description: Return the dynamically allocated memory to avoid memory leak
||
||	Syntax: ~dynamicVector()
||
||	Arguments: No arguments.
||
||	Return: No return.	
*/
template<typename V>
dynamicVector<V>::~dynamicVector(){
    delete[] _data;     //delete the allocated memory
    _data = 0;
}



/*-----------------------------------pushIn method----------------------------------------
||	Description: This method take the new element to be stored and puts it at the end of 
||    the container.
||
||	Syntax: pushIn(newItem)
||
||	Arguments: newItem.-  New element to be stored in the container.
||
||	Return: No return.
*/
template<typename V>
void dynamicVector<V>::pushIn(const V newItem){
    _size++;						//Increases the _size variable
    if(_size==1)                    //If there are only one element 
        _data[0] = newItem;            //Is stored at the begin of the container
    else                            //If there are more than one
    {
        V *temporal;                //Creates a temporal array
        temporal = _data;           //And copy all the data into it
        
        _data = new V[_size];       //New memory are allocated to store the new data
        for(uint8_t i=0;i<_size-1;i++)
            _data[i] = temporal[i]; //Copy the existing data in the new memory
        _data[_size-1] = newItem;   //and add the new item at the end
        
        delete[] temporal;          //Delete the temporal array
        temporal = 0;               //and just for safe equals to zero
    }
}



/*----------------------------------remove method-----------------------------------------
||	Description: This method remove the last element of the vector, destroying the data.
||
||	Syntax:	remove()
||
||	Arguments: No arguments.
||
||	Return: No return.
*/
template<typename V>
void dynamicVector<V>::remove(){
	_size = (_size == 0)? 0 : _size - 1;	//Decrease the _size variable
	if(_size > 0){							//If there is more tha one element
		V *temporal;							//Creates a temporal array;
		temporal = _data;						//And copy all the data into it
		_data =  new V[_size];					//New memory are allocated
		for(int i=0; i<_size; i++)				
			_data[i] = temporal[i];			//The old data are copied back except for the last element
			
		delete[] temporal;					//Delete the temporal array
		temporal = 0;						//and just for safe equals to zero
	}
		
}



/*----------------------------------resize method-----------------------------------------
||	Description: This method change the size of the container, if the new size is bigger 
||		than the actual size new memory are allocated at the end of the container to fill 
||		the data, if the new size are smaller than the actual size, the memory that doesn't
||		fit at the end of the container are deleted, destroying the data.
||
||	Syntax: resize(newSize);
||
||	Arguments: newSize.-	The new size, a number between 0-255
||
||	Return: No return.
*/
template<typename V>
void dynamicVector<V>::resize(uint8_t newSize){
	uint8_t dSize;			//The change in size
	
	if(newSize == _size)						//If no changes in size are needed
		return;										//Exit the method
	else if(newSize > _size){					//If the new size its bigger than the actual size
		V *temporal;					//Creates a temporal container
		temporal = _data;				//And copy all the data into it
		
		_data = new V[newSize];			//Allocates new memory
		for(uint8_t i=0; i<_size; i++)		
			_data[i] = temporal[i];			//The old data are copied back, the last elements remain empty
		
		_size = newSize;				//Set the new size
			
		delete[] temporal;				//Deletes the temporal container
		temporal = 0;					//And just for safe equals to zero
		
	}
	else if(newSize < _size){					//If the new size is smaller than the actual size
		dSize =_size - newSize;							//Calculates the change in the size
		for(uint8_t i=0; i<_size - newSize; i++)		//For each remaining element
			this->remove();								//Is removed
	}
	
}



/*----------------------------------resize method-----------------------------------------
||	Description: This method change the size of the container, if the new size is bigger 
||		than the actual size new memory are allocated at the end of the container to fill 
||		the data, if the new size are smaller than the actual size, the memory that doesn't
||		fit at the end of the container are deleted, destroying the data.
||
||	Syntax: resize(newSize);
||
||	Arguments: newSize.-	The new size, a number between 0-255
||			   value.-		The value of the new elements
||
||	Return: No return.
*/
template<typename V>
void dynamicVector<V>::resize(uint8_t newSize, const V value){
	uint8_t dSize;			//The change in size
	
	if(newSize == _size)						//If no changes in size are needed
		return;										//Exit the method
	else if(newSize > _size){					//If the new size its bigger than the actual size
		dSize = newSize - _size;			//Calculates the change in the size
		for(uint8_t i=0; i<dSize; i++)		//For each new element needed
			this->pushIn(value);						//Add the element 'value' at the end of the container
	}
	else if(newSize < _size){					//If the new size is smaller than the actual size
		dSize =_size - newSize;				//Calculates the change in the size
		for(uint8_t i=0; i<dSize; i++)		//For each remaining element
			this->remove();								//Is removed
	}
	
}



/*-----------------------------------getSize method---------------------------------------
||	Description: This method return the number of elements in the container.
||
||	Syntax: getSize()
||
||	Arguments: No arguments.
||
||	Return: The number of elements in the container, a number between 0-255.
*/
template<typename V>
uint8_t dynamicVector<V>::getSize(){
	return _size;
}



/*----------------------------------[] operator-------------------------------------------
||	Description: This is an overloaded version of the [] operator, its used to randomly 
||    access the individual items of the container as a regular array.
||
||	Syntax: myVector[index]
||
||	Arguments: index.-  A number between 0-255.
||
||	Return: A reference to the element in the index position of the container.
*/
template<typename V>
V& dynamicVector<V>::operator[](uint8_t index){
	index = constrain(index,0,_size-1);
	return _data[index];
}

#endif