#pragma once
#include "ProtocolParse.h"

class ProtocolParse_Writer_Flash : public ProtocolParse_Writer {
public:
	ProtocolParse_Writer_Flash(const string& strPathName);
	virtual ~ProtocolParse_Writer_Flash();
	/**
	* �����ļ����
	*/
	virtual bool    CreateFile_Path(string path, int filetype, bool brefprotocol) override;
	/**
	* ��������Э���Writer
	*/
	virtual ProtocolParse_Writer* Create_RefProtocolParse_Writer(const string& strPathName) override;
	////////////////////////Flash��
public:
	bool    CreateFile_Path_Flash(string path, int filetype, bool brefprotocol);//�Ƿ��Ƿ��������Ϊ0�ͻ��ˣ�1��������2�ϳɵ�filetype
	bool    CreateFile_Flash(string foldername, int filetype, bool bRefProtocol); //���ɶ���ļ�,��������ļ�����
	bool    WriteOneNameSpace_Flash(stNamespace& pNamespace, int filetype, string foldername); //д���������ռ�,�ֳ�.h��.m������
	void    WriteEnum_Flash(stNamespace& pNamespace, stEnum& pEnum, string foldername);//дö��tablenum�����Ʊ����
	void    WriteStruct_Flash(stNamespace& pNamespace, stStructStruct& pStruct, string foldername);//д�ṹ��
	void    WriteVector_Flash(stNamespace& pNamespace, stVector& pVector, string foldername);  //дvector
	void    WriteVector2_Flash(stNamespace& pNamespace, stVector& pVector, string foldername);  //дvector
	void    WriteWJSVector_Flash(stNamespace& pNamespace, stWJSVector& pWJSVector, string foldername);//д��vector
	void    WriteWJSVector2_Flash(stNamespace& pNamespace, stWJSVector& pWJSVector, string foldername);//д��vector
	void    CreateAndWriteFuncIdAndProtocolNo_Flash(stNamespace& pNamespace, string foldername);
	void    WriteClass_Flash(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername);
	void    WriteClass_Send_Flash(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername);//����
	void    WriteFunc_SendByteSeq_Flash(stFunc& pFunc, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp);//������
	void    WriteClass_IReceiver_Flash(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, bool bemptyimplement = false);//���սӿ�
	void    WriteFunc_IReceiver_Flash(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp, bool bemptyimplement = false); //
	void    WriteFunc_IReceiver_Flash2(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp, bool bemptyimplement = false); //
	void    WriteClass_Receiver_Flash(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername);
	void    WriteFunc_Receiver_Flash(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp);
	void    WriteFunc_ReceiverParser_Flash(stClass& pClass, int filetype, int tablenum, stNamespace& pNamespace, string foldername, FILE* fp);
	

	string  WriteComment_Flash(const string& strTable, const string& strComment, bool bAsParam, const string& strPar, bool bWithSummary = true);

#pragma region ����ת�����ж�
	bool    IsFlashBaseType2(string strTypeName);//�Ƿ���Flash��������
	string  TypeConvertToFlashType(const string& strTypeName);
	string  TypeConvertToReadFlashType(const string& strTypeName);
	string  TypeConvertToWriteFlashType(const string& strTypeName);

#pragma endregion

public:
	string			m_strFlashPackage;   //flash�汾�İ�ǰ׺
	string          m_strFlashToolPackage;
};
