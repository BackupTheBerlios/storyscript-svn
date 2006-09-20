/*
Copyright (c) 2004-2006 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.
Full license information is included in the file in the top
directory named "license".
*/

/**
	\file Interpreter.hpp
	\brief Declarations for Interface.  
*/


#if !defined(SS_Interface)
#define SS_Interface

#include "Interpreter.hpp"

namespace SS
{



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
/**
	\brief The base class that interfaces between the Interpreter and your program.
	
	Interface is an abstract base class from which you must derive an interface
	for the interpreter to work with your particular game/program.  For example
	the command line interface (cli) defines an interface which is all text.
	You, on the other hand may want to define an interface that triggers certain
	game events.  For example, you may want to define SayBlock to have characters
	in the game move their mouths an have audio come out.
	
	\see Interpreter
*/
class SS_API Interface
{
public:
	/**
		\brief Constructor
		
		This is the equivalent of passing this to Interpreter::SetInterface.
	*/
	Interface( Interpreter& I );
	
	///Destructor
	virtual ~Interface();

	/**
		\brief Begins executing a conversation file.
		
		\param FileName The name of the conversation file.
		\param BlockName The optional name of the block to begin on.
		
	*/ 
	virtual void StartConversation( const SS::STRING& FileName, const SS::STRING& BlockName = SS::STRING() );

	/// Return a reference to the Interpreter that the interface is attached to.
	Interpreter&	   GetInterpreter();
	
	/// Return a reference to the Interpreter that the interface is attached to.
	const Interpreter& GetInterpreter() const;

	//Return the index of the block chosen.
	
	/**
		\brief Presets conversation choices to the player/user.
		
		You must define this in some way to preset dialog choice to the player.
		
		\param A vector of Block pointers which are the choices
			the player must choose from
			
		\return The index to the chosen block.
	*/
	virtual unsigned int PresentChoice( const std::vector<BlockPtr>& Choices ) = 0;
	
	/**
		\brief Handles anomalies (ie. exceptions) thrown by the interpreter.
		
		If you are feeling lazy, you can just re-throw the exception and
		let the program crash on its own.
		
		\param A The exception to handle.
	*/
	virtual void HandleParserAnomaly( ParserAnomaly A ) = 0;
	
	/**
		\brief Log messages from the interpreter.
		
		The function is responsible for interpreter output.  This included
		user output from the 'print' function.
		
		\param S The output string.
		\param UserOutput This is true if the output if from the 'print' command.
			Otherwise it is unnecessary extra information.
	*/		
	virtual void LogMessage( const SS::STRING& S, bool UserOutput = false ) = 0;
	
	friend class Interpreter; 

protected:
	/**
		\brief Responsible for outputting dialog blocks.
		
		This is the big one.  This is how the user of storyscript defines how
		dialog is actually 'spoken'.
		
		\param B A pointer to the block that is to be 'spoken'.
	*/
	virtual void SayBlock( const BlockPtr ) = 0;

	//void CheckInterpreter();

	///The Interpreter that the interface is attached to.
 	Interpreter& mI;	
 
};



/**
	\brief An extremely minimalist interface.
	
	NullInterface is provided for those cases when you absolutely positively do
	not want to define an interface of your own.  For example, this may be handy
	if you simply want to use storyscript as a fancy config language.
	
	If there are only certain functions you don't want to define, you can simply
	derive your interface from this.
*/
class SS_API NullInterface
{
public:
	///Constructor
	NullInterface();
	///Destructor
	virtual ~NullInterface() {}
	
	virtual unsigned int PresentChoice( const std::vector<BlockPtr>& Choices );
	
	virtual void HandleParserAnomaly();
	
	virtual void LogMessage( const SS::STRING&, bool = false );
	
protected:
	virtual void SayBlock( const BlockPtr );
};


} //namespace

#endif
