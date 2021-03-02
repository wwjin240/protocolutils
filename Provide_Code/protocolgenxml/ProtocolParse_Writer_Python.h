#pragma once
#include "ProtocolParse.h"

class ProtocolParse_Writer_Python : public ProtocolParse_Writer
{
public:
	ProtocolParse_Writer_Python(const string& strPathName);
	virtual ~ProtocolParse_Writer_Python();
	/**
	* 创建文件入口
	*/
	virtual bool    CreateFile_Path(string path, int filetype, bool brefprotocol) override;
	/**
	* 创建引用协议的Writer
	*/
	virtual ProtocolParse_Writer* Create_RefProtocolParse_Writer(const string& strPathName) override;
public:
	////////////////////////////Python
	bool    CreateFile_Path_Python(string path, int filetype, bool brefprotocol);//是否是服务器版改为0客户端，1服务器，2合成的filetype
	bool    CreateFile_Python(string foldername, int filetype, bool bRefProtocol); //生成多个文件,传入的是文件夹名
	bool    WriteOneNameSpace_Python(stNamespace& pNamespace, int filetype, string foldername); //写整个命名空间,分成.h和.m两部分
	void    CreateAndWriteFuncIdAndProtocolNo_Python(stNamespace& pNamespace, string foldername, const string& strWriteNameSpace);
	void    WriteEnum_Python(stNamespace& pNamespace, stEnum& pEnum, string foldername, const string& strWriteNameSpace);//写枚举tablenum代数制表符数
	void    WriteStruct_Python(stNamespace& pNamespace, stStructStruct& pStruct, string foldername, const string& strWriteNameSpace);//写结构体
	void    WriteVector_Python(stNamespace& pNamespace, stVector& pVector, string foldername, const string& strWriteNameSpace);  //写vector
	void    WriteVector2_Python(stNamespace& pNamespace, stVector& pVector, string foldername, const string& strWriteNameSpace);  //写vector
	void    WriteWJSVector_Python(stNamespace& pNamespace, stWJSVector& pWJSVector, string foldername, const string& strWriteNameSpace);  //写vector
	void    WriteClass_Python(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace);
	void    WriteClass_IReceiver_Python(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, bool bemptyimplement, const string& strWriteNameSpace);//接收接口
	void    WriteFunc_IReceiver_Python(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp, bool bemptyimplement); //
	void    WriteClass_Receiver_Python(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace);
	void    WriteFunc_ReceiverParser_Python(stClass& pClass, int filetype, int tablenum, stNamespace& pNamespace, string foldername, FILE* fp);
	void    WriteFunc_Receiver_Python(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp);
	void    WriteClass_Send_Python(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace);//发送
	void    WriteFunc_SendByteSeq_Python(stFunc& pFunc, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp);//发送流


	void    GetUsePacketNamesByParType_Python(const string& strParType, vector<string>& packetNamesList);

	string  WriteComment_Python(const string& strTable, const string& strComment, const string& strPar, bool bInBlockComment = false);

#pragma region 类型转换与判断
	bool    IsPythonBaseType(const string& strTypeName, bool bWrite);
	bool    IsPythonBaseType2(string strTypeName);
	string  TypeConvertToPythonType(const string& strTypeName);
	string  TypeConvertToReadPythonType(const string& strTypeName);
	string  TypeConvertToWritePythonType(const string& strTypeName);


#pragma endregion
};
