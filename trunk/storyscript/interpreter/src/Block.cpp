/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: Block defs.  Blocks are the cornerstone of this language.
*/

#include "Block.hpp"
#include "Variable.hpp"
#include "DLLExport.hpp"
#include "List.hpp"
#include "SpecialVars.hpp"
#include "LanguageConstants.hpp"
#include "CreationFuncs.hpp"

using namespace SS;



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
Block::Block()
{}

Block::Block( const STRING& Name, bool Static, bool Const,
			  const Bookmark& Position, unsigned int ListIndex )
	: Operator(Name, Static, Const), mFilePosition(Position), mListIndex(ListIndex), mBeenSaid(false)
{
	RegisterPredefinedVars();
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Block::RegisterPredefinedVars()
{
	bool WasConst = IsConst();
	SetConst( false );

	Register( ScopeObjectPtr( new BoundFlagVar( LC_BeenSaid, mBeenSaid, true ) ) );
	
	//This is dangerous to create here (it break recursion), so instead it get done in each instance.
	//Register( ScopeObjectPtr( CreateGeneric<List>( LC_Input, true, false ) ) );
	
	Register( CreateVariable<Variable>( LC_Output, true, false, STRING() ) );

	//The STRING(TXT("")) is probably not necessary.  I just want to make sure it
	//gets created as a string.  Not that it matters.

	//The nextline.
	Register( CreateGeneric<List>( LC_NextBlock, true, false ) );

	SetConst( WasConst );
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr Block::Operate( VariableBasePtr In )
{
	//*(GetScopeObject( LC_Input )->CastToVariableBase()) = *In;
	
	Interpreter::Instance().Parse( this->CastToBlock(), false, In );
	
	//Nope, this is not good.  This would allow modifcation of the blocks out var.
	//return GetScopeObject( LC_Output )->CastToVariableBase();	
	
	//Instead we make a copy and return that.
	return CreateVariable<Variable>( LC_Output, false, true,
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
STRING& Block::GetOutString()
{
	return GetScopeObjectLocal( LC_Output )->CastToVariable()->GetActualStringData();
}



