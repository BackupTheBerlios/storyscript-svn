/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: A specialization of ReaderSource that reads from files.
*/

#if !defined(SS_ReaderSourceFile)
#define SS_ReaderSourceFile

#include "ReaderSource.hpp"
#include "Types.hpp"
#include "Unicode.hpp"

#include <fstream>

namespace SS{

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: The standard interface for reading from files.
*/
class ReaderSourceFile : public ReaderSource
{
public:
	ReaderSourceFile();
	ReaderSourceFile( const SS::String& );
	
	~ReaderSourceFile();
	
	
	void Open( const SS::String& );
	void Close();
	bool IsOpen();
	
	//const SS::String& GetFileName() const;
	String GetName() const;
	
private:
	SS::String GetNextLine();
	SS::String mFileName;
	
	IFSTREAM mFile;
	Char* mpBuffer;
};


typedef boost::shared_ptr<ReaderSourceFile> ReaderSourceFilePtr;


} //namespace SS;
#endif

