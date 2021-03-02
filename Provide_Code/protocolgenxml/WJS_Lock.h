/*
* create by wangweijin
*/
#pragma once
#include <windows.h>

class WJS_Lock
{
public:
	WJS_Lock();
	~WJS_Lock();

public:
	void Enter();
	void Leave();
private:

	CRITICAL_SECTION m_cs;

};