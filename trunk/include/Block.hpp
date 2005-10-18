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

namespace SS{


typedef unsigned int BlockIndex;


//~~~~~~~CLASS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Block
// NOTES: The internal representation of a dialog block.   
//
class SS_API Block : public Operator
{
public:
	//Block( bool IsEndBlock = false );
	Block( const SS::STRING& Name, bool Static, bool Const,
		   const Bookmark& Position, BlockIndex ListIndex );
	//Block( unsigned int FilePosition, const SS::STRING& Line );
	void AcceptVisitor( ScopeObjectVisitor& );
	
	VariableBasePointer Operate( VariableBasePointer );

	//The following four functions are to get rid of the compiler warnings
	/*
	ScopePointer GetScopePtr() { return Scope::GetScopePtr(); }
	const ScopePointer GetScopePtr() const { return Scope::GetScopePtr(); }

	VariableBasePointer GetVariableBasePtr() { return VariableBase::GetVariableBasePtr(); }
	const VariableBasePointer GetVariableBasePtr() const { return VariableBase::GetVariableBasePtr(); }
	*/
	
	bool HasBeenSaid() const;
	void SetBeenSaid( bool flag = true );
	
	Bookmark GetFilePosition() const;
	unsigned int GetListIndex() const;

	void AddNextLine( Block* );

	//Add/Set SpeechFileName //I will do this later when I actually have to worry about sound.

	const SS::STRING& GetLine() const;
	void          SetLine( const SS::STRING& );

	BlockPointer	   GetBlockPtr();
	const BlockPointer GetBlockPtr() const;

private:
	void RegisterPredefinedVars();
	
	bool mBeenSaid;

	SS::STRING mLine;

	Bookmark mFilePosition;
	unsigned int mListIndex;

	//I would prefer if this is obtained automatically
	//from the name of the block.
	SS::STRING mSpeechFileName;

	//This has to be a vector to allow branching dialog.
	//NOTE:  A "nextline=none;" is represented by an empty vector.
	std::vector< Block* > mNextLine;
};


} //namespace
#endif
 
