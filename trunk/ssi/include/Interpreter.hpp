/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: Contains declarations for the Interprer class which handles the bulk
	of all the parsing.
*/


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

#include <map>

namespace SS
{


class Interface;


/*~~~~~~~CLASS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Interpreter
 NOTES: The main S-Script interpreter class.  This is a singleton, call
		  Interpreter::Instance()...  to access.  To start the interpreter,
		  Open().  To call blocks, call Parse().
		  However, you need to attach an interface, so it knows how to
		  output and input stuff.
*/
class SS_API Interpreter
{
public:
	static Interpreter& Instance();
	
	~Interpreter();
	

	bool IsVerbose() const;
	void SetVerbose( bool flag = true );
	void SetInterface( Interface& );
	
	void SetSource( ReaderSource& );
	
	void Open( const SS::STRING& FileName );
	void Close();
	
	void Parse( const SS::STRING& BlockName );
	void Parse( BlockPtr, bool SayBlock = true, VariableBasePtr In = VariableBasePtr() );

	Bookmark GetCurrentPos();
	ScopePtr GetCurrentScope();
	void SetPos( Bookmark& );

    BlockPtr GetFirstBlock();	
	
	ScopeObjectPtr GetScopeObject( const SS::STRING& Name );
	
	ScopeObjectPtr MakeScopeObject( ScopeObjectType, const SS::STRING& Name,
										bool Static = false, bool Const = false );
	
	//These are all shortcuts for outsiders
	VariablePtr GetVariable( const SS::STRING& Name );
	BlockPtr    GetBlock   ( const SS::STRING& Name );
	ScopePtr	GetScope   ( const SS::STRING& Name );

	Interface& GetInterface();
	
	//Generally, outsiders will not have to call these.
	void ImportIntoCurrentScope    ( const SS::STRING& ScopeName );
	void ImportFileIntoCurrentScope( const SS::STRING& FileName );

	//I don't think this violates good design.  I consider
	//Interface to be like a part of Interpreter.  A part that can be 
	//swapped out for other similar parts.  Its just simpler than making
	//Interpreter with virtual functions.
	friend class Interface;

private:
	Interpreter();

	void AssertFileOpen();	
	void AssertAttachedInterface();
	
	ReaderSource& GetSource( const Bookmark& );
	
	static boost::shared_ptr<Interpreter> mpInstance;
	
	void Parse( Bookmark = Bookmark(), bool OneStatement = false, bool IgnoreStatic = true );

	void LoadFile( const SS::STRING& FileName );
	

	void RegisterSpecials();

	bool ParseIf( const Expression&, const Bookmark&, bool OneStatement = false );
	bool ParseWhile( const Expression&, const Bookmark&, bool OneStatement = false );

	Word BreakOffFirstID( Word& );
	Word BreakOffLastID ( Word& ); 

	

	void TackOnScriptInfo( ParserAnomaly& );

	void FastForwardToNextStatement( ReaderSource& );

	Interface* mpInterface;

	//This is bound to SS::LangOpts::Verbose, for convenience.
	static bool& mVerboseOutput;


	//This is used to keep track of the order of all the blocks in a file.
	std::vector<BlockPtr> mBlockOrder;

	std::map< SS::STRING, ReaderSourcePtr > mSources;

	ReaderSourcePtr mpCurrentSource;

	ScopePtr mpGlobalScope;
	
	ScopePtr mpCurrentScope;
	ScopePtr mpCurrentStaticScope;
	
	BlockPtr mpEndBlock;
};

}

#endif//!defined(SS_INTERPRETER)

