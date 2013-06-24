////////////////////////////////////////////////////////////////////////////////
//        FILE : mathexpr.h
//      AUTHOR : Charles Hosson
//        DATE :   Creation : April 6 2013
//               Last entry : June 20 2013
// DESCRIPTION : 
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cmath>
#include <string>
#include <sstream>

using namespace std;



namespace Const
{
const double PI = 3.1415926535897932;
const double SPEED_LIGHT = 2.99792458e8;
const double GRAV_CONST = 6.67384e-11;
const double E = 1.602176565e-19;
const double AVOG = 6.02214129e23;
}


double eval ( string );

void parseExpression ( string, string, double&, double& );

double evalAddition ( string );

double evalSubstraction ( string );

double evalMultiplication ( string );

double evalDivision ( string );


//##############################################################################


double eval( string expressionToEval )
{
  double result;
	if ( int(expressionToEval.find("+")) != -1 )
		result = evalAddition(expressionToEval);
		
	else if (
	( int(expressionToEval.find("-")) != -1 ) and
	( int(expressionToEval.find("-")) != 0 ) )
		result = evalSubstraction(expressionToEval);
		
	else if ( int(expressionToEval.find("*")) != -1 )
		result = evalMultiplication(expressionToEval);
		
	else if ( int(expressionToEval.find("/")) != -1 )
		result = evalDivision(expressionToEval);
		
	else {
		stringstream streamExpression;
		streamExpression << expressionToEval;
		streamExpression >> result;
	}
	
	return result;
}


void parseExpression ( string expression, string operation,
					   double& number1, double& number2 )
{
	string str1, str2;
	str1 = str2 = expression;
	str1.resize(expression.find(operation));
	str2.erase(0, expression.find(operation) + 1);

	stringstream conversion;
	
	conversion << str1;
	conversion >> number1;
	conversion.str("");
	
	conversion << str2;
	conversion >> number2;
	conversion.str("");
}


double evalAddition ( string addition )
{
	double dou1, dou2;
	
	parseExpression(addition, "+", dou1, dou2);
	
	return dou1 + dou2;
}


double evalSubstraction ( string substraction )
{
	double dou1, dou2;
	
	parseExpression(substraction, "-", dou1, dou2);
	
	return dou1 - dou2;
}


double evalMultiplication ( string multiplication )
{
	double dou1, dou2;
	
	parseExpression(multiplication, "*", dou1, dou2);
	
	return dou1 * dou2;
}


double evalDivision ( string division )
{
	double dou1, dou2;
	
	parseExpression(division, "/", dou1, dou2);
	
	return dou1 / dou2;
}

