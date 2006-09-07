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


const ScopeObjectPtr NULL_SO_PTR;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Scope::AcceptVisitor( ScopeObjectVisitor& V )
{
	V.VisitScope(this);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~

Scope::Scope( SS_DECLARE_BASE_ARGS )
: ScopeObject( SS_BASE_ARGS )
{
	RegisterPredefinedVars();
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Scope::RegisterPredefinedVars()
{
	mNameCreated = mFullNameCreated = mDocStringCreated = false;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
ScopeObjectPtr Scope::operator[]( const CompoundString& i )
{
	return GetScopeObject( i );
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
ScopeObjectPtr Scope::Register( ScopeObjectPtr pNewScopeObject )
{
	AssertNonConst();
	
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
	
	
	pNewScopeObject->mpParent = this;
	pNewScopeObject->mpThis = pNewScopeObject;

	return pNewScopeObject;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
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
	
	i->second->mpParent = 0;
	
	ScopeObjectPtr RetVal = i->second;
	
	mList.erase( i );
	
	return RetVal;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Scope::Clear()
{
	/*
	ScopeListType::iterator i = mList.begin();;

	while( i != mList.end() ){
		if( !(*i).second->IsStatic() )  mList.erase( i );
		else i++;
	}
	*/
	
	mList.clear();
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
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


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
ScopeObjectPtr Scope::GetScopeObject_NoThrow( const CompoundString& Identifier, unsigned long Level /*= 0*/  )
{	
	if( Level >= Identifier.size() ) return NULL_SO_PTR;

	if( Level == 0 && Identifier[0].empty() )
	{
		return GetGlobalScope().GetScopeObject_NoThrow( Identifier, 1 );	
	}
	
	/*
		This object's hook gets called in case the implementer wants
		to spring something into existance or do some other voodoo.
	*/
	ScopeObjectPtr pPotentialObj = GetScopeObjectHook( Identifier[Level] );
	if( pPotentialObj ) return pPotentialObj;
	
	/*
		Ask the map if its seen the id and pass along the rest of it if necessary.
	*/
	ScopeListType::iterator i;
	
	if( (i = mList.find(Identifier[Level])) != mList.end() )
	{
		if( Identifier.size() > Level+1 ){
			return (*i).second->CastToScope()->GetScopeObject_NoThrow( Identifier, Level+1 );
		}
		else return (*i).second;
	}
	
	/*
		No hits yet, lets check the imported scopes.
	*/	
	unsigned int j;
	for( j = 0; j < mImportedScopes.size(); j++ )
	{
		if( (pPotentialObj = mImportedScopes[j]->GetScopeObject_NoThrow( Identifier, Level )) != NULL_SO_PTR )
		{
			return pPotentialObj;
		}
	}
	
	//Nothing found!! Damn!
	return NULL_SO_PTR;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
ScopeObjectPtr Scope::GetScopeObject( const CompoundString& Identifer )
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
		Temp += CollapseCompoundString( Identifer );
		Temp += TXT("\'.");
		ThrowParserAnomaly( Temp, ANOMALY_IDNOTFOUND );
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
ScopeObjectPtr Scope::GetScopeObjectLocal( const STRING& Identifier )
{
	ScopeObjectPtr pReturnValue;
	if( (pReturnValue = GetScopeObjectLocal_NoThrow( Identifier )) )
	{
		return pReturnValue;
	}
	else 
	{
		//Nothing found
		STRING Temp = TXT("Cannot find any object named \'");
		Temp += Identifier;
		Temp += TXT("\'.");
		ThrowParserAnomaly( Temp, ANOMALY_IDNOTFOUND );
	}
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
ScopeObjectPtr Scope::GetScopeObjectLocal_NoThrow( const STRING& Identifier )
{
	/*
		This object's hook gets called in case the implementer wants
		to spring something into existance or do some other voodoo.
	*/
	ScopeObjectPtr pPotentialObj = GetScopeObjectHook( Identifier );
	if( pPotentialObj ) return pPotentialObj;
	
	/*
		Ask the map if its seen the id and pass along the rest of it if necessary.
	*/
	ScopeListType::iterator i;
	
	if( (i = mList.find(Identifier)) != mList.end() ) return (*i).second;
	
	
	/*
		No hits yet, lets check the imported scopes.
	*/	
	unsigned int j;
	for( j = 0; j < mImportedScopes.size(); j++ )
	{
		if( (pPotentialObj = mImportedScopes[j]->GetScopeObjectLocal_NoThrow( Identifier )) != NULL_SO_PTR )
		{
			return pPotentialObj;
		}
	}
	
	//No matches
	return NULL_SO_PTR;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
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


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
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


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
ScopePtr Scope::CastToScope(){
	AssertCastingAllowed();
	return boost::dynamic_pointer_cast<Scope>( ScopeObjectPtr(mpThis) );
}

const ScopePtr Scope::CastToScope() const{
	AssertCastingAllowed();
	return boost::dynamic_pointer_cast<Scope>( ScopeObjectPtr(mpThis) );
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
Scope& Scope::GetGlobalScope()
{
	if( mpParent ) return mpParent->GetGlobalScope();
	else return *this;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
ScopeObjectPtr Scope::GetScopeObjectHook( const STRING& Name )
{
	if( !mNameCreated && Name == LC_Name )
	{
		mNameCreated = true;	
		return Register( ScopeObjectPtr( new BoundStringVar( LC_Name, true, mName ) ) );
	}
	else if( !mFullNameCreated && Name == LC_FullName )
	{
		mFullNameCreated = true;
		return Register( ScopeObjectPtr( new FullNameVar( LC_FullName, true, *this ) ) );
	}
	else if( !mDocStringCreated && Name == LC_Doc )
	{
		mDocStringCreated = true;
		ScopeObjectPtr Tmp = Register( ScopeObjectPtr( CreateVariable<Variable>( LC_Doc, false, STRING() ) ) );
		return Tmp;
	}
	else return ScopeObjectPtr();
	
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
STRING& Scope::GetDocString()
{
	return GetScopeObjectLocal( LC_Doc )->CastToVariable()->GetActualStringData();
}


