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



/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Block::Block
NOTES: Mr. Ctor 
*/
Block::Block( const STRING& Name, bool Static, bool Const,
			  const Bookmark& Position, unsigned int ListIndex )
	: Operator(Name, Static, Const), mFilePosition(Position), mListIndex(ListIndex), mBeenSaid(false)
{
	RegisterPredefinedVars();
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Block::RegisterPredefinedVars
NOTES: Registers language defined variables.  Should only be called from constructors.
*/
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


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~
// Block::Operate
// NOTES: Blocks can also be treated as operators (thats why they derive from
//		  Operator).  When Operate is called, it sets the input variable to 
//		  whatever the input is, and then it then Parses itself and returns line.
//
VariableBasePtr Block::Operate( VariableBasePtr In )
{
	//*(GetScopeObject( LC_Input )->CastToVariableBase()) = *In;
	
	Interpreter::Instance().Parse( this->CastToBlock(), false, In );
	
	//Nope, this is not good.  This would allow modifcation of the blocks out var.
	//return GetScopeObject( LC_Output )->CastToVariableBase();	
	
	//Instead we make a copy and return that.
	return CreateVariable<Variable>( LC_Output, false, true,
			 					     *GetScopeObject( LC_Output )->CastToVariable() );
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Block::HasBeenSaid
NOTES: Returns true if the block has been sayed once already.
*/
bool Block::HasBeenSaid() const{
	return mBeenSaid;
}

/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Block::SetBeenSaid
NOTES: Sets a flag saying that the block has been said.
*/
void Block::SetBeenSaid( bool flag /*=true*/ ){
	mBeenSaid = flag;
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Block::GetFilePosition
NOTES: Returns the position of the file it resides in.
*/
Bookmark Block::GetFilePosition() const
{
	return mFilePosition;
}

/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Block::GetListIndex
NOTES: Returns the index used with Interpreter::mBlockOrder.
*/
unsigned int Block::GetListIndex() const
{
	return mListIndex;
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Block::AddNextLine
NOTES: Adds another nextline option.
*/
void Block::AddNextLine( Block* pBlock )
{
	//No questions asked.  Which means you better damn well 
	//check if pBlock is null before accessing it.
	mNextLine.push_back(pBlock);
}


//~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Block::AcceptVisitor
// NOTES: ...
//
void Block::AcceptVisitor( ScopeObjectVisitor& V )
{
	V.VisitBlock(this);
}


//~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Block::Get/SetLine
// NOTES: Returns the line that belongs to the block.
//
const STRING& Block::GetLine() const
{
	return mLine;
}

void Block::SetLine( const STRING& NewLine )
{
	mLine = NewLine;
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Block::GetBlockPtr
 NOTES: Overloaded to return itself.
*/
BlockPtr Block::CastToBlock(){
	AssertCastingAllowed();
	return boost::dynamic_pointer_cast<Block>( ScopeObjectPtr(mpThis) );
}

const BlockPtr Block::CastToBlock() const{
	AssertCastingAllowed();
	return boost::dynamic_pointer_cast<Block>( ScopeObjectPtr(mpThis) );
}




