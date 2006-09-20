/*
Copyright (c) 2004-2006 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.
Full license information is included in the file in the top
directory named "license".
*/
 

#include "Variable.hpp"
#include <boost/lexical_cast.hpp>
#include "ParserAnomaly.hpp"
#include "LanguageConstants.hpp"
#include "MagicVars.hpp"
//#include "BaseFuncs.hpp"
#include "List.hpp"
#include "CreationFuncs.hpp"
#include <cstring>

//Just for a quick test.  Please remove this later.
#include <iostream>

using namespace SS;



/*

	DEFINITIONS FOR THE MPFR MINI-WRAPPER

*/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
mpfr_t_wrap::mpfr_t_wrap()
{
	mpfr_init(N);	
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
mpfr_t_wrap::mpfr_t_wrap( int prec )
{
	mpfr_init2( N, prec );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
mpfr_t_wrap::mpfr_t_wrap( const mpfr_t_wrap& X )
{
	mpfr_init(N);
	mpfr_set( N, X.get(), LangOpts::Instance().RoundingMode );	
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
mpfr_t_wrap& mpfr_t_wrap::operator=( const mpfr_t_wrap& X )
{
	mpfr_set( N, X.get(), LangOpts::Instance().RoundingMode );
	return *this;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
mpfr_t_wrap::~mpfr_t_wrap()
{
	mpfr_clear(N);	
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
mpfr_t& mpfr_t_wrap::get() const
{
	return N;	
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
mpfr_t_wrap& mpfr_t_wrap::set( int x )
{
	return set( (signed long)x );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
mpfr_t_wrap& mpfr_t_wrap::set( signed long x )
{
	mpfr_set_si( N, x, LangOpts::Instance().RoundingMode );
	return *this;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
mpfr_t_wrap& mpfr_t_wrap::set( unsigned long x )
{
	mpfr_set_ui( N, x, LangOpts::Instance().RoundingMode );
	return *this;	
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
mpfr_t_wrap& mpfr_t_wrap::set( double x )
{
	mpfr_set_d( N, x, LangOpts::Instance().RoundingMode );
	return *this;	
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void SS::NumType2StringType( const NumType& In, StringType& Out )
{
	//Special Cases
	if( mpfr_nan_p( In.get() ) ){ Out = TXT(""); return; }
	if( mpfr_inf_p( In.get() ) ){ Out = TXT("%Inf%"); return; }

	mp_exp_t Exponent = 0;
	
	char* TmpString = 	
	mpfr_get_str( 0,
				  &Exponent,
				  LangOpts::Instance().NumberBase,
				  LangOpts::Instance().MaxDigitOutput,
				  In.get(),  
				  LangOpts::Instance().RoundingMode );
	
	if( TmpString == 0 )  ThrowParserAnomaly(
		 TXT("Something went horribly wrong during a "
		 "routine Num -> String conversion."), ANOMALY_PANIC ); 
	
	size_t TmpSize = strlen( TmpString );
	
	int Neg;
	TmpString[0] == '-' ? Neg = 1 : Neg = 0;
	
	//This is to prevent copying over any trailing zeros
	int Last = -1, i;
	for( i = 0; (size_t)i < TmpSize; i++ ) if( TmpString[i] != '0' ) Last = i;
	
	Out = TXT("");
	
	//Special case for zeros.
	if( Last == -1 ) { Out =  TXT("0"); return; }


	if( Exponent <= 0 )
	{
		if( Neg ) Out += '-';
		Out += TXT("0.");
		
		for( Exponent++; Exponent < 0; Exponent++ ) Out += '0';	
		
		for( i = 0 + Neg; i <= Last; i++ ) Out += TmpString[i];
	}
	else
	if ( Exponent > 0 )
	{
		 
		Exponent += Neg;
		
		if( (size_t)Exponent < TmpSize )
		{
			//Copy the first part over
			for( i = 0; i < Exponent; i++ ) Out += TmpString[i];
			
			if( i <= Last )
			{
				//Add a decimal point
				Out += LC_DecimalPoint;
				
				//Copy the second part over
				for( i = Exponent; i <= Last; i++ ) Out += TmpString[i];			
			}
		}
		else Out += TmpString;
	}
	
	//Remove trailing zeros
	//while( Out[Out.Length() - 1] == '0' ) 
	
	mpfr_free_str( TmpString );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
StringType SS::NumType2StringType( const NumType& In )
{
	StringType Out;
	NumType2StringType( In, Out );
	return Out;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void SS::StringType2NumType( const StringType& In, NumType& Out )
{
	//We are trusting in mpfr to do The Right Thing.
	//We may want to check for a -1 (error) return value in the future.
	mpfr_set_str( Out.get(),
	              NarrowizeString(In).c_str(),
	              LangOpts::Instance().NumberBase,
	              LangOpts::Instance().RoundingMode );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
NumType SS::StringType2NumType( const StringType& In )
{
	NumType Out;
	StringType2NumType( In, Out );
	return Out;
}
	

const VarType DEFAULT_VARTYPE = VARTYPE_BOOL;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBase::VariableBase( SS_DECLARE_BASE_ARGS )
: Scope( SS_BASE_ARGS )
{
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBase::~VariableBase()
{
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr VariableBase::CastToVariableBase(){
	AssertCastingAllowed();
	return boost::dynamic_pointer_cast<VariableBase>( ScopeObjectPtr( mpThis ) );
}

const VariableBasePtr VariableBase::CastToVariableBase() const{
	AssertCastingAllowed();
	return boost::dynamic_pointer_cast<VariableBase>( ScopeObjectPtr( mpThis ) );
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void VariableBase::AcceptVisitor( ScopeObjectVisitor& V ){
	V.VisitVariableBase( this );
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VarType VariableBase::GetVariableType() const{
	return VARTYPE_STRING;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
ListPtr VariableBase::CastToList()
{
	AssertCastingAllowed();
	ListPtr pNewList = CreateGeneric<List>( GetName(), false );
	pNewList->PushWithoutCopy( this->CastToVariable() );

	return pNewList;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
const ListPtr VariableBase::CastToList() const
{
	AssertCastingAllowed();
	ListPtr pNewList = CreateGeneric<List>( GetName(), false );
	pNewList->PushWithoutCopy( this->CastToVariable() );

	return pNewList;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
NumType VariableBase::GetNumData() const{
	NumType N;
	return N;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BoolType VariableBase::GetBoolData() const{
	return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
StringType VariableBase::GetStringData() const{
	//return this->GetFullName();
	return TXT("");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
NumType& VariableBase::GetNumData( NumType& Out ) const{
	mpfr_set( Out.get(), GetNumData().get(), LangOpts::Instance().RoundingMode );
	return Out;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BoolType& VariableBase::GetBoolData( BoolType& Out ) const{
	Out = GetBoolData();
	return Out;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
StringType& VariableBase::GetStringData( StringType& Out ) const{
	Out = GetStringData();
	return Out;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
CHAR* VariableBase::GetStringData( CHAR* Buffer, unsigned int BufferSize ) const
{
	SS::STRCPY( Buffer, GetStringData().c_str(), BufferSize );
	return Buffer;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr VariableBase::UndefinedOp( const SS::STRING& Op) const
{
	TypeCheckVisitor TypeChecker;

	//This is safe, because TypeChekcer doesn't modify anything.
	const_cast<VariableBase*>(this)->AcceptVisitor( TypeChecker );

	STRING tmp = TXT("\'");
	tmp += Op;
	tmp += TXT("\' operator not defined by \'");
	tmp += TypeChecker.ReturnTypeString();
	tmp += TXT("\'.");

	ThrowParserAnomaly( tmp, ANOMALY_UNDEFINEDOP );

	//THE BELOW WILL NEVER GET EXECUTED
	//The problem is that the operators will not compile unless they return something
	//So I have them returning the result of this functions.
	return VariableBasePtr();
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr VariableBase::operator+(const VariableBase&) const{
    return UndefinedOp( TXT("Addition") );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr VariableBase::operator-(const VariableBase&) const{
	return UndefinedOp( TXT("Subtraction") );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr VariableBase::operator*(const VariableBase&) const{
	return UndefinedOp( TXT("Multiplication") );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr VariableBase::operator/(const VariableBase&) const{
	return UndefinedOp( TXT("Division") );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr VariableBase::operator_pow( const VariableBase& ) const{
	return UndefinedOp( TXT("Exponentation") );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr VariableBase::operator_concat( const VariableBase& ) const{
	return UndefinedOp( TXT("Concatenation") );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr VariableBase::operator=(const VariableBase&){
	return UndefinedOp( TXT("Assignment") );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr VariableBase::operator==(const VariableBase&) const{
	return UndefinedOp( TXT("Equal") );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr VariableBase::operator!=(const VariableBase&) const{
	return UndefinedOp( TXT("Not-Equal") );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr VariableBase::operator>=(const VariableBase&) const{
	return UndefinedOp( TXT("Greater-Than-Or-Equal") );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr VariableBase::operator<=(const VariableBase&) const{
	return UndefinedOp( TXT("Less-Than-Or-Equal") );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr VariableBase::operator>(const VariableBase&) const{
	return UndefinedOp( TXT("Greater-Than") );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr VariableBase::operator<(const VariableBase&) const{
	return UndefinedOp( TXT("Less-Than") );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr VariableBase::operator&&(const VariableBase&) const{
	return UndefinedOp( TXT("Logical-And") );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr VariableBase::operator||(const VariableBase&) const{
	return UndefinedOp( TXT("Logical-Or") );
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr VariableBase::op_not() const{
	return UndefinedOp( TXT("Unary-Not") );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr VariableBase::op_neg() const{
	return UndefinedOp( TXT("Negation") );
}





//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
Variable::Variable( SS_DECLARE_BASE_ARGS,
					const Variable& X )
	: VariableBase( SS_BASE_ARGS ), 
	  mCurrentType( X.mCurrentType ),
	  mBoolPart( X.mBoolPart ),
	  mStringPart( X.mStringPart )
	  
{
	mpfr_set( mNumPart.get(), X.mNumPart.get(), LangOpts::Instance().RoundingMode );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
Variable::Variable( SS_DECLARE_BASE_ARGS )
: VariableBase(SS_BASE_ARGS),
  mCurrentType( DEFAULT_VARTYPE ),
  mNumPart( LangOpts::Instance().DefaultPrecision ),
  mBoolPart(false)
 {
 	RegisterPredefinedVars();
 }


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
Variable::Variable( SS_DECLARE_BASE_ARGS,
					const NumType& X )
: VariableBase(SS_BASE_ARGS),
  mCurrentType( VARTYPE_NUM ),
  mBoolPart(false)
{
	mpfr_set( mNumPart.get(), X.get(), LangOpts::Instance().RoundingMode );
	RegisterPredefinedVars();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
Variable::Variable( SS_DECLARE_BASE_ARGS,
					const StringType& X )
: VariableBase(SS_BASE_ARGS),
  mCurrentType( VARTYPE_STRING ),
  mNumPart( LangOpts::Instance().DefaultPrecision ),
  mBoolPart(false),
  mStringPart( X )
{
	RegisterPredefinedVars();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
Variable::Variable( SS_DECLARE_BASE_ARGS,
					const BoolType& X )
: VariableBase(SS_BASE_ARGS),
  mCurrentType( VARTYPE_BOOL ),
  mNumPart( LangOpts::Instance().DefaultPrecision ),
  mBoolPart( X )
{
	RegisterPredefinedVars();
}





VarType Variable::mTypeConversionTable[VARTYPE_STRING+1][VARTYPE_STRING+1] =
{    //Num            Bool           String
	{VARTYPE_NUM,    VARTYPE_NUM,      VARTYPE_NUM }, //Num
	{VARTYPE_BOOL,   VARTYPE_BOOL,     VARTYPE_BOOL   }, //Bool
	{VARTYPE_NUM,    VARTYPE_STRING,    VARTYPE_STRING }  //String
};  


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Variable::RegisterPredefinedVars()
{
	mPrecisionVarCreated = false;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
ScopeObjectPtr Variable::GetScopeObjectHook( const STRING& Name )
{
	if( !mPrecisionVarCreated && Name == LC_Precision ){
		mPrecisionVarCreated = true;
		return Register( ScopeObjectPtr( new PrecisionVar( LC_Precision, *this ) ) );
	}
	else return VariableBase::GetScopeObjectHook( Name );
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Variable::AcceptVisitor( ScopeObjectVisitor& V )
{
	V.VisitVariable(this);
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariablePtr Variable::CastToVariable(){
	AssertCastingAllowed();
	return boost::dynamic_pointer_cast<Variable>( ScopeObjectPtr(mpThis) );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
const VariablePtr Variable::CastToVariable() const{
	AssertCastingAllowed();
	return boost::dynamic_pointer_cast<Variable>( ScopeObjectPtr(mpThis) );
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VarType Variable::GetVariableType() const{
	return mCurrentType;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr Variable::operator+(const VariableBase& X) const
{
	VarType ContextType = mTypeConversionTable[this->mCurrentType][X.GetVariableType()];

	try{
		switch( ContextType )
		{
		case VARTYPE_NUM:
		{
			VariablePtr tmp = CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, false );
			mpfr_add( tmp->mNumPart.get(), this->GetNumData().get(), X.GetNumData().get(), LangOpts::Instance().RoundingMode );
			tmp->mCurrentType = VARTYPE_NUM;
			
			return tmp;
		}
		
		case VARTYPE_BOOL:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, this->GetBoolData() || X.GetBoolData() ) );
		case VARTYPE_STRING:
		default:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, this->GetStringData() + X.GetStringData() ) );
		}
	}
	catch( ParserAnomaly E )
	{
		if( E.ErrorCode == ANOMALY_NOCONVERSION )		{
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, this->GetStringData() + X.GetStringData() ) );
		}
		else throw;
	}
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr Variable::operator-(const VariableBase& X) const
{
	VarType ContextType = mTypeConversionTable[this->mCurrentType][X.GetVariableType()];

	try{
		switch( ContextType )
		{
		case VARTYPE_NUM:
		{
			VariablePtr tmp = CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, false );
			mpfr_sub( tmp->mNumPart.get(), this->GetNumData().get(), X.GetNumData().get(), LangOpts::Instance().RoundingMode );
			tmp->mCurrentType = VARTYPE_NUM;
			
			return tmp;
		}
		case VARTYPE_BOOL:
			//TODO: Maybe think of something better than using && for boolean subtraction
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, this->GetBoolData() && X.GetBoolData() ) );
			break;
		default:
			ThrowParserAnomaly( TXT("Can't subtract two strings."), ANOMALY_BADSTRINGOP ); 
		}
	}
	catch( ParserAnomaly E )
	{
		if( E.ErrorCode == ANOMALY_NOCONVERSION ){
			ThrowParserAnomaly( TXT("Can't subtract two strings."), ANOMALY_BADSTRINGOP ); 
		}
		else throw;
	}
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr Variable::operator*(const VariableBase& X) const
{
	VarType ContextType = mTypeConversionTable[this->mCurrentType][X.GetVariableType()];

		try{
		switch( ContextType )
		{
		case VARTYPE_STRING:
		case VARTYPE_NUM:
		{
			VariablePtr tmp = CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, false );
			mpfr_mul( tmp->mNumPart.get(), this->GetNumData().get(), X.GetNumData().get(), LangOpts::Instance().RoundingMode );
			tmp->mCurrentType = VARTYPE_NUM;
			
			return tmp;
		}
		case VARTYPE_BOOL:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, this->GetBoolData() || X.GetBoolData() ) );
			break;
		default:
			ThrowParserAnomaly( TXT("Can't multiply two strings."), ANOMALY_BADSTRINGOP ); 
		}
	}
	catch( ParserAnomaly E )
	{
		if( E.ErrorCode == ANOMALY_NOCONVERSION ){
			ThrowParserAnomaly( TXT("Can't multiply two strings."), ANOMALY_BADSTRINGOP ); 
		}
		else throw;
	}
}





//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr Variable::operator_pow( const VariableBase& X ) const
{
	VariablePtr tmp = CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, false );
	mpfr_pow( tmp->mNumPart.get(), GetNumData().get(), X.GetNumData().get(), LangOpts::Instance().RoundingMode );
	tmp->mCurrentType = VARTYPE_NUM;
	
	return tmp;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr Variable::operator_concat( const VariableBase& X ) const
{
	return VariableBasePtr( CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, 
								this->GetStringData() + X.GetStringData() ) );
	
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr Variable::operator/(const VariableBase& X) const
{
	VarType ContextType = mTypeConversionTable[this->mCurrentType][X.GetVariableType()];

	try{
		switch( ContextType )
		{
		case VARTYPE_NUM:
		{
			VariablePtr tmp = CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, false );
			mpfr_div( tmp->mNumPart.get(), this->GetNumData().get(), X.GetNumData().get(),LangOpts::Instance().RoundingMode );
			tmp->mCurrentType = VARTYPE_NUM;
			
			return tmp;
		}
		case VARTYPE_BOOL:
			//TODO: This is illogical (says Spock).
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, this->GetBoolData() && X.GetBoolData() ) );
			break;
		default:
			ThrowParserAnomaly( TXT("Can't divide two strings."), ANOMALY_BADSTRINGOP ); 
		}
	}
	catch( ParserAnomaly E )
	{
		if( E.ErrorCode == ANOMALY_NOCONVERSION ){
			ThrowParserAnomaly( TXT("Can't divide two strings."), ANOMALY_BADSTRINGOP ); 
		}
		else throw;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr Variable::operator=( const VariableBase& X )
{
	//Zero everything first.  Remember that if it is not zeroed it will
	//be assumed that the value held is the correct one.
	this->mStringPart.clear();
	mpfr_set_nan( mNumPart.get() );
	this->mBoolPart = false;

	switch( X.GetVariableType() ){
		case VARTYPE_STRING:
			this->mStringPart = X.GetStringData();
			this->mCurrentType = VARTYPE_STRING;
			break;
		case VARTYPE_NUM:
			mpfr_set( mNumPart.get(), X.GetNumData().get(), LangOpts::Instance().RoundingMode );
			this->mCurrentType = VARTYPE_NUM;
			break;
		case VARTYPE_BOOL:
			this->mBoolPart = X.GetBoolData();
			this->mCurrentType = VARTYPE_BOOL;
			break;

		default:
			//PANIC!!!
			break;
	}
	

	//return *this;
	//return VariableBasePtr( this, null_deleter() );
	return VariableBasePtr( 
		boost::dynamic_pointer_cast<VariableBase>(
			ScopeObjectPtr(mpThis) ) 
		);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr Variable::operator==( const VariableBase& X ) const
{
	VarType ContextType = mTypeConversionTable[this->mCurrentType][X.GetVariableType()];

	try{
		switch( ContextType )
		{
		case VARTYPE_NUM:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS,
					mpfr_equal_p( GetNumData().get(), X.GetNumData().get() ) ) );
				
		case VARTYPE_STRING:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS,
					this->GetStringData() == X.GetStringData() ) );
		case VARTYPE_BOOL:
		default:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS,
					this->GetBoolData() == X.GetBoolData() ) );
			break;
		}
	}
	//Catch failed string conversions
	catch( ParserAnomaly E )
	{
		if( E.ErrorCode == ANOMALY_NOCONVERSION ){
			return VariableBasePtr(	CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, false ) );
		}
		else throw;
	}

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr Variable::operator!=( const VariableBase& X ) const
{
	VarType ContextType = mTypeConversionTable[this->mCurrentType][X.GetVariableType()];

	try{
		switch( ContextType )
		{
		case VARTYPE_NUM:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS,
					!mpfr_equal_p( GetNumData().get(), X.GetNumData().get() ) ) );
			break;
		case VARTYPE_STRING:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS,
					this->GetStringData() != X.GetStringData() ) );
		case VARTYPE_BOOL:
		default:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS,
					this->GetBoolData() != X.GetBoolData() ) );
			break;
		}
	}
	//Catch failed string conversions
	catch( ParserAnomaly E )
	{
		if( E.ErrorCode == ANOMALY_NOCONVERSION ){
			return VariableBasePtr(	CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, false ) );
		}
		else throw;
	}

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr Variable::operator>=( const VariableBase& X ) const
{
	VarType ContextType = mTypeConversionTable[this->mCurrentType][X.GetVariableType()];

	try{
		switch( ContextType )
		{
		case VARTYPE_NUM:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS,
					mpfr_greaterequal_p( GetNumData().get(), X.GetNumData().get() ) ) );

		case VARTYPE_STRING:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS,
					this->GetStringData().length() >= X.GetStringData().length() ) );
		case VARTYPE_BOOL:
		default:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS,
					this->GetBoolData() >= X.GetBoolData() ) );
			break;
		}
	}
	//Catch failed string conversions
	catch( ParserAnomaly E )
	{
		if( E.ErrorCode == ANOMALY_NOCONVERSION ){
			return VariableBasePtr( CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, false ) );
		}
		else throw;
	}
	
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr Variable::operator<=( const VariableBase& X ) const
{
	VarType ContextType = mTypeConversionTable[this->mCurrentType][X.GetVariableType()];

	try{
		switch( ContextType )
		{
		case VARTYPE_NUM:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS,
					mpfr_lessequal_p( GetNumData().get(), X.GetNumData().get() ) ) );

		case VARTYPE_STRING:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS,
					this->GetStringData().length() <= X.GetStringData().length() ) );
		case VARTYPE_BOOL:
		default:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS,
					this->GetBoolData() <= X.GetBoolData() ) );
			break;
		}
	}
	//Catch failed string conversions
	catch( ParserAnomaly E )
	{
		if( E.ErrorCode == ANOMALY_NOCONVERSION ){
			return VariableBasePtr( CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, false ) );
		}
		else throw;
	}

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr Variable::operator>( const VariableBase& X ) const
{
	VarType ContextType = mTypeConversionTable[this->mCurrentType][X.GetVariableType()];

	try{
		switch( ContextType )
		{
		case VARTYPE_NUM:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS,
					mpfr_greater_p( GetNumData().get(), X.GetNumData().get() ) ) );

		case VARTYPE_STRING:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS,
					this->GetStringData().length() > X.GetStringData().length() ) );
		case VARTYPE_BOOL:
		default:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS,
					this->GetBoolData() > X.GetBoolData() ) );
			break;
		}
	}
	//Catch failed string conversions
	catch( ParserAnomaly E )
	{
		if( E.ErrorCode == ANOMALY_NOCONVERSION ){
			return VariableBasePtr( CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, false ) );
		}
		else throw;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr Variable::operator<( const VariableBase& X ) const
{
	VarType ContextType = mTypeConversionTable[this->mCurrentType][X.GetVariableType()];

	try{
		switch( ContextType )
		{
		case VARTYPE_NUM:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS,
					mpfr_less_p( GetNumData().get(), X.GetNumData().get() ) ) );

		case VARTYPE_STRING:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS,
					this->GetStringData().length() <
				X.GetStringData().length() ) );
		case VARTYPE_BOOL:
		default:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS,
					this->GetBoolData() < X.GetBoolData() ) );
			break;
		}
	}
	//Catch failed string conversions
	catch( ParserAnomaly E )
	{
		if( E.ErrorCode == ANOMALY_NOCONVERSION ){
			return VariableBasePtr(	CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, false ) );
		}
		else throw;
	}	
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr Variable::operator&&( const VariableBase& X ) const
{
	return VariableBasePtr( 
		CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, this->GetBoolData() && X.GetBoolData() ) );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr Variable::operator||( const VariableBase& X ) const
{
	return VariableBasePtr( 
		CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, this->GetBoolData() || X.GetBoolData() ) );
}
	


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr Variable::op_not() const
{
	return VariableBasePtr( CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, !(this->GetBoolData()) ) );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr Variable::op_neg() const
{
	switch( mCurrentType )
	{
	case VARTYPE_BOOL:
		return this->op_not();
		
	case VARTYPE_STRING:
	case VARTYPE_NUM:
	default:
	{
		VariablePtr tmp = CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, false );
		mpfr_neg( tmp->mNumPart.get(), this->GetNumData().get(), LangOpts::Instance().RoundingMode );
		tmp->mCurrentType = VARTYPE_NUM;
		
		return tmp;
	}
	}
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Variable::ForceConversion( VarType Type )
{
	//I'm not going to bother checking if it already is of that type.
	//I don't think this function is likely to get overcalled.

	switch( Type )
	{
	case VARTYPE_NUM:
		GetNumData();
		mCurrentType = VARTYPE_NUM;
		break;
	case VARTYPE_BOOL:
		GetBoolData();
		mCurrentType = VARTYPE_BOOL;
		break;
	case VARTYPE_STRING:
		GetStringData();
		mCurrentType = VARTYPE_STRING;
		break;
	}
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
NumType Variable::GetNumData() const
{
	if( ! mpfr_nan_p(mNumPart.get()) ) return mNumPart;

	if( mCurrentType == VARTYPE_BOOL )
	{
		if( mBoolPart ) mNumPart.set( 1 ); //I'm not sure if this a good idea, or what.
		else mNumPart = gpNANConst->mNumPart;
	}
	else if( mCurrentType == VARTYPE_STRING )
	{
		StringType2NumType( mStringPart, mNumPart );
	}

	return mNumPart;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BoolType Variable::GetBoolData() const
{
	if( mBoolPart != false ) return mBoolPart;


	if( mCurrentType == VARTYPE_NUM )
	{
		if( mpfr_nan_p( mNumPart.get() ) ) mBoolPart = false;
		else                              mBoolPart = true;
	}
	else if( mCurrentType == VARTYPE_STRING )
	{
		mBoolPart = mStringPart.empty() ? false : true;
	}

	return mBoolPart;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
StringType Variable::GetStringData() const
{
	if( !mStringPart.empty() ) return mStringPart;

	if( mCurrentType == VARTYPE_NUM )
	{
		NumType2StringType( mNumPart, mStringPart );
	}
	else if( mCurrentType == VARTYPE_BOOL )
	{
		mStringPart = mBoolPart ? TXT("true") : TXT("false");
	}

	return mStringPart;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
NumType& Variable::GetActualNumData()
{
	GetNumData(); //To make sure it has been converted.
	return mNumPart;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BoolType& Variable::GetActualBoolData()
{
	GetBoolData();
	return mBoolPart;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
StringType& Variable::GetActualStringData()
{
	GetStringData();
	return mStringPart;
}
	

