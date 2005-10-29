/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: 
 Basis for custom operators and built in operators/functions.
 Created: 24:5:2005   11:26
*/

#include "Operator.hpp"
#include "LanguageConstants.hpp"
#include "List.hpp"

using namespace SS;




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~
// Operator::Operator
// NOTES: Constructors
//
Operator::Operator()
{
	RegisterPredefinedVars();
}


Operator::Operator( const SS::STRING& Name, bool Static/* = false*/, bool Const/* = false*/ )
	: Scope( Name, Static, Const )
{
	RegisterPredefinedVars();
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~
// Operator::GetOperatorPtr
// NOTES: Constructors
//
OperatorPtr Operator::CastToOperator(){
	return boost::dynamic_pointer_cast<Operator>( ScopeObjectPtr( mpThis ) );
}


const OperatorPtr Operator::CastToOperator() const{
    return boost::dynamic_pointer_cast<Operator>( ScopeObjectPtr( mpThis ) );
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~
// Operator::AcceptVisitor
// NOTES: 
//
void Operator::AcceptVisitor( ScopeObjectVisitor& V ){
	V.VisitOperator(this);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~
// Operator::RegisterPredefinedVars
// NOTES: Registers special predefined variables.
//
void Operator::RegisterPredefinedVars()
{
	bool WasConst = IsConst();
	mConst = false;

	Register( ScopeObjectPtr( CreateGeneric<List>( LC_Input, true, false ) ) );

	SetConst( WasConst );
}


