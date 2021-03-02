#include "ProtocolUtils.h"
#include <windows.h>
#include <regex>
#include "md5.h"
#include <algorithm>
#include <shlwapi.h>

string  ProtocolUtils::GetFullPath(const string& strXDPathName)
{
	string t_xdPath = strXDPathName;
	//都改到右斜
	ProtocolUtils::ChangeXieGang(t_xdPath);

	//绝对路径
	if (t_xdPath.find(":\\") == 1) {
		auto t_pos = t_xdPath.rfind(".");
		if (t_pos != string::npos) {
			t_xdPath = t_xdPath.substr(0, t_pos);
			t_pos = t_xdPath.rfind("\\");
			if (t_pos != string::npos) {
				t_xdPath = t_xdPath.substr(0, t_pos);
			}
		}
		if (t_xdPath.size() > 0 && t_xdPath.back() != '\\')
			t_xdPath.push_back('\\');
		return t_xdPath;
	}
	else {
		auto t_pos = t_xdPath.rfind(".");
		if (t_pos == string::npos) {
			if (t_xdPath.size() > 0 && t_xdPath.back() != '\\')
				t_xdPath.push_back('\\');
		}
	}




	string strFullPath = t_xdPath;
	//是否有相对符
	if (IsHasXDPath(strFullPath)) {
		string strCurFullPath = GetModuleName(); //模块名带后缀
		string tt_fullpath = ComputerPath(strCurFullPath, strFullPath);
		strFullPath = tt_fullpath;
		auto t_pos = strFullPath.rfind(".");
		if (t_pos != string::npos) {
			while (strFullPath.length() > 0 && strFullPath.back() != '\\') {
				strFullPath.pop_back();
			}
		}
	}
	else {
		strFullPath = GetBaseDir_RemoveEndGang();
		strFullPath.append("\\");

		//未找到.
		auto t_pos = t_xdPath.rfind(".");
		if (t_pos == string::npos) {
			if (t_xdPath.length() > 0 && t_xdPath[0] == '\\')
				strFullPath.pop_back();
			strFullPath.append(t_xdPath.c_str());
		}
	}

	if (strFullPath.length() > 0 && strFullPath.back() != '\\')
		strFullPath.append("\\");

	return std::move(strFullPath);
}
string         ProtocolUtils::GetFullPathFile(const string& strXDPathName)
{
	string t_s = GetFullPath(strXDPathName);
	if (t_s.length() > 0 && t_s.back() == '\\') {
		t_s.pop_back();
	}

	return std::move(t_s);
}
int ProtocolUtils::ComputerXDNum(const string strXDPathName, const string& strFind, int num)
{
	size_t npos = strXDPathName.find(strFind);
	if (npos == string::npos) {
		return num;
	}
	else {
		string strFind_2 = strFind;
		strFind_2.append("..\\");
		return ComputerXDNum(strXDPathName, strFind_2, num + 1);
	}
}

//计算路径
string ProtocolUtils::ComputerPath(const string& strFullPath, const string& strXDPathName)
{
	string t_strXDPathName = strXDPathName;
	ProtocolUtils::ChangeXieGang(t_strXDPathName);
	string str2 = t_strXDPathName;
	int xdnum = ComputerXDNum(t_strXDPathName, "..\\", 0);
	if (xdnum <= 0) {
		string str1 = PickPath(strFullPath, xdnum + 1);
		if (str1.length() > 0 && str1.back() != '\\')
			str1.append("\\");
		if (str2.length() > 0 && str2[0] == '\\')
			str1.pop_back();
		str1.append(str2);
	
		return str1;
	}
	string strFound;

	int t_xdnum = xdnum;
	while (t_xdnum > 0) {
		strFound.append("..\\");
		t_xdnum--;
	}

	string str1 = PickPath(strFullPath, xdnum + 1);
	size_t npos = str2.find(strFound);
	if (npos == string::npos) {
		return t_strXDPathName;
	}
	else {
		str2.erase(npos, strFound.length());
		if (str1.length() > 0 && str1.back() != '\\')
			str1.append("\\");
		if (str2.length() > 0 && str2[0] == '\\')
			str1.pop_back();
		str1.append(str2);
		return str1;
	}
	return t_strXDPathName;
}
string ProtocolUtils::PickPath(const string& strFullPath, int houtuinum)
{
	string strPath1 = strFullPath;
	string strPath2 = strFullPath;
	while (houtuinum > 0) {
		int npos = strPath1.find_last_of("\\");
		if (npos == -1)
			break;
		strPath2 = strPath1.substr(0, npos);
		houtuinum--;
		if (houtuinum <= 0)
			break;
		strPath1 = strPath2;
	}

	return strPath2;
}

string         ProtocolUtils::PickEndFolderByPath(const string& strFullPath)
{
	if (strFullPath.length() > 0 && strFullPath.back() == '\\') {
		string s = strFullPath;
		s.pop_back();
		return GetSubFolderName(s);
	}
	return GetSubFolderName(strFullPath);
}

string  ProtocolUtils::GetSubFolderName(const string& foldername)
{
	size_t tt_pos = foldername.rfind("\\");
	if (tt_pos != string::npos) {
		return foldername.substr(tt_pos + 1);
	}
	else {
		tt_pos = foldername.rfind("/");
		if (tt_pos != string::npos) {
			return foldername.substr(tt_pos + 1);
		}
	}

	return foldername;
}

bool    ProtocolUtils::IsHasPath(const string& strFullPath)
{
	size_t npos = strFullPath.find("\\");
	if (npos == string::npos)
		return false;

	return true;
}

bool ProtocolUtils::IsHasXDPath(const string& strFullPath)
{
	size_t npos = strFullPath.find("..\\");
	if (npos == string::npos)
		return false;

	return true;
}

string ProtocolUtils::RemovePath(string strPathFileName)
{
	size_t npos = strPathFileName.find_last_of("\\");
	if (npos != string::npos) {
		strPathFileName.erase(0, npos + 1);
		return strPathFileName;
	}
	else {
		return strPathFileName;
	}

}

void ProtocolUtils::ChangeXieGang(string& strName)
{
	size_t t_pos = strName.find("/");
	while (t_pos != string::npos) {
		strName.replace(t_pos, 1, "\\");
		t_pos = strName.find("/");
	}
}
string ProtocolUtils::ConvertJson(const string& strYuanJson, int& lineType)
{
	if (strYuanJson.find("{ht") == 0) {
		lineType = 0;
		std::regex tt_reg2(R"(^\{ht[ \t]*:[ \t]*[\"]([0-9-]+)[ \t]*([0-9]+:[0-9]+:[0-9]+):([0-9]+)[\"][}])", std::regex_constants::egrep);
		return std::regex_replace(strYuanJson.c_str(), tt_reg2, R"({"ht" : {"$date":"$1T$2.$3Z"}})");
	}
	else if (strYuanJson.find("{src") == 0) {
		lineType = 1;
		std::regex tt_reg1(R"([ \t]*([a-zA-Z0-9_]+)[ \t]*:)", std::regex_constants::egrep);
		return  std::regex_replace(strYuanJson.c_str(), tt_reg1, "\"$1\":");
	}

	lineType = -1;
	return strYuanJson;
}
stJsonObject* ProtocolUtils::CreateJsonObjectA(bool bCreateSub)
{
	WJS_JsonStream  tt_jns;
	tt_jns.WriteNewKey("key1", 0, false);
	tt_jns << 100;
	tt_jns.WriteNewKey("key2", 0, false);
	tt_jns << true;
	tt_jns.WriteNewKey("key3", 0, false);
	tt_jns << 7.893f;
	tt_jns.WriteNewKey("key4", 0, false);
	tt_jns << 'a';

	if (bCreateSub) {
		stJsonObject* tt_pJsonObject = tt_jns.WriteNewKey("keysub", 2, true);
		tt_jns.AddJsonObject(CreateJsonObjectA(false));
		tt_jns.EnterToChildEnd(tt_pJsonObject);
	}

	stJsonObject*   tt_pJsonObject = tt_jns._pBeginJsonObject;
	tt_jns._pBeginJsonObject = NULL;
	return tt_pJsonObject;
}
string   ProtocolUtils::GetBaseDir()//得到模块路径
{
	CHAR strBuffer[MAX_PATH];
	memset(strBuffer, 0, sizeof(CHAR) * MAX_PATH);

	//得到当前模块的路径
	::GetModuleFileName(NULL, strBuffer, MAX_PATH);

	for (int i = MAX_PATH - 1; i > 0; i--) {
		if (strBuffer[i] != '\\') {
			strBuffer[i] = '\0';
		}
		else {
			break;
		}
	}
	return strBuffer;
}
string         ProtocolUtils::GetBaseDir_RemoveEndGang()
{
	CHAR strBuffer[MAX_PATH];
	memset(strBuffer, 0, sizeof(CHAR) * MAX_PATH);

	//得到当前模块的路径
	::GetModuleFileName(NULL, strBuffer, MAX_PATH);

	for (int i = MAX_PATH - 1; i > 0; i--) {
		if (strBuffer[i] != '\\') {
			strBuffer[i] = '\0';
		}
		else {
			strBuffer[i] = '\0';
			break;
		}
	}
	return strBuffer;
}
string         ProtocolUtils::GetModuleName()
{
	CHAR strBuffer[MAX_PATH];
	memset(strBuffer, 0, sizeof(CHAR) * MAX_PATH);
	//得到当前模块的路径
	::GetModuleFileName(NULL, strBuffer, MAX_PATH);

	return std::move(string(strBuffer));
}

void ProtocolUtils::ConvertToMongoJson(const string& strFileName)
{
	string tt_strPath;
	if (strFileName.find(":\\") != string::npos || strFileName.find(":/") != string::npos) {
		tt_strPath = strFileName;
	}
	else {
		tt_strPath = GetBaseDir();
		tt_strPath.append(strFileName.c_str());
	}


	FILE* fp = fopen(tt_strPath.c_str(), "rt");
	if (!fp)
		return;

	string tt_strPathNew = tt_strPath;
	tt_strPathNew.append("22");

	FILE* fp2 = fopen(tt_strPathNew.c_str(), "wb");
	if (!fp2) {
		fclose(fp);
		return;
	}

	char tt_buffer[2048];
	string tt_strLine1;
	while (fgets(tt_buffer, 2048 - 1, fp) != nullptr) {
		int tt_lineType = -1;
		string tt_strLine = ConvertJson(tt_buffer, tt_lineType);
		if (tt_lineType == 0) {
			size_t tt_pos1 = tt_strLine.rfind("}");
			if (tt_pos1 != string::npos)
				tt_strLine1 = tt_strLine.substr(0, tt_pos1);
			else
				tt_strLine1 = tt_strLine;
			tt_strLine1.append(",");
		}
		else if (tt_lineType == 1) {
			if (tt_strLine1.length() > 0) {
				tt_strLine1.append(tt_strLine.substr(1));
				fwrite(tt_strLine1.c_str(), tt_strLine1.length(), 1, fp2);
				tt_strLine1 = "";
			}
			else {
				fwrite(tt_strLine.c_str(), tt_strLine.length(), 1, fp2);
			}
		}
	}


	fclose(fp2);
	fclose(fp);
	//login_2019_3_12.json
}

//ConvertToMongoJson("login_2019_3_12.json");
////std::regex tt_reg("(_[a-zA-Z0-9]+_\\.)?([\\w.]+)$", std::regex_constants::ECMAScript);

////{src : 17235970, eventsrc: "gw",  eventtype : "loginfail", userid:223613 ,gameid:315}
//std::regex tt_reg1(R"([ \t]*([a-zA-Z0-9_]+)[ \t]*:)", std::regex_constants::egrep);
//string tt_s1 = std::regex_replace(R"({src : 17235970, eventsrc: "gw", eventtype : "loginfail", userid:223613 ,gameid:315})", tt_reg1, "\"$1\":");

////{ht : "2019-03-12 15:28:55:187"}
////{需要转义
//std::regex tt_reg2(R"(^\{ht[ \t]*:[ \t]*[\"]([0-9-]+)[ \t]*([0-9:]+)[\"][}])", std::regex_constants::egrep);
//string tt_s2 = std::regex_replace(R"({ht : "2019-03-12 15:28:55:187"})", tt_reg2, R"({"ht" : {"$date":"$1T$2Z"}})");


//return 0;
//WJS_JsonStream  tt_jns;
//stJsonObject*   tt_pJsonObject = NULL;
//tt_jns.WriteNewKey("key1",0,false);
//tt_jns << 100;
//tt_jns.WriteNewKey("key2", 0, false);
//tt_jns << true;
//tt_jns.WriteNewKey("key3", 0, false);
//tt_jns << 7.893f;
//tt_jns.WriteNewKey("key4", 0, false);
//tt_jns << 'a';
//tt_jns.WriteNewKey("key5", 0, false);
//tt_jns << "goodtime";

//tt_pJsonObject = tt_jns.WriteNewKey("key6", 2, true);
//tt_jns.WriteNewKey("karray1", 1, false);
//stJsonArray*  tt_pArray = tt_jns.CreateJsonArray(2);
//if (tt_pArray)
//{
//	tt_pArray->_pv = tt_jns.CreateJsonValue_String("first");
//	(tt_pArray + 1)->_pv = tt_jns.CreateJsonValue_String("second");

//	tt_jns.WriteArrayValue(tt_pArray);
//}
//tt_jns.EnterToChildEnd(tt_pJsonObject);

//tt_jns.WriteNewKey("key7", 0, false);
//tt_jns << "goodbye";

//tt_pJsonObject = tt_jns.WriteNewKey("key66", 2, true);
//tt_jns.AddJsonObject(CreateJsonObjectA(false));
//tt_jns.EnterToChildEnd(tt_pJsonObject);

//tt_pJsonObject = tt_jns.WriteNewKey("key77", 2, true);
//tt_jns.AddJsonObject(CreateJsonObjectA(true));
//tt_jns.EnterToChildEnd(tt_pJsonObject);

//FILE* fp = fopen("abcd.txt", "wb");
//if (fp)
//{
//	tt_jns.WriteStreamToFile(fp);
//	fclose(fp);
//	return 0;
//}


void    ProtocolUtils::GetAllSubString(const string& strInput, const string& strSplit, vector<string>& _strVec, bool bOnlyOne)//得到所有子串,strSplit为分隔符
{

	string t_strLeft = strInput;
	bool bFoundEnd = false;

	while (!bFoundEnd) {
		string strbefore;
		string strend;
		if (findSubStrAndCut(t_strLeft, strSplit, &strbefore, &strend)) {
			_strVec.push_back(strbefore);
			t_strLeft = strend;
			if (t_strLeft.length() == 0) {
				break;
			}

			if (bOnlyOne) {
				_strVec.push_back(t_strLeft);
				break;
			}
		}
		else {
			bFoundEnd = true;
			if (t_strLeft.length() > 0) {
				_strVec.push_back(t_strLeft);
			}
		}

	}
}

bool    ProtocolUtils::findSubStrAndCut(const string& inputStr, const string& subStr, string* cutStr1, string* cutStr2)//查找子串，并分割,true是找到
{
	size_t index = inputStr.find(subStr.c_str());

	if (index != string::npos) {
		cutStr1->append(inputStr.substr(0, index));
		cutStr2->append(inputStr.substr(index + subStr.length()));

		return true;
	}

	return false;
}
string    ProtocolUtils::GetLine(char* pChar, int& readNum, bool& bReadAllEnd)//取到一行
{
	//bool bFoundEnd=false;
	readNum = 0;
	bReadAllEnd = false;
	string strLine;
	char ch[2];

	while ((*pChar) != '\0'/* && (!bFoundEnd)*/) {
		if ((*pChar) != '\n' && (*pChar) != '\r') {
			ch[0] = *pChar;
			ch[1] = '\0';

			strLine.append(ch);
			pChar++;
			readNum++;
		}
		else {
			//bFoundEnd=true;
			pChar++;
			readNum++;
			break;
		}
	}

	if ((*pChar) == '\0') {
		bReadAllEnd = true;
	}

	return strLine;
}

string    ProtocolUtils::GetLine_4(char* pChar, int& readNum, bool& bReadAllEnd, bool& bNewLine)//取到一行
{
	//bool bFoundEnd=false;
	readNum = 0;
	bReadAllEnd = false;
	string strLine;
	char ch[2];

	while ((*pChar) != '\0'/* && (!bFoundEnd)*/) {
		if ((*pChar) != '\n' && (*pChar) != '\r') {
			ch[0] = *pChar;
			ch[1] = '\0';

			strLine.append(ch);
			pChar++;
			readNum++;
		}
		else {
			if ((*pChar) == '\r')
				bNewLine = true;
			//bFoundEnd=true;
			pChar++;
			readNum++;
			break;
		}
	}

	if ((*pChar) == '\0') {
		bReadAllEnd = true;
	}

	return strLine;
}


size_t    ProtocolUtils::FindString(const string& strYuan, const string& strFind, size_t startIndex)
{
	if (strFind.size() == 0)
		return -1;
	size_t tt_retpos = -1;
	size_t tt_testIndex = 0;
	for (int i = startIndex; i < (int)strYuan.size(); i++) {
		if (strYuan[i] == strFind[tt_testIndex]) {
			tt_testIndex++;
			if (tt_testIndex >= (int)strFind.size()) {
				return i + 1 - strFind.size();
			}
		}
		else {
			//i回到之前
			i -= tt_testIndex;

			tt_testIndex = 0;
		}

	}

	return -1;
}
void    ProtocolUtils::Replace(string& strContext, const string& strYuan, const string& strReplace)
{
	size_t tt_startindex;
	size_t tt_retpos;

	tt_startindex = 0;
	tt_retpos = FindString(strContext, strYuan, tt_startindex);
	while (tt_retpos != string::npos) {
		strContext.replace(tt_retpos, strYuan.size(), strReplace.c_str());
		tt_startindex = tt_retpos + strYuan.size();
		tt_retpos = FindString(strContext, strYuan, tt_startindex);
	}
}
bool   ProtocolUtils::IsContextLine(string& strLine)
{
	for (size_t i = 0; i < strLine.size(); i++) {
		if (strLine[i] != ' ' &&  strLine[i] != '\t' &&  strLine[i] != '\r' &&  strLine[i] != '\n')
			return true;
	}

	return false;
}
bool   ProtocolUtils::IsSpaceFu(char ch)
{
	if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
		return true;

	return false;

}
string ProtocolUtils::FoundStringEndSpace(const string& strLine, int startIndex, int& endIndex)
{
	string tt_str;
	for (int i = startIndex; i < (int)strLine.size(); i++) {
		if (IsSpaceFu(strLine[i])) {
			if (tt_str.length() > 0) {
				endIndex = i + 1;
				return tt_str;
			}
		}
		else {
			tt_str.push_back(strLine[i]);
		}
	}

	endIndex = -1;
	return tt_str;
}
string ProtocolUtils::FoundStringEndSpecifyFuHao(const string& strLine, int startIndex, int& endIndex, char sc, bool bKeepSpace)
{
	string tt_str;
	for (int i = startIndex; i < (int)strLine.size(); i++) {
		if (strLine[i] == sc) {
			endIndex = i + 1;
			return tt_str;
		}

		if (bKeepSpace) {
			tt_str.push_back(strLine[i]);
		}
		else if (!IsSpaceFu(strLine[i])) {
			tt_str.push_back(strLine[i]);
		}
	}

	endIndex = -1;
	return tt_str;
}
string    ProtocolUtils::RemoveLineNote(string& strLine, string& strComment)
{
	//auto tt = string::npos;
	size_t index1 = strLine.find("<!--");
	if (index1 == string::npos)
		return strLine;

	size_t index2 = strLine.find("-->");
	if (index2 == string::npos)
		return strLine;

	//注释里的引号变成^
	for (unsigned int i = index1 + 4; i < index2; i++) {
		if (strLine[i] == '\"') {
			strComment.push_back('^');

		}
		else {
			strComment.push_back(strLine[i]);

		}
	}

	//全角“号换成^^
	ProtocolUtils::Replace(strComment, "“", "^^");
	//ProtocolUtils::Replace(strComment,"”","^^");
	//ProtocolUtils::Replace(strComment,"“","^^");
	//ProtocolUtils::Replace(strComment,"”","^^");

	string tt_strOut;
	if (index1 > 0) {
		tt_strOut = strLine.substr(0, index1);
	}
	tt_strOut.append(strLine.substr(index2 + 3).c_str());
	return tt_strOut;
}

string   ProtocolUtils::InsertComment(string& strLine, const vector<string>& strCommentLines)
{
	if (strCommentLines.size() == 0)
		return strLine;

	string tt_strCommentLine;
	tt_strCommentLine.append(" comment=\"");
	for (unsigned int i = 0; i < strCommentLines.size(); i++) {
		if (i != 0) {
			tt_strCommentLine.append("split");
		}
		tt_strCommentLine.append(strCommentLines[i].c_str());
	}
	tt_strCommentLine.append("\"");

	size_t index = strLine.find("/>");
	if (index != string::npos) {
		strLine.insert(index, tt_strCommentLine.c_str());
		return strLine;
	}

	index = strLine.find("<struct");
	if (index != string::npos) {
		index = strLine.find(">");
		if (index != string::npos) {
			strLine.insert(index, tt_strCommentLine.c_str());
			return strLine;
		}
	}

	index = strLine.find("<enum");
	if (index != string::npos) {
		index = strLine.find(">");
		if (index != string::npos) {
			strLine.insert(index, tt_strCommentLine.c_str());
			return strLine;
		}
	}

	index = strLine.find("<method");
	if (index != string::npos) {
		index = strLine.find(">");
		if (index != string::npos) {
			strLine.insert(index, tt_strCommentLine.c_str());
			return strLine;
		}
	}

	index = strLine.find("<VManger");
	if (index != string::npos) {
		index = strLine.find(">");
		if (index != string::npos) {
			strLine.insert(index, tt_strCommentLine.c_str());
			return strLine;
		}
	}

	index = strLine.find("<PartManager");
	if (index != string::npos) {
		index = strLine.find(">");
		if (index != string::npos) {
			strLine.insert(index, tt_strCommentLine.c_str());
			return strLine;
		}
	}
	
	return strLine;
}
string ProtocolUtils::FoundCommont_AfterLine(const string& strLine, int startIndex)
{
	int    tt_LeftXieNum = 0;
	bool   tt_bFoundComment = false;
	string tt_str;
	for (int i = startIndex; i < (int)strLine.size(); i++) {
		if (!tt_bFoundComment) {
			if (strLine[i] == '/') {
				tt_LeftXieNum++;
			}
			else {
				tt_LeftXieNum = 0;
			}

			if (tt_LeftXieNum >= 2)
				tt_bFoundComment = true;
		}
		else {
			if (tt_str.size() == 0 && strLine[i] == '/')
				continue;
			tt_str.push_back(strLine[i]);
		}
	}

	return tt_str;
}
void   ProtocolUtils::SplitStringByFuncPar(const string& strLine, vector<string>& strPars)
{
	//进一步解析参数，以',' ' ' '&'结束

	string tt_str;
	for (int i = 0; i < (int)strLine.size(); i++) {
		if (strLine[i] == ' ') {
			if (tt_str.length() > 0) {
				strPars.push_back(tt_str);
				tt_str = "";
			}
		}
		else if (strLine[i] == ',') {
			if (tt_str.length() > 0) {
				strPars.push_back(tt_str);
				tt_str = "";
			}

			strPars.push_back(",");
		}
		else if (strLine[i] == '&') {
			if (tt_str.length() > 0) {
				strPars.push_back(tt_str);
				tt_str = "";
			}
			strPars.push_back("&");
		}
		else {
			tt_str.push_back(strLine[i]);
		}
	}

	if (tt_str.length() > 0) {
		strPars.push_back(tt_str);
	}


}
string ProtocolUtils::GetFileMd5(const string& strFileName)
{
	FILE *fp;

	//二进制方式打开
	fp = fopen(strFileName.c_str(), "rb");
	if (!fp)
		return "";

	MD5 tt_md5;
	tt_md5.update(fp);
	tt_md5.finalize();

	string tt_strMd5 = tt_md5.hex_digest();

	fclose(fp);
	return tt_strMd5;
}
bool   ProtocolUtils::TestFileMd5(const string& strFileName, const string& strXMLMd5)
{
	//return false;
	FILE *fp;

	//二进制方式打开
	fp = fopen(strFileName.c_str(), "rb");
	if (!fp)
		return false;

	//得到文件大小
	fseek(fp, 0, SEEK_END);
	long fileLen = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	//空文件
	if (fileLen == 0) {
		fclose(fp);
		return false;
	}

	//读出所有数据
	char* pBuf = new char[fileLen + 1];
	fread(pBuf, fileLen, 1, fp);
	pBuf[fileLen] = '\0';
	fclose(fp);

	char* pOld = pBuf;

	//读第一行
	string strLine;
	int readNum = 0;
	bool tt_bEnd = false;
	strLine = ProtocolUtils::GetLine(pBuf, readNum, tt_bEnd);

	if (strLine.length() > 0) {
		size_t tt_pos = strLine.find("//MD5=");
		if (tt_pos != -1) {
			string tt_sub = strLine.substr(6);
			if (tt_sub.compare(strXMLMd5.c_str()) == 0) {
				delete[]pOld;
				return true;
			}
		}
		else {
			tt_pos = strLine.find("--MD5=");
			if (tt_pos != -1) {
				string tt_sub = strLine.substr(6);
				if (tt_sub.compare(strXMLMd5.c_str()) == 0) {
					delete[]pOld;
					return true;
				}
			}
		}

	}


	delete[]pOld;
	return false;
}

void   ProtocolUtils::CheckDirectoryEixst(const string& strDir)
{
	string tt_str = strDir;
	for (size_t i = 0; i < tt_str.length(); i++) {
		if (tt_str[i] == '/')
			tt_str[i] = '\\';
	}
	string tt_Path;
	string tt_Left;

	while (true) {
		int index = tt_str.find("\\");
		if (index != -1) {
			tt_Left = tt_str.substr(0, index);
			if (tt_Path.length() > 0) {
				tt_Path.append("\\");
				tt_Path.append(tt_Left.c_str());
			}
			else
				tt_Path = tt_Left;

			if (!PathFileExists(tt_Path.c_str())) {
				SECURITY_ATTRIBUTES tt_attr;
				tt_attr.nLength = sizeof(SECURITY_ATTRIBUTES);
				tt_attr.lpSecurityDescriptor = NULL;
				tt_attr.bInheritHandle = TRUE;

				CreateDirectory(tt_Path.c_str(), &tt_attr);
			}

			tt_str = tt_str.substr(index + 1);
		}
		else {
			break;
		}

	}
}
string           ProtocolUtils::GetSuffix(const string& fileName)
{
	size_t tt_pos = fileName.rfind(".");
	if (tt_pos != string::npos) {
		return fileName.substr(tt_pos+1);
	}

	return "";
}
bool           ProtocolUtils::IsHasValidString(const string& line)
{
	for (const auto& r: line) {
		if (r != ' ' && r != '\t' && r != '/' && r != '\r' && r != '\n') {
			return true;
		}
	}
	return false;
}

string         ProtocolUtils::GetBeginString(const string& line)
{
	string t;
	for (const auto& r : line) {
		if (r == ' ' || r == '\t') {
			t.push_back(r);
		}
		else
			break;
	}

	return t;
}

void   ProtocolUtils::RemoveLeftGang(string& line)
{
	size_t t = line.size();
	for (size_t i = 0; i < line.size(); i++) {
		if (line[i] == '/') {
			t = i;
		}
		else
			break;
	}

	if (t != line.size()) {
		line = line.substr(t + 1);
	}
}
