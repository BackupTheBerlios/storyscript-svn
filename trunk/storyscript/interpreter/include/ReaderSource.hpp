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
#include "DLLExport.hpp"

namespace SS{


//Some typedef pertaining to ReaderSource
typedef unsigned long ReaderPos;
typedef std::vector<Word> WordBuffer;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: Abstract Base Class that defines a source for the interpreter to read
 		from.  This class is also responsible for the reading (tokenizing) of
 		the source.
*/
class SS_API ReaderSource
{
public:
	ReaderSource();
	virtual ~ReaderSource();
	
	//These are equivalent
	const Word& GetNextWord();
	const Word& operator()();
	
	const Word& PutBackWord();
	
	//Position is in how many words into the source.
	void GotoPos ( ReaderPos Pos );
	void GotoLine( unsigned long LineNumber );
	ReaderPos GetPos() const;
	
	unsigned long GetLineNumber() const;
	
	//Not required, but strongly advised that you declare this.
	virtual String GetName() const { return String(); }

private:
	//This _should_ include newline/form-feed chars,
	//and return an empty string to indicate EOF
	virtual String GetNextLine() = 0;
	
	bool SkipWhitespace();
	bool FastForward();
	
	const Char&  Get();
	const Char&  Peek();
	const void   UnGet();
	String mReadString;
	String mPeekString;
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

