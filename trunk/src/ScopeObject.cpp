/*
ScopeObject.cpp
----------------------

Created: 16:9:2004   15:15
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
	ZeroVars();
	mpThisFallback.reset( this, null_deleter() );
	
	//This lets Get____Ptr functions return something even when they aren't registered.
	mpThis = mpThisFallback;
}


ScopeObject::ScopeObject( const STRING& Name,
						  bool Static /*= false*/, bool Const /*= false*/ )
{
	ZeroVars();
	mName = Name;
	mStatic = Static;
	mConst = Const;
	mpThisFallback.reset( this, null_deleter() );

	mpThis = mpThisFallback;
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
ScopePointer ScopeObject::GetParent() const
{
	return ScopePointer( mpParent, null_deleter() );
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
		ScopeObjectPointer pTemp = (*mpParent)[GetName()];
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
		garbage collection.  Better yet, you should just use CreateObject.
*/
void ScopeObject::SetSharedPtr( const ScopeObjectPointer& pY ){
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

//ScopeObjectPointer
ScopeObjectPointer ScopeObject::GetScopeObjectPtr(){
	return ScopeObjectPointer(mpThis);
}

const ScopeObjectPointer ScopeObject::GetScopeObjectPtr() const{
	return ScopeObjectPointer(mpThis);
}

//ScopePointer
ScopePointer ScopeObject::GetScopePtr(){
	SS::STRING tmp = TXT("Cannot convert the object \"");
	tmp += mName;
	tmp += TXT("\" from a ScopeObject to a Scope.");
	ThrowParserAnomaly( tmp, ANOMALY_NOCONVERSION );
}

const ScopePointer ScopeObject::GetScopePtr() const{
	SS::STRING tmp = TXT("Cannot convert the object \"");
	tmp += mName;
	tmp += TXT("\" from a ScopeObject to a Scope.");
	ThrowParserAnomaly( tmp, ANOMALY_NOCONVERSION );
}


VariableBasePointer ScopeObject::GetVariableBasePtr(){
	return VariableBasePointer( GetVariablePtr() );
}

const VariableBasePointer ScopeObject::GetVariableBasePtr() const{
	return VariableBasePointer( GetVariableBasePtr() );
}


//VariablePointer
VariablePointer ScopeObject::GetVariablePtr(){
	return CreateVariable( GetFullName(), false, true, GetFullName() );
}

const VariablePointer ScopeObject::GetVariablePtr() const{
	return CreateVariable( GetFullName(), false, true, GetFullName() );
}

//BlockPointer
BlockPointer ScopeObject::GetBlockPtr(){
	SS::STRING tmp = TXT("Cannot convert the object \"");
	tmp += mName;
	tmp += TXT("\" from a ScopeObject to a Block.");
	ThrowParserAnomaly( tmp, ANOMALY_NOCONVERSION );
}


const BlockPointer ScopeObject::GetBlockPtr() const{
	SS::STRING tmp = TXT("Cannot convert the object \"");
	tmp += mName;
	tmp += TXT("\" from a ScopeObject to a Block.");
	ThrowParserAnomaly( tmp, ANOMALY_NOCONVERSION );
}


//VarListPointer
ListPointer ScopeObject::GetListPtr(){
	SS::STRING tmp = TXT("Cannot convert the object \"");
	tmp += mName;
	tmp += TXT("\" from a ScopeObject to a List.");
	ThrowParserAnomaly( tmp, ANOMALY_NOCONVERSION );
}

const ListPointer ScopeObject::GetListPtr() const{
	SS::STRING tmp = TXT("Cannot convert the object \"");
	tmp += mName;
	tmp += TXT("\" from a ScopeObject to a List.");
	ThrowParserAnomaly( tmp, ANOMALY_NOCONVERSION );
}



//OperatorPointer
OperatorPointer ScopeObject::GetOperatorPtr(){
	SS::STRING tmp = TXT("Cannot convert the object \"");
	tmp += mName;
	tmp += TXT("\" from a ScopeObject to an Operator. This probably means you left an operator out somewhere.");
	ThrowParserAnomaly( tmp, ANOMALY_NOCONVERSION );
}


const OperatorPointer ScopeObject::GetOperatorPtr() const{
	SS::STRING tmp = TXT("Cannot convert the object \"");
	tmp += mName;
	tmp += TXT("\" from a ScopeObject to an Operator. This probably means you left an operator out somewhere.");
	ThrowParserAnomaly( tmp, ANOMALY_NOCONVERSION );
}



