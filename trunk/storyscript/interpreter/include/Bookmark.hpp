/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.
Full license information is included in the file in the top
directory named "license".
*/

/**
	\file Bookmark.hpp
	\Declarations for Bookmark.  
*/

#if !defined(SS_Bookmark)
#define SS_Bookmark

#include "Types.hpp"
#include "ReaderSource.hpp"
#include "DLLExport.hpp"

namespace SS{

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
/**
	\brief A placeholder for ReaderSource.
	
	Bookmark holds a place that can be used by the interpreter to jump around
	without breaking anything.	
*/
struct SS_API Bookmark
{
	/// Constructor
	Bookmark();
	
	/**
		\brief Constructor
		
		This looks complicated, but you (the user) should rarely if every
		have to set or use these parameters yourself.
		
		\sa ReaderSource
		
		\param FileName The name of the file or stream.
		\param Position The offset (in "Word"'s) into the stream.
		\param Line The line number at the given position.
		\param CurrentScope The current scope at the given position.
		\param CurrentStaticScope The current static scope at the given position.
		(Take a peek at Interpreter to learn what a "static" scope is.)
	*/
	Bookmark( const SS::String& FileName, ReaderPos Position,
			  unsigned long Line, ScopePtr CurrentScope = ScopePtr(),
			  ScopePtr CurrentStaticScope = ScopePtr() );

	/// Returns true if the bookmark is uninitialized.
	bool IsVoid() const;
	
	/// Comparison operator
	bool operator==( const Bookmark& ) const;
	
	/// Assignment operator
	Bookmark& operator=( const Bookmark& );

	/// FileName The name of the file or stream.
	SS::String FileName;
	/// The offset (in "Word"'s) into the stream.
	ReaderPos Position;
	/// The line number at the given position.
	unsigned long Line;
	/// The current scope at the given position.
	ScopePtr CurrentScope;
	/// The current static scope at the given position.
	ScopePtr CurrentStaticScope;
};

}//namespace SS
#endif

