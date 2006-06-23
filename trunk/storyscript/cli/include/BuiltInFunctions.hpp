/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: The are global functions such as use, unuse, and return.
*/


#if !defined(SS_BuildInFunctions)
#define SS_BuildInFunctions


#include "Operator.hpp"

namespace SS{


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: Moves a scope into another scopes import list, so it gets searched on
 		id lookups.
*/
class ImportOperator : public Operator
{
public:
	ImportOperator();
	
	VariableBasePtr Operate( VariableBasePtr );	
};


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: UnImportOperator - Un-imports a scope that has been imported by calling
 		the above operator.
*/
class UnImportOperator : public Operator
{
public:
	UnImportOperator();
	
	VariableBasePtr Operate( VariableBasePtr );	
};


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: Sets the out variable and returns from a block.
*/
class ReturnOperator : public Operator
{
public:
	ReturnOperator();
	
	VariableBasePtr Operate( VariableBasePtr );	
};
	
}
#endif
