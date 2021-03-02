#pragma once
#include "ProtocolParse.h"

class ProtocolParse_Writer_IPhone : public ProtocolParse_Writer {
public:
	ProtocolParse_Writer_IPhone(const string& strPathName);
	virtual ~ProtocolParse_Writer_IPhone();
public:
	/**
	* 创建文件入口
	*/
	virtual bool    CreateFile_Path(string path, int filetype, bool brefprotocol) override;
	/**
	* 创建引用协议的Writer
	*/
	virtual ProtocolParse_Writer* Create_RefProtocolParse_Writer(const string& strPathName) override;
	///////////////////////Iphone版
public:
	bool    CreateFile_Path_Iphone(string path, int filetype, bool brefprotocol);//是否是服务器版改为0客户端，1服务器，2合成的filetype
	bool    CreateFile_Iphone(string filename1, string filename2, int filetype); //生成两个文件，一个.h,一个.m
	void    WriteFileHead_Iphone(FILE* fp, int filetype);//写文件头
	void    WriteFileHead_Iphone_M(FILE* fp, int filetype);//写M文件头
	bool    WriteOneNameSpace_Iphone(stNamespace& pNamespace, FILE* fp1, FILE* fp2, int filetype); //写整个命名空间,分成.h和.m两部分
	void    CreateAndWriteFuncId_Iphone(stNamespace& pNamespace, FILE* fp, int tablenum);  //写函数的id
	void    WriteProtocolNo_Iphone(FILE* fp, int tablenum); //写协议号
	void    WriteEnum_Iphone(stEnum& pEnum, FILE* fp, int tablenum);//写枚举tablenum代数制表符数
	void    WriteStruct_Iphone(stStructStruct& pStruct, FILE* fp, int tablenum);//写结构体
	void    WriteVector_Iphone(stVector& pVector, FILE* fp1, FILE* fp2, int tablenum);  //写vector
	string  TypeConvertToIphoneType(const string& str);//将类型转换成iphone类型名
	string  TypeConvertToIphoneType_Write(const string& str);//将类型转换成iphone类型名,不带*号写法
	bool    IsIphoneBaseType(const string& str); //是否是iphone的基本类型
	bool    IsBaseTypeNeedAddRetain(const string& str); //基础类型是否需要增加Retain
	string  GetIphoneTypeRead(const string& datastr, const string& str); //得到iphone常用类型的读方法，没有则认为是复合的
	string  GetIphoneBaseTypeRead_Vector(const string& datastr, const string& str); //vector基本类型的读

	string  GetIphoneTypeWrite(const string& datastr, const string& str); //得到iphone常用类型的读方法，没有则认为是复合的
	string  GetIphoneTypeWrite_Vector_Pre(const string& str);//前部分
	string  GetIphoneTypeWrite_Vector_Next(const string& str);//后部分
															  //bool    IsIphoneProvideReadWrite(const string& str); //是否是iphone需要提供自己的读写类型
	void    WriteWJSVector_Iphone(stWJSVector& pWJSVector, FILE* fp1, FILE* fp2, int tablenum);//写类vector
	void    WriteFileEnd_Iphone_M(FILE* fp, int filetype);//写M文件的结束
	void    WriteStruct_Iphone_M(stStructStruct& pStruct, FILE* fp, int tablenum);//写结构体的M文件
	void    WriteClass_Iphone(stClass& pClass, FILE* fp1, FILE* fp2, int tablenum, int filetype, const string& spacename, stNamespace& pNamespace);
	void    WriteClass_IReceiver_Iphone(stClass& pClass, FILE* fp1, FILE* fp2, int tablenum, int filetype, const string& spacename, bool bemptyimplement = false);//接收接口
	void    WriteClass_Receiver_Iphone(stClass& pClass, FILE* fp1, FILE* fp2, int tablenum, int filetype, const string& spacename, const string& strcallbackclassname);//接收类
	void    WriteClass_Send_Iphone(stClass& pClass, FILE* fp1, FILE* fp2, int tablenum, int filetype, const string& spacename);//发送
	void    WriteFunc_IReceiver_Iphone(stFunc& pFunc, FILE* fp1, FILE* fp2, int tablenum, int filetype, bool bemptyimplement = false);//写纯虚函数,接收
	string  CovertParToString_Iphone(stBasePar& pPar, bool bFirst);//将参数转换成字符串
	void    WriteFunc_Receiver_Iphone(stFunc& pFunc, FILE* fp1, FILE* fp2, int tablenum, int filetype, const string& spacename);//接收函数
	void    WriteFunc_ReceiverParser_Iphone(stClass& pClass, FILE* fp1, FILE* fp2, int tablenum, int filetype, const string& spacename, const string& strcallbackclassname);//接收解析
	void    WriteFunc_SendByteSeq_Iphone(stFunc& pFunc, FILE* fp1, FILE* fp2, int tablenum, string classname);//发送流

};
