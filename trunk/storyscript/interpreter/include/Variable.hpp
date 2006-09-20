/*
Copyright (c) 2004-2006 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.
Full license information is included in the file in the top
directory named "license".
*/

/**
	\file Variable.hpp
	\brief Declarations for VariableBase and Variable.  
*/

#if !defined(SS_Variable)
#define SS_Variable

#include "Scope.hpp"
#include "ScopeObject.hpp"
#include "ScopeObjectVisitor.hpp"
#include <boost/shared_ptr.hpp>


/*
I'm using the MPFR library to handle multiple-precision floating point
numbers.  It is based off the GNU MP (GMP) library, but it is NOT the version
that is included with the latest release of GMP.  The version GMP includes is
as of now too old for my purposes. 

The MPFR libarary can be found at: http://www.mpfr.org/

The GNU MP Library can be found at: http://www.swox.com/gmp/
*/

namespace SS
{
	
//Conversion functions are external.  It is sometimes for efficient for
//other functions to call these directly rather than creating a whole variable
//just to do conversion.  Users typically won't need these.

/**
	\brief Converts NumType to to StringType.

	Variable utilizes this function to do internal conversions.
	
	\param In The NumType to be converted
	\return The converted StringType
*/ 
StringType NumType2StringType( const NumType& In );

/**
	\brief Converts NumType to to StringType.

	Variable utilizes this function to do internal conversions.
	
	\param In The NumType to be converted
	\param Out The converted StringType
*/ 
void       NumType2StringType( const NumType& In, StringType& Out );

/**
	\brief Converts StringType to to NumType.

	Variable utilizes this function to do internal conversions.
	
	\param In The StringType to be converted
	\return The converted NumType
*/ 
NumType    StringType2NumType( const StringType& In );

/**
	\brief Converts StringType to to NumType.

	Variable utilizes this function to do internal conversions.
	
	\param In The StringType to be converted
	\param Out The converted NumType
*/ 
void       StringType2NumType( const StringType& In, NumType& Out );


	



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
/**
	\brief The base class for any objects that are evaluated to be a value.
	
	This include variables, lists, and the many magic variables.  In storyscript
	this is pretty open ended.  So pretty much anything can behave as variable,
	if you overload the proper functions.  Typically you do this through the by
	overloaded MagicVar.

	This is not an abstract class.  If you create an instance of it,
	it will be interpreted as a NULL value.
	
	\sa MagicVar
*/
class SS_API VariableBase : public Scope
{
protected:
	SS_FRIENDIFY_GENERIC_CREATOR(VariableBase);
	
	///Constructor
	VariableBase( const SS::String& Name = SS::String(), bool Const = false );

public:
	
	///Destructor
	virtual ~VariableBase();
	
	//Conversions for the user
	//void Set

	void AcceptVisitor( ScopeObjectVisitor& );

	virtual VariableBasePtr CastToVariableBase();
	virtual const VariableBasePtr CastToVariableBase() const;

	ListPtr CastToList();
	const ListPtr CastToList() const;
	
	///Additional operator
	virtual VariableBasePtr operator+(const VariableBase&) const;
	///Subtraction operator
	virtual VariableBasePtr operator-(const VariableBase&) const;
	///Multiplication operator
	virtual VariableBasePtr operator*(const VariableBase&) const;
	///Division operator
	virtual VariableBasePtr operator/(const VariableBase&) const;
	///Exponent operator
	virtual VariableBasePtr operator_pow( const VariableBase& ) const;
	///String concatenation operator
	virtual VariableBasePtr operator_concat( const VariableBase& ) const;
	
	///Assignment operator
	virtual VariableBasePtr operator=(const VariableBase&);

	///Equal operator
	virtual VariableBasePtr operator==(const VariableBase&) const;
	///Not-Equal operator
	virtual VariableBasePtr operator!=(const VariableBase&) const;
	///Greater-or-equal operator
	virtual VariableBasePtr operator>=(const VariableBase&) const;
	///Less-or-equal operator
	virtual VariableBasePtr operator<=(const VariableBase&) const;
	///Greater operator
	virtual VariableBasePtr operator> (const VariableBase&) const;
	///Less operator
	virtual VariableBasePtr operator< (const VariableBase&) const;
	///Logical-And operator
	virtual VariableBasePtr operator&&(const VariableBase&) const;
	///Logical-Or operator
	virtual VariableBasePtr operator||(const VariableBase&) const;

	///Logical-Nog operator
	virtual VariableBasePtr op_not() const;
	///Negate operator
	virtual VariableBasePtr op_neg() const;

	/**
		\brief Return the preferred variable type.
		
		As you know, variable in storyscript are dynamically typed.  To help
		decide what gets converted when derivatives of VariableBase return their
		preferred type.  This helps determine the best auto-conversions.
		
		/return The object's preferred simple type
	*/
	virtual VarType GetVariableType() const; 

	///Returns the VariableBase's NumType component.
	virtual NumType    GetNumData() const;
	///Returns the VariableBase's BoolType component.
	virtual BoolType   GetBoolData()   const;
	///Returns the VariableBase's StringType component.
	virtual StringType GetStringData() const;
	
	//If you are linking this against non Multi-threaded DLL code
	//Use these.
	
	/**
		\brief Returns the VariableBase's NumType component.
		
		Alternative, potentially more efficient method.
	*/
	NumType&    GetNumData( NumType& ) const;
	
	/**
		\brief Returns the VariableBase's BoolType component.
		
		Alternative, potentially more efficient method.
	*/
	BoolType&   GetBoolData( BoolType& ) const;
	
	/**
		\brief Returns the VariableBase's StringType component.
		
		Alternative, potentially more efficient method.
	*/
	StringType& GetStringData( StringType& ) const;

	/**
		\brief Copies the VariableBase's StringType component to c-style string.
		 
		There are some specific circumstances when this is useful to avoid memory
		corruption problems.
		
		\param Buffer A pointer to the c-style char array.
		\param BufferSize The size of the provided buffer.
		\return The Buffer parameter
	*/
	Char* GetStringData( Char* Buffer, unsigned int BufferSize ) const;

private:
	/**
		\brief Throws pre-formatted "Undefined Operation" anomalies.
		
		\param Op A string containing the operator that is undefined.
		\return Nothing.  The exception is thrown before it returns.
			This is just a way to make the compiles happy.
	*/
	VariableBasePtr UndefinedOp(const SS::String& Op) const;

};



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
/**
	\brief The storyscript dynamically typed variable.
	
	In storyscript, the variable will convert between three sub-types:
	string, number, and boolean.  These conversions are meant to be as
	seamless and transparent as possible, as this is in some ways meant
	to be a language for non/beginning programmers.  
	
*/
class SS_API Variable : public VariableBase
{
protected:	
	SS_FRIENDIFY_VARIABLE_CREATOR(Variable);
	SS_FRIENDIFY_GENERIC_CREATOR(Variable);

	///Constructor
	Variable( const SS::String& Name = SS::String(), bool Const = false );
	
	///Constructor
	Variable( const SS::String& Name, bool Const,
			  const Variable& );
	
	///Constructor
	Variable( const SS::String& Name, bool Const,
			  const NumType& );
	
	///Constructor
	Variable( const SS::String& Name, bool Const,
			  const StringType& );
	
	///Constructor
	Variable( const SS::String& Name, bool Const,
			  const BoolType& );
public:

    void AcceptVisitor( ScopeObjectVisitor& );

	VariablePtr CastToVariable();
	const VariablePtr CastToVariable() const;

	//VariablePtr Duplicate() const;
	
	VariableBasePtr operator+(const VariableBase&) const;
	VariableBasePtr operator-(const VariableBase&) const;
	VariableBasePtr operator*(const VariableBase&) const;
	VariableBasePtr operator/(const VariableBase&) const;
	VariableBasePtr operator_pow( const VariableBase& ) const;
	VariableBasePtr operator_concat( const VariableBase& ) const;
	VariableBasePtr operator=(const VariableBase&);

	
	VariableBasePtr operator==(const VariableBase&) const;
	VariableBasePtr operator!=(const VariableBase&) const;
	VariableBasePtr operator>=(const VariableBase&) const;
	VariableBasePtr operator<=(const VariableBase&) const;
	VariableBasePtr operator> (const VariableBase&) const;
	VariableBasePtr operator< (const VariableBase&) const;
	VariableBasePtr operator&&(const VariableBase&) const;
	VariableBasePtr operator||(const VariableBase&) const;

	//unary operators
	VariableBasePtr op_not() const;
	VariableBasePtr op_neg() const;

	/**
		\brief Force the variable to convert itself.
		
		This changes the current variable type and does any conversions
		necessary.
		
		\param T The type to convert to.
	*/		
	void ForceConversion( VarType T );

	
	VarType GetVariableType() const;

	NumType GetNumData() const;
	BoolType   GetBoolData()   const;
	StringType GetStringData() const;

	/**
		\brief Returns a reference to the Variable's NumType component.
		
		This provides direct access to one of Variables components.
		Do not use it unless you really know what you are doing.
	*/
	NumType& GetActualNumData();
	
	/**
		\brief Returns a reference to the Variable's NumType component.
		
		This provides direct access to one of Variables components.
		Do not use it unless you really know what you are doing.
	*/
	BoolType& GetActualBoolData();
	
	/**
		\brief Returns a reference to the Variable's NumType component.
		
		This provides direct access to one of Variables components.
		Do not use it unless you really know what you are doing.
	*/
	StringType& GetActualStringData();
	
protected:
	virtual ScopeObjectPtr GetScopeObjectHook( const SS::String& );

private:
	///Called by constructors to handle any common initialization.
	void RegisterPredefinedVars();
	
	///Signifies whether a precision magic-var has been spawned yet.
	bool mPrecisionVarCreated;

	///The current preferred type.
	VarType mCurrentType;

	///A table of type precedences which determines what type takes
	///Which type wins out when to different types are used together.
	static VarType mTypeConversionTable[3][3];

	mutable NumType    mNumPart; //< The NumType component
	mutable BoolType   mBoolPart; //< The BoolType component
	mutable StringType mStringPart; //< The StringType component
};




}//namespace SS
#endif //!defined(SS_Variable)

