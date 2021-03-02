/*
* create by wangweijin
*/
#pragma once
#include <string>

#include <iostream>
#include <fstream>

#include "WJS_Lock.h"
//#include "Lock/WJS_AutoLock.h"
#include <atlstr.h>


using namespace std;

// Logger的类型,可以用来控制或者配置哪些可以输出
enum ELogType
{
	eLogTypeNone = 0,   //直接输出
	eLogTypeDebug,      //debug下输出
	eLogTypeRelease     //release下输出
};

//日记
class  Logger
{
public:
	Logger();
	virtual ~Logger();

	virtual void Serialize( const char* cstr, ELogType e=eLogTypeNone)=0;
	virtual void Flush()=0;
	virtual std::string GetPathName()=0;

	void Log( ELogType e, const char* cstr );
	void Log( ELogType e, const string& str );
	void Logf( ELogType e, const char* fmt, ... );  //格式化输出

protected:
	WJS_Lock  _LogLock;
};


//文件输出
class  FileLogger : public Logger
{
public:
	FileLogger();
	FileLogger(std::string& strPathName,string& strPreFileName); //路径名,文件名的前一部分
	~FileLogger();

	virtual void Serialize(const char* cstr, ELogType e=eLogTypeNone);
	virtual void Flush();
	virtual std::string GetPathName() { return _strPathName;}

	void  CreateLog(const std::string& strPathName,const string& strPreFileName);

	void  CheckDirectoryEixst(CString strDir);
private:
	void  CreateLog_();//内部创建
protected:
	bool			_bValid;
	string		    _strPathName;
	ofstream		_ofstream;
	BOOL            _bHasCreateLog;
	int             _LineCount;//行计数，每满足100W行就重新创建文件
	int             _FileIndex;//文件的序号，当重新创建时使用
	string          _strPreFileName;
	string          _strPrePathName;

	static          int s_fileIndex;
};

//控制台输出
class  StdLogger : public Logger
{
public:
	StdLogger(Logger* pLog,bool b = true, unsigned int c = 128);
	~StdLogger();

	virtual void Serialize( const char* cstr, ELogType e=eLogTypeNone);
	virtual void Flush();
	virtual std::string GetPathName(){return "";}


	void			ToIDE(bool b){ _bToIDE = b; }

protected:
	bool				_bToIDE;           
	unsigned int		_uiBufferCount;    //计满多少个就写进文件
	unsigned int		_uiBufferCurCount; //当前的计数
	string		        _strBuffer;        //缓存
	Logger*             _pLoger;
};

