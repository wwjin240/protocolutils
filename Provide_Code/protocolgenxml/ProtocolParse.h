/*
* create by  wwjin
*/

//Э�������
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

//���ĳ�������Ƿ�ʹ�ù�
class ICheckParUsed {
public:
	virtual bool Call_CheckParUsed(const string& strPar) = 0;
};

/**
 * Э�����_Check������Ƿ�ʹ�ù�
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


	void    StartBiaoZhuUsed(); //��ʼ��עʹ�ù�

	bool    Call_BiaoZhuUsed_My(bool bFirst); //����true�����������µı�ע
	bool    Call_BiaoZhuUsed_My_InNameSpace(stNamespace& pNamespace);
	bool    Call_BiaoZhuUsed_My_InSelfOtherStruct_InNameSpace(stNamespace& pNamespace);
	bool    Call_BiaoZhuUsed_My_InSelfOtherStruct_InNameSpace2(const string& strPar, stNamespace& pNamespace);
	void    Call_BiaoZhuUsed_Direct(const string& strPar, stNamespace& pNamespace);


public:
	ICheckParUsed*  m_pCheckParUsed; //�������Ƿ�ʹ�ù��Ľӿ�
};

/**
 * ����
 */
class ProtocolParse_Share : public ProtocolParse_Check
{
public:
	ProtocolParse_Share();
	virtual ~ProtocolParse_Share();

public:
	string  CreateTableString(int tablenum); //�Ʊ���

	int     CovertStringToInt(const string& str, bool bNew = false);//�ַ���ת����int,ȡǰ24���ַ���int�ĵ�8λΪ24����ĸ��ͬ�����ֵı�־
	int     CreateSerialVersionUID(const string& str);
	int     AddtoStringIntLogMap(int data);  //��ӵ��ַ�����intӳ���

	int     GetExistStringToIntValue(const string& str); //�õ��Ѿ����ڵ��ַ���ת��intֵ

	

	void    CheckDirectoryEixst(CString strDir);

	void    GetRefProtocolMapPar(WJS_BBSTree<int, int>&  StringIntLogMap, WJS_BBSTree<int, string>&  IntStringMap, WJS_BBSTree<string, int>&  StringIntSearchMap);

public:
	bool            m_bUse4SpaceReplaceTable; //�Ƿ�ʹ��4���ո�����Ʊ��

public:
	WJS_BBSTree<int, int>     m_StringIntLogMap;//���ַ���ת����int��¼
	WJS_BBSTree<int, string>  m_IntStringMap; //
	WJS_BBSTree<string, int>  m_StringIntSearchMap;
	string          m_strConflict;    //��ͻ

};

/**
 * д�ļ�
 */
class ProtocolParse_Writer : public ProtocolParse_Share {
public:
	ProtocolParse_Writer(const string& strPathName, eProtocolWriterType writerType);
	virtual ~ProtocolParse_Writer();

	void SetFileName(const string& strPathName); //���ļ���
	void Free();          //�ͷ�
	bool Parser();        //���н���
	
	void   FreeWriteLog();//���д��¼
	string GetShortName();

	/**
	 * �����ļ����
	 */
	virtual bool    CreateFile_Path(string path, int filetype, bool brefprotocol) = 0;

	/**
	 * ��������Э���Writer
	 */
	virtual ProtocolParse_Writer* Create_RefProtocolParse_Writer(const string& strPathName) = 0;

	/**
	* ��������Э���Writer���
	*/
	ProtocolParse_Writer* Create_RefProtocolParse_Writer_Enter(const string& strPathName,bool bNotCreateFile);

#pragma region ������
public:
	virtual void   TestBaseParIsConstRef(stBasePar& pPar) override; //�����Ƿ���constref,��������Э��
	virtual void   RefProtocolProcess() override; //����Э�鴦��
#pragma endregion

#pragma region �����ж�
public:
	virtual bool IsValidType(const string& strtypename) override;  //�Ƿ�����Ч������
	virtual bool IsExType_enum(const string& strtypename) override; //�Ƿ�����չ��ö������
	virtual bool IsExType_struct(const string& strtypename) override;//�Ƿ�����չ����,��ṹ��
	virtual bool IsExType_vector(const string& strtypename) override;//�Ƿ�����չ����, vector
	virtual bool IsExType_WJSVector(const string& strtypename) override; //�Ƿ�������汾����vector��
#pragma endregion

#pragma region �����ж���չ
	string GetExTypeParT_vector(const string& strtypename);
	string GetExTypeParT_WJSVector(const string& strtypename);
	bool IsExType_baseVector(const string& strtypename, int filetype, string& strNameSpace); //�Ƿ���vector ,���ǻ������ͣ������������ռ�
	int  IsExType_baseVector_DiGui(const string& strtypename, int filetype, string& strNameSpace); //�Ƿ���vector ,���ǻ������ͣ������������ռ�,-1���ǣ�1���ǣ�0����ȷ��
#pragma endregion 

	virtual void    WriteInfoHead(FILE* fp, bool bLua = false, bool bPython = false);//дͷ
	virtual void    WriteStartZhongKuoKao(FILE* fp);  //д{}
	virtual void    WriteEndZhongKuoKao(FILE* fp);

	string  GetDefaultValue(const string& str, bool bFlash = false, bool bJava = false, bool bCSharp = false, bool bTScript = false, bool bPython = false); //�������͵õ�Ĭ��ֵ��ֻ���ڻ�������
	

#pragma region ����
	bool    IsFlashBaseType(const string& strTypeName, bool bWrite);
	void    GetUsePacketNamesByParType(const string& strParType, vector<string>& packetNamesList);
	void    AddPacketNames(vector<string>& packetNamesList, const string& strNewPacketName);
	void    RemovePacketNames(vector<string>& packetNamesList, const string& strRemovePacketName, bool bOnlyOne = true);
	void    GetUsePacketNamesByRefHead(vector<string>& packetNamesList, bool bXiaoXie);
#pragma endregion

	
public:
	vector<ProtocolParse_Writer*>           m_pRefProtocolParseList;//���õĽ���
public:
	string		    m_strFileName;//�ļ���
	bool            m_bStdafx;        //�Ƿ����stdafx
	string          m_strXMLMd5;      //Ҫ�����ļ���MD5
	bool            m_bNotSaveToFile; //�Ƿ񲻴���ļ�
	bool            m_bOutJson;       //���Json
	int             m_tablenum;       //�Ʊ����
	bool            m_bWriteSortFuncId; //д����ID�Ƿ�����
	bool            m_bSCSpliteS;       //SCģʽ����S
	bool            m_bSCSpliteC;       //SCģʽ����C
	bool            m_bFastModel;       //�Ƿ��ǿ���ģʽ
	bool            m_bPureVirtual;     //�Ƿ���
	string          m_strJavaPackage;
	bool            m_bTScriptUseNameSpaceVersion; //�Ƿ�ʹ�ÿռ������ NSV

	eProtocolWriterType  m_writerType;   //����

	
};


//Э�����
class ProtocolParse :public ProtocolParse_Writer {
public:
	ProtocolParse(const string& strPathName);
	virtual ~ProtocolParse();

	/**
	* �����ļ����
	*/
	virtual bool    CreateFile_Path(string path, int filetype, bool brefprotocol) override { return false; };
	/**
	* ��������Э���Writer
	*/
	virtual ProtocolParse_Writer* Create_RefProtocolParse_Writer(const string& strPathName) override { return nullptr; };

	virtual void   RefProtocolProcess() override; //����Э�鴦��

	virtual bool IsValidType(const string& strtypename) override;  //�Ƿ�����Ч������

	/**
	 * д��XML�ļ�
	 */
	void    WriterToXML(const string& saveFileName);

	/**
	 * дXML�ļ�
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
	 * �Ƚ�Э���ļ�,������Ӷ������Ӳ�������ʧ����
	 */
	void     CompareProtocol(ProtocolParse* parserB);



public:
	bool            m_bOnlyParseSelf; //�Ƿ�ֻ�����Լ�
	std::function<bool()> m_func_IsSkipFuncPars; //�Ƿ�����������,��ʱΪ�����õıȶԣ�ֻд������
};
