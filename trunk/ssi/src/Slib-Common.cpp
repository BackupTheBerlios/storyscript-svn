/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: S-Lib functions/mvars that are miscilaneous and used often.
*/

#include "Slib-Common.hpp"
#include "LanguageConstants.hpp"
#include "Variable.hpp"

//Needed by print
#include "Interpreter.hpp"
#include "Interface.hpp"



#include "CreationFuncs.hpp"





using namespace SS;
using namespace SS::SLib;


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Common::Common
 NOTES: Common constructor
*/
Common::Common()
	: Scope( LC_SL_Common, true )
{
	RegisterPredefined();
	SetConst();
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Common:RegisterPredefined
 NOTES: Registers all the common functions into one scope.
*/
void Common::RegisterPredefined()
{
	Register( ScopeObjectPointer( new print( TXT("print"), true, true ) ) );
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 print::Operate
 NOTES: Prints a message to the console.
*/
VariableBasePointer print::Operate( VariableBasePointer X )
{
	Interpreter::Instance().GetInterface().LogMessage( X->GetStringData(), true );
	return X;
}



