/*
Copyright (c) 2004-2006 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.
Full license information is included in the file in the top
directory named "license".
*/

/**
	\file Interpreter.hpp
	\brief Declarations for Interpreter.  
*/


/**
	\namespace SS
	\brief The main namespace of which pretty much all storyscript code is held.
*/


///
///	\mainpage The StoryScript API
///	\section A Brief Tutorial to the StoryScript API
///	
///
///1. Include the storyscript header.
///\code
/// #include <StoryScript.hpp>
///\endcode
///
///2. Create an Interpreter object.
///\code
///SS::Interpreter AnInterpreter;
///\endcode
///
///3. Write an Interface.  (Don't worry, this is the hard step.)
///\code
///class MyInterface : public SS::Interface
///{
///public:
///	MyInterface( SS::Interpreter& I ) : Interface(I) {}
///
///	unsigned int PresentChoice( const BlockList& Choices );
///	void HandleParserAnomaly( ParserAnomaly A );
///	void LogMessage( const SS::String& S, bool UserOutput = false );
///
///private:
///	void SayBlock( const BlockPtr );
///
///};
///
///
///unsigned int MyInterface::PresentChoice( const BlockList& Choices )
///{
///	size_t i;
///	SS::String BlockTitle;
///	for( i = 0; i< Choices.size(); i++ )
///	{
///		BlockTitle = Choices[0]->GetDocString();
///
///		/*
///			Send BlockTitle to some fancy GUI widget.
///		*/
///	}
///
///	unsigned int Choice;
///	
///	/*
///		Have the user pick from the choices.
///	*/
///
///	return Choice;
///}
///
///void MyInterface::HandleParserAnomaly( ParserAnomaly A )
///{
///	std::cout << A.what();
///	exit(1);
///}
///
///void MyInterface::LogMessage( const SS::String& Msg, bool UserOutput )
///{
///	if( UserOutput )
///	{
///		/*
///			Draw the message on the screen.
///		*/
///	}
///	else
///	{
///		/*
///			Print the message to some hidden console or log file.
///		*/
///	}
///}
///
///void MyInterface::SayBlock( BlockPtr B )
///{
///	//You may not need all of these.
///	String Character  = B->GetParent()->GetName();
///	String Line       = B->GetOutString();
///	String BlockTitle = B->GetDocString();
///	String BlockName  = B->GetName();
///
///	/*
///		Do some fancy graphics and animation.
///	*/
///
///	/*
///		Or you could play an audio file if you are smart about it.
///		You could name your sound files "CharacterName.BlockName.mp3" and
///		then play a file based off the Character and BlockName.  The downside
///		is that you have to make sure to name all your blocks.
///	*/
///	String AudioFile = Character + SS::String(TXT(".")) + BlockName + SS::String(TXT(".mp3"));
///
///	/*
///		Another way to do audio files with blocks is to simply have a static var
///		in each of your blocks.  Then when you are saying a block you can simply
///		access this object.  This requires a few more function calls, but isn't too
///		difficult.
///	*/
///
///	//We use this call instead of GetScopeObject, because we don't want to search
///	//the blocks parent(s) and we don't want to throw exceptions if it isn't there.
///	ScopeObjectPtr AudioFileVariable = B->GetScopeObjectLocal_NoThrow( "AudioFile" );
///	
///	if( AudioFileVariable )	{	
///		AudioFile = AudioFileVariable->CastToVariable()->GetStringData();
///	}
///	else
///	{
///		/*
///			There is not variable named "AudioFile" in the block.
///			Do something!
///		*/
///	}
///}
///\endcode
///
///4. Create an instance of that killer interface you just wrote.
///\code
///MyInterface AnInterface( AnInterpreter );
///\endcode
///
///5. Play!
///\code
/// /*
///	When a conversation get triggered in your game, simply call the correct script file.
/// */
/// AnInterface.StartConversation( "AVeryInterestingConversation.ssconv" );
///	\endcode
///	
	


#if !defined(SS_INTERPRETER)
#define SS_INTERPRETER

#include "Defines.hpp"
#include "VersionInfo.hpp"
#include "Scope.hpp"
#include "ScopeObject.hpp"
#include "Variable.hpp"
#include "ParserAnomaly.hpp"
#include "Unicode.hpp"
#include "DLLExport.hpp"
//#include "ScriptFile.hpp"
#include "Types.hpp"
#include "Expression.hpp"

#include "Bookmark.hpp"
#include "ReaderSourceFile.hpp"
#include "Word.hpp"


#include <map>

namespace SS
{


class Interface;



//~~~~~~~CLASS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/**
	\brief The central interpretation class.
	
	The is the interpreter class.  You are going to want one of these.
	You are also going to want to (have to) supply it with a Interface
	derivative of some sort.
	
	\sa Interface	
*/
class SS_API Interpreter
{
public:
	///Constructor
	Interpreter();
	
	///Destructor
	~Interpreter();
	
	/**
		\brief Ends execution of a block.
		
		Tells the interpreter to return from the block when it finishes the
 		expression its on.
 	*/
	void EndBlock();

	/**
		\brief Checks if the interpreter is in verbose mode.
		
		\return True if the verbose output is on, false otherwise.
	*/
	bool IsVerbose() const;
	
	/**
		\brief Turns verbose mode on/off.
		
		\param Flag True for on, False for off.
	*/ 
	void SetVerbose( bool Flag = true );
	
	/**
		\brief Set the Interface being used.
		
		\param I Reference to the Interface to use.
		
		\sa Interface 
	*/
	void SetInterface( Interface& I );
	
	
	/**
		\brief Begin executing a generic reader source.
		
		\sa ReaderSource
	*/
	void SetSource( ReaderSource& R );
	
	/**
		\brief Begin executing a file.
		
		\param FileName Name of the file to execute.
	*/
	void OpenFile( const SS::String& FileName );
	
	/**
		\brief Executes a given block.
		
		Finds the block of the given name in the current scope
		and begins executing it.
		
		\param The identifier name of the block.
	*/
	void Parse( const SS::String& BlockName );
	
	/**
		\brief Executes a given block.
		
		The overload is supplied if you already have a pointer to the
		block you want to execute.
		
		\param pB Pointer to the Block object.
		\param SayBlock True if the block is a duologue line and should be
			'spoken'.  False if it is a function and shouldn't be 'said'.
		\param In Pointer to the Blocks arguments/input if it is indeed a function.
	*/
	void Parse( BlockPtr pB, bool SayBlock = true, VariableBasePtr In = VariableBasePtr() );

	///Returns a bookmark to the current position.
	Bookmark GetCurrentPos();
	///Returns a pointer to the current scope.
	ScopePtr GetCurrentScope();
	///Returns a pointer the the current static scope.
	ScopePtr GetCurrentStaticScope();
	///Returns a pointer to the global scope.
	ScopePtr GetGlobalScope();
	
	///Set the current position to a given Bookmark.
	void SetPos( Bookmark& B );

	///Returns a pointer to the first block in the current source.
    BlockPtr GetFirstBlock();	
	
	/**
		\brief Find a identifier in the current scope.
		
		Given an identifier in CompoundString form this will look
		in all the right places and return it (if possible).  It
		will throw an anomaly if the object doesn't exist.
		
		\param Name CompoundString holding the identifier name.
	*/
	ScopeObjectPtr GetScopeObject( const SS::CompoundString& Name );
	
	/**
		\breif A shortcut for object creation/registration.
		
		This function is a handy way to create an object and have it
		automatically registered on the current scope.
		
		\param T The type of object you wish to create.
		\param Name The Name of the object to create.
		\param Static Shall the object be created in the static scope or
			in the current instance.
		\param Const Is the object constant or not.
	*/
	ScopeObjectPtr MakeScopeObject( ScopeObjectType T, const SS::CompoundString& Name,
										bool Static = false, bool Const = false );
	
	/**
		\brief Find a VariableBase object of a given name.
		
		This is a shortcut for simply calling GetScopeObject and casting it.
	*/
	VariableBasePtr GetVariableBase( const SS::String& Name );
	
	/**
		\brief Find a Variable object of a given name.
		
		This is a shortcut for simply calling GetScopeObject and casting it.
	*/
	VariablePtr     GetVariable    ( const SS::String& Name );
	
	/**
		\brief Find a Block object of a given name.
		
		This is a shortcut for simply calling GetScopeObject and casting it.
	*/
	BlockPtr        GetBlock       ( const SS::String& Name );
	
	/**
		\brief Find a Scope object of a given name.
		
		This is a shortcut for simply calling GetScopeObject and casting it.
	*/
	ScopePtr 	    GetScope       ( const SS::String& Name );

	///Return a reference to the current interface.
	Interface& GetInterface();
	
	/**
		\brief Imports a file's scope into the current one.
		
		This is the equivalent of having a 'use Foo' in the script.
		Typically the user (ie. you) shouldn't have to call this.
		
		\param ScopeName Name of the scope to import.
	*/
	void ImportIntoCurrentScope    ( const SS::String& ScopeName );
	
	/**
		\brief Imports a file's scope into the current one.
		
		This is the equivalent of having a 'use "Foo.ssconv"' in the script.
		Typically the user (ie. you) shouldn't have to call this.
		
		\param FileName Name of the file to import.
	*/
	void ImportFileIntoCurrentScope( const SS::String& FileName );

	/**
		I don't think Interpreter befriending Interface constitutes bad design.
		I consider Interface to be like a part of Interpreter.  A part that
		can be swapped out for other similar parts.  Its just simpler
		than making Interpreter with virtual functions.
	*/
	friend class Interface;

private:
	///Throws an anomaly if no reader source has been loaded.
	void AssertSourceOpen();	
	///Throws an anomaly if no interface has been attached.
	void AssertAttachedInterface();
	
	/**
		\brief Get the ReaderSource at the current bookmark.
		
		This will load files as necessary and set the current scopes and
		positions.  Call this when you need to change the source.
	*/
	ReaderSource& GetSource( const Bookmark& );
	
	///The interpreters instance.  There can only be one.
	static boost::shared_ptr<Interpreter> mpInstance;
	
	/**
		\brief The actual parsing function in the interpreter.
		
		\param B Bookmark to the position to start execution.  If nothing is
			supplied, the current position will be used.
		\param OneStatement True if the interpreter should only execute one
			statement and then return.  A statement is typically an one
			expression, one loop, or one conditional.  Its kind of loose. ;)
		\param IgnoreStatic True if statements marked with 'static' should be skipped.
			This is true when the block is being executed for the non-first time.
	*/
	void Parse( Bookmark B = Bookmark(), bool OneStatement = false, bool IgnoreStatic = true );
	
	///Shared pointer to an Expression.
	typedef boost::shared_ptr<Expression> ExpressionPtr;
	
	/**
		\brief Return a pointer to the next expression.
		
		This allows us to cache expressions. This will return the next expression
		after the given source current position.
		
		\param S The ReaderSource from which the next expression should be retrieved.
		\return A pointer to the next expression.
	*/
	ExpressionPtr GetNextExpression( ReaderSource& S );

	/**
		\brief Load a new file.
		
		This is _only_ for loading new files.  Typically you
		don't want to call this directly.  GetFile is more likely what
		you want.  
		
		\sa GetFile
		
		\param FileName Name of the file to load.
	*/
	void LoadFile( const SS::String& FileName );
	
	///Called by the constructor to create built-in and special objects.
	void RegisterSpecials();

	/**
		\brief Parse an 'if' statement.
		
		This handles the parsing of conditionals.
		
		\param E The conditionals condition expression.
		\param B Bookmark to the body of the conditional.
		\param OneStatement True if the body consists of only one statement.
			Otherwise the it will parse until it finds a closing bracket ("}").
			
		\return True if the body was executed, false otherwise.
	*/
	bool ParseIf( const Expression& E, const Bookmark& B, bool OneStatement = false );
	
	/**
		\brief Parse an 'while' statement.
		
		This handles the parsing of loops.
		
		\param E The loops's condition expression.
		\param B Bookmark to the body of the loop.
		\param OneStatement True if the body consists of only one statement.
			Otherwise the it will parse until it finds a closing bracket ("}").
			
		\return True if the body was executed, false otherwise.
	*/
	bool ParseWhile( const Expression& E, const Bookmark& B, bool OneStatement = false );

	//Word BreakOffFirstID( Word& );
	//Word BreakOffLastID ( Word& ); 

	///Throws a End of File Anomaly.
	void ThrowUnexpectedEOF() const;
	
	///Add current script info onto a Anomaly throw by someone else.
	void TackOnScriptInfo( ParserAnomaly& );

	///Move the position up to the next statement.
	void FastForwardToNextStatement( ReaderSource& );
	
	/**
		\brief Shut everything down.
		
		The purges everything and should only really
		be called by the destructor.
	*/ 
	void Close();

	///The Interpreter's current Interface.
	Interface* mpInterface;

	///This is bound to SS::LangOpts::Verbose, for convenience.
	static bool& mVerboseOutput;

	///This is used to keep track of the order of all the blocks in a file.
	std::vector<BlockPtr> mBlockOrder;

	///A map where all the loaded ReaderSources are kept.
	std::map< SS::String, ReaderSourcePtr > mSources;

	///A pointer to the current scope.
	ReaderSourcePtr mpCurrentSource;

	///The global scope.
	ScopePtr mpGlobalScope;
	
	///The current (instance) scope.
	ScopePtr mpCurrentScope;
	///The current scope.
	ScopePtr mpCurrentStaticScope;
	
	///The magic end block.
	BlockPtr mpEndBlock;
	
	
	/**
		\brief An expression that has been cached for latter use.
		
		Caching expressions improved performance a lot be saving us from
		recreating them every time the interpreter comes across it.
	*/
	struct CachedExpression
	{
		CachedExpression() : NextPos(0) {}
		CachedExpression( ExpressionPtr MyExp, ReaderPos NextPos = 0 )
			: MyExp( MyExp ), NextPos( NextPos ) {}
		ExpressionPtr MyExp;
		ReaderPos NextPos;		
	};
	
	///Comparison function for the std::map.
	struct BookmarkCompare
	{
		bool operator()( const Bookmark& First, const Bookmark& Second ) const
		{
			int CmpResult = First.FileName.compare( Second.FileName );
			
			if( CmpResult < 0 ) return true;
			else if( CmpResult > 0 ) return false;
			else 
			{
				if( First.Position < Second.Position ) return true;
				else return false;
			}
		}
	};
	
	///The cached expression map.  Allows expressions to be looked up by position.
	typedef std::map< Bookmark, CachedExpression, BookmarkCompare > CachedExpressionMap;
	
	///The Interpreter's Expression cache.
	CachedExpressionMap mCachedExpressions;
	
	///Keeps track of whether the interpreter should return from a block.
	bool mStop;
};

} //namespace SS

#endif//!defined(SS_INTERPRETER)

