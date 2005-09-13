/*
	QuickListSolid.hpp (NOT FULLY TESTED)
	----------------------
	Created: 2004/05/04
	Description: See QuickList.hpp.  This version is more efficient for smaller data types.
*/



#if !defined(DCJ_QuickListSolid)
#define DCJ_QuickListSolid

#include "Anomaly.hpp"

#include <vector>




namespace DCJ{



//~~~~~~~CLASS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// QuickListSolid
// NOTES: A variation on QuickList which allocated an entire node at once.  This
//		  far more efficient for large lists of basic datatypes because it does not
//		  use pointers.
//
template<typename NODEDATA>
class QuickListSolid
{
private:
	unsigned int mListLength;
	const int mArraySize;
	//LinkedList<NODEDATA*> mList;
	std::vector<NODEDATA*> mList;

public:
	QuickListSolid();
	QuickListSolid( int mArraySize );
	QuickListSolid( int mArraySize, NODEDATA& Data );
	~QuickListSolid();

	NODEDATA& operator[](unsigned int index);
	NODEDATA  operator[](unsigned int index) const;
	NODEDATA& AddEndEntry( const NODEDATA& Data );
	bool DeleteEndEntry();
	NODEDATA& GetLastEntry();
	NODEDATA  GetLastEntry() const;
    unsigned int Length() const;
//	int Size() const;

	mutable unsigned int mShortcutIndex;
	mutable NODEDATA* mpShortcut;

	//New!
	QuickListSolid<NODEDATA>& operator+=( NODEDATA& );
	QuickListSolid<NODEDATA>& operator--();
	QuickListSolid<NODEDATA>& operator=( const QuickListSolid<NODEDATA>& L2 );
	QuickListSolid<NODEDATA>& Clear();
	
};
//
/////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////
//  FUNCTION  - Constructors & Destructors - 'nuff said
//	NOTES - 
template<typename NODEDATA>
QuickListSolid<NODEDATA>::QuickListSolid()
	: mArraySize(100), mList(), mListLength(0), mShortcutIndex(~0)
{
	mList.push_back( new NODEDATA[mArraySize] );
	memset( mList[0], 0, sizeof(NODEDATA) * mArraySize );

	//mpShortcut = mList[0];
}


template<typename NODEDATA>
QuickListSolid<NODEDATA>::QuickListSolid( int mArraySize )
	: mArraySize( mArraySize ), mList(), mListLength(0), mShortcutIndex(~0)
{
	mList.push_back( new NODEDATA[mArraySize] );
	memset( mList[0], 0, sizeof(NODEDATA) * mArraySize );

	//mpShortcut = mList[0];
}



template<typename NODEDATA>
QuickListSolid<NODEDATA>::QuickListSolid( int mArraySize, NODEDATA& Data )
	: mArraySize( mArraySize ), mList(), mListLength(0), mShortcutIndex(~0)
{
	mList.push_back( new NODEDATA[mArraySize] );
	memset( mList[0], 0, sizeof(NODEDATA) * mArraySize );

	//mpShortcut = mList[0];


		//NODEDATA* NewData = new NODEDATA;
		//*NewData = *Data;
	mList[0][0] = Data;
	mListLength = 1;
	
}



template<typename NODEDATA>
QuickListSolid<NODEDATA>::~QuickListSolid()
{
	while( DeleteEndEntry() );
	
	delete[] mList[0];
}






/////////////////////////////////////////////////////////////////////////////////////////
//  FUNCTION  - QuickList::operator[] - The main interface for accessing entries.
//	NOTES - 
template<typename NODEDATA> 
NODEDATA& QuickListSolid<NODEDATA>::operator[]( unsigned int index )
{
	if( index >= mListLength )	{
		ThrowDCJAnomaly( TXT("Index is greater than mListLength."), 0 );
	}

	unsigned VectorIndex = index / mArraySize;

	if( VectorIndex != mShortcutIndex )
	{
		mShortcutIndex = VectorIndex;
		mpShortcut = mList[ index / mArraySize ];
	}

	return mpShortcut[ index % mArraySize ];
}


template<typename NODEDATA> 
NODEDATA QuickListSolid<NODEDATA>::operator[]( unsigned int index ) const
{
	if( index >= mListLength )	{
		ThrowDCJAnomaly( TXT("Index is greater than mListLength."), 0 );
	}

	unsigned VectorIndex = index / mArraySize;

	if( VectorIndex != mShortcutIndex )
	{
		mShortcutIndex = VectorIndex;
		mpShortcut = mList[ index / mArraySize ];
	}

	return mpShortcut[ index % mArraySize ];

	
}



/////////////////////////////////////////////////////////////////////////////////////////
//  FUNCTION  - QuickList::AddEndEntry - Adds an entry to the end of the list.
//	NOTES - returns a reference to the newly created data.
template<typename NODEDATA> 
NODEDATA& QuickListSolid<NODEDATA>::AddEndEntry( const NODEDATA& Data )
{
	unsigned int NodeID = mListLength / mArraySize;
	unsigned int ArraryIndex = mListLength % mArraySize;

	if( NodeID >= mList.size() )
	{
		mList.push_back( new NODEDATA[mArraySize] );
		memset( mList[mList.size()-1], 0, sizeof(NODEDATA) * mArraySize );
	}
	

	mList[NodeID][ArraryIndex] = Data;

	mListLength++;
	return GetLastEntry();
}






/////////////////////////////////////////////////////////////////////////////////////////
//  FUNCTION  - QuickList::DeleteEndEntry
//	NOTES - Returns false if mListLength is greater than 0.  True otherwise.
template<typename NODEDATA>
bool QuickListSolid<NODEDATA>::DeleteEndEntry()
{
	if( 0 == mListLength ) return false;

	unsigned int LastArraryIndex = (mListLength-1) % mArraySize;
	//delete mList[mList.size()-1][LastArraryIndex];
	mList[mList.size()-1][LastArraryIndex] = 0;

	//							DONT'T delete the last node.
	if( 0 == LastArraryIndex && mList.size() > 1 )
	{
		delete[] mList[mList.size()-1];
		mList.pop_back();

	}

	mListLength--;
	
	if( mListLength ) return true;
	else return false;
}





/////////////////////////////////////////////////////////////////////////////////////////
//  FUNCTION  - QuickList::GetLastEntry - Returns a reference to the last entry on the list.
//	NOTES - 
template<typename NODEDATA>
inline NODEDATA& QuickListSolid<NODEDATA>::GetLastEntry()
{
	return (*this)[mListLength-1];
}

template<typename NODEDATA>
inline NODEDATA QuickListSolid<NODEDATA>::GetLastEntry() const
{
	return (*this)[mListLength-1];
}



/////////////////////////////////////////////////////////////////////////////////////////
//  FUNCTION  - QuickList::Length() - Returns the length of the list.
//	NOTES - 
template<typename NODEDATA>
inline unsigned int QuickListSolid<NODEDATA>::Length() const
{
	return mListLength;
}



//DEPRICATED
/////////////////////////////////////////////////////////////////////////////////////////
//  FUNCTION  - QuickList::Size() - returns the size of the list in bytes
//	NOTES - Only a VERY rough estimate
/*
template<typename NODEDATA>
int QuickListSolid<NODEDATA>::Size() const
{
	return sizeof(QuickList< NODEDATA >) + sizeof( LinkedList< NODEDATA* > ) + //the actual structurs
		   (mListLength * sizeof(NODEDATA)); //the allocated NODEDATA's
}
*/

/////////////////////////////////////////////////////////////////////////////////////////
//  FUNCTION  - QuickList:Clear
//	NOTES - Removes every entry on the list.
template<typename NODEDATA>
QuickListSolid<NODEDATA>& QuickListSolid<NODEDATA>::Clear()
{
	while( DeleteEndEntry() );
	return *this;
}


/////////////////////////////////////////////////////////////////////////////////////////
//  FUNCTION  - QuickList::operator+= 
//	NOTES - synonym for AddEndEntry
template<typename NODEDATA>
QuickListSolid<NODEDATA>& QuickListSolid<NODEDATA>::operator+=( NODEDATA& Data )
{
	AddEndEntry( Data );
	return *this;
}


/////////////////////////////////////////////////////////////////////////////////////////
//  FUNCTION  - QuickList::operator--
//	NOTES - Synonym for DeleteEndEntry
template<typename NODEDATA>
QuickListSolid<NODEDATA>& QuickListSolid<NODEDATA>::operator--()
{
	DeleteEndEntry();
	return *this;
}


/////////////////////////////////////////////////////////////////////////////////////////
//  FUNCTION  - QuickList::operator= 
//	NOTES -  
template<typename NODEDATA>
QuickListSolid<NODEDATA>& QuickListSolid<NODEDATA>::operator=( const QuickListSolid<NODEDATA>& X )
{
	//DEPRICATED.
	/*
	Clear();

    unsigned int i;
	for( i = 0; i < Data.Length(); i++ )
	{
		AddEndEntry( Data[i] );
	}

	return *this;
	*/

	unsigned int i;

	if( this->Length() < X.Length() )
	{
		for( i = 0; i < X.Length(); i++ )
		{
			if( i >= this->Length() ) this->AddEndEntry( X[i] );
			else (*this)[i] = X[i];
		}
	}
	else
	{
		while( this->Length() > X.Length() ) this->DeleteEndEntry();

        for( i = 0; i < X.Length(); ++i ) (*this)[i] = X[i];
	}

	return *this;
}


} //namespace DCJ

#endif //#if !defined(DCJ_QuickList)



