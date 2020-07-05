#ifndef _VMAPI_H_
#define _VMAPI_H_

#include "vapi.h"
#include "memop.h"
#include "exs.h"

#ifdef _ENGINE_IN_
extern VMA * _pModuleClassRoot;
#else
VMA * _pModuleClassRoot;
#endif

class VMA
{
protected:
	VMA * pNext;
	long  nHash;
public:
	VMA()
	{
		nHash = 0;
		pNext = _pModuleClassRoot;
		_pModuleClassRoot = this;
	};
	VMA * Next(){return pNext;}
	virtual ~VMA(){};
	long Build_Version() {return ENGINE_VERSION;};
	void SetHash(long _hash) {nHash = _hash;}
	long GetHash() {return nHash;}
	virtual bool Service() {return false;}
	virtual char * GetName() {return null;}
	virtual void * CreateClass() {return null;}
};

#define MAKE_CLASS(a)	class a##vmacd : public VMA { public: char * GetName() {return TEXT(#a);} void * CreateClass() {return new a;}} a##vmacd a##vmaci;
#define MAKE_SERVICE(a)	class a##vmacd : public VMA { public: char * GetName() {return TEXT(#a);} void * CreateClass() {return new a;} bool Service() {return true;}} a##vmacd a##vmaci;


//-------------------------------------------------------------------------------------------------
// Macros for declarating module api class
// Syntax:
// API_MODULE_START(module_description)
// CREATE_CLASS(class_id,class_name)
// API_MODULE_END
// 
// module_description	- text description for module (with "") (can be null)
// class_id				- a simple ordinal number for your class
// class name			- exported class name (without "")
//
//   When you declare your classes with macro CREATE_CLASS(a,b) you must ensure, the value of any
// class_id lies within range 0,..,N-1  where N is a number of exported classes 
// The orders of numbers doesnt matter, but don't make a "holes". Engine enumerates classes by 
// calling to module api with continuously incremented class_id (starting from zero). If module 
// returned null - engine think, there is no more classes in module.
//  This macro must be placed in your module header file AFTER your classes declaration.
//  Macro INTERFACE_FUNCTION must be placed in any of yours module cpp file, but only once.
//
// Also, in INTERFACE_FUNCTION redefined memory operators new and delete.

#define DLL_MODULE_API_FUNCTION __declspec( dllexport) MODULE_API *  _cdecl

#ifndef _XBOX

#define API_MODULE_START(a)	extern VAPI * _CORE_API; extern VSYSTEM_API * _VSYSTEM_API; class MODULE_API : public VMODULE_API { public: long Build_Version(){return MODULE_API_VERSION;}; char * Description() {return a;} void * CreateClass(long code,bool get_class_name, VSYSTEM_API * sysapi_PTR) { _VSYSTEM_API = sysapi_PTR; switch(code){
#define API_MODULE_END		default: return null; } } }; DLL_MODULE_API_FUNCTION DMAInterface();
#define CREATE_CLASS(a,b)	case a:	if(get_class_name) return (void*)TEXT(#b); else return new b;
#define INTERFACE_FUNCTION	VAPI * _CORE_API; VSYSTEM_API * _VSYSTEM_API; void * _SERVICE_PTR = null; extern "C" DLL_MODULE_API_FUNCTION DMAInterface(VAPI * api_PTR){_CORE_API = api_PTR; return new MODULE_API;} void * operator new(size_t size) {return _CORE_API->MemAllocate(size);} void operator delete(void * block_ptr) { _CORE_API->MemFree(block_ptr); } void * resize(void * block_ptr,size_t size){ return _CORE_API->MemReallocate(block_ptr,size);}

#else

#define API_MODULE_START(a)	
#define API_MODULE_END
#define CREATE_CLASS(a,b)
#define INTERFACE_FUNCTION	extern	VAPI * _CORE_API; extern VSYSTEM_API * _VSYSTEM_API; extern void * _SERVICE_PTR;  
//void * operator new(size_t size); {return _CORE_API->MemAllocate(size);} void operator delete(void * block_ptr) { _CORE_API->MemFree(block_ptr); } void * resize(void * block_ptr,size_t size){ return _CORE_API->MemReallocate(block_ptr,size); }

#endif
typedef VMODULE_API * (__cdecl * DLLAPIFUNC)(VAPI *);


//-------------------------------------------------------------------------------------------------
// Macros for declarating service module api class
// Syntax:
// API_SERVICE_START(module_description)
// DECLARE_MAIN_SERVICE(real_class_name)
// DECLARE_BASE_SERVICE(class_id,class_name)
// API_MODULE_END(real_class_name)
// 
// module_description	- text description for module (with "") (can be null)
// class_id				- a simple ordinal number for your class
// class_name			- one of bases class names, pointers of that type can be used by objects
// real_class_name		- real name for exported class (most advanced class in module)
//
// class_id must be within 1..N range with no "holes" (simular to entities classes declaration)

extern VAPI * _CORE_API;

#ifndef _XBOX

#define API_SERVICE_START(a)		extern VSYSTEM_API * _VSYSTEM_API; extern void * _SERVICE_PTR; class MODULE_API : public VMODULE_API { public: long Build_Version(){return MODULE_API_VERSION;}; char * Description() {return a;} bool Service() {return true;}; void * CreateClass(long code,bool get_class_name, VSYSTEM_API * sysapi_PTR) { _VSYSTEM_API = sysapi_PTR; if(get_class_name) { switch(code){
#define API_SERVICE_END(a)			default: return null; }} if(_SERVICE_PTR == null) {_SERVICE_PTR = new a;} return _SERVICE_PTR; } }; DLL_MODULE_API_FUNCTION DMAInterface();
#define DECLARE_MAIN_SERVICE(a)		case 0:	return (void*)TEXT(#a);	
#define DECLARE_BASE_SERVICE(a,b)	case a:	return (void*)TEXT(#b);

#else

#define API_SERVICE_START(a)
#define API_SERVICE_END(a)
#define DECLARE_MAIN_SERVICE(a)
#define DECLARE_BASE_SERVICE(a,b)

#endif

#endif