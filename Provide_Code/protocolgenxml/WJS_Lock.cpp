#include "WJS_Lock.h"

WJS_Lock::WJS_Lock()
{
	InitializeCriticalSection(&m_cs);
}

WJS_Lock::~WJS_Lock()
{
	DeleteCriticalSection(&m_cs);
}

void WJS_Lock::Enter()
{
	EnterCriticalSection(&m_cs);
}

void WJS_Lock::Leave()
{
	LeaveCriticalSection(&m_cs);
}