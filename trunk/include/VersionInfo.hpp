

#if !defined(SS_VersionInfo)
#define SS_VersionInfo


#include "Unicode.hpp"
#include "Defines.hpp"

namespace SS
{

extern const SS_API SS::STRING SSI_VERSION;
extern const SS_API SS::STRING SSI_BUILDNUM;
extern const SS_API SS::STRING SSI_PLATFORM;
    
extern const SS_API SS::STRING SSI_NAME;

extern const SS_API SS::STRING SSI_LASTCOMPILE;
    
SS_API SS::STRING GetInterpreterVersion();
SS_API SS::STRING GetInterpreterName();
SS_API SS::STRING GetLastCompileTime();
SS_API SS::STRING GetBuildNumber();

}

#endif

