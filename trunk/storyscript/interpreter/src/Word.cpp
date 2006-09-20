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
 NOTES: Smushes the CompoundString into one simple string and returns it.
*/
SS::String SS::CollapseCompoundString( const SS::CompoundString& S )
{
	String tmp;
	size_t i = 0;
	for( i = 0; i < S.size(); i++ )
	{
			tmp += S[i];
			if( i != S.size() - 1 ) tmp += LC_ScopeResolution;
	}
	
	return tmp;	
}


SS::CompoundString SS::MakeCompoundString( const SS::String& S )
{
	return CompoundString( 1, S );	
}

SS::CompoundString SS::MakeCompoundID( const SS::String& S )
{
	
	if( S.length() == 0 ) return CompoundString();
	
	CompoundString Out;
	String tmp;
	
	size_t i;
	for( i = 0; i < S.length(); i++ )
	{
		if( S[i] != LC_ScopeResolution[0] ) tmp += S[i];
		else
		{
			Out.push_back( tmp );
			tmp.clear();
		}	
	}
	
	Out.push_back( tmp );
	
	return Out;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Word::Word
 NOTES: Constructors
*/
Word::Word()
	: Type( WORDTYPE_UNKNOWN ), Extra( EXTRA_NULL )
{
}

Word::Word( const SS::String& S,
		    WordType Type,
			ExtraDesc Extra /*=EXTRA_NULL*/ )
	: Type(Type), Extra(Extra)
{
	Str.push_back(S);
}


Word::Word( WordType Type, ExtraDesc Extra /*=EXTRA_NULL*/ )
	: Type(Type), Extra(Extra)
{
}

Word::Word( const Word& SomeWord )
	: Str(SomeWord.Str), Type(SomeWord.Type), Extra(SomeWord.Extra)
{
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
*/
Word& Word::operator=( const Word& W )
{
	Str = W.Str;
	Type = W.Type;
	Extra = W.Extra;

	return *this;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
*/
bool Word::operator==( const Word& W ) const
{
	if( Type == W.Type && Extra == W.Extra && Str == W.Str )
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
	if( Type == WORDTYPE_StringLITERAL ||
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



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Chops up a string into individual identifiers and assignes it to the
 		Word's identifier.  This function expects a well formed compound
 		identifier.  GIGO.
*/
void Word::DivideAndAssignString( const SS::String& S )
{
	if( S.length() == 0 ) return;
	Str.clear();
	String tmp;
	
	size_t i;
	for( i = 0; i < S.length(); i++ )
	{
		if( S[i] != LC_ScopeResolution[0] ) tmp += S[i];
		else
		{
			if( tmp.length() ){
				Str.push_back( tmp );
				tmp.clear();
			} 	
		}	
	}	
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Smushes the CompoundString into one simple string and returns it.
*/
SS::String Word::GetSimpleString() const
{
	return CollapseCompoundString( Str );
}

