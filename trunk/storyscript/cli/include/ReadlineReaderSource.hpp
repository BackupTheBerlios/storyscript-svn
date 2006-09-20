#if !defined(SS_ReadlineReaderSource)
#define SS_ReadlineReaderSource

#include "ReaderSource.hpp"



namespace SS{
	
	
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: Uses the GNU readline library to provide interactive interpreter
 		support.
*/
class ReadlineReaderSource : public ReaderSource
{
public:
	ReadlineReaderSource();
	
	String GetName() const;

private:
	String GetNextLine();
};
	
	
	
} //namespace
#endif

