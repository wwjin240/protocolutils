#include "ParsManager.h"
#include <windows.h>
#include <process.h>
#include "ProtocolUtils.h"
#include "PickParUtil.h"

using namespace std;

ParsManager::ParsManager()
	:_curParIndex(0)
	, _argc(0)
{

}

ParsManager::~ParsManager()
{

}

void ParsManager::Init(int argc, char* argv[])
{
	_argc = argc;
	int t_count = 0;
	for (int i = 0; i < argc; i++) {
		auto v = string(argv[i]);
		auto iter = _exParCfgMaps.find(v);
		if (iter != _exParCfgMaps.end()) {
			if (iter->second == 1) {
				i++;
				if (i < argc) {
					PtKVR t_kvr;
					t_kvr.k = v;
					t_kvr.v = string(argv[i]);
					t_kvr.r = "";
					_parMaps.insert(make_pair(t_kvr.k, t_kvr));
				}
			}
			else /*if (iter->second == 0)*/ {
				PtKVR t_kvr;
				t_kvr.k = v;
				t_kvr.v = "";
				t_kvr.r = "";
				_parMaps.insert(make_pair(t_kvr.k, t_kvr));
			}

		}
		else {	
			char t_ch[64];
			sprintf_s(t_ch, 64, "_%d", t_count);
			PtKVR t_kvr;
			t_kvr.k = t_ch;
			t_kvr.v = v;
			t_kvr.r = "";
			_parMaps.insert(make_pair(t_kvr.k, t_kvr));

			t_count++;
		}

	}

	_argc = t_count;

	unsigned int tt_ProgressId = GetCurrentProcessId();
	char t_ch[64];
	sprintf_s(t_ch, 64, "%d", GetCurrentProcessId());
	PtKVR t_kvr;
	t_kvr.k = "progressid";
	t_kvr.v = t_ch;
	t_kvr.r = "";
	_parMaps.insert(make_pair(t_kvr.k, t_kvr));

	//算出自己的全路径
	if (argc >= 1) {
		string t_fullPath = string(argv[0]);
		//t_fullPath = "../protocolgenxmlD.exe";
		ProtocolUtils::ChangeXieGang(t_fullPath);
		t_fullPath = ProtocolUtils::RemovePath(t_fullPath);
		t_fullPath = ProtocolUtils::GetFullPath(t_fullPath);

		t_kvr.k = "fullpath";
		t_kvr.v = t_fullPath;
		t_kvr.r = "";
		_parMaps.insert(make_pair(t_kvr.k, t_kvr));
	}


}

void ParsManager::InitExParCfg()
{
	//"--excludefolder","Log"
	//_exParCfgMaps.insert(make_pair("--excludefolder",1));

}

std::string ParsManager::BeginGetPar(int beginParIndex)
{
	_curParIndex = beginParIndex;

	char t_ch[64];
	sprintf_s(t_ch, 64, "_%d", _curParIndex);
	PickParUtil t_util;
	return t_util.GetString_KVR(_parMaps,t_ch);

}

std::string ParsManager::GetNextPar()
{
	_curParIndex++;

	char t_ch[64];
	sprintf_s(t_ch, 64, "_%d", _curParIndex);
	PickParUtil t_util;
	return t_util.GetString_KVR(_parMaps, t_ch);
}

void ParsManager::SetParIndex(int curParIndex)
{
	_curParIndex = curParIndex;
}

void ParsManager::AddError(const std::string& errorMsg)
{
	PtKVR t_kvr;
	t_kvr.k = "error";
	t_kvr.v = errorMsg;
	_parMaps.insert(make_pair(t_kvr.k, t_kvr));
	
}