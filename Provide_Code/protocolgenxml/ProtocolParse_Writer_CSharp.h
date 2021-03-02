#pragma once
#include "ProtocolParse.h"

class ProtocolParse_Writer_CSharp : public ProtocolParse_Writer {
public:
	ProtocolParse_Writer_CSharp(const string& strPathName);
	virtual ~ProtocolParse_Writer_CSharp();
	/**
	* �����ļ����
	*/
	virtual bool    CreateFile_Path(string path, int filetype, bool brefprotocol) override;
	/**
	* ��������Э���Writer
	*/
	virtual ProtocolParse_Writer* Create_RefProtocolParse_Writer(const string& strPathName) override;
	////////////////////////CSharp��
public:
	bool    CreateFile_Path_CSharp(string path, int filetype, bool brefprotocol);//�Ƿ��Ƿ��������Ϊ0�ͻ��ˣ�1��������2�ϳɵ�filetype
	bool    CreateFile_CSharp(string foldername, int filetype, bool bRefProtocol); //���ɶ���ļ�,��������ļ�����
	bool    WriteOneNameSpace_CSharp(stNamespace& pNamespace, int filetype, string foldername); //д���������ռ�,�ֳ�.h��.m������
	void    CreateAndWriteFuncIdAndProtocolNo_CSharp(stNamespace& pNamespace, string foldername, const string& strWriteNameSpace);
	void    WriteEnum_CSharp(stNamespace& pNamespace, stEnum& pEnum, string foldername, const string& strWriteNameSpace);//дö��tablenum�����Ʊ����
	void    WriteStruct_CSharp(stNamespace& pNamespace, stStructStruct& pStruct, string foldername, const string& strWriteNameSpace);//д�ṹ��
	void    WriteVector_CSharp(stNamespace& pNamespace, stVector& pVector, string foldername, const string& strWriteNameSpace);  //дvector
	void    WriteVector2_CSharp(stNamespace& pNamespace, stVector& pVector, string foldername, const string& strWriteNameSpace);  //дvector
	void    WriteWJSVector_CSharp(stNamespace& pNamespace, stWJSVector& pWJSVector, string foldername, const string& strWriteNameSpace);  //дvector
	void    WriteWJSVector2_CSharp(stNamespace& pNamespace, stWJSVector& pWJSVector, string foldername, const string& strWriteNameSpace);  //дvector
	void    WriteClass_CSharp(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace);
	void    WriteClass_IReceiver_CSharp(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, bool bemptyimplement, const string& strWriteNameSpace);//���սӿ�
	void    WriteFunc_IReceiver_CSharp(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp, bool bemptyimplement); //
	void    WriteClass_Receiver_CSharp(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace);
	void    WriteFunc_ReceiverParser_CSharp(stClass& pClass, int filetype, int tablenum, stNamespace& pNamespace, string foldername, FILE* fp);
	void    WriteFunc_Receiver_CSharp(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp);
	void    WriteClass_Send_CSharp(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace);//����
	void    WriteFunc_SendByteSeq_CSharp(stFunc& pFunc, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp);//������
	
	void    GetUsePacketNamesByRefHead_CSharp(vector<string>& packetNamesList);


	string  WriteComment_CSharp(const string& strTable, const string& strComment, const string& strPar, bool bWithSummary = true);

#pragma region ����ת�����ж�
	bool    IsCSharpBaseType(const string& strTypeName, bool bWrite);
	bool    IsCSharpBaseType2(string strTypeName);
	string  TypeConvertToCSharpType(const string& strTypeName);
	string  TypeConvertToReadCSharpType(const string& strTypeName);
	string  TypeConvertToWriteCSharpType(const string& strTypeName);

#pragma endregion
public:
	bool            m_bWithLog;
};