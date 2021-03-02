#pragma once
#include <string>
#include <vector>
#include "WJS_JsonStream.h"

using namespace std;

class ProtocolUtils
{
public:
	static string         GetFullPath(const string& strXDPathName); //取到的是路径
	static string         GetFullPathFile(const string& strXDPathName);//取到的是文件
	static int            ComputerXDNum(const string strXDPathName, const string& strFind, int num); //计算相对数
	static string         ComputerPath(const string& strFullPath, const string& strXDPathName); //计算路径
	static string         PickPath(const string& strFullPath, int houtuinum); //提取路径
	static string         PickEndFolderByPath(const string& strFullPath); //提取最后的文件夹
	static string         GetSubFolderName(const string& foldername);
	static bool           IsHasPath(const string& strFullPath); //是否有路径
	static bool           IsHasXDPath(const string& strFullPath);//是否有反斜杠的
	static string         RemovePath(string strPathFileName);
	static void           ChangeXieGang(string& strName); //增加将/转成\\ ;
	static string         ConvertJson(const string& strYuanJson, int& lineType);
	static stJsonObject*  CreateJsonObjectA(bool bCreateSub);
	static string         GetBaseDir();//得到模块路径
	static string         GetBaseDir_RemoveEndGang();//得到模块路径,并移除最后的杠
	static string         GetModuleName(); //得到模块名
	static void           ConvertToMongoJson(const string& strFileName);
	static void           GetAllSubString(const string& strInput, const string& strSplit, vector<string>& _strVec, bool bOnlyOne = false);//得到所有子串,strSplit为分隔符
	static bool           findSubStrAndCut(const string& inputStr, const string& subStr, string* cutStr1, string* cutStr2);//查找子串，并分割,true是找到
	static string         GetLine(char* pChar, int& readNum, bool& bReadAllEnd);//取到一行
	static string         GetLine_4(char* pChar, int& readNum, bool& bReadAllEnd, bool& bNewLine);//取到一行
	static size_t         FindString(const string& strYuan, const string& strFind, size_t startIndex);   //查找字符串，得到位置，-1表未找到
	static void           Replace(string& strContext, const string& strYuan, const string& strReplace);
	static bool           IsContextLine(string& strLine);//本行是否有内容
	static bool           IsSpaceFu(char ch);//是否空白符
	static string         FoundStringEndSpace(const string& strLine, int startIndex, int& endIndex); //查找一个字符串，以空白符结束
	static string         FoundStringEndSpecifyFuHao(const string& strLine, int startIndex, int& endIndex, char sc, bool bKeepSpace); //查找一个字符串,以指定字符结束
	static string         RemoveLineNote(string& strLine, string& strComment); //移除注释行
	static string         InsertComment(string& strLine, const vector<string>& strCommentLines);//插入注释
	static string         FoundCommont_AfterLine(const string& strLine, int startIndex); //查找行后的注释
	static void           SplitStringByFuncPar(const string& strLine, vector<string>& strPars); //将字符串分离为函数的参数
	static string         GetFileMd5(const string& strFileName); //得到文件的MD5
	static bool           TestFileMd5(const string& strFileName,const string& strXMLMd5);//测试生成文件当时使用的XML的MD5,与当前的是否相同
	static void           CheckDirectoryEixst(const std::string& strDir);
	static string         GetSuffix(const string& fileName); //得到后缀名
	static bool           IsHasValidString(const string& line); //是否有有效字符
	static string         GetBeginString(const string& line); //得到开始空字符
	static void           RemoveLeftGang(string& line); //移除杠左边的

};
