////////////////////////////////////////////////////////////////////////////////
//        FILE : LinearAlgebra.hpp
//      AUTHOR : Charles Hosson
//        DATE :   Creation : April 10 2013
//               Last entry : July 9 2013
// DESCRIPTION : Operations on real vectors and matrices.
////////////////////////////////////////////////////////////////////////////////

#pragma once

using namespace std;


//{ Includes

#include <cmath>
#include <iomanip>
#include <string>
#include <sstream>
#include <initializer_list>

#include "MathParser.hpp"  // To read numbers as fractions in input stream.
#include "LinearAlgebra_Errors.hpp"  // Exception handler.

//}


//{ Declarations

//{ Constants

enum MatrixType {IDENTITY, SCALAR, DIAGONAL, TRIANGULAR_UP, TRIANGULAR_DOWN};

//}


//{ Classes

// Forward declarations
class Matrix; class Vector;


class Matrix
{
public:
  // Constructors and destructor
	Matrix ( );
	Matrix ( int, int, double = 0.0 );
	Matrix ( int, MatrixType, double = 0.0 );
	Matrix ( initializer_list<initializer_list<double>> );
	Matrix ( const Matrix& );
	Matrix ( const Vector& );
	Matrix ( initializer_list<Vector> );
	~Matrix ( );
	
	// Modifying methods
	void read ( istream& );
	void fill ( double );
	void resize ( int, int );
	void transpose ( );
	void inverse ( );
	void gaussElimination ( );
	
	// Non-modifying methods
	   int getHeight ( );
	   int getWidth ( );
	  void print ( ostream& );
	Matrix submatrix ( int, int );
	Matrix cofactors ( );
	double ruleOfSarrus ( );
	double determinant ( );
	double trace ( );
	
	// Proxy class used to check subscript errors with matrix operator [].
	class Proxy
	{
	public:
		Proxy ( double*, int );
		
		double& operator [] ( int );
		 double operator [] ( int ) const;
	
	protected:
		    int width_;
		double* row_;
	};
	
	// Modifying operators
	  Proxy operator [] ( int );
	Matrix& operator = ( const Matrix& );
	Matrix& operator = ( initializer_list<initializer_list<double>> );
	Matrix& operator += ( const Matrix& );
	Matrix& operator -= ( const Matrix& );
	Matrix& operator *= ( const Matrix& );
	Matrix& operator *= ( double );
	Matrix& operator /= ( double );
	
	// Non-modifying operators
	 Proxy operator [] ( int ) const;
	  bool operator == ( const Matrix& ) const;
	  bool operator != ( const Matrix& ) const;
	Matrix operator - ( ) const;
	Matrix operator + ( const Matrix& ) const;
	Matrix operator - ( const Matrix& ) const;
	Matrix operator * ( const Matrix& ) const;
	Matrix operator * ( double ) const;
	Matrix operator / ( double ) const;


protected:
	// Attributes
	     int height_;
	     int width_;
	double** array_;
};


class Vector
{
public:
	// Constructors and destructor
	Vector ( );
	Vector ( int, double = 0.0);
	Vector ( initializer_list<double> );
	Vector ( const Vector& );
	~Vector ( );
	
	// Modifying methods
	void read ( istream& );
	void fill ( double );
	void resize ( int );
	void normalise ( );
	
	// Non-modifying methods
	   int getDimension ( ) const;
	  void print ( ostream& );
	double magnitude ( );
	
	// Modifying operators
	double& operator [] ( int );
	Vector& operator = ( const Vector& );
	Vector& operator = ( initializer_list<double> );
	Vector& operator += ( const Vector& );
	Vector& operator -= ( const Vector& );
	Vector& operator *= ( double );
	Vector& operator /= ( double );
	 
	// Non-modifying operators
	double operator [] ( int ) const;
	  bool operator == ( const Vector& );
	  bool operator != ( const Vector& );
	Vector operator - ( );
	Vector operator + ( const Vector& ) const;
	Vector operator - ( const Vector& ) const;
	Vector operator * ( double ) const;
	Vector operator / ( double ) const;

protected:
	// Attributes
	    int dimension_;
	double* array_;
};

//}


//{ Functions

Matrix pow ( const Matrix&, int );

inline
Matrix operator * ( double scalarTerm, const Matrix& matrixTerm )
{
	return matrixTerm * scalarTerm;
}

inline
Vector operator * ( double scalarTerm, const Vector& vectorTerm )
{
	return vectorTerm * scalarTerm;
}

double scalarProduct ( const Vector&, const Vector& );

Vector crossProduct ( const Vector&, const Vector& );

//}

//}




//{ Matrix

//{ Matrix::Constructors and destructor

Matrix::Matrix ( )
{
	height_ = 0;
	width_ = 0;
	
	array_ = NULL;
}


Matrix::Matrix ( int initHeight, int initWidth, double initValue )
{
	if ( initHeight <= 0 )
		throw LinAlgError(MatErr::HEIGHT);
	if ( initWidth <= 0 )
		throw LinAlgError(MatErr::WIDTH);
	
	height_ = initHeight;
	width_ = initWidth;
	
	array_ = new double*[height_];
	for ( int i = 0; i < height_; i ++ ) 
		array_[i] = new double[width_];
	
	(*this).fill(initValue);
}


Matrix::Matrix ( int initOrder, MatrixType type, double initValue )
{
	if ( initOrder <= 0 )
		throw LinAlgError(MatErr::HEIGHT);
	
	height_ = width_ = initOrder;
	
	array_ = new double*[height_];
	for ( int i = 0; i < height_; i ++ )
		array_[i] = new double[width_];
	
	for ( int i = 0; i < height_; i ++ )
		for ( int j = 0; j < width_; j ++ )
			array_[i][j] = 0.0;
	
	if ( type == IDENTITY )
		for ( int i = 0; i < height_; i ++ )
			array_[i][i] = 1.0;
	
	if ( type == SCALAR )
		for ( int i = 0; i < height_; i ++ )
			array_[i][i] = initValue;
	
	if ( type == TRIANGULAR_UP )
		for ( int i = 0; i < height_; i ++ )
			for ( int j = i; j < width_; j ++ )
				array_[i][j] = initValue;
	
	if ( type == TRIANGULAR_DOWN )
		for ( int i = 0; i < height_; i ++ )
			for ( int j = 0; j <= i; j ++ )
				array_[i][j] = initValue;
}


Matrix::Matrix ( initializer_list<initializer_list<double>> initValuesList )
{
	height_ = initValuesList.size();
	width_ = initValuesList.begin()->size();
	
	array_ = new double*[height_];
	for ( int i = 0; i < height_; i ++ ) 
		array_[i] = new double[width_];
	
	int i = 0;
	for ( initializer_list<double> row : initValuesList) {
		int j = 0;
		for ( double column : row ) {
			array_[i][j] = column;
			
			j ++;
		}
		
		i ++;
	}
}


Matrix::Matrix ( const Matrix& model )
{
	height_ = model.height_;
	width_ = model.width_;
	
	array_ = new double*[height_];
	for ( int i = 0; i < height_; i ++ ) 
		array_[i] = new double[width_];
	
	for ( int i = 0; i < height_; i ++ )
		for ( int j = 0; j < width_; j ++ )
			array_[i][j] = model.array_[i][j];
}


Matrix::Matrix ( const Vector& modelVector )
{
	height_ = modelVector.getDimension();
	width_ = 1;
	
	array_ = new double*[height_];
	for ( int i = 0; i < height_; i ++ )
		array_[i] = new double;
	
	for ( int i = 0; i < height_; i ++ )
		array_[i][0] = modelVector[i];
}


Matrix::Matrix ( initializer_list<Vector> initVectorsList )
{
	int matrixHeight = initVectorsList.begin()->getDimension();
	for ( Vector columnVector : initVectorsList )
		if ( columnVector.getDimension() != matrixHeight )
			throw LinAlgError(MatErr::VECTORS_DIMENSIONS);
	
	height_ = matrixHeight;
	width_ = initVectorsList.size();
	
	array_ = new double*[height_];
	for ( int i = 0; i < height_; i ++ )
		array_[i] = new double[width_];
	
	int i = 0;
	for ( Vector columnVector : initVectorsList ) {
		for ( int j = 0; j < height_; j ++ )
			array_[j][i] = columnVector[j];
		
		i ++;
	}
}


inline
Matrix::~Matrix ( )
{
	for ( int i = 0; i < height_; i ++ )
		delete []array_[i];
	delete []array_;
}

//}


//{ Matrix::Modifying methods

void Matrix::read ( istream& source )
{
	int readHeight, readWidth;
	source >> readHeight >> readWidth;
	
	if ( readHeight < 0 )
		throw LinAlgError(MatErr::HEIGHT);
	if ( readWidth < 0 )
		throw LinAlgError(MatErr::WIDTH);
	
	(*this).resize(readHeight, readWidth);
	
	for ( int i = 0; i < height_; i ++ )
		for ( int j = 0; j < width_; j ++ ) {
			string valueExpression;
			source >> valueExpression;
			(*this)[i][j] = eval(valueExpression);
		}
}


void Matrix::fill ( double value )
{
	for ( int i = 0; i < height_; i ++ )
		for ( int j = 0; j < width_; j ++ )
			(*this)[i][j] = value;
}


void Matrix::resize ( int newHeight, int newWidth )
{
	if ( newHeight <= 0 )
		throw LinAlgError(MatErr::HEIGHT);
	if ( newWidth <= 0 )
		throw LinAlgError(MatErr::WIDTH);
	
	Matrix buffer(*this);
	
	for ( int i = 0; i < height_; i ++ )
		delete []array_[i];
	delete []array_;
	
	height_ = newHeight;
	width_ = newWidth;
	
	array_ = new double*[height_];
	for ( int i = 0; i < height_; i ++ )
		array_[i] = new double[width_];
	
	(*this).fill(0.0);
	
	for ( int i = 0; i < height_ and i < buffer.height_; i ++ )
		for ( int j = 0; j < width_ and j < buffer.width_; j ++ )
			(*this)[i][j] = buffer[i][j];
}


void Matrix::transpose ( )
{
	Matrix buffer(*this);
	
	for ( int i = 0; i < height_; i ++ )
		delete []array_[i];
	delete []array_;
	
	height_ = buffer.width_;
	width_ = buffer.height_;
	
	array_ = new double*[height_];
	for ( int i = 0; i < height_; i ++ )
		array_[i] = new double[width_];
	
	for ( int i = 0; i < height_; i ++ )
		for ( int j = 0; j < width_; j ++ )
			(*this)[i][j] = buffer[j][i];
}


void Matrix::inverse ( )
{
	if ( height_ != width_ )
		throw LinAlgError(MatErr::NOT_SQUARE);
	
	double determinant = (*this).determinant();
	
	if ( determinant == 0.0 )
		throw LinAlgError(MatErr::SINGULAR);
	
	*this = (*this).cofactors();
	(*this).transpose();
	(*this) *= (1.0 / determinant);
}


void Matrix::gaussElimination ( )
{
	if ( height_ != width_ )
		throw LinAlgError(MatErr::NOT_SQUARE);
	
	for ( int i = 0; i < height_; i ++ )
		for ( int j = i + 1; j < height_; j ++ ) {
			double coeff;
			
			if ( (*this)[i][i] != 0.0 )
				coeff = (*this)[j][i] / (*this)[i][i];
			
			for ( int k = 0; k < width_; k ++ )
				(*this)[j][k] -= coeff * (*this)[i][k];
		}
}

//}


//{ Matrix::Non-modifying methods

inline
int Matrix::getHeight ( )
{
	return height_;
}


inline
int Matrix::getWidth ( )
{
	return width_;
}


void Matrix::print ( ostream& destination )
{
	int maxLength = 0;
	for ( int i = 0; i < height_; i ++ )
		for ( int j = 0; j < width_; j ++ ) {
			stringstream buffer;
			buffer << (*this)[i][j];
			
			if ( int(buffer.str().size()) > maxLength)
				maxLength = buffer.str().size();
		}
	
	destination << height_ << " " << width_ << "\n\n";
	
	for ( int i = 0; i < height_; i ++ ) {
		for ( int j = 0; j < width_; j ++ )
			destination << setw(maxLength + 1) << (*this)[i][j];
		
		destination << "\n";
	}
}


Matrix Matrix::submatrix ( int row, int column )
{
	if ( row >= height_ or row < 0 )
		throw LinAlgError(MatErr::HEIGHT);
	if ( column >= width_ or column < 0 )
		throw LinAlgError(MatErr::WIDTH);
	
	Matrix submatrix(height_ - 1, width_ - 1);
	
	for ( int i = 0; i < submatrix.height_; i ++ ) {
		if ( i >= row )
			for ( int j = 0; j < submatrix.width_; j ++ ) {
				if ( j >= column )
					submatrix[i][j] = (*this)[i + 1][j + 1];
				else
					submatrix[i][j] = (*this)[i + 1][j];
			}
		else
			for ( int j = 0; j < submatrix.width_; j ++ ) {
				if ( j >= column )
					submatrix[i][j] = (*this)[i][j + 1];
				else
					submatrix[i][j] = (*this)[i][j];
			}
	}
	
	return submatrix;
}


Matrix Matrix::cofactors ( )
{
	if ( height_ != width_ )
		throw LinAlgError(MatErr::NOT_SQUARE);
	
	Matrix matrixCof(height_, width_);
	
	for ( int i = 0; i < matrixCof.height_; i ++ )
		for ( int j = 0; j < matrixCof.width_; j ++ ) {
			Matrix buffer = (*this).submatrix(i, j);
			
			matrixCof[i][j] = pow(-1, i + j) * buffer.determinant();
		}
	
	return matrixCof;
}


double Matrix::ruleOfSarrus ( )
{
	double result = 0;
	
	for ( int k = 0; k < 3; k ++ ) {
		double term = 1;
		for ( int i = 0, j = k; i < 3; i ++, j ++ )
			term *= (*this)[i][j % 3];
		
		result += term;
	}
	
	for ( int k = 0; k < 3; k ++ ) {
		double term = 1;
		for ( int i = 2, j = k; i >= 0; i --, j ++ )
			term *= (*this)[i][j % 3];
		
		result -= term;
	}

	return result;
}


double Matrix::determinant ( )
{
	if ( height_ != width_ )
		throw LinAlgError(MatErr::NOT_SQUARE);
	
	double result = 1.0;
	
	if ( width_ == 1 )
		result = (*this)[0][0];
	
	else if ( width_ == 2 )
		result = (*this)[0][0] * (*this)[1][1] - (*this)[1][0] * (*this)[0][1];
	
	else if ( width_ == 3 )
		result = (*this).ruleOfSarrus();
	
	else {
		Matrix echelon(*this);
		echelon.gaussElimination();
		
		for ( int i = 0; i < height_; i ++ )
			result *= echelon[i][i];
	}

	return result;
}


double Matrix::trace ( )
{
	if ( height_ != width_ )
		throw LinAlgError(MatErr::NOT_SQUARE);
	
	double trace = 0.0;
	
	for ( int i = 0; i < height_; i ++ )
		trace += (*this)[i][i];
	
	return trace;
}

//}


//{ Matrix::Proxy

inline
Matrix::Proxy::Proxy ( double* matrixRow, int width )
{
	row_ = matrixRow;
	width_ = width;
}


inline
double& Matrix::Proxy::operator [] ( int column )
{
	if ( column >= width_ or column < 0 )
		throw LinAlgError(MatErr::COLUMN);
	
	return row_[column];
}


inline
double Matrix::Proxy::operator [] ( int column ) const
{
	if ( column >= width_ or column < 0 )
		throw LinAlgError(MatErr::COLUMN);
	
	return row_[column];
}

//}


//{ Matrix::Modifying operators

inline
Matrix::Proxy Matrix::operator [] ( int row )
{
	if ( row >= height_ or row < 0 )
		throw LinAlgError(MatErr::ROW);
	
	return Proxy(array_[row], width_);
}


Matrix& Matrix::operator = ( const Matrix& rightTerm )
{
	if ( rightTerm.width_ != width_ or rightTerm.height_ != height_ ) {
		for ( int i = 0; i < height_; i ++ )
			delete []array_[i];
		delete []array_;
		
		height_ = rightTerm.height_;
		width_ = rightTerm.width_;
		
		array_ = new double*[height_];
		for ( int i = 0; i < height_; i ++ )
			array_[i] = new double[width_];
	}
	
	for ( int i = 0; i < height_; i ++ )
		for ( int j = 0; j < width_; j ++ )
			(*this)[i][j] = rightTerm[i][j];
	
	return *this;
}


Matrix& Matrix::operator = ( initializer_list<initializer_list<double>> 
                             valuesList )
{
	if (
	int(valuesList.size()) != height_ or
	int(valuesList.begin()->size()) != width_ ) {
		for ( int i = 0; i < height_; i ++ )
			delete []array_[i];
		delete []array_;
		
		height_ = valuesList.size();
		width_ = valuesList.begin()->size();
		
		array_ = new double*[height_];
		for ( int i = 0; i < height_; i ++ )
			array_[i] = new double[width_];
	}
	
	int i = 0;
	for ( initializer_list<double> row : valuesList ) {
		int j = 0;
		for ( double column : row ) {
			array_[i][j] = column;
			
			j ++;
		}
	}
	
	return *this;
}


Matrix& Matrix::operator += ( const Matrix& rightTerm )
{
	if ( height_ != rightTerm.height_ or width_ != rightTerm.width_ )
		throw LinAlgError(MatErr::INCOMPATIBLE);
	
	for ( int i = 0; i < height_; i ++ )
		for ( int j = 0; j < width_; j ++ )
			(*this)[i][j] += rightTerm[i][j];
	
	return *this;
}


Matrix& Matrix::operator -= ( const Matrix& rightTerm )
{
	if ( height_ != rightTerm.height_ or width_ != rightTerm.width_ )
		throw LinAlgError(MatErr::INCOMPATIBLE);
	
	for ( int i = 0; i < height_; i ++ )
		for ( int j = 0; j < width_; j ++ )
			(*this)[i][j] += rightTerm[i][j];
	
	return *this;
}


Matrix& Matrix::operator *= ( const Matrix& rightTerm )
{
	*this = *this * rightTerm;
	
	return *this;
}


Matrix& Matrix::operator *= ( double rightScalarTerm )
{
	for ( int i = 0; i < height_; i ++ )
		for ( int j = 0; j < width_; j ++ )
			(*this)[i][j] *= rightScalarTerm;
	
	return *this;
}


Matrix& Matrix::operator /= ( double rightScalarTerm )
{
	for ( int i = 0; i < height_; i ++ )
		for ( int j = 0; j < width_; j ++ )
			(*this)[i][j] /= rightScalarTerm;
	
	return *this;
}

//}


//{ Matrix::Non-modifying operators

inline
Matrix::Proxy Matrix::operator [] ( int row ) const
{
	if ( row >= height_ or row < 0 )
		throw LinAlgError(MatErr::ROW);
	
	return Proxy(array_[row], width_);
}


bool Matrix::operator == ( const Matrix& compared ) const
{
	bool isSame = true;
	
	if ( height_ != compared.height_ or width_ != compared.width_ )
		isSame = false;
	else
		for ( int i = 0; i < height_; i ++ )
			for ( int j = 0; j < width_; j ++ )
				if ( (*this)[i][j] != compared[i][j] ) {
					isSame = false;
					break;
				}
	
	return isSame;
}


bool Matrix::operator != ( const Matrix& compared ) const
{
	if ( *this == compared )
		return false;
	else
		return true;
}


Matrix Matrix::operator - ( ) const
{
	Matrix opposite(*this);
	
	for ( int i = 0; i < opposite.height_; i ++ )
		for ( int j = 0; j < opposite.width_; j ++ )
			opposite[i][j] = -opposite[i][j];
			
	return opposite;
}


Matrix Matrix::operator + ( const Matrix& rightTerm ) const
{
	if ( height_ != rightTerm.height_ or width_ != rightTerm.width_ )
		throw LinAlgError(MatErr::INCOMPATIBLE);
	
	Matrix sum(height_, width_);
	
	for ( int i = 0; i < height_; i ++ )
		for ( int j = 0; j < width_; j ++ )
			sum[i][j] = (*this)[i][j] + rightTerm[i][j];
	
	return sum;
}


Matrix Matrix::operator - ( const Matrix& rightTerm ) const
{
	if ( height_ != rightTerm.height_ or width_ != rightTerm.width_ )
		throw LinAlgError(MatErr::INCOMPATIBLE);
	
	Matrix difference(height_, width_);
	
	for ( int i = 0; i < height_; i ++ )
		for ( int j = 0; j < width_; j ++ )
			difference[i][j] = (*this)[i][j] - rightTerm[i][j];
	
	return difference;
}


Matrix Matrix::operator * ( const Matrix& rightTerm ) const
{
	if ( width_ != rightTerm.height_ )
		throw LinAlgError(MatErr::INCOMPATIBLE);
	
	Matrix product(height_, rightTerm.width_);
	
	for ( int i = 0; i < product.height_; i ++ )
		for ( int j = 0; j < product.width_; j ++ ) {
			double element = 0.0;
			
			for ( int k = 0; k < width_; k ++ )
				element += (*this)[i][k] * rightTerm[k][j];
			
			product[i][j] = element;
		}
	
	return product;
}


Matrix Matrix::operator * ( double rightScalarTerm ) const
{
	Matrix result(*this);
	
	for ( int i = 0; i < height_; i ++ )
		for ( int j = 0; j < width_; j ++ )
			result[i][j] *= rightScalarTerm;
	
	return result;
}


Matrix Matrix::operator / ( double rightScalarTerm ) const
{
	Matrix result(*this);
	
	for ( int i = 0; i < height_; i ++ )
		for ( int j = 0; j < width_; j ++ )
			result[i][j] /= rightScalarTerm;
	
	return result;
}

//}

//}


//{ Vector

//{ Vector::Constructors and destructor

Vector::Vector ( )
{
	dimension_ = 0;
	
	array_ = NULL;
}


Vector::Vector ( int initDimension, double initValue )
{
	if ( initDimension < 0 )
		throw LinAlgError(VecErr::DIMENSION);
	
	dimension_ = initDimension;
	
	array_ = new double[dimension_];
	
	for ( int i = 0; i < dimension_; i ++ )
		array_[i] = initValue;
}


Vector::Vector ( initializer_list<double> initValuesList )
{
	dimension_ = initValuesList.size();
	
	array_ = new double[dimension_];
	
	int i = 0;
	for ( double element : initValuesList ) {
		array_[i] = element;
		
		i ++;
	}
}


Vector::Vector ( const Vector& model )
{
	dimension_ = model.dimension_;
	
	array_ = new double[dimension_];
	
	for ( int i = 0; i < dimension_; i ++ )
		array_[i] = model.array_[i];
}


inline
Vector::~Vector ( )
{
	delete []array_;
}

//}


//{ Vector::Modifying methods

void Vector::read ( istream& source )
{
	int readDimension;
	source >> readDimension;
	
	if ( readDimension < 0 )
		throw LinAlgError(VecErr::DIMENSION);
	
	(*this).resize(readDimension);
	
	for ( int i = 0; i < dimension_; i ++ ){
		string valueExpression;
		source >> valueExpression;
		(*this)[i] = eval(valueExpression);
	}
}


void Vector::fill ( double value )
{
	for ( int i = 0; i < dimension_; i ++ )
		(*this)[i] = value;
}


void Vector::resize ( int newDimension )
{
	if ( newDimension < 0 )
		throw LinAlgError(VecErr::DIMENSION);
	
	Vector buffer(*this);
	
	delete []array_;
	
	dimension_ = newDimension;
	
	array_ = new double[dimension_];
	
	(*this).fill(0.0);
	
	for ( int i = 0; i < dimension_ and i < buffer.dimension_; i ++ )
		(*this)[i] = buffer[i];
}


void Vector::normalise ( )
{
	double magnitude = (*this).magnitude();
	
	for ( int i = 0; i < dimension_; i ++ )
		if ( magnitude != 0.0 )
			(*this)[i] /= magnitude;
}

//}


//{ Vector::Non-modifying methods

inline
int Vector::getDimension ( ) const
{
	return dimension_;
}


void Vector::print ( ostream& destination )
{
	int maxLength = 0;
	for ( int i = 0; i < dimension_; i ++ ) {
		stringstream buffer;
		buffer << (*this)[i];
		
		if ( int(buffer.str().size()) > maxLength )
			maxLength = buffer.str().size();
	}
	
	destination << dimension_ << "\n\n";
	
	for ( int i = 0; i < dimension_; i ++ )
		destination << (*this)[i] << "\n";
}


double Vector::magnitude ( )
{
	double result = 0.0;
	
	for ( int i = 0; i < dimension_; i ++ )
		result += pow((*this)[i], 2);
	
	return sqrt(result);
}

//}


//{ Vector::Modifying operators

inline
double& Vector::operator [] ( int element )
{
	if ( element >= dimension_ or element < 0 )
		throw LinAlgError(VecErr::ELEMENT);
	
	return array_[element];
}


Vector& Vector::operator = ( const Vector& rightTerm )
{
	if ( rightTerm.dimension_ != dimension_ ) {
		delete []array_;
		
		dimension_ = rightTerm.dimension_;
		
		array_ = new double[dimension_];
	}
	
	for ( int i = 0; i < dimension_; i ++ )
		(*this)[i] = rightTerm[i];
	
	return *this;
}


Vector& Vector::operator = ( initializer_list<double> valuesList )
{
	if ( int(valuesList.size()) != dimension_ ) {
		delete []array_;
		
		dimension_ = valuesList.size();
		
		array_ = new double[dimension_];
	}
	
	int i = 0;
	for ( double element : valuesList ) {
		(*this)[i] = element;
		
		i ++;
	}
	
	return *this;
}


Vector& Vector::operator += ( const Vector& rightTerm )
{
	if ( dimension_ != rightTerm.dimension_ )
		throw LinAlgError(VecErr::INCOMPATIBLE);
	
	for ( int i = 0; i < dimension_; i ++ )
		(*this)[i] += rightTerm[i];
	
	return *this;
}


Vector& Vector::operator -= ( const Vector& rightTerm )
{
	if ( dimension_ != rightTerm.dimension_ )
		throw LinAlgError(VecErr::INCOMPATIBLE);
	
	for ( int i = 0; i < dimension_; i ++ )
		(*this)[i] -= rightTerm[i];
	
	return *this;
}


Vector& Vector::operator *= ( double rightScalarTerm )
{
	for ( int i = 0; i < dimension_; i ++ )
		(*this)[i] *= rightScalarTerm;
	
	return *this;
}


Vector& Vector::operator /= ( double rightScalarTerm )
{
	for ( int i = 0; i < dimension_; i ++ )
		(*this)[i] /= rightScalarTerm;
	
	return *this;
}

//}


//{ Vector::Non-modifying operators

inline
double Vector::operator [] ( int element ) const
{
	if ( element >= dimension_ or element < 0 )
		throw LinAlgError(VecErr::ELEMENT);
	
	return array_[element];
}


bool Vector::operator == ( const Vector& compared )
{
	bool isSame = true;
	
	if ( dimension_ != compared.dimension_ )
		isSame = false;
	else
		for ( int i = 0; i < dimension_; i ++ )
			if ( (*this)[i] != compared[i] )
				isSame = false;
	
	return isSame;
}


bool Vector::operator != ( const Vector& compared )
{
	if ( *this == compared )
		return false;
	else
		return true;
}


Vector Vector::operator - ( )
{
	Vector opposite(*this);
	
	for ( int i = 0; i < dimension_; i ++ )
		opposite[i] = -opposite[i];
	
	return opposite;
}


Vector Vector::operator + ( const Vector& rightTerm ) const
{
	if ( dimension_ != rightTerm.dimension_ )
		throw LinAlgError(VecErr::INCOMPATIBLE);
	
	Vector result(dimension_);
	
	for ( int i = 0; i < dimension_; i ++ )
		result[i] = (*this)[i] + rightTerm[i];
	
	return result;
}


Vector Vector::operator - ( const Vector& rightTerm ) const
{
	if ( dimension_ != rightTerm.dimension_ )
		throw LinAlgError(VecErr::INCOMPATIBLE);

	Vector result(dimension_);
	
	for ( int i = 0; i < dimension_; i ++ )
		result[i] = (*this)[i] - rightTerm[i];
	
	return result;
}


Vector Vector::operator * ( double rightScalarTerm ) const
{
	Vector result(*this);
	
	for ( int i = 0; i < dimension_; i ++ )
		result[i] *= rightScalarTerm;
	
	return result;
}


Vector Vector::operator / ( double rightScalarTerm ) const
{
	Vector result(*this);
	
	for ( int i = 0; i < dimension_; i ++ )
		result[i] /= rightScalarTerm;
	
	return result;
}


//}

//}




//{ Functions

//{ Functions

Matrix pow ( Matrix base, int exponent )
{
	Matrix result(base);
	
	if ( exponent == 0 )
		result = Matrix(base.getHeight(), IDENTITY);
	else
		for ( int i = 0; i < exponent; i ++ )
			result *= base;

	return result;
}


double scalarProduct ( const Vector& leftTerm, const Vector& rightTerm )
{
	if ( leftTerm.getDimension() != rightTerm.getDimension() )
		throw LinAlgError(VecErr::INCOMPATIBLE);
	
	double result;
	for ( int i = 0; i < leftTerm.getDimension(); i ++ )
		result += leftTerm[i] * rightTerm[i];
	
	return result;
}


Vector crossProduct ( const Vector& leftTerm, const Vector& rightTerm )
{
	if ( leftTerm.getDimension() != 3 or rightTerm.getDimension() != 3 )
		throw LinAlgError(VecErr::NOT_3D);
	
	Vector result(3);
	
	result[0] = leftTerm[1] * rightTerm[2] - leftTerm[2] * rightTerm[1];
	result[1] = leftTerm[0] * rightTerm[2] - leftTerm[2] * rightTerm[0];
	result[2] = leftTerm[0] * rightTerm[1] - leftTerm[1] * rightTerm[0];
	
	return result;
}

//}


//{ Operators

//}

//}

