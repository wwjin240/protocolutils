#pragma once
#include "ProtocolParse.h"

class ProtocolParse_Writer_Python : public ProtocolParse_Writer
{
public:
	ProtocolParse_Writer_Python(const string& strPathName);
	virtual ~ProtocolParse_Writer_Python();
	/**
	* �����ļ����
	*/
	virtual bool    CreateFile_Path(string path, int filetype, bool brefprotocol) override;
	/**
	* ��������Э���Writer
	*/
	virtual ProtocolParse_Writer* Create_RefProtocolParse_Writer(const string& strPathName) override;
public:
	////////////////////////////Python
	bool    CreateFile_Path_Python(string path, int filetype, bool brefprotocol);//�Ƿ��Ƿ��������Ϊ0�ͻ��ˣ�1��������2�ϳɵ�filetype
	bool    CreateFile_Python(string foldername, int filetype, bool bRefProtocol); //���ɶ���ļ�,��������ļ�����
	bool    WriteOneNameSpace_Python(stNamespace& pNamespace, int filetype, string foldername); //д���������ռ�,�ֳ�.h��.m������
	void    CreateAndWriteFuncIdAndProtocolNo_Python(stNamespace& pNamespace, string foldername, const string& strWriteNameSpace);
	void    WriteEnum_Python(stNamespace& pNamespace, stEnum& pEnum, string foldername, const string& strWriteNameSpace);//дö��tablenum�����Ʊ����
	void    WriteStruct_Python(stNamespace& pNamespace, stStructStruct& pStruct, string foldername, const string& strWriteNameSpace);//д�ṹ��
	void    WriteVector_Python(stNamespace& pNamespace, stVector& pVector, string foldername, const string& strWriteNameSpace);  //дvector
	void    WriteVector2_Python(stNamespace& pNamespace, stVector& pVector, string foldername, const string& strWriteNameSpace);  //дvector
	void    WriteWJSVector_Python(stNamespace& pNamespace, stWJSVector& pWJSVector, string foldername, const string& strWriteNameSpace);  //дvector
	void    WriteClass_Python(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace);
	void    WriteClass_IReceiver_Python(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, bool bemptyimplement, const string& strWriteNameSpace);//���սӿ�
	void    WriteFunc_IReceiver_Python(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp, bool bemptyimplement); //
	void    WriteClass_Receiver_Python(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace);
	void    WriteFunc_ReceiverParser_Python(stClass& pClass, int filetype, int tablenum, stNamespace& pNamespace, string foldername, FILE* fp);
	void    WriteFunc_Receiver_Python(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp);
	void    WriteClass_Send_Python(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace);//����
	void    WriteFunc_SendByteSeq_Python(stFunc& pFunc, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp);//������


	void    GetUsePacketNamesByParType_Python(const string& strParType, vector<string>& packetNamesList);

	string  WriteComment_Python(const string& strTable, const string& strComment, const string& strPar, bool bInBlockComment = false);

#pragma region ����ת�����ж�
	bool    IsPythonBaseType(const string& strTypeName, bool bWrite);
	bool    IsPythonBaseType2(string strTypeName);
	string  TypeConvertToPythonType(const string& strTypeName);
	string  TypeConvertToReadPythonType(const string& strTypeName);
	string  TypeConvertToWritePythonType(const string& strTypeName);


#pragma endregion
};
