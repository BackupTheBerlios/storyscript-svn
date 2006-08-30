/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: Definitions for the List class; a dynamicaly sized list.
*/

#include "List.hpp"
#include "LanguageConstants.hpp"
#include "SpecialVars.hpp"
#include "CreationFuncs.hpp"
#include <boost/lexical_cast.hpp>

#include <mpfr.h>

using namespace SS;


//The global empty list variable
ListPtr SS::gpEmptyList;

//const ListPtr SS::gpEmptyList( new List( SS::STRING(), true, true ) );


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 List::List
 NOTES: 
*/
List::List()
{
	RegisterPredefinedVars();
}

List::List( const STRING& Name, bool Static /*= false*/, bool Const /*= false*/ )
: VariableBase( Name, Static, Const )
{
	RegisterPredefinedVars();
}

List::~List()
{

}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 List::AcceptVisitor
 NOTES: 
*/
void List::AcceptVisitor( ScopeObjectVisitor& V ){
	V.VisitList( this );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 List::CastToList
 NOTES: Returns itself in list form.
*/
ListPtr List::CastToList(){
	AssertCastingAllowed();
	return boost::dynamic_pointer_cast<List>( ScopeObjectPtr( mpThis ) );
}

const ListPtr List::CastToList() const{
	AssertCastingAllowed();
	return boost::dynamic_pointer_cast<List>( ScopeObjectPtr( mpThis ) );
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 List::Resize
 NOTES: Resize the list.
*/
void List::Resize( const NumType& NewSize )
{
	
	unsigned int iNewSize = mpfr_get_ui( NewSize.get(), GMP_RNDN );

	if( iNewSize > mList.max_size() )
	{
		STRING tmp = TXT("Cannot resize list \'");
		tmp += this->GetFullName();
		tmp += TXT("\' to \'");
		tmp += NumType2StringType(NewSize);
		tmp += TXT("\'.  That's just too damn big.");
		ThrowParserAnomaly( tmp, ANOMALY_LISTTOOBIG );
	}
	else if( iNewSize < 0 )	{
		ThrowParserAnomaly( TXT("Cannot create a list of negative size."),
							ANOMALY_LISTTOOBIG );
	}

	mList.resize( iNewSize );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 List::Push
 NOTES: Adds an element to the end of the list.
*/
VariableBasePtr List::Push( VariableBasePtr pNewElement )
{
	VariableBasePtr pNewVar = CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, 0 );
	*pNewVar = *pNewElement;
	
	mList.push_back( pNewVar );
	
	return pNewVar;	
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 List::Pop
 NOTES: Removes an element from the end of the list.
*/
VariableBasePtr List::Pop(){
	if( mList.size() == 0 )	{		
		SS::STRING tmp = mName;
		tmp += TXT(" has no element to remove.");
		ThrowParserAnomaly( tmp, ANOMALY_NOLISTELEMENT );
	}

	VariableBasePtr OldEndElement = mList.back();
	mList.pop_back();
	return OldEndElement;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 List::AppendList
 NOTES: Adds all the elements of one list onto another.
*/
void List::AppendList( const ListPtr OtherList ){
	unsigned int i;
	const ListType& OtherVector = OtherList->GetInternalList();
	
	for( i = 0; i < OtherVector.size(); i++ ){
		mList.push_back( CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, *(OtherVector[i]->CastToVariable()) ) );
	}
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Appens an element to the list, but instead of making a copy, it just
 		uses the pointer its given.  This is useful if you want to have
 		multiple aliases for an object.
*/
void List::PushWithoutCopy( VariableBasePtr X )
{
	mList.push_back( X );	
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Similar as the above, but it adds every element in the given list,
 		rather than the list itself.
*/
void List::AppendListWithoutCopy( ListPtr X )
{
	unsigned int i;
	for( i = 0; i< X->GetInternalList().size(); i++ )
	{
		PushWithoutCopy( X->GetInternalList()[i] );
	}
}

//DEPRICATED:  Lists should never be multideminsional
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: If a the list is multideminsional, FlattenList will create a single
 		deminsional version and return it.
*/
/*
ListPtr List::MakeFlatList() const
{
	
	
	unsigned int i;
	
	//Go through once and just check.  Because if you don't have
	//to create a new list, it will be a big performance boost
	for( i = 0; i < mList.size(); i++ )
	{
		if( GetScopeObjectType( mList[i] ) == SCOPEOBJ_LIST ) break;
	}
	if( i == mList.size() ) return CastToList();
	
	ListPtr pNewList = CreateGeneric<List>();
	
	for( i = 0; i < mList.size(); i++ )
	{
		if( GetScopeObjectType( mList[i] ) == SCOPEOBJ_LIST )
		{
			pNewList->AppendListWithoutCopy( mList[i]->CastToList()->MakeFlatList() );
		}
		else
		{
			pNewList->AppendWithoutCopy( mList[i] );			 
		}
	}
		
	return pNewList;
}
*/



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 List::DetermineRealIndex
 NOTES: Give it a variable and it will covert it to the true index.
		(ie. negative numbers will get transformed the the apropriate
		positive number.)
*/
unsigned int List::DetermineRealIndex( const VariableBase& Index )
{
	long TrueIndex = 
	mpfr_get_si( Index.CastToVariable()->GetNumData().get(), GMP_RNDN );
	
	//Special Case for Empty Lists
	//if( mList.size() == 0 && (TrueIndex == 0 || TrueIndex == -1) ) return 0;
	
	unsigned int GoodIndex = 0;

	if( TrueIndex < 0 ){
		//Special case for empty lists
		if( mList.size() == 0 ) GoodIndex = 0;
		else GoodIndex = (unsigned int)mList.size() - (unsigned int)-TrueIndex;
	}
	else if( (unsigned int)TrueIndex >= mList.size() )
	{
		while( (unsigned int)TrueIndex >= mList.size() )
		{
			if( LangOpts::Instance().UseStrictLists )
			{
				SS::STRING tmp = TXT("Cannot access element ");
				tmp += boost::lexical_cast<SS::STRING>(GoodIndex);
				tmp += TXT(" of list ");
				tmp += mName;
				tmp += TXT(".");
				ThrowParserAnomaly( tmp, ANOMALY_NOLISTELEMENT );
			}
			else{
				this->Push( CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, 0 ) );
			}
		}

		GoodIndex = TrueIndex;
	}
	else GoodIndex = (unsigned int)TrueIndex;

	return GoodIndex;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 List::operator[]
 NOTES: Used to access element of a a list.  
		Negative values will access the list from the back
*/
VariableBasePtr List::operator []( const VariableBasePtr Index ){
	return mList[ DetermineRealIndex(*Index) ];
}

//Careful with this one, it doesn't do any fancy warp around.
VariableBasePtr List::operator[]( unsigned int Index ){
	return mList[ Index ];
}


//DEPRICATED: It doesn't seem like I would ever need such a function
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: This one copies the pointers rather than the pointer's values.
*/
/*
void List::CopyExactly( const ListPtr X )
{
	mList = X->mList;
}
*/

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 List::operator=
 NOTES: Assign one list to another.
*/
void List::operator=( const List& OtherList )
{
	//ListPtr pFlatList = OtherList.MakeFlatList();
	
	if( IsConst() )
	{
		unsigned int i;
		unsigned int MaxSize;

		//This is just the equivalent of the the min macro, but I don't want
		//to declare it just for this.
		if( OtherList.GetInternalList().size() > mList.size() ){
			MaxSize = (unsigned int)mList.size();
		}
		else{
			MaxSize = (unsigned int)OtherList.GetInternalList().size();
		}

		for( i = 0; i < MaxSize; i++ )
		{
			*(mList[i]) = *(OtherList.GetInternalList()[i]);
		}
	}
	else
	{
		//this->mList = OtherList.mList;
		unsigned int i;
		for( i = 0; i < OtherList.mList.size(); i++ )
		{
			if( i >= this->mList.size() ){
				AppendList( OtherList.mList[i]->CastToList() );
			}
			else{
				*(this->mList[i]) = *(OtherList.mList[i]);
			}
			
		}
		
		//In the case that this->mList.size() > OtherList.mList.size()
		while( i < mList.size() ){
			mList.erase( mList.end() - 1 );
		}
	}
}

VariableBasePtr List::operator=( const VariableBase& X )
{
	ListPtr XList = X.CastToList();
	*this = *XList;
	return CastToVariableBase();
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 List::Remove
 NOTES: Removes an element from the list.
*/
VariableBasePtr List::Remove( const VariableBasePtr Index )
{
	unsigned int RealIndex = DetermineRealIndex( *Index );

	VariableBasePtr TempElement = mList[RealIndex];
	mList.erase( mList.begin() + RealIndex );

	return TempElement;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 List::Insert
 NOTES: Inserts an element from the list.
*/
VariableBasePtr List::Insert( const VariableBasePtr Index )
{
	//This catches a bug where an extra element is added when
	//+[] is called with an index >= to the size.
	if( *Index >= *Length() )
	{
		return (*this)[Index];
	}

	unsigned int RealIndex = DetermineRealIndex(*Index);
	mList.insert( mList.begin() + RealIndex, CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, 0 ) );
	return mList[RealIndex];
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 List::Length
 NOTES: Returns the length of the list.
*/
VariableBasePtr List::Length() const{
	return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, (NumType)(unsigned long)mList.size() );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 List::RegisterPredefinedVars
 NOTES: 
*/
void List::RegisterPredefinedVars()
{
	mPopCreated = mPushCreated = mRemoveAllCreated =
		 mRemoveCreated = mLengthCreated = false;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 List::GetInternalList
 NOTES: Returns a const reference to the actual vector.
*/
const ListType& List::GetInternalList() const{
	return mList;
}

ListType& List::GetInternalList(){
	return mList;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Converts a list to one string with every element right next to eachother.
*/
VariablePtr List::MakeVariable() const
{
	STRING TheBigString;
	
	unsigned int i;
	for( i = 0; i < mList.size(); i++ )
	{
		TheBigString += mList[i]->CastToVariableBase()->GetStringData();		
	}
	
	return CreateVariable<Variable>( mName, false, true, TheBigString );
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: These functions are to provide a scalare interface for the list.
*/
StringType List::GetStringData() const{
	return MakeVariable()->GetStringData();
}

NumType List::GetNumData() const{
	return MakeVariable()->GetNumData();
}

BoolType List::GetBoolData() const{
	return MakeVariable()->GetBoolData();
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Returns itself in scalar form
*/
VariablePtr List::CastToVariable(){
	return MakeVariable();
}

const VariablePtr List::CastToVariable() const{
	return MakeVariable();
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Overloaded to intercept lookups to predef'ed objecs and creates them
 		on the fly.  Otherwise there is lot of overhead for stuff you probably
 		won't even use.
*/
ScopeObjectPtr List::GetScopeObjectHook( const STRING& Name )
{
	if( !mPopCreated && Name == LC_LIST_Pop ){
		mPopCreated = true;
		return Register( ScopeObjectPtr( new PopOp( *this ) ) );
	}
	else if( !mPushCreated && Name == LC_LIST_Push ){
		mPushCreated = true;
		return Register( ScopeObjectPtr( new PushOp( *this ) ) );
	}
	else if( !mRemoveAllCreated && Name == LC_LIST_RemoveAll ){
		mRemoveAllCreated = true;
		return Register( ScopeObjectPtr( new RemoveAllOp( *this ) ) );
	}
	else if( !mRemoveCreated && Name == LC_LIST_Remove ){
		mRemoveCreated = true;
		return Register( ScopeObjectPtr( new RemoveOp( *this ) ) );
	}
	else if( !mLengthCreated && Name == LC_Length ){
		mLengthCreated = true;
		return Register( ScopeObjectPtr( new ListLengthVar( LC_Length, *this, true ) ) );
	}	
	else return VariableBase::GetScopeObjectHook( Name );	
}




/////////////////////////////////////////////////////////BUILT IN LIST FUNCTIONS

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Remove function.  Removes the first occourance of every element in the
 		input list.  Returns this list operated on.
*/
List::RemoveOp::RemoveOp( List& Parent )
: InternalListFunc( LC_LIST_Remove, Parent )
{}

VariableBasePtr List::RemoveOp::Operate( VariableBasePtr pX )
{
	const ListType& XList = pX->CastToList()->GetInternalList();
	
	size_t i, k;
	for( i = 0; i < XList.size(); i++ )
	{
			for( k = 0; k < mParentList.mList.size(); k++ )
			{
					if( mParentList.mList[k] == XList[i] )
					{
						mParentList.mList.erase( mParentList.mList.begin() + k );
						break;	
					}
			}		
	}
	
	
	return mParentList.CastToVariableBase();
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Similar to remove, but it removes all occouranced of the items in
 		the given list.  Returns the list operated on.
*/
List::RemoveAllOp::RemoveAllOp( List& Parent )
: InternalListFunc( LC_LIST_RemoveAll, Parent )
{}

VariableBasePtr List::RemoveAllOp::Operate( VariableBasePtr pX )
{
	const ListType& XList = pX->CastToList()->GetInternalList();
	
	size_t i, k;
	for( i = 0; i < XList.size(); i++ )
	{
			for( k = 0; k < mParentList.mList.size(); k++ )
			{
					if( mParentList.mList[k] == XList[i] )
					{
						mParentList.mList.erase( mParentList.mList.begin() + k );
						
						//de-increment to next time around it will test the same index.
						k--;
						continue;
					}
			}
	}
	
	
	return mParentList.CastToVariableBase();
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Push every element from the given list onto this list.  Returns the
 		list operated on.
*/
List::PushOp::PushOp( List& Parent )
: InternalListFunc( LC_LIST_Push, Parent )
{}

VariableBasePtr List::PushOp::Operate( VariableBasePtr pX )
{
	ListPtr pXList = pX->CastToList();
	
	const ListType& XList = pXList->GetInternalList();
	size_t i;
	for( i = 0; i < XList.size(); i++ )
	{
		mParentList.Push( XList[i] );
	}
	
	return mParentList.CastToVariableBase();	
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Remove one element from the list and returns it.
 		TODO: Consider having it take one argument giving a number of element
 		to remove from the end of the list.
*/
List::PopOp::PopOp( List& Parent )
: InternalListFunc( LC_LIST_Pop, Parent )
{}

VariableBasePtr List::PopOp::Operate( VariableBasePtr pX )
{
	return mParentList.Pop();	
}

