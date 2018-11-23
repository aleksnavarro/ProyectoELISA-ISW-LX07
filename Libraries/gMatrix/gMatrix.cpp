#include"gMatrix.h"

gMatrix::gMatrix(){
	_matrix.resize(1);
	_matrix[0].resize(1);
	
	_matrix[0][0] = 0;
}

gMatrix::gMatrix(uint8_t index){
	_matrix.resize(index);
	for(uint8_t i=0; i<index; i++)
		_matrix[i].resize(1,0);
		
}

gMatrix::gMatrix(uint8_t rows, uint8_t columns){

	_matrix.resize(rows);
	for(uint8_t i=0; i<rows; i++)
		_matrix[i].resize(columns,0);

}

gMatrix::gMatrix(const gMatrix& matrix){
	_matrix.resize(matrix.rows());
	for(uint8_t i=0; i<matrix.rows(); i++){
		_matrix[i].resize(matrix.columns());
		for(uint8_t j=0; j<matrix.columns(); j++)
			_matrix[i][j] = matrix(i,j);
	}
}

uint8_t gMatrix::rows(){
	return _matrix.getSize();
}

uint8_t gMatrix::columns(){
	return _matrix[0].getSize();
}

const uint8_t gMatrix::rows() const{
	return _matrix.getSize();
}

const uint8_t gMatrix::columns() const{
	return _matrix[0].getSize();
}

void gMatrix::resize(uint8_t rows, uint8_t columns){	
	_matrix.resize(rows);
	for(uint8_t i=0; i<rows; i++)
		_matrix[i].resize(columns,0);
	

}


gMatrix gMatrix::times(const gMatrix& matrix){
		
	gMatrix auxMatrix(this->rows(), this->columns());
	
	for(uint8_t i=0; i<this->rows(); i++)
		for(uint8_t j=0; j<this->columns(); j++)
			auxMatrix(i,j) = _matrix[i][j] * matrix(i,j);
			
	return auxMatrix;
	
}


float& gMatrix::operator()(uint8_t row, uint8_t column){
	if(row > this->rows() - 1)
		this->resize(row + 1, this->columns());
	if(column > this->columns() - 1)
		this->resize(this->rows(), column + 1);
	
	return _matrix[row][column];
}

float& gMatrix::operator()(uint8_t index){
	if(this->columns() == 1)
		return (*this)(index, 0);
	else if(this->rows() == 1)
		return (*this)(0, index);
}

const float& gMatrix::operator()(uint8_t row, uint8_t column) const {
	return _matrix[row][column];
}

const float& gMatrix::operator()(uint8_t index) const{
	if(this->rows() == 1)
		return (*this)(0, index);
	else if(this->columns() == 1)
		return (*this)(index, 0);
}

gMatrix& gMatrix::operator=(const gMatrix& matrix){

	if(this->rows() != matrix.rows() || this->columns() != matrix.columns())
		this->resize(matrix.rows(), matrix.columns());
		
	for(uint8_t i=0; i<this->rows(); i++)
		for(uint8_t j=0; j<this->columns(); j++)
			_matrix[i][j] = matrix(i,j);
			
	return *this;
	
}

gMatrix gMatrix::operator+(const gMatrix& matrix){
		
	gMatrix auxMatrix(this->rows(), this->columns());
		
	for(uint8_t i=0; i<this->rows(); i++)
		for(uint8_t j=0; j<this->columns(); j++)
			auxMatrix(i,j) = _matrix[i][j] + matrix(i,j);
			
	return auxMatrix;	
}

gMatrix gMatrix::operator+(float value){
	gMatrix aux(this->rows(), this->columns());
	
	for(uint8_t i=0; i<this->rows(); i++)
		for(uint8_t j=0; j<this->columns(); j++)
			aux(i,j) = value + (*this)(i,j);
	
	return aux;
}

gMatrix operator+(float value, const gMatrix& matrix){
	gMatrix aux(matrix.rows(), matrix.columns());
	
	for(uint8_t i=0; i<matrix.rows(); i++)
		for(uint8_t j=0; j<matrix.columns(); j++)
			aux(i,j) = value + matrix(i,j);
	
	return aux;
}

gMatrix gMatrix::operator-(const gMatrix& matrix){	
	gMatrix auxMatrix(this->rows(), this->columns());
		
	for(uint8_t i=0; i<this->rows(); i++)
		for(uint8_t j=0; j<this->columns(); j++)
			auxMatrix(i,j) = _matrix[i][j] - matrix(i,j);
			
	return auxMatrix;
	
}

gMatrix  gMatrix::operator-(){
	gMatrix auxMatrix(this->rows(), this->columns());
	
	for(uint8_t i=0; i<this->rows(); i++)
		for(uint8_t j=0; j<this->columns(); j++)
			auxMatrix(i,j) = -_matrix[i][j];
	
	return auxMatrix;
}

gMatrix gMatrix::operator-(float value){
	gMatrix aux(this->rows(), this->columns());
	
	for(uint8_t i=0; i<this->rows(); i++)
		for(uint8_t j=0; j<this->columns(); j++)
			aux(i,j) = (*this)(i,j) - value;
	
	return aux;
}

gMatrix operator-(float value, const gMatrix& matrix){
	gMatrix aux(matrix.rows(), matrix.columns());
	
	for(uint8_t i=0; i<matrix.rows(); i++)
		for(uint8_t j=0; j<matrix.columns(); j++)
			aux(i,j) = value - matrix(i,j);
	
	return aux;
}

const gMatrix  gMatrix::operator-() const {
	gMatrix auxMatrix(this->rows(), this->columns());
	
	for(uint8_t i=0; i<this->rows(); i++)
		for(uint8_t j=0; j<this->columns(); j++)
			auxMatrix(i,j) = -_matrix[i][j];
	
	return auxMatrix;
}

gMatrix gMatrix::operator*(const gMatrix& matrix){
	gMatrix auxMatrix(this->rows(), matrix.columns());
	
	for(uint8_t i=0; i<auxMatrix.rows(); i++)
		for(uint8_t j=0; j<auxMatrix.columns(); j++)
			for(uint8_t k=0; k<this->columns(); k++)
				auxMatrix(i,j) += _matrix[i][k] * matrix(k,j);
				
	return auxMatrix;
}

gMatrix gMatrix::operator*(float value){

	gMatrix auxMatrix(this->rows(), this->columns());
	
	for(uint8_t i=0; i<this->rows(); i++)
		for(uint8_t j=0; j<this->columns(); j++)
			auxMatrix(i,j) = _matrix[i][j] * value;
			
	return auxMatrix;
}

gMatrix operator*(float value, const gMatrix& matrix){

	gMatrix auxMatrix(matrix.rows(), matrix.columns());
	
	for(uint8_t i=0; i<matrix.rows(); i++)
		for(uint8_t j=0; j<matrix.columns(); j++)
			auxMatrix(i,j) = matrix(i,j) * value;
			
	return auxMatrix;	
}

gMatrix& gMatrix::operator+=(const gMatrix& matrix){				
	for(uint8_t i=0; i<this->rows(); i++)
		for(uint8_t j=0; j<this->columns(); j++)
			_matrix[i][j] += matrix(i,j);
			
	return *this;	
		
}

gMatrix& gMatrix::operator+=(float value){
				
	for(uint8_t i=0; i<this->rows(); i++)
		for(uint8_t j=0; j<this->columns(); j++)
			_matrix[i][j] += value;
			
	return *this;	
		
}

gMatrix& gMatrix::operator-=(const gMatrix& matrix){

	return *this+=(-matrix);
}

gMatrix& gMatrix::operator-=(float value){

	return *this+=(-value);
}

gMatrix& gMatrix::operator*=(float value){
	for(uint8_t i=0; i<this->rows(); i++)
		for(uint8_t j=0; j<this->columns(); j++)
			_matrix[i][j] *= value;
	
	return *this;
}

bool gMatrix::operator==(const gMatrix& matrix){
	for(uint8_t i=0; i<this->rows(); i++)
		for(uint8_t j=0; j<this->columns(); j++)
			if(_matrix[i][j] != matrix(i,j))
				return false;
				
	return true;
		
}

bool gMatrix::operator!=(const gMatrix& matrix){

	return !(*this==matrix);
		
}