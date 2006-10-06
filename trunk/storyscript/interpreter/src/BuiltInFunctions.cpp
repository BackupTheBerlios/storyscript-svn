/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.
Full license information is included in the file in the top
directory named "license".
*/

#include "BuiltInFunctions.hpp"
#include "LanguageConstants.hpp"
#include "Interpreter.hpp"
#include "Interface.hpp"
#include "Block.hpp"

//For PrintF
#include <cstdio>
#include <cstdarg>

using namespace SS;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
ImportOperator::ImportOperator( Interpreter& I )
: Operator( LC_Import, true ), mI(I)
{}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
UnImportOperator::UnImportOperator( Interpreter& I )
: Operator( LC_UnImport, true ), mI(I)
{}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
ReturnOperator::ReturnOperator( Interpreter& I )
: Operator( LC_Return, true ), mI(I)
{}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
PrintOperator::PrintOperator( Interpreter& I )
: Operator( LC_Print, true ), mI(I)
{}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr ImportOperator::Operate( VariableBasePtr X )
{
	//Identifiers will always start with ':'
	if( X->GetStringData()[0] == LC_ScopeResolution[0] )	 {
		mI.ImportIntoCurrentScope(	X->GetStringData() );
	}
	else	 {
		mI.ImportFileIntoCurrentScope( X->GetStringData() );								
	}			
				
	return X;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr UnImportOperator::Operate( VariableBasePtr X )
{
	mI.GetCurrentScope()->UnImport(
			mI.GetScopeObject( MakeCompoundID( X->GetStringData() ) )->CastToScope() );
			
	return X;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr ReturnOperator::Operate( VariableBasePtr X )
{
	/*
	mI.GetCurrentStaticScope()->CastToBlock()->GetOutString() = 
			X->GetStringData();
	*/
	
	*(mI.GetCurrentScope()->GetScopeObjectLocal_NoThrow( LC_Output )->CastToVariableBase()) = *X;	
	mI.EndBlock();
	return X;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr PrintOperator::Operate( VariableBasePtr X )
{
	mI.GetInterface().LogMessage( X->GetStringData(), true );
	return X;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
/*
VariableBasePtr PrintFOperator::Operate( VariableBasePtr X )
{
	ListPtr lX = X->CastToList();
	
	vsprintf(  )
	
	mI.GetInterface().LogMessage( X->GetStringData(), true );
	return X;
}
*/

