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
		CHARTYPE_DEFAULT,
		CHARTYPE_ALPHABETICAL,
		CHARTYPE_NUMERICAL,
		CHARTYPE_OPERATOR,
		CHARTYPE_WHITESPACE,
		CHARTYPE_SPECIAL,
		CHARTYPE_STRING,
		CHARTYPE_PUNCTUATION
	};
	
	
	SS::STRING MakeScopeNameFromFileName( const SS::STRING& FileName );


	SS::STRING SS_API BreakOffFirstID( SS::STRING& );
	SS::STRING SS_API BreakOffLastID ( SS::STRING& ); 

	void SS_API BreakOffLastID( const SS::STRING& src, CHAR* dest, unsigned int DestSize );
	void SS_API BreakOffFirstID( const SS::STRING& src, CHAR* dest, unsigned int DestSize );

	bool IsNewline( SS::CHAR );
	bool IsNumber( SS::CHAR );
	bool IsBinaryOperator( SS::CHAR );
	bool IsBinaryOperator( const SS::STRING& );
	bool IsUnaryOperator( SS::CHAR );
	bool IsUnaryOperator( const SS::STRING& );
	bool IsSpecial( SS::CHAR );
	bool IsWhitespace( SS::CHAR );
	bool IsAlpha( SS::CHAR );

	CharType GetCharType( SS::CHAR );


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

