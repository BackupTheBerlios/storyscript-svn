
#if !defined(SS_ListObject)
#define SS_ListObject

#include "ScopeObject.hpp"
#include "Variable.hpp"
#include "Unicode.hpp"
#include "ParserAnomaly.hpp"

#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>



namespace SS{

/*
List syntax.  So I dont forget.

ListBase Foo = 42, "alpha", "vector", 3.141592653;

Foo[0]; //returns 42

Foo-[1]; //removes "alpha" from the list. Return "vector".

Foo+[0] = "omega"; //inserts "omega" after 42.

Foo: length; //returns whatever the length is.

*/




/*~~~~~~~CLASS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ListBase
 NOTES: A language construct that holds a list of Variables.  
		Denoted with the ListBase keyword.  Essentially a wrapper for std::vector
*/
template <typename T>
class ListBase : public VariableBase
{
public:
	typedef typename boost::shared_ptr<T> TPointer;

	ListBase();
	ListBase( const SS::STRING& );
	ListBase( const SS::STRING&, ScopePointer );

	//void AcceptVisitor( ScopeObjectVisitor& );

	void Resize( const BigNumType& );

	void Push( TPointer );
	VariableBasePointer Pop();

	VariableBasePointer operator[]( const VariableBase& );
	TPointer operator[]( unsigned int );
	void operator=( const ListBase<T>& );

	VariableBasePointer Remove( const VariableBase& );
	VariableBasePointer Insert( const VariableBase& ); 

	unsigned int Length() const;


	//TODO: Define these!!
	
	/*
	virtual VariableBasePointer operator+(const VariableBase&) const;
	virtual VariableBasePointer operator-(const VariableBase&) const;
	virtual VariableBasePointer operator*(const VariableBase&) const;
	virtual VariableBasePointer operator/(const VariableBase&) const;
	virtual VariableBasePointer operator=(const VariableBase&);

	virtual VariableBasePointer operator==(const VariableBase&) const;
	virtual VariableBasePointer operator>=(const VariableBase&) const;
	virtual VariableBasePointer operator<=(const VariableBase&) const;
	virtual VariableBasePointer operator> (const VariableBase&) const;
	virtual VariableBasePointer operator< (const VariableBase&) const;
	virtual VariableBasePointer operator&&(const VariableBase&) const;
	virtual VariableBasePointer operator||(const VariableBase&) const;
	*/

	//Unary operators
	virtual VariableBasePointer op_not() const;

	/*
	VarType GetVariableType() const; 

	const BigNumType& GetBigNumData() const;
	const FloatType&  GetFloatData()  const;
	const BoolType&   GetBoolData()   const;
	const StringType& GetStringData() const;
	*/
   

protected:
	std::vector<TPointer> mList;
	
private:
	void RegisterPredefinedVars();
};



}//namespace SS
#endif //!defined(SS_ListObject
