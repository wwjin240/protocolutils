#pragma  once

#include <string>
#include <vector>
#include "stdio.h"
using namespace std;

/**
 * void�ͺ�����
 */
struct stVoidFuncPar
{
	string _strParType; //��������
	string _strParName; //������
	bool   _isConst;    //�Ƿ��ǳ���
	bool   _isRef;      //�Ƿ�������


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
 * void����
 */
struct stVoidFunc
{
	stVoidFunc()
	{
	}
	string                 _name;    //����
	vector<stVoidFuncPar>  _InParVec;//��������б�
	vector<string>         _commentList; //ע���б�
};

class CCreateEventFunc
{
public:
	CCreateEventFunc(string& strPathName);
	~CCreateEventFunc();

public:
	void SetFileName(const string& strPathName); //���ļ���
	bool Parser();//���н���
	bool   ParLine(const string& strLine,stVoidFunc& VoidFunc);
	void   WriteVoidFuncToFile(FILE* fp,const stVoidFunc& VoidFunc);
	string GetParTypeMacro(const stVoidFuncPar& VoidFuncPar);

protected:
	string m_strFileName;
};


//ö����ʱֻ���ֹ���
//Ĭ��ֵ��ֱ���Ƴ�