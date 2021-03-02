#pragma once
#include <string>
#include <vector>
#include "WJS_JsonStream.h"

using namespace std;

class ProtocolUtils
{
public:
	static string         GetFullPath(const string& strXDPathName); //ȡ������·��
	static string         GetFullPathFile(const string& strXDPathName);//ȡ�������ļ�
	static int            ComputerXDNum(const string strXDPathName, const string& strFind, int num); //���������
	static string         ComputerPath(const string& strFullPath, const string& strXDPathName); //����·��
	static string         PickPath(const string& strFullPath, int houtuinum); //��ȡ·��
	static string         PickEndFolderByPath(const string& strFullPath); //��ȡ�����ļ���
	static string         GetSubFolderName(const string& foldername);
	static bool           IsHasPath(const string& strFullPath); //�Ƿ���·��
	static bool           IsHasXDPath(const string& strFullPath);//�Ƿ��з�б�ܵ�
	static string         RemovePath(string strPathFileName);
	static void           ChangeXieGang(string& strName); //���ӽ�/ת��\\ ;
	static string         ConvertJson(const string& strYuanJson, int& lineType);
	static stJsonObject*  CreateJsonObjectA(bool bCreateSub);
	static string         GetBaseDir();//�õ�ģ��·��
	static string         GetBaseDir_RemoveEndGang();//�õ�ģ��·��,���Ƴ����ĸ�
	static string         GetModuleName(); //�õ�ģ����
	static void           ConvertToMongoJson(const string& strFileName);
	static void           GetAllSubString(const string& strInput, const string& strSplit, vector<string>& _strVec, bool bOnlyOne = false);//�õ������Ӵ�,strSplitΪ�ָ���
	static bool           findSubStrAndCut(const string& inputStr, const string& subStr, string* cutStr1, string* cutStr2);//�����Ӵ������ָ�,true���ҵ�
	static string         GetLine(char* pChar, int& readNum, bool& bReadAllEnd);//ȡ��һ��
	static string         GetLine_4(char* pChar, int& readNum, bool& bReadAllEnd, bool& bNewLine);//ȡ��һ��
	static size_t         FindString(const string& strYuan, const string& strFind, size_t startIndex);   //�����ַ������õ�λ�ã�-1��δ�ҵ�
	static void           Replace(string& strContext, const string& strYuan, const string& strReplace);
	static bool           IsContextLine(string& strLine);//�����Ƿ�������
	static bool           IsSpaceFu(char ch);//�Ƿ�հ׷�
	static string         FoundStringEndSpace(const string& strLine, int startIndex, int& endIndex); //����һ���ַ������Կհ׷�����
	static string         FoundStringEndSpecifyFuHao(const string& strLine, int startIndex, int& endIndex, char sc, bool bKeepSpace); //����һ���ַ���,��ָ���ַ�����
	static string         RemoveLineNote(string& strLine, string& strComment); //�Ƴ�ע����
	static string         InsertComment(string& strLine, const vector<string>& strCommentLines);//����ע��
	static string         FoundCommont_AfterLine(const string& strLine, int startIndex); //�����к��ע��
	static void           SplitStringByFuncPar(const string& strLine, vector<string>& strPars); //���ַ�������Ϊ�����Ĳ���
	static string         GetFileMd5(const string& strFileName); //�õ��ļ���MD5
	static bool           TestFileMd5(const string& strFileName,const string& strXMLMd5);//���������ļ���ʱʹ�õ�XML��MD5,�뵱ǰ���Ƿ���ͬ
	static void           CheckDirectoryEixst(const std::string& strDir);
	static string         GetSuffix(const string& fileName); //�õ���׺��
	static bool           IsHasValidString(const string& line); //�Ƿ�����Ч�ַ�
	static string         GetBeginString(const string& line); //�õ���ʼ���ַ�
	static void           RemoveLeftGang(string& line); //�Ƴ�����ߵ�

};
