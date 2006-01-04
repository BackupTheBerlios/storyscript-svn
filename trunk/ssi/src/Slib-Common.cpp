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
#include "SpecialVars.hpp"




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
	Register( ScopeObjectPtr( new print( TXT("print"), true, true ) ) );
	Register( ScopeObjectPtr( gpNANConst ) );
	Register( ScopeObjectPtr( gpInfinityConst ) );
	Register( ScopeObjectPtr( gpNegInfinityConst ) );
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Contructor
*/
SS::SLib::LangOpts::LangOpts()
	: Scope( LC_SL_LangOpts, true )
{
	RegisterPredefined();
	SetConst();
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Register bindings for SS:LangOpts.
*/
void SS::SLib::LangOpts::RegisterPredefined()
{
	SS::LangOpts& MyLangOpts = SS::LangOpts::Instance();
	
	Register( ScopeObjectPtr( 
		new BoundULongVar( TXT("default_precision"), MyLangOpts.DefaultPrecision, true, false ) ) );
	Register( ScopeObjectPtr( 
		new BoundULongVar( TXT("rounding_mode"), MyLangOpts.RoundingMode, true, false ) ) );
	Register( ScopeObjectPtr( 
		new BoundULongVar( TXT("max_digit_output"), MyLangOpts.MaxDigitOutput, true, false ) ) );
	Register( ScopeObjectPtr( 
		new BoundULongVar( TXT("number_base"), MyLangOpts.NumberBase, true, false ) ) );
	Register( ScopeObjectPtr( 
		new BoundFlagVar( TXT("use_strict_lists"), MyLangOpts.UseStrictLists, true, false ) ) );
	Register( ScopeObjectPtr(
		new BoundFlagVar( TXT("verbose"), MyLangOpts.Verbose, true, false ) ) );
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 print::Operate
 NOTES: Prints a message to the console.
*/
VariableBasePtr print::Operate( VariableBasePtr X )
{
	Interpreter::Instance().GetInterface().LogMessage( X->GetStringData(), true );
	return X;
}



