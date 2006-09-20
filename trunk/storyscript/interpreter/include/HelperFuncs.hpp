/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: Contains misc. functions that I haven't found a better place for yet.
*/

#include "Unicode.hpp"

#if !defined(SS_HelperFuncs)
#define SS_HelperFuncs

//

namespace SS{
	enum CharType
	{
		CharTYPE_DEFAULT,
		CharTYPE_ALPHABETICAL,
		CharTYPE_NUMERICAL,
		CharTYPE_OPERATOR,
		CharTYPE_WHITESPACE,
		CharTYPE_SPECIAL,
		CharTYPE_String,
		CharTYPE_PUNCTUATION
	};
	
	
	SS::String MakeScopeNameFromFileName( const SS::String& FileName );


	SS::String SS_API BreakOffFirstID( SS::String& );
	SS::String SS_API BreakOffFirstID( const SS::String& Original, SS::String& Remainder );
	SS::String SS_API BreakOffLastID ( SS::String& ); 

	void SS_API BreakOffLastID( const SS::String& src, Char* dest, unsigned int DestSize );
	void SS_API BreakOffFirstID( const SS::String& src, Char* dest, unsigned int DestSize );

	bool IsNewline( SS::Char );
	bool IsNumber( SS::Char );
	bool IsBinaryOperator( SS::Char );
	bool IsBinaryOperator( const SS::String& );
	bool IsUnaryOperator( SS::Char );
	bool IsUnaryOperator( const SS::String& );
	bool IsSpecial( SS::Char );
	bool IsWhitespace( SS::Char );
	bool IsAlpha( SS::Char );

	CharType GetCharType( SS::Char );


	//bool IsSingleListStatement( const Expression& );
	
	//Totally out there...
	unsigned int RoundAndCast( double );
	
	template< typename NewT, typename OldT >
	NewT RoundAndCast( OldT x )
	{
		NewT Result = (NewT)x;
		if( (OldT)Result - x  >= 0.5 ) Result++;
		
		return Result;
	}


} //namespace SS





#endif

