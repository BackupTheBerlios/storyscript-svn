/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: The basic script token.
*/

#if !defined(SS_Word)
#define SS_Word

#include "Unicode.hpp"
#include "LanguageConstants.hpp"
#include "DLLExportString.hpp"

namespace SS{


typedef std::vector<SS::STRING> CompoundString;


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 Word
 NOTES: Once ScriptFile breaks up the file, it puts the 
		individual language elements into these objects.
*/
class Word
{
public:
	Word();
	Word( const SS::STRING& String, WordType Type, ExtraDesc Extra = EXTRA_NULL );
	Word( WordType Type, ExtraDesc Extra = EXTRA_NULL );
	Word( const Word& );
	
	void DivideAndAssignString( const SS::STRING& );
	SS::STRING GetSimpleString() const;
	
	virtual ~Word() {}

	Word& operator=( const Word& W );
	bool operator==( const Word& W ) const;
	bool operator!() const;
	
	bool IsLiteral() const;
	bool IsOperator() const;
	void InterpretAsBinaryOp() const;
	void InterpretAsUnaryOp() const;

	CompoundString String;
	mutable WordType Type;
	mutable ExtraDesc Extra;

	

};

SS::CompoundString MakeCompoundString( const SS::STRING& S );
SS::CompoundString MakeCompoundID( const SS::STRING& S);

SS::STRING CollapseCompoundString( const SS::CompoundString& );

//Defined in Word.cpp:
extern const Word EOF_WORD;
extern const Word NULL_WORD;



} //end namespace SS
#endif

