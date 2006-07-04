/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.
Full license information is included in the file in the top
directory named "license".
*/

/**
	\file Block.hpp
	\brief Declarations for Block.  
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


class Interpreter;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
/**
	\brief The block object.
	
	Blocks are similar to functions in other languages.  Read about
	them in the storyscript langage manual for details.
*/
class SS_API Block : public Operator
{
protected:
	SS_FRIENDIFY_BLOCK_CREATOR(Block);
	/// Default Constructor
	Block();
	
	/**
		\brief Constructor
		
		For other contructor parameter see ScopeObject::ScopeObject.
		
		\param Position The position of the beginning of the block as obtained
		from ReaderSource::GetPosition
		\param ListIndex The order of the block in relation to others.  (So the
		next block an be auto-called
	*/
	Block( const SS::STRING& Name, bool Static, bool Const,
		Interpreter& I, const Bookmark& Position, BlockIndex ListIndex );

public:
	void AcceptVisitor( ScopeObjectVisitor& );
	
	/**
		\brief Execute the block as a function/operator.
		
		This passes a parameter list to the block and executes its
		code as a function, returning the "out" variable as a result.
		
		\return The result of the executed code.  (This "out" variable.) 	
	*/
	VariableBasePtr Operate( VariableBasePtr Params );
	
	/// Returns the "out" string.
	STRING& GetOutString();
	
	/// Returns true if the block has been "said" or executed at least once.
	bool HasBeenSaid() const;
	
	/// Sets the "been said" flag.
	void SetBeenSaid( bool flag = true );
	
	/**
		\brief Get the block's position.
		
		This usually means the position in the file, but can also be
		an input stream.
		
		\sa Bookmark
		
		\return A Bookmark to the beginning of the block.
	*/
	Bookmark GetFilePosition() const;
	
	/**
		\brief Get the block order.
		
		Returns an index to a list of declared blocks kept by Interpreter.
		This way the interpret can figure out which was the next block declared.
	*/
	unsigned int GetListIndex() const;
	

	//Add/Set SpeechFileName //I will do this later when I actually have to worry about sound.
	
	BlockPtr CastToBlock();
	const BlockPtr CastToBlock() const;

private:
	/// Used by the contructor to initialize private data.
	void RegisterPredefinedVars();
	
	/// The "been said" flag.
	bool mBeenSaid;

	/// The position of the block's begninning.
	Bookmark mFilePosition;
	
	/// The block order.  (See GetListIndex.)
	BlockIndex mListIndex;
	
	Interpreter* mpI;

	//SS::STRING mSpeechFileName;
};


} //namespace
#endif
 
