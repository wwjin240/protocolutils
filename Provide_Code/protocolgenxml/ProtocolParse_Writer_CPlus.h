#pragma once
#include "ProtocolParse.h"


/**
* д�ļ�C++��
*/
class ProtocolParse_Writer_CPlus : public ProtocolParse_Writer
{
public:
	ProtocolParse_Writer_CPlus(const string& strPathName, eCPlusStandardType cplusStandardType,bool bUseStdByte);
	virtual ~ProtocolParse_Writer_CPlus();
public:
	virtual void    WriteInfoHead(FILE* fp, bool bLua = false, bool bPython = false) override;//дͷ
	virtual void    WriteStartZhongKuoKao(FILE* fp) override;  //д{}
	virtual void    WriteEndZhongKuoKao(FILE* fp) override;

public:
	/**
	* �����ļ����
	*/
	virtual bool    CreateFile_Path(string path, int filetype, bool brefprotocol) override;
	/**
	* ��������Э���Writer
	*/
	virtual ProtocolParse_Writer* Create_RefProtocolParse_Writer(const string& strPathName) override;
	////////////////////����CPlus�ļ�
public:
	bool    CreateFile_Path_CPlus(string path, int filetype, bool brefprotocol);//�Ƿ��Ƿ��������Ϊ0�ͻ��ˣ�1��������2�ϳɵ�filetype
	bool    CreateFile_CPlus(string filename, int filetype, bool bRefProtocol);
	void    WriteFileHead_CPlus(FILE* fp, int filetype);//д�ļ�ͷ
	bool    WriteOneNameSpace_CPlus(stNamespace& pNamespace, FILE* fp, int filetype, int HCPPType = 0); //д���������ռ�,HCCPType 0��ԭ���ĵ��ļ���ʽ��1��H��2��CPP
	void    CreateAndWriteFuncId_CPlus(stNamespace& pNamespace, FILE* fp, int tablenum);  //д������id
	void    WriteProtocolNo_CPlus(stNamespace& pNamespace, FILE* fp, int tablenum); //дЭ���
	void    WriteEnum_CPlus(stEnum& pEnum, FILE* fp, int tablenum);//дö��tablenum�����Ʊ����

	void    WriteStruct_CPlus(const string& rNameSpace,stStructStruct& pStruct, FILE* fp, int tablenum, int HCPPType = 0);//д�ṹ��
	void    WriteStruct_OutStream_CPlus(const string& rNameSpace, stStructStruct& pStruct, FILE* fp, int tablenum, bool bOutStream2, int HCPPType = 0);//д�ṹ��
	void    WriteStruct_JsonStream_CPlus(const string& rNameSpace, stStructStruct& pStruct, FILE* fp, int tablenum, int HCPPType = 0);
	string  WriteJsonStream_Array(int tablenum, const string& strParName, const string& strParType, const string& strMember, bool bCreateArrayPointer);
	string  WriteJsonStream_Array0ToObject(int tablenum, const string& strParName, const string& strParType, const string& strMember);

	void    WriteVector_JsonStream_CPlus(stVector& pVector, FILE* fp, int tablenum, int HCPPType = 0);  //дvector
	void    WriteWJSVector_JsonStream_CPlus(stWJSVector& pVector, FILE* fp, int tablenum, int HCPPType = 0);  //дvector
	void    WriteVector_CPlus(stVector& pVector, FILE* fp, int tablenum, int HCPPType = 0);  //дvector
	void    WriteWJSVector_CPlus(stWJSVector& pWJSVector, FILE* fp, int tablenum, int HCPPType = 0);//д��vector

	void    WriteClass_CPlus(stClass& pClass, FILE* fp, int tablenum, int filetype, const string& spacename, stNamespace& pNamespace, int HCPPType = 0);

	void    WriteClass_IReceiver_CPlus(stClass& pClass, FILE* fp, int tablenum, int filetype, const string& spacename, bool bemptyimplement = false);//���սӿ�
	void    WriteFunc_IReceiver_CPlus(stFunc& pFunc, FILE* fp, int tablenum, int filetype, bool bemptyimplement = false);//д���麯��,����

	void    WriteClass_Receiver_CPlus(stClass& pClass, FILE* fp, int tablenum, int filetype, const string& spacename, const string& strcallbackclassname, stNamespace& pNamespace, int HCPPType = 0);//������
	void    WriteFunc_Receiver_CPlus(stFunc& pFunc, FILE* fp, int tablenum, int filetype, const string& spacename, stNamespace& pNamespace, int HCPPType = 0);//���պ���
	void    WriteFunc_ReceiverParser_CPlus(stClass& pClass, FILE* fp, int tablenum, int filetype, const string& spacename, const string& strcallbackclassname, stNamespace& pNamespace, int HCPPType = 0);//���ս���

	void    WriteClass_Send_CPlus(stClass& pClass, FILE* fp, int tablenum, int filetype, const string& spacename, stNamespace& pNamespace);//����
	void    WriteFunc_SendByteSeq_CPlus(stFunc& pFunc, FILE* fp, int tablenum, int filetype, string classname, stNamespace& pNamespace);//������
	void    WriteFunc_SendByteSeqV2_CPlus(stFunc& pFunc, FILE* fp, int tablenum, int filetype, string classname, stNamespace& pNamespace);//������,�ڶ��棬����һ������������ٸ��ƹ���,������ı�������Ҫ���

#pragma region ע��
	/**
	* �ɰ��//ע��
	*/
	string  WriteComment_CPlus(const string& strTable, const string& strComment, const string& strPar);
	/**
	* д��ע�͵Ŀ�ʼ
	*/
	string  WriteBlockCommentBegin_CPlus(const string& strTable);
	/**
	* д��ע�͵Ľ���
	*/
	string  WriteBlockCommentEnd_CPlus(const string& strTable);
	/**
	* д�����ע��
	*/
	string  WriteBlockCommentPar_CPlus(const string& strTable, const string& strComment, const string& strPar);
	/**
	* д��briefע��
	*/
	string  WriteBlockCommentBrief_CPlus(const string& strTable, const string& strComment);

#pragma endregion

public:
	/**
	 * @brief �õ��������͵���������,��Ҫ�ǲ�std�����ռ�
	 * @param memberType 
	 * @return 
	*/
	string  GetMemberTypeFullType_CPlus(const string& memberType);

	/**
	 * @brief �Ƿ��ǿɿ��ٴ����vector����
	 * @param strtypename 
	 * @return 
	*/
	bool    IsCanFastProcessVectorType_CPlus(const string& strtypename); 

	/**
	 * @brief ������ת�����ַ���
	 * @param pPar 
	 * @return 
	*/
	string  CovertParToString_CPlus(stBasePar& pPar);
protected:
	std::map<string, string>  _memberTypeFullTypeMap;      //�������͵���������

	std::set<string>          _canFastProcessVectorTypeSet; //�ɿ���ȡ���vector���ͼ�����ֱ��memcopy


	eCPlusStandardType        _cplusStandardType; //C++��׼����

	bool                      _bUseStdByte;   //�Ƿ�ʹ��std::byte,c++11��׼֮��Ĭ��Ϊʹ��
};
