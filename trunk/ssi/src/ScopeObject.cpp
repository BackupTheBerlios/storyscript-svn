/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: Anything that can reside in a scope derives from this (Scope).
*/

#include "ScopeObject.hpp"
#include "Block.hpp"
#include "ParserAnomaly.hpp"
#include "Interpreter.hpp"
#include "HelperFuncs.hpp"
#include "CreationFuncs.hpp"
#include <boost/shared_ptr.hpp>



using namespace SS;

const STRING SS::UNNAMMED;

//~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ScopeObject::ScopeObject
// NOTES: 
//
ScopeObject::ScopeObject()
{
	//Realistically, the default constructor is probabably never called.
	ZeroVars();
}


ScopeObject::ScopeObject( const STRING& Name,
						  bool Static /*= false*/, bool Const /*= false*/ )
{
	ZeroVars();
	mName = Name;
	mStatic = Static;
	mConst = Const;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 ScopeObject::ZeroVars
 NOTES: Sets variables to default/safe positions.  Used by contructors.
*/
void ScopeObject::ZeroVars()
{
	mStatic = false;
	mConst = false;
	mpParent = 0;
	mUniqueID = SS_INITIAL_UID;
}

//~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ScopeObject::~ScopeObject
// NOTES: 
//
ScopeObject::~ScopeObject()
{

}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 ScopeObject::IsStatic
 NOTES: Returns true if the object is static (i.e. it doesn't get deleted
		after the block is finished executing.).
*/
bool ScopeObject::IsStatic() const{
	return mStatic;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 ScopeObject::SetStatic
 NOTES: Sets the object to static mode.  See the above.
*/
void ScopeObject::SetStatic( bool Flag /*= true*/ ){
	mStatic = Flag;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 ScopeObject::IsConst
 NOTES: Returns true if the object is set as constant.  False otherwise.
*/
bool ScopeObject::IsConst() const{
	return mConst;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 ScopeObject::SetConst
 NOTES: Sets the constant flag.
*/
void ScopeObject::SetConst( bool Flag /*= true*/ ){
	mConst = Flag;
}


//~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ScopeObject::AcceptVisitor
// NOTES: For the visitor system.
//
void ScopeObject::AcceptVisitor( ScopeObjectVisitor& V )
{
	V.VisitScopeObject(this);
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
  NOTES: Returns true if the the ScopeObject belongs to a parent and false if
		 it does not.
*/
bool ScopeObject::IsRegistered() const
{
	return mUniqueID != SS_INITIAL_UID;
}

/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ScopeObject::GetParent
 NOTES: Returns a pointer to the parent Scope.
*/
ScopePtr ScopeObject::GetParent() const
{
	return ScopePtr( mpParent, null_deleter() );
}

//~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ScopeObject::SetName
// NOTES: Sets the name of the object.  Note that if the object is registered
//		  it will be UnRegistered and the Registered again.
//
void ScopeObject::SetName( const STRING& NewName )
{
	mName = NewName;

	//If its already registered
	if( IsRegistered() )
	{
		//I'm not using "this" because I'm afraid this
		//will get deleted when the pointer is reset() in UnRegister
		ScopeObjectPtr pTemp = (*mpParent)[GetName()];
		Scope* pParentScope = this->mpParent;
		
		pParentScope->UnRegister( GetName() );
		pParentScope->Register  ( pTemp  );
	}
}


//~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ScopeObject::GetName
// NOTES: Retrieves the simple name of the object.
//
const STRING& ScopeObject::GetName() const
{
	return mName;
}

SS::CHAR* ScopeObject::GetName( SS::CHAR* Buffer, unsigned int BufferSize ) const
{
	return SS::STRCPY( Buffer, mName.c_str(), BufferSize );	
}


//~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ScopeObject::GetFullName
// NOTES: Gets the name of the object including the scope it belongs to.
//		  For instance: "ChildhoodBeach:Alice:SayHi"
//
STRING ScopeObject::GetFullName() const
{
    if( mpParent != NULL )
	{
		STRING Temp = mpParent->GetFullName();
		Temp += ':' ;
		Temp += mName;
		
		return Temp;
	}
	else
	{
		return mName;
	}
}


SS::CHAR* ScopeObject::GetFullName( SS::CHAR* Buffer, unsigned int BufferSize ) const
{
	return SS::STRCPY( Buffer, GetFullName().c_str(), BufferSize );
}




/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ScopeObject::SetThisPtr
 NOTES: Sets internal pointer that points to itself.  This gets done automatically
		when an object gets registered on a scope.  If you don't register it
		somewhere and pass it around a bunch, there will be no proper
		garbage collection.  Better yet, you should just use Creator::CreateObject.
*/
void ScopeObject::SetSharedPtr( const ScopeObjectPtr& pY ){
	mpThis = pY;
}

//~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ScopeObject::UnRegister
// NOTES: Merely calls Scope::UnRegister.  Just a convenience thing.
//
void ScopeObject::UnRegister()
{
	if(mpParent) mpParent->UnRegister( GetName() );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 ScopeObject::AssertNonConst
 NOTES: Throws an error if the object is const.  Call this before functions that
		would modify the object.
*/
void ScopeObject::AssertNonConst()
{
	if( mConst )
	{
		ThrowParserAnomaly( TXT("Tried to modify a constant object, or create an object"
								" in constant scope."), ANOMALY_CONSTASSIGN );
	}
}



/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ScopeObject::Get_______Ptr
 NOTES: Gotta be careful with these.  If you try to convert upward to something
        its not, it will throw.
*/

//ScopeObject
ScopeObjectPtr ScopeObject::CastToScopeObject(){
	return ScopeObjectPtr(mpThis);
}

const ScopeObjectPtr ScopeObject::CastToScopeObject() const{
	return ScopeObjectPtr(mpThis);
}

//Scope
ScopePtr ScopeObject::CastToScope(){
	ThrowBadConversion( TXT("Scope") );
}

const ScopePtr ScopeObject::CastToScope() const{
	ThrowBadConversion( TXT("Scope") );
}

//VariableBase
VariableBasePtr ScopeObject::CastToVariableBase(){
	return VariableBasePtr( this->CastToVariable() );
}

const VariableBasePtr ScopeObject::CastToVariableBase() const{
	return VariableBasePtr( this->CastToVariable() );
}

//Variable (Current default behavior is to return a variable of its full name.)
VariablePtr ScopeObject::CastToVariable(){
	return Creator::CreateVariable( GetFullName(), false, true, GetFullName() );
}

const VariablePtr ScopeObject::CastToVariable() const{
	return Creator::CreateVariable( GetFullName(), false, true, GetFullName() );
}



//BlockPtr
BlockPtr ScopeObject::CastToBlock(){
	ThrowBadConversion( TXT("Block") );
}

const BlockPtr ScopeObject::CastToBlock() const{
	ThrowBadConversion( TXT("Block") );
}



//VarListPtr
ListPtr ScopeObject::CastToList(){
	ThrowBadConversion( TXT("List") );
}

const ListPtr ScopeObject::CastToList() const{
	ThrowBadConversion( TXT("List") );
}



//OperatorPtr
OperatorPtr ScopeObject::CastToOperator(){
	ThrowBadConversion( TXT("Operator"), TXT("(Probably due to a missing operator somewhere.)") );
}

const OperatorPtr ScopeObject::CastToOperator() const{
	ThrowBadConversion( TXT("Operator"), TXT("(Probably due to a missing operator somewhere.)") );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: To easily handle all those messy bad conversion anomalies.
*/
void ScopeObject::ThrowBadConversion( const STRING& Type, const STRING& Addendum /*= STRING()*/ ) const
{
	STRING m = TXT("Cannot cast \'");
	m += mName;
	m += TXT("\' to a ");
	m += Type;
	m += TXT(". ");
	m += Addendum;

	ThrowParserAnomaly( m, ANOMALY_NOCONVERSION );
}


