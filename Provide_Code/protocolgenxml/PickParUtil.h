#pragma once
#include "ShareHead.h"

#include <map>
#include <vector>
#include <string>

#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON


/**
 * 提取参数功能函数
 */
class PickParUtil
{
public:
	PickParUtil();
	~PickParUtil();

public:
	/**
	* 自动提取json参，只支持字符串，整型，64位整型
	*/
	bool   AutoPickOutJsonPars(const std::string& strJsonPars, std::map<std::string, std::string>& strMap, std::map<std::string, __int64>& int64Map, bool bDiGui = false);
	/**
	* 自动提取json参，只支持字符串，整型，64位整型
	*/
	void   AutoPickOutJsonPars2(rapidjson::Document::GenericValue& d, std::map<std::string, std::string>& strMap, std::map<std::string, __int64>& int64Map, bool bDiGui);

public:
	/**
	* 自动提取json参，支持字符串，__int64->double->int->float
	*/
	bool   AutoPickOutJsonPars(const std::string& strJsonPars, std::map<std::string, ServerCommon::PtKVR>& kvrMap, bool bDiGui = false);
	/**
	* 自动提取json参，只支持字符串，整型，64位整型
	*/
	void   AutoPickOutJsonPars2(rapidjson::Document::GenericValue& d, std::map<std::string, ServerCommon::PtKVR>& kvrMap, bool bDiGui);
public:
	/**
	* 自动提取json参，支持字符串，__int64->double->int->float,名字按a.b.c
	*/
	bool   AutoPickOutJsonPars_ABC(const std::string& strJsonPars, std::map<std::string, ServerCommon::PtKVR>& kvrMap);
	/**
	* 自动提取json参，只支持字符串，整型，64位整型
	*/
	void   AutoPickOutJsonPars2_ABC(rapidjson::Document::GenericValue& d, std::map<std::string, ServerCommon::PtKVR>& kvrMap, const std::string& strPrefix);
	/**
	* 自动提取json参，支持字符串，__int64->double->int->float,名字按a.b.c
	*/
	bool   AutoPickOutJsonPars_ABC_PickObject(const std::string& strJsonPars, const std::string& strObject, std::map<std::string, ServerCommon::PtKVR>& kvrMap,bool bIncludeInitObj=true);
private:
	void   AutoPickOutJsonParsArray_ABC_(rapidjson::Document::GenericValue& d, std::map<std::string, ServerCommon::PtKVR>& kvrMap, const std::string& strPrefix);
private:
	std::string ComputeArrayABC(size_t i);

public:
	/**
	* 将json对象转成字符串
	*/
	std::string ToJsonString(rapidjson::Document::GenericValue& gValue,bool bPretty=false);

	/**
	 * 只取字符串版
	 */
	bool   AutoPickOutStringJsonParsFromJsonObject(rapidjson::Document::GenericValue& d, std::map<std::string, std::string>& strMap);
	/**
	 * 从某个对象开始取
	 */
	bool   AutoPickOutJsonPars_PickObject(const std::string& strJsonPars, const std::string& strObject, std::map<std::string, std::string>& strMap, std::map<std::string, __int64>& int64Map, bool bDiGui = false);


public:
	/**
	* 取到字符串值
	*/
	const std::string GetString_KVR(const std::map<std::string, ServerCommon::PtKVR>& kvrMap, const std::string& key, const std::string& defValue = "") const;
	/**
	* 取到Int
	*/
	int GetInt_KVR(const std::map<std::string, ServerCommon::PtKVR>& kvrMap, const std::string& key, int defValue = 0) const;
	/**
	* 取到Int64
	*/
	__int64 GetInt64_KVR(const std::map<std::string, ServerCommon::PtKVR>& kvrMap, const std::string& key, __int64 defValue = 0) const;
	/**
	* 取到Double
	*/
	double GetDouble_KVR(const std::map<std::string, ServerCommon::PtKVR>& kvrMap, const std::string& key, double defValue = 0) const;
	/**
	* 取到float
	*/
	float GetFloat_KVR(const std::map<std::string, ServerCommon::PtKVR>& kvrMap, const std::string& key, float defValue = 0) const;
	/**
	 * 取到bool型
	 */
	bool  GetBool_KVR(const std::map<std::string, ServerCommon::PtKVR>& kvrMap, const std::string& key,bool defValue=false) const;
	/**
	 * 是否有某个键
	 */
	bool  IsHasKey_KVR(const std::map<std::string, ServerCommon::PtKVR>& kvrMap, const std::string& key) const;
	/**
	 * 复制
	 */
	void  CopyListFrom_KVR(const std::map<std::string, ServerCommon::PtKVR>& kvrMap_s, std::map<std::string, ServerCommon::PtKVR>& kvrMap_t, const std::vector<std::string>& kList,bool bCover=true) const;
	/**
	* 复制
	*/
	void  CopyFrom_KVR(const std::map<std::string, ServerCommon::PtKVR>& kvrMap_s, std::map<std::string, ServerCommon::PtKVR>& kvrMap_t, const std::string& key, bool bCover = true) const;

public:
	/**
	* 取到字符串值
	*/
	const std::string GetString(const ServerCommon::PtKVSSList& kvssList, const std::string& key, const std::string& defValue = "") const;
	/**
	* 取到Int值
	*/
	int GetInt(const ServerCommon::PtKVSIList& kvsiList, const std::string& key, int defValue = 0) const;
	/**
	* 取到Int64值
	*/
	__int64 GetInt64(const ServerCommon::PtKVSI64List& kvsi64List, const std::string& key, __int64 defValue = 0) const;
	/**
	* 取到字符串值
	*/
	const std::string GetString_KVRList(const ServerCommon::PtKVRList& kvrList, const std::string& key, const std::string& defValue = "") const;

public:
	/**
	* map转成列表
	*/
	void  Create_kvrList(const std::map<std::string, __int64>& kvMap, ServerCommon::PtKVRList& kvrList);
	/**
	* map转成列表
	*/
	void  Create_kvrList(const std::map<std::string, std::string>& kvMap, ServerCommon::PtKVRList& kvrList);
	/**
	* 将列表转成Map
	*/
	void  Create_kvrMap(const ServerCommon::PtKVRList& kvrList, std::map<std::string, ServerCommon::PtKVR>& kvrMap);
	/**
	 * 转换成kvrList
	 */
	void Convert_kvrMap_kvrList(const std::map<std::string, ServerCommon::PtKVR>& kvrMap, ServerCommon::PtKVRList& kvrList);
	/**
	 * 转换成kvrMap
	 */
	void Convert_kvrList_kvrMap(const ServerCommon::PtKVRList& kvrList, std::map<std::string, ServerCommon::PtKVR>& kvrMap);
	/**
	 * 转换成kvrMap
	 */
	void Convert_strMap_kvrMap(const std::map<std::string,std::string>& strMap, std::map<std::string, ServerCommon::PtKVR>& kvrMap);
	/**
	 * 转换成kvrMap
	 */
	void Convert_int64Map_kvrMap(const std::map<std::string, __int64>& int64Map, std::map<std::string, ServerCommon::PtKVR>& kvrMap);
	/**
	 * 得到KVR列表
	 * 找到所有以kCondition开始的KVR
	 */
	void GetKvrList(ServerCommon::PtKVRList& kvrList,const std::map<std::string, ServerCommon::PtKVR>& kvrMap,const std::string& kCondition);
	/**
	 * 得到KVRMap
	 * 找到所有以kCondition开始的KVR
	 */
	void GetKvrMap(std::map<std::string, ServerCommon::PtKVR>& kvrMap_out, const std::map<std::string, ServerCommon::PtKVR>& kvrMap, const std::string& kCondition);
	/**
	 * 是否有对应键
	 */
	bool IsHasConditionK(const std::map<std::string, ServerCommon::PtKVR>& kvrMap, const std::string& kCondition);
	/**
	 * 取出最后段的列表
	 */
	void GetKvrList_EndDuan(ServerCommon::PtKVRList& kvrList_out, const ServerCommon::PtKVRList& kvrList);
public:
#ifndef _EXCLUDEXML_
	/**
	* 自动提取xml参
	*/
	void    AutoPickOutXmlPar(const std::string& strXML, std::map<std::string, std::string>& strMap);
#endif

public:
	/**
	 * 将字符串列表，i64列表转成kvr列表
	 */
	void   CreateKVRList(const std::map<std::string, std::string>& strMap, const std::map<std::string, __int64>& i64Map, ServerCommon::PtKVRList& kvrList);

public:
	/**
	* 创建json版body
	*/
	std::string  CreateBody_Json(const ServerCommon::PtKVSSList& kvList, const  std::map<std::string, ServerCommon::PtKVR>& kvrMap);
	/**
	* 创建body
	*/
	std::string  CreateBody_Url(const ServerCommon::PtKVSSList& kvList, const  std::map<std::string, ServerCommon::PtKVR>& kvrMap, const std::string& split = "&");
	/**
	* 创建body
	*/
	std::string  CreateBody_Url(const ServerCommon::PtKVSSList& kvList, const  std::map<std::string, std::string>& strMap, const std::string& split = "&");

public:
	__int64 GetInt64(const std::map<std::string, __int64>& i64Map, const std::string& key, __int64 defValue = 0) const;

public:
	/**
	 * 设字符串值
	 */
	void         SetString(ServerCommon::PtKVSSList& kvssList, const std::string& key, const std::string& value);
	/**
	 * 设Int值
	 */
	void SetInt(ServerCommon::PtKVSIList& kvsiList, const std::string& key, int v);
	/**
	 * 设Int64值
	 */
	void    SetInt64(ServerCommon::PtKVSI64List& kvsi64List, const std::string& key, __int64 v64);

public:
	/**
	* 取到键对应的值
	*/
	std::string  GetValueFromKVList(const ServerCommon::PtKeyValueDataList& ParList, const std::string& strKey);
	/**
	 * 创建KVIIList
	 */
	void CreateKVIIList(ServerCommon::PtKVIIList& rKVIIList, const ServerCommon::PtKVRList& kvrList);
};
