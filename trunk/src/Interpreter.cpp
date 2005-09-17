/*
Interpreter.cpp
----------------------

Created: 30:8:2004   13:28
*/

#include "Interpreter.hpp"
#include "Interface.hpp"
#include "HelperFuncs.hpp"
#include "ParserAnomaly.hpp"
#include "Block.hpp"
#include "Character.hpp"
#include "List.hpp"
#include "Variable.hpp"
#include "SpecialVars.hpp"
#include "DLLExport.hpp"
#include "LanguageConstants.hpp"
#include "CreationFuncs.hpp"

//Standard Lib
#include "Slib-Common.hpp"
#include "Slib-Time.hpp"
#include "Slib-List.hpp"
#include "Slib-Math.hpp"



#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <memory>
#include <map>




using namespace SS;





boost::shared_ptr<Interpreter> Interpreter::mpInstance;



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~
// Interpreter::Instance
// NOTES: Retrieves the single instance of Interpreter.
//
Interpreter& Interpreter::Instance(){
	if( !mpInstance ) mpInstance.reset( new Interpreter );
	
	return *(mpInstance.get());	
}


//~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Interpreter::Interpreter
// NOTES: ctor
//
Interpreter::Interpreter()
: mVerboseOutput(false)
{
	InitConstants();
	RegisterSpecials();
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Interpreter::~Interpreter
NOTES: destructor
*/
Interpreter::~Interpreter()
{
}



/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Interpreter::SetInterface
NOTES: Sets the interface.
*/
void Interpreter::SetInterface( Interface& SomeInterface )
{
	mpInterface = &SomeInterface;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Interpreter::GetInterface
 NOTES: Retrieve a reference the the interface.
*/
Interface& Interpreter::GetInterface()
{
	if( !mpInterface ){
		ThrowParserAnomaly( TXT("Interface requested before being set."
							" This means there is a serious bug in the interpreter."),
							ANOMALY_PANIC );
	}
	else return *mpInterface;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Interpreter::RegisterSpecials
 NOTES: Registers all the unique special variables that only have one instance
		which is in the global scope
*/
void Interpreter::RegisterSpecials()
{
	//The last two values are just bullshit.
	mpEndBlock = CreateBlock( LC_EndBlock, true, true, Bookmark(), 0 ); 
	mGlobalScope.Register( ScopeObjectPointer( mpEndBlock ) );

	//SLib-Common gets special treatment.  It gets auto-imported.
	ScopePointer pCommonScope( new SS::SLib::Common );
	mGlobalScope.Register( pCommonScope );
	mGlobalScope.Import( pCommonScope );
	
	//I'm goig to import List also.  Those seem like pretty common functions
	ScopePointer pListScope( new SS::SLib::List );
	mGlobalScope.Register( pListScope );
	mGlobalScope.Import( pListScope );

	mGlobalScope.Register( ScopeObjectPointer( new SS::SLib::Time ) );
	mGlobalScope.Register( ScopeObjectPointer( new SS::SLib::Math ) );
	
	mGlobalScope.Register( ScopeObjectPointer( new BoundFlagVar  ( TXT("verbose"), mVerboseOutput, true, true ) ) );
	mGlobalScope.Register( ScopeObjectPointer( new BoundFlagVar  ( TXT("strict_lists"), gUsingStrictLists, true, true ) ) );
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Interpreter::GetCurrentScope
 NOTES: Returns a pointer the current scope.
*/
ScopePointer Interpreter::GetCurrentScope()
{
	return mpCurrentScope;
}

/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Interpreter::GetCurrentPos
 NOTES: Returns a Bookmark to the current position.
*/
Bookmark Interpreter::GetCurrentPos()
{
	AssertFileOpen();
	Bookmark BM( mpCurrentFile->GetFileName(),
				 mpCurrentFile->GetPos(),
				 mpCurrentFile->GetLineNumber(),
				 mpCurrentScope,
				 mpCurrentStaticScope );
	
	return BM;
}

void Interpreter::SetPos( Bookmark& NewPos )
{
	GetFile( NewPos );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Interpreter::GetFirstBlock
 NOTES: Returns the first block.  Often used by the Interface if no starting
		block is set.
*/
BlockPointer Interpreter::GetFirstBlock()
{
	AssertFileOpen();

    if( mBlockOrder.size() > 0 ) return mBlockOrder[0];
	else
	{
		ThrowParserAnomaly( TXT("No blocks found in file."), ANOMALY_NOBLOCKS );
	}
}




/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Interpreter::Verbose
 NOTES: true turns on verbose output.  False turns it off.  
		No value just toggles back and forth.
*/
void Interpreter::SetVerbose( bool flag /*=true*/ )
{
	mVerboseOutput = flag;
}

//And also

bool Interpreter::IsVerbose() const{
	return mVerboseOutput;
}



/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Interpreter::LoadLevel
 NOTES: This is only for loading new files.  USE GetFile INSTEAD!!
*/
void Interpreter::LoadFile( const STRING& FileName )
{
	ReaderSourceFilePtr pNewFile( new ReaderSourceFile );

	pNewFile->Open( FileName );
	mFiles[FileName] = pNewFile;


	ScopePointer pNewScope( CreateObject<Scope>( 
			MakeScopeNameFromFileName( pNewFile->GetFileName() ) ) );
	
	mGlobalScope.Register( ScopeObjectPointer( pNewScope ) );
	mpCurrentScope = pNewScope;

	mpCurrentFile = pNewFile;

	try{
		Parse(); //Position should be 0,0
	}
	catch( ParserAnomaly E )
	{
		TackOnScriptInfo( E );
		throw E;
	}
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Interpreter::GetFile
 NOTES:  Call this whenever you need to go to a bookmark.
*/
ReaderSourceFile& Interpreter::GetFile( const Bookmark& Pos )
{
	if( Pos.IsVoid() ){
		ThrowParserAnomaly( TXT("Attempted to open a empty file name. "
					"Most likely this is due to a bug in the interpreter."),
					ANOMALY_BADFILE );
	}

	if( mFiles.find( Pos.FileName ) == mFiles.end() )	{
		LoadFile( Pos.FileName );
	}

	ReaderSourceFilePtr pFile = mFiles[Pos.FileName];
	pFile->GotoPos( Pos.Position );

	mpCurrentFile = pFile;

	if( Pos.CurrentScope ){
		mpCurrentScope = Pos.CurrentScope;
	}
	else{
		mpCurrentScope = mGlobalScope.GetScopeObject( 
				MakeScopeNameFromFileName(pFile->GetFileName()) )->GetScopePtr();
	}

	if( Pos.CurrentStaticScope ){
		mpCurrentStaticScope = Pos.CurrentStaticScope;
	}
	else{
		mpCurrentStaticScope = mpCurrentScope;
	}

	return *pFile;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Interpreter::Open
 NOTES: Closes the previous file and opens up a new one.  
*/
void Interpreter::Open( const SS::STRING& FileName )
{
	AssertAttachedInterface();
	Close();
	LoadFile( FileName );	
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Interpreter::Close
 NOTES: Closes all open files.  Purges all registered variables.
*/
void Interpreter::Close()
{
	mBlockOrder.clear();
	mFiles.clear();
	mpCurrentScope.reset();
	mGlobalScope.Clear();
}




/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Interpreter::Parse
 NOTES: Parses the currently opened  (usually a header) to find characters and
	    variables to add to the scope.  It ignores everything else.

		First two versions just parses the block specified.  The third version
		Parses everything (not including blocks).
*/
void Interpreter::Parse( const STRING& BlockName )
{
	//Attempt to find the block
	ScopeObjectPointer pBlockHopeful = GetScopeObject( BlockName );

	//This will throw if it is indeed not a Block.
	BlockPointer pBlock = pBlockHopeful->GetBlockPtr();
    
	return Parse( pBlock );
}




void Interpreter::Parse( BlockPointer pBlock, bool SayBlock /*=true*/ )
{
	try{
	if( mVerboseOutput )
	{
		STRING tmp = TXT("Trying to parse block \"");
		tmp += pBlock->GetFullName();
		tmp += TXT("\" at ");
		tmp += boost::lexical_cast<STRING>(pBlock->GetFilePosition().Position);
		tmp += TXT("\n");
		mpInterface->LogMessage( tmp );
	}

	Bookmark OldPos = GetCurrentPos();
    
	//This is needed for any kind of recurssion to be possible.
	if( pBlock == mpCurrentStaticScope && mpCurrentScope != mpCurrentStaticScope)
	{
        mpCurrentStaticScope->UnImport( mpCurrentScope );
	}


	//Clear all non-static variables
	pBlock->Clear();

	Bookmark Pos = pBlock->GetFilePosition();
	
	//This is how the magical instance system works.
	//Every time a block gets executed it creates this temporary instance
	//scope, which all non-statics get created on
	ScopePointer pInstance = CreateObject<Scope>( TXT("") );
	pBlock->Import( pInstance );

	Pos.CurrentStaticScope = pBlock;
	Pos.CurrentScope = pInstance;


	if( pBlock->HasBeenSaid() ) 	Parse( Pos, false, true );
	else                            Parse( Pos, false, false );

	//Now the instance gets destroyed
	pBlock->UnImport( pInstance );
	pInstance.reset();

	//This should also reset the scopes
	SetPos( OldPos );

	//ReImport the old instance
	if( pBlock == mpCurrentStaticScope && mpCurrentScope != mpCurrentStaticScope ){
		mpCurrentStaticScope->Import( mpCurrentScope );
	}

	if( !SayBlock ) return;

	//... and Say the block
	mpInterface->SayBlock( pBlock );
	pBlock->SetBeenSaid();

	


	//Now figure out what block to say next
	ListPointer pNextLine = pBlock->GetScopeObject( LC_NextBlock )->GetListPtr();

	//Create a list of a actual choices.  Throw out bad values.
	const ListType& OrigChoices = pNextLine->GetInternalList();
	std::vector<BlockPointer> GoodChoices;
	
    unsigned int i;
	for( i = 0; i < OrigChoices.size(); i ++ )
	{
		GoodChoices.push_back( GetScopeObject( OrigChoices[i]->GetStringData() )->GetBlockPtr() );
	}

    if( GoodChoices.size() != 0 )
	{
		//TODO: Discard non-player choices (maybe?)
		if( GoodChoices.size() > 1 )
		{
			BlockPointer pChoice = GoodChoices[ mpInterface->PresentChoice( GoodChoices ) ];
			if( pChoice->GetFullName() == mpEndBlock->GetFullName() ) return;
			else Parse( pChoice );
		}
		else if( GoodChoices[0]->GetFullName() == mpEndBlock->GetFullName() ) return;
		else Parse( GoodChoices[0] );
	}
	else if( pBlock->GetListIndex() < mBlockOrder.size() - 1 )
	{
		Parse( mBlockOrder[pBlock->GetListIndex() + 1] );
	}
	//else Nothing left to say I guess.

	}
	catch( ParserAnomaly E )
	{
		TackOnScriptInfo( E );
		throw E;
	}
}



void Interpreter::Parse( Bookmark Pos /*Bookmark()*/,
						 bool OneStatement /*= false*/,
						 bool IgnoreStatic /*= true*/ )
{
	if( mVerboseOutput ) mpInterface->LogMessage( TXT("PARSING...\n") );

	if( Pos.IsVoid() ) Pos = GetCurrentPos();	
	ReaderSourceFile& TheFile = GetFile( Pos );
	
	

	Word TempWord;
	unsigned int BracketCount = 0;
	WordType LastWordType = WORDTYPE_UNKNOWN;

	//I'm going to put this on the heap to take some of the
	//pressure of the stack during recursion.
	boost::shared_ptr<Expression> pWordBuffer( new Expression );
	Expression& WordBuffer = *pWordBuffer;

	//Expression WordBuffer;

	//This is a shortcut, so when something is determined to be an expression,
	//it will just throw everything up to the ';' into the buffer and let
	//ParseExpression sort it out.
	bool FoundExpression = false;

	//This is for condition expression for if and while statements.
	//It will be set to true when it starts reading the condition for a control
	//statement, and when it gets to the end, it will call ParseIf/ParseWhile,
	//rather than just calling ParseExpression.
	bool FoundCondition = false;

	//I'm gunna need these too..
	bool FoundWhile = false;
	bool FoundIf = false;

	bool AllowElse = false;
	bool ExecuteElse = false;
	bool FoundElse = false;
	
	bool FoundStatic = false;
    	
    while( true )
	{
		
		//-----------------------------------
		//Get the next word

		TempWord = TheFile.GetNextWord();
 		if( TempWord == EOF_WORD ) return;


		//-------------------------------------
		//Figure out what to do with the next word


		//Special Case: This catches the "else <expression>" case in which
		//the else should not be parsed.  
		if( FoundElse && !ExecuteElse &&
			TempWord.Extra != EXTRA_CONTROL_If &&
			TempWord.Extra != EXTRA_CONTROL_While )
		{
			if( TempWord.Extra == EXTRA_BRACKET_Left ) TheFile.PutBackWord();

			FastForwardToNextStatement( TheFile );
			FoundElse = false;
			AllowElse = false;
			continue;
		}


		//'}' BRACKETS
		if( TempWord.Type == WORDTYPE_BRACKET && TempWord.Extra == EXTRA_BRACKET_Right )
		{
			//check for extra } brackets
			if( BracketCount == 0 )
			{
				//Assume that we are parsing a block or a control statement.
				//So just quit from here.
				return;
			}

			BracketCount--;

			if( BracketCount == 0 )
			{
				WordBuffer.clear();
                continue;
			}

		}
		//'{' BRACKETS AND BLOCK DECLARATIONS
		else if( TempWord.Type == WORDTYPE_BRACKET && TempWord.Extra == EXTRA_BRACKET_Left )
		{
			//a pretty paranoid check
			if( BracketCount == ~0 )
			{
				ThrowParserAnomaly( TXT("Either you are a complete dumbass and decided to "
						            "flood your code with brackets, or there is a problem "
						            "with the interpreter"), ANOMALY_BADPUNCTUATION );
			}
			
			if( FoundStatic ){
				FoundStatic = false;
				continue;
			}

			if( FoundCondition )
			{
				if( FoundWhile ){
					ExecuteElse = !ParseWhile( WordBuffer, GetCurrentPos(), false );
					AllowElse = true;
				}
				else if( FoundIf ){
					ExecuteElse = !ParseIf( WordBuffer, GetCurrentPos(), false );
					AllowElse = true;
				}
				else if( FoundElse && ExecuteElse ){
					Parse( GetCurrentPos() );
				}
				//else conditions that will not be parsed
				else BracketCount++;

				FoundCondition = FoundIf = FoundWhile = FoundElse = false;

				if( OneStatement ) return;
				WordBuffer.clear();
				continue;
			}


			BracketCount++;


			//Block Declarations
			if( WordBuffer.size() == 1 && WordBuffer[0].Type == WORDTYPE_IDENTIFIER )
			{
				//Right now, it isn't very strict.  A block can belong to anything.
				//In the future it may be a good idea to force it to belong to a character.

				MakeScopeObject( SCOPEOBJ_BLOCK, WordBuffer[0].String );	
				

				//clear & continue
				if( mVerboseOutput )
				{
					STRING tmp = TXT("Registered a new block: \'");
					tmp += WordBuffer[0].String;
					tmp += TXT("\'\n");
					mpInterface->LogMessage( tmp );
				}

				FoundExpression = false;
				WordBuffer.clear();
				continue;			
			}
		}
		//DONT PARSE INSIDE BLOCKS
		else if( BracketCount != 0 )
		{
			continue;
		}
		//CLEAR ON TERMINALS (';')
		else if( TempWord.Type == WORDTYPE_TERMINAL )
		{
			if( FoundExpression == true ||
				//Single element expression check
				(WordBuffer.size() == 1 &&
				( WordBuffer[0].Type == WORDTYPE_IDENTIFIER ||
				WordBuffer[0].IsLiteral() )))
			{
				
				//Expressions doesn't know what to do with static, so I'll just take it out
				if( WordBuffer[0].Extra == EXTRA_CONTROL_Static ){
					WordBuffer.erase( 0 );
					WordBuffer.SetStatic( true );
				}
				
				
				//to allow "else <expression>" syntax (as opposed to "else do <expression>")
				if( FoundElse )
				{
					FoundElse = false;
					if( !ExecuteElse )
					{
						if( OneStatement ) return;
						WordBuffer.clear();
						continue;
					}
				}

				if( mVerboseOutput )
				{
					STRING tmp = TXT("Expression Result: ");
					tmp += WordBuffer.Evaluate()->GetVariablePtr()->GetStringData();
					tmp += TXT("\n");
					mpInterface->LogMessage( tmp );
				}
				else
				{
					WordBuffer.Evaluate();
				}

				FoundExpression = false;
			}

			if( OneStatement ) return;
			WordBuffer.clear();
			continue;
		}
		/*
		//SKIP OVER STATIC STATEMENTS
		else if( FoundStatic && IgnoreStatic ){
			continue;
		}	
		*/
		//CONTROL STATEMENTS
		else if( TempWord.Type == WORDTYPE_CONTROL )
		{
			if( TempWord.Extra == EXTRA_CONTROL_Static ){
				if( !IgnoreStatic ){
					FoundStatic = true;
					WordBuffer.SetStatic();
				}
				else FastForwardToNextStatement( TheFile );
				continue;				
			}
			
			if( TempWord.Extra == EXTRA_CONTROL_Do )
			{
				if( FoundWhile ){
					ExecuteElse = !ParseWhile( WordBuffer, GetCurrentPos(), true );
					AllowElse = true;
				}
				else if( FoundIf ){
					ExecuteElse = !ParseIf( WordBuffer, GetCurrentPos(), true );
					AllowElse = true;
				}
				else if( FoundElse && ExecuteElse ){
					Parse( GetCurrentPos(), true );
				}

                FoundCondition = FoundIf = FoundWhile = FoundElse = false;

				if( OneStatement ) return;
				WordBuffer.clear();
				continue;
			}


			if( FoundIf || FoundWhile )
			{
                ThrowParserAnomaly( TXT("You can't have an 'if' or 'while' in the "
									"condition of another 'if' or 'while'"), ANOMALY_BADGRAMMAR );
			}

			if( TempWord.Extra == EXTRA_CONTROL_If ){
				//For "else if" syntax.
				if( FoundElse )
				{
					FoundElse = false;
					AllowElse = true;

					if( ExecuteElse ) FoundIf = true;
					else
					{
						FastForwardToNextStatement( TheFile );
						FoundIf = false;
						continue;
					}
				}
				else FoundIf = true;
			}
			else if( TempWord.Extra == EXTRA_CONTROL_While ){
				//For "else while" syntax.
				if( FoundElse )
				{
					FoundElse = false;
					AllowElse = true;

					if( ExecuteElse )FoundWhile = true;
					else
					{
						FastForwardToNextStatement( TheFile );
						FoundWhile = false;
						continue;
					}					
				}
				else FoundWhile = true;
			}
			else if( TempWord.Extra == EXTRA_CONTROL_Else  ){
				//Disallow "else else"
				if( FoundElse )
				{
					ThrowParserAnomaly( TXT("Cannot have an else statement inside another else statement."), ANOMALY_BADGRAMMAR );
				}

				if( AllowElse )
				{
					AllowElse = false;
					FoundElse = true;
					
					//NOTE: Whether or not to skip this else will be handled later
				}
				else{
					ThrowParserAnomaly( TXT("Found an \'else\' without a matching \'if\' or \'while\'"), ANOMALY_BADGRAMMAR );
				}
			}
			
			FoundCondition = true;

			//Don't bother copying this to WordBuffer
			continue;
		}// END CONTROL WORDS
		
		
		//IF NOTHING ELSE, IT MUST BE PART OF AN EXPRESSION
		else if( FoundExpression == true )
		{
			WordBuffer.push_back( TempWord );
			continue;
		}
		else
		{
            FoundExpression = true;
		}


		//Add it on to the stack
		WordBuffer.push_back( TempWord );

		
	}

}




/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 interpreter::ParseIf
 NOTES: Parses if's.  Pass in the condition and a bookmark to the
		beginning of the body, and it will do the rest.

		It returns true if the body gets executed, and false otherwise.
		(this is for the benifit of else)
*/
bool Interpreter::ParseIf( const Expression& Condition, const Bookmark& Body, bool OneStatement /*= false*/ )
{
	bool WasParsed = false;

	if( Condition.Evaluate()->GetVariablePtr()->GetBoolData() == true )
	{
		Parse( Body, OneStatement );

		WasParsed = true;
	}
	else if( OneStatement )
	{
		//Fast forward to the end of the statement.
		ReaderSourceFile& FileRef = GetFile( Body );
		Word TempWord;
		while( TempWord.Type != WORDTYPE_TERMINAL ) TempWord = FileRef.GetNextWord();
	}
	else
	{
		//Fast forward to the end of the body
		ReaderSourceFile& FileRef = GetFile( Body );
		unsigned int BracketCount = 1;
		while( BracketCount )
		{
			Word tmp = FileRef.GetNextWord();
			if     ( tmp.Extra == EXTRA_BRACKET_Left ) BracketCount++;
			else if( tmp.Extra == EXTRA_BRACKET_Right ) BracketCount--;
		}
	}

	return WasParsed;
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Interpreter::ParseWhile
 NOTES: Parses while statements.  Read the notes for Interpreter::ParseIf, if
		you still have questions.
*/
bool Interpreter::ParseWhile( const Expression& Condition, const Bookmark& Body, bool OneStatement /*= false*/ )
{
	bool WasParsed = false;

	while( Condition.Evaluate()->GetVariablePtr()->GetBoolData() == true )
	{
		Parse( Body, OneStatement );

		WasParsed = true;
	}

	if( !WasParsed && OneStatement )
	{
		//Fast forward to the end of the statement.
		ReaderSourceFile& FileRef = GetFile( Body );
		Word TempWord;
		while( TempWord.Type != WORDTYPE_TERMINAL ) TempWord = FileRef.GetNextWord();
	}
	else if( !WasParsed )
	{

		//Fast forward to the end of the body
		ReaderSourceFile& FileRef = GetFile( Body );
		unsigned int BracketCount = 1;
		while( BracketCount )
		{
			Word tmp = FileRef.GetNextWord();
			if     ( tmp.Extra == EXTRA_BRACKET_Left ) BracketCount++;
			else if( tmp.Extra == EXTRA_BRACKET_Right ) BracketCount--;
		}
	}

	return WasParsed;
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Interpreter::BreakOffFirstID
 NOTES: In a ID such as "Charlie: Hello", it will remove the "Charlie: "
		and return "Charlie".  It does nothing if the word is not an ID at all.
*/
Word Interpreter::BreakOffFirstID( Word& W )
{
	Word TempWord;

	if( W.Type != WORDTYPE_IDENTIFIER )
	{
		TempWord.Type = WORDTYPE_UNKNOWN;
		return TempWord;
	}

	TempWord.Type = WORDTYPE_IDENTIFIER;

	while( !W.String.empty() && (IsAlpha(W.String[0]) || IsNumber(W.String[0])) )
	{
		TempWord.String += W.String[0];
		W.String.erase(0,1);
	}

	//remove the excess fat
	while( !W.String.empty() && !(IsAlpha(W.String[0]) || IsNumber(W.String[0])) )
	{
		W.String.erase(0,1);
	}

	return TempWord;
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Interpreter::BreakOffLastID
 NOTES: See "BreakOffFirstID" this its opposite.
*/
Word Interpreter::BreakOffLastID( Word& W )
{
	Word TempWord;

	if( W.Type != WORDTYPE_IDENTIFIER )
	{
		TempWord.Type = WORDTYPE_UNKNOWN;
		return TempWord;
	}

	TempWord.Type = WORDTYPE_IDENTIFIER;

	while( !W.String.empty() && 
		   (IsAlpha( W.String[W.String.length()-1] ) || IsNumber( W.String[W.String.length()-1] )) )
	{
		TempWord.String.insert(0, 1, W.String[W.String.length()-1]);
		W.String.erase( W.String.length()-1, 1 );
	}

	//remove the excess fat
	while( !W.String.empty() &&
		   !(IsAlpha( W.String[W.String.length()-1] ) || IsNumber( W.String[W.String.length()-1] )) )
	{
		W.String.erase( W.String.length()-1, 1 );
	}

	return TempWord;
}



/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 NOTES: This is the universal function to register new ScopeObjects.  This is a more
        intelligent solution than just calling mpCurrentScope.Register.  This will
		figure out where the object really belongs.
*/
ScopeObjectPointer Interpreter::MakeScopeObject( ScopeObjectType Type, const STRING& S,
								   bool Static /*= false*/, bool Const /*= false*/ )
 {
	ScopeObjectPointer pNewObj;

	Word TempWord( S, WORDTYPE_IDENTIFIER );
	STRING Name = BreakOffLastID( TempWord ).String;
	STRING& ScopeName = TempWord.String;

	switch( Type )
	{
	case SCOPEOBJ_BLOCK:
		{
		BlockPointer pTempBlock( CreateBlock( Name, Static, Const,
								 GetCurrentPos(), (BlockIndex)mBlockOrder.size() ) );
		mBlockOrder.push_back( pTempBlock );
		pNewObj = (ScopeObjectPointer)pTempBlock ;
		}
		break;
	case SCOPEOBJ_VARIABLE:
		pNewObj = CreateVariable( Name, Static, Const, 0 );
		break;
	case SCOPEOBJ_CHARACTER:
		pNewObj = CreateObject<Character>( Name, Static, Const );
		break;
	case SCOPEOBJ_SCOPE:
		pNewObj = CreateObject<Scope>( Name, Static, Const );
		break;
	case SCOPEOBJ_LIST:
		pNewObj = CreateObject<List>( Name, Static, Const );
		break;
	default:
		SS::STRING tmp = TXT("Tried to register an object with an unknown type named: \'");
		tmp += S;
		tmp += TXT("\'.");
		ThrowParserAnomaly( tmp, ANOMALY_UNKNOWNTYPE );

	}

	if( ScopeName.length() == 0 )
	{
		if( Static && mpCurrentStaticScope ){
			mpCurrentStaticScope->Register( pNewObj );
		}
		else mpCurrentScope->Register( pNewObj );
	}
	else
	{
		//Note that because of this, if a variable is declared outside the scope it
		//belongs to (eg. 'var Donkey::Foo;'), it is automatically static.
		//This is the only behavior that really makes sense.
		
		pNewObj->SetStatic();
		GetScopeObject( ScopeName )->GetScopePtr()->Register( pNewObj );
	}

	return pNewObj;
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Interpreter::GetScopeObject
 NOTES: Sister function to RegisterScopeObject.  This will figure out the correct
        scope to look in, and find the object you are looking for.
        
        WARNING: Don't confuse this with Scope::GetScopeObject which only
        searches that scope and any scopes imported.
        
        This one searches in all the correct places taking into account the current
        scope.
*/
ScopeObjectPointer Interpreter::GetScopeObject( const STRING& Name )
{
	//Try the local scope first

	ScopePointer pPotentialScope = mpCurrentScope;
	ScopeObjectPointer pObject;

	while( true )
	{
		pObject = pPotentialScope->GetScopeObject_NoThrow( Name );

		if( pObject ) return pObject;
		else if( pPotentialScope->GetParent().get() )
		{
			pPotentialScope = pPotentialScope->GetParent();
		}
		else break;
	}

	//Try again with the current static scope
	if( mpCurrentStaticScope && mpCurrentStaticScope != mpCurrentScope )
	{
		pPotentialScope = mpCurrentStaticScope;

		while( true )
		{
			pObject = pPotentialScope->GetScopeObject_NoThrow( Name );

			if( pObject ) return pObject;
			else if( pPotentialScope->GetParent().get() )
			{
				pPotentialScope = pPotentialScope->GetParent();
			}
			else break;
		}
	}

	//Last chance!  Check the current file scope.  
	//This is already checked if mpCurrentScope is a child of the current file,
	//but not if it mpCurrentScope belongs to another scope that was imported.
	STRING ScopeName( MakeScopeNameFromFileName( mpCurrentFile->GetFileName() ) );
	
	pObject = mGlobalScope.GetScopeObject( ScopeName )->GetScopePtr()->GetScopeObject_NoThrow( Name );
	if( pObject ) return pObject;


	//Nothing
	STRING tmp = TXT("Cannot find an indentifier by the name \'");
	tmp += Name;
	tmp += TXT("\'.  Check your spelling.");
	ThrowParserAnomaly( tmp, ANOMALY_IDNOTFOUND );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: These are all shortcuts for the above.  Mainly just for used by
 		external software.
*/
ScopePointer Interpreter::GetScope( const STRING& Name ){
	return GetScopeObject( Name )->GetScopePtr();
}

VariablePointer Interpreter::GetVariable( const STRING& Name ){
	return GetScopeObject( Name )->GetVariablePtr();
}

BlockPointer Interpreter::GetBlock( const STRING& Name ){
	return GetScopeObject( Name )->GetBlockPtr();
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Interpreter::TackOnScriptInfo
 NOTES: Adds information about the script currently being parsed
		to the ParserAnomaly.
*/
void Interpreter::TackOnScriptInfo( ParserAnomaly& E )
{
	AssertFileOpen();

	E.ScriptFile = mpCurrentFile->GetFileName();
	E.ScriptLine = mpCurrentFile->GetLineNumber();
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Interpreter::AssertFileOpen
 NOTES: Tests if the file is open and throws if it is not.
*/
void Interpreter::AssertFileOpen()
{
	if( !mpCurrentFile ){
		ThrowParserAnomaly( TXT("Cannot perform an operation due to no file being open. "
								"This is probably due to a bug in the interpreter."), ANOMALY_PANIC );
	}
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: This is called whenever we need to ensure that a valid interface is
 		attached.
*/
void Interpreter::AssertAttachedInterface()
{
	if( !mpInterface ){
		ThrowParserAnomaly( TXT("No interface attached to Interpreter!"), ANOMALY_PANIC );
	}
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Interpreter::FastForwardToNextStatement
 NOTES: Simply skips over all the next words in the current file up
		to (and including) the next terminal (;) it finds and stops.
*/
void Interpreter::FastForwardToNextStatement( ReaderSourceFile& TheFile )
{
	Word TempWord;

	for( TempWord = TheFile.GetNextWord();
		 TempWord.Type != WORDTYPE_TERMINAL;
		 TempWord = TheFile.GetNextWord() )
	{
		//Override the search for the ';' and instead break on
		//a complete bracket set.
		if( TempWord.Extra == EXTRA_BRACKET_Left )
		{
			unsigned long BracketCount = 1;

			while( true )
			{
				TempWord = TheFile.GetNextWord();

				if( TempWord.Extra == EXTRA_BRACKET_Left ) BracketCount++;
				else if( TempWord.Extra == EXTRA_BRACKET_Right )
				{
					if( BracketCount == 0 ){
						ThrowParserAnomaly( TXT("Found \'}\' without a matching \'{\'."), ANOMALY_BADGRAMMAR );
					}
					else BracketCount--;

					if( BracketCount == 0 ) return;
				}
			}
		}
	}
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Give it the name of a scope, and it will import it into the current
 		scope.
*/
void Interpreter::ImportIntoCurrentScope( const STRING& Name )
{
	if( mVerboseOutput ){
		mpInterface->LogMessage( STRING(TXT("Importing \'")) + Name + STRING(TXT("\'.\n")) );
	}
	
	mpCurrentScope->Import( GetScope( Name ) );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Give it the name of the file and it will import that file into
 		the current scope, loading and parsing the file whenever necessary.
*/
void Interpreter::ImportFileIntoCurrentScope( const STRING& FileName )
{
	if( mVerboseOutput ){
		mpInterface->LogMessage( STRING(TXT("Importing \'")) + FileName + STRING(TXT("\'.\n")) );
	}
	
	Bookmark OldPos = Interpreter::Instance().GetCurrentPos();
					
	Bookmark ImportedFile( FileName, 0, 0 );
	ReaderSourceFile& TheFile = Interpreter::Instance().GetFile( ImportedFile ); 
	
	ScopePointer pTmpObj = GetScope( MakeScopeNameFromFileName( TheFile.GetFileName() ) );

	//Return the file and position to its previous settings
	GetFile( OldPos );

	mpCurrentScope->Import( pTmpObj );
}
