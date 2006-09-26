/*
Copyright (c) 2004-2006 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.
Full license information is included in the file in the top
directory named "license".
*/

/**
	\file ReaderSource.hpp
	\brief Declarations for ReaderSource.  
*/


#if !defined(SS_ReaderSource)
#define SS_ReaderSource

#include "Word.hpp"
#include "Types.hpp"
#include "DLLExport.hpp"

namespace SS{


///Hold a position in words (not characters or bytes!)
typedef unsigned long ReaderPos;

///A buffer to hold words
typedef std::vector<Word> WordBuffer;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
/**
	\brief An interface for feeding source into the Interpreter
	
	An abstract base class that defines a source for the interpreter to read
	from.  This class is also responsible for the reading (tokenizing) of
	the source.  The user needs only to supply it with text, one line at
	a time.
*/
class SS_API ReaderSource
{
public:
	///Constrctor
	ReaderSource();
	///Destructor
	virtual ~ReaderSource();
	
	/**
		\brief Returns the next word in the stream.
		
		The whole tokenizer is more or less contained in this function.
		
		\return The next word in the stream.
	*/
	const Word& GetNextWord();
	///Returns the next word in the stream. (Exquivalent to GetNextWord)
	const Word& operator()();
	
	///Put the word removed last back onto the stream.
	const Word& PutBackWord();
	
	/**
		\brief Skip to a certain position in the stream.
		
		The position is in words, not characters or bytes, so
		be careful.  It is also typically not a good idea to
		move the position forward without knowing what's there.
		
		\param Pos Position in the stream in words.
	*/ 
	void GotoPos ( ReaderPos Pos );
	
	///Move the stream position to the beginning of the given line.
	void GotoLine( unsigned long LineNumber );
	
	///Return current position in the stream.
	ReaderPos GetPos() const;
	
	///Return the current line number.
	unsigned long GetLineNumber() const;
	
	/**
		\brief Returns the name of the stream.
		
		Streams all have their own scope, so it is important that
		they return some kind of name.  The interpreter will take
		care of mutating it into a proper identifier name.
		
		For instance, the file reader source (ReaderSourceFile)
		returns the file name.
		
		\return The name of the stream.
	*/ 
	virtual String GetName() const { return String(); }

protected:
	//This _should_ include newline/form-feed chars,
	//and return an empty string to indicate EOF
	
	/**
		\brief Returns the next line in the stream.
		
		This is the only function that the user is required to
		implement.  Whatever form the stream takes, this should  return the
		next line.  
		
		Do _not_ strip out newline or form-feed characters.
		
		Return an empty string to indicate the end of the stream.
		
		\return The next line in the stream. 
	*/
	virtual String GetNextLine() = 0;

private:
	/**
		\brief Moves the stream position past any whitespace
		
		\return True if the anything was skipped, false otherwise.
	*/
	bool SkipWhitespace();
	
	/**
		\brief Moves the stream position past any comments or whitespace
		
		\return True if anything was skipped, false otherwise.
	*/
	bool FastForward();
	
	///Returns the next character in the stream.
	const Char&  Get();
	///Returns the next character in the stream without advancing the position.
	const Char&  Peek();
	///Moves the stream position back one character
	const void   UnGet();
	
	///The buffer containing the current line
	String mReadString;
	///A buffer that is sometimes needed to hold the next line.
	String mPeekString;
	///The position (in characters) into the current line.
	size_t mReadStringPos;
	
	/**
		\brief Adds a word to the end up the word buffer.
		
		\param W The word to the add.
		\return A reference to the added word.
	*/		
	const Word& PushWord( const Word& W );
	
	///The buffer representing the word stream.
	WordBuffer mBuffer;
	
	///The current position into the word stream
	size_t mBufferPos;
	
	///When the position is screwed with, this figures out what line we are on
	void UpdateCurrentLine() const;
	
	///Keeps track of the current line
	mutable unsigned long mCurrentLine;
	
	///Keeps track of at what positions newline begin
	std::vector<ReaderPos> mLinePositions;	
};

///A pointer to a ReaderSource
typedef boost::shared_ptr<ReaderSource> ReaderSourcePtr;




} //namespace SS
#endif 

