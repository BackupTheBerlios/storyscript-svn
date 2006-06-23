/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: The base Scope class.
*/

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
	mNameCreated = mFullNameCreated = mUniqueIDCreated = mDocStringCreated = false;
}





//~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Scope::operator[]
// NOTES: A shared_ptr to the 
//
ScopeObjectPtr Scope::operator[]( const STRING& i )
{
	return GetScopeObject( i );
}


//~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Scope::Register
// NOTES: Adds a ScopeObject into the Scope.
//
ScopeObjectPtr Scope::Register( ScopeObjectPtr pNewScopeObject )
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
	
	return pNewScopeObject;
}



//~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Scope::UnRegister
// NOTES: Removes a ScopeObject from the Scope list.
//
ScopeObjectPtr Scope::UnRegister( const SS::STRING& ObjName )
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
	
	ScopeObjectPtr RetVal = i->second;
	
	mList.erase( i );
	
	return RetVal;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Scope::Clear
 NOTES: Unregisters all non-static objects.
 
 		This function is pretty much considered depricated.  It is a remnant of
 		before I had a real instacing system.
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
BlockPtr Scope::GetNextBlock( BlockPtr pBlock )
{
	if( pBlock->mpParent != this )
	{
		ThrowParserAnomaly( TXT("Some kind of strange scoping bug ocoured."
								"Please contact the author."), ANOMALY_PANIC );
	}
		
	//We will now iterate through every object looking for the next highest
	//block.  That is, the lowest ID block that is > pBlock->mpUniqueID.
	BlockPtr PotentialNextBlock;
	ScopeListType::iterator i;
	TypeCheckVisitor TypeChecker;
	
	for( i = mList.begin(); i != mList.end(); i++ )
	{
		(*i).second->AcceptVisitor( TypeChecker );
		if( TypeChecker.ReturnType() == SCOPEOBJ_BLOCK )
		{
			BlockPtr tmp( (*i).second->CastToBlock() );
			
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
	return BlockPtr();
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 GetScopeObject
 NOTES: Tries to find the scope object and return a pointer to it.  Throws a
		ParserAnomaly if it doesn't exists.  Note that you can use long names with
		this. (eg. Bedroom:Bobby:Hello).  (Doesn't work with whitespace?)

		The nothrow version will return an empty pointer if it can't find the object.
*/

ScopeObjectPtr Scope::GetScopeObject_NoThrow( const STRING& Identifier )
{	
	TokenizedID TokenList;
	SplitUpID( Identifier, TokenList );
	
	if( TokenList[0]->empty() )
	{
		return GetGlobalScope().GetScopeObjectInternal( TokenList, 1 );	
	}
	else
	{
		return GetScopeObjectInternal( TokenList, 0 );
	}
}


ScopeObjectPtr Scope::GetScopeObject( const STRING& Identifer )
{
	ScopeObjectPtr pReturnValue;
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



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: This is where all the actual work is done.
*/
ScopeObjectPtr Scope::GetScopeObjectInternal( const TokenizedID& TokenList, unsigned long Level /*= 0*/ )
{
	const ScopeObjectPtr NULL_SO_PTR;
	if( Level >= TokenList.size() ) return NULL_SO_PTR;
	
	
	/*
		This object's hook gets called in case the implementer wants
		to spring something into existance or do some other voodoo.
	*/
	ScopeObjectPtr pPotentialObj = GetScopeObjectHook( *TokenList[Level] );
	if( pPotentialObj ) return pPotentialObj;
	
	
	/*
		Ask the map if its seen the id and pass along the rest of it if necessary.
	*/
	ScopeListType::iterator i;
	
	if( (i = mList.find(*TokenList[Level])) != mList.end() )
	{
		if( TokenList.size() > Level+1 ){
			return (*i).second->CastToScope()->GetScopeObjectInternal( TokenList, Level+1 );
		}
		else return (*i).second;
	}
	
	
	/*
		No hits yet, lets check the imported scopes.
	*/	
	unsigned int j;
	for( j = 0; j < mImportedScopes.size(); j++ )
	{
		if( (pPotentialObj = mImportedScopes[j]->GetScopeObjectInternal( TokenList, Level )) != NULL_SO_PTR )
		{
			return pPotentialObj;
		}
	}
	
	//Nothing found!! Damn!
	return NULL_SO_PTR;	
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Splits a compound identifier (eg. foo:bar:doc) into seperate identifers.
*/
void Scope::SplitUpID( const STRING& ID, TokenizedID& TokenList )
{
	STRING::const_iterator last_i;
	STRING::const_iterator i;
	for( last_i = i = ID.begin(); i != ID.end(); i++ )
	{
		if( 	*i == LC_ScopeResolution[0] )
		{
			TokenList.push_back( StringPtr( new STRING( last_i, i ) ) );
			
			last_i = i + 1;
			continue;	
		}
	}
	
	//If this wasn't compound at all
	if( TokenList.size() == 0 )
	{
		TokenList.push_back( StringPtr( &ID, null_deleter() ) );
	}
	//Grab the last one.
	else if( last_i != i )
	{
		TokenList.push_back( StringPtr( new STRING( last_i, i ) ) );
	}
}



/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Scope::Import
 NOTES: Moves everything from one scope into another.  Or simulates that
		anyway.
*/
void Scope::Import( ScopePtr pScope )
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
void Scope::UnImport( ScopePtr pScope )
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
ScopePtr Scope::CastToScope(){
	AssertCastingAllowed();
	return boost::dynamic_pointer_cast<Scope>( ScopeObjectPtr(mpThis) );
}

const ScopePtr Scope::CastToScope() const{
	AssertCastingAllowed();
	return boost::dynamic_pointer_cast<Scope>( ScopeObjectPtr(mpThis) );
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


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Intercept identifier lookups to spring object into existance.
*/
ScopeObjectPtr Scope::GetScopeObjectHook( const STRING& Name )
{
	if( !mNameCreated && Name == LC_Name )
	{
		mNameCreated = true;	
		return Register( ScopeObjectPtr( new BoundStringVar( LC_Name, mName, true, true ) ) );
	}
	else if( !mFullNameCreated && Name == LC_FullName )
	{
		mFullNameCreated = true;
		return Register( ScopeObjectPtr( new FullNameVar( LC_FullName, *this, true, true ) ) );
	}
	else if( !mUniqueIDCreated && Name == LC_UniqueID )
	{
		mUniqueIDCreated = true;
		return Register( ScopeObjectPtr( new BoundNumVar( LC_UniqueID, mUniqueID, true, false ) ) );
	}
	else if( !mDocStringCreated && Name == LC_Doc )
	{
		mDocStringCreated = true;
		ScopeObjectPtr Tmp = Register( ScopeObjectPtr( CreateVariable<Variable>( LC_Doc, true, false, STRING() ) ) );
		return Tmp;
	}
	else return ScopeObjectPtr();
	
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Just a handy shortcut for the user.
*/
STRING& Scope::GetDocString()
{
	return GetScopeObject( LC_Doc )->CastToVariable()->GetActualStringData();
}


