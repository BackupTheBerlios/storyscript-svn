/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: This is a list of language constants that can be changed to effect
	in the source file to rename operators, etc.
*/

#if !defined( SS_LanguageConstants )
#define SS_LanguageConstants

#include "Unicode.hpp"
#include <boost/shared_ptr.hpp>
#include <map>
#include "Types.hpp"


namespace SS{
	
	
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: A singleton containing important variable affecting language behavior.
*/
struct LangOpts
{
public:
	static LangOpts& Instance();
	
	unsigned long DefaultPrecision;
	mpfr_rnd_t RoundingMode;
	unsigned long MaxDigitOutput;
	unsigned long NumberBase;
	bool UseStrictLists;
	bool Verbose;
	
			

private:
	LangOpts();
	static boost::shared_ptr<LangOpts> mpInstance;
	
};


/*
	EXTREMELY IMPORTANT:
	These are language constants that will be registered in the global scope.
*/
extern VariablePtr gpNANConst;
extern VariablePtr gpInfinityConst;
extern VariablePtr gpNegInfinityConst;
extern VariablePtr gpNewLineConst;


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
	WORDTYPE_StringLITERAL,
	WORDTYPE_BOOLLITERAL,
	WORDTYPE_EMPTYLISTLITERAL,
	WORDTYPE_BINARYOPERATOR,//
	WORDTYPE_UNARYOPERATOR,
	WORDTYPE_AMBIGUOUSOPERATOR,
	WORDTYPE_BRACKET,
	WORDTYPE_PARENTHESIS,
	WORDTYPE_CONTROL,
	WORDTYPE_TERMINAL,
	WORDTYPE_DOCString
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
	EXTRA_UNOP_Negative,
	EXTRA_UNOP_Var,
	EXTRA_UNOP_Character,
	EXTRA_UNOP_Player,
	EXTRA_UNOP_List,
	EXTRA_UNOP_ScopeResolution,

	EXTRA_AMBIGOP_Minus,
	EXTRA_AMBIGOP_ScopeResolution,

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

struct ltstr{
	bool operator()( const SS::Char* a, const SS::Char* b ) const;
};

extern std::map< const SS::Char*, ExtraDesc, ltstr > gUnaryOperatorMap;
extern std::map< const SS::Char*, ExtraDesc, ltstr > gBinaryOperatorMap;
extern std::map< const SS::Char*, ExtraDesc, ltstr > gAmbigOperatorMap;

extern std::map< ExtraDesc, const SS::Char* > gUnaryOperatorReverseMap;
extern std::map< ExtraDesc, const SS::Char* > gBinaryOperatorReverseMap;
extern std::map< ExtraDesc, const SS::Char* > gAmbigOperatorReverseMap;


//Special Variable Names
extern const SS::Char* LC_Precision;


extern const SS::Char* LC_Length;
extern const SS::Char* LC_Name;
extern const SS::Char* LC_FullName;
extern const SS::Char* LC_EndBlock;
extern const SS::Char* LC_Static;
extern const SS::Char* LC_BeenSaid;
extern const SS::Char* LC_Input;
extern const SS::Char* LC_Output;
extern const SS::Char* LC_NextBlock;
extern const SS::Char* LC_UniqueID;
extern const SS::Char* LC_Doc;

//Control Statements
extern const SS::Char* LC_While;
extern const SS::Char* LC_If;
extern const SS::Char* LC_Else;
extern const SS::Char* LC_Do;
extern const SS::Char* LC_Then;

//Declarators
extern const SS::Char* LC_Var;
extern const SS::Char* LC_Character;
extern const SS::Char* LC_Player;
extern const SS::Char* LC_List;

//Boolean constants
extern const SS::Char* LC_True;
extern const SS::Char* LC_False;

//Binary Operators
extern const SS::Char* LC_ListSeperator;
extern const SS::Char* LC_Assign;
extern const SS::Char* LC_Equals;
extern const SS::Char* LC_NotEquals;
extern const SS::Char* LC_Minus;
extern const SS::Char* LC_MinusAssign;
extern const SS::Char* LC_Plus;
extern const SS::Char* LC_PlusAssign;
extern const SS::Char* LC_Times;
extern const SS::Char* LC_TimesAssign;
extern const SS::Char* LC_Divide;
extern const SS::Char* LC_DivideAssign;
extern const SS::Char* LC_Exponent;
extern const SS::Char* LC_ExponentAssign;
//extern const SS::String LC_Mod;
//extern const SS::String LC_ModAssign;
extern const SS::Char* LC_Concat;
extern const SS::Char* LC_ConcatAssign;
extern const SS::Char* LC_LargerThan;
extern const SS::Char* LC_LargerThanOrEqual;
extern const SS::Char* LC_LessThan;
extern const SS::Char* LC_LessThanOrEqual;
extern const SS::Char* LC_ScopeResolution;
extern const SS::Char* LC_LogicalOr;
extern const SS::Char* LC_LogicalOrAlt;
extern const SS::Char* LC_LogicalAnd;
extern const SS::Char* LC_LogicalAndAlt;

//Unary Operators
extern const SS::Char* LC_Not1;
extern const SS::Char* LC_Not2;
extern const SS::Char* LC_Negate;
extern const SS::Char* LC_Import;
extern const SS::Char* LC_UnImport;
extern const SS::Char* LC_Return;
extern const SS::Char* LC_Print;
extern const SS::Char* LC_PrintF;

//Misc.
extern const SS::Char* LC_Terminal;
extern const SS::Char* LC_NameOfBracket;


extern const SS::Char* LC_DecimalPoint;
extern const SS::Char* LC_ThousandsSep;


//List built-ins
extern const SS::Char* LC_LIST_Pop;
extern const SS::Char* LC_LIST_Push;
extern const SS::Char* LC_LIST_RemoveAll;
extern const SS::Char* LC_LIST_Remove;


//SLib Stuff
extern const SS::Char* LC_SL_Time;
extern const SS::Char* LC_SL_Time_Time;
extern const SS::Char* LC_SL_Time_ElapsedSeconds;
extern const SS::Char* LC_SL_Time_TimeZone;
extern const SS::Char* LC_SL_Time_Date;

extern const SS::Char* LC_SL_Math;

extern const SS::Char* LC_SL_Common;

extern const SS::Char* LC_SL_List;

extern const SS::Char* LC_SL_LangOpts;


}
#endif

