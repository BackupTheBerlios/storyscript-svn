/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.
Full license information is included in the file in the top
directory named "license".
*/

/**
	\file BuiltInFunctions.hpp
	\breif Special functions that must be aware of Interpreter.  
*/


#if !defined(SS_BuildInFunctions)
#define SS_BuildInFunctions


#include "Operator.hpp"

namespace SS{


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
/**
	\brief Prints a string.
	
	The real world effect depends entirely on how the Interface defines
	"LogMessage".
*/
class PrintOperator : public Operator
{
public:
	/// Constructor
	PrintOperator( Interpreter& I );

	VariableBasePtr Operate( VariableBasePtr );

private:
	Interpreter& mI;
};


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
/**
	\brief Imports a scope or file into the current scope.
*/
class ImportOperator : public Operator
{
public:
	/// Constructor
	ImportOperator( Interpreter& I );
	
	VariableBasePtr Operate( VariableBasePtr );
	
private:
	Interpreter& mI;
};


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
/**
	\brief Un-Imports a scope from the current scope.
*/
class UnImportOperator : public Operator
{
public:
	/// Constructor
	UnImportOperator( Interpreter& I );
	
	VariableBasePtr Operate( VariableBasePtr );

private:
	Interpreter& mI;	
};


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
/**
	\brief Sets the "out" variable and end execution of block.
*/
class ReturnOperator : public Operator
{
public:
	/// Constructor
	ReturnOperator( Interpreter& I );
	
	VariableBasePtr Operate( VariableBasePtr );

private:
	Interpreter& mI;	
};
	
}
#endif
