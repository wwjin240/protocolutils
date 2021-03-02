//#include "stdafx.h"
#include "Logger.h"
#include <time.h>
#include <sys/timeb.h>
#include "Shlwapi.h"
using namespace std;

#define GetVarArgs(out, count, fmt)\
	va_list args;\
	va_start( args, fmt );\
	int re = _vsntprintf( tmp, count, fmt, args );\
	va_end( args );

//------------------------------------
Logger::Logger()
{
}

Logger::~Logger()
{
}

void Logger::Log( ELogType e, const char* cstr )
{
	_LogLock.Enter();
	Serialize( cstr, e);
	_LogLock.Leave();

}
void Logger::Log( ELogType e, const string& str )
{
	_LogLock.Enter();
	Serialize( str.c_str(), e);
	_LogLock.Leave();

}
void Logger::Logf( ELogType e, const char* fmt, ... )  //格式化输出
{
	static char tmp[2048]; 
	_LogLock.Enter();
	//memset(tmp, 0, 2048*sizeof(char));
	tmp[0]=0;
	GetVarArgs(tmp, 2048-1, fmt);
	Serialize( tmp, e);
	_LogLock.Leave();
}


//---------------------------
int FileLogger::s_fileIndex=0;
FileLogger::FileLogger()
:Logger()
,_bHasCreateLog(FALSE)
,_LineCount(0)
,_FileIndex(0)
{
}

FileLogger::FileLogger(string& strPathName,string& strPreFileName)
{
	//locale::global(locale("chs"));
	CreateLog(strPathName,strPreFileName);
}

FileLogger::~FileLogger()
{
	_ofstream.close();
}

void  FileLogger::CreateLog(const std::string& strPathName,const string& strPreFileName)
{
	_strPrePathName=strPathName;
	_strPreFileName=strPreFileName;
	//没有路径先生成此路径
	CString csPathName;
	csPathName.Format("%s",strPathName.c_str());

	TCHAR strPathBuffer[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, strPathBuffer);
	CString strOutPath = CString(strPathBuffer);
	if( strOutPath[strOutPath.GetLength()-1] != _T('\\') )
		strOutPath += _T("\\");
	strOutPath += csPathName + _T("\\");

	CheckDirectoryEixst(strOutPath);
	//struct tm *today;
	//time_t long_time;
	//time( &long_time );		    /* Get time as long integer. */
	//today = localtime( &long_time );    /* Convert to local time. */

	struct tm *today;
	__time64_t long_time;
	_time64( &long_time ); 
	today = _localtime64( &long_time ); 

	char strBuffer[MAX_PATH];
	char info[500];
	memset(info, 0, sizeof(info));

	struct _timeb tstruct;
	_ftime(&tstruct);
	int pid=GetCurrentProcessId();

	sprintf(strBuffer, "%s/%s[%d-%02d-%02d_%02d.%02d.%02d.%02d.%d.%d].txt"
		, strPathName.c_str()
		, strPreFileName.c_str()
		, today->tm_year + 1900 
		, today->tm_mon + 1
		, today->tm_mday
		, today->tm_hour
		, today->tm_min
		, today->tm_sec
		, tstruct.millitm
		, pid
		, s_fileIndex
		);

	s_fileIndex++;

	_strPathName = strBuffer;
	_ofstream.open(_strPathName.c_str(),ios::out | ios::ate);

	_bHasCreateLog=TRUE;
}
void FileLogger::Serialize( const char* cstr, ELogType e )
{
	if(!_bHasCreateLog)
		return;

	BOOL bOutPut=TRUE;
	switch(e)
	{
	case eLogTypeDebug:
		{
#ifndef _DEBUG
			bOutPut=FALSE;
#endif
		}
		break;
case eLogTypeRelease:
		{
#ifndef NDEBUG
			bOutPut=FALSE;
#endif
		}
		break;
	}

	if(bOutPut && cstr!=0)
	{
		struct tm *today;
		__time64_t long_time;
		_time64( &long_time ); 
		today = _localtime64( &long_time ); 

		//struct tm *today;
		//time_t long_time;
		//time( &long_time );		    /* Get time as long integer. */
		//today = localtime( &long_time );    /* Convert to local time. */
		struct _timeb tstruct;
		_ftime(&tstruct);
		char strBuffer[MAX_PATH];
		sprintf(strBuffer, "[%d-%02d-%02d %02d:%02d:%02d:%02d] "
			, today->tm_year + 1900 
			, today->tm_mon + 1
			, today->tm_mday
			, today->tm_hour
			, today->tm_min
			, today->tm_sec
			, tstruct.millitm
			);
		_ofstream << strBuffer << cstr << endl;

		_LineCount++;
		if(_LineCount>800000)
		{
			_LineCount=0;
			_FileIndex++;

			_ofstream.flush();
			_ofstream.close();
			CreateLog_();
		}
	}
}

void  FileLogger::CreateLog_()
{
	struct tm *today;
	__time64_t long_time;
	_time64( &long_time ); 
	today = _localtime64( &long_time ); 

	char strBuffer[MAX_PATH];
	char info[500];
	memset(info, 0, sizeof(info));

	struct _timeb tstruct;
	_ftime(&tstruct);
	int pid=GetCurrentProcessId();

	sprintf(strBuffer, "%s/%s[%d-%02d-%02d_%02d.%02d.%02d.%02d.%d.%d].txt"
		, _strPrePathName.c_str()
		, _strPreFileName.c_str()
		, today->tm_year + 1900 
		, today->tm_mon + 1
		, today->tm_mday
		, today->tm_hour
		, today->tm_min
		, today->tm_sec
		, tstruct.millitm
		, pid
		, _FileIndex
		);


	_strPathName = strBuffer;
	_ofstream.open(_strPathName.c_str(),ios::out | ios::ate);

	_bHasCreateLog=TRUE;
}
void FileLogger::Flush()
{
	if(!_bHasCreateLog)
		return;
	_LogLock.Enter();
	_ofstream.flush();
	_LogLock.Leave();
}

void FileLogger::CheckDirectoryEixst(CString strDir)
{
	CString tt_str=strDir;
	CString tt_Path;
	CString tt_Left;

	while(true)
	{
		int index=tt_str.Find(_T("\\"));
		if(index!=-1)
		{
			tt_Left=tt_str.Left(index);
			if(tt_Path.GetLength()>0)
				tt_Path+=(_T("\\") +tt_Left); 
			else
				tt_Path=tt_Left;

			if(!PathFileExists(tt_Path))
			{
				SECURITY_ATTRIBUTES tt_attr;
				tt_attr.nLength=sizeof(SECURITY_ATTRIBUTES);
				tt_attr.lpSecurityDescriptor=NULL;
				tt_attr.bInheritHandle=TRUE;

				CreateDirectory(tt_Path,&tt_attr);
			}

			tt_Left+=_T("\\");
			tt_str = tt_str.Right(tt_str.GetLength()-tt_Left.GetLength()); 	
		}
		else
		{
			break;
		}

	}
}

//---------------------------



StdLogger::StdLogger(Logger* pLog,bool b, unsigned int c)
:_pLoger(pLog)	
,_bToIDE(b)
,_uiBufferCount(c)
,_uiBufferCurCount(0)
{
}

StdLogger::~StdLogger()
{
}

void StdLogger::Serialize( const char* cstr, ELogType e)
{
	_LogLock.Enter();

	string strLogOut = cstr;
	
	cout << strLogOut << endl;

	_strBuffer += strLogOut;
	if(++_uiBufferCurCount == _uiBufferCount)
	{
		_pLoger->Log(eLogTypeNone, _strBuffer);

		_strBuffer = "";
		_uiBufferCurCount = 0;
	}
	else
	{
		_strBuffer.append("\n");
	}


	if(_bToIDE)
	{
		OutputDebugString(strLogOut.c_str());
		OutputDebugString(_T("\n"));
	}

	_LogLock.Leave();
}

void StdLogger::Flush()
{
	if(_uiBufferCurCount != 0)
	{
		_pLoger->Log(eLogTypeNone, _strBuffer);
		_strBuffer = "";
		_uiBufferCurCount = 0;
	}
}

