 /*--------------------
 
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
EXPIMP_TEMPLATE template class SS_API boost::shared_ptr<ScriptFile>;
EXPIMP_TEMPLATE template class boost::shared_ptr<Scope>;
EXPIMP_TEMPLATE template class boost::shared_ptr<Variable>;
EXPIMP_TEMPLATE template class SS_API boost::shared_ptr<Block>;
EXPIMP_TEMPLATE template class SS_API boost::shared_ptr<Interpreter>;


EXPIMP_TEMPLATE template class SS_API boost::weak_ptr<ScopeObject>;
EXPIMP_TEMPLATE template class boost::weak_ptr<Scope>;
EXPIMP_TEMPLATE template class boost::weak_ptr<Variable>;
EXPIMP_TEMPLATE template class boost::weak_ptr<Block>;


//For Scope
EXPIMP_TEMPLATE template class SS_API std::allocator<ScopeObjectPointer>;
EXPIMP_TEMPLATE template class SS_API std::vector<ScopeObjectPointer>;
EXPIMP_TEMPLATE template class SS_API std::allocator<ScopePointer>;
EXPIMP_TEMPLATE template class SS_API std::vector<ScopePointer>;


//For Interpreter
EXPIMP_TEMPLATE template class SS_API std::allocator<BlockPointer>;
EXPIMP_TEMPLATE template class SS_API std::vector<BlockPointer>;


//All this...

EXPIMP_TEMPLATE template struct SS_API std::less< SS::STRING >;
EXPIMP_TEMPLATE template class SS_API std::allocator< std::_Tree_nod<std::_Tmap_traits<SS::STRING,SS::ScriptFilePointer,std::less<SS::STRING>,std::allocator<std::pair<const SS::STRING,SS::ScriptFilePointer> >,false> >::_Node >;
EXPIMP_TEMPLATE template class SS_API std::allocator< std::_Tree_ptr<std::_Tmap_traits<SS::STRING,SS::ScriptFilePointer,std::less<SS::STRING>,std::allocator<std::pair<const SS::STRING,SS::ScriptFilePointer> >,false> >::_Nodeptr >;
EXPIMP_TEMPLATE template class SS_API std::allocator< std::pair<const SS::STRING,SS::ScriptFilePointer> >;
//...is just so this will work.
EXPIMP_TEMPLATE template class SS_API std::map< SS::STRING, ScriptFilePointer >;





//For List
//EXPIMP_TEMPLATE template class std::allocator<VariableBasePointer>;
//EXPIMP_TEMPLATE template class std::vector<VariableBasePointer>;




class SS_API boost::detail::shared_count;
EXPIMP_TEMPLATE template class SS_API boost::shared_ptr<Scope>;

}
 
 
#endif

//Reenable some stuff
#pragma warning (default : 4231)
 
