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

using namespace SS;


//The global empty list variable
ListPointer SS::gpEmptyList;

//const ListPointer SS::gpEmptyList( new List( SS::STRING(), true, true ) );


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 List::List
 NOTES: 
*/
List::List(){
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
 List::GetListPtr
 NOTES: Returns itself in list form.
*/
ListPointer List::GetListPtr(){
	return boost::dynamic_pointer_cast<List>( ScopeObjectPointer( mpThis ) );
}

const ListPointer List::GetListPtr() const{
	return boost::dynamic_pointer_cast<List>( ScopeObjectPointer( mpThis ) );
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 List::Resize
 NOTES: Resize the list.
*/
void List::Resize( const NumType& NewSize )
{

	if( NewSize.get_ui() > mList.max_size() )
	{
		STRING tmp = TXT("Cannot resize list \'");
		tmp += this->GetFullName();
		tmp += TXT("\' to \'");
		tmp += NumType2StringType(NewSize);
		tmp += TXT("\'.  That's just too damn big.");
		ThrowParserAnomaly( tmp, ANOMALY_LISTTOOBIG );
	}
	else if( NewSize < (NumType)0 )	{
		ThrowParserAnomaly( TXT("Cannot create a list of negative size."),
							ANOMALY_LISTTOOBIG );
	}

	mList.resize( NewSize.get_ui() );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 List::Push
 NOTES: Adds an element to the end of the list.
*/
VariableBasePointer List::Push( VariableBasePointer pNewElement )
{
	VariableBasePointer pNewVar = CreateVariable( SS_BASE_ARGS_DEFAULTS, 0 );
	*pNewVar = *pNewElement;
	
	mList.push_back( pNewVar );
	
	return pNewVar;	
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 List::Pop
 NOTES: Removes an element from the end of the list.
*/
VariableBasePointer List::Pop(){
	if( mList.size() == 0 )	{		
		SS::STRING tmp = mName;
		tmp += TXT(" has no element to remove.");
		ThrowParserAnomaly( tmp, ANOMALY_NOLISTELEMENT );
	}

	VariableBasePointer OldEndElement = mList.back();
	mList.pop_back();
	return OldEndElement;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 List::Append
 NOTES: Adds all the elements of one list onto another.
*/
void List::Append( const ListPointer OtherList ){
	unsigned int i;
	const ListType& OtherVector = OtherList->GetInternalList();
	
	for( i = 0; i < OtherVector.size(); i++ ){
		mList.push_back( CreateVariable( SS_BASE_ARGS_DEFAULTS, *(OtherVector[i]->GetVariablePtr()) ) );
	}
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Appens an element to the list, but instead of making a copy, it just
 		uses the pointer its given.  This is useful if you want to have
 		multiple aliases for an object.
*/
void List::AppendWithoutCopy( VariableBasePointer X )
{
	mList.push_back( X );	
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Similar as the above, but it adds every element in the given list,
 		rather than the list itself.
*/
void List::AppendListWithoutCopy( ListPointer X )
{
	unsigned int i;
	for( i = 0; i< X->GetInternalList().size(); i++ )
	{
		AppendWithoutCopy( X->GetInternalList()[i] );
	}
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: If a the list is multideminsional, FlattenList will create a single
 		deminsional version and return it.
*/
ListPointer List::MakeFlatList() const
{
	
	
	unsigned int i;
	
	//Go through once and just check.  Because if you don't have
	//to create a new list, it will be a big performance boost
	for( i = 0; i < mList.size(); i++ )
	{
		if( GetScopeObjectType( mList[i] ) == SCOPEOBJ_LIST ) break;
	}
	if( i == mList.size() ) return GetListPtr();
	
	ListPointer pNewList = CreateObject<List>();
	
	for( i = 0; i < mList.size(); i++ )
	{
		if( GetScopeObjectType( mList[i] ) == SCOPEOBJ_LIST )
		{
			pNewList->AppendListWithoutCopy( mList[i]->GetListPtr()->MakeFlatList() );
		}
		else
		{
			pNewList->AppendWithoutCopy( mList[i] );			 
		}
	}
		
	return pNewList;
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 List::DetermineRealIndex
 NOTES: Give it a variable and it will covert it to the true index.
		(ie. negative numbers will get transformed the the apropriate
		positive number.)
*/
unsigned int List::DetermineRealIndex( const VariableBase& Index )
{
	long TrueIndex = (Index.GetVariablePtr()->GetNumData().get_si());
	
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
			if( gUsingStrictLists )
			{
				SS::STRING tmp = TXT("Cannot access element ");
				tmp += boost::lexical_cast<SS::STRING>(GoodIndex);
				tmp += TXT(" of list ");
				tmp += mName;
				tmp += TXT(".");
				ThrowParserAnomaly( tmp, ANOMALY_NOLISTELEMENT );
			}
			else{
				this->Push( CreateVariable( SS_BASE_ARGS_DEFAULTS, 0 ) );
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
VariableBasePointer List::operator []( const VariableBasePointer Index ){
	return mList[ DetermineRealIndex(*Index) ];
}

//Careful with this one, it doesn't do any fancy warp around.
VariableBasePointer List::operator[]( unsigned int Index ){
	return mList[ Index ];
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: This one copies the pointers rather than the pointer's values.
*/
void List::CopyExactly( const ListPointer X )
{
	mList = X->mList;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 List::operator=
 NOTES: Assign one list to another.
*/
void List::operator=( const List& OtherList )
{
	ListPointer pFlatList = OtherList.MakeFlatList();
	
	if( IsConst() )
	{
		unsigned int i;
		unsigned int MaxSize;

		//This is just the equivalent of the the min macro, but I don't want
		//to declare it just for this.
		if( pFlatList->GetInternalList().size() > mList.size() ){
			MaxSize = (unsigned int)mList.size();
		}
		else{
			MaxSize = (unsigned int)pFlatList->GetInternalList().size();
		}

		for( i = 0; i < MaxSize; i++ )
		{
			*(mList[i]) = *(pFlatList->GetInternalList()[i]);
		}
	}
	else
	{
		//this->mList = OtherList.mList;
		unsigned int i;
		for( i = 0; i < pFlatList->mList.size(); i++ )
		{
			if( i >= this->mList.size() ){
				Append( pFlatList->mList[i]->GetListPtr() );
			}
			else{
				*(this->mList[i]) = *(pFlatList->mList[i]);
			}
			
		}
		
		//In the case that this->mList.size() > OtherList.mList.size()
		while( i < mList.size() ){
			mList.erase( mList.end() - 1 );
		}
	}
}

VariableBasePointer List::operator=( const VariableBase& X )
{
	*this = *(X.GetListPtr()); //This should with lists or scalars
	return GetVariableBasePtr();
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 List::Remove
 NOTES: Removes an element from the list.
*/
VariableBasePointer List::Remove( const VariableBasePointer Index )
{
	unsigned int RealIndex = DetermineRealIndex( *Index );

	VariableBasePointer TempElement = mList[RealIndex];
	mList.erase( mList.begin() + RealIndex );

	return TempElement;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 List::Insert
 NOTES: Inserts an element from the list.
*/
VariableBasePointer List::Insert( const VariableBasePointer Index )
{
	//This catches a bug where an extra element is added when
	//+[] is called with an index >= to the size.
	if( *Index >= *Length() )
	{
		return (*this)[Index];
	}

	unsigned int RealIndex = DetermineRealIndex(*Index);
	mList.insert( mList.begin() + RealIndex, CreateVariable( SS_BASE_ARGS_DEFAULTS, 0 ) );
	return mList[RealIndex];
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 List::Length
 NOTES: Returns the length of the list.
*/
VariableBasePointer List::Length() const{
	return CreateVariable( SS_BASE_ARGS_DEFAULTS, (NumType)(unsigned long)mList.size() );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 List::RegisterPredefinedVars
 NOTES: 
*/
void List::RegisterPredefinedVars()
{
	bool WasConst = IsConst();
	SetConst( false );

	this->Register( ScopeObjectPointer( new ListLengthVar( LC_Length, *this, true ) ) );

	SetConst( WasConst );
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
VariablePointer List::MakeVariable() const
{
	STRING TheBigString;
	
	unsigned int i;
	for( i = 0; i < mList.size(); i++ )
	{
		TheBigString += mList[i]->GetVariableBasePtr()->GetStringData();		
	}
	
	return CreateVariable( mName, false, true, TheBigString );
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
VariablePointer List::GetVariablePtr(){
	return MakeVariable();
}

const VariablePointer List::GetVariablePtr() const{
	return MakeVariable();
}
