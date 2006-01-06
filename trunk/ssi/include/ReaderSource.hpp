/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: The abstract base class that is responsible for reading from the script
	from a source and tokenizing it.
*/

#if !defined(SS_ReaderSource)
#define SS_ReaderSource

#include "Word.hpp"
#include "Types.hpp"

namespace SS{


//Some typedef pertaining to ReaderSource
typedef unsigned long ReaderPos;
typedef std::vector<Word> WordBuffer;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: Abstract Base Class that defines a source for the interpreter to read
 		from.  This class is also responsible for the reading (tokenizing) of
 		the source.
*/
class ReaderSource
{
public:
	ReaderSource();
	virtual ~ReaderSource();
	
	//These are equivalent
	Word GetNextWord();
	Word operator()();
	
	void PutBackWord();
	
	//Position is in how many words into the source.
	void GotoPos ( ReaderPos Pos );
	void GotoLine( unsigned long LineNumber );
	ReaderPos GetPos() const;
	
	unsigned long GetLineNumber() const;
	
	//Not required, but strongly advised that you declare this.
	virtual STRING GetName() const { return STRING(); }

private:
	//This _should_ include newline/formfeed chars,
	//and return an empty string to idicate EOF
	virtual STRING GetNextLine() = 0;
	
	bool SkipWhitespace();
	bool FastForward();
	
	const CHAR&  Get();
	const CHAR&  Peek();
	const void   UnGet();
	STRING mReadString;
	STRING mPeekString;
	size_t mReadStringPos;
	
	const Word& PushWord( const Word& );
	WordBuffer mBuffer;
	size_t mBufferPos;
	
	void UpdateCurrentLine() const;
	mutable unsigned long mCurrentLine;
	std::vector<ReaderPos> mLinePositions;	
};

typedef boost::shared_ptr<ReaderSource> ReaderSourcePtr;




} //namespace SS
#endif 

