/*
Copyright (c) 2004-2006 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.
Full license information is included in the file in the top
directory named "license".
*/


#include "Expression.hpp"
#include "List.hpp"
#include "Interpreter.hpp"
#include "Interface.hpp"
#include "Operator.hpp"
#include "Unicode.hpp"
#include "CreationFuncs.hpp"
#include <boost/lexical_cast.hpp>

using namespace SS;


const unsigned long BAD_PRECEDENCE = ~0U;


/**
	\brief Magic hack that makes incomplete identifiers possible.
	
	There are cases such as "(Foo):Bar, or even "var Foo", where a nonexistant
	identifier must be evaluated but should not generate an error.  This class
	is part of a little loophole that makes this possible.	
*/
class LooseIdentifier : public VariableBase
{
public:
	LooseIdentifier( const CompoundString& LooseID ) : LooseID( LooseID )
	{}
	
	StringType GetStringData() const{
		ThrowError(); 
		return StringType();
	}
	BoolType GetBoolData() const {
		ThrowError(); 
		return BoolType();
	}
	NumType GetNumData()  const {
		ThrowError(); 
		return NumType();
	}
	
	const CompoundString& GetLooseIDName(){ return LooseID; }
	const STRING GetLooseIDNameSimple(){ return CollapseCompoundString(LooseID); }
	
				
private:
	void ThrowError() const
	{
		STRING Tmp = TXT("Cannot find object named \'");
		Tmp += CollapseCompoundString(LooseID);
		Tmp += TXT("\'.");
		ThrowParserAnomaly( Tmp, ANOMALY_IDNOTFOUND );
	}
	
	CompoundString LooseID;
};


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
Expression::Expression( Interpreter& I )
	: mSyntaxChecked(false), mI(I), mpWordList( new WordList ), mStatic(false)
{
	
}


/*
Expression::Expression()
	: mStatic(false), mpWordList( new WordList ),
      mSyntaxChecked(false)
{
}
*/


Expression::Expression( const Expression& OtherExp, const Bounds& OtherBounds )
	: mpPrecedenceList(OtherExp.mpPrecedenceList),
	  mSyntaxChecked(OtherExp.mSyntaxChecked),
 	  mI(OtherExp.mI),
 	  mBounds(OtherBounds),
 	  mpWordList( OtherExp.mpWordList ),
 	  mStatic(OtherExp.mStatic)
{
	if( mBounds.Upper < mBounds.Lower )
	{
		ThrowParserAnomaly( TXT("UpperBounds is less than LowerBounds in"
							" Expression initialization.  This indicates"
							" a very serious bug."), ANOMALY_PANIC );
	}
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
Expression& Expression::operator=( const Expression& OtherExp )
{
	mpWordList = OtherExp.mpWordList;
	mBounds = OtherExp.mBounds;
	mStatic = OtherExp.mStatic;
	mpPrecedenceList = OtherExp.mpPrecedenceList;
	mSyntaxChecked = OtherExp.mSyntaxChecked;
	return *this;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
bool Expression::IsStatic() const{
	return mStatic;
}

void Expression::SetStatic( bool flag /*= true*/ ) const{
	mStatic = flag;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
Word& Expression::operator[]( unsigned long i )
{
	return (*mpWordList)[ GetAbsoluteIndex(i) ];
}

const Word& Expression::operator[]( unsigned long i ) const
{
	return (*mpWordList)[ GetAbsoluteIndex(i) ];
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Expression::clear()
{
	if( mpWordList.unique() ) mpWordList->clear();
	else mpWordList.reset( new WordList );

	mBounds.Lower = 0;
	mBounds.Upper = 0;

	mStatic = false;
	
	if( mSyntaxChecked ) mSyntaxChecked = false;	
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Expression::push_back( const Word& SomeWord )
{
	RevertToLocalCopy();
	mpWordList->push_back( SomeWord );
	mBounds.Upper++;
	
	if( mSyntaxChecked ) mSyntaxChecked = false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Expression::pop_back()
{
	RevertToLocalCopy();	
	mpWordList->pop_back();
	mBounds.Upper--;
	
	if( mSyntaxChecked ) mSyntaxChecked = false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
bool Expression::empty() const
{
	if( mpWordList->empty() || mBounds.Lower == mBounds.Upper ) return true;
	else return false;
}

unsigned long Expression::size() const
{
	/*
	//Special case, because this give bounds of 0 otherwise
	unsigned long Bounds = mUpperBounds - mLowerBounds;

	if( Bounds < mpWordList->size() ) return Bounds;
	else return (unsigned long)mpWordList->size() - mLowerBounds;
	*/
	
	return (unsigned long)(mBounds.Upper - mBounds.Lower);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Expression::erase( unsigned long i )
{
	RevertToLocalCopy();
	mpWordList->erase( mpWordList->begin() + i );
	mBounds.Upper--;
	
	if( mSyntaxChecked ) mSyntaxChecked = false;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
unsigned long Expression::GetAbsoluteIndex( unsigned long i ) const
{
	unsigned long ProposedIndex = i + mBounds.Lower;
	
	if( ProposedIndex >= mBounds.Upper  ){
		ThrowParserAnomaly( TXT("Expression index is out of range.  This is a serious bug."),
							ANOMALY_PANIC );
	}
	else return ProposedIndex;		
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Expression::RevertToLocalCopy()
{
	if( !mpWordList.unique() )
	{
		boost::shared_ptr<WordList> pNewList( new WordList );

		size_t i;
		for( i = mBounds.Lower; i < mBounds.Upper && i < mpWordList->size(); i++ )
		{
            pNewList->push_back( (*mpWordList)[i] );
		}

		mpWordList = pNewList;
		mBounds.Lower = 0;
		mBounds.Upper = mpWordList->size();
		
		mpPrecedenceList.reset();
	}
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr Expression::Evaluate() const
{
	return InternalEvaluate();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~MONOLITHIC~FUNCTION~~~~~~
VariableBasePtr Expression::InternalEvaluate( 
					 bool TopLevel /*=true*/,
					 ObjectCachePtr pCachedObjects /*= ObjectCachePtr()*/ ) const
{
	/*
		Take care of any business before we get started.
	*/
	
	unsigned long ExpressionSize = size();
	
	if( ExpressionSize == 0 ){
		ThrowParserAnomaly( 
			TXT("Tried to evaluate an empty expression.  Probably a bug, please report. "),
			ANOMALY_PANIC );
	}
	
	if( !mSyntaxChecked ){
		CheckSyntax();
		mSyntaxChecked = true;
	}
	
	if( !pCachedObjects )
	{
		pCachedObjects.reset( new ObjectCache );
		CacheIdentifierObjects( pCachedObjects );
	}
	

    if( StripOutlyingParenthesis() ) ExpressionSize = size();
    
    
    
    
    
    /*
		Handle single word expressions.
	*/
	
	if( ExpressionSize == 1 )
	{
		const Word& FirstWord = GetWord(0);
		
		if( FirstWord.Type == WORDTYPE_IDENTIFIER ) {
			return (*pCachedObjects)[GetAbsoluteIndex(0)]->CastToVariableBase();
		}
		else if( FirstWord.Extra == EXTRA_BOOLLITERAL_True ) {
			return CreateVariable<Variable>( UNNAMMED, true, true );
		}
		else if( FirstWord.Extra == EXTRA_BOOLLITERAL_False ) {
			return CreateVariable<Variable>( UNNAMMED, true, false );
		}
		else if( FirstWord.Type == WORDTYPE_STRINGLITERAL ||
				 FirstWord.Type == WORDTYPE_FLOATLITERAL )
		{
			//Here is where the effectiveness of my autoconversions get tested.
			VariablePtr pTempVar( CreateVariable<Variable>( UNNAMMED, true, (*this)[0].String[0] ) );

			if( FirstWord.Type == WORDTYPE_FLOATLITERAL ) {
				pTempVar->ForceConversion( VARTYPE_NUM );
			}

			return pTempVar;
		}
		else if( FirstWord.Type == WORDTYPE_EMPTYLISTLITERAL )	{
			return gpEmptyList->CastToVariableBase();
		}
		else {
			ThrowParserAnomaly( TXT("Catastraphic error in single word expression evaluation.  Please report this error."), ANOMALY_PANIC );
		}
	}
	
	
	
	/*
		Determine the low precedence operator
	*/
	unsigned long LowPrecedenceOpIndex;
	PrecedenceList::iterator i;
	if( !mpPrecedenceList || (i = mpPrecedenceList->find( mBounds )) == mpPrecedenceList->end()  )
	{
		LowPrecedenceOpIndex = (unsigned long)CalculateLowPrecedenceOperator(pCachedObjects);
		
		if( !mpPrecedenceList ) mpPrecedenceList.reset( new PrecedenceList );
		
		(*mpPrecedenceList)[mBounds] = LowPrecedenceOpIndex;
	}
	else
	{
		LowPrecedenceOpIndex = (*mpPrecedenceList)[mBounds];
	}
	
	
	const ExtraDesc& LowPrecedenceOp = GetWord( LowPrecedenceOpIndex ).Extra;
	//Wow that was easy =)
	
	
	
	/*
		Split the expression in two parts: everything left of the the low precedence op,
		and everything right of it.
	*/
	
	const Expression Left( *this, Bounds( GetAbsoluteIndex(0), GetAbsoluteIndex(LowPrecedenceOpIndex) ) );

	size_t RightLower, RightUpper;
	if( LowPrecedenceOpIndex + 1 < ExpressionSize )	{ 
		RightLower = GetAbsoluteIndex(LowPrecedenceOpIndex + 1);
		RightUpper = mBounds.Upper;		
	}
	else{
		RightLower = 0;
		RightUpper = 0;
	}

	const Expression Right( *this, Bounds( RightLower, RightUpper ) );

	if( IsStatic() ){
		Left.SetStatic();
		Right.SetStatic();
	}
	


	VariableBasePtr pLeftVar, pRightVar, pResultant;

	
	
	/*
		Special Case for handling short-circuting of logical operators.
		This prevents the right side from being evaluated if the left side is true/false.
	*/
	
	if( LowPrecedenceOp == EXTRA_BINOP_LogicalOr ||
		LowPrecedenceOp == EXTRA_BINOP_LogicalAnd )
	{
		pLeftVar =	Left.InternalEvaluate( false, pCachedObjects ); 		
		
		if( LowPrecedenceOp == EXTRA_BINOP_LogicalOr && pLeftVar->GetBoolData() == true ){
			return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, true );
		}
		else if( LowPrecedenceOp == EXTRA_BINOP_LogicalAnd && pLeftVar->GetBoolData() == false ){
			return CreateVariable<Variable>( SS_BASE_ARGS_DEFAULTS, false );
		}
	
		//Shit, no short-circuting necessary so now we have to deal with the right side!
		//Don't worry, if we just let it fall through,
		//and stop it from re-evaluating the left-side, we'll be fine.
	}
	
	
	
	/*
		Evaluate the right side expression.
	*/
	if( !Right.empty() ) {
		pRightVar = Right.InternalEvaluate( false, pCachedObjects );
			
	}
	//Somehow a trailing operator got flagged as the low precedence op.
	else {
		ThrowExpressionAnomaly( TXT("Trailing operator with no argument."),
								ANOMALY_BADGRAMMAR );
	}
	
	
	
	/*
		Evaluate a functions/usr-ops.
	*/
	if( GetWord( LowPrecedenceOpIndex ).Type == WORDTYPE_IDENTIFIER )
	{
		//TODO: (Known Issue) Right now syntax like: "(SSMath):sin 0.5" is not supported.
		//I'm going to have to find a work around for this, but I don't know of one off the
		//op of my head that won't hurt performance.

		
		OperatorPtr pOp = (*pCachedObjects)[ GetAbsoluteIndex(LowPrecedenceOpIndex) ]->CastToOperator();
		
		VariableBasePtr ReturnVal = pOp->Operate( pRightVar );
		if( !Left.empty() ){
			Left.InternalEvaluate( false, pCachedObjects );
		}
		
		return ReturnVal;
	}
	
	
	/*
		(Hard-coded) Unary Operators.
	*/
	else if( GetWord( LowPrecedenceOpIndex ).Type == WORDTYPE_UNARYOPERATOR )
	{
		VariableBasePtr ReturnVal = EvaluateUnaryOp( GetWord( LowPrecedenceOpIndex ).Extra, pRightVar );
		if( !Left.empty() ){
			Left.InternalEvaluate( false, pCachedObjects );
		}
		
		return ReturnVal;
	}
	
	
	/*
		Evaluate the left side expression
	*/
	if( !Left.empty() ) {
		if( !pLeftVar ){
			pLeftVar = Left.InternalEvaluate( false, pCachedObjects );
		}
	}
	//Binary operator without a left operand.
	else {
		ThrowExpressionAnomaly( TXT("Binary operator found without a left operand."),
								ANOMALY_BADGRAMMAR );
	}
	
	
	/*
		Binary Operators
	*/
	if( GetWord( LowPrecedenceOpIndex ).Type == WORDTYPE_BINARYOPERATOR )
	{
		return EvaluateBinaryOp( GetWord( LowPrecedenceOpIndex ).Extra, pLeftVar, pRightVar );
	}
	
	
	/*
		Whoa! WTF!  The LPO isn't a unary-operator, a binary-operator, or a function!
	*/
	ThrowExpressionAnomaly( TXT("Catastrophicly, horrificly, terrifyingly bad bug in the "
								"expression evaluater.  Report this, please!"), ANOMALY_PANIC );
	return VariableBasePtr(); //To placate the compiler.
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Expression::CheckSyntax( bool IgnoreTrailingOps /*=false*/ ) const
{
	const Word* pPrevWord = &NULL_WORD;
	const Word* pCurrentWord = &NULL_WORD;
	
	const unsigned long ExpressionSize = size();

	unsigned int i;
	unsigned int ParenthesisCount = 0;
	for( i = 0; i < ExpressionSize; i++, pPrevWord = pCurrentWord )
	{
		pCurrentWord = &(*this)[i];

		//RULE: All ')' must have a matching '('
		if( pCurrentWord->Extra == EXTRA_PARENTHESIS_Left ) ParenthesisCount++;
		else if( pCurrentWord->Extra == EXTRA_PARENTHESIS_Right )
		{
			if( ParenthesisCount == 0 )
			{
				ThrowExpressionAnomaly( 
					TXT("Extra \')\'.  Can't find the matching \'(\'."),
					ANOMALY_BADPUNCTUATION );
			}
			else ParenthesisCount--;
		}

		//RULE: Expressions must begin with an identifier, literal, or '(', or (unary) operator
		if( pPrevWord->Type == WORDTYPE_UNKNOWN && 
			!(pCurrentWord->Type == WORDTYPE_IDENTIFIER || 
			pCurrentWord->IsLiteral() ||
			pCurrentWord->Extra == EXTRA_PARENTHESIS_Left ||
			pCurrentWord->Type == WORDTYPE_UNARYOPERATOR ||
			pCurrentWord->Type == WORDTYPE_AMBIGUOUSOPERATOR )  )
		{
			ThrowExpressionAnomaly( 
				TXT("Expressions must begin with an identifier, "
				"literal, or unary operator."),
				ANOMALY_BADGRAMMAR );
		}
		//RULE: Either an identifier, literal, or '(' must follow an operator
		else if( (pPrevWord->Type == WORDTYPE_BINARYOPERATOR || 
				  pPrevWord->Type == WORDTYPE_UNARYOPERATOR ||
				  pPrevWord->Type == WORDTYPE_AMBIGUOUSOPERATOR ) &&
				  !( pCurrentWord->IsLiteral() || pCurrentWord->Type == WORDTYPE_IDENTIFIER ||
				 pCurrentWord->Extra == EXTRA_PARENTHESIS_Left || pCurrentWord->Type == WORDTYPE_UNARYOPERATOR ||
				 pCurrentWord->Type == WORDTYPE_AMBIGUOUSOPERATOR ) )
		{
			ThrowExpressionAnomaly( 
				TXT("Either an identifier, literal, or a unary "
				"operator must follow an operator"),
				ANOMALY_BADGRAMMAR );
		}
		//RULE: operators can not be followed by ')' or preceded by '('.  (i.e. No "x (+) b" stuff)
		else if( pPrevWord->Extra == EXTRA_PARENTHESIS_Left &&
				 pCurrentWord->Type == WORDTYPE_BINARYOPERATOR )
		{
			ThrowExpressionAnomaly( TXT("Operators cannot be preceded by a \'(\'."), ANOMALY_BADGRAMMAR );
		}
		else if( (pPrevWord->Type == WORDTYPE_BINARYOPERATOR ||
				  pPrevWord->Type == WORDTYPE_UNARYOPERATOR ||
				  pPrevWord->Type == WORDTYPE_AMBIGUOUSOPERATOR) &&
				 pCurrentWord->Extra == EXTRA_PARENTHESIS_Right )
		{
			ThrowExpressionAnomaly( TXT("Operators cannot be followed by a \')\'."), ANOMALY_BADGRAMMAR );
		}
		
		
	
	}

	//RULE: The last word must be an identifier, literal, or ')'
	if( !IgnoreTrailingOps )
	{
		if( !( pCurrentWord->IsLiteral() || pCurrentWord->Type == WORDTYPE_IDENTIFIER ||
			pCurrentWord->Extra == EXTRA_PARENTHESIS_Right ) )
		{
			ThrowExpressionAnomaly( TXT("Last word in an expression must be an identifier or literal."),
									ANOMALY_BADGRAMMAR );
		}
	}

	//RULE: All '(' must have a matching ')'
	if( ParenthesisCount != 0 )
	{
		ThrowExpressionAnomaly( 
			TXT("Extra \'(\'. Can't find the matching \')\', or the matching "
				"\')\' is outside of the expression"),
			ANOMALY_BADGRAMMAR );
	}
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
bool Expression::StripOutlyingParenthesis() const
{
	if( (*this)[0].Extra != EXTRA_PARENTHESIS_Left ) return false;
	
	const unsigned long ExpressionSize = size();
	const Word* pTempWord = 0;
	
	unsigned int i, B, E, P;
	bool InBeginning = true;
	
	for( i = B = E = P = 0; i < ExpressionSize; i++ )
	{
		pTempWord = &(*this)[i];	 
		
		if( pTempWord->Extra == EXTRA_PARENTHESIS_Left )
		{
			if( InBeginning ) B++;
			else              P++;
		}
		else if( pTempWord->Extra == EXTRA_PARENTHESIS_Right )
		{
			if( P == 0 && i + B < ExpressionSize ) B--;
			else if( P > 0 )                       P--;
			else                                   E++;	
		}
		else InBeginning = false;
	}
	
	if( E != B || P != 0 )
	{
		ThrowParserAnomaly( TXT("Parenthesis mismatch."), ANOMALY_BADGRAMMAR );	
		return false; // Just to placate compilers.
	}
	
	if( B > 0 )
	{
		mBounds.Lower += B;
		mBounds.Upper -= B;
		return true;
	}
	else return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Expression::ThrowExpressionAnomaly( const STRING& Desc, AnomalyCode Code ) const
{
	STRING FullDesc = TXT("Bad expression: \'");
	FullDesc += DumpToString();
	FullDesc += TXT("\' --- ");
	FullDesc += Desc;
	
	ThrowParserAnomaly( FullDesc, Code );	
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
STRING Expression::DumpToString() const
{
	STRING Out;
	unsigned int i;
	for( i = 0; i < size(); i++ )
	{
		//TODO: This is now broken since I no longer dump everything into the
		//		string member. (i.e. "foo + bar" will end up looking like "foo  bar")

		const Word& Temp = (*this)[i];

		switch( Temp.Type )
		{
		case WORDTYPE_BINARYOPERATOR:
			Out += gBinaryOperatorReverseMap[ Temp.Extra ];
			break;

		case WORDTYPE_UNARYOPERATOR:
			Out += gUnaryOperatorReverseMap[ Temp.Extra ];
			break;

		case WORDTYPE_STRINGLITERAL:
			Out += TXT("\"");
			Out += Temp.String[0];
			Out += TXT("\"");
			break;

		case WORDTYPE_BOOLLITERAL:
			Out += Temp.Extra == EXTRA_BOOLLITERAL_True ? LC_True : LC_False;
			break;

		case WORDTYPE_EMPTYLISTLITERAL:
			Out += TXT("()");
			break;

		//These shouldn't every be in an expression, but I might as well...
		case WORDTYPE_BRACKET:
			Out += Temp.Extra == EXTRA_BRACKET_Left ? TXT("{") : TXT("}");
			break;

		case WORDTYPE_PARENTHESIS:
			Out += Temp.Extra == EXTRA_PARENTHESIS_Left ? TXT("(") : TXT(")");
			break;

		default:
			Out += Temp.String[0];
		}

		Out += TXT(" ");
	}
	
	return Out;	
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
Expression::OperatorPrecedence Expression::GetPrecedenceLevel( const Word& W ) const
{
		//A list of operators in order of lowest to highest precedence

	static bool FirstRun = true;
	static std::map< ExtraDesc, int > PrecedenceList;

	if( FirstRun )
	{
		PrecedenceList[EXTRA_BINOP_Assign]            = 1;
		PrecedenceList[EXTRA_BINOP_MinusAssign]       = 2;
		PrecedenceList[EXTRA_BINOP_PlusAssign]        = 3;
		PrecedenceList[EXTRA_BINOP_ConcatAssign]		  = 3;
		PrecedenceList[EXTRA_BINOP_DivideAssign]      = 4;
		PrecedenceList[EXTRA_BINOP_TimesAssign]       = 5;
		PrecedenceList[EXTRA_BINOP_ExponentAssign]    = 6;
		PrecedenceList[EXTRA_BINOP_LogicalAnd]        = 7;
		PrecedenceList[EXTRA_BINOP_LogicalOr]         = 8;
		PrecedenceList[EXTRA_BINOP_Equals]            = 9;
		PrecedenceList[EXTRA_BINOP_NotEquals]         = 9;
		PrecedenceList[EXTRA_BINOP_LessThanOrEqual]   = 10;
		PrecedenceList[EXTRA_BINOP_LargerThanOrEqual] = 10;
		PrecedenceList[EXTRA_BINOP_LessThan]          = 11;
		PrecedenceList[EXTRA_BINOP_LargerThan]        = 11;
		
		/*
			Where to put the precedence of unary operators has been a great
			concern for me.  For now it is right here, below all the math
			operators and above logical comparisons.  
		*/
		PrecedenceList[EXTRA_UNOP_GenericUnaryOperator] = 12;
		
		
		PrecedenceList[EXTRA_BINOP_ListSeperator]     = 13;
		PrecedenceList[EXTRA_BINOP_Minus]             = 14;
		PrecedenceList[EXTRA_BINOP_Concat]			  = 15;
		PrecedenceList[EXTRA_BINOP_Plus]              = 15;
		PrecedenceList[EXTRA_BINOP_Divide]            = 16;
		PrecedenceList[EXTRA_BINOP_Times]             = 17;
		PrecedenceList[EXTRA_BINOP_Exponent]          = 18;
		
		PrecedenceList[EXTRA_UNOP_Negative]           = 19;
        PrecedenceList[EXTRA_UNOP_Not]                = 19;

		PrecedenceList[EXTRA_UNOP_Var]				  = 19;
		PrecedenceList[EXTRA_UNOP_List]				  = 19;
		PrecedenceList[EXTRA_UNOP_Character]		  = 19;
		PrecedenceList[EXTRA_UNOP_Player]			  = 19;		


		PrecedenceList[EXTRA_BINOP_ListAccess]        = 20; 
		PrecedenceList[EXTRA_BINOP_ListAppend]        = 20;
		PrecedenceList[EXTRA_BINOP_ListRemove]        = 20;

		//Always keep these at the highest
		PrecedenceList[EXTRA_BINOP_ScopeResolution]   = 21;
		
		FirstRun = false;
	}


	//Here we assume it is a function (an Operator or Block), and return the highest precedence.
	if( PrecedenceList.find( W.Extra ) == PrecedenceList.end() )
	{
		return PrecedenceList[EXTRA_UNOP_GenericUnaryOperator];
	}

	return PrecedenceList[ W.Extra ];
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
size_t Expression::CalculateLowPrecedenceOperator( ObjectCachePtr pCache ) const
{
	/*
		Its important to keep track of what the last word was,
		so these are simple classifications only used by this function.
	*/
	enum SimpleType{
		UNARY_OP, //built-in, or functions
		BINARY_OP,
		OPERAND, //literals, identifiers, etc
		RIGHT_PAREN, //closing parenthesis
		OTHER //nothing/other
	};	
	
	
	unsigned long ParenthesisCount = 0;
	size_t LowPrecedenceOpIndex = BAD_PRECEDENCE;
	SimpleType LastWordType = OTHER;
	
	const Word* pCurrentWord;
	const Word* pLowPrecWord;
	
	static Word NullWord;
	
	//const unsigned long ExpressionSize = size();
	
	//We are using absolute indexes because its faster, and this function is slowing us down.
	size_t i = mBounds.Lower;
	
	for( ; i < mBounds.Upper && i < mpWordList->size(); i++ )
	{
		pCurrentWord = &(*mpWordList)[ i ];
		if( LowPrecedenceOpIndex != BAD_PRECEDENCE ) pLowPrecWord = &(*mpWordList)[ LowPrecedenceOpIndex ];
		else pLowPrecWord = &NullWord;
		
		
		/*
		  Skip through parenthesis. Don't worry about mismatches.
		  They were already checked handled by CheckSyntax();
		*/
				
		if( pCurrentWord->Extra == EXTRA_PARENTHESIS_Left ){
			ParenthesisCount++;
			LastWordType = OTHER;
			continue;
		}
		else
		if( pCurrentWord->Extra == EXTRA_PARENTHESIS_Right ){			
			ParenthesisCount--;	
			LastWordType = RIGHT_PAREN;
			continue;
		}
		
		if( ParenthesisCount != 0 ) continue;
		
		
		
		
		/*
			Ambigous operators (operators that can be either binary or unary
			depending on the contex) are intercepted here and their identity
			determined.		
		*/
		if( pCurrentWord->Type == WORDTYPE_AMBIGUOUSOPERATOR )
		{
			if( i == 0 || LastWordType == UNARY_OP || LastWordType == BINARY_OP )
			{
				pCurrentWord->InterpretAsUnaryOp();
			}
			else
			{
				pCurrentWord->InterpretAsBinaryOp();	
			}			
		}
		
		
		
		/*
			Here we determine if identifiers are functions or just variables, etc.
		*/
		if( pCurrentWord->Type == WORDTYPE_IDENTIFIER )
		{
			if( ! (*pCache)[ i ] ){
				LastWordType = OPERAND;
				continue;
			}
			
			ScopeObjectType ObjType = GetScopeObjectType( (*pCache)[ i ] );
			if( ObjType == SCOPEOBJ_OPERATOR || ObjType == SCOPEOBJ_BLOCK )
			{
				//The current word is not a function if it is at the end of the expression,
				//followed by by a binary operator, or followed by a closing parenthesis.
				if( i == mBounds.Upper-1 ||
					(*mpWordList)[ i+1 ].Type == WORDTYPE_BINARYOPERATOR ||
					(*mpWordList)[ i+1 ].Extra == EXTRA_PARENTHESIS_Right )
				{
					LastWordType = OPERAND;
					continue;
				}
				
				//Unary operators will not take low precedence over each other.
				//They are always parsed left->right.  Hence we don't even test
				//the precedence if the last word was also a unary operator.
				if( LastWordType != UNARY_OP &&
					(LowPrecedenceOpIndex == BAD_PRECEDENCE ||
					(GetPrecedenceLevel( *pCurrentWord ) <= GetPrecedenceLevel( *pLowPrecWord ) &&
					
					/*Unary operators are not alowed to take low precedence from
					  a binary operator.  Consider the expression:
					      a + -b
					  If '-' gets flaged as the low precedence operator it will leave
					  on the left side 'a +', which is not a valid expression.  This solves
					  that problem.
					*/
					pLowPrecWord->Type != WORDTYPE_BINARYOPERATOR))
				  )
				{
					LowPrecedenceOpIndex = i;
				}
				
				LastWordType = UNARY_OP;
				continue;				
			}
			else
			{			
				LastWordType = OPERAND;
				continue;
			}
		}
		
		
		/*
			(Hard-coded) Unary Operators. 
		*/
		else if( pCurrentWord->Type == WORDTYPE_UNARYOPERATOR )
		{
			if( LastWordType != UNARY_OP &&
				(LowPrecedenceOpIndex == BAD_PRECEDENCE ||
				(GetPrecedenceLevel( *pCurrentWord ) <= GetPrecedenceLevel( *pLowPrecWord ) &&
				pLowPrecWord->Type != WORDTYPE_BINARYOPERATOR))
			  )				
			{
				LowPrecedenceOpIndex = i;
			}
			
			LastWordType = UNARY_OP;
			continue;				
		}
		
		
		/*
			Binary Operators.
		*/
		else if( pCurrentWord->Type == WORDTYPE_BINARYOPERATOR )
		{
			if( LowPrecedenceOpIndex == BAD_PRECEDENCE ||
				GetPrecedenceLevel( *pCurrentWord ) <= GetPrecedenceLevel( *pLowPrecWord ) )
			{
				LowPrecedenceOpIndex = i;
			}
			
			LastWordType = BINARY_OP;
			continue;			
		}
		
		
		/*
			Anything else get skipped over.  I'm really not sure if
			anything can possibly trigger this, but better safe than sorry.
		*/
		else
		{
			LastWordType = OTHER;
			continue;	
		}
	}
	
	
	if( LowPrecedenceOpIndex == BAD_PRECEDENCE ){
		ThrowExpressionAnomaly( TXT("Cannot find an operator in this expression."), ANOMALY_NOOPERATOR );
		return 0; //To placate the compiler.
	}
	else return LowPrecedenceOpIndex - mBounds.Lower;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
Word& Expression::GetWord( unsigned long i ){
	return (*mpWordList)[ GetAbsoluteIndex(i) ];
}

const Word& Expression::GetWord( unsigned long i ) const{
	return (*mpWordList)[ GetAbsoluteIndex(i) ];
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
void Expression::CacheIdentifierObjects( ObjectCachePtr pCache ) const
{
	size_t i;
	
	const size_t ExpressionSize = mpWordList->size();
	
	for( i = 0; i < ExpressionSize; i++ )
	{
		if( (*mpWordList)[i].Type == WORDTYPE_IDENTIFIER )
		{
			//If the identifier comes right after a scope resolution operator (:),
			//don't even try to get it, because it won't be the one we want.
			//Just returns a LooseID.				
			if( i > 0 && (*mpWordList)[ i-1 ].Extra == EXTRA_BINOP_ScopeResolution )
			{
				ScopeObjectPtr pTmpPtr( new LooseIdentifier( (*mpWordList)[i].String ) );
				pTmpPtr->SetSharedPtr( pTmpPtr );
				
				(*pCache)[i] = pTmpPtr;
				
				continue;
			}			
			
			
			try{
				ScopeObjectPtr pTmpPtr =
				mI.GetScopeObject( (*mpWordList)[i].String );
				
				(*pCache)[i] = pTmpPtr;
			}
			catch( ParserAnomaly E )
			{
				if( E.ErrorCode == ANOMALY_IDNOTFOUND )
				{
					//We don't throw an error yet, because this may be a variable/block/character
					//declaration.  Just create a LooseID and it will get taken care of later.
					ScopeObjectPtr pTmpPtr( new LooseIdentifier( (*mpWordList)[i].String ) );
					pTmpPtr->SetSharedPtr( pTmpPtr );
					
					(*pCache)[i] = pTmpPtr;
				}
				else throw;				
			}
		} 
		
	}//end for	
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr Expression::EvaluateUnaryOp ( ExtraDesc Op, VariableBasePtr pRight ) const
{
	//not
	if( Op == EXTRA_UNOP_Not ){
		return pRight->op_not();
	}
	//- (negate)
	else if( Op == EXTRA_UNOP_Negative ){
		return pRight->op_neg();
	}
	else if( Op == EXTRA_UNOP_ScopeResolution ){
		ScopePtr pTmp = mI.GetGlobalScope();
		return pTmp->GetScopeObject( MakeCompoundID( pRight->GetStringData() ) )->CastToVariableBase();	
	}	
	//Declarations
	else if( Op == EXTRA_UNOP_Var ||
			 Op == EXTRA_UNOP_List ||
			 Op == EXTRA_UNOP_Character ||
			 Op == EXTRA_UNOP_Player )
	{
		boost::shared_ptr<LooseIdentifier> pLooseID;
		if( pLooseID = boost::dynamic_pointer_cast<LooseIdentifier>( pRight ) )
		{
			if( Op == EXTRA_UNOP_Var )
			{
				return
					mI.MakeScopeObject( 
					SCOPEOBJ_VARIABLE, pLooseID->GetLooseIDName(), IsStatic() )->CastToVariableBase();
			}
			else if( Op == EXTRA_UNOP_List )
			{
				return
					mI.MakeScopeObject( 
					SCOPEOBJ_LIST, pLooseID->GetLooseIDName(), IsStatic() )->CastToVariableBase();
			}
			else if( Op == EXTRA_UNOP_Player )
			{
				//HACK: This says player, but currently it just creates a standard character.
				//		At this point there is no spereation between players and characters.
				return
					mI.MakeScopeObject( 
					SCOPEOBJ_CHARACTER, pLooseID->GetLooseIDName(), IsStatic() )->CastToVariableBase();
			}
			else if( Op == EXTRA_UNOP_Character )
			{
				return
					mI.MakeScopeObject( 
					SCOPEOBJ_CHARACTER, pLooseID->GetLooseIDName(), IsStatic() )->CastToVariableBase();
			}
			else
			{
				//This will throw an error saying that the identifier cannot be found.
				pRight->GetStringData();
				
				//This point never reached.  This is just to placate the compiler.
				return VariableBasePtr();
			}
		}
		//Trying to declare a variable with something other than an unused identifier.
		else
		{
			ThrowExpressionAnomaly( TXT("Bad declaration syntax.  The identifier you used is already"
										"in use (or it isn't even an identifier)."), ANOMALY_BADDECLARATION );
		}
	}
	
	ThrowExpressionAnomaly( TXT("Unhandled hard-coded unary operator.  Please report this."),
								ANOMALY_PANIC );
	return VariableBasePtr(); //To placate the compiler.
	
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FUNCTION~~~~~~
VariableBasePtr Expression::EvaluateBinaryOp( ExtraDesc Op, VariableBasePtr pLeft,
											  VariableBasePtr pRight ) const
{
	// -
	if( Op == EXTRA_BINOP_Minus ){
		return *pLeft - *pRight;
	}
	// +
	else if( Op == EXTRA_BINOP_Plus ){
		return *pLeft + *pRight;
	}
	// /
	else if( Op == EXTRA_BINOP_Divide ){
		return *pLeft / *pRight;
	}
	// *
	else if( Op == EXTRA_BINOP_Times ){
		return *pLeft * *pRight;
	}
	// **
	else if( Op == EXTRA_BINOP_Exponent ){
		return pLeft->operator_pow( *pRight );
	}
	// >
	else if( Op == EXTRA_BINOP_LargerThan ){
		return *pLeft > *pRight;
	}
	// <
	else if( Op == EXTRA_BINOP_LessThan ){
		return *pLeft < *pRight;
	}
	// >=
	else if( Op == EXTRA_BINOP_LargerThanOrEqual ){
		return *pLeft >= *pRight;
	}
	// <=
	else if( Op == EXTRA_BINOP_LessThanOrEqual ){
		return *pLeft <= *pRight;
	}
	// ==
	else if( Op == EXTRA_BINOP_Equals ){
		return *pLeft == *pRight;
	}
	// !=
	else if( Op == EXTRA_BINOP_NotEquals ){
		return *pLeft != *pRight;
	}
	// &=
	else if( Op == EXTRA_BINOP_LogicalAnd ){
		return *pLeft && *pRight;
	}
	// ||
	else if( Op == EXTRA_BINOP_LogicalOr ){
		return *pLeft || *pRight;
	}
	// -=
	else if( Op == EXTRA_BINOP_MinusAssign ){
		return (*pLeft = *(*pLeft - *pRight));
	}
	// +=
	else if( Op == EXTRA_BINOP_PlusAssign ){
		return (*pLeft = *(*pLeft + *pRight));
	}
	// /=
	else if( Op == EXTRA_BINOP_DivideAssign ){
		return (*pLeft = *(*pLeft / *pRight));
	}
	// *=
	else if( Op == EXTRA_BINOP_TimesAssign ){
		return (*pLeft = *(*pLeft * *pRight));
	}
	// **=
	else if( Op == EXTRA_BINOP_ExponentAssign ){
		return (*pLeft = *(pLeft->operator_pow( *pRight ) ) );
	}
	else if( Op == EXTRA_BINOP_Concat ){
		return pLeft->operator_concat( *pRight );
	}
	else if( Op == EXTRA_BINOP_ConcatAssign ){
		return (*pLeft = *(pLeft->operator_concat( *pRight ) ) );
	}
	// =
	else if( Op == EXTRA_BINOP_Assign ){
		return (*pLeft = *pRight);
	}
	// []
	else if( Op == EXTRA_BINOP_ListAccess )
	{		
		return (*(pLeft->CastToList()))[ pRight ];
	}
	// +[]
	else if( Op == EXTRA_BINOP_ListAppend )
	{
		return pLeft->CastToList()->Insert( pRight );
	}
	// -[]
	else if( Op == EXTRA_BINOP_ListRemove )
	{
		return pLeft->CastToList()->Remove( pRight );
	}
	// ,
	else if( Op == EXTRA_BINOP_ListSeperator )
	{
		//Third try's the charm.
		
		if( GetScopeObjectType( pLeft ) == SCOPEOBJ_LIST && //If pLeft is a list
			pLeft->IsConst() && 						   //that is constant
			pLeft->GetName() == UNNAMMED ) 			   //and unnamed.
		{
			//If the right var is an actual variable and not something special.
			if( GetScopeObjectType( pRight ) == SCOPEOBJ_VARIABLE )
			{
				pLeft->CastToList()->PushWithoutCopy( pRight );
			}
			//If it is something special, just make a new variable and copy the value of rightvar.
			else
			{
				pLeft->CastToList()->Push( pRight );
			}
			
			return pLeft;
		}
		//pLeft is not a list, so we have to create a new one.
		else
		{
			ListPtr pNewList( CreateGeneric<List>( UNNAMMED, true ) );
			
			//If its a real variable, append the actual pointer,
			//otherwise make a copy of the value.
			if( GetScopeObjectType( pLeft ) == SCOPEOBJ_VARIABLE ){
				 pNewList->PushWithoutCopy( pLeft );
			}
			else pNewList->Push( pLeft );
			
			if( GetScopeObjectType( pRight ) == SCOPEOBJ_VARIABLE ){
				 pNewList->PushWithoutCopy( pRight );
			}
			else pNewList->Push( pRight );
			
			return pNewList->CastToVariableBase();			
		}
		
		//Otherwise create a new a list and append the actual pointers
		
	}
	// :
	else if( Op == EXTRA_BINOP_ScopeResolution )
	{
		//Try to interpret it as a LooseID
		boost::shared_ptr<LooseIdentifier> pRightLooseID = boost::dynamic_pointer_cast<LooseIdentifier>(pRight);
		if( pRightLooseID )
		{
			return (pLeft->GetScopeObject( pRightLooseID->GetLooseIDName() ))->CastToVariableBase();
		}
		//Otherwise just assume its a string literal of some
		//TODO: Is this really a good idea?  What if someone puts a variable after a scope resolutio operator?
		//		Won't it read the string from the variable and try to access that variable.  Is that what I want?
		else
		{
			//TODO: Maybe get the partial name instead???
			return (pLeft->GetScopeObject( MakeCompoundID( pRight->GetStringData() ) ))->CastToVariableBase();
		}
	}
	//No operator found!
	else
	{
		STRING tmp = TXT("Binary operator \'");
		tmp += Op;
		tmp += TXT("\' is unknown.");
		ThrowParserAnomaly( tmp, ANOMALY_UNKNOWNOP );
	}
}


