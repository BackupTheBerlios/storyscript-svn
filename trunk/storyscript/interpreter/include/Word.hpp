/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.
Full license information is included in the file in the top
directory named "license".
*/


/**
	\file Word.hpp
	\Declarations for Word.  
*/

#if !defined(SS_Word)
#define SS_Word

#include "Unicode.hpp"
#include "LanguageConstants.hpp"
#include "DLLExportString.hpp"

namespace SS{

/** 
 * \brief A class for holding a number of a strings.
 * 
 * These are used for comples identifiers (foo:bar), to hold the individual
 * identifiers.
 */
typedef std::vector<SS::STRING> CompoundString;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
/**
	\brief A basic language element.
	
	Word represents an individual language element (eg. an identifier, operator, 
	literal), as tokenized by the reader.
	
	Normally the user should never have to deal with this class.
	
	\see ReaderSource
		
*/
class Word
{
public:
	/// Default Constructor
	Word();
	
	/// Constructor
	Word( const SS::STRING& String, WordType Type, ExtraDesc Extra = EXTRA_NULL );
	
	/// Constructor
	Word( WordType Type, ExtraDesc Extra = EXTRA_NULL );
	
	/// Copy Constructor
	Word( const Word& );
	
	/**
		\brief Set the compound string from an identifier string.
		
		Treats the string as an identifer and divides it into individual parts
		before assigning it to the compound string.
		
		\param String The string being assigned.
	
	*/
	void DivideAndAssignString( const SS::STRING& String );
	
	/// Returns the compound string smushed into a single string.
	SS::STRING GetSimpleString() const;
	
	/// Destructor
	virtual ~Word() {}

	/// Assignment operator
	Word& operator=( const Word& W );
	
	/// Comparison operator
	bool operator==( const Word& W ) const;
	
	//bool operator!() const;
	
	/// Returns true if the Word is some sort of literal.
	bool IsLiteral() const;
	
	/// Returns true if the Word is some sort of operator.
	bool IsOperator() const;
	
	/// If the word is an ambiguous operator, treat it as binary.
	void InterpretAsBinaryOp() const;
	
	/// If the word is an ambiguous operator, treat it as unary.
	void InterpretAsUnaryOp() const;

	/// The compound string that hold the identifier name or a literal.
	CompoundString String;
	
	/// The simple type.
	mutable WordType Type;
	
	/// The more specific type.
	mutable ExtraDesc Extra;

	

};

/**
	\brief Make a CompoundString out of a simple one.
	
	Simple copies the string to the first element of the CompoundString.
	
	\param S The string to be converted.
*/
SS::CompoundString MakeCompoundString( const SS::STRING& S );

/**
	\brief Make a CompoundString out of a simple string containing an ID.
	
	Breaks the identifier string into individual indentifiers
	in the seperate elements of the CompoundString.
	
	\param S The string to be converted.
*/
SS::CompoundString MakeCompoundID( const SS::STRING& S);

/**
	\brief Collapses a compound string into a simple string.
	
	Copies each element into a string with a scope resolution
	operator (":") inserted between each.
	
	\param CS The CompoundString you would like converted.
*/
SS::STRING CollapseCompoundString( const SS::CompoundString& CS );

//Defined in Word.cpp:

/// A special word that signifies the end of the file.
extern const Word EOF_WORD;

/// An special unassigned word that has no value.
extern const Word NULL_WORD;



} //end namespace SS
#endif

