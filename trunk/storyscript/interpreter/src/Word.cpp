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
SS::STRING SS::CollapseCompoundString( const SS::CompoundString& String )
{
	STRING tmp;
	size_t i = 0;
	for( i = 0; i < String.size(); i++ )
	{
			tmp += String[i];
			if( i != String.size() - 1 ) tmp += LC_ScopeResolution;
	}
	
	return tmp;	
}


SS::CompoundString SS::MakeCompoundString( const SS::STRING& S )
{
	return CompoundString( 1, S );	
}

SS::CompoundString SS::MakeCompoundID( const SS::STRING& S )
{
	
	if( S.length() == 0 ) return CompoundString();
	
	CompoundString String;
	STRING tmp;
	
	size_t i;
	for( i = 0; i < S.length(); i++ )
	{
		if( S[i] != LC_ScopeResolution[0] ) tmp += S[i];
		else
		{
			String.push_back( tmp );
			tmp.clear();
		}	
	}
	
	String.push_back( tmp );
	
	return String;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Word::Word
 NOTES: Constructors
*/
Word::Word()
	: Type( WORDTYPE_UNKNOWN ), Extra( EXTRA_NULL )
{
}

Word::Word( const SS::STRING& S,
		    WordType Type,
			ExtraDesc Extra /*=EXTRA_NULL*/ )
	: Type(Type), Extra(Extra)
{
	String.push_back(S);
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



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Chops up a string into individual identifiers and assignes it to the
 		Word's identifier.  This function expects a well formed compound
 		identifier.  GIGO.
*/
void Word::DivideAndAssignString( const SS::STRING& S )
{
	if( S.length() == 0 ) return;
	String.clear();
	STRING tmp;
	
	size_t i;
	for( i = 0; i < S.length(); i++ )
	{
		if( S[i] != LC_ScopeResolution[0] ) tmp += S[i];
		else
		{
			if( tmp.length() ){
				String.push_back( tmp );
				tmp.clear();
			} 	
		}	
	}	
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Smushes the CompoundString into one simple string and returns it.
*/
SS::STRING Word::GetSimpleString() const
{
	return CollapseCompoundString( String );
}

