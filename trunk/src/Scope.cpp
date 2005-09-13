
#include "Scope.hpp"
#include "Variable.hpp"
#include "Block.hpp"
#include "Types.hpp"
#include "HelperFuncs.hpp"
#include "LanguageConstants.hpp"
#include "SpecialVars.hpp"

using namespace SS;


//~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Scope::AcceptVisitor
// NOTES: ... 
//
void Scope::AcceptVisitor( ScopeObjectVisitor& V )
{
	V.VisitScope(this);
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Scope::Scope
 NOTES: 
*/
Scope::Scope()
{
	RegisterPredefinedVars();
}

Scope::Scope( const STRING& Name,
			  bool Static /*= false*/, bool Const /*= false*/ )
: ScopeObject( Name, Static, Const )
{
	RegisterPredefinedVars();
}



/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Scope::RegisterPredefinedVars
 NOTES: Registers language defined variable.  This should only be called from the
		constructors.
*/
void Scope::RegisterPredefinedVars()
{
	//TThis check is very, very important.
	//Without it there will be an infinite loop that will cause a very quick
	//and very hard to track down stack overflow.
	//Unnamed variables and other __name__ variables don't have this value which
	//breaks the cycle.
	static bool FirstRun = true;
	static std::vector<STRING> BanList;

	if( FirstRun ){
		BanList.push_back( LC_Name );
		BanList.push_back( LC_FullName );
		BanList.push_back( LC_UniqueID );
		FirstRun = false;
	}
	
	unsigned int i;
	for( i = 0; i < BanList.size(); i++ ){
		if( mName == BanList[i] ) return;
	}
	
	if( mName.empty() ) return;

	bool WasConst = mConst;
	mConst = false;

	Register( ScopeObjectPointer( new BoundStringVar( LC_Name, mName, true, true ) ) );
	Register( ScopeObjectPointer( new FullNameVar( LC_FullName, *this, true, true ) ) );
	Register( ScopeObjectPointer( new BoundNumVar( LC_UniqueID, mUniqueID, true, false ) ) );
	
	mConst = WasConst;
}





//~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Scope::operator[]
// NOTES: A shared_ptr to the 
//
ScopeObjectPointer Scope::operator[]( const STRING& i )
{
	return GetScopeObject( i );
}


//~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Scope::Register
// NOTES: Adds a ScopeObject into the Scope.
//
void Scope::Register( ScopeObjectPointer pNewScopeObject )
{
	AssertNonConst();
	
	//This get incremented on every new object that gets registered.
	static NumType NextID( 1 );

	
	if( Exists( pNewScopeObject->GetName() ) )
	{
		STRING Temp = TXT("An object called \'");
		Temp += pNewScopeObject->GetName();
		Temp += TXT("\' already exists in scope \'");	
		Temp += this->GetFullName();
		Temp += TXT("\'.");

		ThrowParserAnomaly( Temp, ANOMALY_ALREADYEXISTS );
	}
	
	
		

	//Unregister just in case
	pNewScopeObject->UnRegister();
	
	//Put it on the list!
	mList[pNewScopeObject->GetName()] = pNewScopeObject;
	
	/*
	if( !pNewScopeObject->mpUniqueID ){
		pNewScopeObject->mpUniqueID.reset( new Variable( LC_UniqueID, NumType(SS_INITIAL_UID), true, true ) );
	}
	*/
	

	pNewScopeObject->mUniqueID = NextID;
	pNewScopeObject->mpParent = this;
	pNewScopeObject->mpThis = pNewScopeObject;
	
	NextID += 1;
}



//~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Scope::UnRegister
// NOTES: Removes a ScopeObject from the Scope list.
//
void Scope::UnRegister( const SS::STRING& ObjName )
{
	AssertNonConst();
	
	ScopeListType::iterator i = mList.find( ObjName );
	
	if( i == mList.end() )
	{
		STRING tmp = TXT("Cannot delete object \'");
		tmp += ObjName;
		tmp += TXT("\'.  No such object exists.  This is probably caused by a bug in the interpreter.");
		ThrowParserAnomaly( tmp, ANOMALY_PANIC );		
	}
	
	i->second->mUniqueID = SS_INITIAL_UID;
	i->second->mpParent = 0;
	
	mList.erase( i );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Scope::Clear
 NOTES: Unregisters all non-static objects.
*/
void Scope::Clear()
{
	ScopeListType::iterator i = mList.begin();;

	while( i != mList.end() ){
		if( !(*i).second->IsStatic() )  mList.erase( i );
		else i++;
	}
}



/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Scope::Exists
 NOTES: Checks if an identifier exists in the scope.
*/
bool Scope::Exists( const STRING& ID )
{
	//Unnamed objects are ignored by this
	if( ID.length() == 0 ) return false;
	
	if( mList.find( ID ) != mList.end() ) return true;
	
	
	//Search imported Scopes
	unsigned int i;
	for( i = 0; i < mImportedScopes.size(); i++ ){
		if( mImportedScopes[i]->Exists( ID ) ) return true;
	}

	return false;
}



/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Scope::GetNextBlock
 NOTES: Returns the next block object in its scope after index.
		If there is none, it will return an empty pointer.
*/
BlockPointer Scope::GetNextBlock( BlockPointer pBlock )
{
	if( pBlock->mpParent != this )
	{
		ThrowParserAnomaly( TXT("Some kind of strange scoping bug ocoured."
								"Please contact the author."), ANOMALY_PANIC );
	}
		
	//We will now iterate through every object looking for the next highest
	//block.  That is, the lowest ID block that is > pBlock->mpUniqueID.
	BlockPointer PotentialNextBlock;
	ScopeListType::iterator i;
	TypeCheckVisitor TypeChecker;
	
	for( i = mList.begin(); i != mList.end(); i++ )
	{
		(*i).second->AcceptVisitor( TypeChecker );
		if( TypeChecker.ReturnType() == SCOPEOBJ_BLOCK )
		{
			BlockPointer tmp( (*i).second->GetBlockPtr() );
			
				// tmp's id is higher than pBlock'ss
			if( tmp->mUniqueID > pBlock->mUniqueID &&
				//and PotentialNextBlock is null
				(!PotentialNextBlock || 
				//or tmp's id is lower than PotentialNextBlock's
				tmp->mUniqueID < PotentialNextBlock->mUniqueID ) )
			{
				PotentialNextBlock = tmp;
			}
			
		}
	}

	//Found nothing
	return BlockPointer();
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 GetScopeObject
 NOTES: Tries to find the scope object and return a pointer to it.  Throws a
		ParserAnomaly if it doesn't exists.  Note that you can use long names with
		this. (eg. Bedroom:Bobby:Hello).  (Doesn't work with whitespace?)

		The nothrow version will return an empty pointer if it can't find the object.
*/

ScopeObjectPointer Scope::GetScopeObject_NoThrow( const STRING& Identifier )
{
	//Extract the first part of the identifier (maybe there just is one part).
	STRING FirstPart;
	STRING RemainingPart = Identifier;

	FirstPart = BreakOffFirstID( RemainingPart );


	//If the first part is empty, and the remaining part is not
	//then it must have started with a :, and therefore be in the global scope.
	if( FirstPart.empty() && !RemainingPart.empty() )
	{
		return GetGlobalScope().GetScopeObject( RemainingPart );
	}
	
	ScopeListType::iterator i;
	
	if( (i = mList.find(FirstPart)) != mList.end() )
	{
		if( !RemainingPart.empty() ){
			return (*i).second->GetScopePtr()->GetScopeObject_NoThrow( RemainingPart );
		}
		else return (*i).second;
	}
	
	
	//Check the imported scopes
	const ScopeObjectPointer NULL_SO_PTR;
	ScopeObjectPointer pPotentialObj;
		
	unsigned int j;
	for( j = 0; j < mImportedScopes.size(); j++ )
	{
		if( (pPotentialObj = mImportedScopes[j]->GetScopeObject_NoThrow( FirstPart ))
			!= NULL_SO_PTR )
		{
			if( !RemainingPart.empty() )
			{
                return pPotentialObj->GetScopePtr()->GetScopeObject_NoThrow( RemainingPart );
			}
			else return pPotentialObj;
		}
	}
	
	//Nothing found! 
	return NULL_SO_PTR;	
}


ScopeObjectPointer Scope::GetScopeObject( const STRING& Identifer )
{
	ScopeObjectPointer pReturnValue;
	if( (pReturnValue = GetScopeObject_NoThrow( Identifer )) )
	{
		return pReturnValue;
	}
	else 
	{
		//Nothing found
		STRING Temp = TXT("Cannot find any object named \'");
		Temp += Identifer;
		Temp += TXT("\'.");
		ThrowParserAnomaly( Temp, ANOMALY_IDNOTFOUND );
	}
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Scope::Import
 NOTES: Moves everything from one scope into another.  Or simulates that
		anyway.
*/
void Scope::Import( ScopePointer pScope )
{
	AssertNonConst();

	//Make sure this scope isn't already imported
	unsigned int i;
	for( i = 0; i < mImportedScopes.size(); i++ ){
		if( pScope == mImportedScopes[i] )
		{
			STRING tmp = TXT("The scope \'");
			tmp += pScope->GetFullName();
			tmp += TXT("\' has already been imported into the scope \'");
			tmp += this->GetFullName();
			tmp += TXT("\'.");
			ThrowParserAnomaly( tmp, ANOMALY_ALREADYEXISTS );
		}
	}
	
	mImportedScopes.push_back( pScope );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: UnImports a scope that has been previously imported.  This will throw
		an exception if the scope hasn't been imported.
*/
void Scope::UnImport( ScopePointer pScope )
{
    AssertNonConst();

	unsigned int i;
	for( i = 0; i < mImportedScopes.size(); i++ )
	{
		if( pScope == mImportedScopes[i] )
		{
            mImportedScopes.erase( mImportedScopes.begin() + i );
			return;
		}
	}

	//The scope doesn't exist
	ThrowParserAnomaly( TXT("Tried to un-import a scope that wasn't imported"
						"in the first place.  Probably due to a bug in the interpreter"),
						ANOMALY_PANIC );
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 GetScopePtr
 NOTES: Overloaded to return this rather than throwing.
*/
ScopePointer Scope::GetScopePtr()
{
	return boost::dynamic_pointer_cast<Scope>( ScopeObjectPointer(mpThis) );
}

const ScopePointer Scope::GetScopePtr() const
{
	return boost::dynamic_pointer_cast<Scope>( ScopeObjectPointer(mpThis) );
}



/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Scope::GetGlobalScope
 NOTES: Finds the global scope and returns a pointer to it. 
 
 		Note that this is depricated in favor of using Interpreter::Instance()
*/
Scope& Scope::GetGlobalScope()
{
	if( mpParent ) return mpParent->GetGlobalScope();
	else return *this;
}



