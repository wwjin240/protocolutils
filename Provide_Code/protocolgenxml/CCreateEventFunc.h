#pragma  once

#include <string>
#include <vector>
#include "stdio.h"
using namespace std;

/**
 * void型函数参
 */
struct stVoidFuncPar
{
	string _strParType; //参数类型
	string _strParName; //参数名
	bool   _isConst;    //是否是常量
	bool   _isRef;      //是否是引用


	stVoidFuncPar()
	{
		_isConst=false;
		_isRef = false;
	}

	void Reset()
	{
		_isConst=false;
		_isRef = false;
		_strParType = "";
		_strParName = "";


	}

};

/**
 * void函数
 */
struct stVoidFunc
{
	stVoidFunc()
	{
	}
	string                 _name;    //名字
	vector<stVoidFuncPar>  _InParVec;//输入参数列表
	vector<string>         _commentList; //注释列表
};

class CCreateEventFunc
{
public:
	CCreateEventFunc(string& strPathName);
	~CCreateEventFunc();

public:
	void SetFileName(const string& strPathName); //设文件名
	bool Parser();//进行解析
	bool   ParLine(const string& strLine,stVoidFunc& VoidFunc);
	void   WriteVoidFuncToFile(FILE* fp,const stVoidFunc& VoidFunc);
	string GetParTypeMacro(const stVoidFuncPar& VoidFuncPar);

protected:
	string m_strFileName;
};


//枚举暂时只能手工改
//默认值，直接移除