/*
LanguageConstants.cpp
----------------------

Created: 6:5:2005   17:39
*/


#include "LanguageConstants.hpp"

//To initialize gpEmptyList
#include "CreationFuncs.hpp"
#include "List.hpp"

using namespace SS;

#include <clocale>

namespace SS{
std::map< SS::STRING, ExtraDesc > gUnaryOperatorMap;
std::map< SS::STRING, ExtraDesc > gBinaryOperatorMap;
std::map< SS::STRING, ExtraDesc > gAmbigOperatorMap;

std::map< ExtraDesc, SS::STRING > gUnaryOperatorReverseMap;
std::map< ExtraDesc, SS::STRING > gBinaryOperatorReverseMap;
std::map< ExtraDesc, SS::STRING > gAmbigOperatorReverseMap;

//Default Values (i.e. They may be overridden by InitConstants)
SS::STRING LC_DecimalPoint = TXT(".");
SS::STRING LC_ThousandsSep = TXT(",");

//Special Variable Names
const SS::STRING LC_Precision = TXT("precision");


const SS::STRING LC_Length = TXT("length");
const SS::STRING LC_Name( TXT("name") );
const SS::STRING LC_FullName( TXT("fullname") );
const SS::STRING LC_EndBlock = TXT("end");
const SS::STRING LC_Static = TXT("static");
const SS::STRING LC_BeenSaid = TXT("BeenSaid");
const SS::STRING LC_Input = TXT("in");
const SS::STRING LC_Output = TXT("out");
const SS::STRING LC_NextBlock = TXT("next");
const SS::STRING LC_UniqueID = TXT("_id_");

//Control Statements
const SS::STRING LC_While = TXT("while");
const SS::STRING LC_If    = TXT("if");
const SS::STRING LC_Else  = TXT("else");
const SS::STRING LC_Do    = TXT("do");
const SS::STRING LC_Then  = TXT("then");

//Declarators
const SS::STRING LC_Var = TXT("var");
const SS::STRING LC_Character = TXT("character");
const SS::STRING LC_Player = TXT("player");
const SS::STRING LC_List = TXT("list");

//Boolean constants
const SS::STRING LC_True = TXT("true");
const SS::STRING LC_False = TXT("false");

//Binary Operators
const SS::STRING LC_ListSeperator     = TXT(",");
const SS::STRING LC_Assign            = TXT("=");
const SS::STRING LC_Equals            = TXT("==");
const SS::STRING LC_NotEquals         = TXT("!=");
const SS::STRING LC_Minus             = TXT("-");
const SS::STRING LC_MinusAssign       = TXT("-=");
const SS::STRING LC_Plus              = TXT("+");
const SS::STRING LC_PlusAssign        = TXT("+=");
const SS::STRING LC_Times             = TXT("*");
const SS::STRING LC_TimesAssign       = TXT("*=");
const SS::STRING LC_Divide            = TXT("/");
const SS::STRING LC_DivideAssign      = TXT("/=");
const SS::STRING LC_Exponent          = TXT("^");
const SS::STRING LC_ExponentAssign    = TXT("^=");
//const SS::STRING LC_Mod               = TXT("%");
//const SS::STRING LC_ModAssign         = TXT("%=");
const SS::STRING LC_Concat            = TXT(".");
const SS::STRING LC_ConcatAssign      = TXT(".=");
const SS::STRING LC_LargerThan        = TXT(">");
const SS::STRING LC_LargerThanOrEqual = TXT(">=");
const SS::STRING LC_LessThan          = TXT("<");
const SS::STRING LC_LessThanOrEqual   = TXT("<=");
const SS::STRING LC_ScopeResolution   = TXT(":");
const SS::STRING LC_LogicalOr         = TXT("||");
const SS::STRING LC_LogicalOrAlt      = TXT("or");
const SS::STRING LC_LogicalAnd        = TXT("&&");
const SS::STRING LC_LogicalAndAlt     = TXT("and");

//Unary Operators
const SS::STRING LC_Not1   = TXT("not");
const SS::STRING LC_Not2   = TXT("!");
const SS::STRING LC_Negate = TXT("-");
const SS::STRING LC_Import = TXT("use");

//Misc.
const SS::STRING LC_Terminal = TXT(";");
const SS::STRING LC_NameOfBracket  = TXT("|");


//SLib Stuff
const SS::STRING LC_SL_Time = TXT("SSTime");
const SS::STRING LC_SL_Time_Time = TXT("CurrentTime");
const SS::STRING LC_SL_Time_ElapsedSeconds = TXT("ElapsedSeconds");
const SS::STRING LC_SL_Time_TimeZone = TXT("TimeZone");
const SS::STRING LC_SL_Time_Date = TXT("Date");

const SS::STRING LC_SL_Math = TXT("SSMath");

const SS::STRING LC_SL_Common = TXT("SSCommon");

const SS::STRING LC_SL_List = TXT("SSList");

} //end namespace SS





/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 InitConstants
 NOTES: This gets called once to set up all the necessary language constants
		such as locale info.
*/
void SS::InitConstants(){
	static bool HasBeenCalled = false;
	if( HasBeenCalled ) return;

	lconv* pLconv = localeconv();

	if( strlen( pLconv->decimal_point ) > 0 ){
		LC_DecimalPoint = NormalizeString( pLconv->decimal_point );
	}
	LC_ThousandsSep = NormalizeString( pLconv->thousands_sep );
	
	
	//VERY IMPORTANT THAT THIS GETS SET
	gpEmptyList = CreateObject<List>( STRING(), true, true );



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

	gUnaryOperatorMap[ LC_Import ]    = EXTRA_UNOP_Import;
	gUnaryOperatorMap[ LC_Not1 ]      = EXTRA_UNOP_Not;
	gUnaryOperatorMap[ LC_Not2 ]      = EXTRA_UNOP_Not;
	gUnaryOperatorMap[ LC_Negate ]    = EXTRA_UNOP_Negative;
	gUnaryOperatorMap[ LC_Var ]       = EXTRA_UNOP_Var;
	gUnaryOperatorMap[ LC_List ]      = EXTRA_UNOP_List;
	gUnaryOperatorMap[ LC_Character ] = EXTRA_UNOP_Character;
	gUnaryOperatorMap[ LC_Player ]    = EXTRA_UNOP_Player;

	gAmbigOperatorMap[ LC_Negate ] = EXTRA_AMBIGOP_Minus;


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

	gUnaryOperatorReverseMap[ EXTRA_UNOP_Import ]   = LC_Import;
	gUnaryOperatorReverseMap[ EXTRA_UNOP_Not ]      = LC_Not1;
	gUnaryOperatorReverseMap[ EXTRA_UNOP_Negative ] = LC_Negate;
	gUnaryOperatorReverseMap[ EXTRA_UNOP_Var ]      = LC_Var;
	gUnaryOperatorReverseMap[ EXTRA_UNOP_List ]     = LC_List;
	gUnaryOperatorReverseMap[ EXTRA_UNOP_Player ]   = LC_Player;
	gUnaryOperatorReverseMap[ EXTRA_UNOP_Character] = LC_Character;

	gAmbigOperatorReverseMap[ EXTRA_AMBIGOP_Minus ] = LC_Negate;


	HasBeenCalled = true;
}


