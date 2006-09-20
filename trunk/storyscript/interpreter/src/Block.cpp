/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.
Full license information is included in the file in the top
directory named "license".
*/

#include "Block.hpp"
#include "Variable.hpp"
#include "DLLExport.hpp"
#include "List.hpp"
#include "MagicVars.hpp"
#include "LanguageConstants.hpp"
#include "CreationFuncs.hpp"

using namespace SS;



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
Block::Block()
{}

Block::Block( SS_DECLARE_BASE_ARGS,
			  Interpreter& I, const Bookmark& Position, unsigned int ListIndex )
	: Operator( SS_BASE_ARGS ), mBeenSaid(false),
	  mFilePosition(Position), mListIndex(ListIndex), mpI(&I)
{
	RegisterPredefinedVars();
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Block::RegisterPredefinedVars()
{
	bool WasConst = IsConst();
	SetConst( false );

	Register( ScopeObjectPtr( new BoundFlagVar( LC_BeenSaid, true, mBeenSaid ) ) );
	
	//This is dangerous to create here (it break recursion), so instead it get done in each instance.
	//Register( ScopeObjectPtr( CreateGeneric<List>( LC_Input, true, false ) ) );
	
	Register( CreateVariable<Variable>( LC_Output, false, String() ) );

	//The String(TXT("")) is probably not necessary.  I just want to make sure it
	//gets created as a string.  Not that it matters.

	//The nextline.
	Register( CreateGeneric<List>( LC_NextBlock, false ) );

	SetConst( WasConst );
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr Block::Operate( VariableBasePtr In )
{
	//*(GetScopeObject( LC_Input )->CastToVariableBase()) = *In;
	
	mpI->Parse( this->CastToBlock(), false, In );
	
	//Nope, this is not good.  This would allow modifcation of the blocks out var.
	//return GetScopeObject( LC_Output )->CastToVariableBase();	
	
	//Instead we make a copy and return that.
	return CreateVariable<Variable>( LC_Output, true,
			 					     *GetScopeObjectLocal( LC_Output )->CastToVariable() );
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
bool Block::HasBeenSaid() const{
	return mBeenSaid;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Block::SetBeenSaid( bool flag /*=true*/ ){
	mBeenSaid = flag;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
Bookmark Block::GetFilePosition() const
{
	return mFilePosition;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
unsigned int Block::GetListIndex() const
{
	return mListIndex;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Block::AcceptVisitor( ScopeObjectVisitor& V )
{
	V.VisitBlock(this);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BlockPtr Block::CastToBlock(){
	AssertCastingAllowed();
	return boost::dynamic_pointer_cast<Block>( ScopeObjectPtr(mpThis) );
}

const BlockPtr Block::CastToBlock() const{
	AssertCastingAllowed();
	return boost::dynamic_pointer_cast<Block>( ScopeObjectPtr(mpThis) );
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
String& Block::GetOutString()
{
	return GetScopeObjectLocal( LC_Output )->CastToVariable()->GetActualStringData();
}



