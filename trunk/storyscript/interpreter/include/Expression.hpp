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
	

class Interpreter;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 Expression
 NOTES: The expression class.
*/
class Expression
{
private:
	struct Bounds
	{
		Bounds() : Lower(0), Upper(0) {}
		Bounds( size_t L, size_t U ) : Lower(L), Upper(U) {}
		Bounds( const Bounds& B ) : Lower(B.Lower), Upper(B.Upper) {}
		
		size_t Lower, Upper;
	};
	
	Expression();
	
public:
	Expression( Interpreter& I );

	Expression& operator=( const Expression& );
	
	bool IsStatic() const;
	void SetStatic( bool flag = true ) const;

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
	Expression( const Expression&, const Bounds& );
	typedef unsigned int               OperatorPrecedence;
	typedef std::vector<Word> WordList;
	typedef std::map<size_t, ScopeObjectPtr> ObjectCache;
	typedef boost::shared_ptr<ObjectCache> ObjectCachePtr;
	
	struct BoundsCompare
	{
		//How this is done is more or less arbitrary
		bool operator()( const Bounds& B1, const Bounds& B2 ) const
		{
			if( B1.Lower < B2.Lower ) return true;
			else if( B1.Lower == B2.Lower )
			{
				if( B1.Upper < B2.Upper ) return true;
				else return false;		
			}
			else return false;			
		}		
	};
	
	
	//Precedence cache
	typedef std::map< Bounds, unsigned long, BoundsCompare > PrecedenceList;
	typedef boost::shared_ptr<PrecedenceList> PrecedenceListPtr;
	mutable PrecedenceListPtr mpPrecedenceList;
	
	
	//These are the same as operator[] but they return ExtendedWord's
	Word&       GetWord( unsigned long );
	const Word& GetWord( unsigned long ) const;
	
	VariableBasePtr InternalEvaluate( bool TopLevel = true, ObjectCachePtr = ObjectCachePtr() ) const;
	
	
	VariableBasePtr EvaluateUnaryOp ( ExtraDesc Op, VariableBasePtr Right ) const;
	VariableBasePtr EvaluateBinaryOp( ExtraDesc Op, VariableBasePtr Left, VariableBasePtr Right ) const;

	size_t CalculateLowPrecedenceOperator( ObjectCachePtr ) const;
	OperatorPrecedence GetPrecedenceLevel( const Word& ) const;
	
	void CacheIdentifierObjects( ObjectCachePtr ) const;
	
	void CheckSyntax( bool IgnoreTrailingOps = false ) const;
	mutable bool mSyntaxChecked;
	
	
	bool StripOutlyingParenthesis() const;
	
	
	void ThrowExpressionAnomaly( const SS::STRING& Desc, AnomalyCode Code ) const;
	SS::STRING DumpToString() const;
	
	unsigned long GetAbsoluteIndex( unsigned long i ) const;
	void RevertToLocalCopy();

	
	
	
	Interpreter& mI;
	
	
	//LowerBounds is inclusive, UpperBouds is exclusive
	//mutable unsigned long mLowerBounds, mUpperBounds;
	mutable Bounds mBounds;
	boost::shared_ptr< WordList > mpWordList;
	
	//static std::vector<VariableBasePtr> mUnnamedVariables;
	
	mutable bool mStatic;
};

}
#endif

