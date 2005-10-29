/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: The magic variable interface.  Ala-kazam!
*/

#include "SpecialVars.hpp"
#include "ParserAnomaly.hpp"
#include "CreationFuncs.hpp"

//Use by ListLengthVar
#include "List.hpp"

#include "Variable.hpp"


//used by PrecesionVar::operator=
#include <boost/lexical_cast.hpp>





using namespace SS;



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 SpecialVarBase::SpecialVar/~SpecialVar
 NOTES: Ctors and Dtor
*/
SpecialVarBase::SpecialVarBase()
{
}

SpecialVarBase::SpecialVarBase( const STRING& Name,
							    bool Static /*= false*/, bool Const /*= false*/ )
: VariableBase( Name, Static, Const )
{
}

SpecialVarBase::~SpecialVarBase()
{
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 SpecialVarBase::MakeVariable
 NOTES: This is a useful little function that converts the current value
		of the SpecialVar into a real live Variable.
*/
VariablePtr SpecialVarBase::MakeVariable() const
{
	VariablePtr pNewVar;

	switch( this->GetVariableType() ){
		case VARTYPE_NUM:
			return Creator::CreateVariable( UNNAMMED, false, true, GetNumData() );

		case VARTYPE_STRING:
			return Creator::CreateVariable( UNNAMMED, false, true, GetStringData() );

		case VARTYPE_BOOL:
			return Creator::CreateVariable( UNNAMMED, false, true, GetBoolData() );

		default:
			ThrowParserAnomaly( TXT("Unknown variable type."), ANOMALY_PANIC );
	}
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 SpecialVarBase::Get____DataFrom____
 NOTES: These are to aid the children.
*/
NumType SpecialVarBase::GetNumDataFromBool() const{
	return Variable( SS_BASE_ARGS_DEFAULTS, GetBoolData() ).GetNumData();
}

NumType SpecialVarBase::GetNumDataFromString() const{
	return Variable( SS_BASE_ARGS_DEFAULTS, GetStringData() ).GetNumData();
}

BoolType SpecialVarBase::GetBoolDataFromNum() const{
	return Variable( SS_BASE_ARGS_DEFAULTS, GetNumData() ).GetBoolData();
}

BoolType SpecialVarBase::GetBoolDataFromString() const{
	return Variable( SS_BASE_ARGS_DEFAULTS, GetStringData() ).GetBoolData();
}

StringType SpecialVarBase::GetStringDataFromNum() const{
	return Variable( SS_BASE_ARGS_DEFAULTS, GetNumData() ).GetStringData();
}

StringType SpecialVarBase::GetStringDataFromBool() const{
	return Variable( SS_BASE_ARGS_DEFAULTS, GetBoolData() ).GetStringData();
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 SpecialVarBase::GetVariablePtr
 NOTES: Creates a variable and returns it.
*/
VariablePtr SpecialVarBase::GetVariablePtr(){
	return MakeVariable();
}

const VariablePtr SpecialVarBase::GetVariablePtr() const{
	return MakeVariable();
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 SpecialVarBase::TonsOfMathFuncs
 NOTES: The method I'm going to use, so that SpecialVars act just like Variables
		is to spring a new Variable into being whenever an operator is called.
*/
VariableBasePtr SpecialVarBase::operator+(const VariableBase& X) const{
	return (*MakeVariable()) + X;
}

VariableBasePtr SpecialVarBase::operator-(const VariableBase& X) const{
return (*MakeVariable()) - X;
}


VariableBasePtr SpecialVarBase::operator*(const VariableBase& X) const{
	return (*MakeVariable()) * X;
}

VariableBasePtr SpecialVarBase::operator/(const VariableBase& X) const{
	return (*MakeVariable()) / X;
}

VariableBasePtr SpecialVarBase::operator=(const VariableBase& X){
	STRING tmp = TXT("Cannot assign to special variable: \'");
	tmp += this->GetFullName();
	tmp += TXT("\'.");
	ThrowParserAnomaly( tmp, ANOMALY_CONSTASSIGN );
}


VariableBasePtr SpecialVarBase::operator==(const VariableBase& X) const{
	return (*MakeVariable()) == X;
}

VariableBasePtr SpecialVarBase::operator!=(const VariableBase& X) const{
	return (*MakeVariable()) != X;
}

VariableBasePtr SpecialVarBase::operator>=(const VariableBase& X) const{
	return (*MakeVariable()) >= X;
}

VariableBasePtr SpecialVarBase::operator<=(const VariableBase& X) const{
	return (*MakeVariable()) <= X;
}

VariableBasePtr SpecialVarBase::operator> (const VariableBase& X) const{
	return (*MakeVariable()) > X;
}

VariableBasePtr SpecialVarBase::operator< (const VariableBase& X) const{
	return (*MakeVariable()) < X;
}

VariableBasePtr SpecialVarBase::operator&&(const VariableBase& X) const{
	return (*MakeVariable()) && X;
}

VariableBasePtr SpecialVarBase::operator||(const VariableBase& X) const{
	return (*MakeVariable()) || X;
}


VariableBasePtr SpecialVarBase::op_not() const{
	return (*MakeVariable()).op_not();
}


VariableBasePtr SpecialVarBase::op_neg() const{
	return (*MakeVariable()).op_not();
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BoundFlagVar::BoundFlagVar
NOTES: 
*/
BoundFlagVar::BoundFlagVar( const STRING& Name, bool& Flag,
						    bool Static /*= false*/, bool Const /*= false*/)
: SpecialVarBase( Name, Static, Const ), mFlag(Flag)
{}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BoundFlagVar::operator=
NOTES: Assignment
*/
VariableBasePtr BoundFlagVar::operator=( const VariableBase& X ){
	mFlag = X.GetBoolData();
	return boost::dynamic_pointer_cast<VariableBase>( ScopeObjectPtr(mpThis) );
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BoundFlagVar::GetVariableType
NOTES: Return the prefered type.  In this case, bool.
*/
VarType BoundFlagVar::GetVariableType() const{
	return VARTYPE_BOOL;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BoundFlagVar::Get____Data
NOTES: 
*/
StringType BoundFlagVar::GetStringData() const{
	return Variable( SS_BASE_ARGS_DEFAULTS, mFlag ).GetStringData();
}

NumType BoundFlagVar::GetNumData() const{
	return Variable( SS_BASE_ARGS_DEFAULTS, mFlag ).GetNumData();
}


BoolType BoundFlagVar::GetBoolData() const{
	return mFlag;
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 BoundStringVar::BoundStringVar
 NOTES: contructor
*/
BoundStringVar::BoundStringVar( const STRING& Name, STRING& String,
							    bool Static /*=false*/, bool Const /*=false*/ )
	: SpecialVarBase( Name, Static, Const ), mString(String)
{
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 BoundStringVar::operator=
 NOTES: Actually changes the name of a variable.  Scary.
*/
VariableBasePtr BoundStringVar::operator=( const VariableBase& X )
{
	mString = X.GetStringData();
	return GetVariableBasePtr();
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 BoundStringVar::GetVariableType
 NOTES: Prefered type is obviously a string.
*/
VarType BoundStringVar::GetVariableType() const{
	return VARTYPE_STRING;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 BoundStringVar::Get___Data
 NOTES: 
*/
StringType BoundStringVar::GetStringData() const
{
	return mString;
}

NumType BoundStringVar::GetNumData() const
{
	return GetNumDataFromString();
}

BoolType BoundStringVar::GetBoolData() const
{
	return GetBoolDataFromString();
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BoundNumVar::BoundNumVar
NOTES: constructor
*/
BoundNumVar::BoundNumVar( const STRING& Name, NumType& Num,
						  bool Static /*=false*/, bool Const /*=false*/ )
							   : SpecialVarBase( Name, Static, Const ), mNum(Num)
{
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BoundNumVar::operator=
NOTES: Actually changes the name of a variable.  Scary.
*/
VariableBasePtr BoundNumVar::operator=( const VariableBase& X )
{
	mNum = X.GetNumData();
	return GetVariableBasePtr();
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BoundStringVar::GetVariableType
NOTES: Preferred type is obviously a num.
*/
VarType BoundNumVar::GetVariableType() const{
	return VARTYPE_NUM;
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BoundStringVar::Get___Data
NOTES: 
*/
StringType BoundNumVar::GetStringData() const
{
	return GetStringDataFromNum();
}

NumType BoundNumVar::GetNumData() const
{
	return mNum;
}

BoolType BoundNumVar::GetBoolData() const
{
	return GetBoolDataFromNum();
}




/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 FullNameVar::FullNameVar
 NOTES: constructor
*/
FullNameVar::FullNameVar( const STRING& Name, const ScopeObject& Parent,
						  bool Static /*=false*/, bool Const /*=false*/ )
						  : SpecialVarBase( Name, Static, Const ), mParent(Parent)
{
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 FullNameVar::GetVariableType
 NOTES: Returns the preferred type: a string.
*/
VarType FullNameVar::GetVariableType() const{
	return VARTYPE_STRING;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 FullNameVar::Get_____Data
 NOTES: Returns the full name of the parent in one form or another.
*/
NumType FullNameVar::GetNumData() const{
	return GetNumDataFromString();
}

BoolType FullNameVar::GetBoolData() const{
	return GetBoolDataFromString();
}

StringType FullNameVar::GetStringData() const{
	return mParent.GetFullName();
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
ListLengthVar::ListLengthVar
NOTES: 
*/
ListLengthVar::ListLengthVar( const STRING& Name, List& Parent,
							  bool Static /*= false*/, bool Const /*= false*/)
: SpecialVarBase( Name, Static, Const ), mParent(Parent)
{

}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
ListLengthVar
NOTES: Used to resize the list.  Must an integer >= 0.
*/
VariableBasePtr ListLengthVar::operator=( const VariableBase& X )
{
	mParent.Resize( X.GetNumData() );
	return  GetVariableBasePtr();
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
ListLengthVar::GetVariableType
NOTES: It prefers BigNum
*/
VarType ListLengthVar::GetVariableType() const{
	return VARTYPE_NUM;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
ListLengthVar::Get_____Data
NOTES: Returns the length of the list.
*/
StringType ListLengthVar::GetStringData() const
{
 	return mParent.Length()->GetStringData();	
}

NumType ListLengthVar::GetNumData() const
{
	return mParent.Length()->GetNumData();
}


BoolType ListLengthVar::GetBoolData() const
{
	return mParent.Length()->GetBoolData();	
}





/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Precision::Precision
 NOTES: constructor
*/
PrecisionVar::PrecisionVar( const SS::STRING& Name, Variable& Parent,
						    bool Static /*=false*/, bool Const /*=false*/ )
	: SpecialVarBase( Name, Static, Parent.IsConst() ), mParent(Parent)
{}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 PrecisionVar::operator=
 NOTES: Magical assignment changes the NumType precision.
*/
VariableBasePtr PrecisionVar::operator=(const VariableBase& X )
{
	static const NumType MinPrecision( MPFR_PREC_MIN );
	static const NumType MaxPrecision( MPFR_PREC_MAX );
    

	if( X.GetNumData() < MinPrecision )
	{
		STRING tmp = TXT("Tried to set precision to \'");
		tmp += X.GetStringData();
		tmp += TXT("\'.  Can't handle lower than \'");
		tmp += boost::lexical_cast<STRING>( MPFR_PREC_MIN );
		tmp += TXT("\'.");
		ThrowParserAnomaly( tmp, ANOMALY_BADPRECISION );
	}
	
	if( X.GetNumData() > MaxPrecision )
	{
		STRING tmp = TXT("Tried to set precision to \'");
		tmp += X.GetStringData();
		tmp += TXT("\'.  Can't handle higher than \'");
		tmp += boost::lexical_cast<STRING>( MPFR_PREC_MAX );
		tmp += TXT("\'.");
		ThrowParserAnomaly( tmp, ANOMALY_BADPRECISION );
	}

	mpfr_prec_t NewPrec = X.GetNumData().get_ui();


	mParent.GetActualNumData().set_prec( NewPrec );

	return GetVariableBasePtr();
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 PrecisionVar::GetVariableType
 NOTES: Returns the preferred type (NumType).
*/
VarType PrecisionVar::GetVariableType() const{
	return VARTYPE_NUM;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 PrecisionVar::Get____Data
 NOTES: Returns the precision of the variables.
*/
NumType PrecisionVar::GetNumData() const
{
    return NumType( mParent.GetActualNumData().get_prec() );
}

BoolType PrecisionVar::GetBoolData() const
{
	return GetBoolDataFromNum();
}

StringType PrecisionVar::GetStringData() const
{
	return GetStringDataFromNum();
}


