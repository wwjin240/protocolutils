//#include "stdafx.h"
#include "PickParUtil.h"
#include <time.h>
#ifndef _EXCLUDEXML_
#include "../tinyxml/tinyxml.h"
#endif


using namespace rapidjson;
using namespace std;
using namespace ServerCommon;

PickParUtil::PickParUtil()
{

}
PickParUtil::~PickParUtil()
{

}

bool   PickParUtil::AutoPickOutJsonPars(const string& strJsonPars, std::map<string, string>& strMap, std::map<string, __int64>& int64Map, bool bDiGui)
{
	Document d;
	if (!d.Parse(strJsonPars.c_str()).HasParseError()) {
		AutoPickOutJsonPars2(d, strMap, int64Map, bDiGui);
		return true;
	}

	return false;
}

void   PickParUtil::AutoPickOutJsonPars2(rapidjson::Document::GenericValue& d, std::map<string, string>& strMap, std::map<string, __int64>& int64Map, bool bDiGui)
{
	if (d.IsObject()) {
		auto c_iter = d.MemberBegin();
		for (; c_iter != d.MemberEnd(); c_iter++) {
			if (c_iter->value.GetType() == kStringType)
				strMap.insert(make_pair(c_iter->name.GetString(), c_iter->value.GetString()));
			else if (c_iter->value.GetType() == kNumberType) {
				if (c_iter->value.IsInt64() || c_iter->value.IsInt())
					int64Map.insert(make_pair(c_iter->name.GetString(), c_iter->value.GetInt64()));
			}
			else if (bDiGui) {
				if (c_iter->value.GetType() == kObjectType) {
					AutoPickOutJsonPars2(c_iter->value, strMap, int64Map, bDiGui);
				}
			}
		}
	}
	else if (d.IsArray()) {
		for (SizeType i = 0; i < d.Size(); i++) {
			auto& d2 = d[i];
			if (d2.GetType() == kObjectType) {
				AutoPickOutJsonPars2(d2, strMap, int64Map, bDiGui);
			}
		}
	}
}

bool   PickParUtil::AutoPickOutJsonPars(const string& strJsonPars, std::map<string, PtKVR>& kvrMap, bool bDiGui)
{
	Document d;
	if (!d.Parse(strJsonPars.c_str()).HasParseError()) {
		AutoPickOutJsonPars2(d, kvrMap, bDiGui);
		return true;
	}

	return false;
}

void   PickParUtil::AutoPickOutJsonPars2(rapidjson::Document::GenericValue& d, std::map<string, PtKVR>& kvrMap, bool bDiGui)
{
	if (d.IsObject()) {
		auto c_iter = d.MemberBegin();
		for (; c_iter != d.MemberEnd(); c_iter++) {
			if (c_iter->value.GetType() == kStringType) {
				PtKVR t_kvr;
				t_kvr.k = c_iter->name.GetString();
				t_kvr.v = c_iter->value.GetString();
				t_kvr.r = "s";
				kvrMap.insert(make_pair(t_kvr.k, t_kvr));
			}
			else if (c_iter->value.GetType() == kNumberType) {
				if (c_iter->value.IsInt64()) {
					PtKVR t_kvr;
					t_kvr.k = c_iter->name.GetString();
					char t_ch[64];
					sprintf_s(t_ch, "%lld", c_iter->value.GetInt64());
					t_kvr.v = t_ch;
					t_kvr.r = "i64";
					kvrMap.insert(make_pair(t_kvr.k, t_kvr));
				}
				else if (c_iter->value.IsDouble()) {
					PtKVR t_kvr;
					t_kvr.k = c_iter->name.GetString();
					char t_ch[64];
					sprintf_s(t_ch, "%.2lf", c_iter->value.GetDouble());
					t_kvr.v = t_ch;
					t_kvr.r = "f64";
					kvrMap.insert(make_pair(t_kvr.k, t_kvr));
				}
				else if (c_iter->value.IsInt()) {
					PtKVR t_kvr;
					t_kvr.k = c_iter->name.GetString();
					char t_ch[64];
					sprintf_s(t_ch, "%d", c_iter->value.GetInt());
					t_kvr.v = t_ch;
					t_kvr.r = "i";
					kvrMap.insert(make_pair(t_kvr.k, t_kvr));
				}
				else if (c_iter->value.IsFloat()) {
					PtKVR t_kvr;
					t_kvr.k = c_iter->name.GetString();
					char t_ch[64];
					sprintf_s(t_ch, "%.2lf", c_iter->value.GetFloat());
					t_kvr.v = t_ch;
					t_kvr.r = "f";
					kvrMap.insert(make_pair(t_kvr.k, t_kvr));
				}

			}
			else if (bDiGui) {
				if (c_iter->value.GetType() == kObjectType) {
					AutoPickOutJsonPars2(c_iter->value, kvrMap, bDiGui);
				}
			}

		}
	}
	else if (d.IsArray()) {
		for (SizeType i = 0; i < d.Size(); i++) {
			auto& d2 = d[i];
			if (d2.GetType() == kObjectType) {
				AutoPickOutJsonPars2(d2, kvrMap, bDiGui);
			}
		}
	}
}

bool   PickParUtil::AutoPickOutJsonPars_ABC(const std::string& strJsonPars, std::map<std::string, ServerCommon::PtKVR>& kvrMap)
{
	Document d;
	if (!d.Parse(strJsonPars.c_str()).HasParseError()) {
		AutoPickOutJsonPars2_ABC(d, kvrMap,"");
		return true;
	}

	return false;
}
void   PickParUtil::AutoPickOutJsonPars2_ABC(rapidjson::Document::GenericValue& d, std::map<std::string, ServerCommon::PtKVR>& kvrMap, const string& strPrefix)
{
	//"a.b.c"能很好的表达对象
	if (d.IsObject()) {
		auto c_iter = d.MemberBegin();
		for (; c_iter != d.MemberEnd(); c_iter++) {
			if (c_iter->value.GetType() == kStringType) {
				PtKVR t_kvr;
				if (strPrefix.length() > 0) {
					t_kvr.k = strPrefix;
					t_kvr.k.append(".");
				}
				t_kvr.k.append(c_iter->name.GetString());
				t_kvr.v = c_iter->value.GetString();
				t_kvr.r = "s";
				kvrMap.insert(make_pair(t_kvr.k, t_kvr));
			}
			else if (c_iter->value.GetType() == kNumberType) {
				if (c_iter->value.IsInt64()) {
					PtKVR t_kvr;
					if (strPrefix.length() > 0) {
						t_kvr.k = strPrefix;
						t_kvr.k.append(".");
					}
					t_kvr.k.append(c_iter->name.GetString());
					char t_ch[64];
					sprintf_s(t_ch, "%lld", c_iter->value.GetInt64());
					t_kvr.v = t_ch;
					t_kvr.r = "i64";
					kvrMap.insert(make_pair(t_kvr.k, t_kvr));
				}
				else if (c_iter->value.IsDouble()) {
					PtKVR t_kvr;
					if (strPrefix.length() > 0) {
						t_kvr.k = strPrefix;
						t_kvr.k.append(".");
					}
					t_kvr.k.append(c_iter->name.GetString());
					char t_ch[64];
					sprintf_s(t_ch, "%.2lf", c_iter->value.GetDouble());
					t_kvr.v = t_ch;
					t_kvr.r = "f64";
					kvrMap.insert(make_pair(t_kvr.k, t_kvr));
				}
				else if (c_iter->value.IsInt()) {
					PtKVR t_kvr;
					if (strPrefix.length() > 0) {
						t_kvr.k = strPrefix;
						t_kvr.k.append(".");
					}
					t_kvr.k.append(c_iter->name.GetString());
					char t_ch[64];
					sprintf_s(t_ch, "%d", c_iter->value.GetInt());
					t_kvr.v = t_ch;
					t_kvr.r = "i";
					kvrMap.insert(make_pair(t_kvr.k, t_kvr));
				}
				else if (c_iter->value.IsFloat()) {
					PtKVR t_kvr;
					if (strPrefix.length() > 0) {
						t_kvr.k = strPrefix;
						t_kvr.k.append(".");
					}
					t_kvr.k.append(c_iter->name.GetString());
					char t_ch[64];
					sprintf_s(t_ch, "%.2lf", c_iter->value.GetFloat());
					t_kvr.v = t_ch;
					t_kvr.r = "f";
					kvrMap.insert(make_pair(t_kvr.k, t_kvr));
				}
			}
			else if (c_iter->value.GetType() == kObjectType) {
				string t_prefix;
				if (strPrefix.length() > 0) {
					t_prefix = strPrefix;
					t_prefix.append(".");
				}
				t_prefix.append(c_iter->name.GetString());
				AutoPickOutJsonPars2_ABC(c_iter->value, kvrMap, t_prefix);
			}
			else if (c_iter->value.GetType() == kArrayType) {
				//数组要特别一些
				string t_prefix;
				if (strPrefix.length() > 0) {
					t_prefix = strPrefix;
					t_prefix.append(".");
				}
				t_prefix.append(c_iter->name.GetString());
				AutoPickOutJsonParsArray_ABC_(c_iter->value, kvrMap, t_prefix);
			}
			else if (c_iter->value.GetType() == kTrueType) {
				PtKVR t_kvr;
				if (strPrefix.length() > 0) {
					t_kvr.k = strPrefix;
					t_kvr.k.append(".");
				}
				t_kvr.k.append(c_iter->name.GetString());
				t_kvr.v = "true";
				t_kvr.r = "b";
				kvrMap.insert(make_pair(t_kvr.k, t_kvr));
			}
			else if (c_iter->value.GetType() == kFalseType) {
				PtKVR t_kvr;
				if (strPrefix.length() > 0) {
					t_kvr.k = strPrefix;
					t_kvr.k.append(".");
				}
				t_kvr.k.append(c_iter->name.GetString());
				t_kvr.v = "false";
				t_kvr.r = "b";
				kvrMap.insert(make_pair(t_kvr.k, t_kvr));
			}
		}
	}
	else if (d.IsArray()) {
		AutoPickOutJsonParsArray_ABC_(d,kvrMap,strPrefix);
	}
}
void   PickParUtil::AutoPickOutJsonParsArray_ABC_(rapidjson::Document::GenericValue& d, std::map<std::string, ServerCommon::PtKVR>& kvrMap, const string& strPrefix)
{
		if (d.IsArray()) {
			//数组只取到前3个，表示方式为...
			//假定b是数组
			//a.b.c,a.b..c,a.b...c,
			//超过3个的表示方式为
			//a.bN....c
			for (SizeType i = 0; i < d.Size(); i++) {
				auto& d2 = d[i];
				if (d2.GetType() == kStringType) {
					PtKVR t_kvr;
					if (strPrefix.length() > 0) {
						t_kvr.k = strPrefix;
						t_kvr.k.append(".");
					}					
					t_kvr.k.append(ComputeArrayABC(i).c_str());

					t_kvr.v = d2.GetString();
					t_kvr.r = "s";
					kvrMap.insert(make_pair(t_kvr.k, t_kvr));
				}
				else if (d2.GetType() == kNumberType) {
					if (d2.IsInt64()) {
						PtKVR t_kvr;
						if (strPrefix.length() > 0) {
							t_kvr.k = strPrefix;
							t_kvr.k.append(".");
						}
						t_kvr.k.append(ComputeArrayABC(i).c_str());
						char t_ch[64];
						sprintf_s(t_ch, "%lld", d2.GetInt64());
						t_kvr.v = t_ch;
						t_kvr.r = "i64";
						kvrMap.insert(make_pair(t_kvr.k, t_kvr));
					}
					else if (d2.IsDouble()) {
						PtKVR t_kvr;
						if (strPrefix.length() > 0) {
							t_kvr.k = strPrefix;
							t_kvr.k.append(".");
						}
						//t_kvr.k.append(c_iter->name.GetString());
						t_kvr.k.append(ComputeArrayABC(i).c_str());
						char t_ch[64];
						sprintf_s(t_ch, "%.2lf", d2.GetDouble());
						t_kvr.v = t_ch;
						t_kvr.r = "f64";
						kvrMap.insert(make_pair(t_kvr.k, t_kvr));
					}
					else if (d2.IsInt()) {
						PtKVR t_kvr;
						if (strPrefix.length() > 0) {
							t_kvr.k = strPrefix;
							t_kvr.k.append(".");
						}
						//t_kvr.k.append(c_iter->name.GetString());
						t_kvr.k.append(ComputeArrayABC(i).c_str());
						char t_ch[64];
						sprintf_s(t_ch, "%d", d2.GetInt());
						t_kvr.v = t_ch;
						t_kvr.r = "i";
						kvrMap.insert(make_pair(t_kvr.k, t_kvr));
					}
					else if (d2.IsFloat()) {
						PtKVR t_kvr;
						if (strPrefix.length() > 0) {
							t_kvr.k = strPrefix;
							t_kvr.k.append(".");
						}
						//t_kvr.k.append(c_iter->name.GetString());
						t_kvr.k.append(ComputeArrayABC(i).c_str());
						char t_ch[64];
						sprintf_s(t_ch, "%.2lf", d2.GetFloat());
						t_kvr.v = t_ch;
						t_kvr.r = "f";
						kvrMap.insert(make_pair(t_kvr.k, t_kvr));
					}
				}
				else if (d2.GetType() == kObjectType) {
					string t_prefix;
					if (strPrefix.length() > 0) {
						t_prefix = strPrefix;
						t_prefix.append(".");
					}
					t_prefix.append(ComputeArrayABC(i).c_str());
					AutoPickOutJsonPars2_ABC(d2, kvrMap, t_prefix);
				}
				else if (d2.GetType() == kTrueType) {
					PtKVR t_kvr;
					if (strPrefix.length() > 0) {
						t_kvr.k = strPrefix;
						t_kvr.k.append(".");
					}
					t_kvr.k.append(ComputeArrayABC(i).c_str());

					t_kvr.v = "true";
					t_kvr.r = "s";
					kvrMap.insert(make_pair(t_kvr.k, t_kvr));
				}
				else if (d2.GetType() == kFalseType) {
					PtKVR t_kvr;
					if (strPrefix.length() > 0) {
						t_kvr.k = strPrefix;
						t_kvr.k.append(".");
					}
					t_kvr.k.append(ComputeArrayABC(i).c_str());

					t_kvr.v = "false";
					t_kvr.r = "s";
					kvrMap.insert(make_pair(t_kvr.k, t_kvr));
				}
			}
		}
}
bool   PickParUtil::AutoPickOutJsonPars_ABC_PickObject(const std::string& strJsonPars, const std::string& strObject, std::map<std::string, ServerCommon::PtKVR>& kvrMap, bool bIncludeInitObj)
{
	Document d;
	if (!d.Parse(strJsonPars.c_str()).HasParseError()) {
		if (d.IsObject()) {
			auto c_iter = d.MemberBegin();
			for (; c_iter != d.MemberEnd(); c_iter++) {
				if (c_iter->value.GetType() == kObjectType && strcmp(c_iter->name.GetString(), strObject.c_str()) == 0) {
					AutoPickOutJsonPars2_ABC(c_iter->value, kvrMap, bIncludeInitObj ? strObject : "");
					return true;
				}

			}
		}
	}

	return false;
}
string PickParUtil::ComputeArrayABC(size_t i)
{
	if (i == 0)
		return "";
	else if (i == 1)
		return ".";
	else if (i == 2)
		return "..";
	
	char t_ch[32];
	sprintf_s(t_ch,32,"%d...",i);
	return t_ch;		
}
std::string PickParUtil::ToJsonString(rapidjson::Document::GenericValue& gValue, bool bPretty)
{
	rapidjson::StringBuffer buffer;
	if (bPretty) {
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
		gValue.Accept(writer);
	}
	else {
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		gValue.Accept(writer);
	}
	auto p = buffer.GetString();
	if (p)
		return p;
	return "";
}
bool  PickParUtil::AutoPickOutStringJsonParsFromJsonObject(rapidjson::Document::GenericValue& d, std::map<string, string>& strMap)
{
	if (d.IsObject()) {
		auto c_iter = d.MemberBegin();
		for (; c_iter != d.MemberEnd(); c_iter++) {
			if (c_iter->value.GetType() == kStringType)
				strMap.insert(make_pair(c_iter->name.GetString(), c_iter->value.GetString()));
		}
		return true;
	}

	return false;
}
bool   PickParUtil::AutoPickOutJsonPars_PickObject(const string& strJsonPars, const string& strObject, std::map<string, string>& strMap, std::map<string, __int64>& int64Map, bool bDiGui)
{
	Document d;
	if (!d.Parse(strJsonPars.c_str()).HasParseError()) {
		if (d.IsObject()) {
			auto c_iter = d.MemberBegin();
			for (; c_iter != d.MemberEnd(); c_iter++) {
				if (c_iter->value.GetType() == kObjectType && strcmp(c_iter->name.GetString(), strObject.c_str()) == 0) {
					AutoPickOutJsonPars2(c_iter->value, strMap, int64Map, bDiGui);
					return true;
				}

			}
		}
	}

	return false;
}
//-------------
const std::string PickParUtil::GetString_KVR(const std::map<string, PtKVR>& kvrMap, const std::string& key, const std::string& defValue) const
{
	auto iter = kvrMap.find(key);
	if (iter != kvrMap.end()) {
		return iter->second.v;
	}
	return defValue;

}
int PickParUtil::GetInt_KVR(const std::map<string, PtKVR>& kvrMap, const std::string& key, int defValue) const
{
	auto iter = kvrMap.find(key);
	if (iter != kvrMap.end()) {
		return atoi(iter->second.v.c_str());
	}
	return defValue;
}
__int64 PickParUtil::GetInt64_KVR(const std::map<string, PtKVR>& kvrMap, const std::string& key, __int64 defValue) const
{
	auto iter = kvrMap.find(key);
	if (iter != kvrMap.end()) {
		return _atoi64(iter->second.v.c_str());
	}
	return defValue;
}
/**
* 取到Double
*/
double PickParUtil::GetDouble_KVR(const std::map<string, PtKVR>& kvrMap, const std::string& key, double defValue) const
{
	auto iter = kvrMap.find(key);
	if (iter != kvrMap.end()) {
		return atof(iter->second.v.c_str());
	}
	return defValue;
}
float PickParUtil::GetFloat_KVR(const std::map<string, PtKVR>& kvrMap, const std::string& key, float defValue) const
{
	auto iter = kvrMap.find(key);
	if (iter != kvrMap.end()) {
		return (float)atof(iter->second.v.c_str());
	}
	return defValue;
}

bool  PickParUtil::GetBool_KVR(const std::map<std::string, ServerCommon::PtKVR>& kvrMap, const std::string& key, bool defValue/* = false*/) const
{
	auto iter = kvrMap.find(key);
	if (iter != kvrMap.end()) {
		if (iter->second.v == "true" || iter->second.v == "1")
			return true;
		else if (iter->second.v == "false" || iter->second.v == "0")
			return false;
	}
	return defValue;
}
bool  PickParUtil::IsHasKey_KVR(const std::map<std::string, ServerCommon::PtKVR>& kvrMap, const std::string& key) const
{
	auto iter = kvrMap.find(key);
	if (iter != kvrMap.end()) {
		return true;
	}
	return false;
}
void  PickParUtil::CopyListFrom_KVR(const std::map<std::string, ServerCommon::PtKVR>& kvrMap_s, std::map<std::string, ServerCommon::PtKVR>& kvrMap_t, const std::vector<string>& kList, bool bCover) const
{
	for (size_t i = 0; i < kList.size(); i++) {
		auto iter_s = kvrMap_s.find(kList[i]);
		if (iter_s != kvrMap_s.end()) {
			auto iter_t = kvrMap_t.find(kList[i]);
			if (iter_t != kvrMap_t.end()) {
				if (bCover)
					iter_t->second = iter_s->second;
			}
			else {
				kvrMap_t.insert(make_pair(kList[i],iter_s->second));
			}
			
		}
	}
}
void  PickParUtil::CopyFrom_KVR(const std::map<std::string, ServerCommon::PtKVR>& kvrMap_s, std::map<std::string, ServerCommon::PtKVR>& kvrMap_t, const std::string& key, bool bCover /*= true*/) const
{
	auto iter_s = kvrMap_s.find(key);
	if (iter_s != kvrMap_s.end()) {
		auto iter_t = kvrMap_t.find(key);
		if (iter_t != kvrMap_t.end()) {
			if (bCover)
				iter_t->second = iter_s->second;
		}
		else {
			kvrMap_t.insert(make_pair(key, iter_s->second));
		}

	}
}
//-------------
const std::string PickParUtil::GetString(const PtKVSSList& kvssList, const std::string& key, const std::string& defValue) const
{
	auto kv = kvssList.begin();
	for (; kv != kvssList.end(); kv++) {
		if (kv->k == key)
			return kv->v;
	}
	return defValue;
}
int PickParUtil::GetInt(const PtKVSIList& kvsiList, const std::string& key, int defValue) const
{
	auto kv = kvsiList.begin();
	for (; kv != kvsiList.end(); kv++) {
		if (kv->k == key)
			return kv->v;
	}
	return defValue;

}
__int64 PickParUtil::GetInt64(const PtKVSI64List& kvsi64List, const std::string& key, __int64 defValue) const
{
	auto kv = kvsi64List.begin();
	for (; kv != kvsi64List.end(); kv++) {
		if (kv->k == key)
			return kv->v;
	}
	return defValue;
}
const std::string PickParUtil::GetString_KVRList(const PtKVRList& kvrList, const std::string& key, const std::string& defValue) const
{
	auto kv = kvrList.begin();
	for (; kv != kvrList.end(); kv++) {
		if (kv->k == key)
			return kv->v;
	}
	return defValue;
}

//-----------
void  PickParUtil::Create_kvrList(const map<string, __int64>& kvMap, PtKVRList& kvrList)
{
	auto iter = kvMap.begin();
	for (; iter != kvMap.end(); iter++) {
		PtKVR tt_kvr;
		tt_kvr.k = iter->first;
		char tt_ch[64];
		sprintf_s(tt_ch, 64, "%lld", iter->second);
		tt_kvr.v = tt_ch;
		tt_kvr.r = "i64";
		kvrList.push_back(tt_kvr);
	}
}
void  PickParUtil::Create_kvrList(const map<string, string>& kvMap, PtKVRList& kvrList)
{
	auto iter = kvMap.begin();
	for (; iter != kvMap.end(); iter++) {
		PtKVR tt_kvr;
		tt_kvr.k = iter->first;
		tt_kvr.v = iter->second;
		tt_kvr.r = "s";
		kvrList.push_back(tt_kvr);
	}
}
void  PickParUtil::Create_kvrMap(const ServerCommon::PtKVRList& kvrList, std::map<std::string, ServerCommon::PtKVR>& kvrMap)
{
	for (size_t i = 0; i < kvrList.size(); i++) {
		kvrMap.insert(make_pair(kvrList[i].k, kvrList[i]));
	}
}
void PickParUtil::Convert_kvrMap_kvrList(const std::map<std::string, ServerCommon::PtKVR>& kvrMap, ServerCommon::PtKVRList& kvrList)
{
	auto iter = kvrMap.begin();
	for (; iter != kvrMap.end(); iter++) {
		kvrList.push_back(iter->second);
	}
}
void PickParUtil::Convert_kvrList_kvrMap(const ServerCommon::PtKVRList& kvrList, std::map<std::string, ServerCommon::PtKVR>& kvrMap)
{
	for (size_t i = 0; i < kvrList.size(); i++) {
		kvrMap.insert(make_pair(kvrList[i].k, kvrList[i]));
	}
}

void PickParUtil::Convert_strMap_kvrMap(const std::map<string, string>& strMap, std::map<std::string, ServerCommon::PtKVR>& kvrMap)
{
	auto iter_s = strMap.begin();
	for (; iter_s != strMap.end(); iter_s++) {
		PtKVR t_kvr;
		t_kvr.k = iter_s->first;
		t_kvr.v = iter_s->second;
		t_kvr.r = "s";
		kvrMap.insert(make_pair(t_kvr.k, t_kvr));
	}
}
void PickParUtil::Convert_int64Map_kvrMap(const std::map<string, __int64>& int64Map, std::map<std::string, ServerCommon::PtKVR>& kvrMap)
{
	char t_ch[64];
	auto iter_s = int64Map.begin();
	for (; iter_s != int64Map.end(); iter_s++) {
		PtKVR t_kvr;
		t_kvr.k = iter_s->first;
		sprintf_s(t_ch, 64, "%lld", iter_s->second);
		t_kvr.v = t_ch;
		t_kvr.r = "s";
		kvrMap.insert(make_pair(t_kvr.k, t_kvr));
	}
}
void PickParUtil::GetKvrList(ServerCommon::PtKVRList& kvrList, const std::map<std::string, ServerCommon::PtKVR>& kvrMap, const string& kCondition)
{
	auto iter = kvrMap.begin();
	for (; iter != kvrMap.end(); iter++) {
		if (iter->first.find(kCondition.c_str()) == 0) {
			kvrList.push_back(iter->second);
		}
	}
}
void PickParUtil::GetKvrMap(std::map<std::string, ServerCommon::PtKVR>& kvrMap_out, const std::map<std::string, ServerCommon::PtKVR>& kvrMap, const string& kCondition)
{
	auto iter = kvrMap.begin();
	for (; iter != kvrMap.end(); iter++) {
		if (iter->first.find(kCondition.c_str()) == 0) {
			kvrMap_out.insert(make_pair(iter->first, iter->second));
		}
	}
}
bool PickParUtil::IsHasConditionK(const std::map<std::string, ServerCommon::PtKVR>& kvrMap, const string& kCondition)
{
	auto iter = kvrMap.begin();
	for (; iter != kvrMap.end(); iter++) {
		if (iter->first.find(kCondition.c_str()) == 0) {
			return true;
		}
	}

	return false;
}
void PickParUtil::GetKvrList_EndDuan(ServerCommon::PtKVRList& kvrList_out, const ServerCommon::PtKVRList& kvrList)
{
	for (size_t i = 0; i < kvrList.size(); i++) {
		auto t_pos = kvrList[i].k.rfind(".");
		if (t_pos != string::npos) {
			auto s = kvrList[i].k.substr(t_pos + 1);
			if(s.length()==0)
				continue;

			PtKVR t;
			t.k = s;
			t.v = kvrList[i].v;
			t.r = kvrList[i].r;
			kvrList_out.push_back(t);
		}
	}
}
#ifndef _EXCLUDEXML_
void    PickParUtil::AutoPickOutXmlPar(const string& strXML, std::map<string, string>& strMap)
{
	TiXmlDocument tt_doc;
	tt_doc.Parse(strXML.c_str());
	TiXmlNode* pNode = 0;
	TiXmlElement* pElement = 0;
	//解析根节点
	pNode = tt_doc.RootElement();
	if (pNode) {
		pElement = pNode->ToElement();
	}

	while (true && pNode && pElement) {

		const TiXmlAttribute* pAttr = pElement->FirstAttribute();
		while (pAttr) {
			string s1 = pAttr->Name();
			string s2 = pAttr->Value();
			auto iter = strMap.find(s1);
			if (iter != strMap.end()) {
				iter->second = s2;
			}
			else
				strMap.insert(make_pair(s1, s2));
			pAttr = pAttr->Next();
		}
		pNode = pNode->NextSiblingElement();
		if (!pNode)
			break;
		pElement = pNode->ToElement();
	}//while	
}
#endif

void   PickParUtil::CreateKVRList(const map<string, string>& strMap, const map<string, __int64>& i64Map, PtKVRList& kvrList)
{
	auto iter_s = strMap.begin();
	for (; iter_s != strMap.end(); iter_s++) {
		PtKVR tt_kvr;
		tt_kvr.k = iter_s->first;
		tt_kvr.v = iter_s->second;
		tt_kvr.r = "s";
		kvrList.push_back(tt_kvr);
	}

	auto iter_i = i64Map.begin();
	for (; iter_i != i64Map.end(); iter_i++) {
		PtKVR tt_kvr;
		tt_kvr.k = iter_i->first;
		char tt_ch[64];
		sprintf_s(tt_ch, 64, "%lld", iter_i->second);
		tt_kvr.v = tt_ch;
		tt_kvr.r = "i64";
		kvrList.push_back(tt_kvr);
	}
}
string  PickParUtil::CreateBody_Json(const PtKVSSList& kvList, const  std::map<string, PtKVR>& kvrMap)
{
	rapidjson::StringBuffer strJson;
	rapidjson::Writer<rapidjson::StringBuffer> writer(strJson);
	writer.StartObject();

	for (size_t i = 0; i < kvList.size(); i++) {
		if (kvList[i].v == "objopen") {
			writer.Key(kvList[i].k.c_str());
			writer.StartObject();
			continue;
		}
		else if (kvList[i].v == "objclose") {
			writer.EndObject();
			continue;
		}
		else if (kvList[i].v == "arropen") {
			writer.Key(kvList[i].k.c_str());
			writer.StartArray();
			continue;
		}
		else if (kvList[i].v == "arrclose") {
			writer.EndArray();
			continue;
		}

		auto iter = kvrMap.find(kvList[i].k);
		if (iter != kvrMap.end()) {
			writer.Key(kvList[i].k.c_str());
			if (kvList[i].v == "i64")
				writer.Int64(_atoi64(iter->second.v.c_str()));
			else if (kvList[i].v == "i")
				writer.Int(atoi(iter->second.v.c_str()));
			else if (kvList[i].v == "ts" || kvList[i].v == "tms")
				writer.Int64(_atoi64(iter->second.v.c_str()));
			else if (kvList[i].v == "s")
				writer.String(iter->second.v.c_str());
			else if (kvList[i].v == "f")
				writer.Double(atof(iter->second.v.c_str()));
			else if (kvList[i].v == "f64")
				writer.Double(atof(iter->second.v.c_str()));
			else
				writer.String(iter->second.v.c_str());
		}
		else {
			if (kvList[i].v == "tms") {
				__int64 tt_curTime = 0;
				_time64(&tt_curTime);
				writer.Key(kvList[i].k.c_str());
				writer.Int64(tt_curTime * 1000);
			}
			else if (kvList[i].v == "ts") {
				__int64 tt_curTime = 0;
				_time64(&tt_curTime);
				writer.Key(kvList[i].k.c_str());
				writer.Int64(tt_curTime);
			}
		}
	}

	writer.EndObject();
	//writer.Flush();
	return string(strJson.GetString(), strJson.GetLength());
}

string  PickParUtil::CreateBody_Url(const PtKVSSList& kvList, const  std::map<string, PtKVR>& kvrMap, const string& split)
{
	string tt_s;
	for (size_t i = 0; i < kvList.size(); i++) {
		if (i != 0) {
			tt_s.append(split.c_str());
		}
		tt_s.append(kvList[i].k.c_str());
		tt_s.append("=");
		auto iter = kvrMap.find(kvList[i].k);
		if (iter != kvrMap.end()) {
			tt_s.append(iter->second.v.c_str());
		}
	}
	return std::move(tt_s);
}
string  PickParUtil::CreateBody_Url(const PtKVSSList& kvList, const  std::map<string, string>& strMap, const string& split)
{
	string tt_s;
	for (size_t i = 0; i < kvList.size(); i++) {
		if (i != 0) {
			tt_s.append(split.c_str());
		}
		tt_s.append(kvList[i].k.c_str());
		tt_s.append("=");
		auto iter = strMap.find(kvList[i].k);
		if (iter != strMap.end()) {
			tt_s.append(iter->second.c_str());
		}
	}
	return std::move(tt_s);
}

__int64 PickParUtil::GetInt64(const map<string, __int64>& i64Map, const std::string& key, __int64 defValue /*= 0*/) const
{
	auto iter = i64Map.find(key);
	if (iter != i64Map.end())
		return iter->second;

	return defValue;
}
void  PickParUtil::SetString(PtKVSSList& kvssList, const std::string& key, const std::string& value)
{
	auto kv = kvssList.begin();
	for (; kv != kvssList.end(); kv++) {
		if (kv->k == key) {
			kv->v = value;
			return;
		}
	}

	PtKVSS tt_kv;
	tt_kv.k = key;
	tt_kv.v = value;
	kvssList.push_back(tt_kv);
}
void PickParUtil::SetInt(PtKVSIList& kvsiList, const std::string& key, int v)
{
	auto kv = kvsiList.begin();
	for (; kv != kvsiList.end(); kv++) {
		if (kv->k == key) {
			kv->v = v;
			return;
		}
	}
	PtKVSI tt_kv;
	tt_kv.k = key;
	tt_kv.v = v;
	kvsiList.push_back(tt_kv);

}
void    PickParUtil::SetInt64(PtKVSI64List& kvsi64List, const std::string& key, __int64 v64)
{
	auto kv = kvsi64List.begin();
	for (; kv != kvsi64List.end(); kv++) {
		if (kv->k == key) {
			kv->v = v64;
			return;
		}
	}
	PtKVSI64 tt_kv64;
	tt_kv64.k = key;
	tt_kv64.v = v64;
	kvsi64List.push_back(tt_kv64);

}

//-----------
string  PickParUtil::GetValueFromKVList(const PtKeyValueDataList& ParList, const string& strKey)
{
	for (size_t i = 0; i < ParList.size(); i++) {
		if (ParList[i].key.compare(strKey.c_str()) == 0)
			return ParList[i].value;
	}

	return "";
}

void PickParUtil::CreateKVIIList(ServerCommon::PtKVIIList& rKVIIList, const ServerCommon::PtKVRList& kvrList)
{
	for (size_t i = 0; i < kvrList.size(); i++) {
		PtKVII t_kvII;
		t_kvII.k = atoi(kvrList[i].k.c_str());
		t_kvII.v = atoi(kvrList[i].v.c_str());
		rKVIIList.push_back(t_kvII);
	}
}