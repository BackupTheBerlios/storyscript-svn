/*
 Operator.cpp
 ----------------------
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
OperatorPointer Operator::GetOperatorPtr(){
	return boost::dynamic_pointer_cast<Operator>( ScopeObjectPointer( mpThis ) );
}


const OperatorPointer Operator::GetOperatorPtr() const{
    return boost::dynamic_pointer_cast<Operator>( ScopeObjectPointer( mpThis ) );
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

	Register( ScopeObjectPointer( new List( LC_Input, true ) ) );

	SetConst( WasConst );
}


