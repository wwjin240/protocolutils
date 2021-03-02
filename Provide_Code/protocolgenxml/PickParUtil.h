#pragma once
#include "ShareHead.h"

#include <map>
#include <vector>
#include <string>

#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON


/**
 * ��ȡ�������ܺ���
 */
class PickParUtil
{
public:
	PickParUtil();
	~PickParUtil();

public:
	/**
	* �Զ���ȡjson�Σ�ֻ֧���ַ��������ͣ�64λ����
	*/
	bool   AutoPickOutJsonPars(const std::string& strJsonPars, std::map<std::string, std::string>& strMap, std::map<std::string, __int64>& int64Map, bool bDiGui = false);
	/**
	* �Զ���ȡjson�Σ�ֻ֧���ַ��������ͣ�64λ����
	*/
	void   AutoPickOutJsonPars2(rapidjson::Document::GenericValue& d, std::map<std::string, std::string>& strMap, std::map<std::string, __int64>& int64Map, bool bDiGui);

public:
	/**
	* �Զ���ȡjson�Σ�֧���ַ�����__int64->double->int->float
	*/
	bool   AutoPickOutJsonPars(const std::string& strJsonPars, std::map<std::string, ServerCommon::PtKVR>& kvrMap, bool bDiGui = false);
	/**
	* �Զ���ȡjson�Σ�ֻ֧���ַ��������ͣ�64λ����
	*/
	void   AutoPickOutJsonPars2(rapidjson::Document::GenericValue& d, std::map<std::string, ServerCommon::PtKVR>& kvrMap, bool bDiGui);
public:
	/**
	* �Զ���ȡjson�Σ�֧���ַ�����__int64->double->int->float,���ְ�a.b.c
	*/
	bool   AutoPickOutJsonPars_ABC(const std::string& strJsonPars, std::map<std::string, ServerCommon::PtKVR>& kvrMap);
	/**
	* �Զ���ȡjson�Σ�ֻ֧���ַ��������ͣ�64λ����
	*/
	void   AutoPickOutJsonPars2_ABC(rapidjson::Document::GenericValue& d, std::map<std::string, ServerCommon::PtKVR>& kvrMap, const std::string& strPrefix);
	/**
	* �Զ���ȡjson�Σ�֧���ַ�����__int64->double->int->float,���ְ�a.b.c
	*/
	bool   AutoPickOutJsonPars_ABC_PickObject(const std::string& strJsonPars, const std::string& strObject, std::map<std::string, ServerCommon::PtKVR>& kvrMap,bool bIncludeInitObj=true);
private:
	void   AutoPickOutJsonParsArray_ABC_(rapidjson::Document::GenericValue& d, std::map<std::string, ServerCommon::PtKVR>& kvrMap, const std::string& strPrefix);
private:
	std::string ComputeArrayABC(size_t i);

public:
	/**
	* ��json����ת���ַ���
	*/
	std::string ToJsonString(rapidjson::Document::GenericValue& gValue,bool bPretty=false);

	/**
	 * ֻȡ�ַ�����
	 */
	bool   AutoPickOutStringJsonParsFromJsonObject(rapidjson::Document::GenericValue& d, std::map<std::string, std::string>& strMap);
	/**
	 * ��ĳ������ʼȡ
	 */
	bool   AutoPickOutJsonPars_PickObject(const std::string& strJsonPars, const std::string& strObject, std::map<std::string, std::string>& strMap, std::map<std::string, __int64>& int64Map, bool bDiGui = false);


public:
	/**
	* ȡ���ַ���ֵ
	*/
	const std::string GetString_KVR(const std::map<std::string, ServerCommon::PtKVR>& kvrMap, const std::string& key, const std::string& defValue = "") const;
	/**
	* ȡ��Int
	*/
	int GetInt_KVR(const std::map<std::string, ServerCommon::PtKVR>& kvrMap, const std::string& key, int defValue = 0) const;
	/**
	* ȡ��Int64
	*/
	__int64 GetInt64_KVR(const std::map<std::string, ServerCommon::PtKVR>& kvrMap, const std::string& key, __int64 defValue = 0) const;
	/**
	* ȡ��Double
	*/
	double GetDouble_KVR(const std::map<std::string, ServerCommon::PtKVR>& kvrMap, const std::string& key, double defValue = 0) const;
	/**
	* ȡ��float
	*/
	float GetFloat_KVR(const std::map<std::string, ServerCommon::PtKVR>& kvrMap, const std::string& key, float defValue = 0) const;
	/**
	 * ȡ��bool��
	 */
	bool  GetBool_KVR(const std::map<std::string, ServerCommon::PtKVR>& kvrMap, const std::string& key,bool defValue=false) const;
	/**
	 * �Ƿ���ĳ����
	 */
	bool  IsHasKey_KVR(const std::map<std::string, ServerCommon::PtKVR>& kvrMap, const std::string& key) const;
	/**
	 * ����
	 */
	void  CopyListFrom_KVR(const std::map<std::string, ServerCommon::PtKVR>& kvrMap_s, std::map<std::string, ServerCommon::PtKVR>& kvrMap_t, const std::vector<std::string>& kList,bool bCover=true) const;
	/**
	* ����
	*/
	void  CopyFrom_KVR(const std::map<std::string, ServerCommon::PtKVR>& kvrMap_s, std::map<std::string, ServerCommon::PtKVR>& kvrMap_t, const std::string& key, bool bCover = true) const;

public:
	/**
	* ȡ���ַ���ֵ
	*/
	const std::string GetString(const ServerCommon::PtKVSSList& kvssList, const std::string& key, const std::string& defValue = "") const;
	/**
	* ȡ��Intֵ
	*/
	int GetInt(const ServerCommon::PtKVSIList& kvsiList, const std::string& key, int defValue = 0) const;
	/**
	* ȡ��Int64ֵ
	*/
	__int64 GetInt64(const ServerCommon::PtKVSI64List& kvsi64List, const std::string& key, __int64 defValue = 0) const;
	/**
	* ȡ���ַ���ֵ
	*/
	const std::string GetString_KVRList(const ServerCommon::PtKVRList& kvrList, const std::string& key, const std::string& defValue = "") const;

public:
	/**
	* mapת���б�
	*/
	void  Create_kvrList(const std::map<std::string, __int64>& kvMap, ServerCommon::PtKVRList& kvrList);
	/**
	* mapת���б�
	*/
	void  Create_kvrList(const std::map<std::string, std::string>& kvMap, ServerCommon::PtKVRList& kvrList);
	/**
	* ���б�ת��Map
	*/
	void  Create_kvrMap(const ServerCommon::PtKVRList& kvrList, std::map<std::string, ServerCommon::PtKVR>& kvrMap);
	/**
	 * ת����kvrList
	 */
	void Convert_kvrMap_kvrList(const std::map<std::string, ServerCommon::PtKVR>& kvrMap, ServerCommon::PtKVRList& kvrList);
	/**
	 * ת����kvrMap
	 */
	void Convert_kvrList_kvrMap(const ServerCommon::PtKVRList& kvrList, std::map<std::string, ServerCommon::PtKVR>& kvrMap);
	/**
	 * ת����kvrMap
	 */
	void Convert_strMap_kvrMap(const std::map<std::string,std::string>& strMap, std::map<std::string, ServerCommon::PtKVR>& kvrMap);
	/**
	 * ת����kvrMap
	 */
	void Convert_int64Map_kvrMap(const std::map<std::string, __int64>& int64Map, std::map<std::string, ServerCommon::PtKVR>& kvrMap);
	/**
	 * �õ�KVR�б�
	 * �ҵ�������kCondition��ʼ��KVR
	 */
	void GetKvrList(ServerCommon::PtKVRList& kvrList,const std::map<std::string, ServerCommon::PtKVR>& kvrMap,const std::string& kCondition);
	/**
	 * �õ�KVRMap
	 * �ҵ�������kCondition��ʼ��KVR
	 */
	void GetKvrMap(std::map<std::string, ServerCommon::PtKVR>& kvrMap_out, const std::map<std::string, ServerCommon::PtKVR>& kvrMap, const std::string& kCondition);
	/**
	 * �Ƿ��ж�Ӧ��
	 */
	bool IsHasConditionK(const std::map<std::string, ServerCommon::PtKVR>& kvrMap, const std::string& kCondition);
	/**
	 * ȡ�����ε��б�
	 */
	void GetKvrList_EndDuan(ServerCommon::PtKVRList& kvrList_out, const ServerCommon::PtKVRList& kvrList);
public:
#ifndef _EXCLUDEXML_
	/**
	* �Զ���ȡxml��
	*/
	void    AutoPickOutXmlPar(const std::string& strXML, std::map<std::string, std::string>& strMap);
#endif

public:
	/**
	 * ���ַ����б�i64�б�ת��kvr�б�
	 */
	void   CreateKVRList(const std::map<std::string, std::string>& strMap, const std::map<std::string, __int64>& i64Map, ServerCommon::PtKVRList& kvrList);

public:
	/**
	* ����json��body
	*/
	std::string  CreateBody_Json(const ServerCommon::PtKVSSList& kvList, const  std::map<std::string, ServerCommon::PtKVR>& kvrMap);
	/**
	* ����body
	*/
	std::string  CreateBody_Url(const ServerCommon::PtKVSSList& kvList, const  std::map<std::string, ServerCommon::PtKVR>& kvrMap, const std::string& split = "&");
	/**
	* ����body
	*/
	std::string  CreateBody_Url(const ServerCommon::PtKVSSList& kvList, const  std::map<std::string, std::string>& strMap, const std::string& split = "&");

public:
	__int64 GetInt64(const std::map<std::string, __int64>& i64Map, const std::string& key, __int64 defValue = 0) const;

public:
	/**
	 * ���ַ���ֵ
	 */
	void         SetString(ServerCommon::PtKVSSList& kvssList, const std::string& key, const std::string& value);
	/**
	 * ��Intֵ
	 */
	void SetInt(ServerCommon::PtKVSIList& kvsiList, const std::string& key, int v);
	/**
	 * ��Int64ֵ
	 */
	void    SetInt64(ServerCommon::PtKVSI64List& kvsi64List, const std::string& key, __int64 v64);

public:
	/**
	* ȡ������Ӧ��ֵ
	*/
	std::string  GetValueFromKVList(const ServerCommon::PtKeyValueDataList& ParList, const std::string& strKey);
	/**
	 * ����KVIIList
	 */
	void CreateKVIIList(ServerCommon::PtKVIIList& rKVIIList, const ServerCommon::PtKVRList& kvrList);
};
