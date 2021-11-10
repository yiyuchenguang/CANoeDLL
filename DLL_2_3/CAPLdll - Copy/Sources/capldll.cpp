/*Example of a CAPL DLL implementation */

#define USECDLL_FEATURE
#define _BUILDNODELAYERDLL

#pragma warning( disable : 4786 )

#include "..\Includes\cdll.h"
#include "..\Includes\via.h"
#include "..\Includes\via_CDLL.h"

#include <stdio.h>
#include <stdlib.h>
#include <map>

class CaplInstanceData;
typedef std::map<uint32, CaplInstanceData*> VCaplMap;
typedef std::map<uint32, VIACapl*> VServiceMap;


// ============================================================================
// global variables
// ============================================================================

static unsigned long data = 0;
static char dlldata[100];

char        gModuleName[_MAX_FNAME];  // filename of this  dll 
HINSTANCE   gModuleHandle;            // windows instance handle of this DLL
VCaplMap    gCaplMap;   
VServiceMap gServiceMap;


// ============================================================================
// CaplInstanceData
// 
// Data local for a single CAPL Block. 
//
// A CAPL-DLL can be used by more than one CAPL-Block, so every piece of 
// information thats like a globale variable in CAPL, must now be wraped into 
// an instance of an object.
// ============================================================================
class CaplInstanceData
{
public:
  CaplInstanceData(VIACapl* capl);
  
  void GetCallbackFunctions();
  void ReleaseCallbackFunctions();

  // Definition of the class function. 
  // This class function will call the 
  // CAPL callback functions 
  uint32 ShowValue(uint32 x);
  uint32 ShowDates(int16 x, uint32 y, int16 z);
  void   Print_8888_Num(uint32 numberOfDatabytes);
  void   DllInfo(char* x);
  void   ArrayValues(uint32 flags, uint32 numberOfDatabytes, uint8 databytes[], uint8 controlcode);
  void DllVersion(char* y);

private:  

  // Pointer of the CAPL callback functions 
  VIACaplFunction*  mShowValue;
  VIACaplFunction*  mShowDates;
  VIACaplFunction* mPrint_8888_Num;
  VIACaplFunction*  mDllInfo;
  VIACaplFunction*  mArrayValues;
  VIACaplFunction*  mDllVersion;

  VIACapl*          mCapl;
};


CaplInstanceData::CaplInstanceData(VIACapl* capl) 
  // This function will initialize the CAPL callback function 
  // with the NLL Pointer 
 : mCapl(capl), 
   mShowValue(NULL),
   mShowDates(NULL),
   mPrint_8888_Num(NULL),
   mDllInfo(NULL),
   mArrayValues(NULL),
   mDllVersion(NULL)
{}

static bool sCheckParams(VIACaplFunction* f, char rtype, char* ptype)
{
  char      type;
  int32     pcount;
  VIAResult rc;

  // check return type
  rc = f->ResultType(&type);
  if (rc!=kVIA_OK || type!=rtype)
  {
    return false;
  }

  // check number of parameters
  rc = f->ParamCount(&pcount);
  if (rc!=kVIA_OK || strlen(ptype)!=pcount )
  {
    return false;
  }

  // check type of parameters
  for (int i=0; i<pcount; ++i)
  {
    rc = f->ParamType(&type, i);
    if (rc!=kVIA_OK || type!=ptype[i])
    {
      return false;
    }
  }

  return true;
}

static VIACaplFunction* sGetCaplFunc(VIACapl* capl, const char * fname, char rtype, char* ptype)
{
  VIACaplFunction* f; 

  // get capl function object
  VIAResult rc =  capl->GetCaplFunction(&f, fname);
  if (rc!=kVIA_OK || f==NULL) 
  {
    return NULL;
  }

  // check signature of function
  if ( sCheckParams(f, rtype, ptype) )
  {
     return f;
  }
  else
  {
    capl->ReleaseCaplFunction(f);
    return NULL;
  }
}

void CaplInstanceData::GetCallbackFunctions()
{
  // Get a CAPL function handle. The handle stays valid until end of
  // measurement or a call of ReleaseCaplFunction. 
  mShowValue   = sGetCaplFunc(mCapl, "CALLBACK_ShowValue", 'D', "D");
  mShowDates   = sGetCaplFunc(mCapl, "CALLBACK_ShowDates", 'D', "IDI");
  mPrint_8888_Num = sGetCaplFunc(mCapl, "CALLBACK_Print_8888_Num", 'V', "D");
  mDllInfo     = sGetCaplFunc(mCapl, "CALLBACK_DllInfo", 'V', "C");
  mArrayValues = sGetCaplFunc(mCapl, "CALLBACK_ArrayValues", 'V', "DBB");
  mDllVersion  = sGetCaplFunc(mCapl, "CALLBACK_DllVersion", 'V', "C");
}

void CaplInstanceData::ReleaseCallbackFunctions()
{
  // Release all the requested Callback functions 
  mCapl->ReleaseCaplFunction(mShowValue);
  mShowValue = NULL;
  mCapl->ReleaseCaplFunction(mShowDates);
  mShowDates = NULL;
  mCapl->ReleaseCaplFunction(mPrint_8888_Num);
  mPrint_8888_Num = NULL;
  
  mCapl->ReleaseCaplFunction(mDllInfo);
  mDllInfo = NULL;
  mCapl->ReleaseCaplFunction(mArrayValues);
  mArrayValues = NULL;
  mCapl->ReleaseCaplFunction(mDllVersion);
  mDllVersion = NULL;
}

void CaplInstanceData::DllVersion(char* y)
{
  // Prepare the parameters for the call stack of CAPL. 
  // Arrays uses a 8 byte on the stack, 4 Bytes for the number of element,
  // and 4 bytes for the pointer to the array
  int32 sizeX = strlen(y)+1;

  uint8 params[8];               // parameters for call stack, 8 Bytes total    
  memcpy(params+0, &sizeX, 4);   // array size    of first parameter, 4 Bytes
  memcpy(params+4, &y,     4);   // array pointer of first parameter, 4 Bytes
  
  if(mDllVersion!=NULL)
  {
    uint32 result; // dummy variable
    VIAResult rc =  mDllVersion->Call(&result, params);
  }
}


uint32 CaplInstanceData::ShowValue(uint32 x)
{
  void* params = &x;   // parameters for call stack

  uint32 result;

  if(mShowValue!=NULL)
  {
    VIAResult rc =  mShowValue->Call(&result, params);
    if (rc==kVIA_OK)
    {
       return result;
    }
  }    
  return -1;
}

uint32 CaplInstanceData::ShowDates(int16 x, uint32 y, int16 z)
{
  // Prepare the parameters for the call stack of CAPL. The stack grows
  // from top to down, so the first parameter in the parameter list is the last 
  // one in memory. CAPL uses also a 32 bit alignment for the parameters. 
  uint8 params[12];         // parameters for call stack, 12 Bytes total    
  memcpy(params+0, &z, 2);  // third  parameter, offset 0, 2 Bytes
  memcpy(params+4, &y, 4);  // second parameter, offset 4, 4 Bytes
  memcpy(params+8, &x, 2);  // first  parameter, offset 8, 2 Bytes 

  uint32 result;

  if(mShowDates!=NULL)
  {
    VIAResult rc =  mShowDates->Call(&result, params);
    if (rc==kVIA_OK)
    {
       return rc;   // call successful
    }
  } 
    
  return -1; // call failed
}

void CaplInstanceData::DllInfo(char* x)
{
  // Prepare the parameters for the call stack of CAPL. 
  // Arrays uses a 8 byte on the stack, 4 Bytes for the number of element,
  // and 4 bytes for the pointer to the array
  int32 sizeX = strlen(x)+1;

  uint8 params[8];               // parameters for call stack, 8 Bytes total    
  memcpy(params+0, &sizeX, 4);   // array size    of first parameter, 4 Bytes
  memcpy(params+4, &x,     4);   // array pointer of first parameter, 4 Bytes
  
  if(mDllInfo!=NULL)
  {
    uint32 result; // dummy variable
    VIAResult rc =  mDllInfo->Call(&result, params);
  }
}



void CaplInstanceData::Print_8888_Num( uint32 numberOfDatabytes)
{
    void* params = &numberOfDatabytes;   // parameters for call stack

    uint32 result;

    if (mPrint_8888_Num != NULL)
    {
        VIAResult rc = mPrint_8888_Num->Call(&result, params);
    }
}


void CaplInstanceData::ArrayValues(uint32 flags, uint32 numberOfDatabytes, uint8 databytes[], uint8 controlcode)
{
  // Prepare the parameters for the call stack of CAPL. The stack grows
  // from top to down, so the first parameter in the parameter list is the last 
  // one in memory. CAPL uses also a 32 bit alignment for the parameters.
  // Arrays uses a 8 byte on the stack, 4 Bytes for the number of element,
  // and 4 bytes for the pointer to the array
  
  uint8 params[16];                           // parameters for call stack, 16 Bytes total    
  memcpy(params+ 0, &controlcode,       1);   // third parameter,                  offset  0, 1 Bytes
  memcpy(params+ 4, &numberOfDatabytes, 4);   // second parameter (array size),    offset  4, 4 Bytes
  memcpy(params+ 8, &databytes,         4);   // second parameter (array pointer), offset  8, 4 Bytes
  memcpy(params+12, &flags,             4);   // first  parameter,                 offset 12, 4 Bytes 

  if(mArrayValues!=NULL)
  {
    uint32 result; // dummy variable
    VIAResult rc =  mArrayValues ->Call(&result, params);
  }

}

CaplInstanceData* GetCaplInstanceData(uint32 handle)
{
  VCaplMap::iterator lSearchResult(gCaplMap.find(handle));
  if ( gCaplMap.end()==lSearchResult )
  {
    return NULL;
  } else {
    return lSearchResult->second;
  }
}

// ============================================================================
// CaplInstanceData
// 
// Data local for a single CAPL Block. 
//
// A CAPL-DLL can be used by more than one CAPL-Block, so every piece of 
// information thats like a global variable in CAPL, must now be wrapped into 
// an instance of an object.
// ============================================================================

void CAPLEXPORT far CAPLPASCAL appInit (uint32 handle)
{
  CaplInstanceData* instance = GetCaplInstanceData(handle);
  if ( NULL==instance )
  {
    VServiceMap::iterator lSearchService(gServiceMap.find(handle));
    if ( gServiceMap.end()!=lSearchService )
    {
      VIACapl* service = lSearchService->second;
      try 
      {
        instance = new CaplInstanceData(service);
      }
      catch ( std::bad_alloc& )
      {
        return; // proceed without change
      }
      instance->GetCallbackFunctions();
      gCaplMap[handle] = instance;
    }
  }
}    

void CAPLEXPORT far CAPLPASCAL appEnd (uint32 handle)
{
  CaplInstanceData* inst = GetCaplInstanceData(handle);
  if (inst==NULL)
  {
    return;
  }
  inst->ReleaseCallbackFunctions();

  delete inst;
  inst = NULL; 
  gCaplMap.erase(handle);
}    

long CAPLEXPORT far CAPLPASCAL appSetValue (uint32 handle, long x)
{
  CaplInstanceData* inst = GetCaplInstanceData(handle);
  if (inst==NULL)
  {
    return -1; 
  }

  inst->Print_8888_Num(8888);
  return 1;
}    

long CAPLEXPORT far CAPLPASCAL appReadData (uint32 handle, long a)
{
  CaplInstanceData* inst = GetCaplInstanceData(handle);
  if (inst==NULL)
  {
    return -1; 
  }

  int16  x = (a>=0) ? +1 : -1;
  uint32 y = abs(a);
  int16  z = (int16)(a & 0x0f000000) >> 24;

  inst->DllVersion("Version 1.1");
  
  inst->DllInfo("DLL: processing");

  uint8 databytes[8] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};

  inst->ArrayValues( 0xaabbccdd, sizeof(databytes), databytes, 0x01);

  return inst->ShowDates( x, y, z);
}    


// ============================================================================
// VIARegisterCDLL
// ============================================================================

VIACLIENT(void) VIARegisterCDLL (VIACapl* service)
{
  uint32    handle;
  VIAResult result;

  if (service==NULL)
  {
    return;
  }

  result = service->GetCaplHandle(&handle);
  if(result!=kVIA_OK)
  {
    return;
  }

  // appInit (internal) resp. "DllInit" (CAPL code) has to follow
  gServiceMap[handle] = service;
}

void ClearAll()
{
  // destroy objects created by this DLL
  // may result from forgotten DllEnd calls
  VCaplMap::iterator lIter=gCaplMap.begin();
  const long cNumberOfEntries = gCaplMap.size();
  long i = 0;
  while ( lIter!=gCaplMap.end() && i<cNumberOfEntries )
  {
    appEnd( (*lIter).first );
    lIter = gCaplMap.begin(); // first element should have vanished
    i++; // assure that no more erase trials take place than the original size of the map
  }

  // just for clarity (would be done automatically)
  gCaplMap.clear();
  gServiceMap.clear();
}

void CAPLEXPORT far CAPLPASCAL voidFct( void )
{
   // do something
   data = 55;
}

unsigned long CAPLEXPORT far CAPLPASCAL appLongFuncName( void )
{
  return 1;
}

void CAPLEXPORT far CAPLPASCAL appPut(unsigned long x)
{
  data = x;
}

unsigned long CAPLEXPORT far CAPLPASCAL appGet(void)
{
  return data;
}

long CAPLEXPORT far CAPLPASCAL appAdd(long x, long y)
{
  long z = x + y;
  
  return z;
}    
long CAPLEXPORT far CAPLPASCAL appSubtract(long x, long y)
{
  long z = x - y;
  
  return z;
}

long CAPLEXPORT far CAPLPASCAL appAddValues63(long val01, long val02, long val03, long val04, long val05, long val06, long val07, long val08,
                                              long val09, long val10, long val11, long val12, long val13, long val14, long val15, long val16,
                                              long val17, long val18, long val19, long val20, long val21, long val22, long val23, long val24,
                                              long val25, long val26, long val27, long val28, long val29, long val30, long val31, long val32,
                                              long val33, long val34, long val35, long val36, long val37, long val38, long val39, long val40,
                                              long val41, long val42, long val43, long val44, long val45, long val46, long val47, long val48,
                                              long val49, long val50, long val51, long val52, long val53, long val54, long val55, long val56,
                                              long val57, long val58, long val59, long val60, long val61, long val62, long val63)
{
  long z = val01+val02+val03+val04+val05+val06+val07+val08
         + val09+val10+val11+val12+val13+val14+val15+val16
         + val17+val18+val19+val20+val21+val22+val23+val24
         + val25+val26+val27+val28+val29+val30+val31+val32
         + val33+val34+val35+val36+val37+val38+val39+val40
         + val41+val42+val43+val44+val45+val46+val47+val48
         + val49+val50+val51+val52+val53+val54+val55+val56
         + val57+val58+val59+val60+val61+val62+val63;

  return z;
}

long CAPLEXPORT far CAPLPASCAL appAddValues64(long val01, long val02, long val03, long val04, long val05, long val06, long val07, long val08,
                                              long val09, long val10, long val11, long val12, long val13, long val14, long val15, long val16,
                                              long val17, long val18, long val19, long val20, long val21, long val22, long val23, long val24,
                                              long val25, long val26, long val27, long val28, long val29, long val30, long val31, long val32,
                                              long val33, long val34, long val35, long val36, long val37, long val38, long val39, long val40,
                                              long val41, long val42, long val43, long val44, long val45, long val46, long val47, long val48,
                                              long val49, long val50, long val51, long val52, long val53, long val54, long val55, long val56,
                                              long val57, long val58, long val59, long val60, long val61, long val62, long val63, long val64)
{
  long z = val01+val02+val03+val04+val05+val06+val07+val08
         + val09+val10+val11+val12+val13+val14+val15+val16
         + val17+val18+val19+val20+val21+val22+val23+val24
         + val25+val26+val27+val28+val29+val30+val31+val32
         + val33+val34+val35+val36+val37+val38+val39+val40
         + val41+val42+val43+val44+val45+val46+val47+val48
         + val49+val50+val51+val52+val53+val54+val55+val56
         + val57+val58+val59+val60+val61+val62+val63+val64;

  return z;
}
#define EightLongPars 'L','L','L','L','L','L','L','L'
#define SixtyFourLongPars EightLongPars,EightLongPars,EightLongPars,EightLongPars,EightLongPars,EightLongPars,EightLongPars,EightLongPars

void CAPLEXPORT far CAPLPASCAL appGetDataTwoPars(  unsigned long numberBytes,
    unsigned char dataBlock[] )
{
  unsigned int i;
  for (i = 0; i < numberBytes; i++) {
    dataBlock[i] = dlldata[i];
  }
}

void CAPLEXPORT far CAPLPASCAL appPutDataTwoPars( unsigned long numberBytes,
    const unsigned char dataBlock[] )
{
  unsigned int i;
  for (i = 0; i < numberBytes; i++) {
    dlldata[i] = dataBlock[i];
  }
}

// get data from DLL into CAPL memory
void CAPLEXPORT far CAPLPASCAL appGetDataOnePar( unsigned char dataBlock[] )
{
  //  get first element
  dataBlock[0] = (unsigned char)data;
}
void CAPLEXPORT far CAPLPASCAL appSum(long i, long j, long* s)
{
    *s = i + j;
}



// put data from CAPL array to DLL
void CAPLEXPORT far CAPLPASCAL appPutDataOnePar( const unsigned char dataBlock[] )
{
  // put first element
  data = dataBlock[0];

}
// ============================================================================
// CAPL_DLL_INFO_LIST : list of exported functions
//   The first field is predefined and mustn't be changed!
//   The list has to end with a {0,0} entry!
// New struct supporting function names with up to 50 characters
// ============================================================================
CAPL_DLL_INFO4 table[] = {
{CDLL_VERSION_NAME, (CAPL_FARCALL)CDLL_VERSION, "", "", CAPL_DLL_CDECL, 0xabcd, CDLL_EXPORT },

  {"dllInit",           (CAPL_FARCALL)appInit,          "CAPL_DLL","This function will initialize all callback functions in the CAPLDLL",'V', 1, "D", "", {"handle"}},
  {"dllEnd",            (CAPL_FARCALL)appEnd,           "CAPL_DLL","This function will release the CAPL function handle in the CAPLDLL",'V', 1, "D", "", {"handle"}}, 
  {"dllSetValue",       (CAPL_FARCALL)appSetValue,      "CAPL_DLL","This function will call a callback functions",'L', 2, "DL", "", {"handle","x"}},
  {"dllReadData",       (CAPL_FARCALL)appReadData,      "CAPL_DLL","This function will call a callback functions",'L', 2, "DL", "", {"handle","x"}},
  {"dllPut",            (CAPL_FARCALL)appPut,           "CAPL_DLL","This function will save data from CAPL to DLL memory",'V', 1, "D", "", {"x"}}, 
  {"dllGet",            (CAPL_FARCALL)appGet,           "CAPL_DLL","This function will read data from DLL memory to CAPL",'D', 0, "", "", {""}}, 
  {"dllVoid",           (CAPL_FARCALL)voidFct,          "CAPL_DLL","This function will overwrite DLL memory from CAPL without parameter",'V', 0, "", "", {""}}, 
  {"dllPutDataOnePar",  (CAPL_FARCALL)appPutDataOnePar, "CAPL_DLL","This function will put data from CAPL array to DLL",'V', 1, "B", "\001", {"datablock"}}, 
  {"dllGetDataOnePar",  (CAPL_FARCALL)appGetDataOnePar, "CAPL_DLL","This function will get data from DLL into CAPL memory",'V', 1, "B", "\001", {"datablock"}}, 
  {"dllPutDataTwoPars", (CAPL_FARCALL)appPutDataTwoPars,"CAPL_DLL","This function will put two datas from CAPL array to DLL",'V', 2, "DB", "\000\001", {"noOfBytes","datablock"}},// number of pars in octal format
  {"dllGetDataTwoPars", (CAPL_FARCALL)appGetDataTwoPars,"CAPL_DLL","This function will get two datas from DLL into CAPL memory",'V', 2, "DB", "\000\001", {"noOfBytes","datablock"}},
  {"dllAdd",            (CAPL_FARCALL)appAdd,           "CAPL_DLL","This function will add two values. The return value is the result",'L', 2, "LL", "", {"x","y"}},
  {"dll_test_Add",      (CAPL_FARCALL)appAdd,           "TEST_DLL","T",'L', 2, "LL", "", {"x","y"}},
  {"dll_test_Sum",      (CAPL_FARCALL)appSum,           "TEST_DLL","Sum via reference parameter",'V', 3, {'L', 'L', 'L' - 128}, "", {"i", "j", "s"} },
  {"dllSubtract",       (CAPL_FARCALL)appSubtract,      "CAPL_DLL","This function will substract two values. The return value is the result",'L', 2, "LL", "", {"x","y"}},
  {"dllSupportLongFunctionNamesWithUpTo50Characters",   (CAPL_FARCALL)appLongFuncName,      "CAPL_DLL","This function shows the support of long function names",'D', 0, "", "", {""}},
  {"dllAdd63Parameters", (CAPL_FARCALL)appAddValues63,  "CAPL_DLL", "This function will add 63 values. The return value is the result",'L', 63, "LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL",  "", {"val01","val02","val03","val04","val05","val06","val07","val08","val09","val10","val11","val12","val13","val14","val15","val16","val17","val18","val19","val20","val21","val22","val23","val24","val25","val26","val27","val28","val29","val30","val31","val32","val33","val34","val35","val36","val37","val38","val39","val40","val41","val42","val43","val44","val45","val46","val47","val48","val49","val50","val51","val52","val53","val54","val55","val56","val57","val58","val59","val60","val61","val62","val63"}},
  {"dllAdd64Parameters", (CAPL_FARCALL)appAddValues64,  "CAPL_DLL", "This function will add 64 values. The return value is the result",'L', 64, {SixtyFourLongPars},                                                "", {"val01","val02","val03","val04","val05","val06","val07","val08","val09","val10","val11","val12","val13","val14","val15","val16","val17","val18","val19","val20","val21","val22","val23","val24","val25","val26","val27","val28","val29","val30","val31","val32","val33","val34","val35","val36","val37","val38","val39","val40","val41","val42","val43","val44","val45","val46","val47","val48","val49","val50","val51","val52","val53","val54","val55","val56","val57","val58","val59","val60","val61","val62","val63","val64"}},

{0, 0}
};
CAPLEXPORT CAPL_DLL_INFO4 far * caplDllTable4 = table;

// ============================================================================
// DllMain, entry Point of DLL  
// ============================================================================

BOOL WINAPI DllMain(HINSTANCE handle, DWORD reason, void*)
{     
  switch (reason) 
  {
    case DLL_PROCESS_ATTACH:
    {
      gModuleHandle = handle;
         
      // Get full filename of module
      char path_buffer[_MAX_PATH];
      DWORD result = GetModuleFileName(gModuleHandle, path_buffer,_MAX_PATH);

      // split filename into parts
      char drive[_MAX_DRIVE];
      char dir[_MAX_DIR];
      char fname[_MAX_FNAME];
      char ext[_MAX_EXT];
#if _MSC_VER>=1400 // >= Visual Studio 2005
      _splitpath_s( path_buffer, drive, dir, fname, ext );
      strcpy_s(gModuleName, fname);
#else
      _splitpath( path_buffer, drive, dir, fname, ext );
      strcpy(gModuleName, fname);
#endif
      
      return 1;   // Indicate that the DLL was initialized successfully.
    }

    case DLL_PROCESS_DETACH:                                              
    {
      ClearAll();
      return 1;   // Indicate that the DLL was detached successfully.
    }
  }
  return 1;
}



