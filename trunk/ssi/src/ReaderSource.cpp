/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: The abstract base class that is responsible for reading from the script
	from a source and tokenizing it.
*/

#include "ReaderSource.hpp"
#include "HelperFuncs.hpp"
#include "ParserAnomaly.hpp"

#include <queue>

using namespace SS;


const CHAR EOF_CHAR = 3; //ETX Character (I'm not exactly sure what to use here)


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: ctor
*/
ReaderSource::ReaderSource() 
	: mReadStringPos(0),
      mBufferPos(0),
	  mCurrentLine(0)
{
	//First line starts at 0 pos of course.
	mLinePositions.push_back(0);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: dtor
*/
ReaderSource::~ReaderSource()
{
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: This _is_ the reader essentially.  This function is responsible for all
 		the tokenizing.  This is some sloppy-ass code, but it gets the job done.
*/

Word ReaderSource::GetNextWord()
{
	static unsigned int BracketCount = 0;
	static unsigned int NameOfBracketCount = 0;
	
	static std::queue< Word > QueuedWords;
	
	//If we are not at the end, just read off of the buffer.
	if( mBufferPos != mBuffer.size() )
	{
		Word ReturnWord = mBuffer[ mBufferPos ];
		mBufferPos++;
		//UpdateCurrentLine();
		return ReturnWord;		
	}


	if( QueuedWords.size() != 0 )
	{
		Word TempWord = QueuedWords.front();
		QueuedWords.pop();

		return PushWord( TempWord );
	}


	CHAR TempChar;
	STRING TempString;

	FastForward();
	//UpdateCurrentLine();
	
	TempChar = Get();

	if( TempChar == EOF_CHAR )
	{
		return PushWord( EOF_WORD );
	}	

	//TERMINAL
	if( TempChar == LC_Terminal[0] )
	{
		if( BracketCount != 0 ){
			ThrowParserAnomaly( TXT("Found one or more \'[\' without matching \']\'"), ANOMALY_BADPUNCTUATION );
		}
		
		if( NameOfBracketCount != 0 ){
			ThrowParserAnomaly( TXT("Found a \'|\' without an ending \'|\'."), ANOMALY_BADPUNCTUATION );
		}

		return PushWord( Word( WORDTYPE_TERMINAL, EXTRA_TERMINAL ) ); 

	}
	//BRACKET OPERATORS
	else if( TempChar == '[' )
	{
		BracketCount++;

		QueuedWords.push( Word( WORDTYPE_PARENTHESIS, EXTRA_PARENTHESIS_Left ) );

		return PushWord( Word( WORDTYPE_BINARYOPERATOR, EXTRA_BINOP_ListAccess ) );
	}
	else if( TempChar == ']' )
	{
		if( BracketCount == 0 )
		{
			ThrowParserAnomaly( TXT("\']\' bracket found without coresponding \'[\'"),
								ANOMALY_BADPUNCTUATION );
		}

		BracketCount--;
		return PushWord( Word( WORDTYPE_PARENTHESIS, EXTRA_PARENTHESIS_Right ) );
	}
	//NAME-OF BRACKETS
	else if( TempChar == LC_NameOfBracket[0] )
	{
		if( NameOfBracketCount == 0 )
		{
			NameOfBracketCount++;
			QueuedWords.push( Word( WORDTYPE_PARENTHESIS, EXTRA_PARENTHESIS_Left ) );
			return PushWord( Word( WORDTYPE_PARENTHESIS, EXTRA_PARENTHESIS_Left ) );
		}
		else
		{
			NameOfBracketCount--;
			QueuedWords.push( Word( WORDTYPE_BINARYOPERATOR, EXTRA_BINOP_ScopeResolution ) );
			QueuedWords.push( Word( LC_FullName, WORDTYPE_IDENTIFIER ) );
			QueuedWords.push( Word( WORDTYPE_PARENTHESIS, EXTRA_PARENTHESIS_Right ) );
			return PushWord( Word( WORDTYPE_PARENTHESIS, EXTRA_PARENTHESIS_Right ) );
		}
	}
	//STRING LITERAL
	else if( TempChar == '\"' || TempChar == '`' || TempChar == '\'' )
	{
		CHAR TerminalChar = TempChar;
		
		while( (TempChar = Get()) != EOF_CHAR  )
		{
			if( TempChar == TerminalChar )
			{
				//terminal case for out-strings
				/*
				if( Peek() == TerminalChar )
				{
					Get(); //skip past quote
					FastForward(); //skip past any whitespace
				}
				*/

				//Check for adjacent strings (eg: "Hello " "World" )
				FastForward();
				if( Peek() == TerminalChar )
				{
					Get();
					continue;
				}
				else
				{
					if( TerminalChar == '`' )
					{
						//Output something like: (out = "Foo")
	
						QueuedWords.push( Word( LC_Output, WORDTYPE_IDENTIFIER ) );
						QueuedWords.push( Word( WORDTYPE_BINARYOPERATOR, EXTRA_BINOP_PlusAssign ) );
						QueuedWords.push( Word( TempString, WORDTYPE_STRINGLITERAL ) );
						QueuedWords.push( Word( WORDTYPE_PARENTHESIS, EXTRA_PARENTHESIS_Right ) );
						
						return PushWord( Word( WORDTYPE_PARENTHESIS, EXTRA_PARENTHESIS_Left ) );						
					}
					else if( TerminalChar == '\'' )
					{
						return PushWord( Word( TempString, WORDTYPE_DOCSTRING ) );						
					}
					else return PushWord( Word( TempString, WORDTYPE_STRINGLITERAL ) );
				}
			}
			//Handle backslash character codes
			else if( TempChar == '\\' )
			{
				char CharCode = Get();

				if( CharCode == EOF_CHAR ){
					ThrowParserAnomaly( TXT("Unexpected end of file."), ANOMALY_EOF );
				}
				
				TempString += CharCode;
			}
			else if( IsNewline( TempChar  ) && !IsWhitespace( TempString[ TempString.length() - 1] ) )
			{
				/*
				Behavior of newline in strings: To allow more flexibility in formating
				all newlines and all whitespace after the newline is replaced with a single	space.
				*/
				SkipWhitespace();
				TempString += ' ';
			}
			else TempString += TempChar;
		}

		ThrowParserAnomaly( TXT("Where is the end quote?"), ANOMALY_BADGRAMMAR );
	}
	//BRACKETS
	else if( TempChar == '{' )	return PushWord( Word( WORDTYPE_BRACKET, EXTRA_BRACKET_Left ) );
	else if( TempChar == '}' )  return PushWord( Word( WORDTYPE_BRACKET, EXTRA_BRACKET_Right ) );
	//PARENTHESIS
	else if( TempChar == '(' )
	{
		FastForward();
		if( Peek() == ')' )
		{
			Get();
			return PushWord( Word( WORDTYPE_EMPTYLISTLITERAL, EXTRA_NULL ) );
		}
		else return PushWord( Word( WORDTYPE_PARENTHESIS, EXTRA_PARENTHESIS_Left ) );
	}
	else if( TempChar == ')' )  return PushWord( Word( WORDTYPE_PARENTHESIS, EXTRA_PARENTHESIS_Right ) );
	//FLOAT OR INT LITERAL
	else if( IsNumber(TempChar))
	{
		TempString += TempChar;
		

		while( IsNumber( Peek() ) ) TempString += Get();


		if( Peek() == LC_DecimalPoint[0] ) 
		{
			TempString += Get();
			while( IsNumber( Peek() ) ) TempString += Get();
		}
		
		return PushWord( Word( TempString, WORDTYPE_FLOATLITERAL ) );
	}
	//IDENTIFIER OR KEYWORD, OR SPECIAL CASE
	else if( IsAlpha(TempChar) || TempChar == '_' ) 
	{
		TempString += TempChar;

		//Get all alphabetic characters
		while( IsAlpha( Peek() ) || Peek() == '_' ) TempString += Get();
		
		//CONTROL WORDS
		if( TempString == LC_While )       return PushWord( Word( WORDTYPE_CONTROL, EXTRA_CONTROL_While ) );
		else if( TempString == LC_If )     return PushWord( Word( WORDTYPE_CONTROL, EXTRA_CONTROL_If ) );
		else if( TempString == LC_Else )   return PushWord( Word( WORDTYPE_CONTROL, EXTRA_CONTROL_Else ) );
		else if( TempString == LC_Static ) return PushWord( Word( WORDTYPE_CONTROL, EXTRA_CONTROL_Static ) );
		else if( TempString == LC_Then || TempString == LC_Do ){
			return PushWord( Word( WORDTYPE_CONTROL, EXTRA_CONTROL_Do ) );
		}
		//BOOLEAN LITERALS
		else if( TempString == LC_True )  return PushWord( Word( WORDTYPE_BOOLLITERAL, EXTRA_BOOLLITERAL_True ) );
		else if( TempString == LC_False ) return PushWord( Word( WORDTYPE_BOOLLITERAL, EXTRA_BOOLLITERAL_False ) );
		//UNARY OPERATORS
		else if( IsUnaryOperator( TempString ) )
		{
			//Special check for operators that can be binary or unary (e.g. '-').
			if( IsBinaryOperator( TempString ) ){
				return PushWord( Word( WORDTYPE_AMBIGUOUSOPERATOR, gAmbigOperatorMap[TempString] ) );
			}
			else return PushWord( Word( WORDTYPE_UNARYOPERATOR, gUnaryOperatorMap[TempString] ) );
		}
		//BINAY OPERATORS
		else if( IsBinaryOperator( TempString ) ) return PushWord( Word( WORDTYPE_BINARYOPERATOR, gBinaryOperatorMap[TempString] ) );
		
		//VARIABLE DECLARATORS
		/*
		else if( TempString == LC_Var ) return Word( WORDTYPE_DECLARATOR, EXTRA_DECLARATOR_Var );
		else if( TempString == LC_List ) return Word( WORDTYPE_DECLARATOR, EXTRA_DECLARATOR_List );
		else if( TempString == LC_Character ) return Word( WORDTYPE_DECLARATOR, EXTRA_DECLARATOR_Character );
		//HACK: Notice that a match on LC_Player returns as a character.  This is because I haven't
		//		implemented any player code yet.  Remember to change this when I do.
		else if( TempString == LC_Player ) return Word( WORDTYPE_DECLARATOR, EXTRA_DECLARATOR_Character );
		*/


		//This part determines what belongs to the identifier and what doesn't
		bool IDBeginning = false;
		bool ExpectScopeResOp = false;
		
		//Infinate loop.  This one may be dangerous.  Watch it.
		while( true )
		{
			CHAR TempChar = Get();

			if( ExpectScopeResOp && TempChar != LC_ScopeResolution[0] )
			{
				UnGet();
				break;
			}

			if( IsWhitespace(TempChar) )
			{
				if( !IDBeginning ) ExpectScopeResOp = true;
				continue;                
			}
			else
				if( IsAlpha(TempChar) || TempChar == '_' )
			{
				TempString += TempChar;
				if( IDBeginning ) IDBeginning = false;                
			}
			else
				if( IsNumber( TempChar ) )
			{
				if( !IDBeginning ) TempString += TempChar;
				else
				{
					UnGet();
					break;
				}
			}
			else
				if( TempChar == LC_ScopeResolution[0] )
			{
				TempString += TempChar;
				IDBeginning = true;
				if( ExpectScopeResOp ) ExpectScopeResOp = false;
			}
			else
			{
				UnGet();
				break;
			}
		}
		
		return PushWord( Word(TempString, WORDTYPE_IDENTIFIER) );
	}

	//BINARY OPERATOR
	else if( IsBinaryOperator(TempChar) )
	{
		TempString += TempChar;

		//SSPECIAL CASE: for the bizarre list operators
		if( (TempChar == '+' || TempChar == '-') &&
				   Peek() == '[' )
		{
			Get();
			BracketCount++;

			QueuedWords.push( Word( WORDTYPE_PARENTHESIS, EXTRA_PARENTHESIS_Left ) );

			if( TempChar == '+' ) return PushWord( Word( WORDTYPE_BINARYOPERATOR, EXTRA_BINOP_ListAppend ) );
			else return PushWord( Word( WORDTYPE_BINARYOPERATOR, EXTRA_BINOP_ListRemove ) );
		}

		//Match greedily
		//NOTE: This is not very good greedy matching.  In fact it is very very bad greedy matching.
		while( IsBinaryOperator( TempString + (CHAR)Peek() ) ) TempString += Get();

		if( gBinaryOperatorMap.find( TempString ) != gBinaryOperatorMap.end() ) {

			//Check for ambiguous operators.
			if( IsUnaryOperator( TempString ) ){
				return PushWord( Word( WORDTYPE_AMBIGUOUSOPERATOR, gAmbigOperatorMap[TempString] ) );
			}
			else return PushWord( Word( WORDTYPE_BINARYOPERATOR, gBinaryOperatorMap[TempString] ) );
		}
		else{
			STRING TmpError = TXT("Cannot figure out what type of binary operator \'");
			TmpError += TempString;
			TmpError += TXT("\' is.");
			ThrowParserAnomaly( TmpError, ANOMALY_UNKNOWNOP );
		}
	}
	//UNARY OPERATOR
	else if( IsUnaryOperator( TempChar ) )
	{
		TempString += TempChar;

		while( IsUnaryOperator( TempString + (CHAR)Peek() ) ) TempString += Get();

		if( gUnaryOperatorMap.find( TempString ) != gUnaryOperatorMap.end() ) {
			return PushWord( Word( WORDTYPE_UNARYOPERATOR, gUnaryOperatorMap[TempString] ) );
		}
		else{
			STRING TmpError = TXT("Cannot figure out what type of unary operator \'");
			TmpError += TempString;
			TmpError += TXT("\' is.");
			ThrowParserAnomaly( TmpError, ANOMALY_UNKNOWNOP );
		}
	}
	else //Not even sure if this is possible //It is.
	{
		STRING Temp = TXT("Cannot figure out what kind of word this is.\n  TempChar: ");
		Temp += TempChar;
		Temp += TXT("\nTempString:");
		Temp += TempString;
		Temp += TXT("\n");
		ThrowParserAnomaly( Temp, ANOMALY_UNKNOWNWORD );
	}	
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Puts back the last word returned by GetNextWord.
*/
void ReaderSource::PutBackWord()
{
	mBufferPos--;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Sets the stream to start reading at the given location.  The position
 		is given as words into the stream.  It is not a good idea to set this
 		forward without knowing what lies ahead.
*/
void ReaderSource::GotoPos( ReaderPos Pos )
{
	while( Pos > mBufferPos )
	{
		GetNextWord();
	}
	
	if( Pos < mBufferPos )
	{
		mBufferPos = Pos;
		UpdateCurrentLine();
	}
	
	return;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Sets the position to the start of the given line.
*/
void ReaderSource::GotoLine( unsigned long LineNumber )
{
	if( LineNumber < mLinePositions.size() ){	
		GotoPos( mLinePositions[LineNumber] );
		return;
	}
	
	UpdateCurrentLine();
	
	while( LineNumber > mCurrentLine )
	{
		UpdateCurrentLine();
		GetNextWord();
	}
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Returns the position in how many words into the stream we are.
*/
ReaderPos ReaderSource::GetPos() const
{
	return (ReaderPos)mBufferPos;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Returns the current line number.
*/
unsigned long ReaderSource::GetLineNumber() const
{
	UpdateCurrentLine();
	
	/*
		If we are at the beginning of a new line, we just report it as
		the previous line.  Otherwise it will report the wrong line
		when it throws errors sometimes.
	*/
	if( mLinePositions[mCurrentLine] == mBufferPos )
	{
		unsigned long i = mCurrentLine;
		while( mBufferPos <= mLinePositions[i] && i > 0 ) i--;
		return i+1;
	}
	else return mCurrentLine + 1;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 ScriptFile::SkipWhitespace
 NOTES: Moves the position past any spaces, tabs, or linebreaks.
		Returns true if anything was skipped.
*/
bool ReaderSource::SkipWhitespace()
{
	bool ReturnVal = IsWhitespace( Peek() );

	while( IsWhitespace( Peek() ) ) Get();

	return ReturnVal;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Kind of a vague name.  This reads past any comments or whitespace.
		If it skips anything it return true, if it doesn't it returns false.
		!This is the function where comment types are defined!
*/
bool ReaderSource::FastForward()
{
	bool SkippedChars = false;

	//The almighty infinite loop
	while( true ) 
	{
		CHAR TempChar = Get();
		
		//all line comments
		if( TempChar == '/' &&
				  ( Peek() == '/' || Peek() == '\"' || Peek() == '>' ) )
		{
			while( !IsNewline( Get() ) );
			SkippedChars = true;
		}
		//utility comments
		else if( TempChar == '/' && Peek() == '*' )
		{
			Get(); //skip '*'
			while( !(Get() == '*' && Peek() == '/') );
			Get(); //skip '/'

			SkippedChars = true;
		}
		//scene
		else if( TempChar == '/' && Peek() == '.' )
		{
			Get(); //skip '.'
			while( !(Get() == '.' && Peek() == '/') );
			Get(); //skip '/'

			SkippedChars = true;
		}
		//character comments
		else if( TempChar == '/' && Peek() == '\'' )
		{
			Get(); //skip '\''
			while( !(Get() == '\'' && Peek() == '/') );
			Get(); //skip '/'

			SkippedChars = true;
		}
		else if( IsWhitespace( TempChar ) )
		{
			SkipWhitespace();

			//Even if SkipWhitespace returns false, we still skipped TempChar.
			SkippedChars = true;
		}
		else
		{
			UnGet();
			return SkippedChars;
		}
	}
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Returns one character from the stream and advances the position.
 		Also note that this and Peek do not care what the current position is
 		they will just return the next character on the stream.
*/
const CHAR& ReaderSource::Get()
{
	if( mReadStringPos >= mReadString.size() )
	{
		mReadStringPos = 0;
		
		if( !mPeekString.empty() ){
			mReadString.swap( mPeekString );
			mPeekString.clear();
		}
		else{
			mReadString = GetNextLine();
		}
	}
	
	if( mReadString.empty() ) return EOF_CHAR;
	
	
	const CHAR& R = mReadString[mReadStringPos];
	mReadStringPos++;
	
	if( R == '\n' || R == '\r' ){
		 mLinePositions.push_back( GetPos() );
		 
		 //This is to (hopefully) handle every whack-ass kind of newline correctly.
		 mReadStringPos = mReadString.size();
	}
	
	return R;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Undoes a call to Get.  HOWEVER, it is limited in that the farthest it
 		can go is the begining of the line.
*/
const void ReaderSource::UnGet()
{
	//This is the compensate for the fact that any group of newline
	//characters at the of the string are streated as one newline.
	if( mReadStringPos == mReadString.size() )
	{
		do{
			mReadStringPos--;
		}while( mReadStringPos > 0 && 
			    IsNewline( mReadString[mReadStringPos] ) );
		
		return;
	}
	else if( mReadStringPos != 0 ) mReadStringPos--;	
}
	


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Returns one character from the stream put does NOT advance the position.
*/
const CHAR& ReaderSource::Peek()
{
	if( mReadStringPos >= mReadString.size() )
	{
		if( mPeekString.empty() ) mPeekString = GetNextLine();
		
		if( mPeekString.empty() ) return EOF_CHAR;		
		else return mPeekString[0];		
	}
	else return mReadString[mReadStringPos];
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: This is a little shortcut for use by GetNextWord.  This allows us
 		to write "return PushWord( w )" rather than have two line to push and
 		return.
*/
const Word& ReaderSource::PushWord( const Word& W )
{
	mBuffer.push_back( W );
	
	//There isn't a better place to do this, so...
	mBufferPos++;
	//UpdateCurrentLine();
	
	return mBuffer.back();	
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: After mBufferPos is updated, this function is called to figure out
 		what the current line is.
*/
void ReaderSource::UpdateCurrentLine() const
{
	//Go down a line
	while( mCurrentLine != 0 &&
		   mBufferPos < mLinePositions[mCurrentLine] )
	{
		mCurrentLine--;		
	}
	
	//Go up a line
	while( mLinePositions.size() > mCurrentLine + 1 &&
		   mBufferPos >= mLinePositions[mCurrentLine + 1] )
	{
		mCurrentLine++;
	}	
}
