/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: 
 This file is specially made so that all the special
 exporting code for windows dll files is in one
 place, and can be turned off all at once when compiling
 on other OS's.
 
 Mainly just all the empty class declarations that are 
 used to export stuff from other libraries.
*/

//Keep this include at the top
#include "Defines.hpp"

#if !defined(SS_DLLExport) && defined(PLAT_WIN32)
#define SS_DLLExport

#include "Types.hpp"
#include <vector>
#include <map>
#include <queue>

#include "DLLExportString.hpp"
#include "Unicode.hpp"
#include "Word.hpp"

 
namespace SS{



//This is slightly unsafe but it will get rid of many unnecessary errors caused by exporting
//STL classes.  It is unnecessary because I don't need to export private memebers.
#pragma warning (disable : 4251)

//This one is the warning that the "extern" before the template is non-standard.
//But msdn told me that I can ignore this, so I will.
#pragma warning (disable : 4231)

//Strings




//Needed for ParserAnomaly
class SS_API std::exception;

 
//A whole crap-load of other stuff.
//I don't even know if I need all of this
class SS_API boost::detail::shared_count;
class SS_API boost::detail::weak_count;


EXPIMP_TEMPLATE template class SS_API boost::shared_ptr<ScopeObject>;
//EXPIMP_TEMPLATE template class SS_API boost::shared_ptr<ScriptFile>;
EXPIMP_TEMPLATE template class boost::shared_ptr<Scope>;
EXPIMP_TEMPLATE template class boost::shared_ptr<Variable>;
EXPIMP_TEMPLATE template class SS_API boost::shared_ptr<Block>;
EXPIMP_TEMPLATE template class SS_API boost::shared_ptr<Interpreter>;


EXPIMP_TEMPLATE template class SS_API boost::weak_ptr<ScopeObject>;
EXPIMP_TEMPLATE template class boost::weak_ptr<Scope>;
EXPIMP_TEMPLATE template class boost::weak_ptr<Variable>;
EXPIMP_TEMPLATE template class boost::weak_ptr<Block>;


//For Scope
EXPIMP_TEMPLATE template class SS_API std::allocator<ScopeObjectPtr>;
EXPIMP_TEMPLATE template class SS_API std::vector<ScopeObjectPtr>;
EXPIMP_TEMPLATE template class SS_API std::allocator<ScopePtr>;
EXPIMP_TEMPLATE template class SS_API std::vector<ScopePtr>;


//For Interpreter
EXPIMP_TEMPLATE template class SS_API std::allocator<BlockPtr>;
EXPIMP_TEMPLATE template class SS_API std::vector<BlockPtr>;

//For ReaderSource
EXPIMP_TEMPLATE template class SS_API std::allocator<Word>;
EXPIMP_TEMPLATE template class SS_API std::vector<Word>;
EXPIMP_TEMPLATE template class SS_API std::allocator<Word>;
EXPIMP_TEMPLATE template class SS_API std::vector<Word>;

EXPIMP_TEMPLATE template class SS_API std::allocator<unsigned long>;
EXPIMP_TEMPLATE template class SS_API std::vector<unsigned long>;
EXPIMP_TEMPLATE template class SS_API std::allocator<unsigned long>;
EXPIMP_TEMPLATE template class SS_API std::vector<unsigned long>;



//For List
//EXPIMP_TEMPLATE template class std::allocator<VariableBasePtr>;
//EXPIMP_TEMPLATE template class std::vector<VariableBasePtr>;




class SS_API boost::detail::shared_count;
EXPIMP_TEMPLATE template class SS_API boost::shared_ptr<Scope>;

}
 
 
#endif

//Reenable some stuff
#pragma warning (default : 4231)
 
