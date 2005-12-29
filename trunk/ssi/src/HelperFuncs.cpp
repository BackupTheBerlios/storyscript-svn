/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: Contains misc. functions that I haven't found a better place for yet.
*/

#include "HelperFuncs.hpp"
#include "Unicode.hpp"
#include "LanguageConstants.hpp"



using namespace SS;



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Mangles a file name into a valid scope name.
*/
SS::STRING SS::MakeScopeNameFromFileName( const SS::STRING& FileName )
{
	STRING ScopeName;

	unsigned int i;
	for( i = 0; i < FileName.length(); i++ )
	{
		//Can't start with a number; replace "3Jane" with "_3Jane"
		if( i == 0 && IsNumber( FileName[0] ) ){
			ScopeName += '_';
			ScopeName += FileName[0];
		}
		//If the character isn't a number or letter, just stick a "_" in there
		else if( !IsAlpha( FileName[i] ) && !IsNumber( FileName[i] ) ){
			ScopeName += '_';
		}
		//Otherwise go ahead any use whats there
		else{
			ScopeName += FileName[i];
		}
	}
	
	return ScopeName;
}



/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 BreakOffFirstID
 NOTES:  With something like :Foo:Bar, it will return "Foo" and S will be "Bar".
*/
SS::STRING SS::BreakOffFirstID( SS::STRING& S )
{
	SS::STRING TempString;
	
	size_t i = 0;

	if( S[0] == LC_ScopeResolution[0] ) i++;
	
	for( ; i < S.size(); i++ ){
		if( IsAlpha(S[i]) || IsNumber(S[i]) || S[i] == '_' ){
			TempString += S[i];
		}	
	}

	//remove the excess fat
	while( i < S.size() && !(IsAlpha(S[i]) || IsNumber(S[i]) || S[i] == '_') )	{
		i++;
	}
	
	if( i ) S.erase( 0, i );

	return TempString;
}

//This version should be faster, I think
SS::STRING SS::BreakOffFirstID( const SS::STRING& S, SS::STRING& Remainder )
{
	SS::STRING TempString;
	
	size_t i = 0;

	if( S[0] == LC_ScopeResolution[0] ) i++;
	
	for( ; i < S.size(); i++ ){
		if( IsAlpha(S[i]) || IsNumber(S[i]) || S[i] == '_' ){
			TempString += S[i];
		}	
	}

	//remove the excess fat
	while( i < S.size() && !(IsAlpha(S[i]) || IsNumber(S[i]) || S[i] == '_') )	{
		i++;
	}

	if( i < S.size() ) Remainder.assign( S, i+1, S.size() );

	return TempString;	
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: This version is different.  Besides it taking a c-style string, it doesn't
		modify the source.  This version is safe for strange linking stuff.
*/
void SS::BreakOffLastID( const SS::STRING& src, CHAR* dest, unsigned int DestSize )
{
	SS::STRING SrcCpy = src;
	BreakOffFirstID( SrcCpy );

	SS::STRCPY( dest, SrcCpy.c_str(), DestSize );
}

void SS::BreakOffFirstID( const SS::STRING& src, CHAR* dest, unsigned int DestSize )
{
	SS::STRING SrcCpy = src;
	BreakOffFirstID( SrcCpy );

	SS::STRCPY( dest, SrcCpy.c_str(), DestSize );
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 BreakOffLastID
 NOTES: 
*/
SS::STRING SS::BreakOffLastID( SS::STRING& S )
{
	SS::STRING TempString;

	while( !S.empty() && 
		(IsAlpha( S[S.length()-1] ) || IsNumber( S[S.length()-1] ) || S[S.length()-1] == '_') )
	{
		//TempString.insert(0, S[S.length()-1]);
		TempString.insert( 0, 1, S[S.length()-1] );
		S.erase( S.length()-1, 1 );
	}

	//remove the excess fat
	while( !S.empty() &&
		!(IsAlpha( S[S.length()-1] ) || IsNumber( S[S.length()-1] ) || S[S.length()-1] == '_') )
	{
		S.erase( S.length()-1, 1 );
	}

	return TempString;
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 GetCharType
 NOTES: 
*/
CharType SS::GetCharType( CHAR c )
{
	if( IsNumber( c ) )       return CHARTYPE_NUMERICAL;
	else if( IsBinaryOperator( c ) || IsUnaryOperator( c ) )   return CHARTYPE_OPERATOR;
	else if( IsWhitespace( c ) ) return CHARTYPE_WHITESPACE;
	else if( IsAlpha( c ) ) return CHARTYPE_ALPHABETICAL;
	else                         return CHARTYPE_SPECIAL;
}




bool SS::IsNewline( CHAR c )
{
	return (c == '\n' || c == '\r') ? true : false;
}

bool SS::IsNumber( CHAR c )
{
	if( (('0' <= c) && (c <= '9')) /*|| c == '.'*/ ) return true;
	else return false;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 IsBinaryOperator
 NOTES: Returns true if the string is a binary operator, and false otherwise.
		The version that takes a single char returns true if that character is 
		a character that appears in the first char of an operator.
*/
bool SS::IsBinaryOperator( const SS::STRING& c )
{
	if( gBinaryOperatorMap.find( c ) != gBinaryOperatorMap.end() ) return true;
	else return false;
}

//Checks single characters
bool SS::IsBinaryOperator( CHAR c )
{
	std::map< ExtraDesc, SS::STRING >::iterator i;
	
	for( i = gBinaryOperatorReverseMap.begin(); i != gBinaryOperatorReverseMap.end(); i++ )	{
		if( (*i).second[0] == c ) return true;
	}
	
	return false;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 IsUnaryOperator
 NOTES: See the above (IsBinaryOperator)
*/
bool SS::IsUnaryOperator( const SS::STRING& c )
{
	if( gUnaryOperatorMap.find( c ) != gUnaryOperatorMap.end() ) return true;
	else return false;
}

bool SS::IsUnaryOperator( CHAR c )
{
	std::map< ExtraDesc, SS::STRING >::iterator i;
	
	for( i = gUnaryOperatorReverseMap.begin(); i != gUnaryOperatorReverseMap.end(); i++ )	{
		if( (*i).second[0] == c ) return true;	
	}
	
	return false;
}



bool SS::IsSpecial( CHAR c )
{
	switch( c )
	{
	case '{':
	case '}':
	case '(':
	case ')':
		return true;
	default:
		return false;
	}
}

bool SS::IsWhitespace( CHAR c )
{
	if( IsNewline(c) || c == ' ' || c == '\t' ) return true;
	else return false;

}

bool SS::IsAlpha( CHAR c )
{
	if( ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') ) return true;
	else return false;
}








/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 IsSingleListStatement
 NOTES: Returns true if the expression is a single list statement, such as foo[42].  
*/
/*
bool SS::IsSingleListStatement( const Expression& ExpressionBuffer )
{
	//A list satement must start with one and only one identifier

	if( ExpressionBuffer.size() >= 3 &&
		ExpressionBuffer[0].Type == WORDTYPE_IDENTIFIER &&
		ExpressionBuffer[1].SS::STRING == TXT("[") &&
		ExpressionBuffer.back().SS::STRING == TXT("]") )
	{
		//if there are no other ]'s, than return true
		int i;
		for( i = 0; i < ExpressionBuffer.size()-1; i++ ){
			if(

	}
	else return false;



}
*/

