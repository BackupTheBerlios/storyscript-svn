/*
    Slib-Math.hpp
    --------------------------
    Standard library math variables and functions.

	Created:	28:5:2005   22:13
*/


#if !defined(SS_Slib_Math)
#define SS_Slib_Math

#include "Scope.hpp"
#include "Operator.hpp"
#include "SpecialVars.hpp"

namespace SS{
namespace SLib{


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 Math
 NOTES: Special scope that creates an instance of all the math funcs/vars.
*/
class Math : public Scope
{
public:
	Math();

private:
	void RegisterPredefined();
};


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~
// sqrt
// NOTES: The square root function.
//
class sqrt : public Operator
{
public:
	sqrt( const SS::STRING& Name, bool Static = false, bool Const = false )
		: Operator( Name, Static, Const ) {}
	
	VariableBasePointer Operate( VariableBasePointer );
	
};


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 abs
 NOTES: The absolute value function
*/
class abs : public Operator
{
public:
	abs( const SS::STRING& Name, bool Static = false, bool Const = false ) 
		: Operator( Name, Static, Const ) {}

    VariableBasePointer Operate( VariableBasePointer );
};


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 mean
 NOTES: Find the mean of a list of numbers
*/
class mean : public Operator
{
public:
	mean(  const SS::STRING& Name, bool Static = false, bool Const = false )
		: Operator( Name, Static, Const ) {}

	VariableBasePointer Operate( VariableBasePointer );
};

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 sin
 NOTES: The sine function.  (Input is in radians)
*/
class sin : public Operator
{
public:
	sin( const SS::STRING& Name, bool Static = false, bool Const = false )
		: Operator( Name, Static, Const ) {}

	VariableBasePointer Operate( VariableBasePointer );
};

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 cos
 NOTES: The cosine function.  (Input is in radians)
*/
class cos : public Operator
{
public:
	cos( const SS::STRING& Name, bool Static = false, bool Const = false )
		: Operator( Name, Static, Const ) {}

	VariableBasePointer Operate( VariableBasePointer );
};


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 tan
 NOTES: The tangent function.  (Input is in radians.)
*/
class tan : public Operator
{
public:
	tan( const SS::STRING& Name, bool Static = false, bool Const = false )
		: Operator( Name, Static, Const ) {}

	VariableBasePointer Operate( VariableBasePointer );
};


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
sin
NOTES: The arc-sine function.  (Output is in radians)
*/
class asin : public Operator
{
public:
	asin( const SS::STRING& Name, bool Static = false, bool Const = false )
		: Operator( Name, Static, Const ) {}

		VariableBasePointer Operate( VariableBasePointer );
};

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
cos
NOTES: The arc-cosine function.  (Output is in radians)
*/
class acos : public Operator
{
public:
	acos( const SS::STRING& Name, bool Static = false, bool Const = false )
		: Operator( Name, Static, Const ) {}

		VariableBasePointer Operate( VariableBasePointer );
};


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
tan
NOTES: The arc-tangent function.  (Output is in radians.)
*/
class atan : public Operator
{
public:
	atan( const SS::STRING& Name, bool Static = false, bool Const = false )
		: Operator( Name, Static, Const ) {}

		VariableBasePointer Operate( VariableBasePointer );
};


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 max
 NOTES: Returns the maximum from a list of elements.
*/
class max: public Operator
{
public:
	max( const SS::STRING& Name, bool Static = false, bool Const = false )
		: Operator( Name, Static, Const ) {}

	VariableBasePointer Operate( VariableBasePointer );
};


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
min
NOTES: Returns the minimum from a list of elements.
*/
class min: public Operator
{
public:
	min( const SS::STRING& Name, bool Static = false, bool Const = false )
		: Operator( Name, Static, Const ) {}

		VariableBasePointer Operate( VariableBasePointer );
};




/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: Rounds to the nearest integer.  Rounds halfways away from 0.
*/
SS_DECLARE_OPERATOR(Int);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: Rounds to the next lowest integer.
*/
SS_DECLARE_OPERATOR(Floor);



//class MathConstPrec;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: Abstract Base Class that provides the basis for mathematical constant
 		magic vars.
*/
class MathConst : public SpecialVarBase
{
public:
	MathConst( SS_DECLARE_BASE_ARGS );
	
	VarType GetVariableType() const;

	NumType    GetNumData() const;
	BoolType   GetBoolData()   const;
	StringType GetStringData() const;
	
	friend class MathConstPrec;
	
protected:
	virtual void Generate() const = 0;
	
	mutable NumType mBufferValue;
	
};


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: A special version of precision made for MathConst.
*/
class MathConstPrec : public SpecialVarBase
{
	public:
		MathConstPrec( const SS::STRING& Name, MathConst& Parent,
					   bool Static = false, bool Const = false );

		VariableBasePointer operator=( const VariableBase& );
	
		VarType GetVariableType() const;

		NumType GetNumData() const;
		BoolType GetBoolData() const;
		StringType GetStringData() const;
	
	private:
		MathConst& mParent;		
};


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: Magical Pi!
*/
class Pi : public MathConst
{
public:
	Pi( SS_DECLARE_BASE_ARGS );
	
private:
	void Generate() const;
};


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: Magical e!
*/
class Euler : public MathConst
{
public:
	Euler( SS_DECLARE_BASE_ARGS );
	
private:
	void Generate() const;
};
		
		
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CLASS~~~~~~
 NOTES: Magical log2!
*/




}} //namespace(s)
#endif


