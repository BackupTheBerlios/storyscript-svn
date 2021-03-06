/*
Copyright (c) 2004-2005 Daniel Jones (DanielCJones@gmail.com)

This is part of the  StoryScript (AKA: SS, S^2, SSqared, etc) software.  Full license information is included in the file in the top directory named "license".

NOTES: 
	This is to resolve a circular dependency with DLLExport.hpp and Word.hpp.
	DLLExport needs to have Word defined, and Word needs to have std::string
	exported.
	
	(Am I still using the file???)
*/


/*
    DLLExportString.hpp
    --------------------------
    I know it seems goofy to have this whole file just to export stl string.

	This is to resolve a circular dependency with DLLExport.hpp and Word.hpp.
	DLLExport needs to have Word defined, and Word needs to have std::string
	exported.

	Created:	4:7:2005   13:00
*/

#if !defined(SS_DLLExportString) && defined(PLAT_WIN32)
#define SS_DLLExportString

#include <string>
#include "Defines.hpp"

#pragma warning (disable : 4231)

/*
#if defined(SINGLE_THREAD)
	#if defined(USING_UNICODE)
		EXPIMP_TEMPLATE template class std::allocator<wchar_t>;
		EXPIMP_TEMPLATE template class std::basic_string< wchar_t >;
	#else
		EXPIMP_TEMPLATE template class std::allocator<char>;
		EXPIMP_TEMPLATE template class std::basic_string<char>;
	#endif
#endif
*/



#pragma warning (default : 4231)


#endif
