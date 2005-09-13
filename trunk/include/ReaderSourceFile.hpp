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
	ReaderSourceFile( const SS::STRING& );
	
	~ReaderSourceFile();
	
	
	void Open( const SS::STRING& );
	void Close();
	bool IsOpen();
	
	const SS::STRING& GetFileName() const;
	
private:
	SS::STRING GetNextLine();
	SS::STRING mFileName;
	
	IFSTREAM mFile;
	CHAR* mpBuffer;
};


typedef boost::shared_ptr<ReaderSourceFile> ReaderSourceFilePtr;


} //namespace SS;
#endif

