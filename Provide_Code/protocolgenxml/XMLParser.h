#pragma once
#include "ProtocolParserHead.h"

#include <vector>
#include <string>
#include <map>
#include <set>

#include "../tinyxml/tinyxml.h"

#include "Logger.h"
#include "ProtocolUtils.h"

using namespace std;

/**
 * XML解析
 */
class XMLParser
{
public:
	XMLParser();
	virtual ~XMLParser();

public:
	void Free_();

public:
	bool ParserXMLFile(const string& strFileName);

public:
	bool ParserNamespace(TiXmlNode* pNode, TiXmlElement* pElement); //解析命名空间
	bool ParserProtocolNo(TiXmlNode* pNode, TiXmlElement* pElement); //解析协议号
	bool ParserProtocolRef(TiXmlNode* pNode, TiXmlElement* pElement); //解析协议引用
	bool ParserVersionManger(TiXmlNode* pNode, TiXmlElement* pElement); //解析协议版本信息
	bool ParserPartManager(TiXmlNode* pNode, TiXmlElement* pElement);  //解析区的管理

public:
	bool ParserEnum(TiXmlNode* pNode, TiXmlElement* pElement); //解析枚举

public:
	bool ParserStruct(TiXmlNode* pNode, TiXmlElement* pElement); //解析结构体
	bool IsAllAttributeRecongnize_Struct(TiXmlElement* pElement, const string& structName);
	bool IsAllAttributeRecongnize_StructPar(TiXmlElement* pElement, const string& structName, const string& parName);

public:
	bool ParserVector(TiXmlNode* pNode, TiXmlElement* pElement); //解析vector
	bool ParserWJSVector(TiXmlNode* pNode, TiXmlElement* pElement);//解析WJSByteSeq

public:
	bool ParserClass(TiXmlNode* pNode, TiXmlElement* pElement, bool brequest); //解析类
	bool ParserClassMethod(TiXmlNode* pNode, TiXmlElement* pElement, stClass& rClass, const string& part); //解析类里的方法
	bool ParserMethod(TiXmlNode* pNode, TiXmlElement* pElement, stFunc& rFunc);//解析方法
	bool IsAllAttributeRecongnize_Class(TiXmlElement* pElement, const string& className);
	bool IsAllAttributeRecongnize_Func(TiXmlElement* pElement, const string& funcName);
	bool IsAllAttributeRecongnize_FuncPar(TiXmlElement* pElement, const string& funcName, const string& parName);

#pragma region 类型判定
public:
	virtual bool IsValidType(const string& strtypename);  //是否是有效的类型
	bool IsBaseType(const string& strtypename); //确认是否是基础类型
	bool IsBaseType_WriterType(const string& strTypeName, eProtocolWriterType writerType);//确认是否是基础类型,writerType版
	virtual bool IsExType_enum(const string& strtypename); //是否是扩展的枚举类型
	virtual bool IsExType_struct(const string& strtypename);//是否是扩展类型,如结构体
	virtual bool IsExType_vector(const string& strtypename);//是否是扩展类型, vector
	virtual bool IsExType_WJSVector(const string& strtypename); //是否是另外版本的类vector型

#pragma endregion

#pragma region 测试区
public:
	bool    TestPartIsOk(const string& strPart); //测试区是否OK
	void    TestChangeInt64ToLongLong(string& strOldType); //int64转到longlong
	virtual void   TestBaseParIsConstRef(stBasePar& pPar); //测试是否是constref,这里不判引用的协议的
	virtual void   RefProtocolProcess() {}; //引用协议处理
#pragma endregion

#pragma region 工具型函数
	void   ToLower(string &str);  //转成小写
	string ToLowerV2(string &str);
	string         GetUTF8(LPCTSTR lpInput);
	void           UTF8Encode(LPCTSTR lpInput, std::string &strOutput);
	void           UTF8Decode(const char *pInput, string &strOutput);

	int    GetBaseTypeNum(); //得到基本类型数

	void   AddToPartManage(const string& partManageName,const string& partName,const string& comment); //加到区管理

	void   AddToYuanPartList(const string& partName); //加到原partList

	string  GetBaseTypeDefault(const std::string& typeName);


#pragma endregion 

public:
	stProtocolNo              m_ProtocolNo; //协议号
	vector<stRefProtocol>     m_RefProtocolList;//引用协议
	stSaveFileName            m_SaveFileName; //存储名
	vector<stNamespace>       m_NamespaceVec; //命名空间
	map<string, set<string>>  m_PartManageMap; //区的管理
	vector<stPartManager>     m_PartManageList;//顺序版的区管理
	stVersionManger           m_VersionManger; //版本管理
	string                    m_shortName;     //短名

	vector<string>            m_YuanPartList;  //原本的partlist

public:
	int             m_nStructVecCount; //考虑到结构体和vector的顺序问题，加此变量

public:
	bool            m_bHCPP;               //生成H与CPP两个文件
	bool            m_bUsePartProtocol;    //是否使用了区协议 
	bool            m_bExceptMoba;         //是否排除Moba
	string          m_UsePartManager;      //使用哪个区管理
	bool            m_bAndroidPeer;       //是否是安卓端

public:
	string          m_strErrorReason;//错误原因

	std::map<std::string, std::string>  m_baseTypeDefaultMap;  //基本类型的默认值



public:
#ifdef _DEBUG
	FileLogger      _Logger;       //日记
#endif
};