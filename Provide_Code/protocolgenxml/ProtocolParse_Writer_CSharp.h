#pragma once
#include "ProtocolParse.h"

class ProtocolParse_Writer_CSharp : public ProtocolParse_Writer {
public:
	ProtocolParse_Writer_CSharp(const string& strPathName);
	virtual ~ProtocolParse_Writer_CSharp();
	/**
	* 创建文件入口
	*/
	virtual bool    CreateFile_Path(string path, int filetype, bool brefprotocol) override;
	/**
	* 创建引用协议的Writer
	*/
	virtual ProtocolParse_Writer* Create_RefProtocolParse_Writer(const string& strPathName) override;
	////////////////////////CSharp版
public:
	bool    CreateFile_Path_CSharp(string path, int filetype, bool brefprotocol);//是否是服务器版改为0客户端，1服务器，2合成的filetype
	bool    CreateFile_CSharp(string foldername, int filetype, bool bRefProtocol); //生成多个文件,传入的是文件夹名
	bool    WriteOneNameSpace_CSharp(stNamespace& pNamespace, int filetype, string foldername); //写整个命名空间,分成.h和.m两部分
	void    CreateAndWriteFuncIdAndProtocolNo_CSharp(stNamespace& pNamespace, string foldername, const string& strWriteNameSpace);
	void    WriteEnum_CSharp(stNamespace& pNamespace, stEnum& pEnum, string foldername, const string& strWriteNameSpace);//写枚举tablenum代数制表符数
	void    WriteStruct_CSharp(stNamespace& pNamespace, stStructStruct& pStruct, string foldername, const string& strWriteNameSpace);//写结构体
	void    WriteVector_CSharp(stNamespace& pNamespace, stVector& pVector, string foldername, const string& strWriteNameSpace);  //写vector
	void    WriteVector2_CSharp(stNamespace& pNamespace, stVector& pVector, string foldername, const string& strWriteNameSpace);  //写vector
	void    WriteWJSVector_CSharp(stNamespace& pNamespace, stWJSVector& pWJSVector, string foldername, const string& strWriteNameSpace);  //写vector
	void    WriteWJSVector2_CSharp(stNamespace& pNamespace, stWJSVector& pWJSVector, string foldername, const string& strWriteNameSpace);  //写vector
	void    WriteClass_CSharp(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace);
	void    WriteClass_IReceiver_CSharp(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, bool bemptyimplement, const string& strWriteNameSpace);//接收接口
	void    WriteFunc_IReceiver_CSharp(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp, bool bemptyimplement); //
	void    WriteClass_Receiver_CSharp(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace);
	void    WriteFunc_ReceiverParser_CSharp(stClass& pClass, int filetype, int tablenum, stNamespace& pNamespace, string foldername, FILE* fp);
	void    WriteFunc_Receiver_CSharp(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp);
	void    WriteClass_Send_CSharp(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace);//发送
	void    WriteFunc_SendByteSeq_CSharp(stFunc& pFunc, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp);//发送流
	
	void    GetUsePacketNamesByRefHead_CSharp(vector<string>& packetNamesList);


	string  WriteComment_CSharp(const string& strTable, const string& strComment, const string& strPar, bool bWithSummary = true);

#pragma region 类型转换与判断
	bool    IsCSharpBaseType(const string& strTypeName, bool bWrite);
	bool    IsCSharpBaseType2(string strTypeName);
	string  TypeConvertToCSharpType(const string& strTypeName);
	string  TypeConvertToReadCSharpType(const string& strTypeName);
	string  TypeConvertToWriteCSharpType(const string& strTypeName);

#pragma endregion
public:
	bool            m_bWithLog;
};