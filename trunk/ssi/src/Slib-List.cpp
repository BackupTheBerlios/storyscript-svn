/*
    Slib-List.cpp
    --------------------------
    Standard library functions for list manipulation.

	Created:	02:09:2005    16:16:50
*/


#include "Slib-List.hpp"
#include "List.hpp"
#include "LanguageConstants.hpp"
#include "CreationFuncs.hpp"

//Needed by PickOne
#include <boost/random.hpp>
#include "HelperFuncs.hpp"
#include "List.hpp"

//I may need to put this somewhere else if it
//turns out that its needed other places.
boost::mt19937 gRNG;
//boost::uniform_smallint< boost::mt19937, unsigned int > gRNG;


using namespace SS;
using namespace SS::SLib;


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Constructor
*/
SLib::List::List()
	: Scope( LC_SL_List, true )
{
	RegisterPredefined();
	SetConst();
}

void SLib::List::RegisterPredefined()
{
	Register( ScopeObjectPtr( new Remove( TXT("remove"), true, true ) ) );
	Register( ScopeObjectPtr( new RemoveAll( TXT("removeall"), true, true ) ) );
	Register( ScopeObjectPtr( new Push( TXT("push"), true, true ) ) );
	Register( ScopeObjectPtr( new Pop( TXT("pop"), true, true ) ) );
	Register( ScopeObjectPtr( new Sort( TXT("sort"), true, true ) ) );
	Register( ScopeObjectPtr( new Reverse( TXT("reverse"), true, true ) ) );
	Register( ScopeObjectPtr( new PickOne( TXT("pickone"), true, true ) ) );
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: See declaration.
*/
VariableBasePtr Remove::Operate( VariableBasePtr X )
{
	ListPtr Arguments = X->CastToList();
	ListPtr TheList = Arguments->GetInternalList()[0]->CastToList()->MakeFlatList();
	
	//Remove the first element and flatten
	Arguments->GetInternalList().erase( Arguments->GetInternalList().begin() );
	Arguments = Arguments->MakeFlatList();
	
	
	unsigned int i;
	for( i = 0; i < Arguments->GetInternalList().size(); i++ )
	{
		unsigned int j;
		for( j = 0; j < TheList->GetInternalList().size(); j++ )
		{
			if( (*((*TheList)[j]) == *((*Arguments)[i]))->GetBoolData() )
			{
				TheList->GetInternalList().erase( TheList->GetInternalList().begin() + j );
				break;
			}
		}
		
	}
	
	
	return TheList;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: See declaration.
*/
VariableBasePtr RemoveAll::Operate( VariableBasePtr X )
{
	ListPtr Arguments = X->CastToList();
	ListPtr TheList = Arguments->GetInternalList()[0]->CastToList()->MakeFlatList();
	
	//Remove the first element and flatten
	Arguments->GetInternalList().erase( Arguments->GetInternalList().begin() );
	Arguments = Arguments->MakeFlatList();
	
	
	unsigned long i;
	for( i = 0; i < (unsigned long)Arguments->GetInternalList().size(); i++ )
	{
		unsigned long j;
		for( j = (unsigned long)TheList->GetInternalList().size() - 1; j != 0U - 1U; j-- )
		{
			if( (*((*TheList)[j]) == *((*Arguments)[i]))->GetBoolData() )
			{
				TheList->GetInternalList().erase( TheList->GetInternalList().begin() + j );
			}
		}
		
	}
	
	
	return TheList;	
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: See declaration.
*/
VariableBasePtr Push::Operate( VariableBasePtr X )
{
	ListPtr Arguments = X->CastToList();
	ListPtr TheList = Arguments->GetInternalList()[0]->CastToList()->MakeFlatList();
	
	//Remove the first element and flatten
	Arguments->GetInternalList().erase( Arguments->GetInternalList().begin() );
	Arguments = Arguments->MakeFlatList();
	
	VariableBasePtr LastPushedVar;
	
	unsigned int i;
	for( i = 0; i < Arguments->GetInternalList().size(); i++ )
	{
		LastPushedVar = TheList->Push( (*Arguments)[i] );		
	}
	
	return LastPushedVar;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: See declaration.
*/
VariableBasePtr Pop::Operate( VariableBasePtr X )
{
	ListPtr Arguments = X->CastToList();
	VariableBasePtr LastPoped;
	unsigned int i;
	for( i = 0; i < Arguments->GetInternalList().size(); i++ )
	{
		LastPoped = (*Arguments)[i]->CastToList()->Pop();		
	}	
	
	return LastPoped;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: See declaration.  Thank you wikipedia for teaching me quicksort.
*/
VariableBasePtr Sort::Operate( VariableBasePtr X )
{
	ListPtr ListX = X->CastToList()->MakeFlatList();
	ListPtr pNewList( CreateGeneric<SS::List>( SS_BASE_ARGS_DEFAULTS ) );
	
	//If I don't SetConst to false, it will do the compound assignment trick.
	pNewList->SetConst( false );
	*pNewList = *ListX;
	pNewList->SetConst( true );
	
	return QuickSort( pNewList, 0, (unsigned long)pNewList->GetInternalList().size() );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: This where it gets done.
*/
ListPtr Sort::QuickSort( ListPtr TheList, unsigned long Begin, unsigned long End )
{
	if( End > Begin + 1)
	{
		ListType& ActualList = TheList->GetInternalList();
		VariableBasePtr Pivot = ActualList[Begin];
		VariableBasePtr Temp; //For swaps
		unsigned long Left = Begin + 1;
		unsigned long Right = End;
		
		while( Right > Left )
		{
			if( Compare( ActualList[Left], Pivot ) != 1 )
			{
				Left++;				
			}
			else
			{
				Right--;
				
				//swap TheList[Right] and TheList[Left]
				ActualList[Right].swap( ActualList[Left] );
			}			
		}
		Left--;
		
		//swap TheList[Begin] with TheList[Left]
		ActualList[Begin].swap( ActualList[Left] );
		
		QuickSort( TheList, Begin, Left );
		QuickSort( TheList, Right, End );
	}
	
	return TheList;	
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Compares two values aphabetically.  If you prefer numerically, 
 		feel free to overload Sort.
 		
 		Returns -1 if x is less than y,
 				 0 if x and y are equal,
 				 1 if x is greater than y
 				 
 		Any overloaded versions should do the same.
*/
int Sort::Compare( VariableBasePtr X, VariableBasePtr Y )
{
	int result = X->GetStringData().compare( Y->GetStringData() );	
	if( result == 0 ) return 0;
	else return result < 0 ? -1 : 1;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Reverses any list it is given.  Doesn't modify the orignal.
*/
VariableBasePtr Reverse::Operate( VariableBasePtr X )
{
	//We MUST store pTmp, or (if it is a temporary) the gc will delete it.
	ListPtr pTmp = X->CastToList()->MakeFlatList();
	ListType& Arg = pTmp->GetInternalList();
	
	ListPtr pNewList( CreateGeneric<SS::List>( SS_BASE_ARGS_DEFAULTS ) );
	
	ListType::reverse_iterator i;
	for( i = Arg.rbegin(); i != Arg.rend(); i++ )
	{
		pNewList->Push( *i );		
	}
	
		
	return pNewList;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: This little guy will randomly pick on of the elements from the list
 		and return it.
*/
VariableBasePtr PickOne::Operate( VariableBasePtr X )
{
	ListPtr pList = X->CastToList()->MakeFlatList();

	if( pList->GetInternalList().size() == 0 ) return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, false );

	boost::uniform_int<unsigned int> DistributedRandom( 0, (unsigned int)pList->GetInternalList().size() - 1 );
	return pList->GetInternalList()[ DistributedRandom( gRNG ) ];
	
	/*
	ListPtr pList = X->CastToList();
	double Random = RNG();
	unsigned int ListSize = pList->GetInternalList().size();
	
	unsigned int Index = (unsigned int)( Random * (double)ListSize );
	return pList->GetInternalList()[Index];
	*/
}
