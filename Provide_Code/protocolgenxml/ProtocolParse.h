/*
* create by  wwjin
*/

//协议解析类
#pragma once
#include "ProtocolParserHead.h"

#include <string>
#include <vector>
#include "WJS_BBSTree.h"
#include "Logger.h"
#include <map>
#include <set>

#include "../tinyxml/tinyxml.h"

#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON

#include "XMLParser.h"
#include <functional>

using namespace rapidjson;

using namespace std;

//#ifndef BASETYPENUM
//#define BASETYPENUM 25
//#endif



/////////////////////////////

//检测某个参数是否使用过
class ICheckParUsed {
public:
	virtual bool Call_CheckParUsed(const string& strPar) = 0;
};

/**
 * 协议解析_Check，检测是否使用过
 */
class ProtocolParse_Check : public XMLParser, public ICheckParUsed {
public:
	ProtocolParse_Check();
	virtual ~ProtocolParse_Check();

public:
	virtual bool Call_CheckParUsed(const string& strPar);

public:
	bool    Call_CheckParUsed_My(const string& strPar);
	bool    Call_CheckParUsed_InNameSapce(const string& strPar, const stNamespace& pNamespace);
	//bool    Call_CheckParUsed_InSelfOtherStruct(const string& strPar, const stNamespace& pNamespace);


	void    StartBiaoZhuUsed(); //开始标注使用过

	bool    Call_BiaoZhuUsed_My(bool bFirst); //返回true表明产生了新的标注
	bool    Call_BiaoZhuUsed_My_InNameSpace(stNamespace& pNamespace);
	bool    Call_BiaoZhuUsed_My_InSelfOtherStruct_InNameSpace(stNamespace& pNamespace);
	bool    Call_BiaoZhuUsed_My_InSelfOtherStruct_InNameSpace2(const string& strPar, stNamespace& pNamespace);
	void    Call_BiaoZhuUsed_Direct(const string& strPar, stNamespace& pNamespace);


public:
	ICheckParUsed*  m_pCheckParUsed; //检测参数是否使用过的接口
};

/**
 * 共享
 */
class ProtocolParse_Share : public ProtocolParse_Check
{
public:
	ProtocolParse_Share();
	virtual ~ProtocolParse_Share();

public:
	string  CreateTableString(int tablenum); //制表用

	int     CovertStringToInt(const string& str, bool bNew = false);//字符串转换成int,取前24个字符，int的低8位为24个字母相同后，区分的标志
	int     CreateSerialVersionUID(const string& str);
	int     AddtoStringIntLogMap(int data);  //添加到字符串与int映射表

	int     GetExistStringToIntValue(const string& str); //得到已经存在的字符串转成int值

	

	void    CheckDirectoryEixst(CString strDir);

	void    GetRefProtocolMapPar(WJS_BBSTree<int, int>&  StringIntLogMap, WJS_BBSTree<int, string>&  IntStringMap, WJS_BBSTree<string, int>&  StringIntSearchMap);

public:
	bool            m_bUse4SpaceReplaceTable; //是否使用4个空格代替制表符

public:
	WJS_BBSTree<int, int>     m_StringIntLogMap;//存字符串转换成int记录
	WJS_BBSTree<int, string>  m_IntStringMap; //
	WJS_BBSTree<string, int>  m_StringIntSearchMap;
	string          m_strConflict;    //冲突

};

/**
 * 写文件
 */
class ProtocolParse_Writer : public ProtocolParse_Share {
public:
	ProtocolParse_Writer(const string& strPathName, eProtocolWriterType writerType);
	virtual ~ProtocolParse_Writer();

	void SetFileName(const string& strPathName); //设文件名
	void Free();          //释放
	bool Parser();        //进行解析
	
	void   FreeWriteLog();//清除写记录
	string GetShortName();

	/**
	 * 创建文件入口
	 */
	virtual bool    CreateFile_Path(string path, int filetype, bool brefprotocol) = 0;

	/**
	 * 创建引用协议的Writer
	 */
	virtual ProtocolParse_Writer* Create_RefProtocolParse_Writer(const string& strPathName) = 0;

	/**
	* 创建引用协议的Writer入口
	*/
	ProtocolParse_Writer* Create_RefProtocolParse_Writer_Enter(const string& strPathName,bool bNotCreateFile);

#pragma region 测试区
public:
	virtual void   TestBaseParIsConstRef(stBasePar& pPar) override; //测试是否是constref,加判引用协议
	virtual void   RefProtocolProcess() override; //引用协议处理
#pragma endregion

#pragma region 类型判定
public:
	virtual bool IsValidType(const string& strtypename) override;  //是否是有效的类型
	virtual bool IsExType_enum(const string& strtypename) override; //是否是扩展的枚举类型
	virtual bool IsExType_struct(const string& strtypename) override;//是否是扩展类型,如结构体
	virtual bool IsExType_vector(const string& strtypename) override;//是否是扩展类型, vector
	virtual bool IsExType_WJSVector(const string& strtypename) override; //是否是另外版本的类vector型
#pragma endregion

#pragma region 类型判定扩展
	string GetExTypeParT_vector(const string& strtypename);
	string GetExTypeParT_WJSVector(const string& strtypename);
	bool IsExType_baseVector(const string& strtypename, int filetype, string& strNameSpace); //是否是vector ,且是基本类型，并返回命名空间
	int  IsExType_baseVector_DiGui(const string& strtypename, int filetype, string& strNameSpace); //是否是vector ,且是基本类型，并返回命名空间,-1表不是，1表是，0表不能确认
#pragma endregion 

	virtual void    WriteInfoHead(FILE* fp, bool bLua = false, bool bPython = false);//写头
	virtual void    WriteStartZhongKuoKao(FILE* fp);  //写{}
	virtual void    WriteEndZhongKuoKao(FILE* fp);

	string  GetDefaultValue(const string& str, bool bFlash = false, bool bJava = false, bool bCSharp = false, bool bTScript = false, bool bPython = false); //根据类型得到默认值，只用于基本类型
	

#pragma region 包名
	bool    IsFlashBaseType(const string& strTypeName, bool bWrite);
	void    GetUsePacketNamesByParType(const string& strParType, vector<string>& packetNamesList);
	void    AddPacketNames(vector<string>& packetNamesList, const string& strNewPacketName);
	void    RemovePacketNames(vector<string>& packetNamesList, const string& strRemovePacketName, bool bOnlyOne = true);
	void    GetUsePacketNamesByRefHead(vector<string>& packetNamesList, bool bXiaoXie);
#pragma endregion

	
public:
	vector<ProtocolParse_Writer*>           m_pRefProtocolParseList;//引用的解析
public:
	string		    m_strFileName;//文件名
	bool            m_bStdafx;        //是否包含stdafx
	string          m_strXMLMd5;      //要解析文件的MD5
	bool            m_bNotSaveToFile; //是否不存成文件
	bool            m_bOutJson;       //输出Json
	int             m_tablenum;       //制表符数
	bool            m_bWriteSortFuncId; //写函数ID是否排序
	bool            m_bSCSpliteS;       //SC模式分离S
	bool            m_bSCSpliteC;       //SC模式分离C
	bool            m_bFastModel;       //是否是快速模式
	bool            m_bPureVirtual;     //是否纯虚
	string          m_strJavaPackage;
	bool            m_bTScriptUseNameSpaceVersion; //是否使用空间版生成 NSV

	eProtocolWriterType  m_writerType;   //类型

	
};


//协议解析
class ProtocolParse :public ProtocolParse_Writer {
public:
	ProtocolParse(const string& strPathName);
	virtual ~ProtocolParse();

	/**
	* 创建文件入口
	*/
	virtual bool    CreateFile_Path(string path, int filetype, bool brefprotocol) override { return false; };
	/**
	* 创建引用协议的Writer
	*/
	virtual ProtocolParse_Writer* Create_RefProtocolParse_Writer(const string& strPathName) override { return nullptr; };

	virtual void   RefProtocolProcess() override; //引用协议处理

	virtual bool IsValidType(const string& strtypename) override;  //是否是有效的类型

	/**
	 * 写成XML文件
	 */
	void    WriterToXML(const string& saveFileName);

	/**
	 * 写XML文件
	 */
	void    WriterToXML_Namespace(TiXmlElement *pRoot,const stNamespace& rNamespace);
	void    WriterToXML_Enum(TiXmlElement* pRoot, const stEnum& rEnum);
	void    WriterToXML_StructVectorEnter(TiXmlElement* pRoot, const stNamespace& rNamespace,int index);
	void    WriterToXML_Struct(TiXmlElement* pRoot, const stStructStruct& rStruct);
	void    WriterToXML_Vector(TiXmlElement* pRoot,const stVector& rVector);
	void    WriterToXML_WJSVector(TiXmlElement* pRoot, const stWJSVector& rWJSVector);

	void    WriterToXML_ClassPart(TiXmlElement* pRoot,const stClass& rClass,const string& partName);
	void    WriterToXML_Func(TiXmlElement* pClass,const stFunc& rFunc,bool bFuncAllUseNew);
	void    WriterToXML_FuncPars(TiXmlElement* pFunc, const stFunc& rFunc);

	void    WriterXMLComment(TiXmlElement* pElement,const string& comment);

public:
	/**
	 * 比较协议文件,算出增加对象，增加参数，丢失对象
	 */
	void     CompareProtocol(ProtocolParse* parserB);



public:
	bool            m_bOnlyParseSelf; //是否只解析自己
	std::function<bool()> m_func_IsSkipFuncPars; //是否跳过函数参,有时为他更好的比对，只写函数名
};
