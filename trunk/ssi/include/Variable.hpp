/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: The variable interface and implementation.
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
as of now too old for my purposes. The C++ wrapper however is
the one included with GMP, however I modified it to take advantage of the
newer features included with the version of MPFR I'm using.

The MPFR libarary can be found at: http://www.mpfr.org/

The GNU MP Library can be found at: http://www.swox.com/gmp/

The latest binaries of both are included for convenience

*/

namespace SS
{
	
//To convert from numbers to strings
StringType NumType2StringType( const NumType& );

	



/*~~~~~~~INTERFACE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 VariableBase
 NOTES: This is not an abstract class.  If you create an instance of it,
		it will be interpreter at a NULL value.
*/
class SS_API VariableBase : public Scope
{
protected:
	SS_FRIENDIFY_GENERIC_CREATOR(VariableBase);
	VariableBase();
	VariableBase( const SS::STRING&, bool Static = false, bool Const = false );

public:

	virtual ~VariableBase();

	void AcceptVisitor( ScopeObjectVisitor& );

	virtual VariableBasePtr CastToVariableBase();
	virtual const VariableBasePtr CastToVariableBase() const;

	ListPtr CastToList();
	const ListPtr CastToList() const;

	virtual VariableBasePtr operator+(const VariableBase&) const;
	virtual VariableBasePtr operator-(const VariableBase&) const;
	virtual VariableBasePtr operator*(const VariableBase&) const;
	virtual VariableBasePtr operator/(const VariableBase&) const;
	virtual VariableBasePtr operator_pow( const VariableBase& ) const;
	virtual VariableBasePtr operator_concat( const VariableBase& ) const;
	
	virtual VariableBasePtr operator=(const VariableBase&);


	virtual VariableBasePtr operator==(const VariableBase&) const;
	virtual VariableBasePtr operator!=(const VariableBase&) const;
	virtual VariableBasePtr operator>=(const VariableBase&) const;
	virtual VariableBasePtr operator<=(const VariableBase&) const;
	virtual VariableBasePtr operator> (const VariableBase&) const;
	virtual VariableBasePtr operator< (const VariableBase&) const;
	virtual VariableBasePtr operator&&(const VariableBase&) const;
	virtual VariableBasePtr operator||(const VariableBase&) const;

	//unary operators
	virtual VariableBasePtr op_not() const;
	virtual VariableBasePtr op_neg() const;


	virtual VarType GetVariableType() const; 

	virtual NumType    GetNumData() const;
	virtual BoolType   GetBoolData()   const;
	virtual StringType GetStringData() const;
	
	//If you are linking this againts non Multi-threaded DLL code
	//Use these.
	NumType&    GetNumData( NumType& ) const;
	BoolType&   GetBoolData( BoolType& ) const;
	StringType& GetStringData( StringType& ) const;

	CHAR* GetStringData( CHAR* Buffer, unsigned int BufferSize ) const;

private:
	VariableBasePtr UndefinedOp(const SS::STRING& Op) const;

};



//~~~~~~~CLASS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// class Variable
// NOTES: The dynamically typed variable.
//
class SS_API Variable : public VariableBase
{
protected:	
	SS_FRIENDIFY_VARIABLE_CREATOR(Variable);

	Variable();
	
	Variable( const SS::STRING& Name,
			  bool Static, bool Const,
			  const Variable& );
	
	Variable( const SS::STRING& Name,
			  bool Static, bool Const,
			  const NumType& );
	
	Variable( const SS::STRING& Name, 
			  bool Static, bool Const,
			  const StringType& );
	
	Variable( const SS::STRING& Name, 
			  bool Static, bool Const,
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


	void ForceConversion( VarType );

	VarType GetVariableType() const;

	NumType GetNumData() const;
	BoolType   GetBoolData()   const;
	StringType GetStringData() const;

	//Only Variable promises provides these.
	//Use them rarely and with care.
	NumType& GetActualNumData();
	BoolType& GetActualBoolData();
	StringType& GetActualStringData();
	
protected:
	virtual ScopeObjectPtr GetScopeObjectHook( const STRING& );

private:
	void RegisterPredefinedVars();

	VarType mCurrentType;

	static VarType mTypeConversionTable[3][3];

	mutable NumType    mNumPart;
	mutable BoolType   mBoolPart;
	mutable StringType mStringPart;
};




}//namespace SS
#endif //!defined(SS_Variable)

