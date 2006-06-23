/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: The are global functions such as use, unuse, and return.
*/

#include "BuiltInFunctions.hpp"
#include "LanguageConstants.hpp"
#include "Interpreter.hpp"
#include "Block.hpp"

using namespace SS;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Constructor
*/
ImportOperator::ImportOperator()
: Operator( LC_Import, true, true )
{}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Constructor
*/
UnImportOperator::UnImportOperator()
: Operator( LC_UnImport, true, true )
{}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Constructor
*/
ReturnOperator::ReturnOperator()
: Operator( LC_Return, true, true )
{}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Import operation.
*/
VariableBasePtr ImportOperator::Operate( VariableBasePtr X )
{
	//Identifiers will always start with ':'
	if( X->GetStringData()[0] == LC_ScopeResolution[0] )	 {
		Interpreter::Instance().ImportIntoCurrentScope(	X->GetStringData() );
	}
	else	 {
		Interpreter::Instance().ImportFileIntoCurrentScope( X->GetStringData() );								
	}			
				
	return X;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: UnImport operation.
*/
VariableBasePtr UnImportOperator::Operate( VariableBasePtr X )
{
	Interpreter::Instance().GetCurrentScope()->UnImport(
			Interpreter::Instance().GetScopeObject( X->GetStringData() )->CastToScope() );
			
	return X;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Return operation.
*/
VariableBasePtr ReturnOperator::Operate( VariableBasePtr X )
{
	Interpreter::Instance().GetCurrentStaticScope()->CastToBlock()->GetOutString() = 
			X->GetStringData();
	Interpreter::Instance().Stop();
	return X;
}




