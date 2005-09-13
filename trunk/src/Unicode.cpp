/*
	Unicode.cpp
	----------------------
	Created: Fri, Jan 30 2004, 0:12
	Description: 
	
*/

#include "Unicode.hpp"
#include <string>

using namespace SS;

//This one is common

CHAR* SS::STRCPY( CHAR* dest, const CHAR* src, unsigned int dest_size )
{
	if( dest_size == 0 ) return dest;
	
	unsigned int src_len = STRLEN( src );
	
	dest_size -= 1;
	dest[dest_size] = 0;
	
	if( dest_size == 0 ) return dest;
	
	//dest_size -= 1;
	
	while( dest_size != 0 )
	{
		if( dest_size > src_len ) dest[dest_size-1] = 0;
		else dest[dest_size-1] = src[dest_size-1];
		
		
		dest_size--;
	}
	
	return dest;
}

//UNICODE FUNCTION DEFS
#if defined(USING_UNICODE)

/*
These were shamelessly stolen from whoever the hell this guys is:
http://disemia.com/software/docs/stdtypes_8cc-source.html
*/
std::string SS::NarrowString( const wchar_t* wide )
{
	size_t len = wcslen( wide ) * 4 + 1;    //*4 for worst conversion case, +1 for null
	char* narrow = new char[len+1]; //+1 for length check
	size_t c = wcstombs( narrow, wide, len + 1 );
        
	if( c > len || c == (size_t)-1 )
		throw std::runtime_error( "Cannot narrow string" );

	std::basic_string<char> narrows( narrow );
	delete[] narrow;
        
	return narrows;
}


std::wstring SS::WidenString( const char* narrow )
{
	size_t len = strlen( narrow ) + 1;      //+1 for null
        //unlikely that the wide string is longer than the narrows one
	wchar_t* wide = new wchar_t[len+1]; //+1 for length check
	size_t c = mbstowcs( wide, narrow, len+1 );

        //string longer than expected or failed to convert
	if( c > len || c == (size_t)-1 )        //retarded standard passing -1 in unsigned size_t type!
		throw std::runtime_error( "Cannot widen string" );
                
	std::basic_string<wchar_t> wides( wide );
	delete[] wide;
        
	return wides;
}


//NON-UNICODE DEFS
#else


#endif

