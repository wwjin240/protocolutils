#pragma once
#include "ProtocolParse.h"

class ProtocolParse_Writer_IPhone : public ProtocolParse_Writer {
public:
	ProtocolParse_Writer_IPhone(const string& strPathName);
	virtual ~ProtocolParse_Writer_IPhone();
public:
	/**
	* �����ļ����
	*/
	virtual bool    CreateFile_Path(string path, int filetype, bool brefprotocol) override;
	/**
	* ��������Э���Writer
	*/
	virtual ProtocolParse_Writer* Create_RefProtocolParse_Writer(const string& strPathName) override;
	///////////////////////Iphone��
public:
	bool    CreateFile_Path_Iphone(string path, int filetype, bool brefprotocol);//�Ƿ��Ƿ��������Ϊ0�ͻ��ˣ�1��������2�ϳɵ�filetype
	bool    CreateFile_Iphone(string filename1, string filename2, int filetype); //���������ļ���һ��.h,һ��.m
	void    WriteFileHead_Iphone(FILE* fp, int filetype);//д�ļ�ͷ
	void    WriteFileHead_Iphone_M(FILE* fp, int filetype);//дM�ļ�ͷ
	bool    WriteOneNameSpace_Iphone(stNamespace& pNamespace, FILE* fp1, FILE* fp2, int filetype); //д���������ռ�,�ֳ�.h��.m������
	void    CreateAndWriteFuncId_Iphone(stNamespace& pNamespace, FILE* fp, int tablenum);  //д������id
	void    WriteProtocolNo_Iphone(FILE* fp, int tablenum); //дЭ���
	void    WriteEnum_Iphone(stEnum& pEnum, FILE* fp, int tablenum);//дö��tablenum�����Ʊ����
	void    WriteStruct_Iphone(stStructStruct& pStruct, FILE* fp, int tablenum);//д�ṹ��
	void    WriteVector_Iphone(stVector& pVector, FILE* fp1, FILE* fp2, int tablenum);  //дvector
	string  TypeConvertToIphoneType(const string& str);//������ת����iphone������
	string  TypeConvertToIphoneType_Write(const string& str);//������ת����iphone������,����*��д��
	bool    IsIphoneBaseType(const string& str); //�Ƿ���iphone�Ļ�������
	bool    IsBaseTypeNeedAddRetain(const string& str); //���������Ƿ���Ҫ����Retain
	string  GetIphoneTypeRead(const string& datastr, const string& str); //�õ�iphone�������͵Ķ�������û������Ϊ�Ǹ��ϵ�
	string  GetIphoneBaseTypeRead_Vector(const string& datastr, const string& str); //vector�������͵Ķ�

	string  GetIphoneTypeWrite(const string& datastr, const string& str); //�õ�iphone�������͵Ķ�������û������Ϊ�Ǹ��ϵ�
	string  GetIphoneTypeWrite_Vector_Pre(const string& str);//ǰ����
	string  GetIphoneTypeWrite_Vector_Next(const string& str);//�󲿷�
															  //bool    IsIphoneProvideReadWrite(const string& str); //�Ƿ���iphone��Ҫ�ṩ�Լ��Ķ�д����
	void    WriteWJSVector_Iphone(stWJSVector& pWJSVector, FILE* fp1, FILE* fp2, int tablenum);//д��vector
	void    WriteFileEnd_Iphone_M(FILE* fp, int filetype);//дM�ļ��Ľ���
	void    WriteStruct_Iphone_M(stStructStruct& pStruct, FILE* fp, int tablenum);//д�ṹ���M�ļ�
	void    WriteClass_Iphone(stClass& pClass, FILE* fp1, FILE* fp2, int tablenum, int filetype, const string& spacename, stNamespace& pNamespace);
	void    WriteClass_IReceiver_Iphone(stClass& pClass, FILE* fp1, FILE* fp2, int tablenum, int filetype, const string& spacename, bool bemptyimplement = false);//���սӿ�
	void    WriteClass_Receiver_Iphone(stClass& pClass, FILE* fp1, FILE* fp2, int tablenum, int filetype, const string& spacename, const string& strcallbackclassname);//������
	void    WriteClass_Send_Iphone(stClass& pClass, FILE* fp1, FILE* fp2, int tablenum, int filetype, const string& spacename);//����
	void    WriteFunc_IReceiver_Iphone(stFunc& pFunc, FILE* fp1, FILE* fp2, int tablenum, int filetype, bool bemptyimplement = false);//д���麯��,����
	string  CovertParToString_Iphone(stBasePar& pPar, bool bFirst);//������ת�����ַ���
	void    WriteFunc_Receiver_Iphone(stFunc& pFunc, FILE* fp1, FILE* fp2, int tablenum, int filetype, const string& spacename);//���պ���
	void    WriteFunc_ReceiverParser_Iphone(stClass& pClass, FILE* fp1, FILE* fp2, int tablenum, int filetype, const string& spacename, const string& strcallbackclassname);//���ս���
	void    WriteFunc_SendByteSeq_Iphone(stFunc& pFunc, FILE* fp1, FILE* fp2, int tablenum, string classname);//������

};
