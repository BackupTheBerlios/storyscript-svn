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
public:
	VariableBase();
	VariableBase( const SS::STRING&, bool Static = false, bool Const = false );

	virtual ~VariableBase();

	void AcceptVisitor( ScopeObjectVisitor& );

	virtual VariableBasePointer GetVariableBasePtr();
	virtual const VariableBasePointer GetVariableBasePtr() const;

	ListPointer GetListPtr();
	const ListPointer GetListPtr() const;


	virtual VariableBasePointer operator+(const VariableBase&) const;
	virtual VariableBasePointer operator-(const VariableBase&) const;
	virtual VariableBasePointer operator*(const VariableBase&) const;
	virtual VariableBasePointer operator/(const VariableBase&) const;
	virtual VariableBasePointer operator_pow( const VariableBase& ) const;
	virtual VariableBasePointer operator_concat( const VariableBase& ) const;
	
	virtual VariableBasePointer operator=(const VariableBase&);


	virtual VariableBasePointer operator==(const VariableBase&) const;
	virtual VariableBasePointer operator!=(const VariableBase&) const;
	virtual VariableBasePointer operator>=(const VariableBase&) const;
	virtual VariableBasePointer operator<=(const VariableBase&) const;
	virtual VariableBasePointer operator> (const VariableBase&) const;
	virtual VariableBasePointer operator< (const VariableBase&) const;
	virtual VariableBasePointer operator&&(const VariableBase&) const;
	virtual VariableBasePointer operator||(const VariableBase&) const;

	//unary operators
	virtual VariableBasePointer op_not() const;
	virtual VariableBasePointer op_neg() const;


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
	VariableBasePointer UndefinedOp(const SS::STRING& Op) const;

};



//~~~~~~~CLASS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// class Variable
// NOTES: The dynamically typed variable.
//
class SS_API Variable : public VariableBase
{
public:
	
	//Actualy, I don't really need this.
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

    void AcceptVisitor( ScopeObjectVisitor& );

	VariablePointer GetVariablePtr();
	const VariablePointer GetVariablePtr() const;

	//VariablePointer Duplicate() const;
	
	VariableBasePointer operator+(const VariableBase&) const;
	VariableBasePointer operator-(const VariableBase&) const;
	VariableBasePointer operator*(const VariableBase&) const;
	VariableBasePointer operator/(const VariableBase&) const;
	VariableBasePointer operator_pow( const VariableBase& ) const;
	VariableBasePointer operator_concat( const VariableBase& ) const;
	VariableBasePointer operator=(const VariableBase&);

	
	VariableBasePointer operator==(const VariableBase&) const;
	VariableBasePointer operator!=(const VariableBase&) const;
	VariableBasePointer operator>=(const VariableBase&) const;
	VariableBasePointer operator<=(const VariableBase&) const;
	VariableBasePointer operator> (const VariableBase&) const;
	VariableBasePointer operator< (const VariableBase&) const;
	VariableBasePointer operator&&(const VariableBase&) const;
	VariableBasePointer operator||(const VariableBase&) const;

	//unary operators
	VariableBasePointer op_not() const;
	VariableBasePointer op_neg() const;


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

