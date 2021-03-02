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

// Logger������,�����������ƻ���������Щ�������
enum ELogType
{
	eLogTypeNone = 0,   //ֱ�����
	eLogTypeDebug,      //debug�����
	eLogTypeRelease     //release�����
};

//�ռ�
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
	void Logf( ELogType e, const char* fmt, ... );  //��ʽ�����

protected:
	WJS_Lock  _LogLock;
};


//�ļ����
class  FileLogger : public Logger
{
public:
	FileLogger();
	FileLogger(std::string& strPathName,string& strPreFileName); //·����,�ļ�����ǰһ����
	~FileLogger();

	virtual void Serialize(const char* cstr, ELogType e=eLogTypeNone);
	virtual void Flush();
	virtual std::string GetPathName() { return _strPathName;}

	void  CreateLog(const std::string& strPathName,const string& strPreFileName);

	void  CheckDirectoryEixst(CString strDir);
private:
	void  CreateLog_();//�ڲ�����
protected:
	bool			_bValid;
	string		    _strPathName;
	ofstream		_ofstream;
	BOOL            _bHasCreateLog;
	int             _LineCount;//�м�����ÿ����100W�о����´����ļ�
	int             _FileIndex;//�ļ�����ţ������´���ʱʹ��
	string          _strPreFileName;
	string          _strPrePathName;

	static          int s_fileIndex;
};

//����̨���
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
	unsigned int		_uiBufferCount;    //�������ٸ���д���ļ�
	unsigned int		_uiBufferCurCount; //��ǰ�ļ���
	string		        _strBuffer;        //����
	Logger*             _pLoger;
};

