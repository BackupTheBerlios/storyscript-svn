/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: Contains declarations for the Block class.
*/

#if !defined(SS_Block)
#define SS_Block

#include "Operator.hpp"
#include "Interpreter.hpp"
//#include "ScriptFile.hpp"
#include "Bookmark.hpp"
#include "Unicode.hpp"

#include "CreationFuncs.hpp"

namespace SS{


//~~~~~~~CLASS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Block
// NOTES: The internal representation of a dialog block.   
//
class SS_API Block : public Operator
{
protected:
	SS_FRIENDIFY_BLOCK_CREATOR(Block);
	Block();
	Block( const SS::STRING& Name, bool Static, bool Const,
		const Bookmark& Position, BlockIndex ListIndex );

public:
	void AcceptVisitor( ScopeObjectVisitor& );
	VariableBasePtr Operate( VariableBasePtr );
	
	STRING& GetOutString();

	bool HasBeenSaid() const;
	void SetBeenSaid( bool flag = true );
	
	Bookmark GetFilePosition() const;
	unsigned int GetListIndex() const;

	void AddNextLine( Block* );

	//Add/Set SpeechFileName //I will do this later when I actually have to worry about sound.

	const SS::STRING& GetLine() const;
	void          SetLine( const SS::STRING& );

	BlockPtr CastToBlock();
	const BlockPtr CastToBlock() const;

private:

	void RegisterPredefinedVars();
	
	bool mBeenSaid;

	SS::STRING mLine;

	Bookmark mFilePosition;
	BlockIndex mListIndex;

	//I would prefer if this is obtained automatically
	//from the name of the block.
	SS::STRING mSpeechFileName;

	//This has to be a vector to allow branching dialog.
	//NOTE:  A "nextline=none;" is represented by an empty vector.
	std::vector< Block* > mNextLine;
};


} //namespace
#endif
 
