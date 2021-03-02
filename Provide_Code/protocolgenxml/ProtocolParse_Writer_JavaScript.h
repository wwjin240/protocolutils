#pragma once
#include "ProtocolParse.h"

class ProtocolParse_Writer_JavaScript : public ProtocolParse_Writer {
public:
	ProtocolParse_Writer_JavaScript(const string& strPathName);
	virtual ~ProtocolParse_Writer_JavaScript();
	/**
	* �����ļ����
	*/
	virtual bool    CreateFile_Path(string path, int filetype, bool brefprotocol) override;
	/**
	* ��������Э���Writer
	*/
	virtual ProtocolParse_Writer* Create_RefProtocolParse_Writer(const string& strPathName) override;
public:
	bool    CreateFile_Path_JavaScript(string path, int filetype, bool brefprotocol);//�Ƿ��Ƿ��������Ϊ0�ͻ��ˣ�1��������2�ϳɵ�filetype
	bool    CreateFile_JavaScript(string fileName, int filetype, bool bRefProtocol); //���ɶ���ļ�,��������ļ�����
};
