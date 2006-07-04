/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: Contains the abstrct base class Inteface, and and declarations for
	the NullInterface class.
*/


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
	Interface( Interpreter& I );
	virtual ~Interface();

    
	virtual void StartConversation( const SS::STRING& FileName, const SS::STRING& BlockName = SS::STRING() );

	Interpreter&	   GetInterpreter();
	const Interpreter& GetInterpreter() const;

	//Return the index of the block chosen.
	virtual unsigned int PresentChoice( const std::vector<BlockPtr>& Choices ) = 0;
	
	virtual void HandleParserAnomaly( ParserAnomaly ) = 0;
	virtual void LogMessage( const SS::STRING&, bool UserOutput = false ) = 0;
	
	//So Interpreter can set mpInterpreter without any messiness.
	//As noted in Interpreter.hpp, I will deny to the end that this violates
	//good design.
	friend class Interpreter; 

protected:
	virtual void SayBlock( const BlockPtr ) = 0;

	//void CheckInterpreter();

 	Interpreter& mI;	
 
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
	
	virtual unsigned int PresentChoice( const std::vector<BlockPtr>& Choices ){
		return 0;
	}
	
	virtual void HandleParserAnomaly(){
		exit(1);
	}
	
	virtual void LogMessage( const SS::STRING&, bool = false ){}
	
protected:
	virtual void SayBlock( const BlockPtr ) {}
};


} //namespace

#endif
