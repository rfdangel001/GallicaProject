////////////////////////////////////////////////////////////////////////////////
//        FILE : LinearAlgebra_Errors.hpp
//      AUTHOR : Charles Hosson
//        DATE :   Creation : May 29 2013
//               Last entry : June 22 2013
// DESCRIPTION : Exception types and handlers for "LinearAlgebra.hpp".
////////////////////////////////////////////////////////////////////////////////

#pragma once

using namespace std;


#include <string>




namespace MatErr
{
  enum Code {CAST,
			   HEIGHT,
			   WIDTH,
			   ROW,
			   COLUMN,
			   NOT_SQUARE,
			   SINGULAR,
			   INCOMPATIBLE,};

	const string messages[] = {"Casting error.",
							   "Invalid height.",
							   "Invalid width.",
							   "Row subscript out of range."
							   "Column subscript out of range."
							   "Not square matrix.",
							   "Singular matrix (not invertible).",
							   "Incompatible operands."};
}

namespace VecErr
{
	enum Code {CAST,
			   DIMENSION,
			   ELEMENT,
			   INCOMPATIBLE,
			   NOT_3D};
	
	const string messages[] = {"Casting error.",
							   "Invalid dimension.",
							   "Element subscript out of range",
							   "Incompatible operands.",
							   "Not a 3D vector."};
}



class LinAlgError
{
public:
	// Constructors
	LinAlgError ( MatErr::Code errorCode )
				{ errorType_ = "Matrix";
				 errorMessage_ = MatErr::messages[errorCode]; }
	LinAlgError ( VecErr::Code errorCode )
				{ errorType_ = "Vector";
				  errorMessage_ = VecErr::messages[errorCode]; }
	
	// Non-modifying methods
	void print ( ostream& destination )
		 { destination << errorType_ << " : " << errorMessage_ << "\n"; }

protected:
	// Attributes
	string errorType_;
	string errorMessage_;
};

