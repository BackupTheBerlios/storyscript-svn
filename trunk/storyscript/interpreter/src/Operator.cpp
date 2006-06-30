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




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
Operator::Operator()
{
	RegisterPredefinedVars();
}


Operator::Operator( const SS::STRING& Name, bool Static/* = false*/, bool Const/* = false*/ )
	: Scope( Name, Static, Const )
{
	RegisterPredefinedVars();
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
OperatorPtr Operator::CastToOperator(){
	return boost::dynamic_pointer_cast<Operator>( ScopeObjectPtr( mpThis ) );
}


const OperatorPtr Operator::CastToOperator() const{
    return boost::dynamic_pointer_cast<Operator>( ScopeObjectPtr( mpThis ) );
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Operator::AcceptVisitor( ScopeObjectVisitor& V ){
	V.VisitOperator(this);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Operator::RegisterPredefinedVars()
{
	// There's nothing to see here right now.  This is declared/defined because
	// is was being used at one point.
}


