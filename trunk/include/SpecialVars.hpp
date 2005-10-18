/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: The magic variable interface.  Ala-kazam!
*/


#if !defined( SS_SpecialVars )
#define SS_SpecialVars

#include "Variable.hpp"
#include "Unicode.hpp"

namespace SS{



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 SpecialVarBase
 NOTES: This is a base that takes care of a lot of the hassle of special variables.

		Special variables are anything that acts exactly like a Variable except
		that its value calculated every time its referenced.  Like timers
		and stuff.
*/
class SpecialVarBase : public VariableBase
{
public:
	SpecialVarBase();
	SpecialVarBase( const SS::STRING& Name, 
					bool Static = false, bool Const = false );
	~SpecialVarBase();

	VariableBasePointer operator+(const VariableBase&) const;
	VariableBasePointer operator-(const VariableBase&) const;
	VariableBasePointer operator*(const VariableBase&) const;
	VariableBasePointer operator/(const VariableBase&) const;
	VariableBasePointer operator=(const VariableBase&);


	VariableBasePointer operator==(const VariableBase&) const;
	VariableBasePointer operator!=(const VariableBase&) const;
	VariableBasePointer operator>=(const VariableBase&) const;
	VariableBasePointer operator<=(const VariableBase&) const;
	VariableBasePointer operator> (const VariableBase&) const;
	VariableBasePointer operator< (const VariableBase&) const;
	VariableBasePointer operator&&(const VariableBase&) const;
	VariableBasePointer operator||(const VariableBase&) const;
	
	virtual VariableBasePointer op_not() const;
	virtual VariableBasePointer op_neg() const;

	VariablePointer GetVariablePtr();
	const VariablePointer GetVariablePtr() const;



protected:
	VariablePointer MakeVariable() const;

	NumType GetNumDataFromBool() const;
	NumType GetNumDataFromString() const;

	BoolType GetBoolDataFromNum() const;
	BoolType GetBoolDataFromString() const;

	StringType GetStringDataFromNum() const;
	StringType GetStringDataFromBool() const;

};

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
BoundFlagVar
NOTES: This is used to create a variable in the ss script that is bound to
	   a flag in the interpreter.  Such as verbose, or something.
*/
class BoundFlagVar : public SpecialVarBase
{
public:
	BoundFlagVar( const SS::STRING& Name, bool& Flag, 
				  bool Static = false, bool Const = false );
	
	virtual VariableBasePointer operator=(const VariableBase&);

	VarType GetVariableType() const;

	NumType    GetNumData() const;
	BoolType   GetBoolData()   const;
	StringType GetStringData() const;

private:
	bool& mFlag;

};


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 BoundStringVar
 NOTES: Binds a hard-coded string to an SS variable.
*/
class BoundStringVar : public SpecialVarBase
{
public:
	BoundStringVar( const SS::STRING& Name, SS::STRING& String,
					bool Static = false, bool Const = false );

	virtual VariableBasePointer operator=( const VariableBase& );

	VarType GetVariableType() const;

	NumType    GetNumData() const;
	BoolType   GetBoolData()   const;
	StringType GetStringData() const;

private:
	SS::STRING& mString;
};



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
BoundNumVar
NOTES: Binds a hard-coded string to an SS variable.
*/
class BoundNumVar : public SpecialVarBase
{
public:
	BoundNumVar( const SS::STRING& Name, NumType& Num,
		bool Static = false, bool Const = false );

	virtual VariableBasePointer operator=( const VariableBase& );

	VarType GetVariableType() const;

	NumType    GetNumData() const;
	BoolType   GetBoolData()   const;
	StringType GetStringData() const;

private:
	NumType& mNum;
};



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 FullNameVar
 NOTES: Returns the full name of an object.
*/
class FullNameVar : public SpecialVarBase
{
public:
	FullNameVar( const SS::STRING& Name, const ScopeObject& Parent,
				 bool Static = false, bool Const = false );

	VarType GetVariableType() const;

	NumType    GetNumData() const;
	BoolType   GetBoolData()   const;
	StringType GetStringData() const;

private:
	const ScopeObject& mParent;
};





/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
ListLengthVar
NOTES: This one reports the length of a list.
*/
class ListLengthVar : public SpecialVarBase
{
public:
	ListLengthVar( const SS::STRING& Name, List& Parent,
				   bool Static = false, bool Const = false );

	VariableBasePointer operator=(const VariableBase&);

	VarType GetVariableType() const;

	NumType    GetNumData() const;
	BoolType   GetBoolData()   const;
	StringType GetStringData() const;

private:
	List& mParent;
};



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 PrecisionVar
 NOTES: Reports and changes the precision of num values.
*/
class PrecisionVar : public SpecialVarBase
{
public:
	PrecisionVar( const SS::STRING& Name, Variable& Parent,
				  bool Static = false, bool Const = false );

	VariableBasePointer operator=( const VariableBase&);

	VarType GetVariableType() const;

	NumType GetNumData() const;
	BoolType GetBoolData() const;
	StringType GetStringData() const;

private:
	Variable& mParent;
};







}
#endif


