/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: The basic script token.
*/

#include "Word.hpp"

using namespace SS;



const Word SS::EOF_WORD( WORDTYPE_EOFWORD );
const Word SS::NULL_WORD;


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Word::Word
 NOTES: Constructors
*/
Word::Word()
	: Type( WORDTYPE_UNKNOWN ), Extra( EXTRA_NULL )
{
}

Word::Word( const SS::STRING& String,
		    WordType Type,
			ExtraDesc Extra /*=EXTRA_NULL*/ )
	: String(String), Type(Type), Extra(Extra)
{
}


Word::Word( WordType Type, ExtraDesc Extra /*=EXTRA_NULL*/ )
	: Type(Type), Extra(Extra)
{
}

Word::Word( const Word& SomeWord )
	: Type(SomeWord.Type), Extra(SomeWord.Extra), String(SomeWord.String)
{
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Word::operator=
 NOTES: 
*/
Word& Word::operator=( const Word& W )
{
	String = W.String;
	Type = W.Type;
	Extra = W.Extra;

	return *this;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Word::operator==
 NOTES: 
*/
bool Word::operator==( const Word& W ) const
{
	if( Type == W.Type && Extra == W.Extra && String == W.String )
	{
		return true;
	}
	else
	{
		return false;
	}
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Word::IsLiteral
 NOTES: Returns true if the word is some sort of literal.
*/
bool Word::IsLiteral() const
{
	if( Type == WORDTYPE_STRINGLITERAL ||
		Type == WORDTYPE_BOOLLITERAL ||
		Type == WORDTYPE_FLOATLITERAL ||
		Type == WORDTYPE_EMPTYLISTLITERAL )
	{
		return true;
	}
	else return false;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Word::IsOperator
 NOTES: Returns true if the word is some sort of operator.
*/
bool Word::IsOperator() const
{
	if( Type == WORDTYPE_BINARYOPERATOR ||
		Type == WORDTYPE_UNARYOPERATOR ||
		Type == WORDTYPE_AMBIGUOUSOPERATOR )
	{
		return true;
	}
	else return false;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Word::InterpretAs____Op
 NOTES: These two function are for converting ambigous operators.
		It may be a bit dubious to mark these as const, but it makes things much
		easier.
*/
void Word::InterpretAsBinaryOp() const
{
	if( Type != WORDTYPE_AMBIGUOUSOPERATOR ) return;

	Type = WORDTYPE_BINARYOPERATOR;
	Extra = gBinaryOperatorMap[ gAmbigOperatorReverseMap[Extra] ];
}

void Word::InterpretAsUnaryOp() const
{
	if( Type != WORDTYPE_AMBIGUOUSOPERATOR ) return;

	Type = WORDTYPE_UNARYOPERATOR;
	Extra = gUnaryOperatorMap[ gAmbigOperatorReverseMap[Extra] ];

}



