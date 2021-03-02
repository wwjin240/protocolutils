#pragma once
#include "ProtocolParse.h"

class ProtocolParse_Writer_JavaScript : public ProtocolParse_Writer {
public:
	ProtocolParse_Writer_JavaScript(const string& strPathName);
	virtual ~ProtocolParse_Writer_JavaScript();
	/**
	* 创建文件入口
	*/
	virtual bool    CreateFile_Path(string path, int filetype, bool brefprotocol) override;
	/**
	* 创建引用协议的Writer
	*/
	virtual ProtocolParse_Writer* Create_RefProtocolParse_Writer(const string& strPathName) override;
public:
	bool    CreateFile_Path_JavaScript(string path, int filetype, bool brefprotocol);//是否是服务器版改为0客户端，1服务器，2合成的filetype
	bool    CreateFile_JavaScript(string fileName, int filetype, bool bRefProtocol); //生成多个文件,传入的是文件夹名
};
