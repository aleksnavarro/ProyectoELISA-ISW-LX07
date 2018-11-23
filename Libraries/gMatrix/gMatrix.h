#ifndef __gMATRIX__
#define __gMATRIX__

#include<dynamicVector.h>

class gMatrix{
	
	public:
		gMatrix();
		gMatrix(uint8_t index);
		gMatrix(uint8_t rows, uint8_t columns);
		gMatrix(const gMatrix& matrix);
		
		uint8_t rows();
		uint8_t columns();
		const uint8_t rows() const;
		const uint8_t columns() const;
		void resize(uint8_t rows, uint8_t columns);
		gMatrix times(const gMatrix& matrix);
		
		float& operator()(uint8_t row, uint8_t column);
		float& operator()(uint8_t index);
		const float& operator()(uint8_t row, uint8_t column) const;
		const float& operator()(uint8_t index) const;
		gMatrix& operator=(const gMatrix& matrix);
		gMatrix  operator+(const gMatrix& matrix);
		gMatrix  operator+(float value);
		friend gMatrix operator+(float value, const gMatrix& matrix);
		gMatrix  operator-(const gMatrix& matrix);
		gMatrix  operator-(float value);
		friend gMatrix operator-(float value, const gMatrix& matrix);
		gMatrix  operator-();
		const gMatrix operator-() const;
		gMatrix  operator*(const gMatrix& matrix);
		gMatrix  operator*(float value);
		friend gMatrix operator*(float value, const gMatrix& matrix);
		gMatrix& operator+=(const gMatrix& matrix);
		gMatrix& operator+=(float value);
		gMatrix& operator-=(const gMatrix& matrix);
		gMatrix& operator-=(float value);
		gMatrix& operator*=(float value);
		bool     operator==(const gMatrix& matrix);
		bool     operator!=(const gMatrix& matrix);
		
	private:
		dynamicVector<dynamicVector<float> > _matrix;
	
};

#endif