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
	Register( ScopeObjectPointer( new Remove( TXT("remove"), true, true ) ) );
	Register( ScopeObjectPointer( new RemoveAll( TXT("removeall"), true, true ) ) );
	Register( ScopeObjectPointer( new Push( TXT("push"), true, true ) ) );
	Register( ScopeObjectPointer( new Pop( TXT("pop"), true, true ) ) );
	Register( ScopeObjectPointer( new Sort( TXT("sort"), true, true ) ) );
	Register( ScopeObjectPointer( new Reverse( TXT("reverse"), true, true ) ) );
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: See declaration.
*/
VariableBasePointer Remove::Operate( VariableBasePointer X )
{
	ListPointer Arguments = X->GetListPtr();
	ListPointer TheList = Arguments->GetInternalList()[0]->GetListPtr()->MakeFlatList();
	
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
VariableBasePointer RemoveAll::Operate( VariableBasePointer X )
{
	ListPointer Arguments = X->GetListPtr();
	ListPointer TheList = Arguments->GetInternalList()[0]->GetListPtr()->MakeFlatList();
	
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
VariableBasePointer Push::Operate( VariableBasePointer X )
{
	ListPointer Arguments = X->GetListPtr();
	ListPointer TheList = Arguments->GetInternalList()[0]->GetListPtr()->MakeFlatList();
	
	//Remove the first element and flatten
	Arguments->GetInternalList().erase( Arguments->GetInternalList().begin() );
	Arguments = Arguments->MakeFlatList();
	
	VariableBasePointer LastPushedVar;
	
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
VariableBasePointer Pop::Operate( VariableBasePointer X )
{
	ListPointer Arguments = X->GetListPtr();
	VariableBasePointer LastPoped;
	unsigned int i;
	for( i = 0; i < Arguments->GetInternalList().size(); i++ )
	{
		LastPoped = (*Arguments)[i]->GetListPtr()->Pop();		
	}	
	
	return LastPoped;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: See declaration.  Thank you wikipedia for teaching me quicksort.
*/
VariableBasePointer Sort::Operate( VariableBasePointer X )
{
	ListPointer ListX = X->GetListPtr()->MakeFlatList();
	ListPointer pNewList( CreateObject<SS::List>( SS_BASE_ARGS_DEFAULTS ) );
	
	//If I don't SetConst to false, it will do the compound assignment trick.
	pNewList->SetConst( false );
	*pNewList = *ListX;
	pNewList->SetConst( true );
	
	return QuickSort( pNewList, 0, pNewList->GetInternalList().size() );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: This where it gets done.
*/
ListPointer Sort::QuickSort( ListPointer TheList, unsigned int Begin, unsigned int End )
{
	if( End > Begin + 1)
	{
		ListType& ActualList = TheList->GetInternalList();
		VariableBasePointer Pivot = ActualList[Begin];
		VariableBasePointer Temp; //For swaps
		unsigned int Left = Begin + 1;
		unsigned int Right = End;
		
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
int Sort::Compare( VariableBasePointer X, VariableBasePointer Y )
{
	int result = X->GetStringData().compare( Y->GetStringData() );	
	if( result == 0 ) return 0;
	else return result < 0 ? -1 : 1;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Reverses any list it is given.  Doesn't modify the orignal.
*/
VariableBasePointer Reverse::Operate( VariableBasePointer X )
{
	//We MUST store pTmp, or (if it is a temporary) the gc will delete it.
	ListPointer pTmp = X->GetListPtr()->MakeFlatList();
	ListType& Arg = pTmp->GetInternalList();
	
	ListPointer pNewList( CreateObject<SS::List>( SS_BASE_ARGS_DEFAULTS ) );
	
	ListType::reverse_iterator i;
	for( i = Arg.rbegin(); i != Arg.rend(); i++ )
	{
		pNewList->Push( *i );		
	}
	
		
	return pNewList;
}
