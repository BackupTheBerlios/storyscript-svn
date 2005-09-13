/*
    Word.hpp
    --------------------------
    

	Created:	1:7:2005   14:25
*/

#if !defined(SS_Word)
#define SS_Word

#include "Unicode.hpp"
#include "LanguageConstants.hpp"
#include "DLLExportString.hpp"

namespace SS{

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

	Word& operator=( const Word& W );
	bool operator==( const Word& W ) const;
	bool operator!() const;
	
	bool IsLiteral() const;
	bool IsOperator() const;
	void InterpretAsBinaryOp() const;
	void InterpretAsUnaryOp() const;

	SS::STRING String;
	mutable WordType Type;
	mutable ExtraDesc Extra;
};

//Defined in Word.cpp:
extern const Word EOF_WORD;
extern const Word NULL_WORD;



} //end namespace SS
#endif

