#pragma once
#include "ProtocolParse.h"

class ProtocolParse_Writer_Lua : public ProtocolParse_Writer {
public:
	ProtocolParse_Writer_Lua(const string& strPathName);
	virtual ~ProtocolParse_Writer_Lua();
	/**
	* �����ļ����
	*/
	virtual bool    CreateFile_Path(string path, int filetype, bool brefprotocol) override;
	/**
	* ��������Э���Writer
	*/
	virtual ProtocolParse_Writer* Create_RefProtocolParse_Writer(const string& strPathName) override;
	/////////////////////////LUA
public:
	bool    CreateFile_Path_Lua(string path, int filetype, bool brefprotocol);//�Ƿ��Ƿ��������Ϊ0�ͻ��ˣ�1��������2�ϳɵ�filetype
	bool    CreateFile_Lua(string foldername, int filetype, bool bRefProtocol); //���ɶ���ļ�,��������ļ�����
	bool    WriteOneNameSpace_Lua(stNamespace& pNamespace, int filetype, string foldername); //д���������ռ�,�ֳ�.h��.m������
	void    CreateAndWriteFuncIdAndProtocolNo_Lua(stNamespace& pNamespace, string foldername, const string& strWriteNameSpace);
	void    WriteEnum_Lua(stNamespace& pNamespace, stEnum& pEnum, string foldername, const string& strWriteNameSpace);//дö��tablenum�����Ʊ����
	void    WriteStruct_Lua(stNamespace& pNamespace, stStructStruct& pStruct, string foldername, const string& strWriteNameSpace);//д�ṹ��
	void    WriteVector_Lua(stNamespace& pNamespace, stVector& pVector, string foldername, const string& strWriteNameSpace);  //дvector
	void    WriteVector2_Lua(stNamespace& pNamespace, stVector& pVector, string foldername, const string& strWriteNameSpace);  //дvector
	void    WriteWJSVector_Lua(stNamespace& pNamespace, stWJSVector& pWJSVector, string foldername, const string& strWriteNameSpace);  //дvector
	void    WriteClass_Lua(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace);
	void    WriteClass_IReceiver_Lua(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, bool bemptyimplement, const string& strWriteNameSpace);//���սӿ�
	void    WriteFunc_IReceiver_Lua(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp, bool bemptyimplement); //
	void    WriteClass_Receiver_Lua(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace);
	void    WriteFunc_ReceiverParser_Lua(stClass& pClass, int filetype, int tablenum, stNamespace& pNamespace, string foldername, FILE* fp);
	void    WriteFunc_Receiver_Lua(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp);
	void    WriteClass_Send_Lua(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace);//����
	void    WriteFunc_SendByteSeq_Lua(stFunc& pFunc, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp);//������

	
	void    GetUsePacketNamesByParType_Lua(const string& strParType, vector<string>& packetNamesList, bool bOnlyOneLevel = false);


	string  WriteComment_Lua(const string& strTable, const string& strComment, const string& strPar, bool bWithSummary = true);
#pragma region ����ת�����ж�
	bool    IsLuaBaseType(const string& strTypeName, bool bWrite);
	bool    IsLuaBaseType2(string strTypeName);
	string  TypeConvertToLuaType(const string& strTypeName);
	string  TypeConvertToReadLuaType(const string& strTypeName);
	string  TypeConvertToWriteLuaType(const string& strTypeName);
#pragma endregion

public:
	string          m_strLuaPackage;  //lua����ǰ׺
	bool            m_bLuaRequireInFunc;

};
