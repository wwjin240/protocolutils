#pragma once
#include "ProtocolParse.h"


/**
* 写文件C++版
*/
class ProtocolParse_Writer_CPlus : public ProtocolParse_Writer
{
public:
	ProtocolParse_Writer_CPlus(const string& strPathName, eCPlusStandardType cplusStandardType,bool bUseStdByte);
	virtual ~ProtocolParse_Writer_CPlus();
public:
	virtual void    WriteInfoHead(FILE* fp, bool bLua = false, bool bPython = false) override;//写头
	virtual void    WriteStartZhongKuoKao(FILE* fp) override;  //写{}
	virtual void    WriteEndZhongKuoKao(FILE* fp) override;

public:
	/**
	* 创建文件入口
	*/
	virtual bool    CreateFile_Path(string path, int filetype, bool brefprotocol) override;
	/**
	* 创建引用协议的Writer
	*/
	virtual ProtocolParse_Writer* Create_RefProtocolParse_Writer(const string& strPathName) override;
	////////////////////生成CPlus文件
public:
	bool    CreateFile_Path_CPlus(string path, int filetype, bool brefprotocol);//是否是服务器版改为0客户端，1服务器，2合成的filetype
	bool    CreateFile_CPlus(string filename, int filetype, bool bRefProtocol);
	void    WriteFileHead_CPlus(FILE* fp, int filetype);//写文件头
	bool    WriteOneNameSpace_CPlus(stNamespace& pNamespace, FILE* fp, int filetype, int HCPPType = 0); //写整个命名空间,HCCPType 0表原来的单文件方式，1表H，2表CPP
	void    CreateAndWriteFuncId_CPlus(stNamespace& pNamespace, FILE* fp, int tablenum);  //写函数的id
	void    WriteProtocolNo_CPlus(stNamespace& pNamespace, FILE* fp, int tablenum); //写协议号
	void    WriteEnum_CPlus(stEnum& pEnum, FILE* fp, int tablenum);//写枚举tablenum代数制表符数

	void    WriteStruct_CPlus(const string& rNameSpace,stStructStruct& pStruct, FILE* fp, int tablenum, int HCPPType = 0);//写结构体
	void    WriteStruct_OutStream_CPlus(const string& rNameSpace, stStructStruct& pStruct, FILE* fp, int tablenum, bool bOutStream2, int HCPPType = 0);//写结构体
	void    WriteStruct_JsonStream_CPlus(const string& rNameSpace, stStructStruct& pStruct, FILE* fp, int tablenum, int HCPPType = 0);
	string  WriteJsonStream_Array(int tablenum, const string& strParName, const string& strParType, const string& strMember, bool bCreateArrayPointer);
	string  WriteJsonStream_Array0ToObject(int tablenum, const string& strParName, const string& strParType, const string& strMember);

	void    WriteVector_JsonStream_CPlus(stVector& pVector, FILE* fp, int tablenum, int HCPPType = 0);  //写vector
	void    WriteWJSVector_JsonStream_CPlus(stWJSVector& pVector, FILE* fp, int tablenum, int HCPPType = 0);  //写vector
	void    WriteVector_CPlus(stVector& pVector, FILE* fp, int tablenum, int HCPPType = 0);  //写vector
	void    WriteWJSVector_CPlus(stWJSVector& pWJSVector, FILE* fp, int tablenum, int HCPPType = 0);//写类vector

	void    WriteClass_CPlus(stClass& pClass, FILE* fp, int tablenum, int filetype, const string& spacename, stNamespace& pNamespace, int HCPPType = 0);

	void    WriteClass_IReceiver_CPlus(stClass& pClass, FILE* fp, int tablenum, int filetype, const string& spacename, bool bemptyimplement = false);//接收接口
	void    WriteFunc_IReceiver_CPlus(stFunc& pFunc, FILE* fp, int tablenum, int filetype, bool bemptyimplement = false);//写纯虚函数,接收

	void    WriteClass_Receiver_CPlus(stClass& pClass, FILE* fp, int tablenum, int filetype, const string& spacename, const string& strcallbackclassname, stNamespace& pNamespace, int HCPPType = 0);//接收类
	void    WriteFunc_Receiver_CPlus(stFunc& pFunc, FILE* fp, int tablenum, int filetype, const string& spacename, stNamespace& pNamespace, int HCPPType = 0);//接收函数
	void    WriteFunc_ReceiverParser_CPlus(stClass& pClass, FILE* fp, int tablenum, int filetype, const string& spacename, const string& strcallbackclassname, stNamespace& pNamespace, int HCPPType = 0);//接收解析

	void    WriteClass_Send_CPlus(stClass& pClass, FILE* fp, int tablenum, int filetype, const string& spacename, stNamespace& pNamespace);//发送
	void    WriteFunc_SendByteSeq_CPlus(stFunc& pFunc, FILE* fp, int tablenum, int filetype, string classname, stNamespace& pNamespace);//发送流
	void    WriteFunc_SendByteSeqV2_CPlus(stFunc& pFunc, FILE* fp, int tablenum, int filetype, string classname, stNamespace& pNamespace);//发送流,第二版，引入一个传入变量减少复制过程,但传入的变量本身要清空

#pragma region 注释
	/**
	* 旧版的//注释
	*/
	string  WriteComment_CPlus(const string& strTable, const string& strComment, const string& strPar);
	/**
	* 写块注释的开始
	*/
	string  WriteBlockCommentBegin_CPlus(const string& strTable);
	/**
	* 写块注释的结束
	*/
	string  WriteBlockCommentEnd_CPlus(const string& strTable);
	/**
	* 写块参数注释
	*/
	string  WriteBlockCommentPar_CPlus(const string& strTable, const string& strComment, const string& strPar);
	/**
	* 写块brief注释
	*/
	string  WriteBlockCommentBrief_CPlus(const string& strTable, const string& strComment);

#pragma endregion

public:
	/**
	 * @brief 得到变量类型的完整类型,主要是补std命名空间
	 * @param memberType 
	 * @return 
	*/
	string  GetMemberTypeFullType_CPlus(const string& memberType);

	/**
	 * @brief 是否是可快速处理的vector类型
	 * @param strtypename 
	 * @return 
	*/
	bool    IsCanFastProcessVectorType_CPlus(const string& strtypename); 

	/**
	 * @brief 将参数转换成字符串
	 * @param pPar 
	 * @return 
	*/
	string  CovertParToString_CPlus(stBasePar& pPar);
protected:
	std::map<string, string>  _memberTypeFullTypeMap;      //变量类型的完整类型

	std::set<string>          _canFastProcessVectorTypeSet; //可快速取理的vector类型集，可直接memcopy


	eCPlusStandardType        _cplusStandardType; //C++标准类型

	bool                      _bUseStdByte;   //是否使用std::byte,c++11标准之上默认为使用
};
