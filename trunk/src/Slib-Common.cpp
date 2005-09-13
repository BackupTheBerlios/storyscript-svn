/*
    Slib-Common.cpp
    --------------------------
    Standard library functions that are automatically
	imported into the global scope.

	Created:	20:06:2005    (DONT KNOW THE DAMN TIME!)
*/

#include "Slib-Common.hpp"
#include "LanguageConstants.hpp"
#include "Variable.hpp"

//Needed by print
#include "Interpreter.hpp"
#include "Interface.hpp"

//Needed by PickOne
#include <boost/random.hpp>
#include "HelperFuncs.hpp"
#include "List.hpp"

#include "CreationFuncs.hpp"


//I may need to put this somewhere else if it
//turns out that its needed other places.
boost::mt19937 gRNG;
//boost::uniform_smallint< boost::mt19937, unsigned int > gRNG;



using namespace SS;
using namespace SS::SLib;


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Common::Common
 NOTES: Common constructor
*/
Common::Common()
	: Scope( LC_SL_Common, true )
{
	RegisterPredefined();
	SetConst();
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Common:RegisterPredefined
 NOTES: Registers all the common functions into one scope.
*/
void Common::RegisterPredefined()
{
	Register( ScopeObjectPointer( new print( TXT("print"), true, true ) ) );
	Register( ScopeObjectPointer( new PickOne( TXT("PickOne"), true, true ) ) );
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 print::Operate
 NOTES: Prints a message to the console.
*/
VariableBasePointer print::Operate( VariableBasePointer X )
{
	Interpreter::Instance().GetInterface().LogMessage( X->GetStringData(), true );
	return X;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: This little guy will randomly pick on of the elements from the list
 		and return it.
*/
VariableBasePointer PickOne::Operate( VariableBasePointer X )
{
	ListPointer pList = X->GetListPtr()->MakeFlatList();

	if( pList->GetInternalList().size() == 0 ) return CreateVariable( SS_BASE_ARGS_DEFAULTS, false );

	boost::uniform_int<unsigned int> DistributedRandom( 0, (unsigned int)pList->GetInternalList().size() - 1 );
	return pList->GetInternalList()[ DistributedRandom( gRNG ) ];
	
	/*
	ListPointer pList = X->GetListPtr();
	double Random = RNG();
	unsigned int ListSize = pList->GetInternalList().size();
	
	unsigned int Index = (unsigned int)( Random * (double)ListSize );
	return pList->GetInternalList()[Index];
	*/
}


