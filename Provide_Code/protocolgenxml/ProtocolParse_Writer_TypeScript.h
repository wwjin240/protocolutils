#pragma once
#include "ProtocolParse.h"

class ProtocolParse_Writer_TypeScript : public ProtocolParse_Writer {
public:
	ProtocolParse_Writer_TypeScript(const string& strPathName);
	virtual ~ProtocolParse_Writer_TypeScript();
	/**
	* 创建文件入口
	*/
	virtual bool    CreateFile_Path(string path, int filetype, bool brefprotocol) override;
	/**
	* 创建引用协议的Writer
	*/
	virtual ProtocolParse_Writer* Create_RefProtocolParse_Writer(const string& strPathName) override;
	//////////////////////////TypeScript_Model
	bool    CreateFile_TScript_Model(string fileName, int filetype, bool bRefProtocol); //生成多个文件,传入的是文件夹名
	bool    WriteOneNameSpace_TScript_Model(stNamespace& pNamespace, int filetype, string foldername, FILE* fp); //写整个命名空间
	void    CreateAndWriteFuncIdAndProtocolNo_TScript_Model(stNamespace& pNamespace, string foldername, const string& strWriteNameSpace, FILE* fp);
	void    WriteEnum_TScript_Model(stNamespace& pNamespace, stEnum& pEnum, string foldername, const string& strWriteNameSpace, FILE* fp);//写枚举tablenum代数制表符数
	void    WriteStruct_TScript_Model(stNamespace& pNamespace, stStructStruct& pStruct, string foldername, const string& strWriteNameSpace, FILE* fp);//写结构体
	void    WriteVector_TScript_Model(stNamespace& pNamespace, stVector& pVector, string foldername, const string& strWriteNameSpace, FILE* fp);  //写vector
	void    WriteVector2_TScript_Model(stNamespace& pNamespace, stVector& pVector, string foldername, const string& strWriteNameSpace, FILE* fp);  //写vector
	void    WriteWJSVector_TScript_Model(stNamespace& pNamespace, stWJSVector& pWJSVector, string foldername, const string& strWriteNameSpace, FILE* fp);  //写vector
	void    WriteWJSVector2_TScript_Model(stNamespace& pNamespace, stWJSVector& pWJSVector, string foldername, const string& strWriteNameSpace, FILE* fp);  //写vector
	void    WriteClass_TScript_Model(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace, FILE* fp);
	void    WriteClass_IReceiver_TScript_Model(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, bool bemptyimplement, const string& strWriteNameSpace, FILE* fp);//接收接口
	void    WriteFunc_IReceiver_TScript_Model(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp, bool bemptyimplement); //
	void    WriteClass_Receiver_TScript_Model(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace, FILE* fp);
	void    WriteFunc_ReceiverParser_TScript_Model(stClass& pClass, int filetype, int tablenum, stNamespace& pNamespace, string foldername, FILE* fp);
	void    WriteClass_Send_TScript_Model(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace, FILE* fp);//发送
	void    WriteFunc_SendByteSeq_TScript_Model(stFunc& pFunc, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp);//发送流
	void    WriteFunc_Receiver_TScript_Model(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp);

	/////////////////////////TypeScript_Namespace
public:
	bool    CreateFile_Path_TScript(string path, int filetype, bool brefprotocol, bool bSaveToFile = true);//是否是服务器版改为0客户端，1服务器，2合成的filetype

public:
	bool    CreateFile_TScript_NSV(string foldername, int filetype, bool bRefProtocol); //生成多个文件,传入的是文件夹名
	bool    WriteOneNameSpace_TScript_NSV(stNamespace& pNamespace, int filetype, string foldername); //写整个命名空间
	void    CreateAndWriteFuncIdAndProtocolNo_TScript_NSV(stNamespace& pNamespace, string foldername, const string& strWriteNameSpace);
	void    WriteEnum_TScript_NSV(stNamespace& pNamespace, stEnum& pEnum, string foldername, const string& strWriteNameSpace);//写枚举tablenum代数制表符数
	void    WriteStruct_TScript_NSV(stNamespace& pNamespace, stStructStruct& pStruct, string foldername, const string& strWriteNameSpace);//写结构体
	void    WriteVector_TScript_NSV(stNamespace& pNamespace, stVector& pVector, string foldername, const string& strWriteNameSpace);  //写vector
	void    WriteVector2_TScript_NSV(stNamespace& pNamespace, stVector& pVector, string foldername, const string& strWriteNameSpace);  //写vector
	void    WriteWJSVector_TScript_NSV(stNamespace& pNamespace, stWJSVector& pWJSVector, string foldername, const string& strWriteNameSpace);  //写vector
	void    WriteWJSVector2_TScript_NSV(stNamespace& pNamespace, stWJSVector& pWJSVector, string foldername, const string& strWriteNameSpace);  //写vector
	void    WriteClass_TScript_NSV(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace);
	void    WriteClass_IReceiver_TScript_NSV(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, bool bemptyimplement, const string& strWriteNameSpace);//接收接口
	void    WriteFunc_IReceiver_TScript_NSV(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp, bool bemptyimplement); //
	void    WriteClass_Receiver_TScript_NSV(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace);
	void    WriteFunc_ReceiverParser_TScript_NSV(stClass& pClass, int filetype, int tablenum, stNamespace& pNamespace, string foldername, FILE* fp);
	void    WriteClass_Send_TScript_NSV(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace);//发送
	void    WriteFunc_SendByteSeq_TScript_NSV(stFunc& pFunc, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp);//发送流
	void    WriteFunc_Receiver_TScript_NSV(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp);


	//void    CreateAndWriteFuncIdAndProtocolNo_TScript(stNamespace& pNamespace, string foldername, const string& strWriteNameSpace);	
	//void    WriteEnum_TScript(stNamespace& pNamespace, stEnum& pEnum, string foldername, const string& strWriteNameSpace);//写枚举tablenum代数制表符数
	//void    WriteStruct_TScript(stNamespace& pNamespace, stStructStruct& pStruct, string foldername, const string& strWriteNameSpace);//写结构体
	//void    WriteVector_TScript(stNamespace& pNamespace, stVector& pVector, string foldername, const string& strWriteNameSpace);  //写vector
	//void    WriteVector2_TScript(stNamespace& pNamespace, stVector& pVector, string foldername, const string& strWriteNameSpace);  //写vector
	//void    WriteWJSVector_TScript(stNamespace& pNamespace, stWJSVector& pWJSVector, string foldername, const string& strWriteNameSpace);  //写vector	
	//void    WriteWJSVector2_TScript(stNamespace& pNamespace, stWJSVector& pWJSVector, string foldername, const string& strWriteNameSpace);  //写vector

	//void    WriteClass_IReceiver_TScript(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, bool bemptyimplement, const string& strWriteNameSpace);//接收接口

	//void    WriteClass_Receiver_TScript(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace);

	//void    WriteFunc_ReceiverParser_TScript(stClass& pClass, int filetype, int tablenum, stNamespace& pNamespace, string foldername, FILE* fp);


	//void    WriteClass_Send_TScript(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace);//发送

	//void    WriteFunc_SendByteSeq_TScript(stFunc& pFunc, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp);//发送流

	void    GetUsePacketNamesByRefHead_TScript(vector<string>& packetNamesList);

	string  ConvertDefaultValue(const string& strTypeName, const string& strDefaultValue);
	void    GetUsePacketNamesByParType_TScript(const string& strParType, vector<string>& packetNamesList, const string& selffoldername);
	void    GetUsePacketNamesByParType_TScript_NSV(const string& strParType, vector<string>& packetNamesList, const string& selffoldername, bool bFirst = true);
	void    GetUsePacketNamesByParType_TScript_reference(const string& strParType, vector<string>& packetNamesList, const string& selffoldername);
	
	string  GetSubFolderName(const string& foldername);


	string  WriteComment_TScript(const string& strTable, const string& strComment, const string& strPar, bool bWithBlock = true);
#pragma region 类型转换与判断	
	bool    IsTScriptBaseType(const string& strTypeName, bool bWrite, bool bNotReadAndWrite = false);
	bool    IsTScriptBaseType2(string strTypeName);
	string  TypeConvertToTScriptType(const string& strTypeName);
	string  TypeConvertToReadTScriptType(const string& strTypeName);
	string  TypeConvertToWriteTScriptType(const string& strTypeName);


	bool    IsInt64_TScript(const string& strTypeName);
	string  ExTypeNameConvert_TScript(const string& strTypeName);

#pragma endregion
public:
	vector<string>  m_ImportPacketNameList_TS_Model;
};