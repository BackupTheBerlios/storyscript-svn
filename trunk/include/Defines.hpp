
#if !defined(SS_Defines)
#define SS_Defines

namespace SS{

//May be a bit dubious, but it works for now.
#if defined(_WIN32)
    #define PLAT_WIN32
#elif defined(__GNUC__)
    #define PLAT_LINUX
#endif

/*
Determines what type of structure is used for scopes.
The default is std::map, which is a standard and works just fine.
The other option is std::hash_map which is non-standard
(it is an sgi extension but gnu and microsoft include it in one
way or another).  hash_map may or may not be faster
(faster best-case senerio, slower worst cast).  Use it if
you like to live dangerously.

Exactly one of these must be uncommented.
*/
//#define SS_USE_MAP_SCOPES
#define SS_USE_HASH_MAP_SCOPES

/*
Similar to the above.  Exactly one of these must be uncommented.
vector is faster at some things, deque at others.
*/
//#define SS_USE_DEQUE_LISTS
#define SS_USE_VECTOR_LISTS


//Turn this on when creating a DLL file.
//Actually, don't mess with this at all, it gets defined
//in the project file.
//#define SS_API_EXP

//Turn this on to compile using unicode
//#define USING_UNICODE


//Magic DLL stuff
#if defined(SS_API_EXP) && defined(PLAT_WIN32)
	#define SS_API __declspec(dllexport)
	#define EXPIMP_TEMPLATE
#elif defined(PLAT_WIN32)
	#define SS_API __declspec(dllimport)
	#define EXPIMP_TEMPLATE extern
//If not using windows, these do nothing.
#else
    #define SS_API
    #define EXPIMP_TEMPLATE
#endif

//----------------Language Settings------------------

//With this turned on, it will throw an error if you try to access
//a list element that doesn't exist.  With it off, the element will
//just quietly spring into existance.

//TODO: This won't work if they get modified.  Make them extern.

static bool gUsingStrictLists = false;

static unsigned int gNumberBase = 10;
//0 for maximum
static unsigned int gMaxDigitOutput = 0;
static unsigned long gDefaultPrecis = 256; //In bits I think

} //namespace
#endif
