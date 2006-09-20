/*
Copyright (c) 2004-2006 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.
Full license information is included in the file in the top
directory named "license".
*/

#include "Interpreter.hpp"
#include "Interface.hpp"
#include "HelperFuncs.hpp"
#include "ParserAnomaly.hpp"
#include "Block.hpp"
#include "Character.hpp"
#include "List.hpp"
#include "Variable.hpp"
#include "MagicVars.hpp"
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



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
Interpreter::Interpreter()
{
	mpGlobalScope = CreateGeneric<Scope>();
	InitConstants();
	RegisterSpecials();
	mStop = false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
Interpreter::~Interpreter()
{
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Interpreter::SetInterface( Interface& SomeInterface )
{
	mpInterface = &SomeInterface;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
Interface& Interpreter::GetInterface()
{
	if( !mpInterface ){
		ThrowParserAnomaly( TXT("Interface requested before being set."
							" This means there is a serious bug in the interpreter."),
							ANOMALY_PANIC );
	}
	else return *mpInterface;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Interpreter::RegisterSpecials()
{
	//The last two values are just bullshit.
	mpEndBlock = CreateBlock<Block>( LC_EndBlock, true, *this, Bookmark(), 0 ); 
	mpGlobalScope->Register( ScopeObjectPtr( mpEndBlock ) );

	//SLib-Common gets special treatment.  It gets auto-imported.
	ScopePtr pCommonScope( CreateBasic<SS::SLib::Common>() );
	mpGlobalScope->Register( pCommonScope );
	mpGlobalScope->Import( pCommonScope );
	
	//I'm goig to import List also.  Those seem like pretty common functions
	ScopePtr pListScope( CreateBasic<SS::SLib::List>() );
	mpGlobalScope->Register( pListScope );
	mpGlobalScope->Import( pListScope );

	mpGlobalScope->Register( ScopeObjectPtr( CreateBasic<SS::SLib::Time>() ) );
	mpGlobalScope->Register( ScopeObjectPtr( CreateBasic<SS::SLib::Math>() ) );
	mpGlobalScope->Register( ScopeObjectPtr( CreateBasic<SS::SLib::LangOpts>() ) );
	
	//Register build in functions
	mpGlobalScope->Register( CreateGeneric<PrintOperator>   ( *this ) );
	mpGlobalScope->Register( CreateGeneric<ImportOperator>  ( *this ) );
	mpGlobalScope->Register( CreateGeneric<UnImportOperator>( *this ) );
	mpGlobalScope->Register( CreateGeneric<ReturnOperator>  ( *this ) );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
ScopePtr Interpreter::GetCurrentScope()
{
	return mpCurrentScope;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
ScopePtr Interpreter::GetCurrentStaticScope()
{
	return mpCurrentStaticScope;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
ScopePtr Interpreter::GetGlobalScope()
{
	return mpGlobalScope;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
Bookmark Interpreter::GetCurrentPos()
{
	AssertSourceOpen();
	Bookmark BM( mpCurrentSource->GetName(),
				 mpCurrentSource->GetPos(),
				 mpCurrentSource->GetLineNumber(),
				 mpCurrentScope,
				 mpCurrentStaticScope );
	
	return BM;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Interpreter::SetPos( Bookmark& NewPos )
{
	GetSource( NewPos );
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BlockPtr Interpreter::GetFirstBlock()
{
	AssertSourceOpen();

    if( mBlockOrder.size() > 0 ) return mBlockOrder[0];
	else
	{
		ThrowParserAnomaly( TXT("No blocks found in file."), ANOMALY_NOBLOCKS );
	}
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Interpreter::SetVerbose( bool flag /*=true*/ )
{
	mVerboseOutput = flag;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
bool Interpreter::IsVerbose() const{
	return mVerboseOutput;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Interpreter::SetSource( ReaderSource& Source )
{
	if( mSources.find( Source.GetName() ) != mSources.end() ){
		GetSource( Bookmark( Source.GetName(), 0, 0 ) );
		return;
	}
	
	mSources[ Source.GetName() ].reset( &Source, null_deleter() );
	
	ScopePtr pNewScope( CreateGeneric<Scope>( Source.GetName(), false ) );
	
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


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Interpreter::LoadFile( const String& FileName )
{
	ReaderSourceFilePtr pNewFile( new ReaderSourceFile );

	pNewFile->Open( FileName );
	mSources[FileName] = pNewFile;


	ScopePtr pNewScope( CreateGeneric<Scope>( 
			MakeScopeNameFromFileName( pNewFile->GetName() ), false ) );
	
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


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
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


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Interpreter::OpenFile( const SS::String& FileName )
{
	AssertAttachedInterface();
	//Keep an eye on the following line.  Close wipes the global scope,
	//and if this get incorrectly triggered, bad things will happen.
	if( mpCurrentSource ) Close();
	LoadFile( FileName );	
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Interpreter::Close()
{
	mBlockOrder.clear();
	mSources.clear();
	mpCurrentSource.reset();
	mpGlobalScope->Clear();
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Interpreter::Parse( const String& BlockName )
{
	//Attempt to find the block
	ScopeObjectPtr pBlockHopeful = GetScopeObject( MakeCompoundID( BlockName ) );

	//This will throw if it is indeed not a Block.
	BlockPtr pBlock = pBlockHopeful->CastToBlock();
    
	return Parse( pBlock );
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Interpreter::Parse( BlockPtr pBlock, bool SayBlock /*=true*/,
						 VariableBasePtr In /*= VariableBasePtr()*/ )
{
	try{
	if( mVerboseOutput )
	{
		String tmp = TXT("Trying to parse block \"");
		tmp += pBlock->GetFullName();
		tmp += TXT("\" at ");
		tmp += boost::lexical_cast<String>(pBlock->GetFilePosition().Position);
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
	ListPtr InInstance = CreateGeneric<List>( LC_Input, false );
	VariablePtr OutInstance = CreateVariable<Variable>( LC_Output, false, String() );
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



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
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
 			CompoundString Id = pTempWord->Str;
 			
 			pTempWord = &MySource.GetNextWord();
 			if( pTempWord->Extra == EXTRA_BRACKET_Left )
 			{
 				pTempWord = &MySource.GetNextWord();
				if( pTempWord->Type == WORDTYPE_DOCString )
				{
					ScopeObjectPtr pNewBlock =
						MakeScopeObject( SCOPEOBJ_BLOCK, Id );
					pNewBlock->CastToScope()->GetDocString() = pTempWord->Str[0];		
				}
				else
				{
					pTempWord = &MySource.PutBackWord();
					MakeScopeObject( SCOPEOBJ_BLOCK, Id );
				}
				
				if( mVerboseOutput ){
					mpInterface->LogMessage( String(TXT("Registered new block: \'")) + CollapseCompoundString(Id) + String(TXT("\'.\n")) );
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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Interpreter::ThrowUnexpectedEOF() const
{
	ThrowParserAnomaly( TXT("Unexpected end of file."), ANOMALY_EOF );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
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
	
	ExpressionPtr NextExpression( new Expression( *this ) );
			
	while( true )
	{
		pTempWord = &MySource.GetNextWord();
		
		if( pTempWord->Type == WORDTYPE_IDENTIFIER ||
			pTempWord->Type == WORDTYPE_FLOATLITERAL ||
			pTempWord->Type == WORDTYPE_StringLITERAL ||
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


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
bool Interpreter::ParseIf( const Expression& Condition,
                           const Bookmark& Body,
                           bool OneStatement /*= false*/ )
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
			pTempWord = &FileRef.GetNextWord();
			if     ( pTempWord->Extra == EXTRA_BRACKET_Left ) BracketCount++;
			else if( pTempWord->Extra == EXTRA_BRACKET_Right ) BracketCount--;
		}
	}

	return WasParsed;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
bool Interpreter::ParseWhile( const Expression& Condition,
                              const Bookmark& Body,
                              bool OneStatement /*= false*/ )
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


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
ScopeObjectPtr Interpreter::MakeScopeObject( ScopeObjectType Type, const CompoundString& S,
								   bool Static /*= false*/, bool Const /*= false*/ )
 {
	ScopeObjectPtr pNewObj;

	const String& Name = S[S.size()-1];
	CompoundString ScopeName( S.begin(), S.end() - 1 );

	switch( Type )
	{
	case SCOPEOBJ_BLOCK:
		{
		BlockPtr pTempBlock( CreateBlock<Block>( Name, Const, *this,
								 GetCurrentPos(), (BlockIndex)mBlockOrder.size() ) );
		mBlockOrder.push_back( pTempBlock );
		pNewObj = (ScopeObjectPtr)pTempBlock ;
		}
		break;
	case SCOPEOBJ_VARIABLE:
		pNewObj = CreateGeneric<Variable>( Name, Const );
		break;
	case SCOPEOBJ_CharACTER:
		pNewObj = CreateGeneric<Character>( Name, Const );
		break;
	case SCOPEOBJ_SCOPE:
		pNewObj = CreateGeneric<Scope>( Name, Const );
		break;
	case SCOPEOBJ_LIST:
		pNewObj = CreateGeneric<List>( Name, Const );
		break;
	default:
		SS::String tmp = TXT("Tried to register an object with an unknown type named: \'");
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
		
		GetScopeObject( ScopeName )->CastToScope()->Register( pNewObj );
	}

	return pNewObj;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
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
	String ScopeName( MakeScopeNameFromFileName( mpCurrentSource->GetName() ) );
	
	pObject = mpGlobalScope->GetScopeObject( MakeCompoundID( ScopeName ) )->CastToScope()->GetScopeObject_NoThrow( Name );
	if( pObject ) return pObject;


	//Nothing
	String tmp = TXT("Cannot find an indentifier by the name \'");
	tmp += CollapseCompoundString( Name );
	tmp += TXT("\'.  Check your spelling.");
	ThrowParserAnomaly( tmp, ANOMALY_IDNOTFOUND );
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr Interpreter::GetVariableBase( const String& Name ){
	return GetScopeObject( MakeCompoundID( Name ) )->CastToVariableBase();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
ScopePtr Interpreter::GetScope( const String& Name ){
	return GetScopeObject( MakeCompoundID( Name ) )->CastToScope();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariablePtr Interpreter::GetVariable( const String& Name ){
	return GetScopeObject( MakeCompoundID( Name ) )->CastToVariable();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
BlockPtr Interpreter::GetBlock( const String& Name ){
	return GetScopeObject( MakeCompoundID( Name ) )->CastToBlock();
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Interpreter::TackOnScriptInfo( ParserAnomaly& E )
{
	AssertSourceOpen();

	E.ScriptFile = mpCurrentSource->GetName();
	E.ScriptLine = mpCurrentSource->GetLineNumber();
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Interpreter::AssertSourceOpen()
{
	if( !mpCurrentSource ){
		ThrowParserAnomaly( 
			TXT("No reader source has been loaded! "
			"This is probably due to a bug in the interpreter."), ANOMALY_PANIC );
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Interpreter::AssertAttachedInterface()
{
	if( !mpInterface ){
		ThrowParserAnomaly( 
			TXT("No interface attached to Interpreter!"), ANOMALY_PANIC );
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Interpreter::ImportIntoCurrentScope( const String& Name )
{
	if( mVerboseOutput ){
		mpInterface->LogMessage( String(TXT("Importing \'")) + Name + String(TXT("\'.\n")) );
	}
	
	mpCurrentScope->Import( GetScope( Name ) );
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Interpreter::ImportFileIntoCurrentScope( const String& FileName )
{
	if( mVerboseOutput ){
		mpInterface->LogMessage( String(TXT("Importing \'")) + FileName + String(TXT("\'.\n")) );
	}
	
	Bookmark OldPos = GetCurrentPos();
					
	Bookmark ImportedFile( FileName, 0, 0 );
	ReaderSource& MySource = GetSource( ImportedFile ); 
	
	ScopePtr pTmpObj = GetScope( MakeScopeNameFromFileName( MySource.GetName() ) );

	//Return the file and position to its previous settings
	GetSource( OldPos );

	mpCurrentScope->Import( pTmpObj );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Interpreter::EndBlock()
{
	mStop = true;	
}

