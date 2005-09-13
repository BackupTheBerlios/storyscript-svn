/*
ScriptFile.cpp
----------------------

Created: 31:3:2005   13:14
*/

#include "ScriptFile.hpp"
#include "ParserAnomaly.hpp"
#include "HelperFuncs.hpp"
#include "LanguageConstants.hpp"
#include <boost/shared_ptr.hpp>
#include <map>


using namespace SS;


//------------EOF_CHAR----------------
const SS::CHAR ScriptFile::EOF_CHAR = 0;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 ScriptFile::ScriptFile
 NOTES: 
*/
ScriptFile::ScriptFile()
: mCurrentLine(0)
{
}

ScriptFile::ScriptFile( const STRING& FileName )
: mCurrentLine(0)
{
	Open( FileName );
}

ScriptFile::ScriptFile( ScriptFile& X )
: mCurrentLine(0)
{
	Bookmark Mark =	X.GetCurrentPos();
	Goto( Mark );
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 ScriptFile::~ScriptFile
 NOTES: 
*/
ScriptFile::~ScriptFile(){
	Close();
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 ScriptFile::Open
 NOTES: Opens a file.  And auto-closes anything that is already open.
*/
void ScriptFile::Open( const STRING& FileName )
{
	if( IsOpen() ) Close();

	mFile.open( NarrowizeString( FileName ).c_str(), std::ios::binary );

	if( !mFile )
	{
		STRING tmp = TXT("Cannot find file \'");
		tmp += FileName;
		tmp += TXT("\'. Check your spelling.");
		ThrowParserAnomaly( tmp, ANOMALY_BADFILE );
	}

    mFileName    = FileName;
	mCurrentLine = 1;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 ScriptFile::Close
 NOTES: Closes a file.  Simple as that.
*/
void ScriptFile::Close()
{
	if( IsOpen() )
	{
		mFile.close();
		mCurrentLine = 0;
		mFileName.clear();
	}
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 ScriptFile::GetCurrentPos
 NOTES: Returns the current position in the file.
*/
Bookmark ScriptFile::GetCurrentPos()
{
	//If this file isn't open you will just get an empty string and two zeros.
	return Bookmark( mFileName, IsOpen() ? (unsigned int)mFile.tellg() : 0U, mCurrentLine );

	//PS. Why don't the STL assholes go through the trouble of making tellg const?
	//Now I have to screw up my code because they can't get their act together.
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 void ScriptFile::Goto
 NOTES: Goes to a stored position.  Closes/opens a file as necessary.
*/
void ScriptFile::Goto( const Bookmark& Mark )
{
	if( !IsOpen() || Mark.FileName != mFileName )
	{
		Close();
		Open( Mark.FileName );
	}
	else if( mFile.eof() ) mFile.clear();

	mFile.seekg( Mark.Position );
	mCurrentLine = Mark.Line;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 ScriptFile::GetFileName
 NOTES: Retrieves the name of the file that is currently open.
*/
const SS::STRING& ScriptFile::GetFileName() const{
	return mFileName;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 ScriptFile::EndOfFile
 NOTES: Returns true if the end of the file has been reached, false otherwise.
*/
bool ScriptFile::EndOfFile() const
{
	if( mFile.eof() ) return true;
	else return false;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 ScriptFile::IsOpen
 NOTES: Returns true if the file is open.  False otherwise.
*/
bool ScriptFile::IsOpen(){
	return mFile.is_open();
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 ScriptFile::SkipWhitespace
 NOTES: Moves the position past any spaces, tabs, or linebreaks.
		Returns true if anything was skipped.
*/
bool ScriptFile::SkipWhitespace()
{
	bool ReturnVal = IsWhitespace( mFile.peek() );

	while( IsWhitespace( get() ) ) 
	{
	}
	
	//Put back the char that was not whitespace.
	mFile.unget();

	return ReturnVal;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 ScriptFile::FastForward
 NOTES: Kind of a vague name.  This reads past any comments or whitespace.
		If it skips anything it return true, if it doesn't it returns false.
*/
bool ScriptFile::FastForward()
{
	bool SkippedChars = false;

	//The almighty infinite loop
	while( true ) 
	{
		CHAR TempChar = get();

		//all line comments
		if( TempChar == '/' &&
			( mFile.peek() == '/' || mFile.peek() == '\"' || mFile.peek() == '>' ) )
		{
			while( !mFile.eof() && !IsNewline( get() ) );
			SkippedChars = true;
		}
		//utility comments
		else if( TempChar == '/' && mFile.peek() == '*' )
		{
			get(); //skip '*'
			while( !mFile.eof() && !(get() == '*' && mFile.peek() == '/') );
			get(); //skip '/'

			SkippedChars = true;
		}
		//scene
		else if( TempChar == '/' && mFile.peek() == '.' )
		{
			get(); //skip '.'
			while( !mFile.eof() && !(get() == '.' && mFile.peek() == '/') );
			get(); //skip '/'

			SkippedChars = true;
		}
		//character comments
		else if( TempChar == '/' && mFile.peek() == '\'' )
		{
			get(); //skip '\''
			while( !mFile.eof() && !(get() == '\'' && mFile.peek() == '/') );
			get(); //skip '/'

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
			mFile.unget();
			return SkippedChars;
		}
	}
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 ScriptFile::MakeScopeName
 NOTES: Mangles the name of the file into a proper identifier name.
*/
STRING ScriptFile::MakeScopeName()
{
	STRING ScopeName;

	unsigned int i;
	for( i = 0; i < mFileName.length(); i++ )
	{
		//Can't start with a number; replace "3Jane" with "_3Jane"
		if( i == 0 && IsNumber( mFileName[0] ) ){
			ScopeName += '_';
			ScopeName += mFileName[0];
		}
		//If the character isn't a number or letter, just stick a "_" in there
		else if( !IsAlpha( mFileName[i] ) && !IsNumber( mFileName[i] ) ){
			ScopeName += '_';
		}
		//Otherwise go ahead any use whats there
		else{
			ScopeName += mFileName[i];
		}
	}
	
	return ScopeName;
}




/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 ScriptFile::InsertWord
 NOTES: Inserts a word.  So that ExtractWord will return it in its next call.
*/
void ScriptFile::InsertWord( const Word& W )
{
	mQueuedWords.push( W );
}




/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 ScriptFile::ExtractWord
 NOTES: This is really important part of this class.  It sort of "pre-parses"
		the file, breaking it down into its core language elements.
*/
Word ScriptFile::ExtractWord()
{
	if( mFile.eof() ) return EOF_WORD;

	static unsigned int BracketCount = 0;
	static unsigned int NameOfBracketCount = 0;


	if( mQueuedWords.size() != 0 )
	{
		Word TempWord = mQueuedWords.front();
		mQueuedWords.pop();

		return TempWord;
	}


	CHAR TempChar;
	STRING TempString;

	FastForward();
	TempChar = get();

	if( TempChar == EOF_CHAR )
	{
		return EOF_WORD;
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

		return Word( WORDTYPE_TERMINAL, EXTRA_TERMINAL ); 

	}
	//BRACKET OPERATORS
	else if( TempChar == '[' )
	{
		BracketCount++;

		mQueuedWords.push( Word( WORDTYPE_PARENTHESIS, EXTRA_PARENTHESIS_Left ) );

		return Word( WORDTYPE_BINARYOPERATOR, EXTRA_BINOP_ListAccess );
	}
	else if( TempChar == ']' )
	{
		if( BracketCount == 0 )
		{
			ThrowParserAnomaly( TXT("\']\' bracket found without coresponding \'[\'"),
								ANOMALY_BADPUNCTUATION );
		}

		BracketCount--;
		return Word( WORDTYPE_PARENTHESIS, EXTRA_PARENTHESIS_Right );
	}
	//NAME-OF BRACKETS
	else if( TempChar == LC_NameOfBracket[0] )
	{
		if( NameOfBracketCount == 0 )
		{
			NameOfBracketCount++;
			mQueuedWords.push( Word( WORDTYPE_PARENTHESIS, EXTRA_PARENTHESIS_Left ) );
			return Word( WORDTYPE_PARENTHESIS, EXTRA_PARENTHESIS_Left );
		}
		else
		{
			NameOfBracketCount--;
			mQueuedWords.push( Word( WORDTYPE_BINARYOPERATOR, EXTRA_BINOP_ScopeResolution ) );
			mQueuedWords.push( Word( LC_FullName, WORDTYPE_IDENTIFIER ) );
			mQueuedWords.push( Word( WORDTYPE_PARENTHESIS, EXTRA_PARENTHESIS_Right ) );
			return Word( WORDTYPE_PARENTHESIS, EXTRA_PARENTHESIS_Right );
		}
	}
	//STRING LITERAL
	else if( TempChar == '\"' || TempChar == '`' )
	{
		CHAR TerminalChar = TempChar;
		
		while( (TempChar = get()) != EOF_CHAR  )
		{
			if( TempChar == TerminalChar )
			{
				//terminal case for out-strings
				if( mFile.peek() == TerminalChar )
				{
					get(); //skip past quote
					FastForward(); //skip past any whitespace
				}

				//Check for adjacent strings (eg: "Hello " "World" )
				FastForward();
				if( mFile.peek() == TerminalChar )
				{
					get();
					continue;
				}
				else
				{
					if( TerminalChar == '`' )
					{
						//Output something like: (out = "Foo")
	
						mQueuedWords.push( Word( LC_Output, WORDTYPE_IDENTIFIER ) );
						mQueuedWords.push( Word( WORDTYPE_BINARYOPERATOR, EXTRA_BINOP_Assign ) );
						mQueuedWords.push( Word( TempString, WORDTYPE_STRINGLITERAL ) );
						mQueuedWords.push( Word( WORDTYPE_PARENTHESIS, EXTRA_PARENTHESIS_Right ) );
						
						return Word( WORDTYPE_PARENTHESIS, EXTRA_PARENTHESIS_Left );						
					}
					else return Word( TempString, WORDTYPE_STRINGLITERAL );
				}
			}
			//Handle backslash character codes
			else if( TempChar == '\\' )
			{
				char CharCode = get();

				if( CharCode == EOF_CHAR ){
					ThrowParserAnomaly( TXT("Unexpected end of file."), ANOMALY_EOF );
				}

				switch( CharCode )
				{
				case 'n':
					TempString += '\n';
					break;

				case '\\':
					TempString += '\\';
					break;

				case '\"':
					TempString += '\"';
					break;

				case '\'':
					TempString += '\'';
					break;

				case '`':
					TempString += '`';
					break;

				default:
					TempString += CharCode;
				}
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
	else if( TempChar == '{' )	return Word( WORDTYPE_BRACKET, EXTRA_BRACKET_Left );
	else if( TempChar == '}' )  return Word( WORDTYPE_BRACKET, EXTRA_BRACKET_Right );
	//PARENTHESIS
	else if( TempChar == '(' )
	{
		FastForward();
		if( mFile.peek() == ')' )
		{
			get();
			return Word( WORDTYPE_EMPTYLISTLITERAL, EXTRA_NULL );
		}
		else return Word( WORDTYPE_PARENTHESIS, EXTRA_PARENTHESIS_Left );
	}
	else if( TempChar == ')' )  return Word( WORDTYPE_PARENTHESIS, EXTRA_PARENTHESIS_Right );
	//FLOAT OR INT LITERAL
	else if( IsNumber(TempChar))
	{
		TempString += TempChar;
		

		while( IsNumber( mFile.peek() ) ) TempString += get();


		if( !mFile.eof() && mFile.peek() == LC_DecimalPoint[0] ) 
		{
			TempString += get();
			while( IsNumber( mFile.peek() ) ) TempString += get();
		}
		
		return Word( TempString, WORDTYPE_FLOATLITERAL );
	}
	//IDENTIFIER OR KEYWORD, OR SPECIAL CASE
	else if( IsAlpha(TempChar) || TempChar == '_' )
	{
		TempString += TempChar;

		//Get all alphabetic characters
		while( IsAlpha( mFile.peek() ) || mFile.peek() == '_' ) TempString += get();

		//CONTROL WORDS
		if( TempString == LC_While )       return Word( WORDTYPE_CONTROL, EXTRA_CONTROL_While );
		else if( TempString == LC_If )     return Word( WORDTYPE_CONTROL, EXTRA_CONTROL_If );
		else if( TempString == LC_Else )   return Word( WORDTYPE_CONTROL, EXTRA_CONTROL_Else );
		else if( TempString == LC_Static ) return Word( WORDTYPE_CONTROL, EXTRA_CONTROL_Static );
		else if( TempString == LC_Then || TempString == LC_Do ){
			return Word( WORDTYPE_CONTROL, EXTRA_CONTROL_Do );
		}
		//BOOLEAN LITERALS
		else if( TempString == LC_True )  return Word( WORDTYPE_BOOLLITERAL, EXTRA_BOOLLITERAL_True );
		else if( TempString == LC_False ) return Word( WORDTYPE_BOOLLITERAL, EXTRA_BOOLLITERAL_False );
		//UNARY OPERATORS
		else if( IsUnaryOperator( TempString ) )
		{
			//Special check for operators that can be binary or unary (e.g. '-').
			if( IsBinaryOperator( TempString ) ){
				return Word( WORDTYPE_AMBIGUOUSOPERATOR, gAmbigOperatorMap[TempString] );
			}
			else return Word( WORDTYPE_UNARYOPERATOR, gUnaryOperatorMap[TempString] );
		}
		//BINAY OPERATORS
		else if( IsBinaryOperator( TempString ) ) return Word( WORDTYPE_BINARYOPERATOR, gBinaryOperatorMap[TempString] );
		
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
		while( !mFile.eof() )
		{
			CHAR TempChar = get();

			if( ExpectScopeResOp && TempChar != LC_ScopeResolution[0] )
			{
				mFile.unget();
				break;
			}

			if( IsWhitespace(TempChar) )
			{
				if( !IDBeginning ) ExpectScopeResOp = true;
				continue;                
			}
			else
			if( IsAlpha(TempChar) )
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
					mFile.unget();
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
				mFile.unget();
				break;
			}
		}

		return Word(TempString, WORDTYPE_IDENTIFIER);
	}

	//BINARY OPERATOR
	else if( IsBinaryOperator(TempChar) )
	{
		TempString += TempChar;


		//Special case for the bizarre list operators
		if( (TempChar == '+' || TempChar == '-') &&
			mFile.peek() == '[' )
		{
			get();
			BracketCount++;

			mQueuedWords.push( Word( WORDTYPE_PARENTHESIS, EXTRA_PARENTHESIS_Left ) );

			if( TempChar == '+' ) return Word( WORDTYPE_BINARYOPERATOR, EXTRA_BINOP_ListAppend );
			else return Word( WORDTYPE_BINARYOPERATOR, EXTRA_BINOP_ListRemove );
		}

		//Match greedily
		//NOTE: This is not very good greedy matching.  In fact it is very very bad greedy matching.
		while( IsBinaryOperator( TempString + (CHAR)mFile.peek() ) ) TempString += get();

		if( gBinaryOperatorMap.find( TempString ) != gBinaryOperatorMap.end() ) {

			//Check for ambiguous operators.
			if( IsUnaryOperator( TempString ) ){
				return Word( WORDTYPE_AMBIGUOUSOPERATOR, gAmbigOperatorMap[TempString] );
			}
			else return Word( WORDTYPE_BINARYOPERATOR, gBinaryOperatorMap[TempString] );
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

		while( IsUnaryOperator( TempString + (CHAR)mFile.peek() ) ) TempString += get();

		if( gUnaryOperatorMap.find( TempString ) != gUnaryOperatorMap.end() ) {
			return Word( WORDTYPE_UNARYOPERATOR, gUnaryOperatorMap[TempString] );
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
 ScriptFile::get
 NOTES: A wrapper for mFile.get().  This one will increment mCurrentLine
		whenever a newline is encountered.
		CALL THIS INSTEAD OF mFile.get()!!!
*/
SS::CHAR ScriptFile::get()
{
	if(mFile.eof())	{
		return EOF_CHAR;
	}

	if( !mFile ){
		
		//This is a bit of a hack.  It tried clearing it and retrying before it throws.
		mFile.clear();

		if( !mFile ){
		ThrowParserAnomaly( TXT("Problem with the file."), ANOMALY_PANIC );
		}
	}

	CHAR TempChar;
	TempChar = mFile.get();

	/* NOTE: It only check for \n because of differences in newlines from
			 platform to platform.  On windows, a newline is \n\r on linux
			 its just \n.  I don't care what those Apple hippies use because
			 I have no current plans to port it to Mac OS.
	*/
	if( TempChar == '\n' /*|| TempChar == '\r'*/ )	{
		mCurrentLine++;
	}

	return TempChar;
}


