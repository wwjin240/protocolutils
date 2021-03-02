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
	void SetFileName(const string& strPathName); //设文件名
	bool Parser();//进行解析
	
protected:
	string m_strFileName;
};


