/*
 Variable.cpp
 ----------------------
 
 Created: 19:9:2004   19:37
 */
 

#include "Variable.hpp"
#include <boost/lexical_cast.hpp>
#include "ParserAnomaly.hpp"
#include "LanguageConstants.hpp"
#include "SpecialVars.hpp"
//#include "BaseFuncs.hpp"
#include "List.hpp"
#include "CreationFuncs.hpp"

using namespace SS;


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 NumType2StringType
 NOTES: Converts a GMP floating point number to a string.  I don't want to
		take the trouble to make my only class when this is the only thing
		that needs changing.
*/
StringType SS::NumType2StringType( const NumType& Foo ){
	
	//Special case because zeros don't seem to want to work.
	//if( Foo == 0 ) return StringType(TXT("0"));

	mp_exp_t Exponent = 0;
	SS::STRING FooStr = NormalizeString( Foo.get_str( &Exponent, gNumberBase, gMaxDigitOutput ) );

	if( Exponent < 0 )
	{
		STRING Prefix = TXT("0.");
		for( ; Exponent < 0; Exponent++ ) Prefix += TXT("0");

        if( FooStr[0] == '-' )
		{
			FooStr.insert( 1, Prefix );
		}
		else
		{
            FooStr.insert( 0, Prefix );
		}
	}
	else if( Exponent < (long)FooStr.length() - 1 && FooStr[0] == '-' ){
		FooStr.insert( FooStr.begin() + Exponent + 1, LC_DecimalPoint[0] );
	}
	else
	if( Exponent < (long)FooStr.length() ){
		FooStr.insert( FooStr.begin() + Exponent, LC_DecimalPoint[0] );
	}

	//MPFR seems to want full precision, including all trailing zeros.
	//I don't want that.
	int i;
	for( i = (int)FooStr.length()-1; i >= 0; i-- )
	{
		if( FooStr[i] == '0' ) FooStr.erase( FooStr.begin() + i );
		else break;
	}

	if( FooStr[ FooStr.length()-1 ] == LC_DecimalPoint[0] ) FooStr.erase( FooStr.length() - 1 );

	return FooStr;
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
BlockListPointer VariableBase::GetBlockListPtr(){
	SS::STRING tmp = TXT("Cannot convert the object \"");
	tmp += mName;
	tmp += TXT("\" from a VariableBase to a BlockList.");
	throw ParserAnomaly( tmp, ANOMALY_NOCONVERSION, FILENAME, LINE, FUNC );
}

const BlockListPointer VariableBase::GetBlockListPtr() const{
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
VariableBasePointer VariableBase::GetVariableBasePtr()
{
	return boost::dynamic_pointer_cast<VariableBase>( ScopeObjectPointer( mpThis ) );
}

const VariableBasePointer VariableBase::GetVariableBasePtr() const
{
	return boost::dynamic_pointer_cast<VariableBase>( ScopeObjectPointer( mpThis ) );
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
ListPointer VariableBase::GetListPtr()
{
	ListPointer pNewList( CreateObject<List>( GetName(), false, false ) );
	pNewList->AppendWithoutCopy( GetVariablePtr() );

	return pNewList;
}


const ListPointer VariableBase::GetListPtr() const
{
	ListPointer pNewList( CreateObject<List>( GetName(), false, false ) );
	pNewList->AppendWithoutCopy( GetVariablePtr() );

	return pNewList;
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 VariableBase::Get____Data
 NOTES: The default implementation.  GetStringData will return its name.
		Everything else will just return 0/0.0/false
*/
NumType VariableBase::GetNumData() const{
	return NumType(0);
}

BoolType VariableBase::GetBoolData() const{
	return false;
}

StringType VariableBase::GetStringData() const{
	//return this->GetFullName();
	return TXT("");
}


NumType& VariableBase::GetNumData( NumType& Out ) const{
	Out = GetNumData();
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
VariableBasePointer VariableBase::UndefinedOp( const SS::STRING& Op) const
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
	return VariableBasePointer();
}




VariableBasePointer VariableBase::operator+(const VariableBase&) const{
    return UndefinedOp( TXT("+") );
}

VariableBasePointer VariableBase::operator-(const VariableBase&) const{
	return UndefinedOp( TXT("-") );
}

VariableBasePointer VariableBase::operator*(const VariableBase&) const{
	return UndefinedOp( TXT("*") );
}

VariableBasePointer VariableBase::operator/(const VariableBase&) const{
	return UndefinedOp( TXT("/") );
}

VariableBasePointer VariableBase::operator_pow( const VariableBase& ) const{
	return UndefinedOp( TXT("**") );
}

VariableBasePointer VariableBase::operator=(const VariableBase&){
	return UndefinedOp( TXT("=") );
}


VariableBasePointer VariableBase::operator==(const VariableBase&) const{
	return UndefinedOp( TXT("==") );
}

VariableBasePointer VariableBase::operator!=(const VariableBase&) const{
	return UndefinedOp( TXT("!=") );
}

VariableBasePointer VariableBase::operator>=(const VariableBase&) const{
	return UndefinedOp( TXT(">=") );
}

VariableBasePointer VariableBase::operator<=(const VariableBase&) const{
	return UndefinedOp( TXT("<=") );
}

VariableBasePointer VariableBase::operator>(const VariableBase&) const{
	return UndefinedOp( TXT(">") );
}

VariableBasePointer VariableBase::operator<(const VariableBase&) const{
	return UndefinedOp( TXT("<") );
}

VariableBasePointer VariableBase::operator&&(const VariableBase&) const{
	return UndefinedOp( TXT("&&") );
}

VariableBasePointer VariableBase::operator||(const VariableBase&) const{
	return UndefinedOp( TXT("||") );
}


//Unary ops
VariableBasePointer VariableBase::op_not() const{
	return UndefinedOp( TXT("unary not") );
}

VariableBasePointer VariableBase::op_neg() const{
	return UndefinedOp( TXT("unary -") );
}





/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Variable::Variable
 NOTES: 
*/
Variable::Variable( const SS::STRING& Name,
					bool Static, bool Const,
					const Variable& X )
	: mCurrentType( X.mCurrentType ),
	  mNumPart( X.mNumPart ),
	  mStringPart( X.mStringPart ),
	  mBoolPart( X.mBoolPart ),
	  VariableBase( Name, Static, Const )
{}

Variable::Variable()
: mCurrentType( DEFAULT_VARTYPE ),
  mNumPart(0, gDefaultPrecis)
{
	RegisterPredefinedVars();
}

Variable::Variable( const STRING& Name,
				    bool Static, bool Const,
					const NumType& X )
: VariableBase(Name, Static, Const),
  mCurrentType( VARTYPE_NUM ),
  mNumPart( X ),
  mBoolPart(false)
{
	RegisterPredefinedVars();
}

Variable::Variable( const STRING& Name,
				    bool Static, bool Const,
					const StringType& X )
: VariableBase(Name, Static, Const),
  mCurrentType( VARTYPE_STRING ),
  mStringPart( X ),
  mBoolPart(false),
  mNumPart(0, gDefaultPrecis)
{
	RegisterPredefinedVars();
}

Variable::Variable( const STRING& Name,
				    bool Static, bool Const,
					const BoolType& X )
: VariableBase(Name, Static, Const),
  mCurrentType( VARTYPE_BOOL ),
  mBoolPart( X ),
  mNumPart(0, gDefaultPrecis)
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
	bool WasConst = IsConst();
	SetConst( false );

	//Every PrecisionVar has a UniqueID,
	//Every UniqueID has a PrecisionVar.
	//This is to break that cycle.  UniqueIDs dont have PrecisionVars.
	if( mName != LC_UniqueID ){
		Register( ScopeObjectPointer( new PrecisionVar( LC_Precision, *this, true ) ) );
	}

	SetConst( WasConst );
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
 Variable::GetVariablePtr
 NOTES: Overloaded to return itself rather than create a new variable.
*/
VariablePointer Variable::GetVariablePtr()
{
	ScopeObjectPointer pTemp( mpThis );
	return boost::dynamic_pointer_cast<Variable>( pTemp );
}

const VariablePointer Variable::GetVariablePtr() const
{
	const ScopeObjectPointer pTemp( mpThis );
	const VariablePointer pReturn = boost::dynamic_pointer_cast<Variable>( pTemp );
	return pReturn;
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
VariableBasePointer Variable::operator+(const VariableBase& X) const
{
	VarType ContextType = mTypeConversionTable[this->mCurrentType][X.GetVariableType()];

	try{
		switch( ContextType )
		{
		case VARTYPE_NUM:
			return VariableBasePointer(
				CreateVariable( SS_BASE_ARGS_DEFAULTS, this->GetNumData() + X.GetNumData() ) );
		case VARTYPE_BOOL:
			return VariableBasePointer(
				CreateVariable( SS_BASE_ARGS_DEFAULTS, this->GetBoolData() || X.GetBoolData() ) );
		case VARTYPE_STRING:
		default:
			return VariableBasePointer(
				CreateVariable( SS_BASE_ARGS_DEFAULTS, this->GetStringData() + X.GetStringData() ) );
		}
	}
	catch( ParserAnomaly E )
	{
		if( E.ErrorCode == ANOMALY_NOCONVERSION )		{
			return VariableBasePointer(
				CreateVariable( SS_BASE_ARGS_DEFAULTS, this->GetStringData() + X.GetStringData() ) );
		}
		else throw;
	}
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Variable::operator-
 NOTES: Magical subtraction operator.
*/
VariableBasePointer Variable::operator-(const VariableBase& X) const
{
	VarType ContextType = mTypeConversionTable[this->mCurrentType][X.GetVariableType()];

	try{
		switch( ContextType )
		{
		case VARTYPE_NUM:
			return VariableBasePointer(
				CreateVariable( SS_BASE_ARGS_DEFAULTS, this->GetNumData() - X.GetNumData() ) );
			break;
		case VARTYPE_BOOL:
			//TODO: Maybe think of something better than using && for boolean subtraction
			return VariableBasePointer(
				CreateVariable( SS_BASE_ARGS_DEFAULTS, this->GetBoolData() && X.GetBoolData() ) );
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
VariableBasePointer Variable::operator*(const VariableBase& X) const
{
	VarType ContextType = mTypeConversionTable[this->mCurrentType][X.GetVariableType()];

	//SPECIAL CASE: Strings multiplied by BigNums result in repeated strings.
	if( this->mCurrentType == VARTYPE_NUM && X.GetVariablePtr()->mCurrentType == VARTYPE_STRING )
	{
		StringType TempString;
		NumType i;
		for( i = 0; i < this->GetNumData(); i++ ){
			TempString += X.GetStringData();            
		}

		return VariableBasePointer(	CreateVariable( SS_BASE_ARGS_DEFAULTS, TempString ) );
	}
	else
	if( this->mCurrentType == VARTYPE_STRING && X.GetVariablePtr()->mCurrentType == VARTYPE_NUM ) 
	{
		StringType TempString;
		NumType i;
		for( i = 0; i < X.GetNumData(); i++ ){
			TempString += this->GetStringData();
		}   

		return VariableBasePointer( CreateVariable( SS_BASE_ARGS_DEFAULTS, TempString ) );
	}


	try{
		switch( ContextType )
		{
		case VARTYPE_NUM:
			return VariableBasePointer(
				CreateVariable( SS_BASE_ARGS_DEFAULTS, this->GetNumData() * X.GetNumData() ) );
			break;
		case VARTYPE_BOOL:
			return VariableBasePointer(
				CreateVariable( SS_BASE_ARGS_DEFAULTS, this->GetBoolData() || X.GetBoolData() ) );
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
VariableBasePointer Variable::operator_pow( const VariableBase& X ) const
{
	NumType NewN;

	mpfr_pow( NewN.get_mpfr_t(), this->GetNumData().get_mpfr_t(),
			  X.GetNumData().get_mpfr_t(), GMP_RNDN );

	return VariableBasePointer( CreateVariable( SS_BASE_ARGS_DEFAULTS, NewN ) );
}




/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Variable::operator/
 NOTES: ...
*/
VariableBasePointer Variable::operator/(const VariableBase& X) const
{
	VarType ContextType = mTypeConversionTable[this->mCurrentType][X.GetVariableType()];

	try{
		switch( ContextType )
		{
		case VARTYPE_NUM:
			return VariableBasePointer(
				CreateVariable( SS_BASE_ARGS_DEFAULTS, this->GetNumData() / X.GetNumData() ) );
			break;
		case VARTYPE_BOOL:
			//TODO: This is illogical (says Spock).
			return VariableBasePointer(
				CreateVariable( SS_BASE_ARGS_DEFAULTS, this->GetBoolData() && X.GetBoolData() ) );
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
VariableBasePointer Variable::operator=( const VariableBase& X )
{
	//Zero everything first.  Remember that if it is not zeroed it will
	//be assumed that the value held is the correct one.
	this->mStringPart.clear();
	this->mNumPart = 0;
	this->mBoolPart = false;

	switch( X.GetVariableType() ){
		case VARTYPE_STRING:
			this->mStringPart = X.GetStringData();
			this->mCurrentType = VARTYPE_STRING;
			break;
		case VARTYPE_NUM:
			this->mNumPart = X.GetNumData();
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
	//return VariableBasePointer( this, null_deleter() );
	return VariableBasePointer( 
		boost::dynamic_pointer_cast<VariableBase>(
			ScopeObjectPointer(mpThis) ) 
		);
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Variable::operator==
 NOTES: The equality operator.
*/
VariableBasePointer Variable::operator==( const VariableBase& X ) const
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
			return VariableBasePointer(
				CreateVariable( SS_BASE_ARGS_DEFAULTS, this->GetNumData() == X.GetNumData() ) );
			break;
		case VARTYPE_STRING:
			return VariableBasePointer(
				CreateVariable( SS_BASE_ARGS_DEFAULTS, this->GetStringData() == X.GetStringData() ) );
		case VARTYPE_BOOL:
		default:
			return VariableBasePointer(
				CreateVariable( SS_BASE_ARGS_DEFAULTS, this->GetBoolData() == X.GetBoolData() ) );
			break;
		}
	}
	//Catch failed string conversions
	catch( ParserAnomaly E )
	{
		if( E.ErrorCode == ANOMALY_NOCONVERSION ){
			return VariableBasePointer(	CreateVariable( SS_BASE_ARGS_DEFAULTS, false ) );
		}
		else throw;
	}

}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Variable::operator==
NOTES: The equality operator.
*/
VariableBasePointer Variable::operator!=( const VariableBase& X ) const
{
	VarType ContextType = mTypeConversionTable[this->mCurrentType][X.GetVariableType()];

	try{
		switch( ContextType )
		{
		case VARTYPE_NUM:
			return VariableBasePointer(
				CreateVariable( SS_BASE_ARGS_DEFAULTS, this->GetNumData() != X.GetNumData() ) );
			break;
		case VARTYPE_STRING:
			return VariableBasePointer(
				CreateVariable( SS_BASE_ARGS_DEFAULTS, this->GetStringData() != X.GetStringData() ) );
		case VARTYPE_BOOL:
		default:
			return VariableBasePointer(
				CreateVariable( SS_BASE_ARGS_DEFAULTS, this->GetBoolData() != X.GetBoolData() ) );
			break;
		}
	}
	//Catch failed string conversions
	catch( ParserAnomaly E )
	{
		if( E.ErrorCode == ANOMALY_NOCONVERSION ){
			return VariableBasePointer(	CreateVariable( SS_BASE_ARGS_DEFAULTS, false ) );
		}
		else throw;
	}

}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Variable::operator>=
 NOTES: blah.
*/
VariableBasePointer Variable::operator>=( const VariableBase& X ) const
{
	VarType ContextType = mTypeConversionTable[this->mCurrentType][X.GetVariableType()];

	try{
		switch( ContextType )
		{
		case VARTYPE_NUM:
			return VariableBasePointer(
				CreateVariable( SS_BASE_ARGS_DEFAULTS, this->GetNumData() >= X.GetNumData() ) );
			break;
		case VARTYPE_STRING:
			return VariableBasePointer(
				CreateVariable( SS_BASE_ARGS_DEFAULTS, this->GetStringData().length() >=
										X.GetStringData().length() ) );
		case VARTYPE_BOOL:
		default:
			return VariableBasePointer(
				CreateVariable( SS_BASE_ARGS_DEFAULTS, this->GetBoolData() >= X.GetBoolData() ) );
			break;
		}
	}
	//Catch failed string conversions
	catch( ParserAnomaly E )
	{
		if( E.ErrorCode == ANOMALY_NOCONVERSION ){
			return VariableBasePointer( CreateVariable( SS_BASE_ARGS_DEFAULTS, false ) );
		}
		else throw;
	}
	
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Variable::operator<=
 NOTES: blah, vol 2.
*/
VariableBasePointer Variable::operator<=( const VariableBase& X ) const
{
	VarType ContextType = mTypeConversionTable[this->mCurrentType][X.GetVariableType()];

	try{
		switch( ContextType )
		{
		case VARTYPE_NUM:
			return VariableBasePointer(
				CreateVariable( SS_BASE_ARGS_DEFAULTS, this->GetNumData() <= X.GetNumData() ) );
			break;
		case VARTYPE_STRING:
			return VariableBasePointer(
				CreateVariable( SS_BASE_ARGS_DEFAULTS, this->GetStringData().length() <=
				X.GetStringData().length() ) );
		case VARTYPE_BOOL:
		default:
			return VariableBasePointer(
				CreateVariable( SS_BASE_ARGS_DEFAULTS, this->GetBoolData() <= X.GetBoolData() ) );
			break;
		}
	}
	//Catch failed string conversions
	catch( ParserAnomaly E )
	{
		if( E.ErrorCode == ANOMALY_NOCONVERSION ){
			return VariableBasePointer( CreateVariable( SS_BASE_ARGS_DEFAULTS, false ) );
		}
		else throw;
	}

}

/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Variable::operator>
NOTES: blah, 3rd edition
*/
VariableBasePointer Variable::operator>( const VariableBase& X ) const
{
	VarType ContextType = mTypeConversionTable[this->mCurrentType][X.GetVariableType()];

	try{
		switch( ContextType )
		{
		case VARTYPE_NUM:
			return VariableBasePointer(
				CreateVariable( SS_BASE_ARGS_DEFAULTS, this->GetNumData() > X.GetNumData() ) );
			break;
		case VARTYPE_STRING:
			return VariableBasePointer(
				CreateVariable( SS_BASE_ARGS_DEFAULTS, this->GetStringData().length() >
				X.GetStringData().length() ) );
		case VARTYPE_BOOL:
		default:
			return VariableBasePointer(
				CreateVariable( SS_BASE_ARGS_DEFAULTS, this->GetBoolData() > X.GetBoolData() ) );
			break;
		}
	}
	//Catch failed string conversions
	catch( ParserAnomaly E )
	{
		if( E.ErrorCode == ANOMALY_NOCONVERSION ){
			return VariableBasePointer( CreateVariable( SS_BASE_ARGS_DEFAULTS, false ) );
		}
		else throw;
	}
}

/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Variable::operator<
NOTES: blah, 4th movement.
*/
VariableBasePointer Variable::operator<( const VariableBase& X ) const
{
	VarType ContextType = mTypeConversionTable[this->mCurrentType][X.GetVariableType()];

	try{
		switch( ContextType )
		{
		case VARTYPE_NUM:
			return VariableBasePointer(
				CreateVariable( SS_BASE_ARGS_DEFAULTS, this->GetNumData() < X.GetNumData() ) );
			break;
		case VARTYPE_STRING:
			return VariableBasePointer(
				CreateVariable( SS_BASE_ARGS_DEFAULTS, this->GetStringData().length() <
				X.GetStringData().length() ) );
		case VARTYPE_BOOL:
		default:
			return VariableBasePointer(
				CreateVariable( SS_BASE_ARGS_DEFAULTS, this->GetBoolData() < X.GetBoolData() ) );
			break;
		}
	}
	//Catch failed string conversions
	catch( ParserAnomaly E )
	{
		if( E.ErrorCode == ANOMALY_NOCONVERSION ){
			return VariableBasePointer(	CreateVariable( SS_BASE_ARGS_DEFAULTS, false ) );
		}
		else throw;
	}	
}

/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Variable::operator&&
 NOTES: Logical AND operator
*/
VariableBasePointer Variable::operator&&( const VariableBase& X ) const
{
	return VariableBasePointer( 
		CreateVariable( SS_BASE_ARGS_DEFAULTS, this->GetBoolData() && X.GetBoolData() ) );
}

/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Variable::operator||
 NOTES: Logical OR operator
*/
VariableBasePointer Variable::operator||( const VariableBase& X ) const
{
	return VariableBasePointer( 
		CreateVariable( SS_BASE_ARGS_DEFAULTS, this->GetBoolData() || X.GetBoolData() ) );
}
	


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Variable::op_not
 NOTES: Unary Logical-NOT operator
*/
VariableBasePointer Variable::op_not() const
{
	return VariableBasePointer( CreateVariable( SS_BASE_ARGS_DEFAULTS, !(this->GetBoolData()) ) );
}

/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Variable::op_neg
 NOTES: Unary negative operator
*/
VariableBasePointer Variable::op_neg() const
{
	switch( mCurrentType )
	{
	case VARTYPE_NUM:
		return VariableBasePointer(
						CreateVariable( SS_BASE_ARGS_DEFAULTS, this->GetNumData() * (NumType)-1 ) );
	
	
	
	case VARTYPE_BOOL:
		return this->op_not();
	
	case VARTYPE_STRING:
	default:
		return VariableBasePointer(
						CreateVariable( SS_BASE_ARGS_DEFAULTS, -(this->GetNumData()) ) );
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
	if( mNumPart != NumType(0) ) return mNumPart;

	switch( mCurrentType )
	{
	case VARTYPE_BOOL:
		mNumPart = mBoolPart ? 1 : 0;
		break;
	case VARTYPE_STRING:
		//This is a little expensive, but we need to make sure the string is indeed a number.
		STRING GoodString;
		unsigned int i;
		for( i = 0; i < mStringPart.length(); ++i )
		{
			if(  !(('0' <= mStringPart[i] && mStringPart[i] <= '9') || mStringPart[i] == LC_DecimalPoint[0] )  )
			{
				STRING tmp = TXT("Cannot convert string \'");
				tmp += mStringPart;
				tmp += TXT("\' to a number.");
                ThrowParserAnomaly( tmp, ANOMALY_NOCONVERSION );                
			}
		}

		//TODO: Calculate the actual precision required to hold that string.
		mNumPart.set_prec( gDefaultPrecis );
		mNumPart = NarrowizeString( mStringPart );
		
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

	switch( mCurrentType )
	{
	case VARTYPE_NUM:
		mBoolPart = mNumPart == NumType(0) ? false : true;
		break;
	case VARTYPE_STRING:
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

	switch( mCurrentType )
	{
	case VARTYPE_NUM:
		mStringPart = NumType2StringType( mNumPart );
		break;
	case VARTYPE_BOOL:
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
	

