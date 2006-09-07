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
	: Scope( LC_SL_Common, false )
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
	Register( ScopeObjectPtr( gpNANConst ) );
	Register( ScopeObjectPtr( gpInfinityConst ) );
	Register( ScopeObjectPtr( gpNegInfinityConst ) );
	Register( ScopeObjectPtr( gpNewLineConst ) );
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Contructor
*/
SS::SLib::LangOpts::LangOpts()
	: Scope( LC_SL_LangOpts, false )
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
		new BoundULongVar(     TXT("default_precision"), false, MyLangOpts.DefaultPrecision ) ) );
	Register( ScopeObjectPtr( 
		new BoundRoundModeVar( TXT("rounding_mode"),     false, MyLangOpts.RoundingMode ) ) );
	Register( ScopeObjectPtr( 
		new BoundULongVar(     TXT("max_digit_output"),  false, MyLangOpts.MaxDigitOutput ) ) );
	Register( ScopeObjectPtr( 
		new BoundULongVar(     TXT("number_base"),       false, MyLangOpts.NumberBase ) ) );
	Register( ScopeObjectPtr( 
		new BoundFlagVar(      TXT("use_strict_lists"),  false, MyLangOpts.UseStrictLists ) ) );
	Register( ScopeObjectPtr(
		new BoundFlagVar(      TXT("verbose"),           false, MyLangOpts.Verbose ) ) );
}




