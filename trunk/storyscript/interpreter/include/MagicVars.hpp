/*
Copyright (c) 2004-2006 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.
Full license information is included in the file in the top
directory named "license".
*/

/**
	\file MagicVars.hpp
	\Declarations for MagicVarBase and various derivatives.  
*/


#if !defined( SS_MagicVars )
#define SS_MagicVars

#include "Variable.hpp"
#include "Unicode.hpp"

namespace SS{



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
/**
	\brief The base class for all magic variables.

	This is a base that takes care of a lot of the hassle of magic variables.

	Magic variables act exactly like a Variable except they can be calculated
	whenever they are referenced.  So we can do handy things like make a clock
	variable that always holds the current time, for example.
	
	When deriving from MagicVarBase, the functions you _must_ (ie. really, really
	should) implement are: GetVariableType, GetBoolData, GetNumData, and GetStringData.
	
	If your magic variable only produces string data (for example) define GetBoolData
	and GetNumData by simply returning GetBoolDataFromString and GetNumDataFromString,
	respectively.
	
	GetVariableType should return the native type of your magic variable, of course.
	
	Take a look at some of the magic variables declared here (and defines in the cpp file)
	if you get stuck.
*/
class MagicVarBase : public VariableBase
{
public:
	///Constructor
	MagicVarBase( SS_DECLARE_DEFAULTED_BASE_ARGS );
	///Destructor
	~MagicVarBase();

	VariableBasePtr operator+(const VariableBase&) const;
	VariableBasePtr operator-(const VariableBase&) const;
	VariableBasePtr operator*(const VariableBase&) const;
	VariableBasePtr operator/(const VariableBase&) const;
	VariableBasePtr operator=(const VariableBase&);


	VariableBasePtr operator==(const VariableBase&) const;
	VariableBasePtr operator!=(const VariableBase&) const;
	VariableBasePtr operator>=(const VariableBase&) const;
	VariableBasePtr operator<=(const VariableBase&) const;
	VariableBasePtr operator> (const VariableBase&) const;
	VariableBasePtr operator< (const VariableBase&) const;
	VariableBasePtr operator&&(const VariableBase&) const;
	VariableBasePtr operator||(const VariableBase&) const;
	
	virtual VariableBasePtr op_not() const;
	virtual VariableBasePtr op_neg() const;

	VariablePtr CastToVariable();
	const VariablePtr CastToVariable() const;



protected:
	/**
		\brief Creates a new variable from the current value of the magic var.
		
		It is too much work to override all the functionality of Variables for
		use with magic vars, so MagicVarBase often makes use of simply creating
		a new variable based of the current value of the MagicVar.  It then
		temporarily uses that variable to perform an operation.
	*/
	VariablePtr MakeVariable() const;
	
	///Converts the result of GetBoolData to a NumType
	NumType GetNumDataFromBool() const;
	///Converts the result of GetStringData to a NumType
	NumType GetNumDataFromString() const;
	
	///Converts the result of GetNumData to a BoolType
	BoolType GetBoolDataFromNum() const;
	///Converts the result of GetStringData to a BoolType
	BoolType GetBoolDataFromString() const;

	///Converts the result of GetNumData to a StringType
	StringType GetStringDataFromNum() const;
	///Converts the result of GetBoolData to a StringType
	StringType GetStringDataFromBool() const;

};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
/**
	\brief Binds a bool to a storyscript variable.

	Create an instance of this to bind a C++ bool to a storyscript (magic) variable.
*/
class BoundFlagVar : public MagicVarBase
{
public:
	BoundFlagVar( const SS::STRING& Name, bool Const, bool& Flag );
	
	virtual VariableBasePtr operator=(const VariableBase&);

	VarType GetVariableType() const;

	NumType    GetNumData() const;
	BoolType   GetBoolData()   const;
	StringType GetStringData() const;

private:
	bool& mFlag;

};


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
/**
	\brief Binds a SS::STRING to a storyscript variable.

	Create an instance of this to bind a SS::STRING to a storyscript (magic) variable.
*/
class BoundStringVar : public MagicVarBase
{
public:
	BoundStringVar( const SS::STRING& Name, bool Const, SS::STRING& String );

	virtual VariableBasePtr operator=( const VariableBase& );

	VarType GetVariableType() const;

	NumType    GetNumData() const;
	BoolType   GetBoolData()   const;
	StringType GetStringData() const;

private:
	SS::STRING& mString;
};



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
/**
	\brief Binds a NumType to a storyscript variable.

	Create an instance of this to bind a NumType to a storyscript (magic) variable.
*/
class BoundNumVar : public MagicVarBase
{
public:
	BoundNumVar( const SS::STRING& Name, bool Const, NumType& Num );

	virtual VariableBasePtr operator=( const VariableBase& );

	VarType GetVariableType() const;

	NumType    GetNumData() const;
	BoolType   GetBoolData()   const;
	StringType GetStringData() const;

private:
	NumType& mNum;
};


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
/**
	\brief Binds an unsigned long to a storyscript variable.

	Create an instance of this to bind a C++ unsigned long to a storyscript (magic) variable.
*/
class BoundULongVar : public MagicVarBase
{
public:
	BoundULongVar( const STRING& Name, bool Const, unsigned long& ULong );

	virtual VariableBasePtr operator=( const VariableBase& );
	
	VarType GetVariableType() const;
	
	NumType    GetNumData() const;
	BoolType   GetBoolData()   const;
	StringType GetStringData() const;

private:
	unsigned long& mNum;
};


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
/**
	\brief Binds a unsigned short to a storyscript variable.

	Create an instance of this to bind a C++ unsigned short to a storyscript (magic) variable.
*/
class BoundUShortVar : public MagicVarBase
{
public:
	BoundUShortVar( const STRING& Name, bool Const, unsigned short& UShort );

	virtual VariableBasePtr operator=( const VariableBase& );
	
	VarType GetVariableType() const;
	
	NumType    GetNumData() const;
	BoolType   GetBoolData()   const;
	StringType GetStringData() const;

private:
	unsigned short& mNum;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
/**
	\brief Binds a mpfr round mode enumeration to a storyscript variable.

	Create an instance of this to bind mpfr_rnd_t to a storyscript (magic) variable.
*/
class BoundRoundModeVar : public MagicVarBase
{
public:
	BoundRoundModeVar( const STRING& Name, bool Const, mpfr_rnd_t& RoundMode );

	virtual VariableBasePtr operator=( const VariableBase& );
	
	VarType GetVariableType() const;
	
	NumType    GetNumData() const;
	BoolType   GetBoolData()   const;
	StringType GetStringData() const;

private:
	mpfr_rnd_t& mRoundMode;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
/**
	\brief Magic variable that evaluates to the parents object's full name
*/
class FullNameVar : public MagicVarBase
{
public:
	FullNameVar( const SS::STRING& Name, bool Const, const ScopeObject& Parent );

	VarType GetVariableType() const;

	NumType    GetNumData() const;
	BoolType   GetBoolData()   const;
	StringType GetStringData() const;

private:
	const ScopeObject& mParent;
};





//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
/**
	\brief Magic variable that evaluates to and modifies the parents list's length.
*/
class ListLengthVar : public MagicVarBase
{
public:
	ListLengthVar( const SS::STRING& Name, bool Const, List& Parent );

	VariableBasePtr operator=(const VariableBase&);

	VarType GetVariableType() const;

	NumType    GetNumData() const;
	BoolType   GetBoolData()   const;
	StringType GetStringData() const;

private:
	List& mParent;
};



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
/**
	\brief Magic variable that evaluates to and modifies
		the parents variable precision.
*/
class PrecisionVar : public MagicVarBase
{
public:
	PrecisionVar( const SS::STRING& Name, Variable& Parent );

	VariableBasePtr operator=( const VariableBase&);

	VarType GetVariableType() const;

	NumType GetNumData() const;
	BoolType GetBoolData() const;
	StringType GetStringData() const;

private:
	Variable& mParent;
};



} //namespace SS
#endif


