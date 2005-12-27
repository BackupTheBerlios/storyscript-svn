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
 NOTES: Word with extended attributes that are used by Expression.
*/
class ExtendedWord : public Word
{
public:
	ExtendedWord();
	ExtendedWord( const STRING& String, WordType Type, ExtraDesc Extra = EXTRA_NULL );
	ExtendedWord( WordType Type, ExtraDesc Extra = EXTRA_NULL );
	ExtendedWord( const Word& );
	
private:
	friend class Expression;
	
	mutable ScopeObjectPtr mCachedObject;	
};

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
	VariableBasePtr Evaluate() const;

private:
	typedef unsigned int               OperatorPrecedence;
	typedef std::vector<ExtendedWord>  WordList;
	
	//These are the same as operator[] but they return ExtendedWord's
	ExtendedWord&       GetWord( unsigned long );
	const ExtendedWord& GetWord( unsigned long ) const;
	
	VariableBasePtr InternalEvaluate( bool TopLevel = true ) const;
	
	VariableBasePtr EvaluateUnaryOp ( ExtraDesc Op, VariableBasePtr Right ) const;
	VariableBasePtr EvaluateBinaryOp( ExtraDesc Op, VariableBasePtr Left, VariableBasePtr Right ) const;

	unsigned long CalculateLowPrecedenceOperator() const;
	OperatorPrecedence GetPrecedenceLevel( const Word& ) const;
	
	void CacheIdentifierObjects() const;
	mutable bool mIdentifiersCached;
	
	void CheckSyntax( bool IgnoreTrailingOps = false ) const;
	void StripOutlyingParenthesis() const;
	
	
	void ThrowExpressionAnomaly( const SS::STRING& Desc, AnomalyCode Code ) const;
	SS::STRING DumpToString() const;
	
	unsigned long GetAbsoluteIndex( unsigned long i ) const;
	void RevertToLocalCopy();

	
	
	

	
	
	//LowerBounds is inclusive, UpperBouds is exclusive
	mutable unsigned long mLowerBounds, mUpperBounds;
	boost::shared_ptr< WordList > mpWordList;
	
	//static std::vector<VariableBasePtr> mUnnamedVariables;
	
	mutable bool mStatic;
};

}
#endif

