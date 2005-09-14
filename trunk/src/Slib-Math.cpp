
#include "Slib-Math.hpp"
#include "LanguageConstants.hpp"
#include "CreationFuncs.hpp"

//Used by mean
#include "List.hpp"

#include "mpfrxx.h"
#include "mpfr.h"

#include <boost/lexical_cast.hpp>


using namespace SS;
using namespace SS::SLib;




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~
// Math::Math
// NOTES: constructor
//
Math::Math()
	: Scope( LC_SL_Math, true )
{
	RegisterPredefined();
	SetConst();
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~
// Math::RegisterPredefined
// NOTES: registers all math specific vars/funcs to one scope.
//
void Math::RegisterPredefined()
{
	//Functions
	Register( ScopeObjectPointer( new sqrt( TXT("sqrt"), true, true ) ) );	
	Register( ScopeObjectPointer( new abs( TXT("abs"), true, true ) ) );
	Register( ScopeObjectPointer( new mean( TXT("mean"), true, true ) ) );
	Register( ScopeObjectPointer( new sin( TXT("sin"), true, true ) ) );
	Register( ScopeObjectPointer( new cos( TXT("cos"), true, true ) ) );
	Register( ScopeObjectPointer( new tan( TXT("tan"), true, true ) ) );
	Register( ScopeObjectPointer( new asin( TXT("asin"), true, true ) ) );
	Register( ScopeObjectPointer( new acos( TXT("acos"), true, true ) ) );
	Register( ScopeObjectPointer( new atan( TXT("atan"), true, true ) ) );
	Register( ScopeObjectPointer( new max( TXT("man"), true, true ) ) );
	Register( ScopeObjectPointer( new min( TXT("min"), true, true ) ) );
	Register( ScopeObjectPointer( new Int( TXT("int"), true, true ) ) );
	Register( ScopeObjectPointer( new Floor( TXT("floor"), true, true ) ) );
	
	//Magic Variables
	Register( ScopeObjectPointer( new Pi( TXT("pi"), true, true ) ) );
	Register( ScopeObjectPointer( new Euler( TXT("e"), true, true ) ) );
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~
// sqrt::Operate
// NOTES: Performes the square root operation.
//
VariableBasePointer SS::SLib::sqrt::Operate( VariableBasePointer X )
{
	NumType Tmp;
	mpfr_sqrt( Tmp.get_mpfr_t(), X->GetNumData().get_mpfr_t(), GMP_RNDN );
	
	return CreateVariable( SS_BASE_ARGS_DEFAULTS, Tmp );	
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 abs::Operate
 NOTES: The absolute value operator.
*/
VariableBasePointer SS::SLib::abs::Operate( VariableBasePointer X )
{
    NumType Tmp;
	mpfr_abs( Tmp.get_mpfr_t(), X->GetNumData().get_mpfr_t(), GMP_RNDN );

	return CreateVariable( SS_BASE_ARGS_DEFAULTS, Tmp );
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 mean::Operate
 NOTES: The mean functions.
*/
VariableBasePointer SS::SLib::mean::Operate( VariableBasePointer X )
{
    ListPointer pLst = X->GetListPtr();

	NumType Tmp = 0;
	unsigned int i;
	for( i = 0; i < pLst->GetInternalList().size(); i++ ){
		Tmp += (pLst->GetInternalList()[i])->GetNumData();
	}

	Tmp /= NumType( i );

	return CreateVariable( SS_BASE_ARGS_DEFAULTS, Tmp );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 sin::Operate
 NOTES: The sine function
*/
VariableBasePointer SS::SLib::sin::Operate( VariableBasePointer X )
{
	NumType Tmp;
	mpfr_sin( Tmp.get_mpfr_t(), X->GetNumData().get_mpfr_t(), GMP_RNDN );

	return CreateVariable( SS_BASE_ARGS_DEFAULTS, Tmp );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 cos::Operate
 NOTES: The cosine function.
*/
VariableBasePointer SS::SLib::cos::Operate( VariableBasePointer X )
{
	NumType Tmp;
	mpfr_cos( Tmp.get_mpfr_t(), X->GetNumData().get_mpfr_t(), GMP_RNDN );

	return CreateVariable( SS_BASE_ARGS_DEFAULTS, Tmp );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 tan::Operate
 NOTES: The tangent function.
*/
VariableBasePointer SS::SLib::tan::Operate( VariableBasePointer X )
{
	NumType Tmp;
	mpfr_tan( Tmp.get_mpfr_t(), X->GetNumData().get_mpfr_t(), GMP_RNDN );

	return CreateVariable( SS_BASE_ARGS_DEFAULTS, Tmp );
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
asin::Operate
NOTES: arc-sine
*/
VariableBasePointer SS::SLib::asin::Operate( VariableBasePointer X )
{
	NumType Tmp;
	mpfr_asin( Tmp.get_mpfr_t(), X->GetNumData().get_mpfr_t(), GMP_RNDN );

	return CreateVariable( SS_BASE_ARGS_DEFAULTS, Tmp );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
acos::Operate
NOTES: arc-cosine
*/
VariableBasePointer SS::SLib::acos::Operate( VariableBasePointer X )
{
	NumType Tmp;
	mpfr_acos( Tmp.get_mpfr_t(), X->GetNumData().get_mpfr_t(), GMP_RNDN );

	return CreateVariable( SS_BASE_ARGS_DEFAULTS, Tmp );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
atan::Operate
NOTES: arc-tangent
*/
VariableBasePointer SS::SLib::atan::Operate( VariableBasePointer X )
{
	NumType Tmp;
	mpfr_atan( Tmp.get_mpfr_t(), X->GetNumData().get_mpfr_t(), GMP_RNDN );

	return CreateVariable( SS_BASE_ARGS_DEFAULTS, Tmp );
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 max::Operate
 NOTES: The maximum function.
*/
VariableBasePointer SS::SLib::max::Operate( VariableBasePointer X )
{
	const ListType& TheList = X->GetListPtr()->MakeFlatList()->GetInternalList();

	VariableBasePointer Result = TheList[0];

	unsigned int i;
	for( i = 0; i < TheList.size(); i++ )	{
		if( *(TheList[i]) > *Result ) Result = TheList[i];
	}

	return Result;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
min::Operate
NOTES: The maximum function.
*/
VariableBasePointer SS::SLib::min::Operate( VariableBasePointer X )
{
	const ListType& TheList = X->GetListPtr()->MakeFlatList()->GetInternalList();

	VariableBasePointer Result = TheList[0];

	unsigned int i;
	for( i = 0; i < TheList.size(); i++ )	{
		if( *(TheList[i]) < *Result ) Result = TheList[i];
	}

	return Result;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Rounds the number to the nearest integer.
*/
VariableBasePointer Int::Operate( VariableBasePointer X )
{
	NumType Tmp;
	mpfr_round( Tmp.get_mpfr_t(), X->GetNumData().get_mpfr_t() );
	
	return VariableBasePointer( CreateVariable( SS_BASE_ARGS_DEFAULTS, Tmp ) );
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Rounds to the next lowest integer.
*/
VariableBasePointer Floor::Operate( VariableBasePointer X )
{
	NumType Tmp;
	mpfr_floor( Tmp.get_mpfr_t(), X->GetNumData().get_mpfr_t() );
	
	return VariableBasePointer( CreateVariable( SS_BASE_ARGS_DEFAULTS, Tmp ) );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: ctor
*/
MathConstPrec::MathConstPrec( const SS::STRING& Name, MathConst& Parent,
							  bool Static /*= false*/, bool Const /*= false*/ )
	: SpecialVarBase( Name, Static, Const ),
	  mParent(Parent)
{
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Along with assigning to the var, it tells the MathConst to update.
 		Hmm..this looks oddly familiar to PrecisionVar::operator= ...
*/
VariableBasePointer MathConstPrec::operator=( const VariableBase& X )
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
	mParent.mBufferValue.set_prec( NewPrec );
	
	//regenerate the constant
	mParent.Generate();

	return GetVariableBasePtr();
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Prefered type: number
*/
VarType MathConstPrec::GetVariableType() const
{
	return VARTYPE_NUM;	
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Get___Data...
*/
NumType MathConstPrec::GetNumData() const
{
	return NumType( mParent.mBufferValue.get_prec() );
}

BoolType MathConstPrec::GetBoolData() const
{
	return GetBoolDataFromNum();
}

StringType MathConstPrec::GetStringData() const
{
	return GetStringDataFromNum();
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: 
*/
MathConst::MathConst( SS_DECLARE_BASE_ARGS )
	: SpecialVarBase( SS_BASE_ARGS )
{
	bool WasConst = mConst;
	SetConst( false );
	Register( ScopeObjectPointer( new MathConstPrec( LC_Precision, *this, true, true ) ) );
	SetConst( WasConst );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Prefered type: num
*/
VarType MathConst::GetVariableType() const
{
	return VARTYPE_NUM;	
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Get the actual value.
*/
NumType MathConst::GetNumData() const
{
	static bool FirstCall = true;
	if( FirstCall ){
		 Generate();
		 FirstCall = false;
	}
	
	return mBufferValue;
}

BoolType MathConst::GetBoolData() const
{
	return GetBoolDataFromNum();
}

StringType MathConst::GetStringData() const
{
	return GetStringDataFromNum();
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Pi ctor
*/
Pi::Pi( SS_DECLARE_BASE_ARGS )
	: MathConst( SS_BASE_ARGS )
{
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Generate pi!
*/
void Pi::Generate() const
{
	mpfr_const_pi( mBufferValue.get_mpfr_t(), GMP_RNDN );	
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Euler ctor
*/
Euler::Euler( SS_DECLARE_BASE_ARGS )
	: MathConst( SS_BASE_ARGS )
{
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Generate pi!
*/
void Euler::Generate() const
{
	mpfr_const_euler( mBufferValue.get_mpfr_t(), GMP_RNDN );	
}
