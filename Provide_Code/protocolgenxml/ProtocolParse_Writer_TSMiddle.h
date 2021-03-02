#pragma once
#include "ProtocolParse.h"

class ProtocolParse_Writer_TSMiddle : public ProtocolParse_Writer {
public:
	ProtocolParse_Writer_TSMiddle(const string& strPathName);
	virtual ~ProtocolParse_Writer_TSMiddle();
	/**
	* �����ļ����
	*/
	virtual bool    CreateFile_Path(string path, int filetype, bool brefprotocol) override;
	/**
	* ��������Э���Writer
	*/
	virtual ProtocolParse_Writer* Create_RefProtocolParse_Writer(const string& strPathName) override;
public:
	///////////////////////////TypeScript_Middle
	//Ԥ��������к���Id
	void    PreComputeAllFuncId(stNamespace& pNamespace);
	//Ԥ���������ö�ٵ�id
	void    PreComputeAllEnumId(stNamespace& pNamespace);
	void    PreComputeAllEnumId_(stEnum& pEnum);

	void  ConvertComment(const string& strComment, int tablenum, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer); //ת����["x","y"]

	bool    CreateFile_Path_TScript_Middle(string path, int filetype, bool brefprotocol, bool bSaveToFile = true);//�Ƿ��Ƿ��������Ϊ0�ͻ��ˣ�1��������2�ϳɵ�filetype
	bool    CreateFile_TScript_Middle(string fileName, int filetype, bool bRefProtocol); //���ɶ���ļ�,��������ļ�����
	bool    WriteOneNameSpace_TScript_Middle(stNamespace& pNamespace, string foldername, int filetype, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer); //д���������ռ�


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
	void    WriteSendRecvs_TScript_Middle(stClass& pClass, stNamespace& pNamespace, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, int SplitSCType);//SplitSCTypeȡ0,-1,1

#pragma region ����ת�����ж�
	string TypeConvertToMiddleType(const string& strType, bool bFirstEnter); //������ת��Ϊ�м�����
#pragma endregion
};
