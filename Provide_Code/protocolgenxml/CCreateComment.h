#pragma  once

#include <string>
#include <vector>
using namespace std;

class CCreateComment
{
public:
	CCreateComment(string& strPathName);
	~CCreateComment();

public:
	void SetFileName(const string& strPathName); //���ļ���
	bool Parser();//���н���
	
protected:
	string m_strFileName;
};


