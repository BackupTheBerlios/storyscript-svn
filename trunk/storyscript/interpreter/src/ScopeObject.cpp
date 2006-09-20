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

const String SS::UNNAMMED;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
ScopeObject::ScopeObject( const String& Name /*= String()*/,
						  bool Const /*= false*/ )
{
	mName = Name;
	mConst = Const;
	mpParent = 0;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void ScopeObject::ZeroVars()
{
	mConst = false;
	mpParent = 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
ScopeObject::~ScopeObject()
{

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
bool ScopeObject::IsConst() const{
	return mConst;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void ScopeObject::SetConst( bool Flag /*= true*/ ){
	mConst = Flag;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void ScopeObject::AcceptVisitor( ScopeObjectVisitor& V )
{
	V.VisitScopeObject(this);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
bool ScopeObject::IsRegistered() const
{
	return mpParent != 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
ScopePtr ScopeObject::GetParent() const
{
	return ScopePtr( mpParent, null_deleter() );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void ScopeObject::SetName( const String& NewName )
{
	mName = NewName;

	//If its already registered
	if( IsRegistered() )
	{
		//I'm not using "this" because I'm afraid this
		//will get deleted when the pointer is reset() in UnRegister
		ScopeObjectPtr pTemp = (*mpParent)[MakeCompoundID(GetName())];
		Scope* pParentScope = this->mpParent;
		
		pParentScope->UnRegister( GetName() );
		pParentScope->Register  ( pTemp  );
	}
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
const String& ScopeObject::GetName() const
{
	return mName;
}

SS::Char* ScopeObject::GetName( SS::Char* Buffer, unsigned int BufferSize ) const
{
	return SS::STRCPY( Buffer, mName.c_str(), BufferSize );	
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
String ScopeObject::GetFullName() const
{
    if( mpParent != NULL )
	{
		String Temp = mpParent->GetFullName();
		Temp += ':' ;
		Temp += mName;
		
		return Temp;
	}
	else
	{
		return mName;
	}
}


SS::Char* ScopeObject::GetFullName( SS::Char* Buffer, unsigned int BufferSize ) const
{
	return SS::STRCPY( Buffer, GetFullName().c_str(), BufferSize );
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void ScopeObject::SetSharedPtr( const ScopeObjectPtr& pY ){
	mpThis = pY;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void ScopeObject::UnRegister()
{
	if(mpParent) mpParent->UnRegister( GetName() );
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void ScopeObject::AssertNonConst() const
{
	if( mConst )
	{
		ThrowParserAnomaly( TXT("Tried to modify a constant object, or create an object"
								" in constant scope."), ANOMALY_CONSTASSIGN );
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void ScopeObject::AssertCastingAllowed() const
{
	if( mpThis.expired() )
	{
		String tmp = TXT("Cannot cast \'");
		tmp += GetFullName();
		tmp += TXT("\'.  Probably a bug in the interpreter.");
		ThrowParserAnomaly( tmp, ANOMALY_NOCONVERSION );
	}
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
//ScopeObject
ScopeObjectPtr ScopeObject::CastToScopeObject(){
	AssertCastingAllowed();
	return ScopeObjectPtr(mpThis);
}

const ScopeObjectPtr ScopeObject::CastToScopeObject() const{
	AssertCastingAllowed();
	return ScopeObjectPtr(mpThis);
}

//Scope
ScopePtr ScopeObject::CastToScope(){
	ThrowBadConversion( TXT("Scope") );
	return ScopePtr();
}

const ScopePtr ScopeObject::CastToScope() const{
	ThrowBadConversion( TXT("Scope") );
	return ScopePtr();
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
	return CreateVariable<Variable>( GetFullName(), true, GetFullName() );
}

const VariablePtr ScopeObject::CastToVariable() const{
	return CreateVariable<Variable>( GetFullName(), true, GetFullName() );
}



//BlockPtr
BlockPtr ScopeObject::CastToBlock(){
	ThrowBadConversion( TXT("Block") );
	return BlockPtr();
}

const BlockPtr ScopeObject::CastToBlock() const{
	ThrowBadConversion( TXT("Block") );
	return BlockPtr();
}



//VarListPtr
ListPtr ScopeObject::CastToList(){
	ThrowBadConversion( TXT("List") );
	return ListPtr();
}

const ListPtr ScopeObject::CastToList() const{
	ThrowBadConversion( TXT("List") );
	return ListPtr();
}



//OperatorPtr
OperatorPtr ScopeObject::CastToOperator(){
	ThrowBadConversion( TXT("Operator"), TXT("(Probably due to a missing operator somewhere.)") );
	return OperatorPtr();
}

const OperatorPtr ScopeObject::CastToOperator() const{
	ThrowBadConversion( TXT("Operator"), TXT("(Probably due to a missing operator somewhere.)") );
	return OperatorPtr();
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void ScopeObject::ThrowBadConversion( const String& Type, const String& Addendum /*= String()*/ ) const
{
	String m = TXT("Cannot cast \'");
	m += mName;
	m += TXT("\' to a ");
	m += Type;
	m += TXT(". ");
	m += Addendum;

	ThrowParserAnomaly( m, ANOMALY_NOCONVERSION );
}


