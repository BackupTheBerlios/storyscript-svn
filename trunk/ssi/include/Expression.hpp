/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: Contains declarations for the Expression class which is responsible for
	interpreting and executing expressions.
*/


#if !defined(SS_Expression)
#define SS_Expression

//#include "ScriptFile.hpp"
#include "Word.hpp"
#include "Variable.hpp"
#include <vector>

namespace SS{

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 Expression
 NOTES: The expression class.
*/
class Expression
{
public:
	Expression();
	Expression( const Expression&,
				unsigned int LowerBounds = 0,
			    unsigned int UpperBounds = ~0U );

	Expression& operator=( const Expression& );
	
	bool IsStatic() const;
	void SetStatic( bool flag = true ) const;

	//These mimic the functions in std::vector
	Word& operator[]( unsigned long );
	const Word& operator[]( unsigned long ) const;
	void clear();
	void push_back( const Word& );
	void pop_back();
	bool empty() const;
	unsigned long size() const;
	void erase( unsigned long );

	//Here's the important stuff
	VariableBasePointer Evaluate() const;

private:
	//Expression( std::vector<Word>::iterator First, std::vector<Word>::iterator Last );

	VariableBasePointer RealInterpret( 
						bool TopLevel = true,
						Word Before = NULL_WORD, Word After = NULL_WORD,
						VariableBasePointer PostValue = VariableBasePointer()  ) const;

	void CheckSyntax( bool IgnoreTrailingOps = false ) const;
	void StripOutlyingParenthesis() const;
	void ThrowExpressionAnomaly( const SS::STRING& Desc, AnomalyCode Code ) const;
	SS::STRING DumpToString() const;
	
	unsigned long GetAbsoluteIndex( unsigned long i ) const;
	void RevertToLocalCopy();

	typedef unsigned int OperatorPrecedence;
	OperatorPrecedence Precedence( const Word& ) const;


	//std::vector<Word> mWordList;
	
	typedef std::vector<Word> WordList;
	
	//LowerBounds is inclusive, UpperBouds is exclusive
	mutable unsigned long mLowerBounds, mUpperBounds;
	boost::shared_ptr< WordList > mpWordList;
	//WordList& mWordList;	
	
	static std::vector<VariableBasePointer> mUnnamedVariables;
	
	mutable bool mStatic;
};

}
#endif

