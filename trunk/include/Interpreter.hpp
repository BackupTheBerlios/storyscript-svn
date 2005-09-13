/*
Interpreter.hpp
----------------------

Created: 30:8:2004   13:27
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
	
	void Open( const SS::STRING& FileName );
	void Close();
	
	void Parse( const SS::STRING& BlockName );
	void Parse( BlockPointer, bool SayBlock = true );

	Bookmark GetCurrentPos();
	ScopePointer GetCurrentScope();
	void SetPos( Bookmark& );

    BlockPointer GetFirstBlock();	
	
	ScopeObjectPointer GetScopeObject( const SS::STRING& Name );
	
	ScopeObjectPointer MakeScopeObject( ScopeObjectType, const SS::STRING& Name,
										bool Static = false, bool Const = false );
	
	//These are all shortcuts for outsiders
	VariablePointer GetVariable( const SS::STRING& Name );
	BlockPointer    GetBlock   ( const SS::STRING& Name );
	ScopePointer	GetScope   ( const SS::STRING& Name );

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
	
	ReaderSourceFile& GetFile( const Bookmark& );
	
	static boost::shared_ptr<Interpreter> mpInstance;
	
	void Parse( Bookmark = Bookmark(), bool OneStatement = false, bool IgnoreStatic = true );

	void LoadFile( const SS::STRING& FileName );
	

	void RegisterSpecials();

	bool ParseIf( const Expression&, const Bookmark&, bool OneStatement = false );
	bool ParseWhile( const Expression&, const Bookmark&, bool OneStatement = false );

	Word BreakOffFirstID( Word& );
	Word BreakOffLastID ( Word& ); 

	

	void TackOnScriptInfo( ParserAnomaly& );

	void FastForwardToNextStatement( ReaderSourceFile& );

	Interface* mpInterface;

	bool mVerboseOutput;


	//This is used to keep track of the order of all the blocks in a file.
	std::vector<BlockPointer> mBlockOrder;

	std::map< SS::STRING, ReaderSourceFilePtr > mFiles;

	ReaderSourceFilePtr mpCurrentFile;

	Scope mGlobalScope;
	ScopePointer mpCurrentScope;
	ScopePointer mpCurrentStaticScope;
	
	BlockPointer mpEndBlock;
};

}

#endif//!defined(SS_INTERPRETER)

