#pragma once
#include "ProtocolParse.h"

class ProtocolParse_Writer_Flash : public ProtocolParse_Writer {
public:
	ProtocolParse_Writer_Flash(const string& strPathName);
	virtual ~ProtocolParse_Writer_Flash();
	/**
	* 创建文件入口
	*/
	virtual bool    CreateFile_Path(string path, int filetype, bool brefprotocol) override;
	/**
	* 创建引用协议的Writer
	*/
	virtual ProtocolParse_Writer* Create_RefProtocolParse_Writer(const string& strPathName) override;
	////////////////////////Flash版
public:
	bool    CreateFile_Path_Flash(string path, int filetype, bool brefprotocol);//是否是服务器版改为0客户端，1服务器，2合成的filetype
	bool    CreateFile_Flash(string foldername, int filetype, bool bRefProtocol); //生成多个文件,传入的是文件夹名
	bool    WriteOneNameSpace_Flash(stNamespace& pNamespace, int filetype, string foldername); //写整个命名空间,分成.h和.m两部分
	void    WriteEnum_Flash(stNamespace& pNamespace, stEnum& pEnum, string foldername);//写枚举tablenum代数制表符数
	void    WriteStruct_Flash(stNamespace& pNamespace, stStructStruct& pStruct, string foldername);//写结构体
	void    WriteVector_Flash(stNamespace& pNamespace, stVector& pVector, string foldername);  //写vector
	void    WriteVector2_Flash(stNamespace& pNamespace, stVector& pVector, string foldername);  //写vector
	void    WriteWJSVector_Flash(stNamespace& pNamespace, stWJSVector& pWJSVector, string foldername);//写类vector
	void    WriteWJSVector2_Flash(stNamespace& pNamespace, stWJSVector& pWJSVector, string foldername);//写类vector
	void    CreateAndWriteFuncIdAndProtocolNo_Flash(stNamespace& pNamespace, string foldername);
	void    WriteClass_Flash(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername);
	void    WriteClass_Send_Flash(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername);//发送
	void    WriteFunc_SendByteSeq_Flash(stFunc& pFunc, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp);//发送流
	void    WriteClass_IReceiver_Flash(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, bool bemptyimplement = false);//接收接口
	void    WriteFunc_IReceiver_Flash(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp, bool bemptyimplement = false); //
	void    WriteFunc_IReceiver_Flash2(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp, bool bemptyimplement = false); //
	void    WriteClass_Receiver_Flash(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername);
	void    WriteFunc_Receiver_Flash(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp);
	void    WriteFunc_ReceiverParser_Flash(stClass& pClass, int filetype, int tablenum, stNamespace& pNamespace, string foldername, FILE* fp);
	

	string  WriteComment_Flash(const string& strTable, const string& strComment, bool bAsParam, const string& strPar, bool bWithSummary = true);

#pragma region 类型转换与判断
	bool    IsFlashBaseType2(string strTypeName);//是否是Flash基本类型
	string  TypeConvertToFlashType(const string& strTypeName);
	string  TypeConvertToReadFlashType(const string& strTypeName);
	string  TypeConvertToWriteFlashType(const string& strTypeName);

#pragma endregion

public:
	string			m_strFlashPackage;   //flash版本的包前缀
	string          m_strFlashToolPackage;
};
