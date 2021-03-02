#pragma once
#include "ProtocolParse.h"

class ProtocolParse_Writer_Java : public ProtocolParse_Writer {
public:
	ProtocolParse_Writer_Java(const string& strPathName);
	virtual ~ProtocolParse_Writer_Java();
	/**
	* 创建文件入口
	*/
	virtual bool    CreateFile_Path(string path, int filetype, bool brefprotocol) override;
	/**
	* 创建引用协议的Writer
	*/
	virtual ProtocolParse_Writer* Create_RefProtocolParse_Writer(const string& strPathName) override;
	////////////////////////Java版
public:
	bool    CreateFile_Path_Java(string path, int filetype, bool brefprotocol);//是否是服务器版改为0客户端，1服务器，2合成的filetype
	bool    CreateFile_Java(string foldername, int filetype, bool bRefProtocol); //生成多个文件,传入的是文件夹名
	bool    WriteOneNameSpace_Java(stNamespace& pNamespace, int filetype, string foldername); //写整个命名空间,分成.h和.m两部分

	void    CreateAndWriteFuncIdAndProtocolNo_Java(stNamespace& pNamespace, string foldername);
	void    WriteEnum_Java(stNamespace& pNamespace, stEnum& pEnum, string foldername);//写枚举tablenum代数制表符数
	void    WriteStruct_Java(stNamespace& pNamespace, stStructStruct& pStruct, string foldername);//写结构体
	void    WriteVector_Java(stNamespace& pNamespace, stVector& pVector, string foldername);  //写vector
	void    WriteClass_Java(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername);
	void    WriteClass_IReceiver_Java(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, bool bemptyimplement = false);//接收接口
	void    WriteFunc_IReceiver_Java(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp, bool bemptyimplement = false); //
	void    WriteClass_Receiver_Java(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername);
	void    WriteFunc_ReceiverParser_Java(stClass& pClass, int filetype, int tablenum, stNamespace& pNamespace, string foldername, FILE* fp);
	void    WriteFunc_Receiver_Java(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp);
	void    WriteClass_Send_Java(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername);//发送
	void    WriteFunc_SendByteSeq_Java(stFunc& pFunc, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp);//发送流

	

	void    SetJavaPackName(const string& strJavaPackName);

	

public:
	void    GetUsePacketNamesByRefHead_Java(vector<string>& packetNamesList);
	void    GetUsePacketNamesByParType_Java(const string& strParType, vector<string>& packetNamesList);


	string  WriteComment_Java(const string& strTable, const string& strComment, const string& strPar);
#pragma region 类型转换与判断
public:
	bool    IsJavaBaseType(const string& strTypeName, bool bWrite);
	bool    IsJavaBaseType2(string strTypeName);
	string  TypeConvertToJavaType(const string& strTypeName);
	string  TypeConvertToReadJavaType(const string& strTypeName);
	string  TypeConvertToWriteJavaType(const string& strTypeName);

#pragma endregion
};
