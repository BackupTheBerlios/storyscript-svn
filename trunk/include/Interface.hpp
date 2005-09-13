
#if !defined(SS_Interface)
#define SS_Interface

#include "Interpreter.hpp"

namespace SS
{




/*~~~~~~~CLASS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Interface
NOTES: This the base class that acts as a buffer between Interpreter and the
game.  The game just has to derive from this.
*/
class SS_API Interface{
public:
	Interface();
	virtual ~Interface();

    
	virtual void StartConversation( const SS::STRING& FileName, const SS::STRING& BlockName = SS::STRING() );

	Interpreter&	   GetInterpreter();
	const Interpreter& GetInterpreter() const;

	//Return the index of the block chosen.
	virtual unsigned int PresentChoice( const std::vector<BlockPointer>& Choices ) = 0;
	
	virtual void HandleParserAnomaly( ParserAnomaly ) = 0;
	virtual void LogMessage( const SS::STRING&, bool UserOutput = false ) = 0;
	
	//So Interpreter can set mpInterpreter without any messiness.
	//As noted in Interpreter.hpp, I will deny to the end that this violates
	//good design.
	friend class Interpreter; 

protected:
	virtual void SayBlock( const BlockPointer ) = 0;

	//void CheckInterpreter();

 	Interpreter& mInterpreter;
};



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: This is provided for cases when you absolutely positively do not want an
 		interface.  For example, if you are using S^2 for config files.
 		Even then though, I would at least provide some error handling.
*/
class SS_API NullInterface
{
public:
	NullInterface();
	virtual ~NullInterface() {}
	
	virtual unsigned int PresentChoice( const std::vector<BlockPointer>& Choices ){
		return 0;
	}
	
	virtual void HandleParserAnomaly(){
		exit(1);
	}
	
	virtual void LogMessage( const SS::STRING&, bool = false ){}
	
protected:
	virtual void SayBlock( const BlockPointer ) {}
};


} //namespace

#endif
