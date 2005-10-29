/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: Typedefs and such for a bunch of fundamental types.
*/



#if !defined(SS_Types)
#define SS_Types

#include "Defines.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <vector>
#include "Unicode.hpp"



//This only account for the gnu and microsoft implementations,
//it may need tweaking to use stuff like stlport or sgi.
#if defined(SS_USE_HASH_MAP_SCOPES)
	#if defined(PLAT_LINUX)
		#include <ext/hash_map>
		#define STDEXT __gnu_cxx
		#define STLHASH STDEXT::hash
	#else
		#include <hash_map>
		#define STDEXT stdext
		#define STLHASH STDEXT::hash_compare
	#endif
#else
	#include <map>
#endif


#if defined(SS_USE_VECTOR_LISTS)
	#include <vector>
#elif defined(SS_USE_DEQUE_LISTS)
	#include <deque>
#endif



//Disable a bunch of annoying msvc warnings that gmp causes:
#if defined(_MSC_VER)
#pragma warning( disable : 4800 )
#pragma warning( disable : 4146 )
#endif

//#include "gmp.h"
//#include "gmpxx.h"
#include "mpfrxx.h"

//Export this shit
//EXPIMP_TEMPLATE class SS_API __gmp_expr<__gmpfr_value, __gmpfr_value>;
//No, please don't.

#if defined(_MSC_VER)
#pragma warning( default : 4800 )
#pragma warning( default : 4146 )
#endif

namespace SS{

typedef mpfr_class NumType;
typedef bool   BoolType;
typedef SS::STRING StringType;

class ScopeObject;
class Scope;
class Variable;
class Character;
class Block;
class VariableBase;
class List;
class Operator;
class Interpreter;

//DONT TOUCH THESE
//They are used as an index in a static array,
//so it is essential that the numbers don't change.
enum VarType
{
	VARTYPE_NUM = 0,
	VARTYPE_BOOL = 1,
	VARTYPE_STRING = 2
};



typedef boost::shared_ptr<ScopeObject>  ScopeObjectPtr;
typedef boost::shared_ptr<Scope>        ScopePtr;
typedef boost::shared_ptr<Variable>     VariablePtr;
typedef boost::shared_ptr<Block>        BlockPtr;
typedef boost::shared_ptr<VariableBase> VariableBasePtr;
typedef boost::shared_ptr<List>         ListPtr;
typedef boost::shared_ptr<Operator>		OperatorPtr;

typedef boost::weak_ptr<ScopeObject>  ScopeObjectPtrWeak;
typedef boost::weak_ptr<Scope>        ScopePtrWeak;
typedef boost::weak_ptr<Variable>     VariablePtrWeak;
typedef boost::weak_ptr<Block>        BlockPtrWeak;
typedef boost::weak_ptr<VariableBase> VariableBasePtrWeak;
typedef boost::weak_ptr<List>         ListPtrWeak;


enum ScopeObjectType
{
	SCOPEOBJ_NULL,
	SCOPEOBJ_SCOPEOBJECT,
	SCOPEOBJ_SCOPE,
	SCOPEOBJ_CHARACTER,
	SCOPEOBJ_VARIABLEBASE,
	SCOPEOBJ_VARIABLE,
	SCOPEOBJ_BLOCK,
	SCOPEOBJ_LIST,
	SCOPEOBJ_OPERATOR
};


#if defined(SS_USE_VECTOR_LISTS)
typedef std::vector<VariableBasePtr> ListType;
#elif defined(SS_USE_DEQUE_LISTS)
typedef std::deque<VariableBasePtr> ListType;
#endif




#if defined(SS_USE_HASH_MAP_SCOPES)

#if defined(PLAT_LINUX)
	//There is no default hash function for strings, so I'm using my own.
	struct StringHash
	{ 
		size_t operator()(const SS::STRING& s) const
		{ 
			return STLHASH<const CHAR*>()(s.c_str()); 
		} 
	};

	typedef STDEXT::hash_map< SS::STRING, ScopeObjectPtr, StringHash > ScopeListType;
#else
	typedef STDEXT::hash_map< SS::STRING, ScopeObjectPtr > ScopeListType;
#endif

#else
	typedef std::map< SS::STRING, ScopeObjectPtr > ScopeListType;
#endif



}


#endif
