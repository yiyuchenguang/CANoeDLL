// KeyGeneration.cpp : Defines the entry point for the DLL application.
//

#include <windows.h>
#include "KeyGenAlgoInterfaceEx.h"



BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}



KEYGENALGO_API VKeyGenResultEx GenerateKeyEx(
      const unsigned char*  iSeedArray,     /* Array for the seed [in] */
      unsigned int          iSeedArraySize, /* Length of the array for the seed [in] */
      const unsigned int    iSecurityLevel, /* Security level [in] */
      const char*           iVariant,       /* Name of the active variant [in] */
      unsigned char*        ioKeyArray,     /* Array for the key [in, out] */
      unsigned int          iKeyArraySize,  /* Maximum length of the array for the key [in] */
      unsigned int&         oSize           /* Length of the key [out] */
      )
{
    if (iSecurityLevel == 0x01)
    {
        for (unsigned int i = 0; i < iSeedArraySize; i++)
            ioKeyArray[i] = ~iSeedArray[i];
    }
    else if (iSecurityLevel == 0x11)
    {
        for (unsigned int i = 0; i < iSeedArraySize; i++)
            ioKeyArray[i] = iSeedArray[i] + 1;
    }
    else
    {
        return KGRE_SecurityLevelInvalid;
    }
    oSize = iSeedArraySize;

    return KGRE_Ok;
}


