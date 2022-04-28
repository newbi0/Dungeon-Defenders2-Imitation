#pragma once

#define _BEGIN(NAMESPACE) namespace NAMESPACE{
#define _END }

#define _USING(NAMESPACE) using namespace NAMESPACE;

#ifdef ENGINE_EXPORTS
#define _ENGINE_DLL _declspec(dllexport)
#else
#define _ENGINE_DLL _declspec(dllimport)
#endif


#define _DECLARE_SINGLETON(CLASSNAME)				\
private: static CLASSNAME* m_pInstance;				\
public: static CLASSNAME* Get_Instance(void);		\
public: unsigned long Destroy_Instance(void);

#define _IMPLEMENT_SINGLETON(CLASSNAME)				\
CLASSNAME* CLASSNAME::m_pInstance = nullptr;		\
CLASSNAME* CLASSNAME::Get_Instance(void){			\
	if(nullptr == m_pInstance){						\
		m_pInstance = new CLASSNAME;}				\
	return m_pInstance;		}						\
unsigned long CLASSNAME::Destroy_Instance(void){	\
	unsigned long dwRefCnt = 0;						\
	if(nullptr != m_pInstance){						\
		dwRefCnt = m_pInstance->Release();			\
		if(0 == dwRefCnt)							\
			m_pInstance = nullptr;	}				\
	return dwRefCnt;	}							\


#define _MSG_BOX(MESSAGE) MessageBox(nullptr, TEXT(MESSAGE), TEXT("System Error"), MB_OK)

#define _RUN 0
#define _EXIT -1

#define _PURE = 0