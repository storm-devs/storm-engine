#ifndef _VMODULE_API_H_
#define _VMODULE_API_H_

#include "vapi.h"
#include "memop.h"
#include "exs.h"

class VMA;

// dll import func definition
#define DLL_MODULE_API_FUNCTION __declspec( dllexport) VMA *  _cdecl
typedef VMA * (__cdecl * DLLAPIFUNC)(VAPI *, VSYSTEM_API *);


#ifdef _ENGINE_IN_

extern VMA * _pModuleClassRoot;
extern VAPI * _CORE_API; 
extern VSYSTEM_API * _VSYSTEM_API; 
extern VAPI * api; 
extern VFILE_SERVICE * fio;
#else

DLL_MODULE_API_FUNCTION DMAInterface();		// import func declaration for dll module
extern VAPI * _CORE_API; 
extern VAPI * api; 
extern VSYSTEM_API * _VSYSTEM_API; 
extern VFILE_SERVICE * fio;

#endif



class VMA
{
protected:
	VMA * pNext;
	long  nHash;
	long  nReference;
public:
	VMA()
	{
		nReference = 0;
		nHash = 0;
		//pNext = _pModuleClassRoot;
		//_pModuleClassRoot = this;
	};
	VMA * Next(){return pNext;}
	virtual ~VMA(){};
	long Build_Version() {return ENGINE_VERSION;};
	void SetHash(long _hash) {nHash = _hash;}
	long GetHash() {return nHash;}
	void Set(VMA * _p){pNext = _p;};
	virtual bool Service() {return false;}
	virtual char * GetName() {return null;}
	virtual void * CreateClass() {return null;}
	virtual void RefDec(){nReference--;};
	virtual long GetReference(){return nReference;}
	virtual void Clear(){nReference = 0;};
	virtual bool ScriptLibriary() {return false;}
};

#ifndef _ENGINE_IN_
#define INTERFACE_FUNCTION	VMA * _pModuleClassRoot = 0; VAPI * _CORE_API; VAPI * api; VFILE_SERVICE * fio; VSYSTEM_API * _VSYSTEM_API; extern "C" DLL_MODULE_API_FUNCTION _cdecl DMAInterface(VAPI * api_PTR,VSYSTEM_API * sapi_PTR){_CORE_API = api_PTR; api = api_PTR; fio = api_PTR->fio; _VSYSTEM_API = sapi_PTR; return _pModuleClassRoot;} void * _cdecl operator new(size_t size) {return _CORE_API->MemAllocate(size);} void _cdecl operator delete(void * block_ptr) { _CORE_API->MemFree(block_ptr); } void * _cdecl resize(void * block_ptr,size_t size){ return _CORE_API->MemReallocate(block_ptr,size);} /**/ void * _cdecl operator new(size_t size,char * p,unsigned long n) {return _CORE_API->MemAllocate(size,p,n);} void _cdecl operator delete(void * block_ptr,char * p, unsigned long n) { _CORE_API->MemFree(block_ptr); } void * _cdecl resize(void * block_ptr,size_t size,char * p,unsigned long n){ return _CORE_API->MemReallocate(block_ptr,size,p,n);}
#else
#define INTERFACE_FUNCTION
#endif
#define CREATE_CLASS(a)	class a##vmacd : public VMA { public: a##vmacd(VMA * & _pR){pNext = _pR;_pR = this;}; char * GetName() {return TEXT(#a);} void * CreateClass() {nReference++; return NEW a;}}; a##vmacd a##vmaci(_pModuleClassRoot);
#define CREATE_SERVICE(a)	class a##vmacd : public VMA { public: a * pService; a##vmacd(VMA * & _pR){pNext = _pR;_pR = this; pService = 0; }; char * GetName() {return TEXT(#a);} void * CreateClass() {if(pService == 0) pService = NEW a; nReference++; return pService;} bool Service() {return true;} void Clear(){nReference = 0; if(pService) delete pService; pService = 0;};}; a##vmacd a##vmaci(_pModuleClassRoot);
#define CREATE_SCRIPTLIBRIARY(a)	class a##vmacd : public VMA { public: /*a * pLibraryInitClass;*/ a##vmacd(VMA * & _pR){pNext = _pR;_pR = this; /*pLibraryInitClass = 0;*/ }; char * GetName() {return TEXT(#a);} void * CreateClass() {/*if(pLibraryInitClass == 0) pLibraryInitClass = NEW a; nReference++; return pLibraryInitClass;*/ return NEW a;} bool ScriptLibriary() {return true;} /*void Clear(){nReference = 0; if(pLibraryInitClass) delete pLibraryInitClass; pLibraryInitClass = 0;}*/;}; a##vmacd a##vmaci(_pModuleClassRoot);
#endif