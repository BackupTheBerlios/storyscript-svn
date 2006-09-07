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
SpecialVarBase::SpecialVarBase( SS_DECLARE_BASE_ARGS )
: VariableBase( SS_BASE_ARGS )
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
			return CreateVariable<Variable>( UNNAMMED, true, GetNumData() );

		case VARTYPE_STRING:
			return CreateVariable<Variable>( UNNAMMED, true, GetStringData() );

		case VARTYPE_BOOL:
			return CreateVariable<Variable>( UNNAMMED, true, GetBoolData() );

		default:
			ThrowParserAnomaly( TXT("Unknown variable type."), ANOMALY_PANIC );
	}
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 SpecialVarBase::Get____DataFrom____
 NOTES: These are to aid the children.
*/
NumType SpecialVarBase::GetNumDataFromBool() const{
	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, GetBoolData() )->GetNumData();
}

NumType SpecialVarBase::GetNumDataFromString() const{
	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, GetStringData() )->GetNumData();
}

BoolType SpecialVarBase::GetBoolDataFromNum() const{
	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, GetNumData() )->GetBoolData();
}

BoolType SpecialVarBase::GetBoolDataFromString() const{
	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, GetStringData() )->GetBoolData();
}

StringType SpecialVarBase::GetStringDataFromNum() const{
	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, GetNumData() )->GetStringData();
}

StringType SpecialVarBase::GetStringDataFromBool() const{
	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, GetBoolData() )->GetStringData();
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 SpecialVarBase::GetVariablePtr
 NOTES: Creates a variable and returns it.
*/
VariablePtr SpecialVarBase::CastToVariable(){
	return MakeVariable();
}

const VariablePtr SpecialVarBase::CastToVariable() const{
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
BoundFlagVar::BoundFlagVar( SS_DECLARE_BASE_ARGS, bool& Flag )
: SpecialVarBase( SS_BASE_ARGS ), mFlag(Flag)
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
	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, mFlag )->GetStringData();
}

NumType BoundFlagVar::GetNumData() const{
	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, mFlag )->GetNumData();
}


BoolType BoundFlagVar::GetBoolData() const{
	return mFlag;
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 BoundStringVar::BoundStringVar
 NOTES: contructor
*/
BoundStringVar::BoundStringVar( SS_DECLARE_BASE_ARGS, STRING& String )
	: SpecialVarBase( SS_BASE_ARGS ), mString(String)
{
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 BoundStringVar::operator=
 NOTES: Actually changes the name of a variable.  Scary.
*/
VariableBasePtr BoundStringVar::operator=( const VariableBase& X )
{
	mString = X.GetStringData();
	return CastToVariableBase();
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
BoundNumVar::BoundNumVar( SS_DECLARE_BASE_ARGS, NumType& Num )
							   : SpecialVarBase( SS_BASE_ARGS ), mNum(Num)
{
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BoundNumVar::operator=
NOTES: Actually changes the name of a variable.  Scary.
*/
VariableBasePtr BoundNumVar::operator=( const VariableBase& X )
{
	mNum = X.GetNumData();
	return CastToVariableBase();
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
 NOTES: Constructor
*/
BoundULongVar::BoundULongVar( SS_DECLARE_BASE_ARGS, unsigned long& ULong )
: SpecialVarBase( SS_BASE_ARGS ), mNum( ULong )
{
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Overloaded assignment operator.
*/
VariableBasePtr BoundULongVar::operator=( const VariableBase& X )
{
	mNum = mpfr_get_ui( X.GetNumData().get(), GMP_RNDN );
	return CastToVariableBase();	
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Return prefered type (number).
*/
VarType BoundULongVar::GetVariableType() const{
	return VARTYPE_NUM;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Get__Data -- Return useable data.
*/
NumType BoundULongVar::GetNumData() const{
	NumType New;
	New.set( mNum );
	return New;
}

BoolType BoundULongVar::GetBoolData() const{
	return GetBoolDataFromNum();
}

StringType BoundULongVar::GetStringData() const{
	return GetStringDataFromNum();
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Constructor
*/
BoundUShortVar::BoundUShortVar( SS_DECLARE_BASE_ARGS, unsigned short& UShort )
: SpecialVarBase( SS_BASE_ARGS ), mNum( UShort )
{
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Overloaded assignment operator.
*/
VariableBasePtr BoundUShortVar::operator=( const VariableBase& X )
{
	mNum = (unsigned short)mpfr_get_ui( X.GetNumData().get(), GMP_RNDN );
	return CastToVariableBase();	
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Return prefered type (number).
*/
VarType BoundUShortVar::GetVariableType() const{
	return VARTYPE_NUM;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Get__Data -- Return useable data.
*/
NumType BoundUShortVar::GetNumData() const{
	NumType New;
	New.set( (unsigned long)mNum );
	return New;
}

BoolType BoundUShortVar::GetBoolData() const{
	return GetBoolDataFromNum();
}

StringType BoundUShortVar::GetStringData() const{
	return GetStringDataFromNum();
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Constructor
*/
BoundRoundModeVar::BoundRoundModeVar( SS_DECLARE_BASE_ARGS, mpfr_rnd_t& RoundMode )
: SpecialVarBase( SS_BASE_ARGS ), mRoundMode( RoundMode )
{
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Overloaded assignment operator.
*/
VariableBasePtr BoundRoundModeVar::operator=( const VariableBase& X )
{
	unsigned long tmp = (unsigned short)mpfr_get_ui( X.GetNumData().get(), GMP_RNDN );
	
	switch( tmp ){
		case 0:
			mRoundMode = GMP_RNDN;
			break;
		case 1:
			mRoundMode = GMP_RNDZ;
			break;
		case 2:
			mRoundMode = GMP_RNDU;
			break;
		case 3:
			mRoundMode = GMP_RNDD;
			break;
		case 4:
			mRoundMode = GMP_RND_MAX;
			break;
		default:
			mRoundMode = GMP_RNDN;
	}
	
	return CastToVariableBase();	
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Return prefered type (number).
*/
VarType BoundRoundModeVar::GetVariableType() const{
	return VARTYPE_NUM;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Get__Data -- Return useable data.
*/
NumType BoundRoundModeVar::GetNumData() const{
	
	NumType New;
	
	switch( mRoundMode ){
		case GMP_RNDN:
			New.set( 0 );
		case GMP_RNDZ:
			New.set( 1 );
		case GMP_RNDU:
			New.set( 2 );
		case GMP_RNDD:
			New.set( 3 );
		case GMP_RND_MAX:
			New.set( 4 );
		default:
			New.set( 0 );
	}
	
	return New;
}

BoolType BoundRoundModeVar::GetBoolData() const{
	return GetBoolDataFromNum();
}

StringType BoundRoundModeVar::GetStringData() const{
	return GetStringDataFromNum();
}





/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 FullNameVar::FullNameVar
 NOTES: constructor
*/
FullNameVar::FullNameVar( SS_DECLARE_BASE_ARGS, const ScopeObject& Parent )
	: SpecialVarBase( SS_BASE_ARGS ), mParent(Parent)
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
ListLengthVar::ListLengthVar( SS_DECLARE_BASE_ARGS, List& Parent )
: SpecialVarBase( SS_BASE_ARGS ), mParent(Parent)
{

}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
ListLengthVar
NOTES: Used to resize the list.  Must an integer >= 0.
*/
VariableBasePtr ListLengthVar::operator=( const VariableBase& X )
{
	mParent.Resize( X.GetNumData() );
	return  CastToVariableBase();
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
PrecisionVar::PrecisionVar( const STRING& Name, Variable& Parent )
	: SpecialVarBase( Name, Parent.IsConst() ), mParent(Parent)
{}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 PrecisionVar::operator=
 NOTES: Magical assignment changes the NumType precision.
*/
VariableBasePtr PrecisionVar::operator=(const VariableBase& X )
{
	static const NumType MinPrecision( MPFR_PREC_MIN );
	static const NumType MaxPrecision( MPFR_PREC_MAX );
    

	if( mpfr_less_p( X.GetNumData().get(), MinPrecision.get() ) )
	{
		STRING tmp = TXT("Tried to set precision to \'");
		tmp += X.GetStringData();
		tmp += TXT("\'.  Can't handle lower than \'");
		tmp += boost::lexical_cast<STRING>( MPFR_PREC_MIN );
		tmp += TXT("\'.");
		ThrowParserAnomaly( tmp, ANOMALY_BADPRECISION );
	}
	
	if( mpfr_greater_p( X.GetNumData().get(), MaxPrecision.get() ) )
	{
		STRING tmp = TXT("Tried to set precision to \'");
		tmp += X.GetStringData();
		tmp += TXT("\'.  Can't handle higher than \'");
		tmp += boost::lexical_cast<STRING>( MPFR_PREC_MAX );
		tmp += TXT("\'.");
		ThrowParserAnomaly( tmp, ANOMALY_BADPRECISION );
	}

	mpfr_prec_t NewPrec = mpfr_get_ui( X.GetNumData().get(), GMP_RNDN );
	mpfr_set_prec( mParent.GetActualNumData().get(), NewPrec );

	return CastToVariableBase();
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
	NumType Out;
	mpfr_set_ui( Out.get(), mpfr_get_prec(mParent.GetActualNumData().get()), GMP_RNDN );
	return Out;
}

BoolType PrecisionVar::GetBoolData() const
{
	return GetBoolDataFromNum();
}

StringType PrecisionVar::GetStringData() const
{
	return GetStringDataFromNum();
}


