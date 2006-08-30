/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: The variable interface and implementation.
*/
 

#include "Variable.hpp"
#include <boost/lexical_cast.hpp>
#include "ParserAnomaly.hpp"
#include "LanguageConstants.hpp"
#include "SpecialVars.hpp"
//#include "BaseFuncs.hpp"
#include "List.hpp"
#include "CreationFuncs.hpp"
#include <cstring>

//Just for a quick test.  Please remove this later.
#include <iostream>

using namespace SS;



//Defs for the mpfr_t mini-wrapper
mpfr_t_wrap::mpfr_t_wrap()
{
	mpfr_init(N);	
}

mpfr_t_wrap::mpfr_t_wrap( int prec )
{
	mpfr_init2( N, prec );
}

mpfr_t_wrap::mpfr_t_wrap( const mpfr_t_wrap& X )
{
	mpfr_init(N);
	mpfr_set( N, X.get(), LangOpts::Instance().RoundingMode );	
}

mpfr_t_wrap& mpfr_t_wrap::operator=( const mpfr_t_wrap& X )
{
	mpfr_set( N, X.get(), LangOpts::Instance().RoundingMode );
	return *this;
}

mpfr_t_wrap::~mpfr_t_wrap()
{
	mpfr_clear(N);	
}

mpfr_t& mpfr_t_wrap::get() const
{
	return N;	
}

mpfr_t_wrap& mpfr_t_wrap::set( int x )
{
	return set( (signed long)x );
}

mpfr_t_wrap& mpfr_t_wrap::set( signed long x )
{
	mpfr_set_si( N, x, LangOpts::Instance().RoundingMode );
	return *this;
}

mpfr_t_wrap& mpfr_t_wrap::set( unsigned long x )
{
	mpfr_set_ui( N, x, LangOpts::Instance().RoundingMode );
	return *this;	
}

mpfr_t_wrap& mpfr_t_wrap::set( double x )
{
	mpfr_set_d( N, x, LangOpts::Instance().RoundingMode );
	return *this;	
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 NumType2StringType
 NOTES: Converts a GMP floating point number to a string.  I don't want to
		take the trouble to make my only class when this is the only thing
		that needs changing.
*/
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

StringType SS::NumType2StringType( const NumType& In )
{
	StringType Out;
	NumType2StringType( In, Out );
	return Out;
}


void SS::StringType2NumType( const StringType& In, NumType& Out )
{
	//We are trusting in mpfr to do The Right Thing.
	//We may want to check for a -1 (error) return value in the future.
	mpfr_set_str( Out.get(),
	              NarrowizeString(In).c_str(),
	              LangOpts::Instance().NumberBase,
	              LangOpts::Instance().RoundingMode );
}

NumType SS::StringType2NumType( const StringType& In )
{
	NumType Out;
	StringType2NumType( In, Out );
	return Out;
}
	

const VarType DEFAULT_VARTYPE = VARTYPE_BOOL;



/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 VariableBase::VariableBase
 NOTES: constructors
*/
VariableBase::VariableBase()
{
}


VariableBase::VariableBase( const SS::STRING& Name,
						    bool Static /*= false*/, bool Const /*= false*/ )
: Scope( Name, Static, Const )
{
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 VariableBase::~VariableBase
 NOTES: Doesn't do anything
*/
VariableBase::~VariableBase()
{
}





/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 VariableBase::GetBlockListPtr
 NOTES: Throw an anomaly
*/
/*
BlockListPtr VariableBase::GetBlockListPtr(){
	SS::STRING tmp = TXT("Cannot convert the object \"");
	tmp += mName;
	tmp += TXT("\" from a VariableBase to a BlockList.");
	throw ParserAnomaly( tmp, ANOMALY_NOCONVERSION, FILENAME, LINE, FUNC );
}

const BlockListPtr VariableBase::GetBlockListPtr() const{
	SS::STRING tmp = TXT("Cannot convert the object \"");
	tmp += mName;
	tmp += TXT("\" from a VariableBase to a BlockList.");
	throw ParserAnomaly( tmp, ANOMALY_NOCONVERSION, FILENAME, LINE, FUNC );
}
*/




/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
VariableBase::GetVariableBasePtr
NOTES: Overloaded to return itself rather than its name as other ScopeObjects do.
*/
VariableBasePtr VariableBase::CastToVariableBase(){
	AssertCastingAllowed();
	return boost::dynamic_pointer_cast<VariableBase>( ScopeObjectPtr( mpThis ) );
}

const VariableBasePtr VariableBase::CastToVariableBase() const{
	AssertCastingAllowed();
	return boost::dynamic_pointer_cast<VariableBase>( ScopeObjectPtr( mpThis ) );
}



/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 VariableBase::AcceptVisitor
 NOTES: ...
*/
void VariableBase::AcceptVisitor( ScopeObjectVisitor& V ){
	V.VisitVariableBase( this );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 VariableBase::GetVariableType
 NOTES: This is the implementation for non-vars.  It should return the
		prefered type.  Since non-vars just return their names, the prefered
		type is a string.
*/
VarType VariableBase::GetVariableType() const{
	return VARTYPE_STRING;
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
VariableBase::GetListPtr
NOTES: Converts the variable to a list holding itself.
*/
ListPtr VariableBase::CastToList()
{
	AssertCastingAllowed();
	ListPtr pNewList = CreateGeneric<List>( GetName(), false, false );
	pNewList->PushWithoutCopy( this->CastToVariable() );

	return pNewList;
}

const ListPtr VariableBase::CastToList() const
{
	AssertCastingAllowed();
	ListPtr pNewList = CreateGeneric<List>( GetName(), false, false );
	pNewList->PushWithoutCopy( this->CastToVariable() );

	return pNewList;
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 VariableBase::Get____Data
 NOTES: The default implementation.  GetStringData will return its name.
		Everything else will just return 0/0.0/false
*/
NumType VariableBase::GetNumData() const{
	NumType N;
	return N;
}

BoolType VariableBase::GetBoolData() const{
	return false;
}

StringType VariableBase::GetStringData() const{
	//return this->GetFullName();
	return TXT("");
}


NumType& VariableBase::GetNumData( NumType& Out ) const{
	mpfr_set( Out.get(), GetNumData().get(), LangOpts::Instance().RoundingMode );
	return Out;
}

BoolType& VariableBase::GetBoolData( BoolType& Out ) const{
	Out = GetBoolData();
	return Out;
}

StringType& VariableBase::GetStringData( StringType& Out ) const{
	Out = GetStringData();
	return Out;
}

CHAR* VariableBase::GetStringData( CHAR* Buffer, unsigned int BufferSize ) const
{
	SS::STRCPY( Buffer, GetStringData().c_str(), BufferSize );
	return Buffer;
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 VariableBase::UndefinedOp
 NOTES: All the errors messages thrown by the undefined operators are 
		basically the same.  So I just put it under one function.

		Just pass what operator is throwing.
*/
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




VariableBasePtr VariableBase::operator+(const VariableBase&) const{
    return UndefinedOp( TXT("Addition") );
}

VariableBasePtr VariableBase::operator-(const VariableBase&) const{
	return UndefinedOp( TXT("Subtraction") );
}

VariableBasePtr VariableBase::operator*(const VariableBase&) const{
	return UndefinedOp( TXT("Multiplication") );
}

VariableBasePtr VariableBase::operator/(const VariableBase&) const{
	return UndefinedOp( TXT("Division") );
}

VariableBasePtr VariableBase::operator_pow( const VariableBase& ) const{
	return UndefinedOp( TXT("Exponentation") );
}

VariableBasePtr VariableBase::operator_concat( const VariableBase& ) const{
	return UndefinedOp( TXT("Concatenation") );
}

VariableBasePtr VariableBase::operator=(const VariableBase&){
	return UndefinedOp( TXT("Assignment") );
}


VariableBasePtr VariableBase::operator==(const VariableBase&) const{
	return UndefinedOp( TXT("Equal") );
}

VariableBasePtr VariableBase::operator!=(const VariableBase&) const{
	return UndefinedOp( TXT("Not-Equal") );
}

VariableBasePtr VariableBase::operator>=(const VariableBase&) const{
	return UndefinedOp( TXT("Greater-Than-Or-Equal") );
}

VariableBasePtr VariableBase::operator<=(const VariableBase&) const{
	return UndefinedOp( TXT("Less-Than-Or-Equal") );
}

VariableBasePtr VariableBase::operator>(const VariableBase&) const{
	return UndefinedOp( TXT("Greater-Than") );
}

VariableBasePtr VariableBase::operator<(const VariableBase&) const{
	return UndefinedOp( TXT("Less-Than") );
}

VariableBasePtr VariableBase::operator&&(const VariableBase&) const{
	return UndefinedOp( TXT("Logical-And") );
}

VariableBasePtr VariableBase::operator||(const VariableBase&) const{
	return UndefinedOp( TXT("Logical-Or") );
}


//Unary ops
VariableBasePtr VariableBase::op_not() const{
	return UndefinedOp( TXT("Unary-Not") );
}

VariableBasePtr VariableBase::op_neg() const{
	return UndefinedOp( TXT("Negation") );
}





/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Variable::Variable
 NOTES: 
*/
Variable::Variable( const SS::STRING& Name,
					bool Static, bool Const,
					const Variable& X )
	: VariableBase( Name, Static, Const ), 
	  mCurrentType( X.mCurrentType ),
	  mBoolPart( X.mBoolPart ),
	  mStringPart( X.mStringPart )
	  
{
	mpfr_set( mNumPart.get(), X.mNumPart.get(), LangOpts::Instance().RoundingMode );
}

Variable::Variable()
: mCurrentType( DEFAULT_VARTYPE ),
  mNumPart( LangOpts::Instance().DefaultPrecision )
{
	RegisterPredefinedVars();
}

Variable::Variable( const STRING& Name,
				    bool Static, bool Const,
					const NumType& X )
: VariableBase(Name, Static, Const),
  mCurrentType( VARTYPE_NUM ),
  mBoolPart(false)
{
	mpfr_set( mNumPart.get(), X.get(), LangOpts::Instance().RoundingMode );
	RegisterPredefinedVars();
}

Variable::Variable( const STRING& Name,
				    bool Static, bool Const,
					const StringType& X )
: VariableBase(Name, Static, Const),
  mCurrentType( VARTYPE_STRING ),
  mNumPart( LangOpts::Instance().DefaultPrecision ),
  mBoolPart(false),
  mStringPart( X )
{
	RegisterPredefinedVars();
}

Variable::Variable( const STRING& Name,
				    bool Static, bool Const,
					const BoolType& X )
: VariableBase(Name, Static, Const),
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


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Variable::RegisterPredefinedVars
 NOTES: Registers any special vars needed.
*/
void Variable::RegisterPredefinedVars()
{
	mPrecisionVarCreated = false;
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Hook to create precision magicvar.
*/
ScopeObjectPtr Variable::GetScopeObjectHook( const STRING& Name )
{
	if( !mPrecisionVarCreated && Name == LC_Precision ){
		mPrecisionVarCreated = true;
		return Register( ScopeObjectPtr( new PrecisionVar( LC_Precision, *this, true ) ) );
	}
	else return VariableBase::GetScopeObjectHook( Name );
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Variable::AcceptVisitor
 NOTES: Accepts the ScopeObjectVisitors.  Part of the visitor design pattern.
*/
void Variable::AcceptVisitor( ScopeObjectVisitor& V )
{
	V.VisitVariable(this);
}



/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 NOTES: Overloaded to return itself rather than create a new variable.
*/
VariablePtr Variable::CastToVariable(){
	AssertCastingAllowed();
	return boost::dynamic_pointer_cast<Variable>( ScopeObjectPtr(mpThis) );
}

const VariablePtr Variable::CastToVariable() const{
	AssertCastingAllowed();
	return boost::dynamic_pointer_cast<Variable>( ScopeObjectPtr(mpThis) );
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 GetVariableType
 NOTES: Returns the prefered type of the variable.  Inherited from Variable Base.
*/
VarType Variable::GetVariableType() const{
	return mCurrentType;
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Variable::operator+
 NOTES: Magical addition operator

*/
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


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Variable::operator-
 NOTES: Magical subtraction operator.
*/
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



/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Variable::operator*
 NOTES: Magically delicious multiplication operator.
*/
VariableBasePtr Variable::operator*(const VariableBase& X) const
{
	VarType ContextType = mTypeConversionTable[this->mCurrentType][X.GetVariableType()];

	//SPECIAL CASE: Strings multiplied by BigNums result in repeated strings.
	/*
	if( this->mCurrentType == VARTYPE_NUM && X.CastToVariable()->mCurrentType == VARTYPE_STRING )
	{
		StringType TempString;
		NumType i;
		for( i = 0; i < this->GetNumData(); i++ ){
			TempString += X.GetStringData();            
		}

		return VariableBasePtr(	CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, TempString ) );
	}
	else
	if( this->mCurrentType == VARTYPE_STRING && X.CastToVariable()->mCurrentType == VARTYPE_NUM ) 
	{
		StringType TempString;
		NumType i;
		for( i = 0; i < X.GetNumData(); i++ ){
			TempString += this->GetStringData();
		}   

		return VariableBasePtr( CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, TempString ) );
	}
	*/


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





/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Variable::operator_pow
 NOTES: Exponent operator
*/
VariableBasePtr Variable::operator_pow( const VariableBase& X ) const
{
	VariablePtr tmp = CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, false );
	mpfr_pow( tmp->mNumPart.get(), GetNumData().get(), X.GetNumData().get(), LangOpts::Instance().RoundingMode );
	tmp->mCurrentType = VARTYPE_NUM;
	
	return tmp;
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Concatenation operator
*/
VariableBasePtr Variable::operator_concat( const VariableBase& X ) const
{
	return VariableBasePtr( CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, 
								this->GetStringData() + X.GetStringData() ) );
	
}



/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Variable::operator/
 NOTES: ...
*/
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

/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Variable::operator=
 NOTES: The assignments operator.  
*/
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


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Variable::operator==
 NOTES: The equality operator.
*/
VariableBasePtr Variable::operator==( const VariableBase& X ) const
{
	VarType ContextType = mTypeConversionTable[this->mCurrentType][X.GetVariableType()];

	//SPECIAL CASE: Strings compared with integers, compares the size of the string.
	/*
	if( this->mCurrentType == VARTYPE_STRING ||
		    X.mCurrentType == VARTYPE_NUM )
	{
		return Variable( TXT(""), (NumType)(this->GetStringData().length()) == X.GetNumData() );
	}
	else if( this->mCurrentType == VARTYPE_NUM &&
				 X.mCurrentType == VARTYPE_STRING )
	{
		return Variable( TXT(""), (NumType)(X.GetStringData().length()) == this->GetNumData() );
	}
	*///NEGATIVE. I don't like this idea.

	try{
		switch( ContextType )
		{
		case VARTYPE_NUM:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, mpfr_equal_p( GetNumData().get(), X.GetNumData().get() ) ) );
				
		case VARTYPE_STRING:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, this->GetStringData() == X.GetStringData() ) );
		case VARTYPE_BOOL:
		default:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, this->GetBoolData() == X.GetBoolData() ) );
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


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Variable::operator==
NOTES: The equality operator.
*/
VariableBasePtr Variable::operator!=( const VariableBase& X ) const
{
	VarType ContextType = mTypeConversionTable[this->mCurrentType][X.GetVariableType()];

	try{
		switch( ContextType )
		{
		case VARTYPE_NUM:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, !mpfr_equal_p( GetNumData().get(), X.GetNumData().get() ) ) );
			break;
		case VARTYPE_STRING:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, this->GetStringData() != X.GetStringData() ) );
		case VARTYPE_BOOL:
		default:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, this->GetBoolData() != X.GetBoolData() ) );
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


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Variable::operator>=
 NOTES: blah.
*/
VariableBasePtr Variable::operator>=( const VariableBase& X ) const
{
	VarType ContextType = mTypeConversionTable[this->mCurrentType][X.GetVariableType()];

	try{
		switch( ContextType )
		{
		case VARTYPE_NUM:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, mpfr_greaterequal_p( GetNumData().get(), X.GetNumData().get() ) ) );

		case VARTYPE_STRING:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, this->GetStringData().length() >=
										X.GetStringData().length() ) );
		case VARTYPE_BOOL:
		default:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, this->GetBoolData() >= X.GetBoolData() ) );
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


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Variable::operator<=
 NOTES: blah, vol 2.
*/
VariableBasePtr Variable::operator<=( const VariableBase& X ) const
{
	VarType ContextType = mTypeConversionTable[this->mCurrentType][X.GetVariableType()];

	try{
		switch( ContextType )
		{
		case VARTYPE_NUM:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, mpfr_lessequal_p( GetNumData().get(), X.GetNumData().get() ) ) );

		case VARTYPE_STRING:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, this->GetStringData().length() <=
				X.GetStringData().length() ) );
		case VARTYPE_BOOL:
		default:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, this->GetBoolData() <= X.GetBoolData() ) );
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

/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Variable::operator>
NOTES: blah, 3rd edition
*/
VariableBasePtr Variable::operator>( const VariableBase& X ) const
{
	VarType ContextType = mTypeConversionTable[this->mCurrentType][X.GetVariableType()];

	try{
		switch( ContextType )
		{
		case VARTYPE_NUM:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, mpfr_greater_p( GetNumData().get(), X.GetNumData().get() ) ) );

		case VARTYPE_STRING:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, this->GetStringData().length() >
				X.GetStringData().length() ) );
		case VARTYPE_BOOL:
		default:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, this->GetBoolData() > X.GetBoolData() ) );
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

/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Variable::operator<
NOTES: blah, 4th movement.
*/
VariableBasePtr Variable::operator<( const VariableBase& X ) const
{
	VarType ContextType = mTypeConversionTable[this->mCurrentType][X.GetVariableType()];

	try{
		switch( ContextType )
		{
		case VARTYPE_NUM:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, mpfr_less_p( GetNumData().get(), X.GetNumData().get() ) ) );

		case VARTYPE_STRING:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, this->GetStringData().length() <
				X.GetStringData().length() ) );
		case VARTYPE_BOOL:
		default:
			return VariableBasePtr(
				CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, this->GetBoolData() < X.GetBoolData() ) );
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

/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Variable::operator&&
 NOTES: Logical AND operator
*/
VariableBasePtr Variable::operator&&( const VariableBase& X ) const
{
	return VariableBasePtr( 
		CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, this->GetBoolData() && X.GetBoolData() ) );
}

/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Variable::operator||
 NOTES: Logical OR operator
*/
VariableBasePtr Variable::operator||( const VariableBase& X ) const
{
	return VariableBasePtr( 
		CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, this->GetBoolData() || X.GetBoolData() ) );
}
	


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Variable::op_not
 NOTES: Unary Logical-NOT operator
*/
VariableBasePtr Variable::op_not() const
{
	return VariableBasePtr( CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, !(this->GetBoolData()) ) );
}

/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Variable::op_neg
 NOTES: Unary negative operator
*/
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


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Variable::ForceConversion( VarType )
 NOTES: 
*/
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



/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Variable::GetNumData
 NOTES: Returns a the value of the Variable in BigNum context
*/
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


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Variable::GetBoolData
 NOTES: Return the value of the Variable in bool context.
*/
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


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Variable::GetStringData
 NOTES: Return the value of the Variable in string context.
*/
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


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Variable::GetActualNumData
 NOTES: Returns a reference the actual data.
*/
NumType& Variable::GetActualNumData()
{
	GetNumData(); //To make sure it has been converted.
	return mNumPart;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Variable::GetActualBoolData
 NOTES: Returns a reference to the bool data.
*/
BoolType& Variable::GetActualBoolData()
{
	GetBoolData();
	return mBoolPart;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Variable::GetActualStringData
 NOTES: Returns a reference to the string data.
*/
StringType& Variable::GetActualStringData()
{
	GetStringData();
	return mStringPart;
}
	

