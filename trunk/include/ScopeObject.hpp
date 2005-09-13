/*
ScopeObject.hpp
----------------------

Created: 16:9:2004   15:15
*/

#if !defined(SS_ScopeObject)
#define SS_ScopeObject

#include "Defines.hpp"
#include "Unicode.hpp"
#include "Types.hpp"
#include "DLLExport.hpp"


#include "ScopeObjectVisitor.hpp"
#include <vector>
#include <map>


namespace SS
{

//

//Typedefs and Enumerations
/*
typedef unsigned int ScopeIndex;
static const ScopeIndex INIT_SCOPEINDEX = ~0U; 
//NOTE: ~0 is, of course, 0xffffffff on 32bit machines
//      I can't imagine there being 4,294,967,295 named objects
//      in the scope, so this should suffice.
	*/
	
//extern VariablePointer INITIAL_UID;
#define SS_INITIAL_UID -1



//This is for a special shared_ptr that I don't want deleted.
struct null_deleter
{
	void operator()(void const *) const
	{
	}
};


class Interpreter;

//A little shortcut, considering this may soon change.
//This allows me the treat the base arguments for ScopeObject constructors as one thing.
#define SS_DECLARE_BASE_ARGS const SS::STRING& BaseName, bool IsStatic, bool IsConst
#define SS_BASE_ARGS BaseName, IsStatic, IsConst
#define SS_BASE_ARGS_DEFAULTS UNNAMMED, false, false
extern SS_API const STRING UNNAMMED;



//~~~~~~~CLASS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ScopeObject
// NOTES: The base class for any accessable object in a scope.
//
class SS_API ScopeObject
{
public:
	ScopeObject();
	ScopeObject( const SS::STRING& Name, bool Static = false, bool Const = false );
	virtual ~ScopeObject();

	virtual void AcceptVisitor( ScopeObjectVisitor& );

	bool IsRegistered() const;
	ScopePointer GetParent() const;

	bool IsStatic() const;
	void SetStatic( bool Flag = true );

	bool IsConst() const;
	void SetConst( bool Flag = true );

	void          SetName(const SS::STRING&);
	const SS::STRING& GetName() const;
	SS::STRING        GetFullName() const;
	
	//These two do the same as the above, only they they write to a string
	//provided.  This is a workaround for memory corruption problems
	//involved when using SSI in dll form and not using the multi-threaded DLL
	//form the standard libraries.
	SS::CHAR* GetName( SS::CHAR* Buffer, unsigned int BufferSize ) const;
	SS::CHAR* GetFullName( SS::CHAR* Buffer, unsigned int BufferSize ) const;

	void SetSharedPtr( const ScopeObjectPointer& );

	void UnRegister();


	//Conversion functions.
	ScopeObjectPointer GetScopeObjectPtr();
	const ScopeObjectPointer GetScopeObjectPtr() const;

	virtual ScopePointer GetScopePtr();
	virtual const ScopePointer GetScopePtr() const;

	virtual VariableBasePointer GetVariableBasePtr();
	virtual const VariableBasePointer GetVariableBasePtr() const;

	virtual VariablePointer GetVariablePtr();
	virtual const VariablePointer GetVariablePtr() const;

	virtual BlockPointer GetBlockPtr();
	virtual const BlockPointer GetBlockPtr() const;

	virtual ListPointer GetListPtr();
	virtual const ListPointer GetListPtr() const;
	
	virtual OperatorPointer GetOperatorPtr();
	virtual const OperatorPointer GetOperatorPtr() const;
	


	//I would rather not do this, but...
	friend class Scope;

protected:
	void AssertNonConst();

	SS::STRING mName;
	ScopeObjectPointerWeak mpThis;
	ScopeObjectPointer mpThisFallback;

	bool mStatic;
	bool mConst;

private:
	void ZeroVars();

	NumType mUniqueID;
	
	Scope* mpParent;
};


}//namespace SS
#endif //if !defined(...

