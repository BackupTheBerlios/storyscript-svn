/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: This is a list of language constants that can be changed to effect
	in the source file to rename operators, etc.
*/


#include "LanguageConstants.hpp"

//To initialize gpEmptyList
#include "CreationFuncs.hpp"
#include "List.hpp"
#include "Variable.hpp"

using namespace SS;

#include <clocale>


//Don't worry, these get initialized by InitConstants
VariablePtr SS::gpNANConst;
VariablePtr SS::gpInfinityConst;
VariablePtr SS::gpNegInfinityConst;
VariablePtr SS::gpNewLineConst;



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Language option singleton intilization code
*/

boost::shared_ptr<LangOpts> LangOpts::mpInstance;

LangOpts& LangOpts::Instance()
{
	if( !mpInstance ){
		mpInstance.reset( new LangOpts );
	}
	
	return *mpInstance;
	
}

LangOpts::LangOpts()
//!!! This is important!  Default Language settings !!!
: DefaultPrecision( 256 ),
  RoundingMode( GMP_RNDN ),
  MaxDigitOutput( 0 ), //0 means infinate
  NumberBase( 10 ),
  UseStrictLists( false ),
  Verbose( false )
{
	
}





namespace SS{
	
bool ltstr::operator()( const SS::CHAR* a, const SS::CHAR* b ) const
{
	return strcmp( a, b ) < 0;	
}
	
std::map< const SS::CHAR*, ExtraDesc, ltstr > gUnaryOperatorMap;
std::map< const SS::CHAR*, ExtraDesc, ltstr > gBinaryOperatorMap;
std::map< const SS::CHAR*, ExtraDesc, ltstr > gAmbigOperatorMap;

std::map< ExtraDesc, const SS::CHAR* > gUnaryOperatorReverseMap;
std::map< ExtraDesc, const SS::CHAR* > gBinaryOperatorReverseMap;
std::map< ExtraDesc, const SS::CHAR* > gAmbigOperatorReverseMap;

//Default Values (i.e. They may be overridden by InitConstants)
const SS::CHAR* LC_DecimalPoint = TXT(".");
const SS::CHAR* LC_ThousandsSep = TXT(",");

//Special Variable Names
const SS::CHAR* LC_Precision = TXT("precision");


const SS::CHAR* LC_Length = TXT("size");
const SS::CHAR* LC_Name( TXT("name") );
const SS::CHAR* LC_FullName( TXT("fullname") );
const SS::CHAR* LC_EndBlock = TXT("end");
const SS::CHAR* LC_Static = TXT("static");
const SS::CHAR* LC_BeenSaid = TXT("beensaid");
const SS::CHAR* LC_Input = TXT("in");
const SS::CHAR* LC_Output = TXT("out");
const SS::CHAR* LC_NextBlock = TXT("next");
const SS::CHAR* LC_UniqueID = TXT("_id_");
const SS::CHAR* LC_Doc = TXT("doc");

//Control Statements
const SS::CHAR* LC_While = TXT("while");
const SS::CHAR* LC_If    = TXT("if");
const SS::CHAR* LC_Else  = TXT("else");
const SS::CHAR* LC_Do    = TXT("do");
const SS::CHAR* LC_Then  = TXT("then");

//Declarators
const SS::CHAR* LC_Var = TXT("var");
const SS::CHAR* LC_Character = TXT("character");
const SS::CHAR* LC_Player = TXT("player");
const SS::CHAR* LC_List = TXT("list");

//Boolean constants
const SS::CHAR* LC_True = TXT("true");
const SS::CHAR* LC_False = TXT("false");

//Binary Operators
const SS::CHAR* LC_ListSeperator     = TXT(",");
const SS::CHAR* LC_Assign            = TXT("=");
const SS::CHAR* LC_Equals            = TXT("==");
const SS::CHAR* LC_NotEquals         = TXT("!=");
const SS::CHAR* LC_Minus             = TXT("-");
const SS::CHAR* LC_MinusAssign       = TXT("-=");
const SS::CHAR* LC_Plus              = TXT("+");
const SS::CHAR* LC_PlusAssign        = TXT("+=");
const SS::CHAR* LC_Times             = TXT("*");
const SS::CHAR* LC_TimesAssign       = TXT("*=");
const SS::CHAR* LC_Divide            = TXT("/");
const SS::CHAR* LC_DivideAssign      = TXT("/=");
const SS::CHAR* LC_Exponent          = TXT("^");
const SS::CHAR* LC_ExponentAssign    = TXT("^=");
//const SS::CHAR* LC_Mod               = TXT("%");
//const SS::CHAR* LC_ModAssign         = TXT("%=");
const SS::CHAR* LC_Concat            = TXT(".");
const SS::CHAR* LC_ConcatAssign      = TXT(".=");
const SS::CHAR* LC_LargerThan        = TXT(">");
const SS::CHAR* LC_LargerThanOrEqual = TXT(">=");
const SS::CHAR* LC_LessThan          = TXT("<");
const SS::CHAR* LC_LessThanOrEqual   = TXT("<=");
const SS::CHAR* LC_ScopeResolution   = TXT(":");
const SS::CHAR* LC_LogicalOr         = TXT("||");
const SS::CHAR* LC_LogicalOrAlt      = TXT("or");
const SS::CHAR* LC_LogicalAnd        = TXT("&&");
const SS::CHAR* LC_LogicalAndAlt     = TXT("and");

//Unary Operators
const SS::CHAR* LC_Not1   = TXT("not");
const SS::CHAR* LC_Not2   = TXT("!");
const SS::CHAR* LC_Negate = TXT("-");
const SS::CHAR* LC_Import = TXT("use");
const SS::CHAR* LC_UnImport = TXT("unuse");
const SS::CHAR* LC_Return = TXT("return");
const SS::CHAR* LC_Print = TXT("print");

//Misc.
const SS::CHAR* LC_Terminal = TXT(";");
const SS::CHAR* LC_NameOfBracket  = TXT("|");

//List built-ins
const SS::CHAR* LC_LIST_Pop = TXT("pop");
const SS::CHAR* LC_LIST_Push = TXT("push");
const SS::CHAR* LC_LIST_RemoveAll = TXT("removeall");
const SS::CHAR* LC_LIST_Remove = TXT("remove");


//SLib Stuff
const SS::CHAR* LC_SL_Time = TXT("SSTime");
const SS::CHAR* LC_SL_Time_Time = TXT("time");
const SS::CHAR* LC_SL_Time_ElapsedSeconds = TXT("elapsed_seconds");
const SS::CHAR* LC_SL_Time_TimeZone = TXT("timezone");
const SS::CHAR* LC_SL_Time_Date = TXT("date");

const SS::CHAR* LC_SL_Math = TXT("SSMath");

const SS::CHAR* LC_SL_Common = TXT("SSCommon");

const SS::CHAR* LC_SL_List = TXT("SSList");

const SS::CHAR* LC_SL_LangOpts = TXT("SSLangOpts");

} //end namespace SS





/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 InitConstants
 NOTES: This gets called once to set up all the necessary language constants
		such as locale info.
*/
void SS::InitConstants(){
	static bool HasBeenCalled = false;
	if( HasBeenCalled ) return;
	
	//Set up the in-language ones
	gpNANConst = CreateVariable<Variable>( TXT("_NAN_"), false, 0 );
	mpfr_set_nan( gpNANConst->GetActualNumData().get() );
	gpNANConst->ForceConversion( VARTYPE_NUM );
	gpNANConst->SetConst();
		
	gpInfinityConst = CreateVariable<Variable>( TXT("_INF_"), false, 0 );
	mpfr_set_inf( gpInfinityConst->GetActualNumData().get(), 1 );
	gpInfinityConst->ForceConversion( VARTYPE_NUM );
	gpInfinityConst->SetConst();
	
	gpNegInfinityConst = CreateVariable<Variable>( TXT("_NEGINF_"), false, 0 );
	mpfr_set_inf( gpNegInfinityConst->GetActualNumData().get(), -1 );
	gpNegInfinityConst->ForceConversion( VARTYPE_NUM );
	gpNegInfinityConst->SetConst();
	
	gpNewLineConst = CreateVariable<Variable>( TXT("endl"), true, STRING(TXT("\n")) );


	//The below part sets the decimal point and thousands seperator to
	//whatever the current locale.  This is absolutely absurd, as it
	//would create different versions of the language for different locales.
	//Decimal points and thousands seperators or going to have to constant.
	//Sorry guys: storyscript is a product of the USA.	
	/*
	lconv* pLconv = localeconv();

	if( strlen( pLconv->decimal_point ) > 0 ){
		LC_DecimalPoint = NormalizeString( pLconv->decimal_point );
	}
	LC_ThousandsSep = NormalizeString( pLconv->thousands_sep );
	*/
	
	
	//VERY IMPORTANT THAT THIS GETS SET
	gpEmptyList = CreateGeneric<List>( STRING(), true );



	//Build Maps
	gBinaryOperatorMap[ LC_ListSeperator ]     = EXTRA_BINOP_ListSeperator;
	gBinaryOperatorMap[ LC_Assign ]            = EXTRA_BINOP_Assign;
	gBinaryOperatorMap[ LC_Equals ]            = EXTRA_BINOP_Equals;
	gBinaryOperatorMap[ LC_NotEquals ]         = EXTRA_BINOP_NotEquals;
	gBinaryOperatorMap[ LC_Minus ]             = EXTRA_BINOP_Minus;
	gBinaryOperatorMap[ LC_MinusAssign ]       = EXTRA_BINOP_MinusAssign;
	gBinaryOperatorMap[ LC_Plus ]              = EXTRA_BINOP_Plus;
	gBinaryOperatorMap[ LC_PlusAssign ]        = EXTRA_BINOP_PlusAssign;
	gBinaryOperatorMap[ LC_Times ]             = EXTRA_BINOP_Times;
	gBinaryOperatorMap[ LC_TimesAssign ]       = EXTRA_BINOP_TimesAssign;
	gBinaryOperatorMap[ LC_Divide ]            = EXTRA_BINOP_Divide;
	gBinaryOperatorMap[ LC_DivideAssign ]      = EXTRA_BINOP_DivideAssign;
	gBinaryOperatorMap[ LC_Exponent ]          = EXTRA_BINOP_Exponent;
	gBinaryOperatorMap[ LC_ExponentAssign ]    = EXTRA_BINOP_ExponentAssign;
//	gBinaryOperatorMap[ LC_Mod ]               = EXTRA_BINOP_Mod;
//	gBinaryOperatorMap[ LC_ModAssign ]         = EXTRA_BINOP_ModAssign;
	gBinaryOperatorMap[ LC_Concat ]            = EXTRA_BINOP_Concat;
	gBinaryOperatorMap[ LC_ConcatAssign ]      = EXTRA_BINOP_ConcatAssign;
	gBinaryOperatorMap[ LC_LargerThan ]        = EXTRA_BINOP_LargerThan;
	gBinaryOperatorMap[ LC_LargerThanOrEqual ] = EXTRA_BINOP_LargerThanOrEqual;
	gBinaryOperatorMap[ LC_LessThan ]          = EXTRA_BINOP_LessThan;
	gBinaryOperatorMap[ LC_LessThanOrEqual ]   = EXTRA_BINOP_LessThanOrEqual;
	gBinaryOperatorMap[ LC_ScopeResolution ]   = EXTRA_BINOP_ScopeResolution;
	gBinaryOperatorMap[ LC_LogicalAnd ]        = EXTRA_BINOP_LogicalAnd;
	gBinaryOperatorMap[ LC_LogicalAndAlt ]     = EXTRA_BINOP_LogicalAnd;
	gBinaryOperatorMap[ LC_LogicalOr ]         = EXTRA_BINOP_LogicalOr;
	gBinaryOperatorMap[ LC_LogicalOrAlt ]      = EXTRA_BINOP_LogicalOr;

	gUnaryOperatorMap[ LC_Not1 ]      = EXTRA_UNOP_Not;
	gUnaryOperatorMap[ LC_Not2 ]      = EXTRA_UNOP_Not;
	gUnaryOperatorMap[ LC_Negate ]    = EXTRA_UNOP_Negative;
	gUnaryOperatorMap[ LC_Var ]       = EXTRA_UNOP_Var;
	gUnaryOperatorMap[ LC_List ]      = EXTRA_UNOP_List;
	gUnaryOperatorMap[ LC_Character ] = EXTRA_UNOP_Character;
	gUnaryOperatorMap[ LC_Player ]    = EXTRA_UNOP_Player;
	gUnaryOperatorMap[ LC_ScopeResolution ] = EXTRA_UNOP_ScopeResolution;

	gAmbigOperatorMap[ LC_Negate ] = EXTRA_AMBIGOP_Minus;
	gAmbigOperatorMap[ LC_ScopeResolution ] = EXTRA_AMBIGOP_ScopeResolution;


	//Build Reverse-Lookups - This is mainly just for outputting human readable errors.
	gBinaryOperatorReverseMap[ EXTRA_BINOP_ListSeperator ]     = LC_ListSeperator;
	gBinaryOperatorReverseMap[ EXTRA_BINOP_Assign ]            = LC_Assign;
	gBinaryOperatorReverseMap[ EXTRA_BINOP_Equals ]            = LC_Equals;
	gBinaryOperatorReverseMap[ EXTRA_BINOP_NotEquals ]         = LC_NotEquals;
	gBinaryOperatorReverseMap[ EXTRA_BINOP_Minus ]             = LC_Minus;
	gBinaryOperatorReverseMap[ EXTRA_BINOP_MinusAssign ]       = LC_MinusAssign;
	gBinaryOperatorReverseMap[ EXTRA_BINOP_Plus ]              = LC_Plus;
	gBinaryOperatorReverseMap[ EXTRA_BINOP_PlusAssign ]        = LC_PlusAssign;
	gBinaryOperatorReverseMap[ EXTRA_BINOP_Times ]             = LC_Times;
	gBinaryOperatorReverseMap[ EXTRA_BINOP_TimesAssign ]       = LC_TimesAssign;
	gBinaryOperatorReverseMap[ EXTRA_BINOP_Divide ]            = LC_Divide;
	gBinaryOperatorReverseMap[ EXTRA_BINOP_DivideAssign ]      = LC_DivideAssign;
	gBinaryOperatorReverseMap[ EXTRA_BINOP_Exponent ]          = LC_Exponent;
	gBinaryOperatorReverseMap[ EXTRA_BINOP_ExponentAssign ]    = LC_ExponentAssign;
//	gBinaryOperatorReverseMap[ EXTRA_BINOP_Mod ]               = LC_Mod;
//	gBinaryOperatorReverseMap[ EXTRA_BINOP_ModAssign ]         = LC_ModAssign;
	gBinaryOperatorReverseMap[ EXTRA_BINOP_Concat ]			   = LC_Concat;
	gBinaryOperatorReverseMap[ EXTRA_BINOP_ConcatAssign ]      = LC_ConcatAssign;
	gBinaryOperatorReverseMap[ EXTRA_BINOP_LargerThan ]        = LC_LargerThan;
	gBinaryOperatorReverseMap[ EXTRA_BINOP_LargerThanOrEqual ] = LC_LargerThanOrEqual;
	gBinaryOperatorReverseMap[ EXTRA_BINOP_LessThan ]          = LC_LessThan;
	gBinaryOperatorReverseMap[ EXTRA_BINOP_LessThanOrEqual ]   = LC_LargerThanOrEqual;
	gBinaryOperatorReverseMap[ EXTRA_BINOP_ScopeResolution ]   = LC_ScopeResolution;
	gBinaryOperatorReverseMap[ EXTRA_BINOP_LogicalAnd ]        = LC_LogicalAnd;
	gBinaryOperatorReverseMap[ EXTRA_BINOP_LogicalAnd ]        = LC_LogicalAndAlt;
	gBinaryOperatorReverseMap[ EXTRA_BINOP_LogicalOr ]         = LC_LogicalOr;
	gBinaryOperatorReverseMap[ EXTRA_BINOP_LogicalOr ]         = LC_LogicalOrAlt;

	gUnaryOperatorReverseMap[ EXTRA_UNOP_Not ]      = LC_Not1;
	gUnaryOperatorReverseMap[ EXTRA_UNOP_Negative ] = LC_Negate;
	gUnaryOperatorReverseMap[ EXTRA_UNOP_Var ]      = LC_Var;
	gUnaryOperatorReverseMap[ EXTRA_UNOP_List ]     = LC_List;
	gUnaryOperatorReverseMap[ EXTRA_UNOP_Player ]   = LC_Player;
	gUnaryOperatorReverseMap[ EXTRA_UNOP_Character] = LC_Character;
	gUnaryOperatorReverseMap[ EXTRA_UNOP_ScopeResolution ] = LC_ScopeResolution;

	gAmbigOperatorReverseMap[ EXTRA_AMBIGOP_Minus ] = LC_Negate;
	gAmbigOperatorReverseMap[ EXTRA_AMBIGOP_ScopeResolution ] = LC_ScopeResolution;


	HasBeenCalled = true;
}


