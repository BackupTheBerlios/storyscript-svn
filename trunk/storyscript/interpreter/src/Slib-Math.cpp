/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: S-Lib functions/mvars that are used for more advanced math operations.
*/

#include "Slib-Math.hpp"
#include "LanguageConstants.hpp"
#include "CreationFuncs.hpp"

//Used by mean
#include "List.hpp"

//#include "mpfrxx.h"
#include <mpfr.h>

#include <boost/lexical_cast.hpp>


using namespace SS;
using namespace SS::SLib;




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~
// Math::Math
// NOTES: constructor
//
Math::Math()
	: Scope( LC_SL_Math, false )
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
	Register( ScopeObjectPtr( new sqrt ( TXT("sqrt"),  true ) ) );	
	Register( ScopeObjectPtr( new abs  ( TXT("abs"),   true ) ) );
	Register( ScopeObjectPtr( new mean ( TXT("mean"),  true ) ) );
	Register( ScopeObjectPtr( new sin  ( TXT("sin"),   true ) ) );
	Register( ScopeObjectPtr( new cos  ( TXT("cos"),   true ) ) );
	Register( ScopeObjectPtr( new tan  ( TXT("tan"),   true ) ) );
	Register( ScopeObjectPtr( new asin ( TXT("asin"),  true ) ) );
	Register( ScopeObjectPtr( new acos ( TXT("acos"),  true ) ) );
	Register( ScopeObjectPtr( new atan ( TXT("atan"),  true ) ) );
	Register( ScopeObjectPtr( new max  ( TXT("max"),   true ) ) );
	Register( ScopeObjectPtr( new min  ( TXT("min"),   true ) ) );
	Register( ScopeObjectPtr( new Int  ( TXT("int"),   true ) ) );
	Register( ScopeObjectPtr( new Floor( TXT("floor"), true ) ) );
	
	//Magic Variables
	Register( ScopeObjectPtr( new Pi(    TXT("pi"), true ) ) );
	Register( ScopeObjectPtr( new Euler( TXT("e"),  true ) ) );
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~
// sqrt::Operate
// NOTES: Performes the square root operation.
//
VariableBasePtr SS::SLib::sqrt::Operate( VariableBasePtr X )
{
	NumType Tmp;
	mpfr_sqrt( Tmp.get(), X->GetNumData().get(), GMP_RNDN );
	
	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, Tmp );	
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 abs::Operate
 NOTES: The absolute value operator.
*/
VariableBasePtr SS::SLib::abs::Operate( VariableBasePtr X )
{
    NumType Tmp;
	mpfr_abs( Tmp.get(), X->GetNumData().get(), GMP_RNDN );

	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, Tmp );
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 mean::Operate
 NOTES: The mean functions.
*/
VariableBasePtr SS::SLib::mean::Operate( VariableBasePtr X )
{
    ListPtr pLst = X->CastToList();

	NumType Tmp;
	unsigned int i;
	for( i = 0; i < pLst->GetInternalList().size(); i++ ){
		mpfr_add( Tmp.get(), Tmp.get(), (pLst->GetInternalList()[i])->GetNumData().get(), GMP_RNDN );
	}
	
	NumType BigI;
	mpfr_set_ui( BigI.get(), i, GMP_RNDN );

	mpfr_div( Tmp.get(), Tmp.get(), BigI.get(), GMP_RNDN );

	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, Tmp );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 sin::Operate
 NOTES: The sine function
*/
VariableBasePtr SS::SLib::sin::Operate( VariableBasePtr X )
{
	NumType Tmp;
	mpfr_sin( Tmp.get(), X->GetNumData().get(), GMP_RNDN );

	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, Tmp );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 cos::Operate
 NOTES: The cosine function.
*/
VariableBasePtr SS::SLib::cos::Operate( VariableBasePtr X )
{
	NumType Tmp;
	mpfr_cos( Tmp.get(), X->GetNumData().get(), GMP_RNDN );

	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, Tmp );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 tan::Operate
 NOTES: The tangent function.
*/
VariableBasePtr SS::SLib::tan::Operate( VariableBasePtr X )
{
	NumType Tmp;
	mpfr_tan( Tmp.get(), X->GetNumData().get(), GMP_RNDN );

	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, Tmp );
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
asin::Operate
NOTES: arc-sine
*/
VariableBasePtr SS::SLib::asin::Operate( VariableBasePtr X )
{
	NumType Tmp;
	mpfr_asin( Tmp.get(), X->GetNumData().get(), GMP_RNDN );

	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, Tmp );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
acos::Operate
NOTES: arc-cosine
*/
VariableBasePtr SS::SLib::acos::Operate( VariableBasePtr X )
{
	NumType Tmp;
	mpfr_acos( Tmp.get(), X->GetNumData().get(), GMP_RNDN );

	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, Tmp );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
atan::Operate
NOTES: arc-tangent
*/
VariableBasePtr SS::SLib::atan::Operate( VariableBasePtr X )
{
	NumType Tmp;
	mpfr_atan( Tmp.get(), X->GetNumData().get(), GMP_RNDN );

	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, Tmp );
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 max::Operate
 NOTES: The maximum function.
*/
VariableBasePtr SS::SLib::max::Operate( VariableBasePtr X )
{
	const ListType& TheList = X->CastToList()->GetInternalList();

	VariableBasePtr Result = TheList[0];

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
VariableBasePtr SS::SLib::min::Operate( VariableBasePtr X )
{
	const ListType& TheList = X->CastToList()->GetInternalList();

	VariableBasePtr Result = TheList[0];

	unsigned int i;
	for( i = 0; i < TheList.size(); i++ )	{
		if( *(TheList[i]) < *Result ) Result = TheList[i];
	}

	return Result;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Rounds the number to the nearest integer.
*/
VariableBasePtr Int::Operate( VariableBasePtr X )
{
	NumType Tmp;
	mpfr_round( Tmp.get(), X->GetNumData().get() );
	
	return VariableBasePtr( CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, Tmp ) );
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Rounds to the next lowest integer.
*/
VariableBasePtr Floor::Operate( VariableBasePtr X )
{
	NumType Tmp;
	mpfr_floor( Tmp.get(), X->GetNumData().get() );
	
	return VariableBasePtr( CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, Tmp ) );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: ctor
*/
MathConstPrec::MathConstPrec( const SS::STRING& Name, bool Const, MathConst& Parent )
	: SpecialVarBase( Name, Const ),
	  mParent(Parent)
{
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Along with assigning to the var, it tells the MathConst to update.
 		Hmm..this looks oddly familiar to PrecisionVar::operator= ...
*/
VariableBasePtr MathConstPrec::operator=( const VariableBase& X )
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
	mpfr_set_prec( mParent.mBufferValue.get(), NewPrec );
	
	//regenerate the constant
	mParent.Generate();

	return CastToVariableBase();
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
	NumType Out;
	mpfr_set_ui( Out.get(), mpfr_get_prec(mParent.mBufferValue.get()), GMP_RNDN );
	return Out;
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
	Register( ScopeObjectPtr( new MathConstPrec( LC_Precision, true, *this ) ) );
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
	mpfr_const_pi( mBufferValue.get(), GMP_RNDN );	
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
	mpfr_const_euler( mBufferValue.get(), GMP_RNDN );	
}
