#pragma once
#include "ProtocolParse.h"

class ProtocolParse_Writer_TSMiddle : public ProtocolParse_Writer {
public:
	ProtocolParse_Writer_TSMiddle(const string& strPathName);
	virtual ~ProtocolParse_Writer_TSMiddle();
	/**
	* 创建文件入口
	*/
	virtual bool    CreateFile_Path(string path, int filetype, bool brefprotocol) override;
	/**
	* 创建引用协议的Writer
	*/
	virtual ProtocolParse_Writer* Create_RefProtocolParse_Writer(const string& strPathName) override;
public:
	///////////////////////////TypeScript_Middle
	//预计算出所有函数Id
	void    PreComputeAllFuncId(stNamespace& pNamespace);
	//预计算出所有枚举的id
	void    PreComputeAllEnumId(stNamespace& pNamespace);
	void    PreComputeAllEnumId_(stEnum& pEnum);

	void  ConvertComment(const string& strComment, int tablenum, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer); //转换成["x","y"]

	bool    CreateFile_Path_TScript_Middle(string path, int filetype, bool brefprotocol, bool bSaveToFile = true);//是否是服务器版改为0客户端，1服务器，2合成的filetype
	bool    CreateFile_TScript_Middle(string fileName, int filetype, bool bRefProtocol); //生成多个文件,传入的是文件夹名
	bool    WriteOneNameSpace_TScript_Middle(stNamespace& pNamespace, string foldername, int filetype, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer); //写整个命名空间


	void    CreateAndWriteFuncIdAndProtocolNo_TScript_Middle(stNamespace& pNamespace, string foldername, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void    WriteTypes_TScript_Middle(stNamespace& pNamespace, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void    WriteTypes_Enum_TScript_Middle(stNamespace& pNamespace, stEnum& pEnum, int tablenum, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void    WriteTypes_Struct_TScript_Middle(stNamespace& pNamespace, stStructStruct& pStruct, int tablenum, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void    WriteTypes_Vector_TScript_Middle(stNamespace& pNamespace, stVector& pVector, int tablenum, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void    WriteTypes_WJSVector_TScript_Middle(stNamespace& pNamespace, stWJSVector& pWJSVector, int tablenum, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void    WriteClass_TScript_Middle(stClass& pClass, stNamespace& pNamespace, int filetype, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void    WriteSends_TScript_Middle(stClass& pClass, stNamespace& pNamespace, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void    Write_Func_TScript_Middle(stFunc& pFunc, stNamespace& pNamespace, int tablenum, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void    WriteRecvs_TScript_Middle(stClass& pClass, stNamespace& pNamespace, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void    WriteSendRecvs_TScript_Middle(stClass& pClass, stNamespace& pNamespace, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, int SplitSCType);//SplitSCType取0,-1,1

#pragma region 类型转换与判断
	string TypeConvertToMiddleType(const string& strType, bool bFirstEnter); //将类型转换为中间类型
#pragma endregion
};
