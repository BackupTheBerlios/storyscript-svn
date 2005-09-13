
#include "Slib-Math.hpp"
#include "LanguageConstants.hpp"
#include "CreationFuncs.hpp"

//Used by mean
#include "List.hpp"

#include "mpfrxx.h"
#include "mpfr.h"


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
    const ListType& TheList = X->GetListPtr()->GetInternalList();

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
	const ListType& TheList = X->GetListPtr()->GetInternalList();

	VariableBasePointer Result = TheList[0];

	unsigned int i;
	for( i = 0; i < TheList.size(); i++ )	{
		if( *(TheList[i]) < *Result ) Result = TheList[i];
	}

	return Result;
}



