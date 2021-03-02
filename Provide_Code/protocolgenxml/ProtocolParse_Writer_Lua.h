#pragma once
#include "ProtocolParse.h"

class ProtocolParse_Writer_Lua : public ProtocolParse_Writer {
public:
	ProtocolParse_Writer_Lua(const string& strPathName);
	virtual ~ProtocolParse_Writer_Lua();
	/**
	* 创建文件入口
	*/
	virtual bool    CreateFile_Path(string path, int filetype, bool brefprotocol) override;
	/**
	* 创建引用协议的Writer
	*/
	virtual ProtocolParse_Writer* Create_RefProtocolParse_Writer(const string& strPathName) override;
	/////////////////////////LUA
public:
	bool    CreateFile_Path_Lua(string path, int filetype, bool brefprotocol);//是否是服务器版改为0客户端，1服务器，2合成的filetype
	bool    CreateFile_Lua(string foldername, int filetype, bool bRefProtocol); //生成多个文件,传入的是文件夹名
	bool    WriteOneNameSpace_Lua(stNamespace& pNamespace, int filetype, string foldername); //写整个命名空间,分成.h和.m两部分
	void    CreateAndWriteFuncIdAndProtocolNo_Lua(stNamespace& pNamespace, string foldername, const string& strWriteNameSpace);
	void    WriteEnum_Lua(stNamespace& pNamespace, stEnum& pEnum, string foldername, const string& strWriteNameSpace);//写枚举tablenum代数制表符数
	void    WriteStruct_Lua(stNamespace& pNamespace, stStructStruct& pStruct, string foldername, const string& strWriteNameSpace);//写结构体
	void    WriteVector_Lua(stNamespace& pNamespace, stVector& pVector, string foldername, const string& strWriteNameSpace);  //写vector
	void    WriteVector2_Lua(stNamespace& pNamespace, stVector& pVector, string foldername, const string& strWriteNameSpace);  //写vector
	void    WriteWJSVector_Lua(stNamespace& pNamespace, stWJSVector& pWJSVector, string foldername, const string& strWriteNameSpace);  //写vector
	void    WriteClass_Lua(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace);
	void    WriteClass_IReceiver_Lua(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, bool bemptyimplement, const string& strWriteNameSpace);//接收接口
	void    WriteFunc_IReceiver_Lua(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp, bool bemptyimplement); //
	void    WriteClass_Receiver_Lua(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace);
	void    WriteFunc_ReceiverParser_Lua(stClass& pClass, int filetype, int tablenum, stNamespace& pNamespace, string foldername, FILE* fp);
	void    WriteFunc_Receiver_Lua(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp);
	void    WriteClass_Send_Lua(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace);//发送
	void    WriteFunc_SendByteSeq_Lua(stFunc& pFunc, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp);//发送流

	
	void    GetUsePacketNamesByParType_Lua(const string& strParType, vector<string>& packetNamesList, bool bOnlyOneLevel = false);


	string  WriteComment_Lua(const string& strTable, const string& strComment, const string& strPar, bool bWithSummary = true);
#pragma region 类型转换与判断
	bool    IsLuaBaseType(const string& strTypeName, bool bWrite);
	bool    IsLuaBaseType2(string strTypeName);
	string  TypeConvertToLuaType(const string& strTypeName);
	string  TypeConvertToReadLuaType(const string& strTypeName);
	string  TypeConvertToWriteLuaType(const string& strTypeName);
#pragma endregion

public:
	string          m_strLuaPackage;  //lua包的前缀
	bool            m_bLuaRequireInFunc;

};
