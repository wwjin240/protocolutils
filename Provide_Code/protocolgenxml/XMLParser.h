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
 * XML����
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
	bool ParserNamespace(TiXmlNode* pNode, TiXmlElement* pElement); //���������ռ�
	bool ParserProtocolNo(TiXmlNode* pNode, TiXmlElement* pElement); //����Э���
	bool ParserProtocolRef(TiXmlNode* pNode, TiXmlElement* pElement); //����Э������
	bool ParserVersionManger(TiXmlNode* pNode, TiXmlElement* pElement); //����Э��汾��Ϣ
	bool ParserPartManager(TiXmlNode* pNode, TiXmlElement* pElement);  //�������Ĺ���

public:
	bool ParserEnum(TiXmlNode* pNode, TiXmlElement* pElement); //����ö��

public:
	bool ParserStruct(TiXmlNode* pNode, TiXmlElement* pElement); //�����ṹ��
	bool IsAllAttributeRecongnize_Struct(TiXmlElement* pElement, const string& structName);
	bool IsAllAttributeRecongnize_StructPar(TiXmlElement* pElement, const string& structName, const string& parName);

public:
	bool ParserVector(TiXmlNode* pNode, TiXmlElement* pElement); //����vector
	bool ParserWJSVector(TiXmlNode* pNode, TiXmlElement* pElement);//����WJSByteSeq

public:
	bool ParserClass(TiXmlNode* pNode, TiXmlElement* pElement, bool brequest); //������
	bool ParserClassMethod(TiXmlNode* pNode, TiXmlElement* pElement, stClass& rClass, const string& part); //��������ķ���
	bool ParserMethod(TiXmlNode* pNode, TiXmlElement* pElement, stFunc& rFunc);//��������
	bool IsAllAttributeRecongnize_Class(TiXmlElement* pElement, const string& className);
	bool IsAllAttributeRecongnize_Func(TiXmlElement* pElement, const string& funcName);
	bool IsAllAttributeRecongnize_FuncPar(TiXmlElement* pElement, const string& funcName, const string& parName);

#pragma region �����ж�
public:
	virtual bool IsValidType(const string& strtypename);  //�Ƿ�����Ч������
	bool IsBaseType(const string& strtypename); //ȷ���Ƿ��ǻ�������
	bool IsBaseType_WriterType(const string& strTypeName, eProtocolWriterType writerType);//ȷ���Ƿ��ǻ�������,writerType��
	virtual bool IsExType_enum(const string& strtypename); //�Ƿ�����չ��ö������
	virtual bool IsExType_struct(const string& strtypename);//�Ƿ�����չ����,��ṹ��
	virtual bool IsExType_vector(const string& strtypename);//�Ƿ�����չ����, vector
	virtual bool IsExType_WJSVector(const string& strtypename); //�Ƿ�������汾����vector��

#pragma endregion

#pragma region ������
public:
	bool    TestPartIsOk(const string& strPart); //�������Ƿ�OK
	void    TestChangeInt64ToLongLong(string& strOldType); //int64ת��longlong
	virtual void   TestBaseParIsConstRef(stBasePar& pPar); //�����Ƿ���constref,���ﲻ�����õ�Э���
	virtual void   RefProtocolProcess() {}; //����Э�鴦��
#pragma endregion

#pragma region �����ͺ���
	void   ToLower(string &str);  //ת��Сд
	string ToLowerV2(string &str);
	string         GetUTF8(LPCTSTR lpInput);
	void           UTF8Encode(LPCTSTR lpInput, std::string &strOutput);
	void           UTF8Decode(const char *pInput, string &strOutput);

	int    GetBaseTypeNum(); //�õ�����������

	void   AddToPartManage(const string& partManageName,const string& partName,const string& comment); //�ӵ�������

	void   AddToYuanPartList(const string& partName); //�ӵ�ԭpartList

	string  GetBaseTypeDefault(const std::string& typeName);


#pragma endregion 

public:
	stProtocolNo              m_ProtocolNo; //Э���
	vector<stRefProtocol>     m_RefProtocolList;//����Э��
	stSaveFileName            m_SaveFileName; //�洢��
	vector<stNamespace>       m_NamespaceVec; //�����ռ�
	map<string, set<string>>  m_PartManageMap; //���Ĺ���
	vector<stPartManager>     m_PartManageList;//˳����������
	stVersionManger           m_VersionManger; //�汾����
	string                    m_shortName;     //����

	vector<string>            m_YuanPartList;  //ԭ����partlist

public:
	int             m_nStructVecCount; //���ǵ��ṹ���vector��˳�����⣬�Ӵ˱���

public:
	bool            m_bHCPP;               //����H��CPP�����ļ�
	bool            m_bUsePartProtocol;    //�Ƿ�ʹ������Э�� 
	bool            m_bExceptMoba;         //�Ƿ��ų�Moba
	string          m_UsePartManager;      //ʹ���ĸ�������
	bool            m_bAndroidPeer;       //�Ƿ��ǰ�׿��

public:
	string          m_strErrorReason;//����ԭ��

	std::map<std::string, std::string>  m_baseTypeDefaultMap;  //�������͵�Ĭ��ֵ



public:
#ifdef _DEBUG
	FileLogger      _Logger;       //�ռ�
#endif
};