/*
CreationFuncs.hpp
----------------------
Provides a standard and safe way of creating new language objects.

Created: Who fucking cares.
*/

#if !defined(SS_CreationFuncs)
#define SS_CreationFuncs

#include "Scope.hpp"
#include "Variable.hpp"
#include "Block.hpp"

namespace SS{

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: The proper and safe way the create a new object.
 		This only works for some things using the same constructor format as
 		ScopeObject.
*/
template<typename T>
boost::shared_ptr<T> CreateObject( const STRING& Name = SS::STRING(),
								   bool Static = false,
								   bool Const = false )
{
	boost::shared_ptr<T> pNewObj( new T( Name, Static, Const ) );
	
	pNewObj->SetSharedPtr( pNewObj );
	
	return pNewObj;
}




/*
	
	Variable();
	
	Variable( const SS::STRING& Name, const Variable&,
			  bool Static = false, bool Const = false );
	
	Variable( const SS::STRING& Name, const NumType&,
			  bool Static = false, bool Const = false );
	
	Variable( const SS::STRING& Name, const StringType&, 
			  bool Static = false, bool Const = false );
	
	Variable( const SS::STRING& Name, const BoolType&, 
			  bool Static = false, bool Const = false );
			 */


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: For creation of Variables.  CreateObject won't work.
*/
VariablePointer SS_API CreateVariable( SS_DECLARE_BASE_ARGS, const Variable& );
VariablePointer SS_API CreateVariable( SS_DECLARE_BASE_ARGS, const NumType& );
VariablePointer SS_API CreateVariable( SS_DECLARE_BASE_ARGS, const StringType& );
VariablePointer SS_API CreateVariable( SS_DECLARE_BASE_ARGS, const BoolType& );

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
 NOTES: For creation of Blocks.
*/
BlockPointer SS_API CreateBlock( SS_DECLARE_BASE_ARGS, const Bookmark& Position, unsigned int ListIndex );


}//end namespace SS
#endif


