////////////////////////////////////////////////////////////////////////////////
//        FILE : LinearAlgebra_Errors.hpp
//      AUTHOR : Charles Hosson
//        DATE :   Creation : May 29 2013
//               Last entry : July 7 2013
// DESCRIPTION : Exception types and handlers for "LinearAlgebra.hpp".
////////////////////////////////////////////////////////////////////////////////

#pragma once

using namespace std;


#include <string>




namespace MatErr
{
	enum Code {HEIGHT,
	           WIDTH,
	           ROW,
	           COLUMN,
	           NOT_SQUARE,
	           SINGULAR,
	           NOT_SYMMETRIC,
	           INCOMPATIBLE,
	           VECTORS_DIMENSIONS};

	const string messages[] = {"Invalid height.",
	                           "Invalid width.",
	                           "Row subscript out of range.",
	                           "Column subscript out of range.",
	                           "Not square matrix.",
	                           "Singular matrix (not invertible).",
	                           "Not symmetric matrix.",
	                           "Incompatible operands.",
	                           "Column vectors not the same dimension."};
}

namespace VecErr
{
	enum Code {DIMENSION,
	           ELEMENT,
	           INCOMPATIBLE,
	           NOT_3D};
	
	const string messages[] = {"Invalid dimension.",
	                           "Element subscript out of range.",
	                           "Incompatible operands.",
	                           "Not a 3D vector."};
}



class LinAlgError
{
public:
	// Constructors
	LinAlgError ( MatErr::Code );
	LinAlgError ( VecErr::Code );
	
	// Non-modifying methods
	void print ( ostream& );

protected:
	// Attributes
	string errorType_;
	string errorMessage_;
};




inline
LinAlgError::LinAlgError ( MatErr::Code errorCode )
{
	errorType_ = "Matrix";
	errorMessage_ = MatErr::messages[errorCode];
}


inline
LinAlgError::LinAlgError ( VecErr::Code errorCode )
{
	errorType_ = "Vector";
	errorMessage_ = VecErr::messages[errorCode];
}


inline
void LinAlgError::print ( ostream& destination )
{
	destination << errorType_ << " : " << errorMessage_ << "\n";
}
