/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: Contains definitions for the Interprer class which handles the bulk
	of all the parsing.
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
#include "BuiltInFunctions.hpp"

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







bool& Interpreter::mVerboseOutput = SS::LangOpts::Instance().Verbose;
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
{
	mpGlobalScope = CreateGeneric<Scope>();
	InitConstants();
	RegisterSpecials();
	mStop = false;
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
	mpEndBlock = CreateBlock<Block>( LC_EndBlock, true, true, Bookmark(), 0 ); 
	mpGlobalScope->Register( ScopeObjectPtr( mpEndBlock ) );

	//SLib-Common gets special treatment.  It gets auto-imported.
	ScopePtr pCommonScope( CreateGeneric<SS::SLib::Common>() );
	mpGlobalScope->Register( pCommonScope );
	mpGlobalScope->Import( pCommonScope );
	
	//I'm goig to import List also.  Those seem like pretty common functions
	ScopePtr pListScope( CreateGeneric<SS::SLib::List>() );
	mpGlobalScope->Register( pListScope );
	mpGlobalScope->Import( pListScope );

	mpGlobalScope->Register( ScopeObjectPtr( CreateGeneric<SS::SLib::Time>() ) );
	mpGlobalScope->Register( ScopeObjectPtr( CreateGeneric<SS::SLib::Math>() ) );
	mpGlobalScope->Register( ScopeObjectPtr( CreateGeneric<SS::SLib::LangOpts>() ) );
	
	//Register build in functions
	mpGlobalScope->Register( CreateGeneric<ImportOperator>() );
	mpGlobalScope->Register( CreateGeneric<UnImportOperator>() );
	mpGlobalScope->Register( CreateGeneric<ReturnOperator>() );
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Interpreter::GetCurrentScope
 NOTES: Returns a pointer the current scope.
*/
ScopePtr Interpreter::GetCurrentScope()
{
	return mpCurrentScope;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Returns a pointer the current static scope.
*/
ScopePtr Interpreter::GetCurrentStaticScope()
{
	return mpCurrentStaticScope;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Returns a pointer to the global scope.
*/
ScopePtr Interpreter::GetGlobalScope()
{
	return mpGlobalScope;
}

/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Interpreter::GetCurrentPos
 NOTES: Returns a Bookmark to the current position.
*/
Bookmark Interpreter::GetCurrentPos()
{
	AssertFileOpen();
	Bookmark BM( mpCurrentSource->GetName(),
				 mpCurrentSource->GetPos(),
				 mpCurrentSource->GetLineNumber(),
				 mpCurrentScope,
				 mpCurrentStaticScope );
	
	return BM;
}

void Interpreter::SetPos( Bookmark& NewPos )
{
	GetSource( NewPos );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Interpreter::GetFirstBlock
 NOTES: Returns the first block.  Often used by the Interface if no starting
		block is set.
*/
BlockPtr Interpreter::GetFirstBlock()
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


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Sets the source to some other ReaderSource (ie. not a file).
*/
void Interpreter::SetSource( ReaderSource& Source )
{
	if( mSources.find( Source.GetName() ) != mSources.end() ){
		GetSource( Bookmark( Source.GetName(), 0, 0 ) );
		return;
	}
	
	mSources[ Source.GetName() ].reset( &Source, null_deleter() );
	
	ScopePtr pNewScope( CreateGeneric<Scope>( Source.GetName(), false, false ) );
	
	mpGlobalScope->Register( ScopeObjectPtr( pNewScope ) );
	mpCurrentScope = pNewScope;
	
	mpCurrentSource.reset( &Source, null_deleter() );
	
	try{
		Parse(); //Position should be 0,0
	}
	catch( ParserAnomaly E )
	{
		TackOnScriptInfo( E );
		throw E;
	}
}



/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Interpreter::LoadLevel
 NOTES: This is only for loading new files.  USE GetFile INSTEAD!!
*/
void Interpreter::LoadFile( const STRING& FileName )
{
	ReaderSourceFilePtr pNewFile( new ReaderSourceFile );

	pNewFile->Open( FileName );
	mSources[FileName] = pNewFile;


	ScopePtr pNewScope( CreateGeneric<Scope>( 
			MakeScopeNameFromFileName( pNewFile->GetName() ), false, false ) );
	
	mpGlobalScope->Register( ScopeObjectPtr( pNewScope ) );
	mpCurrentScope = pNewScope;

	mpCurrentSource = pNewFile;

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
 Interpreter::GetSource
 NOTES:  Call this whenever you need to go to a bookmark.
*/
ReaderSource& Interpreter::GetSource( const Bookmark& Pos )
{
	if( Pos.IsVoid() ){
		ThrowParserAnomaly( TXT("Attempted to open a empty file name. "
					"Most likely this is due to a bug in the interpreter."),
					ANOMALY_BADFILE );
	}

	if( mSources.find( Pos.FileName ) == mSources.end() )	{
		LoadFile( Pos.FileName );
	}

	ReaderSourcePtr pSource = mSources[Pos.FileName];
	pSource->GotoPos( Pos.Position );

	mpCurrentSource = pSource;

	if( Pos.CurrentScope ){
		mpCurrentScope = Pos.CurrentScope;
	}
	else{
		mpCurrentScope = mpGlobalScope->GetScopeObjectLocal( 
				MakeScopeNameFromFileName(pSource->GetName()) )->CastToScope();
	}

	if( Pos.CurrentStaticScope ){
		mpCurrentStaticScope = Pos.CurrentStaticScope;
	}
	else{
		mpCurrentStaticScope = mpCurrentScope;
	}

	return *pSource;
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
	mSources.clear();
	mpCurrentSource.reset();
	mpGlobalScope->Clear();
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
	ScopeObjectPtr pBlockHopeful = GetScopeObject( MakeCompoundID( BlockName ) );

	//This will throw if it is indeed not a Block.
	BlockPtr pBlock = pBlockHopeful->CastToBlock();
    
	return Parse( pBlock );
}




void Interpreter::Parse( BlockPtr pBlock, bool SayBlock /*=true*/,
						 VariableBasePtr In /*= VariableBasePtr()*/ )
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
	//pBlock->Clear();
	//Actuall, please don't.

	Bookmark Pos = pBlock->GetFilePosition();
	
	//This is how the magical instance system works.
	//Every time a block gets executed it creates this temporary instance
	//scope, which all non-statics get created on
	ScopePtr pInstance = CreateGeneric<Scope>();
	ListPtr InInstance = CreateGeneric<List>( LC_Input, false, false );
	VariablePtr OutInstance = CreateVariable<Variable>( LC_Output, false, false, STRING() );
	if( In ) *InInstance = *In;
	pInstance->Register( InInstance );
	pInstance->Register( OutInstance );
	
	pBlock->Import( pInstance );

	Pos.CurrentStaticScope = pBlock;
	Pos.CurrentScope = pInstance;


	if( pBlock->HasBeenSaid() ) 	Parse( Pos, false, true  );
	else                        Parse( Pos, false, false );
		
	//This a special little trick that the out variable does:
	//There is a static 'out' and a 'out' that is created with each instance.
	//When the block is finished the instanced 'out' gets copied to the static out.
	//Trust me.  This makes sense.
	*(pBlock->GetScopeObjectLocal( LC_Output )->CastToVariable()) = *OutInstance->CastToVariableBase();

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
	ListPtr pNextLine = pBlock->GetScopeObjectLocal( LC_NextBlock )->CastToList();

	//Create a list of a actual choices.  Throw out bad values.
	const ListType& OrigChoices = pNextLine->GetInternalList();
	std::vector<BlockPtr> GoodChoices;
	
    unsigned int i;
	for( i = 0; i < OrigChoices.size(); i ++ )
	{
		GoodChoices.push_back( GetScopeObject( 
				MakeCompoundID( OrigChoices[i]->GetStringData() ) )->CastToBlock() );
	}

    if( GoodChoices.size() != 0 )
	{
		//TODO: Discard non-player choices (maybe?)
		if( GoodChoices.size() > 1 )
		{
			BlockPtr pChoice = GoodChoices[ mpInterface->PresentChoice( GoodChoices ) ];
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



//REWRITE
void Interpreter::Parse( Bookmark Pos /*Bookmark()*/,
						 bool OneStatement /*= false*/,
						 bool IgnoreStatic /*= true*/ )
{
	if( mVerboseOutput ) mpInterface->LogMessage( TXT("PARSING...\n") );

	if( Pos.IsVoid() ) Pos = GetCurrentPos();	
	ReaderSource& MySource = GetSource( Pos );
	
	ExpressionPtr pExpression;
	

	//For handling else's
	bool AllowElse = false;
	bool LastConditionalResult = false;
	
	const Word* pTempWord;

	
	while( true )
	{
		/*
			When blocks call return this flag gets set and the interpreter ends parsing.
		*/
		if( mStop )
		{
			while( MySource.GetNextWord().Extra != EXTRA_BRACKET_Right ){}
			mStop = false;	
		}		
		
		/*
			Grab the next word.
		*/
		pTempWord = &MySource.GetNextWord();
 		if( *pTempWord == EOF_WORD ) return;
 		
 		//We don't parse past '}'
 		if( pTempWord->Extra == EXTRA_BRACKET_Right ) return;
 		
 		//We don't parse past '{' either in this case.
 		if( pTempWord->Extra == EXTRA_BRACKET_Left  ) return;
 		
 		//Else only works immediatley after the control statement.
 		if( pTempWord->Extra != EXTRA_CONTROL_Else && AllowElse ) AllowElse = false;
 		
 		//Shall we skip statics?
 		if( pTempWord->Extra == EXTRA_CONTROL_Static )
 		{
 			if( !IgnoreStatic ) FastForwardToNextStatement( MySource );
 			continue; 
 		}	
 		
 		//If
 		if( pTempWord->Extra == EXTRA_CONTROL_If )
 		{
 			//Get the condition
 			ExpressionPtr pCondition = GetNextExpression( MySource );
 			
 			pTempWord = &MySource.GetNextWord();
 			if( *pTempWord == EOF_WORD ) ThrowUnexpectedEOF();
 			else if( pTempWord->Extra == EXTRA_CONTROL_Do )
 			{
 				LastConditionalResult = ParseIf( *pCondition, GetCurrentPos(), true );
 			}
 			else if( pTempWord->Extra == EXTRA_BRACKET_Left )
 			{
 				LastConditionalResult = ParseIf( *pCondition, GetCurrentPos(), false );
 			}
 			else
 			{
 				ThrowParserAnomaly( TXT("Malformed \'if\' statement."), ANOMALY_BADGRAMMAR );	
 			}
 			
 			AllowElse = true;
 			
 			if( OneStatement ) return;
 			else continue;
 		}
 		
 		//While
 		if( pTempWord->Extra == EXTRA_CONTROL_While )
 		{
 			 //Get the condition
 			ExpressionPtr pCondition = GetNextExpression( MySource );
 			
 			pTempWord = &MySource.GetNextWord();
 			if( *pTempWord == EOF_WORD ) ThrowUnexpectedEOF();
 			else if( pTempWord->Extra == EXTRA_CONTROL_Do )
 			{
 				LastConditionalResult = ParseWhile( *pCondition, GetCurrentPos(), true );
 			}
 			else if( pTempWord->Extra == EXTRA_BRACKET_Left )
 			{
 				LastConditionalResult = ParseWhile( *pCondition, GetCurrentPos(), false );
 			}
 			else
 			{
 				ThrowParserAnomaly( TXT("Malformed \'if\' statement."), ANOMALY_BADGRAMMAR );	
 			}
 			
 			AllowElse = true;
 			
 			if( OneStatement ) return;
 			else continue;
 		}
 		
 		//Else
 		if( pTempWord->Extra == EXTRA_CONTROL_Else )
 		{
 			if( !AllowElse ){
 				ThrowParserAnomaly( TXT("Found an \'else\' without a matching \'if\' or \'while\'."), ANOMALY_BADGRAMMAR );
 			}
 			
 			if( LastConditionalResult == false )
 			{
 				pTempWord = &MySource.GetNextWord();
 				if( pTempWord->Extra == EXTRA_BRACKET_Left )
 				{
 					Parse( GetCurrentPos() );
 				}
 					
 				//Go ahead and suck up a do/then if its there.
 				if( pTempWord->Extra != EXTRA_CONTROL_Do ) MySource.PutBackWord();
 				
 				AllowElse = false;
 			}
 			else
 			{
 				Word NextWord = MySource.GetNextWord();
 				
 				if( NextWord.Extra == EXTRA_CONTROL_If ) AllowElse = true;
 				else AllowElse = false;
 				
 				MySource.PutBackWord();
 				FastForwardToNextStatement( MySource );
 			}
 			
 			continue; 				
 		}
 		
 		//Block declarations
 		if( pTempWord->Type == WORDTYPE_IDENTIFIER )
 		{
 			CompoundString Id = pTempWord->String;
 			
 			pTempWord = &MySource.GetNextWord();
 			if( pTempWord->Extra == EXTRA_BRACKET_Left )
 			{
 				pTempWord = &MySource.GetNextWord();
				if( pTempWord->Type == WORDTYPE_DOCSTRING )
				{
					ScopeObjectPtr pNewBlock =
						MakeScopeObject( SCOPEOBJ_BLOCK, Id );
					pNewBlock->CastToScope()->GetDocString() = pTempWord->String[0];		
				}
				else
				{
					pTempWord = &MySource.PutBackWord();
					MakeScopeObject( SCOPEOBJ_BLOCK, Id );
				}
				
				if( mVerboseOutput ){
					mpInterface->LogMessage( STRING(TXT("Registered new block: \'")) + CollapseCompoundString(Id) + STRING(TXT("\'.\n")) );
				}
				
				//Now we have to skip to the end }
				unsigned long BracketCount = 1;
				do{
					pTempWord = &MySource.GetNextWord();
					if( pTempWord->Extra == EXTRA_BRACKET_Left ) BracketCount++;
					else if( pTempWord->Extra == EXTRA_BRACKET_Right ) BracketCount--;					
				}
				while( BracketCount > 0 );	
				
				
				
				/*
				unsigned long BracketCount;
				NextWord = MySource.GetNextWord();
				for( BracketCount = 1; BracketCount > 0; NextWord = MySource.GetNextWord() )
				{
					if( NextWord.Extra == EXTRA_BRACKET_Left ) BracketCount++;
					else if( NextWord.Extra == 	EXTRA_BRACKET_Right ) BracketCount--;
				}
				*/
 				
 				if( OneStatement ) return;
 				else continue;
 			}
 			else pTempWord = &MySource.PutBackWord();
 		}
 		
 		
 		//Otherwise we assume it's an expression and try to evaluate it.
 		MySource.PutBackWord(); //Let GetNextExpression take it.
 		GetNextExpression( MySource )->Evaluate();
 		pTempWord = &MySource.GetNextWord();
 		if( pTempWord->Type != WORDTYPE_TERMINAL )
 		{
 			ThrowParserAnomaly( TXT("Missing \';\' at the end of this expression."), ANOMALY_BADPUNCTUATION );
 		}
 		
 		if( OneStatement ) return;
 		else continue;
	}	
}


void Interpreter::ThrowUnexpectedEOF() const
{
	ThrowParserAnomaly( TXT("Unexpected end of file."), ANOMALY_EOF );
}


Interpreter::ExpressionPtr Interpreter::GetNextExpression( ReaderSource& MySource )
{
	const Word* pTempWord;
	
	Bookmark CurrentPos = GetCurrentPos();
	CachedExpressionMap::iterator i = mCachedExpressions.find( CurrentPos );
	if( i != mCachedExpressions.end() )
	{
		MySource.GotoPos( i->second.NextPos );
		return i->second.MyExp;
	}
	
	ExpressionPtr NextExpression( new Expression );
			
	while( true )
	{
		pTempWord = &MySource.GetNextWord();
		
		if( pTempWord->Type == WORDTYPE_IDENTIFIER ||
			pTempWord->Type == WORDTYPE_FLOATLITERAL ||
			pTempWord->Type == WORDTYPE_STRINGLITERAL ||
			pTempWord->Type == WORDTYPE_BOOLLITERAL ||
			pTempWord->Type == WORDTYPE_EMPTYLISTLITERAL ||
			pTempWord->Type == WORDTYPE_BINARYOPERATOR ||
			pTempWord->Type == WORDTYPE_UNARYOPERATOR ||
			pTempWord->Type == WORDTYPE_AMBIGUOUSOPERATOR ||
			pTempWord->Type == WORDTYPE_PARENTHESIS )
		{
			NextExpression->push_back( *pTempWord );			
		}
		else break;
	}
	
	//The last word (the non-matching one) gets put back.
	MySource.PutBackWord();
	
	//Remember to save it!!
	mCachedExpressions[ CurrentPos ] = CachedExpression( NextExpression, MySource.GetPos()  );
	
	return NextExpression;
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

	if( Condition.Evaluate()->GetBoolData() == true )
	{
		Parse( Body, OneStatement );

		WasParsed = true;
	}
	else if( OneStatement )
	{
		//Fast forward to the end of the statement.
		ReaderSource& FileRef = GetSource( Body );
		const Word* pTempWord;
		while( pTempWord->Type != WORDTYPE_TERMINAL ) pTempWord = &FileRef.GetNextWord();
	}
	else
	{
		//Fast forward to the end of the body
		ReaderSource& FileRef = GetSource( Body );
		unsigned int BracketCount = 1;
		const Word* pTempWord;
		while( BracketCount )
		{
			const Word* pTempWord = &FileRef.GetNextWord();
			if     ( pTempWord->Extra == EXTRA_BRACKET_Left ) BracketCount++;
			else if( pTempWord->Extra == EXTRA_BRACKET_Right ) BracketCount--;
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

	while( Condition.Evaluate()->GetBoolData() == true )
	{
		Parse( Body, OneStatement );
		WasParsed = true;
		if( mStop ) break;
	}

	if( !WasParsed && OneStatement )
	{
		//Fast forward to the end of the statement.
		ReaderSource& FileRef = GetSource( Body );
		const Word* pTempWord;
		while( pTempWord->Type != WORDTYPE_TERMINAL ) pTempWord = &FileRef.GetNextWord();
	}
	else if( !WasParsed )
	{

		//Fast forward to the end of the body
		ReaderSource& FileRef = GetSource( Body );
		unsigned int BracketCount = 1;
		
		while( BracketCount )
		{
			const Word* pTempWord = &FileRef.GetNextWord();
			if     ( pTempWord->Extra == EXTRA_BRACKET_Left ) BracketCount++;
			else if( pTempWord->Extra == EXTRA_BRACKET_Right ) BracketCount--;
		}
	}

	return WasParsed;
}


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Interpreter::BreakOffFirstID
 NOTES: In a ID such as "Charlie: Hello", it will remove the "Charlie: "
		and return "Charlie".  It does nothing if the word is not an ID at all.
*/
/*
Word Interpreter::BreakOffFirstID( Word& W )
{
	Word TempWord;

	if( W.Type != WORDTYPE_IDENTIFIER )
	{
		TempWord.Type = WORDTYPE_UNKNOWN;
		return TempWord;
	}

	TempWord.Type = WORDTYPE_IDENTIFIER;

	while( !W.String.empty() && (IsAlpha(W.String[0]) ||
				     W.String[0] == '_' ||
				     IsNumber(W.String[0])) )
	{
		TempWord.String += W.String[0];
		W.String.erase(0,1);
	}

	//remove the excess fat
	while( !W.String.empty() && !(IsAlpha(W.String[0]) ||
				      W.String[0] == '_' ||
				      IsNumber(W.String[0])) )
	{
		W.String.erase(0,1);
	}

	return TempWord;
}
*/


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Interpreter::BreakOffLastID
 NOTES: See "BreakOffFirstID" this its opposite.
*/
/*
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
		   (IsAlpha( W.String[W.String.length()-1] ) ||
		    W.String[W.String.length()-1] == '_' ||
		    IsNumber( W.String[W.String.length()-1] )) )
	{
		TempWord.String.insert(0, 1, W.String[W.String.length()-1]);
		W.String.erase( W.String.length()-1, 1 );
	}

	//remove the excess fat
	while( !W.String.empty() &&
		   !(IsAlpha( W.String[W.String.length()-1] ) ||
		     W.String[W.String.length()-1] == '_' ||
		     IsNumber( W.String[W.String.length()-1] )) )
	{
		W.String.erase( W.String.length()-1, 1 );
	}

	return TempWord;
}
*/


/*~~~~~~~FUNCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 NOTES: This is the universal function to register new ScopeObjects.  This is a more
        intelligent solution than just calling mpCurrentScope.Register.  This will
		figure out where the object really belongs.
*/
ScopeObjectPtr Interpreter::MakeScopeObject( ScopeObjectType Type, const CompoundString& S,
								   bool Static /*= false*/, bool Const /*= false*/ )
 {
	ScopeObjectPtr pNewObj;

	const STRING& Name = S[0];
	CompoundString ScopeName( S.begin(), S.end() - 1 );

	switch( Type )
	{
	case SCOPEOBJ_BLOCK:
		{
		BlockPtr pTempBlock( CreateBlock<Block>( Name, Static, Const,
								 GetCurrentPos(), (BlockIndex)mBlockOrder.size() ) );
		mBlockOrder.push_back( pTempBlock );
		pNewObj = (ScopeObjectPtr)pTempBlock ;
		}
		break;
	case SCOPEOBJ_VARIABLE:
		pNewObj = CreateVariable<Variable>( Name, Static, Const, 0 );
		break;
	case SCOPEOBJ_CHARACTER:
		pNewObj = CreateGeneric<Character>( Name, Static, Const );
		break;
	case SCOPEOBJ_SCOPE:
		pNewObj = CreateGeneric<Scope>( Name, Static, Const );
		break;
	case SCOPEOBJ_LIST:
		pNewObj = CreateGeneric<List>( Name, Static, Const );
		break;
	default:
		SS::STRING tmp = TXT("Tried to register an object with an unknown type named: \'");
		tmp += CollapseCompoundString(S);
		tmp += TXT("\'.");
		ThrowParserAnomaly( tmp, ANOMALY_UNKNOWNTYPE );

	}

	if( ScopeName.size() == 0 || ( ScopeName.size() == 1 && ScopeName[0].length() == 0 ) )
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
		GetScopeObject( ScopeName )->CastToScope()->Register( pNewObj );
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
ScopeObjectPtr Interpreter::GetScopeObject( const CompoundString& Name )
{
	//Try the local scope first

	ScopePtr pPotentialScope = mpCurrentScope;
	ScopeObjectPtr pObject;

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
	STRING ScopeName( MakeScopeNameFromFileName( mpCurrentSource->GetName() ) );
	
	pObject = mpGlobalScope->GetScopeObject( MakeCompoundID( ScopeName ) )->CastToScope()->GetScopeObject_NoThrow( Name );
	if( pObject ) return pObject;


	//Nothing
	STRING tmp = TXT("Cannot find an indentifier by the name \'");
	tmp += CollapseCompoundString( Name );
	tmp += TXT("\'.  Check your spelling.");
	ThrowParserAnomaly( tmp, ANOMALY_IDNOTFOUND );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: These are all shortcuts for the above.  Mainly just for used by
 		external software.
*/
ScopePtr Interpreter::GetScope( const STRING& Name ){
	return GetScopeObject( MakeCompoundID( Name ) )->CastToScope();
}

VariablePtr Interpreter::GetVariable( const STRING& Name ){
	return GetScopeObject( MakeCompoundID( Name ) )->CastToVariable();
}

BlockPtr Interpreter::GetBlock( const STRING& Name ){
	return GetScopeObject( MakeCompoundID( Name ) )->CastToBlock();
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Interpreter::TackOnScriptInfo
 NOTES: Adds information about the script currently being parsed
		to the ParserAnomaly.
*/
void Interpreter::TackOnScriptInfo( ParserAnomaly& E )
{
	AssertFileOpen();

	E.ScriptFile = mpCurrentSource->GetName();
	E.ScriptLine = mpCurrentSource->GetLineNumber();
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 Interpreter::AssertFileOpen
 NOTES: Tests if the file is open and throws if it is not.
*/
void Interpreter::AssertFileOpen()
{
	if( !mpCurrentSource ){
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
void Interpreter::FastForwardToNextStatement( ReaderSource& MySource )
{
	const Word* pTempWord;

	for( pTempWord = &MySource.GetNextWord();
		 pTempWord->Type != WORDTYPE_TERMINAL;
		 pTempWord = &MySource.GetNextWord() )
	{
		//Override the search for the ';' and instead break on
		//a complete bracket set.
		if( pTempWord->Extra == EXTRA_BRACKET_Left )
		{
			unsigned long BracketCount = 1;

			while( true )
			{
				pTempWord = &MySource.GetNextWord();

				if( pTempWord->Extra == EXTRA_BRACKET_Left ) BracketCount++;
				else if( pTempWord->Extra == EXTRA_BRACKET_Right )
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
	ReaderSource& MySource = Interpreter::Instance().GetSource( ImportedFile ); 
	
	ScopePtr pTmpObj = GetScope( MakeScopeNameFromFileName( MySource.GetName() ) );

	//Return the file and position to its previous settings
	GetSource( OldPos );

	mpCurrentScope->Import( pTmpObj );
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: Tells the interpreter to return from the block when it finishes the
 		expression its on.
*/
void Interpreter::Stop()
{
	mStop = true;	
}

