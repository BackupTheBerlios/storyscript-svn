/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.
Full license information is included in the file in the top
directory named "license".
*/


/**
	\file Types.hpp
	\Various typedefs and such.
*/



#if !defined(SS_Types)
#define SS_Types

#include "Defines.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <mpfr.h>
#include <vector>
#include "Unicode.hpp"


/**
	Used for boost::shared_ptrs that should not be deleted
	when they go out of scope.
*/
struct null_deleter
{
    void operator()(void const *) const
    {
    }
};



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
//#include "mpfr.h"

//Export this shit
//EXPIMP_TEMPLATE class SS_API __gmp_expr<__gmpfr_value, __gmpfr_value>;
//No, please don't.

#if defined(_MSC_VER)
#pragma warning( default : 4800 )
#pragma warning( default : 4146 )
#endif

namespace SS{


//This way I don't have to worry about init/cleanup
class mpfr_t_wrap
{
public:
	mpfr_t_wrap();
	explicit mpfr_t_wrap( int prec );
	mpfr_t_wrap( const mpfr_t_wrap& X );
	mpfr_t_wrap& operator=( const mpfr_t_wrap& );
	~mpfr_t_wrap();

	mpfr_t& get() const;
	
	mpfr_t_wrap& set( int );
	mpfr_t_wrap& set( signed long );
	mpfr_t_wrap& set( unsigned long );
	mpfr_t_wrap& set( double );
	
	
private:
	mutable mpfr_t N;
};

typedef mpfr_t_wrap NumType; ///< Internal representation of storyscript's number type.
typedef bool   BoolType; ///< Internal representation of storyscript's boolean type.
typedef SS::String StringType; ///< Internal representation of story script's string type.

typedef size_t BlockIndex;

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
	VARTYPE_String = 2
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


typedef std::vector<BlockPtr> BlockList;

/**
	\brief Enumeration of ScopeObject types.
*/
enum ScopeObjectType
{
	SCOPEOBJ_NULL,
	SCOPEOBJ_SCOPEOBJECT,
	SCOPEOBJ_SCOPE,
	SCOPEOBJ_CharACTER,
	SCOPEOBJ_VARIABLEBASE,
	SCOPEOBJ_VARIABLE,
	SCOPEOBJ_BLOCK,
	SCOPEOBJ_LIST,
	SCOPEOBJ_OPERATOR
};

/**
	Internal representation of storyscript lists.
*/
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
		size_t operator()(const SS::String& s) const
		{ 
			return STLHASH<const Char*>()(s.c_str()); 
		} 
	};

	typedef STDEXT::hash_map< SS::String, ScopeObjectPtr, StringHash > ScopeListType;
#else
	typedef STDEXT::hash_map< SS::String, ScopeObjectPtr > ScopeListType;
#endif

#else
	typedef std::map< SS::String, ScopeObjectPtr > ScopeListType;
#endif



}


#endif
