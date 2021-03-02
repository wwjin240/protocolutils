#pragma once
#include "ProtocolParse.h"

class ProtocolParse_Writer_Java : public ProtocolParse_Writer {
public:
	ProtocolParse_Writer_Java(const string& strPathName);
	virtual ~ProtocolParse_Writer_Java();
	/**
	* �����ļ����
	*/
	virtual bool    CreateFile_Path(string path, int filetype, bool brefprotocol) override;
	/**
	* ��������Э���Writer
	*/
	virtual ProtocolParse_Writer* Create_RefProtocolParse_Writer(const string& strPathName) override;
	////////////////////////Java��
public:
	bool    CreateFile_Path_Java(string path, int filetype, bool brefprotocol);//�Ƿ��Ƿ��������Ϊ0�ͻ��ˣ�1��������2�ϳɵ�filetype
	bool    CreateFile_Java(string foldername, int filetype, bool bRefProtocol); //���ɶ���ļ�,��������ļ�����
	bool    WriteOneNameSpace_Java(stNamespace& pNamespace, int filetype, string foldername); //д���������ռ�,�ֳ�.h��.m������

	void    CreateAndWriteFuncIdAndProtocolNo_Java(stNamespace& pNamespace, string foldername);
	void    WriteEnum_Java(stNamespace& pNamespace, stEnum& pEnum, string foldername);//дö��tablenum�����Ʊ����
	void    WriteStruct_Java(stNamespace& pNamespace, stStructStruct& pStruct, string foldername);//д�ṹ��
	void    WriteVector_Java(stNamespace& pNamespace, stVector& pVector, string foldername);  //дvector
	void    WriteClass_Java(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername);
	void    WriteClass_IReceiver_Java(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, bool bemptyimplement = false);//���սӿ�
	void    WriteFunc_IReceiver_Java(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp, bool bemptyimplement = false); //
	void    WriteClass_Receiver_Java(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername);
	void    WriteFunc_ReceiverParser_Java(stClass& pClass, int filetype, int tablenum, stNamespace& pNamespace, string foldername, FILE* fp);
	void    WriteFunc_Receiver_Java(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp);
	void    WriteClass_Send_Java(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername);//����
	void    WriteFunc_SendByteSeq_Java(stFunc& pFunc, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp);//������

	

	void    SetJavaPackName(const string& strJavaPackName);

	

public:
	void    GetUsePacketNamesByRefHead_Java(vector<string>& packetNamesList);
	void    GetUsePacketNamesByParType_Java(const string& strParType, vector<string>& packetNamesList);


	string  WriteComment_Java(const string& strTable, const string& strComment, const string& strPar);
#pragma region ����ת�����ж�
public:
	bool    IsJavaBaseType(const string& strTypeName, bool bWrite);
	bool    IsJavaBaseType2(string strTypeName);
	string  TypeConvertToJavaType(const string& strTypeName);
	string  TypeConvertToReadJavaType(const string& strTypeName);
	string  TypeConvertToWriteJavaType(const string& strTypeName);

#pragma endregion
};
