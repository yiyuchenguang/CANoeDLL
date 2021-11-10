// KeyGeneration.cpp : Defines the entry point for the DLL application.
//

#include <windows.h>
#define KEYGENALGO_EXPORTS
#include "KeyGenAlgoInterfaceEx.h"
#include <tchar.h>
#include <iostream>


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

int GenerateKeyExOpt_new(
    const unsigned char* ipSeedArray,     /* Array for the seed [in] */
    unsigned int          iSeedArraySize, /* Length of the array for the seed [in] */
    const unsigned int    iSecurityLevel, /* Security level [in] */
    const char* iVariant,       /* Name of the active variant [in] */
    const char* ipOptions,
    unsigned char* ioKeyArray,     /* Array for the key [in, out] */
    unsigned int          iKeyArraySize,  /* Maximum length of the array for the key [in] */
    unsigned int& oSize           /* Length of the key [out] */
)
{

    HINSTANCE handle = LoadLibrary(_T("C:\\Users\\Public\\Documents\\Vector\\CANoe\\Sample Configurations 11.0.55\\CAN\\Diagnostics\\UDSSystem\\SecurityAccess\\SeednKey_Opt_More.dll"));//LoadLibrary填入ddl文件名赋值给句柄
    printf("dll的句柄返回值%d\n", handle); //打印吧

    if (handle) //判读句柄内dll是否可用
    {
        //typedef int(*DLL_FUNCTION_GenerateKeyExOpt) (unsigned char ipSeedArray[], unsigned int iSeedArraySize, unsigned int iSecurityLevel, char ipVariant[], char ipOptions[], unsigned char iopKeyArray[], unsigned int iMaxKeyArraySize, unsigned int& oActualKeyArraySize);

        typedef int(*DLL_FUNCTION_GenerateKeyExOpt) (const unsigned char*, unsigned int, const unsigned int, const char*, const char*, unsigned char*, unsigned int, unsigned int&); //typedef定义一下函数指针，你不懂的话就记住末尾两个是你需要函数的形参。

        DLL_FUNCTION_GenerateKeyExOpt dll_GenerateKeyExOpt = (DLL_FUNCTION_GenerateKeyExOpt)GetProcAddress(handle, "GenerateKeyExOpt"); //使用GetProcAddress得到函数，参数是句柄名和函数名
        printf("dll_函数返回值%d\n", dll_GenerateKeyExOpt); //打印吧
        if (dll_GenerateKeyExOpt) //还是判断一下函数指针是否有效
        {
            int result = dll_GenerateKeyExOpt(ipSeedArray, iSeedArraySize, iSecurityLevel, iVariant, ipOptions, ioKeyArray, iKeyArraySize, oSize);

            //for (i = 0; i < 4; i++)
            //{
            //	printf("dll_GenerateKeyExOpt:::iopKeyArray[%d]：0x%x\n", i, iopKeyArray[i]); //打印iopKeyArray数组值
            //}
        }
        FreeLibrary(handle); //卸载句柄，，
    }
    return 0;
}

KEYGENALGO_API VKeyGenResultEx GenerateKeyExOpt(
      const unsigned char*  ipSeedArray,     /* Array for the seed [in] */
      unsigned int          iSeedArraySize, /* Length of the array for the seed [in] */
      const unsigned int    iSecurityLevel, /* Security level [in] */
      const char*           iVariant,       /* Name of the active variant [in] */
      const char* ipOptions,
      unsigned char*        ioKeyArray,     /* Array for the key [in, out] */
      unsigned int          iKeyArraySize,  /* Maximum length of the array for the key [in] */
      unsigned int&         oSize           /* Length of the key [out] */
      )
{
    int result;
    if (iSeedArraySize>iKeyArraySize)
      return KGRE_BufferToSmall;


    if (iSecurityLevel == 0x01)
    {
        result = GenerateKeyExOpt_new(ipSeedArray, iSeedArraySize, iSecurityLevel, iVariant, ipOptions, ioKeyArray, iKeyArraySize, oSize);
    }
    else if (iSecurityLevel == 0x11)
    {
        result = GenerateKeyExOpt_new(ipSeedArray, iSeedArraySize, iSecurityLevel, iVariant, ipOptions, ioKeyArray, iKeyArraySize, oSize);
    }
    else if (iSecurityLevel == 0x13)
    {
        for (unsigned int i = 0; i < iSeedArraySize; i++)
            ioKeyArray[i] = ipSeedArray[i] - 1;
    }
    else
    {
        return KGRE_SecurityLevelInvalid;
    }
    oSize = iSeedArraySize;  
  return KGRE_Ok;
}


