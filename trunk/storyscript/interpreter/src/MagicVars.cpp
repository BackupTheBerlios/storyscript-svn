/*
Copyright (c) 2004-2006 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.
Full license information is included in the file in the top
directory named "license".
*/

#include "MagicVars.hpp"
#include "ParserAnomaly.hpp"
#include "CreationFuncs.hpp"

//Use by ListLengthVar
#include "List.hpp"

#include "Variable.hpp"


//used by PrecisionVar::operator=
#include <boost/lexical_cast.hpp>





using namespace SS;



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
MagicVarBase::MagicVarBase( SS_DECLARE_BASE_ARGS )
: VariableBase( SS_BASE_ARGS )
{
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
MagicVarBase::~MagicVarBase()
{
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariablePtr MagicVarBase::MakeVariable() const
{
	VariablePtr pNewVar;

	switch( this->GetVariableType() ){
		case VARTYPE_NUM:
			return CreateVariable<Variable>( UNNAMMED, true, GetNumData() );

		case VARTYPE_String:
			return CreateVariable<Variable>( UNNAMMED, true, GetStringData() );

		case VARTYPE_BOOL:
			return CreateVariable<Variable>( UNNAMMED, true, GetBoolData() );

		default:
			ThrowParserAnomaly( TXT("Unknown variable type."), ANOMALY_PANIC );
	}
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
NumType MagicVarBase::GetNumDataFromBool() const{
	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, GetBoolData() )->GetNumData();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
NumType MagicVarBase::GetNumDataFromString() const{
	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, GetStringData() )->GetNumData();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BoolType MagicVarBase::GetBoolDataFromNum() const{
	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, GetNumData() )->GetBoolData();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BoolType MagicVarBase::GetBoolDataFromString() const{
	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, GetStringData() )->GetBoolData();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
StringType MagicVarBase::GetStringDataFromNum() const{
	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, GetNumData() )->GetStringData();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
StringType MagicVarBase::GetStringDataFromBool() const{
	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, GetBoolData() )->GetStringData();
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariablePtr MagicVarBase::CastToVariable(){
	return MakeVariable();
}

const VariablePtr MagicVarBase::CastToVariable() const{
	return MakeVariable();
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr MagicVarBase::operator+(const VariableBase& X) const{
	return (*MakeVariable()) + X;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr MagicVarBase::operator-(const VariableBase& X) const{
	return (*MakeVariable()) - X;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr MagicVarBase::operator*(const VariableBase& X) const{
	return (*MakeVariable()) * X;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr MagicVarBase::operator/(const VariableBase& X) const{
	return (*MakeVariable()) / X;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr MagicVarBase::operator=(const VariableBase& X){
	String tmp = TXT("Cannot assign to magic variable: \'");
	tmp += this->GetFullName();
	tmp += TXT("\'.");
	ThrowParserAnomaly( tmp, ANOMALY_CONSTASSIGN );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr MagicVarBase::operator==(const VariableBase& X) const{
	return (*MakeVariable()) == X;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr MagicVarBase::operator!=(const VariableBase& X) const{
	return (*MakeVariable()) != X;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr MagicVarBase::operator>=(const VariableBase& X) const{
	return (*MakeVariable()) >= X;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr MagicVarBase::operator<=(const VariableBase& X) const{
	return (*MakeVariable()) <= X;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr MagicVarBase::operator> (const VariableBase& X) const{
	return (*MakeVariable()) > X;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr MagicVarBase::operator< (const VariableBase& X) const{
	return (*MakeVariable()) < X;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr MagicVarBase::operator&&(const VariableBase& X) const{
	return (*MakeVariable()) && X;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr MagicVarBase::operator||(const VariableBase& X) const{
	return (*MakeVariable()) || X;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr MagicVarBase::op_not() const{
	return (*MakeVariable()).op_not();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr MagicVarBase::op_neg() const{
	return (*MakeVariable()).op_not();
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BoundFlagVar::BoundFlagVar( SS_DECLARE_BASE_ARGS, bool& Flag )
: MagicVarBase( SS_BASE_ARGS ), mFlag(Flag)
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr BoundFlagVar::operator=( const VariableBase& X ){
	mFlag = X.GetBoolData();
	return boost::dynamic_pointer_cast<VariableBase>( ScopeObjectPtr(mpThis) );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VarType BoundFlagVar::GetVariableType() const{
	return VARTYPE_BOOL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
StringType BoundFlagVar::GetStringData() const{
	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, mFlag )->GetStringData();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
NumType BoundFlagVar::GetNumData() const{
	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, mFlag )->GetNumData();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BoolType BoundFlagVar::GetBoolData() const{
	return mFlag;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BoundStringVar::BoundStringVar( SS_DECLARE_BASE_ARGS, String& String )
	: MagicVarBase( SS_BASE_ARGS ), mString(String)
{
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr BoundStringVar::operator=( const VariableBase& X )
{
	mString = X.GetStringData();
	return CastToVariableBase();
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VarType BoundStringVar::GetVariableType() const{
	return VARTYPE_String;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
StringType BoundStringVar::GetStringData() const
{
	return mString;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
NumType BoundStringVar::GetNumData() const
{
	return GetNumDataFromString();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BoolType BoundStringVar::GetBoolData() const
{
	return GetBoolDataFromString();
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BoundNumVar::BoundNumVar( SS_DECLARE_BASE_ARGS, NumType& Num )
							   : MagicVarBase( SS_BASE_ARGS ), mNum(Num)
{
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr BoundNumVar::operator=( const VariableBase& X )
{
	mNum = X.GetNumData();
	return CastToVariableBase();
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VarType BoundNumVar::GetVariableType() const{
	return VARTYPE_NUM;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
StringType BoundNumVar::GetStringData() const
{
	return GetStringDataFromNum();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
NumType BoundNumVar::GetNumData() const
{
	return mNum;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BoolType BoundNumVar::GetBoolData() const
{
	return GetBoolDataFromNum();
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BoundULongVar::BoundULongVar( SS_DECLARE_BASE_ARGS, unsigned long& ULong )
: MagicVarBase( SS_BASE_ARGS ), mNum( ULong )
{
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr BoundULongVar::operator=( const VariableBase& X )
{
	mNum = mpfr_get_ui( X.GetNumData().get(), GMP_RNDN );
	return CastToVariableBase();	
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VarType BoundULongVar::GetVariableType() const{
	return VARTYPE_NUM;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
NumType BoundULongVar::GetNumData() const{
	NumType New;
	New.set( mNum );
	return New;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BoolType BoundULongVar::GetBoolData() const{
	return GetBoolDataFromNum();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
StringType BoundULongVar::GetStringData() const{
	return GetStringDataFromNum();
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BoundUShortVar::BoundUShortVar( SS_DECLARE_BASE_ARGS, unsigned short& UShort )
: MagicVarBase( SS_BASE_ARGS ), mNum( UShort )
{
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr BoundUShortVar::operator=( const VariableBase& X )
{
	mNum = (unsigned short)mpfr_get_ui( X.GetNumData().get(), GMP_RNDN );
	return CastToVariableBase();	
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VarType BoundUShortVar::GetVariableType() const{
	return VARTYPE_NUM;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
NumType BoundUShortVar::GetNumData() const{
	NumType New;
	New.set( (unsigned long)mNum );
	return New;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BoolType BoundUShortVar::GetBoolData() const{
	return GetBoolDataFromNum();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
StringType BoundUShortVar::GetStringData() const{
	return GetStringDataFromNum();
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BoundRoundModeVar::BoundRoundModeVar( SS_DECLARE_BASE_ARGS, mpfr_rnd_t& RoundMode )
: MagicVarBase( SS_BASE_ARGS ), mRoundMode( RoundMode )
{
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VarType BoundRoundModeVar::GetVariableType() const{
	return VARTYPE_NUM;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BoolType BoundRoundModeVar::GetBoolData() const{
	return GetBoolDataFromNum();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
StringType BoundRoundModeVar::GetStringData() const{
	return GetStringDataFromNum();
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
FullNameVar::FullNameVar( SS_DECLARE_BASE_ARGS, const ScopeObject& Parent )
	: MagicVarBase( SS_BASE_ARGS ), mParent(Parent)
{
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VarType FullNameVar::GetVariableType() const{
	return VARTYPE_String;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
NumType FullNameVar::GetNumData() const{
	return GetNumDataFromString();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BoolType FullNameVar::GetBoolData() const{
	return GetBoolDataFromString();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
StringType FullNameVar::GetStringData() const{
	return mParent.GetFullName();
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
ListLengthVar::ListLengthVar( SS_DECLARE_BASE_ARGS, List& Parent )
: MagicVarBase( SS_BASE_ARGS ), mParent(Parent)
{

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr ListLengthVar::operator=( const VariableBase& X )
{
	mParent.Resize( X.GetNumData() );
	return  CastToVariableBase();
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VarType ListLengthVar::GetVariableType() const{
	return VARTYPE_NUM;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
StringType ListLengthVar::GetStringData() const
{
 	return mParent.Length()->GetStringData();	
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
NumType ListLengthVar::GetNumData() const
{
	return mParent.Length()->GetNumData();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BoolType ListLengthVar::GetBoolData() const
{
	return mParent.Length()->GetBoolData();	
}





//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
PrecisionVar::PrecisionVar( const String& Name, Variable& Parent )
	: MagicVarBase( Name, Parent.IsConst() ), mParent(Parent)
{}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr PrecisionVar::operator=(const VariableBase& X )
{
	static const NumType MinPrecision( MPFR_PREC_MIN );
	static const NumType MaxPrecision( MPFR_PREC_MAX );
    

	if( mpfr_less_p( X.GetNumData().get(), MinPrecision.get() ) )
	{
		String tmp = TXT("Tried to set precision to \'");
		tmp += X.GetStringData();
		tmp += TXT("\'.  Can't handle lower than \'");
		tmp += boost::lexical_cast<String>( MPFR_PREC_MIN );
		tmp += TXT("\'.");
		ThrowParserAnomaly( tmp, ANOMALY_BADPRECISION );
	}
	
	if( mpfr_greater_p( X.GetNumData().get(), MaxPrecision.get() ) )
	{
		String tmp = TXT("Tried to set precision to \'");
		tmp += X.GetStringData();
		tmp += TXT("\'.  Can't handle higher than \'");
		tmp += boost::lexical_cast<String>( MPFR_PREC_MAX );
		tmp += TXT("\'.");
		ThrowParserAnomaly( tmp, ANOMALY_BADPRECISION );
	}

	mpfr_prec_t NewPrec = mpfr_get_ui( X.GetNumData().get(), GMP_RNDN );
	mpfr_set_prec( mParent.GetActualNumData().get(), NewPrec );

	return CastToVariableBase();
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VarType PrecisionVar::GetVariableType() const{
	return VARTYPE_NUM;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
NumType PrecisionVar::GetNumData() const
{
	NumType Out;
	mpfr_set_ui( Out.get(), mpfr_get_prec(mParent.GetActualNumData().get()), GMP_RNDN );
	return Out;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BoolType PrecisionVar::GetBoolData() const
{
	return GetBoolDataFromNum();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
StringType PrecisionVar::GetStringData() const
{
	return GetStringDataFromNum();
}


