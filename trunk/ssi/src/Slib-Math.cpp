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
	Register( ScopeObjectPtr( new sqrt( TXT("sqrt"), true, true ) ) );	
	Register( ScopeObjectPtr( new abs( TXT("abs"), true, true ) ) );
	Register( ScopeObjectPtr( new mean( TXT("mean"), true, true ) ) );
	Register( ScopeObjectPtr( new sin( TXT("sin"), true, true ) ) );
	Register( ScopeObjectPtr( new cos( TXT("cos"), true, true ) ) );
	Register( ScopeObjectPtr( new tan( TXT("tan"), true, true ) ) );
	Register( ScopeObjectPtr( new asin( TXT("asin"), true, true ) ) );
	Register( ScopeObjectPtr( new acos( TXT("acos"), true, true ) ) );
	Register( ScopeObjectPtr( new atan( TXT("atan"), true, true ) ) );
	Register( ScopeObjectPtr( new max( TXT("max"), true, true ) ) );
	Register( ScopeObjectPtr( new min( TXT("min"), true, true ) ) );
	Register( ScopeObjectPtr( new Int( TXT("int"), true, true ) ) );
	Register( ScopeObjectPtr( new Floor( TXT("floor"), true, true ) ) );
	
	//Magic Variables
	Register( ScopeObjectPtr( new Pi( TXT("pi"), true, true ) ) );
	Register( ScopeObjectPtr( new Euler( TXT("e"), true, true ) ) );
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~
// sqrt::Operate
// NOTES: Performes the square root operation.
//
VariableBasePtr SS::SLib::sqrt::Operate( VariableBasePtr X )
{
	NumType Tmp;
	mpfr_sqrt( Tmp.get_mpfr_t(), X->GetNumData().get_mpfr_t(), GMP_RNDN );
	
	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, Tmp );	
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 abs::Operate
 NOTES: The absolute value operator.
*/
VariableBasePtr SS::SLib::abs::Operate( VariableBasePtr X )
{
    NumType Tmp;
	mpfr_abs( Tmp.get_mpfr_t(), X->GetNumData().get_mpfr_t(), GMP_RNDN );

	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, Tmp );
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 mean::Operate
 NOTES: The mean functions.
*/
VariableBasePtr SS::SLib::mean::Operate( VariableBasePtr X )
{
    ListPtr pLst = X->CastToList();

	NumType Tmp = 0;
	unsigned int i;
	for( i = 0; i < pLst->GetInternalList().size(); i++ ){
		Tmp += (pLst->GetInternalList()[i])->GetNumData();
	}

	Tmp /= NumType( i );

	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, Tmp );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 sin::Operate
 NOTES: The sine function
*/
VariableBasePtr SS::SLib::sin::Operate( VariableBasePtr X )
{
	NumType Tmp;
	mpfr_sin( Tmp.get_mpfr_t(), X->GetNumData().get_mpfr_t(), GMP_RNDN );

	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, Tmp );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 cos::Operate
 NOTES: The cosine function.
*/
VariableBasePtr SS::SLib::cos::Operate( VariableBasePtr X )
{
	NumType Tmp;
	mpfr_cos( Tmp.get_mpfr_t(), X->GetNumData().get_mpfr_t(), GMP_RNDN );

	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, Tmp );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 tan::Operate
 NOTES: The tangent function.
*/
VariableBasePtr SS::SLib::tan::Operate( VariableBasePtr X )
{
	NumType Tmp;
	mpfr_tan( Tmp.get_mpfr_t(), X->GetNumData().get_mpfr_t(), GMP_RNDN );

	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, Tmp );
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
asin::Operate
NOTES: arc-sine
*/
VariableBasePtr SS::SLib::asin::Operate( VariableBasePtr X )
{
	NumType Tmp;
	mpfr_asin( Tmp.get_mpfr_t(), X->GetNumData().get_mpfr_t(), GMP_RNDN );

	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, Tmp );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
acos::Operate
NOTES: arc-cosine
*/
VariableBasePtr SS::SLib::acos::Operate( VariableBasePtr X )
{
	NumType Tmp;
	mpfr_acos( Tmp.get_mpfr_t(), X->GetNumData().get_mpfr_t(), GMP_RNDN );

	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, Tmp );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
atan::Operate
NOTES: arc-tangent
*/
VariableBasePtr SS::SLib::atan::Operate( VariableBasePtr X )
{
	NumType Tmp;
	mpfr_atan( Tmp.get_mpfr_t(), X->GetNumData().get_mpfr_t(), GMP_RNDN );

	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, Tmp );
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 max::Operate
 NOTES: The maximum function.
*/
VariableBasePtr SS::SLib::max::Operate( VariableBasePtr X )
{
	const ListType& TheList = X->CastToList()->MakeFlatList()->GetInternalList();

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
	const ListType& TheList = X->CastToList()->MakeFlatList()->GetInternalList();

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
	mpfr_round( Tmp.get_mpfr_t(), X->GetNumData().get_mpfr_t() );
	
	return VariableBasePtr( CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, Tmp ) );
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Rounds to the next lowest integer.
*/
VariableBasePtr Floor::Operate( VariableBasePtr X )
{
	NumType Tmp;
	mpfr_floor( Tmp.get_mpfr_t(), X->GetNumData().get_mpfr_t() );
	
	return VariableBasePtr( CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, Tmp ) );
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
VariableBasePtr MathConstPrec::operator=( const VariableBase& X )
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
	Register( ScopeObjectPtr( new MathConstPrec( LC_Precision, *this, true, true ) ) );
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
