/*
ScriptFile.hpp
----------------------
This is a wrapper for file I/O.  Instead of binary I/O or character I/O, this
thing does language element I/O.

Created: 31:3:2005   13:04
*/

#if !defined(SS_ScriptFile)
#define SS_ScriptFile


#include "Unicode.hpp"
#include "LanguageConstants.hpp"
#include "DLLExport.hpp"
#include "Word.hpp"
#include "Types.hpp"

#include <fstream>
#include <queue>

namespace SS{


/*~~~~~~~CLASS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Bookmark
NOTES: Stores the a specific location in a script file to allow the interpreter
to jump back and forth without breaking anything.
*/
struct SS_API Bookmark
{
	Bookmark()
		: Position(0), Line(0) {}

		Bookmark( const SS::STRING& FileName, unsigned int Position,
				  unsigned int Line, ScopePointer CurrentScope = ScopePointer(),
				  ScopePointer CurrentStaticScope = ScopePointer() )
			: FileName(FileName), Position(Position), Line(Line), CurrentScope(CurrentScope)
		{
		}

		bool IsVoid() const{
			if( FileName.empty() && Position == 0 && Line == 0 ) return true;
			else return false;
		}

		SS::STRING FileName;
		unsigned int Position;
		unsigned int Line;
		ScopePointer CurrentScope;
		ScopePointer CurrentStaticScope;
};



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 ScriptFile
 NOTES: See the top of the file.  This is resposible for the low-level tokenizing
 		of a script file.
*/
class SS_API ScriptFile
{
public:
	ScriptFile();
	ScriptFile( const SS::STRING& );
	ScriptFile( ScriptFile& );
	
	~ScriptFile();

	void Open( const SS::STRING& );
	void Close();

	Bookmark GetCurrentPos();
	void Goto( const Bookmark& );
	
	const SS::STRING& GetFileName() const;
	bool EndOfFile() const;
	bool IsOpen   ();

	bool SkipWhitespace();
	bool FastForward();

	SS::STRING MakeScopeName();
	
	Word ExtractWord();
	void InsertWord( const Word& );

private:
	SS::CHAR get();

	unsigned int mCurrentLine;
	SS::STRING   mFileName;
	SS::IFSTREAM mFile;

	std::queue< Word > mQueuedWords;

	static const SS::CHAR EOF_CHAR;
};




} //namespace
#endif 

