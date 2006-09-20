/*
Copyright (c) 2004-2006 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.
Full license information is included in the file in the top
directory named "license".
*/

/**
	\file Expression.hpp
	\brief Declarations for Expression.  
*/


#if !defined(SS_Expression)
#define SS_Expression

//#include "ScriptFile.hpp"
#include "Word.hpp"
#include "Variable.hpp"
#include <vector>

namespace SS{
	

class Interpreter;

//~~~~~~~CLASS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/**
	\brief Wrapper class responsible for executing expressions.
	
	Expressions are held as a list of words.  When an expression is executed
	(via Evaluate), it gets divided and conquered.
	
*/
class Expression
{
private:

	/**
		\brief A simple bounds class for Expression.
		
		Instead of actually chopping up the expression and doing numerous copies,
		the new sub-expressions simply have the same list with an upper and lower bounds.
	*/
	struct Bounds
	{
		Bounds() : Lower(0), Upper(0) {}
		Bounds( size_t L, size_t U ) : Lower(L), Upper(U) {}
		Bounds( const Bounds& B ) : Lower(B.Lower), Upper(B.Upper) {}
		
		size_t Lower, Upper;
	};
	
	/// Default constructor (Don't use.)
	Expression();
	
public:
	/// Constructor
	Expression( Interpreter& I );
	
	/// Assignment operator
	Expression& operator=( const Expression& );
	
	/**
		\brief Returns true if the expression is static.
		
		\sa Expression::SetStatic
	*/
	bool IsStatic() const;
	
	/**
		\brief Sets the static flag.
		
		The static flag determines whether any new objects created by the
		expression get added to the static scope or the instance scope.
	*/
	void SetStatic( bool flag = true ) const;
	
	/**
		\brief Retrieves of specific word.
		
		\param Index Index to the word to retrieve.
	*/
	Word& operator[]( unsigned long Index );
	
	/**
		\brief Retrieves of specific word. (Const version)
		
		\param Index Index to the word to retrieve.
	*/
	const Word& operator[]( unsigned long Index ) const;
	
	/// Remove all elements from the expression.
	void clear();
	
	/// Add a word to the end of the expression.
	void push_back( const Word& );
	
	/// Remove a word from the end of the expression.
	void pop_back();
	
	/// Returns true if the expression has no words in it.
	bool empty() const;
	
	/// Returns the number of words in the expression.
	unsigned long size() const;
	
	/**
		\brief Remove a specific element from the list.

		\param Index Index of the word to remove.
	*/
	void erase( unsigned long Index );

	/**
		\brief The all powerful expression evaluator.
		
		\return The result of the expression.
	*/
	VariableBasePtr Evaluate() const;

private:
	/**
		\brief Copy constructor
		
		\param E The other expression.
		\param B The sub-expression of to 'copy' over.
	*/
	Expression( const Expression& E, const Bounds& B );
	
	/// Precedence level for operators.
	typedef unsigned int               OperatorPrecedence;
	
	/// The actual list of Words.
	typedef std::vector<Word> WordList;
	
	/// Object Cache: Objects are cached to reduce redundant lookups.
	typedef std::map<size_t, ScopeObjectPtr> ObjectCache;
	
	/// Pointer to an Object Cache
	typedef boost::shared_ptr<ObjectCache> ObjectCachePtr;
	
	/// Comparison function used by PrecedenceList
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
	
	
	/**
		\brief Cache of precedence indexes. 
		
		The low precedence operator for each sub-expression is cached so
		when the expression is evaluated again, it doesn't have to go through
		of calculating the low precedence expression.
	*/
	typedef std::map< Bounds, unsigned long, BoundsCompare > PrecedenceList;
	
	/// Pointer to PrecedenceList
	typedef boost::shared_ptr<PrecedenceList> PrecedenceListPtr;
	
	/// The expression's precedence cache.
	mutable PrecedenceListPtr mpPrecedenceList;
	
	
	/// Has the same effect as operator[].
	Word&       GetWord( unsigned long );
	/// Has the same effect as operator[].
	const Word& GetWord( unsigned long ) const;
	
	/**
		\brief The 'real' evaluate function.
		
		This a recursive function that gets called by the public
		Evaluate function.
		
		\param TopLevel This is true if it is being called by Evaluate, and
		false if it is being called recursionally.
		
		\param O A pointer to the object cache for the expression.
		
		\return The result of the expression.
	*/
	VariableBasePtr InternalEvaluate( bool TopLevel = true, ObjectCachePtr O = ObjectCachePtr() ) const;
	
	/**
		\brief Maps unary operator words to their actual function calls.
		
		\param Op The unary operator.
		
		\param Right The r-value of the unary operator.
		
		\return The result of the operation.
	*/
	VariableBasePtr EvaluateUnaryOp ( ExtraDesc Op, VariableBasePtr Right ) const;
	
	/**
		\brief Maps binary operator words to their actual function calls.
		
		\param Op The binary operator.
		
		\param Right The r-value of the unary operator.
		
		\param Left The l-value of the binary operator.
		
		\return The result of the operation.
	*/
	VariableBasePtr EvaluateBinaryOp( ExtraDesc Op, VariableBasePtr Left, VariableBasePtr Right ) const;

	/**
		\brief Finds the operator with the lowest precedence.
		
		\param O The object cache for the expression.
		
		\return The index to the low precedence operator.
	*/
	size_t CalculateLowPrecedenceOperator( ObjectCachePtr O ) const;
	
	/**
		\brief Retrieves the precedence level for a given word.
		
		\param The word.
		
		\return The precedence level.
	*/	
	OperatorPrecedence GetPrecedenceLevel( const Word& W ) const;
	
	/**
		\brief Caches all the identifiers.
		
		Looks up all the identifiers and saves them to the given object cache.
		
		\param O The object cache to use.
	*/		
	void CacheIdentifierObjects( ObjectCachePtr O ) const;
	
	/**
		\brief Checks the expression for any obvious errors.
		
		\param IgnoreTrailingOps Special case for when trailing operators should
		not trigger an anomaly.
	*/
	void CheckSyntax( bool IgnoreTrailingOps = false ) const;
	/// Flag specifying whether the syntax has already been checked or not.
	mutable bool mSyntaxChecked;
	
	/// Removes any useless outlying parentheses from the expression.
	bool StripOutlyingParenthesis() const;
	
	/// Throws a specially formatted expression anomaly.
	void ThrowExpressionAnomaly( const SS::STRING& Desc, AnomalyCode Code ) const;
	
	/// Converts the expression to a (hopefully) human readable form.
	SS::STRING DumpToString() const;
	
	/**
		\brief Convert a local index to a absolute index.
		
		\sa Bounds
	*/
	unsigned long GetAbsoluteIndex( unsigned long i ) const;
	
	/**
		\brief Convert a shared word list to a local one.
		
		If the expression is using another expressions word list,
		this will copy the word list over so it is modifiable.
	*/
	void RevertToLocalCopy();

	
	/**
		\brief The expression's Interpreter.
		
		The expression keeps a reference to the interpreter so it can register
		objects on the current scope, and such.
	*/	
	Interpreter& mI;
	
	
	/**
		\brief The expression's upper and lower bounds.
		
		\sa Bounds
	*/
	mutable Bounds mBounds;
	
	/// The expression's list of words.
	boost::shared_ptr< WordList > mpWordList;
	
	//static std::vector<VariableBasePtr> mUnnamedVariables;
	
	/**
		\brief The static flag.
		
		\sa GetStatic SetStatic
	*/
	mutable bool mStatic;
};

}
#endif

