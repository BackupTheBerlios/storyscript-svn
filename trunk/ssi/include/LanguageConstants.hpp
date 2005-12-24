/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: This is a list of language constants that can be changed to effect
	in the source file to rename operators, etc.
*/

#if !defined( SS_LanguageConstants )
#define SS_LanguageConstants

#include "Unicode.hpp"
#include <map>


namespace SS{


//Call this once to set up all the locale info and such
void InitConstants();

////////////////////////////////////////////////////////////////////////////////
/*
You can sort of think of the next two structure as the "bytecode" that 
SS gets translated into
*/
enum WordType
{
	WORDTYPE_EOFWORD, //ExtractWord output this as a friendly way of saying its at the end of the file.
	WORDTYPE_UNKNOWN,//
	WORDTYPE_IDENTIFIER,
	WORDTYPE_FLOATLITERAL,//
	WORDTYPE_STRINGLITERAL,
	WORDTYPE_BOOLLITERAL,
	WORDTYPE_EMPTYLISTLITERAL,
	WORDTYPE_BINARYOPERATOR,//
	WORDTYPE_UNARYOPERATOR,
	WORDTYPE_AMBIGUOUSOPERATOR,
	WORDTYPE_BRACKET,
	WORDTYPE_PARENTHESIS,
	WORDTYPE_CONTROL,
	WORDTYPE_TERMINAL
};

/*
This structure provides more specific information than
WordType.  This is to minimize the need to work with strings.
*/
enum ExtraDesc
{
	EXTRA_NULL,

	EXTRA_BINOP_ListAccess,
	EXTRA_BINOP_ListAppend,
	EXTRA_BINOP_ListRemove,
	EXTRA_BINOP_ListSeperator,
	EXTRA_BINOP_Assign,
	EXTRA_BINOP_Equals,
	EXTRA_BINOP_NotEquals,
	EXTRA_BINOP_Minus,
	EXTRA_BINOP_MinusAssign,
	EXTRA_BINOP_Plus,
	EXTRA_BINOP_PlusAssign,
	EXTRA_BINOP_Times,
	EXTRA_BINOP_TimesAssign,
	EXTRA_BINOP_Divide,
	EXTRA_BINOP_DivideAssign,
	EXTRA_BINOP_Exponent,
	EXTRA_BINOP_ExponentAssign,
//	EXTRA_BINOP_Mod,
//	EXTRA_BINOP_ModAssign,
	EXTRA_BINOP_Concat,
	EXTRA_BINOP_ConcatAssign,
	EXTRA_BINOP_LargerThan,
	EXTRA_BINOP_LargerThanOrEqual,
	EXTRA_BINOP_LessThan,
	EXTRA_BINOP_LessThanOrEqual,
	EXTRA_BINOP_ScopeResolution,
	EXTRA_BINOP_LogicalAnd,
	EXTRA_BINOP_LogicalOr,

	EXTRA_UNOP_GenericUnaryOperator,
	EXTRA_UNOP_Not,
	EXTRA_UNOP_Import,
	EXTRA_UNOP_Negative,
	EXTRA_UNOP_Var,
	EXTRA_UNOP_Character,
	EXTRA_UNOP_Player,
	EXTRA_UNOP_List,

	EXTRA_AMBIGOP_Minus,

	EXTRA_TERMINAL,

    EXTRA_PARENTHESIS_Left,
	EXTRA_PARENTHESIS_Right,

	EXTRA_BOOLLITERAL_True,
	EXTRA_BOOLLITERAL_False,

	EXTRA_BRACKET_Left,
	EXTRA_BRACKET_Right,

	EXTRA_CONTROL_While,
	EXTRA_CONTROL_If,
	EXTRA_CONTROL_Else,
	EXTRA_CONTROL_Do,
	EXTRA_CONTROL_Static
};


extern std::map< SS::STRING, ExtraDesc > gUnaryOperatorMap;
extern std::map< SS::STRING, ExtraDesc > gBinaryOperatorMap;

extern std::map< ExtraDesc, SS::STRING > gUnaryOperatorReverseMap;
extern std::map< ExtraDesc, SS::STRING > gBinaryOperatorReverseMap;

extern std::map< SS::STRING, ExtraDesc > gAmbigOperatorMap;
extern std::map< ExtraDesc, SS::STRING > gAmbigOperatorReverseMap;


//Special Variable Names
extern const SS::STRING LC_Precision;


extern const SS::STRING LC_Length;
extern const SS::STRING LC_Name;
extern const SS::STRING LC_FullName;
extern const SS::STRING LC_EndBlock;
extern const SS::STRING LC_Static;
extern const SS::STRING LC_BeenSaid;
extern const SS::STRING LC_Input;
SS_API extern const SS::STRING LC_Output;
extern const SS::STRING LC_NextBlock;
extern const SS::STRING LC_UniqueID;

//Control Statements
extern const SS::STRING LC_While;
extern const SS::STRING LC_If;
extern const SS::STRING LC_Else;
extern const SS::STRING LC_Do;
extern const SS::STRING LC_Then;

//Declarators
extern const SS::STRING LC_Var;
extern const SS::STRING LC_Character;
extern const SS::STRING LC_Player;
extern const SS::STRING LC_List;

//Boolean constants
extern const SS::STRING LC_True;
extern const SS::STRING LC_False;

//Binary Operators
extern const SS::STRING LC_ListSeperator;
extern const SS::STRING LC_Assign;
extern const SS::STRING LC_Equals;
extern const SS::STRING LC_NotEquals;
extern const SS::STRING LC_Minus;
extern const SS::STRING LC_MinusAssign;
extern const SS::STRING LC_Plus;
extern const SS::STRING LC_PlusAssign;
extern const SS::STRING LC_Times;
extern const SS::STRING LC_TimesAssign;
extern const SS::STRING LC_Divide;
extern const SS::STRING LC_DivideAssign;
extern const SS::STRING LC_Exponent;
extern const SS::STRING LC_ExponentAssign;
//extern const SS::STRING LC_Mod;
//extern const SS::STRING LC_ModAssign;
extern const SS::STRING LC_Concat;
extern const SS::STRING LC_ConcatAssign;
extern const SS::STRING LC_LargerThan;
extern const SS::STRING LC_LargerThanOrEqual;
extern const SS::STRING LC_LessThan;
extern const SS::STRING LC_LessThanOrEqual;
extern const SS::STRING LC_ScopeResolution;
extern const SS::STRING LC_LogicalOr;
extern const SS::STRING LC_LogicalOrAlt;
extern const SS::STRING LC_LogicalAnd;
extern const SS::STRING LC_LogicalAndAlt;

//Unary Operators
extern const SS::STRING LC_Not1;
extern const SS::STRING LC_Not2;
extern const SS::STRING LC_Negate;
extern const SS::STRING LC_Import;

//Misc.
extern const SS::STRING LC_Terminal;
extern const SS::STRING LC_NameOfBracket;

//These are just defaults.  They may be overridden by InitConstants
extern SS::STRING LC_DecimalPoint;
extern SS::STRING LC_ThousandsSep;


//SLib Stuff
extern const SS::STRING LC_SL_Time;
extern const SS::STRING LC_SL_Time_Time;
extern const SS::STRING LC_SL_Time_ElapsedSeconds;
extern const SS::STRING LC_SL_Time_TimeZone;
extern const SS::STRING LC_SL_Time_Date;

extern const SS::STRING LC_SL_Math;

extern const SS::STRING LC_SL_Common;

extern const SS::STRING LC_SL_List;


}
#endif

