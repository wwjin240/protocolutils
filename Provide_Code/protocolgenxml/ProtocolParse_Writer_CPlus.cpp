#include "ProtocolParse_Writer_CPlus.h"
#include "ProtocolParse.h"
#include <time.h>
#include <sys/timeb.h>
#include <algorithm>

extern string g_strBaseType[];
extern string g_ProgressId;
extern bool g_bCreateCopy;
extern bool g_bCreateCopy2;

struct stTempFuncId {
	string _funcName;
	int    _funcId;

};

struct stFuncIdSort {
	bool operator()(const stTempFuncId& left, const stTempFuncId& right)
	{
		return left._funcName < right._funcName;
	}
};

ProtocolParse_Writer_CPlus::ProtocolParse_Writer_CPlus(const string& strPathName, eCPlusStandardType cplusStandardType, bool bUseStdByte)
	:ProtocolParse_Writer(strPathName, eProtocolWriterType::CPlus)
	,_cplusStandardType(cplusStandardType)
	, _bUseStdByte(bUseStdByte)
{
	if(_bUseStdByte)
	   _memberTypeFullTypeMap.insert(make_pair("byte", "std::byte"));

	_memberTypeFullTypeMap.insert(make_pair("string","std::string"));
	_memberTypeFullTypeMap.insert(make_pair("u16string", "std::u16string"));
	_memberTypeFullTypeMap.insert(make_pair("wstring", "std::wstring"));
	//_memberTypeFullTypeMap.insert(make_pair("wjs_utf8", "std::string"));

	string t_strFastBaseType[] = { "int","unsigned int","short","unsigned short","char","byte","BYTE","bool","BOOL", \
	"DWORD","float","long","unsigned long","__int64","long long" };

	for (int i = 0; i < sizeof(t_strFastBaseType) / sizeof(string); i++) {
		_canFastProcessVectorTypeSet.insert(t_strFastBaseType[i]);
	}

}
ProtocolParse_Writer_CPlus::~ProtocolParse_Writer_CPlus()
{

}

void    ProtocolParse_Writer_CPlus::WriteInfoHead(FILE* fp, bool bLua, bool bPython)//写头
{
	//先写对应XML的MD5
	char tt_ch[512];
	if (bLua)
		sprintf(tt_ch, "--MD5=%s\r\n", m_strXMLMd5.c_str());
	else if (bPython)
		sprintf(tt_ch, "#MD5=%s\r\n", m_strXMLMd5.c_str());
	else
		sprintf(tt_ch, "//MD5=%s\r\n", m_strXMLMd5.c_str());


	string tt_strLine = tt_ch;
	fwrite(tt_strLine.c_str(), tt_strLine.length(), 1, fp);


	ProtocolHead head;
	if (bLua)
		head.SetLua();
	else if (bPython)
		head.SetPython();
	//得到生成时间
	struct tm* today;
	__time64_t long_time;
	_time64(&long_time);
	today = _localtime64(&long_time);

	//struct tm *today;
	//time_t long_time;
	//time( &long_time );		    /* Get time as long integer. */
	//today = localtime( &long_time );    /* Convert to local time. */
	struct _timeb tstruct;
	_ftime(&tstruct);
	char strBuffer[512];
	if (bLua) {
		sprintf(strBuffer, "--protocolgen:[%d-%02d-%02d %02d:%02d:%02d]\r\n"
			, today->tm_year + 1900
			, today->tm_mon + 1
			, today->tm_mday
			, today->tm_hour
			, today->tm_min
			, today->tm_sec
		);
	}
	else if (bPython) {
		sprintf(strBuffer, "#protocolgen:[%d-%02d-%02d %02d:%02d:%02d]\r\n"
			, today->tm_year + 1900
			, today->tm_mon + 1
			, today->tm_mday
			, today->tm_hour
			, today->tm_min
			, today->tm_sec
		);
	}
	else {
		sprintf(strBuffer, "//protocolgen:[%d-%02d-%02d %02d:%02d:%02d]\r\n"
			, today->tm_year + 1900
			, today->tm_mon + 1
			, today->tm_mday
			, today->tm_hour
			, today->tm_min
			, today->tm_sec
		);
	}


	int len = strlen(strBuffer);
	memcpy(head._createtime, strBuffer, len * sizeof(char));
	head._createtime[len] = '\0';

	fwrite(head._cvertion, strlen(head._cvertion), 1, fp);
	fwrite(head._author, strlen(head._author), 1, fp);
	fwrite(head._createtime, strlen(head._createtime), 1, fp);
	fwrite(head._copyInfo, strlen(head._copyInfo), 1, fp);

}
void    ProtocolParse_Writer_CPlus::WriteStartZhongKuoKao(FILE* fp)
{
	char ch[10];
	sprintf(ch, "{\r\n");
	size_t len = strlen(ch);
	fwrite(ch, len, 1, fp);
}

void    ProtocolParse_Writer_CPlus::WriteEndZhongKuoKao(FILE* fp)
{
	char ch[10];
	sprintf(ch, "}\r\n");
	size_t len = strlen(ch);
	fwrite(ch, len, 1, fp);
}


bool    ProtocolParse_Writer_CPlus::CreateFile_Path(string path, int filetype, bool brefprotocol)
{
	return CreateFile_Path_CPlus(path,filetype,brefprotocol);
}

ProtocolParse_Writer* ProtocolParse_Writer_CPlus::Create_RefProtocolParse_Writer(const string& strPathName)
{
	return new ProtocolParse_Writer_CPlus(strPathName,_cplusStandardType, _bUseStdByte);
}

bool    ProtocolParse_Writer_CPlus::CreateFile_Path_CPlus(string path, int filetype, bool brefprotocol)
{
	if (filetype < 0 || filetype>2)
		return false;

	string tt_fullfoldername = path;
	//tt_fullfoldername.append("\\");
	//首先创建文件夹
	CheckDirectoryEixst(tt_fullfoldername.c_str());

	if (m_NamespaceVec.size() > 0) {
		if (brefprotocol) {
			char tt_ch[256];
			sprintf(tt_ch, "%sProtocol.h", m_NamespaceVec[0]._protocolname.c_str());
			m_SaveFileName._servername = tt_ch;
			sprintf(tt_ch, "%sProtocol.h", m_NamespaceVec[0]._protocolname.c_str());
			m_SaveFileName._clientname = tt_ch;
		}
		else {
			char tt_ch[256];
			sprintf(tt_ch, "%sProtocol_server.h", m_NamespaceVec[0]._protocolname.c_str());
			m_SaveFileName._servername = tt_ch;
			sprintf(tt_ch, "%sProtocol_client.h", m_NamespaceVec[0]._protocolname.c_str());
			m_SaveFileName._clientname = tt_ch;
		}

	}


	switch (filetype) {
	case 1://服务器
	{
		for (auto& p : m_pRefProtocolParseList) {
			//主要为了使得id不出现重复
			if (p) {
				p->m_bStdafx = m_bStdafx;
				p->CreateFile_Path(path, filetype, true);
				p->GetRefProtocolMapPar(m_StringIntLogMap, m_IntStringMap, m_StringIntSearchMap);
			}
		}

		string str = path;
		str.append(m_SaveFileName._servername);
		return CreateFile_CPlus(str, filetype, brefprotocol);
	}
	break;
	case 0://客户端
	{
		for (auto& p : m_pRefProtocolParseList) {
			if (p) {
				p->m_bStdafx = m_bStdafx;
				p->CreateFile_Path(path, filetype, true);
				p->GetRefProtocolMapPar(m_StringIntLogMap, m_IntStringMap, m_StringIntSearchMap);
			}
		}

		string str = path;
		str.append(m_SaveFileName._clientname);
		return CreateFile_CPlus(str, filetype, brefprotocol);
	}
	break;
	case 2: //合成
	{
		//主要为了使得id不出现重复
		for (auto& p : m_pRefProtocolParseList) {
			if (p) {
				p->m_bStdafx = m_bStdafx;
				p->CreateFile_Path(path, filetype, true);
				p->GetRefProtocolMapPar(m_StringIntLogMap, m_IntStringMap, m_StringIntSearchMap);
			}
		}

		//先按服务器名存
		string str = path;
		str.append(m_SaveFileName._servername);
		return CreateFile_CPlus(str, filetype, brefprotocol);
	}
	break;
	default:
		return false;
		break;
	}

}

bool    ProtocolParse_Writer_CPlus::CreateFile_CPlus(string filename, int filetype, bool bRefProtocol)
{
	bool tt_bNotSaveFile = false;
	//先读取文件，判定MD5是否相同，相同则不用再生成
	if (ProtocolUtils::TestFileMd5(filename,m_strXMLMd5)) {
		if (bRefProtocol) {
			tt_bNotSaveFile = true;
		}
		else
			return true;
	}

	//setlocale(LC_ALL,"chs");//有效

	string tt_strFileNameCpp;
	size_t tt_pos = filename.rfind(".h");
	if (tt_pos != string::npos) {
		tt_strFileNameCpp = filename.substr(0, tt_pos);
		tt_strFileNameCpp.replace(tt_pos, 2, ".cpp");
	}
	else {
		tt_strFileNameCpp = filename;
		tt_strFileNameCpp.append(".cpp");
	}

	FILE *fp;
	string tt_tempFileName;

	if (tt_bNotSaveFile) {
		tt_tempFileName = filename;
		tt_tempFileName.append(".tmp");
		fp = fopen(tt_tempFileName.c_str(), "w");
	}
	else {
		if (m_bNotSaveToFile)
			return true;

		fp = fopen(filename.c_str(), "w");
	}

	if (!fp)
		return false;

	FILE *fp_CPP = NULL;
	string tt_tempFileNameCpp;
	if (m_bHCPP) {
		if (tt_bNotSaveFile) {
			tt_tempFileNameCpp = tt_strFileNameCpp;
			tt_tempFileNameCpp.append(".tmp");
			fp_CPP = fopen(tt_tempFileNameCpp.c_str(), "w");
		}
		else {
			fp_CPP = fopen(tt_strFileNameCpp.c_str(), "w");
		}

		if (!fp_CPP) {
			fclose(fp);
			return false;
		}
	}

	//

	fseek(fp, 0, SEEK_SET);

	//写信息头
	WriteInfoHead(fp);
	//写文件头
	WriteFileHead_CPlus(fp, filetype);

	if (m_bHCPP) {

		//写命名空间
		for (auto& m : m_NamespaceVec) {
			WriteOneNameSpace_CPlus(m, fp, filetype, 1);
		}

		string tt_strline;
		if (m_bStdafx) {
			tt_strline.append("#include \"stdafx.h\"\r\n");
		}
		char tt_ch[512];
		size_t tt_pos2 = filename.rfind("\\");
		if (tt_pos2 != string::npos) {
			sprintf(tt_ch, "#include \"%s\"\r\n", filename.substr(tt_pos2 + 1).c_str());
		}
		else
			sprintf(tt_ch, "#include \"%s\"\r\n", filename.c_str());
		tt_strline.append(tt_ch);
		fwrite(tt_strline.c_str(), tt_strline.size(), 1, fp_CPP);
		for (auto& m : m_NamespaceVec) {
			WriteOneNameSpace_CPlus(m, fp_CPP, filetype, 2);
		}
	}
	else {
		//写命名空间
		for (auto& m : m_NamespaceVec) {
			WriteOneNameSpace_CPlus(m, fp, filetype, 0);
		}
	}

	///////

	if (m_strConflict.length() > 6) {
		fwrite(m_strConflict.c_str(), m_strConflict.length(), 1, fp);
	}
	string tt_tempstr = "\r\n";
	fwrite(tt_tempstr.c_str(), tt_tempstr.length(), 1, fp);


	if (m_bAndroidPeer) {
		string tt_str = "\r\n#endif\r\n";
		fwrite(tt_str.c_str(), tt_str.length(), 1, fp);
	}

	fclose(fp);
	if (tt_tempFileName.length() > 0) {
		remove(tt_tempFileName.c_str());
	}

	if (m_bHCPP) {
		fclose(fp_CPP);
		if (tt_tempFileNameCpp.length() > 0) {
			remove(tt_tempFileNameCpp.c_str());
		}
	}

	return true;
}
void    ProtocolParse_Writer_CPlus::WriteFileHead_CPlus(FILE* fp, int filetype)
{
	string str;
	if (m_pRefProtocolParseList.size() > 0) {
		char ch[256];
		if (m_bAndroidPeer) {
			if (m_NamespaceVec.size() > 0) {
				sprintf(ch, "#ifndef __%s__\r\n#define __%s__\r\n", m_NamespaceVec[0]._clientname.c_str(), m_NamespaceVec[0]._clientname.c_str());
			}
			else
				sprintf(ch, "#ifndef __%s__\r\n#define __%s__\r\n", m_RefProtocolList[0]._usingspacename.c_str(), m_RefProtocolList[0]._usingspacename.c_str());
			str.append(ch);
			sprintf(ch, "#include <string>\r\n#include <vector>\r\n#include \"WJS_IByteStream.h\"\r\n#include \"WJS_ArrayVector.h\"\r\n");
			str.append(ch);
		}
		else {
			if (m_bOutJson) {
				sprintf(ch, "#pragma once\r\n#include <string>\r\n#include <vector>\r\n#include \"stream/WJS_IByteStream.h\"\r\n#include \"std/WJS_ArrayVector.h\"\r\n#include \"WJS_JsonStream.h\"\r\n");
				str.append(ch);
			}
			else {
				sprintf(ch, "#pragma once\r\n#include <string>\r\n#include <vector>\r\n#include \"stream/WJS_IByteStream.h\"\r\n#include \"std/WJS_ArrayVector.h\"\r\n");
				str.append(ch);
			}



		}


		for (auto& r: m_RefProtocolList) {
			switch (filetype) {
			case 0:
			{
				sprintf(ch, "#include \"%s\"\r\n", r._includeheadnamec.c_str());

			}
			break;
			case 1:
			{
				sprintf(ch, "#include \"%s\"\r\n", r._includeheadnames.c_str());
			}
			break;
			case 2:
			{
				sprintf(ch, "#include \"%s\"\r\n", r._includeheadnames.c_str());
			}
			break;
			default:
				break;
			}

			str.append(ch);
		}


		str.append("using namespace std;\r\n");

		for (auto& r : m_RefProtocolList) {
			sprintf(ch, "using namespace %s;\r\n", r._usingspacename.c_str());
			str.append(ch);
		}

		if (m_bAndroidPeer) {
			str.append("#ifndef _WINDOWS\r\n");
			str.append("#define GNUC_PACKED1 __attribute((aligned (1)))\r\n");
			str.append("#else \r\n");
			str.append("#define GNUC_PACKED1 \r\n");
			str.append("#endif\r\n");
		}



	}
	else {
		if (m_bAndroidPeer) {
			char ch[256];
			if (m_NamespaceVec.size() > 0) {
				switch (filetype) {
				case 0:
					sprintf(ch, "#ifndef __%s__\r\n#define __%s__\r\n", m_NamespaceVec[0]._clientname.c_str(), m_NamespaceVec[0]._clientname.c_str());
					break;
				case 1:
					sprintf(ch, "#ifndef __%s__\r\n#define __%s__\r\n", m_NamespaceVec[0]._servername.c_str(), m_NamespaceVec[0]._servername.c_str());
					break;
				case 2:
					sprintf(ch, "#ifndef __%s__\r\n#define __%s__\r\n", m_NamespaceVec[0]._servername.c_str(), m_NamespaceVec[0]._servername.c_str());
					break;
				default:
					sprintf(ch, "#ifndef __%s__\r\n#define __%s__\r\n", m_NamespaceVec[0]._clientname.c_str(), m_NamespaceVec[0]._clientname.c_str());
					break;
				}
				str.append(ch);
			}
			else {
				sprintf(ch, "#ifndef __%s__\r\n#define __%s__\r\n", m_ProtocolNo._name.c_str(), m_ProtocolNo._name.c_str());
				str.append(ch);
			}



			str.append("#include <string>\r\n#include <vector>\r\n#include \"WJS_IByteStream.h\"\r\n#include \"WJS_ArrayVector.h\" \nusing namespace std;\r\n");

			str.append("#ifndef _WINDOWS\r\n");
			str.append("#define GNUC_PACKED1 __attribute((aligned (1)))\r\n");
			str.append("#else \r\n");
			str.append("#define GNUC_PACKED1 \r\n");
			str.append("#endif\r\n");
		}
		else {
			if (m_bOutJson)
				str = "#pragma once\r\n#include <string>\r\n#include <vector>\r\n#include \"stream/WJS_IByteStream.h\"\r\n#include \"std/WJS_ArrayVector.h\" \r\n#include \"WJS_JsonStream.h\"\nusing namespace std;\r\n";
			else
				str = "#pragma once\r\n#include <string>\r\n#include <vector>\r\n#include \"stream/WJS_IByteStream.h\"\r\n#include \"std/WJS_ArrayVector.h\" \nusing namespace std;\r\n";

		}

	}

	fwrite(str.c_str(), str.length(), 1, fp);
}
bool    ProtocolParse_Writer_CPlus::WriteOneNameSpace_CPlus(stNamespace& pNamespace, FILE* fp, int filetype, int HCPPType)
{
	m_tablenum = 0;

	string t_namespace = pNamespace._servername;

	char ch[256];
	switch (filetype) {
	case 0:
		sprintf(ch, "namespace %s\r\n", pNamespace._clientname.c_str());
		t_namespace = pNamespace._clientname;
		break;
	case 1:
		sprintf(ch, "namespace %s\r\n", pNamespace._servername.c_str());
		break;
	case 2:
		sprintf(ch, "namespace %s\r\n", pNamespace._servername.c_str());
		break;
	default:
		break;
	}

	//sprintf(ch,"namespace %s\r\n",pNamespace._name.c_str());
	size_t len = strlen(ch);
	fwrite(ch, len, 1, fp);

	//写开始的中括号{
	WriteStartZhongKuoKao(fp);

	//写函数的id
	if (HCPPType == 0 || HCPPType == 1)
		CreateAndWriteFuncId_CPlus(pNamespace, fp, m_tablenum + 1);

	//写协议号
	if (HCPPType == 0 || HCPPType == 1)
		WriteProtocolNo_CPlus(pNamespace, fp, m_tablenum + 1);

	//先写枚举
	if (HCPPType == 0 || HCPPType == 1) {
		for (auto& r : pNamespace._EnumVec) {
			WriteEnum_CPlus(r, fp, m_tablenum + 1);
		}
	}


	////////需要考虑顺序,按顺序插着写
	//结构体和vector型大小
	int totalsize = pNamespace._StructVec.size() + pNamespace._VectorVec.size() + pNamespace._WJSVectorVec.size();
	int structindex = 0;
	int vectorindex = 0;
	int wjsvectorindex = 0;
	bool bstructend = false;
	bool bvectorend = false;
	bool bwjsvectorend = false;

	if (pNamespace._StructVec.size() == 0) {
		bstructend = true;
	}
	if (pNamespace._VectorVec.size() == 0) {
		bvectorend = true;
	}
	if (pNamespace._WJSVectorVec.size() == 0) {
		bwjsvectorend = true;
	}

	for (int i = 0; i < totalsize; i++) {
		if ((!bvectorend) && (!bstructend) && (!bwjsvectorend)) {
			if (pNamespace._StructVec[structindex]._index < pNamespace._VectorVec[vectorindex]._index
				&& (pNamespace._StructVec[structindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index)) {
				//写结构体
				WriteStruct_CPlus(t_namespace,pNamespace._StructVec[structindex], fp, m_tablenum + 1, HCPPType);
				structindex++;
			}
			else if (pNamespace._VectorVec[vectorindex]._index < pNamespace._StructVec[structindex]._index
				&& (pNamespace._VectorVec[vectorindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index)) {
				//写vector
				WriteVector_CPlus(pNamespace._VectorVec[vectorindex], fp, m_tablenum + 1, HCPPType);
				vectorindex++;
			}
			else {
				//写类vector
				WriteWJSVector_CPlus(pNamespace._WJSVectorVec[wjsvectorindex], fp, m_tablenum + 1, HCPPType);
				wjsvectorindex++;
			}
		}
		else if (!bvectorend && (!bstructend)) {
			if (pNamespace._StructVec[structindex]._index < pNamespace._VectorVec[vectorindex]._index) {
				//写结构体
				WriteStruct_CPlus(t_namespace,pNamespace._StructVec[structindex], fp, m_tablenum + 1, HCPPType);
				structindex++;
			}
			else {
				//写vector
				WriteVector_CPlus(pNamespace._VectorVec[vectorindex], fp, m_tablenum + 1, HCPPType);
				vectorindex++;
			}
		}
		else if (!bvectorend && (!bwjsvectorend)) {
			if (pNamespace._VectorVec[vectorindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index) {
				//写vector
				WriteVector_CPlus(pNamespace._VectorVec[vectorindex], fp, m_tablenum + 1, HCPPType);
				vectorindex++;
			}
			else {
				//写类vector
				WriteWJSVector_CPlus(pNamespace._WJSVectorVec[wjsvectorindex], fp, m_tablenum + 1, HCPPType);
				wjsvectorindex++;
			}
		}
		else if ((!bstructend) && (!bwjsvectorend)) {
			if (pNamespace._StructVec[structindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index) {
				//写结构体
				WriteStruct_CPlus(t_namespace,pNamespace._StructVec[structindex], fp, m_tablenum + 1, HCPPType);
				structindex++;
			}
			else {
				//写类vector
				WriteWJSVector_CPlus(pNamespace._WJSVectorVec[wjsvectorindex], fp, m_tablenum + 1, HCPPType);
				wjsvectorindex++;
			}
		}
		else if (!bvectorend) {
			//写vector
			WriteVector_CPlus(pNamespace._VectorVec[vectorindex], fp, m_tablenum + 1, HCPPType);
			vectorindex++;

		}
		else if (!bstructend) {
			//写结构体
			WriteStruct_CPlus(t_namespace,pNamespace._StructVec[structindex], fp, m_tablenum + 1, HCPPType);
			structindex++;
		}
		else if (!bwjsvectorend) {
			//写类vector
			WriteWJSVector_CPlus(pNamespace._WJSVectorVec[wjsvectorindex], fp, m_tablenum + 1, HCPPType);
			wjsvectorindex++;
		}

		if (vectorindex >= (int)pNamespace._VectorVec.size()) {
			bvectorend = true;
		}
		if (structindex >= (int)pNamespace._StructVec.size()) {
			bstructend = true;
		}
		if (wjsvectorindex >= (int)pNamespace._WJSVectorVec.size()) {
			bwjsvectorend = true;
		}
	}

	if (filetype == 2 && pNamespace._ClassVec.size() > 1) {
		vector<stClass>  tt_bakClassList = pNamespace._ClassVec;
		for (size_t i = 1; i < pNamespace._ClassVec.size(); i++) {
			for (auto& m : pNamespace._ClassVec[i]._FuncVec) {
				m._strSCClassName = pNamespace._ClassVec[i]._name;
				pNamespace._ClassVec[0]._FuncVec.push_back(m);
			}
		}
		while (pNamespace._ClassVec.size() > 1) {
			pNamespace._ClassVec.pop_back();
		}

		for (auto& m : pNamespace._ClassVec[0]._FuncVec) {
			if (!m._bSCFlag) {
				m._bDiscard = true;
			}
		}

		WriteClass_CPlus(pNamespace._ClassVec[0], fp, m_tablenum + 1, filetype, pNamespace._protocolname, pNamespace, HCPPType);

		pNamespace._ClassVec.clear();
		pNamespace._ClassVec = tt_bakClassList;

	}
	else if (filetype == 1 && pNamespace._ClassVec.size() > 1) {
		vector<stClass>  tt_bakClassList = pNamespace._ClassVec;
		int tt_fucsize = pNamespace._ClassVec[0]._FuncVec.size();
		for (size_t i = 1; i < pNamespace._ClassVec.size(); i++) {
			for (auto& r : pNamespace._ClassVec[i]._FuncVec) {
				if (r._bSCFlag) {
					pNamespace._ClassVec[0]._FuncVec.push_back(r);
					pNamespace._ClassVec[0]._FuncVec.back()._strSCClassName = pNamespace._ClassVec[i]._name;
				}
			}
		}

		for (int m = 0; m < tt_fucsize; m++) {
			if (!pNamespace._ClassVec[0]._FuncVec[m]._bSCFlag)
				continue;
			for (size_t i = 1; i < pNamespace._ClassVec.size(); i++) {
				pNamespace._ClassVec[i]._FuncVec.push_back(pNamespace._ClassVec[0]._FuncVec[m]);
				pNamespace._ClassVec[i]._FuncVec.back()._strSCClassName = pNamespace._ClassVec[0]._name;

			}
		}



		//写类
		for (auto& r : pNamespace._ClassVec) {
			WriteClass_CPlus(r, fp, m_tablenum + 1, filetype, pNamespace._protocolname, pNamespace, HCPPType);
		}

		pNamespace._ClassVec.clear();
		pNamespace._ClassVec = tt_bakClassList;
	}
	else {
		//写类
		for (auto& r : pNamespace._ClassVec) {
			WriteClass_CPlus(r, fp, m_tablenum + 1, filetype, pNamespace._protocolname, pNamespace, HCPPType);
		}
	}


	//写结束的中括号}
	WriteEndZhongKuoKao(fp);
	return true;
}
void    ProtocolParse_Writer_CPlus::CreateAndWriteFuncId_CPlus(stNamespace& pNamespace, FILE* fp, int tablenum)
{
	//没有类是不用写方法的
	if (pNamespace._ClassVec.size() == 0)
		return;

	char t_ch[512];
	string strTable = CreateTableString(tablenum);  //写表格符
	string strTable_2 = CreateTableString(tablenum + 1);

	string strFuncId;

	strFuncId.append(strTable);
	strFuncId.append("namespace METHOD\r\n");
	strFuncId.append(strTable);
	strFuncId.append("{\r\n");



	vector<stTempFuncId> tt_TempFuncIdList;

	for (size_t i = 0; i < pNamespace._ClassVec.size(); i++) {
		stTempFuncId tt_TempFuncId;
		for (size_t j = 0; j < pNamespace._ClassVec[i]._FuncVec.size(); j++) {
			strFuncId.append(strTable_2);
			strFuncId.append("const int ");
			sprintf(t_ch, "ID_%s_%s", pNamespace._ClassVec[i]._name.c_str(), pNamespace._ClassVec[i]._FuncVec[j]._name.c_str());
			strFuncId.append(t_ch);

			if (m_bWriteSortFuncId)
				tt_TempFuncId._funcName = t_ch;

			int tempId;  //将字符串转成id
			if (pNamespace._ClassVec[i]._FuncVec[j]._userFuncId != 0)
				tempId = pNamespace._ClassVec[i]._FuncVec[j]._userFuncId;
			else
				tempId = CovertStringToInt(t_ch, pNamespace._ClassVec[i]._FuncVec[j]._bNew);
			sprintf(t_ch, "=%d;\r\n", tempId);
			strFuncId.append(t_ch);

			if (m_bWriteSortFuncId) {
				tt_TempFuncId._funcId = tempId;
				tt_TempFuncIdList.push_back(tt_TempFuncId);
			}
		}
	}

	if (m_bWriteSortFuncId && tt_TempFuncIdList.size() > 0) {
		std::sort(tt_TempFuncIdList.begin(), tt_TempFuncIdList.end(), stFuncIdSort());
		for (auto& r : tt_TempFuncIdList) {
			strFuncId.append(strTable_2);
			strFuncId.append("//");
			strFuncId.append(r._funcName.c_str());
			sprintf(t_ch, "=%d;\r\n", r._funcId);
			strFuncId.append(t_ch);
		}
	}

	strFuncId.append(strTable);
	strFuncId.append("}\r\n");

	fwrite(strFuncId.c_str(), strFuncId.length(), 1, fp);
}
void    ProtocolParse_Writer_CPlus::WriteProtocolNo_CPlus(stNamespace& pNamespace, FILE* fp, int tablenum)
{
	char t_ch[512];
	string strTable = CreateTableString(tablenum);

	string strProtocolNo;
	strProtocolNo.append(strTable);

	if (m_ProtocolNo._value != 1) {
		sprintf(t_ch, "const int %s=%d;\r\n", m_ProtocolNo._name.c_str(), m_ProtocolNo._value);
		strProtocolNo.append(t_ch);
	}
	else {
		strProtocolNo.append("\r\n");
	}

	///////////////再写协议号
	//strProtocolNo.append(strTable);
	//sprintf(t_ch,"const int %s_versionno = %d;\r\n",pNamespace._protocolname.c_str(),m_VersionManger._protocolVersion);
	//strProtocolNo.append(t_ch);


	fwrite(strProtocolNo.c_str(), strProtocolNo.length(), 1, fp);


}
void    ProtocolParse_Writer_CPlus::WriteEnum_CPlus(stEnum& pEnum, FILE* fp, int tablenum)//tablenum代数制表符数
{
	char t_ch[512];
	string strTable = CreateTableString(tablenum);
	string strTable_2 = CreateTableString(tablenum + 1);
	string strTable_3 = CreateTableString(tablenum + 3);
	string strEnum;

	strEnum.append(WriteBlockCommentBegin_CPlus(strTable));
	strEnum.append(WriteBlockCommentBrief_CPlus(strTable, pEnum._comment));
	for (size_t i = 0; i < pEnum._ParNameList.size(); i++) {
		strEnum.append(WriteBlockCommentPar_CPlus(strTable, pEnum._commentList[i], pEnum._ParNameList[i]).c_str());
	}
	strEnum.append(WriteBlockCommentEnd_CPlus(strTable));

	strEnum.append(strTable);
	if (pEnum._bNewVersion) {
		sprintf(t_ch, "enum class %s\r\n", pEnum._name.c_str());
		strEnum.append(t_ch);
	}
	else {
		sprintf(t_ch, "enum %s\r\n", pEnum._name.c_str());
		strEnum.append(t_ch);
	}


	strEnum.append(strTable);
	strEnum.append("{\r\n");

	for (size_t i = 0; i < pEnum._ParNameList.size(); i++) {
		strEnum.append(strTable_2);
		if (i == 0) {
			sprintf(t_ch, "%s = 0,\r\n", pEnum._ParNameList[i].c_str());
		}
		else if (i != pEnum._ParNameList.size() - 1) {
			sprintf(t_ch, "%s,\r\n", pEnum._ParNameList[i].c_str());
		}
		else {
			sprintf(t_ch, "%s\r\n", pEnum._ParNameList[i].c_str());
		}
		strEnum.append(t_ch);
	}
	strEnum.append(strTable);
	strEnum.append("};\r\n");

	fwrite(strEnum.c_str(), strEnum.length(), 1, fp);


	strEnum = "";

	//输入流的实现
	strEnum.append(strTable);

	sprintf(t_ch, "inline WJS_InByteStream& operator << (WJS_InByteStream& ins,const %s& v)\r\n", pEnum._name.c_str());
	strEnum.append(t_ch);

	strEnum.append(strTable);
	strEnum.append("{\r\n");

	strEnum.append(strTable_2);
	sprintf(t_ch, "switch(v)\r\n");
	strEnum.append(t_ch);

	strEnum.append(strTable_2);
	strEnum.append("{\r\n");

	int tt_0 = 0;
	for (size_t i = 0; i < pEnum._ParNameList.size(); i++) {
		if (pEnum._bNewVersion && (!pEnum._keepOldList[i])) {
			strEnum.append(strTable_2);
			sprintf(t_ch, "case %s::%s:\r\n", pEnum._name.c_str(), pEnum._ParNameList[i].c_str());
			strEnum.append(t_ch);

			strEnum.append(strTable_3);
			sprintf(t_ch, "ins<<%d;\r\n", i);
			strEnum.append(t_ch);

			strEnum.append(strTable_3);
			strEnum.append("break;\r\n");
		}
		else {
			if (pEnum._bNewVersion) {
				strEnum.append(strTable_2);
				sprintf(t_ch, "case %s::%s:\r\n", pEnum._name.c_str(), pEnum._ParNameList[i].c_str());
				strEnum.append(t_ch);
			}
			else {
				strEnum.append(strTable_2);
				sprintf(t_ch, "case %s:\r\n", pEnum._ParNameList[i].c_str());
				strEnum.append(t_ch);
			}


			strEnum.append(strTable_3);
			if (i == 0) {
				tt_0 = CovertStringToInt(pEnum._ParNameList[i]);
				sprintf(t_ch, "ins<<%d;\r\n", tt_0);
			}
			else {
				sprintf(t_ch, "ins<<%d;\r\n", CovertStringToInt(pEnum._ParNameList[i]));
			}

			strEnum.append(t_ch);

			strEnum.append(strTable_3);
			strEnum.append("break;\r\n");
		}

	}

	if (pEnum._ParNameList.size() > 0) {
		strEnum.append(strTable_2);
		strEnum.append("default:\r\n");

		strEnum.append(strTable_3);
		sprintf(t_ch, "ins<<%d;\r\n", tt_0);
		strEnum.append(t_ch);

		strEnum.append(strTable_3);
		strEnum.append("break;\r\n");
	}

	strEnum.append(strTable_2);
	strEnum.append("}\r\n");

	strEnum.append(strTable_2);
	strEnum.append("return ins;\r\n");


	strEnum.append(strTable);
	strEnum.append("}\r\n");

	fwrite(strEnum.c_str(), strEnum.length(), 1, fp);


	//输出流
	strEnum = "";
	strEnum.append(strTable);

	sprintf(t_ch, "inline WJS_OutByteStream& operator >> (WJS_OutByteStream& out, %s& v)\r\n", pEnum._name.c_str());
	strEnum.append(t_ch);

	strEnum.append(strTable);
	strEnum.append("{\r\n");

	strEnum.append(strTable_2);
	strEnum.append("int m=0;\r\n");
	strEnum.append(strTable_2);
	strEnum.append("out >> m;\r\n");

	strEnum.append(strTable_2);
	sprintf(t_ch, "switch(m)\r\n");
	strEnum.append(t_ch);

	strEnum.append(strTable_2);
	strEnum.append("{\r\n");


	for (size_t i = 0; i < pEnum._ParNameList.size(); i++) {
		if (pEnum._bNewVersion && (!pEnum._keepOldList[i])) {
			strEnum.append(strTable_2);
			sprintf(t_ch, "case %d:\r\n", i);
			strEnum.append(t_ch);

			strEnum.append(strTable_3);
			sprintf(t_ch, "v=%s::%s;\r\n", pEnum._name.c_str(), pEnum._ParNameList[i].c_str());
			strEnum.append(t_ch);

			strEnum.append(strTable_3);
			strEnum.append("break;\r\n");
		}
		else {
			strEnum.append(strTable_2);
			sprintf(t_ch, "case %d:\r\n", GetExistStringToIntValue(pEnum._ParNameList[i]));
			strEnum.append(t_ch);

			if (pEnum._bNewVersion) {
				strEnum.append(strTable_3);
				sprintf(t_ch, "v=%s::%s;\r\n", pEnum._name.c_str(), pEnum._ParNameList[i].c_str());
				strEnum.append(t_ch);
			}
			else {
				strEnum.append(strTable_3);
				sprintf(t_ch, "v=%s;\r\n", pEnum._ParNameList[i].c_str());
				strEnum.append(t_ch);
			}


			strEnum.append(strTable_3);
			strEnum.append("break;\r\n");
		}

	}



	if (pEnum._ParNameList.size() > 0) {
		strEnum.append(strTable_2);
		strEnum.append("default:\r\n");

		if (pEnum._bNewVersion) {
			strEnum.append(strTable_3);
			sprintf(t_ch, "v=%s::%s;\r\n", pEnum._name.c_str(), pEnum._ParNameList[0].c_str());
			strEnum.append(t_ch);
		}
		else {
			strEnum.append(strTable_3);
			sprintf(t_ch, "v=%s;\r\n", pEnum._ParNameList[0].c_str());
			strEnum.append(t_ch);
		}


		strEnum.append(strTable_3);
		strEnum.append("break;\r\n");
	}

	strEnum.append(strTable_2);
	strEnum.append("}\r\n");

	strEnum.append(strTable_2);
	strEnum.append("return out;\r\n");


	strEnum.append(strTable);
	strEnum.append("}\r\n");

	fwrite(strEnum.c_str(), strEnum.length(), 1, fp);


	//输出流
	strEnum = "";
	strEnum.append(strTable);

	sprintf(t_ch, "inline WJS_OutByteStream2& operator >> (WJS_OutByteStream2& out, %s& v)\r\n", pEnum._name.c_str());
	strEnum.append(t_ch);

	strEnum.append(strTable);
	strEnum.append("{\r\n");

	strEnum.append(strTable_2);
	strEnum.append("int m=0;\r\n");
	strEnum.append(strTable_2);
	strEnum.append("out >> m;\r\n");

	strEnum.append(strTable_2);
	sprintf(t_ch, "switch(m)\r\n");
	strEnum.append(t_ch);

	strEnum.append(strTable_2);
	strEnum.append("{\r\n");

	for (size_t i = 0; i < pEnum._ParNameList.size(); i++) {
		if (pEnum._bNewVersion && (!pEnum._keepOldList[i])) {
			strEnum.append(strTable_2);
			sprintf(t_ch, "case %d:\r\n", i);
			strEnum.append(t_ch);

			strEnum.append(strTable_3);
			sprintf(t_ch, "v=%s::%s;\r\n", pEnum._name.c_str(), pEnum._ParNameList[i].c_str());
			strEnum.append(t_ch);

			strEnum.append(strTable_3);
			strEnum.append("break;\r\n");
		}
		else {
			strEnum.append(strTable_2);
			sprintf(t_ch, "case %d:\r\n", GetExistStringToIntValue(pEnum._ParNameList[i]));
			strEnum.append(t_ch);

			if (pEnum._bNewVersion) {
				strEnum.append(strTable_3);
				sprintf(t_ch, "v=%s::%s;\r\n", pEnum._name.c_str(), pEnum._ParNameList[i].c_str());
				strEnum.append(t_ch);
			}
			else {
				strEnum.append(strTable_3);
				sprintf(t_ch, "v=%s;\r\n", pEnum._ParNameList[i].c_str());
				strEnum.append(t_ch);
			}


			strEnum.append(strTable_3);
			strEnum.append("break;\r\n");
		}

	}


	if (pEnum._ParNameList.size() > 0) {
		//WJS_BBSTreeNode<string, int>* pNode = m_StringIntSearchMap.Search(pEnum._ParNameList[0]);
		//int temp = 0;
		//if (pNode)
		//{
		//	temp = pNode->Second();
		//}
		strEnum.append(strTable_2);
		strEnum.append("default:\r\n");

		if (pEnum._bNewVersion) {
			strEnum.append(strTable_3);
			sprintf(t_ch, "v=%s::%s;\r\n", pEnum._name.c_str(), pEnum._ParNameList[0].c_str());
			strEnum.append(t_ch);
		}
		else {
			strEnum.append(strTable_3);
			sprintf(t_ch, "v=%s;\r\n", pEnum._ParNameList[0].c_str());
			strEnum.append(t_ch);
		}


		strEnum.append(strTable_3);
		strEnum.append("break;\r\n");
	}

	strEnum.append(strTable_2);
	strEnum.append("}\r\n");

	strEnum.append(strTable_2);
	strEnum.append("return out;\r\n");


	strEnum.append(strTable);
	strEnum.append("}\r\n");

	fwrite(strEnum.c_str(), strEnum.length(), 1, fp);

}
string  ProtocolParse_Writer_CPlus::WriteJsonStream_Array(int tablenum, const string& strParName, const string& strParType, const string& strMember, bool bCreateArrayPointer)
{
	char t_ch[512];
	string strTable_2 = CreateTableString(tablenum + 0);
	string strTable_3 = CreateTableString(tablenum + 1);
	string strTable_4 = CreateTableString(tablenum + 2);
	string strTable_5 = CreateTableString(tablenum + 3);

	string strStruct;

	strStruct.append(strTable_2);
	sprintf(t_ch, "tt_jns.WriteNewKey(\"%s\", 1, false);\r\n", strParName.c_str());
	strStruct.append(t_ch);
	strStruct.append(strTable_2);
	if (bCreateArrayPointer)
		strStruct.append("stJsonArray*  tt_pArray = tt_jns.CreateJsonArray(2);{\r\n");
	else
		strStruct.append("tt_pArray = tt_jns.CreateJsonArray(2);{\r\n");

	strStruct.append(strTable_3);
	strStruct.append("if (tt_pArray){\r\n");
	strStruct.append(strTable_4);
	sprintf(t_ch, "tt_pArray->_pv = tt_jns.CreateJsonValue_String(\"%s\");\r\n", strParType.c_str());
	strStruct.append(t_ch);
	strStruct.append(strTable_4);
	if (strMember.length() > 0)
		sprintf(t_ch, "(tt_pArray + 1)->_pv = tt_jns.CreateJsonValue_String(tt_jns.ToString(%s.%s.size()));\r\n", strMember.c_str(), strParName.c_str());
	else
		sprintf(t_ch, "(tt_pArray + 1)->_pv = tt_jns.CreateJsonValue_String(tt_jns.ToString(%s.size()));\r\n", strParName.c_str());

	strStruct.append(t_ch);
	strStruct.append(strTable_4);
	strStruct.append("tt_jns.WriteArrayValue(tt_pArray);\r\n");
	strStruct.append(strTable_3);
	strStruct.append("}\r\n");
	strStruct.append(strTable_2);
	strStruct.append("}\r\n");


	return strStruct;
}
string  ProtocolParse_Writer_CPlus::WriteJsonStream_Array0ToObject(int tablenum, const string& strParName, const string& strParType, const string& strMember)
{
	char t_ch[512];
	string strTable_3 = CreateTableString(tablenum + 0);
	string strTable_4 = CreateTableString(tablenum + 1);
	string strTable_5 = CreateTableString(tablenum + 2);

	string strStruct;

	strStruct.append(strTable_3);
	sprintf(t_ch, "tt_pJsonObject = tt_jns.WriteNewKey(\"%s_d0\", 2, true);\r\n", strParName.c_str());
	strStruct.append(t_ch);

	strStruct.append(strTable_3);
	if (strMember.length() > 0) {
		if (IsExType_enum(strParType))
			sprintf(t_ch, "tt_jns<<(int)%s.%s[0];\r\n", strMember.c_str(), strParName.c_str());
		else
			sprintf(t_ch, "tt_jns<<%s.%s[0];\r\n", strMember.c_str(), strParName.c_str());
	}

	else
		sprintf(t_ch, "tt_jns<<%s[0];\r\n", strParName.c_str());

	strStruct.append(t_ch);

	strStruct.append(strTable_3);
	strStruct.append("tt_jns.EnterToChildEnd(tt_pJsonObject);\r\n");

	return strStruct;
}
void    ProtocolParse_Writer_CPlus::WriteStruct_JsonStream_CPlus(const string& rNameSpace, stStructStruct& pStruct, FILE* fp, int tablenum, int HCPPType)
{
	char t_ch[512];
	string strTable = CreateTableString(tablenum);
	string strTable_2 = CreateTableString(tablenum + 1);
	string strTable_3 = CreateTableString(tablenum + 2);
	string strTable_4 = CreateTableString(tablenum + 3);
	string strTable_5 = CreateTableString(tablenum + 4);

	string strStruct;
	strStruct.append(strTable);

	if (HCPPType == 0) {
		sprintf(t_ch, "inline WJS_JsonStream& operator << (WJS_JsonStream& jns,const %s& v)\r\n", pStruct._name.c_str());
		strStruct.append(t_ch);
	}
	else if (HCPPType == 1) {
		sprintf(t_ch, "extern WJS_JsonStream& operator << (WJS_JsonStream& jns,const %s& v);\r\n", pStruct._name.c_str());
		strStruct.append(t_ch);
	}
	else if (HCPPType == 2) {
		sprintf(t_ch, "WJS_JsonStream& operator << (WJS_JsonStream& jns,const %s& v)\r\n", pStruct._name.c_str());
		strStruct.append(t_ch);
	}

	fwrite(strStruct.c_str(), strStruct.length(), 1, fp);
	strStruct = "";

	if (HCPPType == 0 || HCPPType == 2) {
		strStruct.append(strTable);
		strStruct.append("{\r\n");

		bool tt_bCreatedJsonObject = false;
		strStruct.append(strTable_2);
		strStruct.append("WJS_JsonStream  tt_jns;\r\n");

		if (pStruct._inherited.length() > 0) {
			strStruct.append(strTable_2);
			sprintf(t_ch, "%s::operator <<(tt_jns,(const %s&)v);\r\n", rNameSpace.c_str(), pStruct._inherited.c_str());
			strStruct.append(t_ch);
		}
		

		//PtDoubleColorCfg:: << (tt_jns, (PtDoubleColorCfg)v);

		bool tt_bCreateArrayPointer = true;
		for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
			string tt_strNameSpace;
			if (IsExType_baseVector(pStruct._ParVec[i]._strParType, 0, tt_strNameSpace)) {
				strStruct.append(WriteJsonStream_Array(tablenum + 1, pStruct._ParVec[i]._strParName, pStruct._ParVec[i]._strParType, "v", tt_bCreateArrayPointer).c_str());
				tt_bCreateArrayPointer = false;
			}
			else if (IsExType_vector(pStruct._ParVec[i]._strParType)) {
				strStruct.append(WriteJsonStream_Array(tablenum + 1, pStruct._ParVec[i]._strParName, pStruct._ParVec[i]._strParType, "v", tt_bCreateArrayPointer).c_str());
				tt_bCreateArrayPointer = false;
				if (!tt_bCreatedJsonObject) {
					tt_bCreatedJsonObject = true;
					strStruct.append(strTable_2);
					strStruct.append("stJsonObject*   tt_pJsonObject = NULL;\r\n");
				}

				strStruct.append(strTable_2);
				sprintf(t_ch, "if(v.%s.size()>0){\r\n", pStruct._ParVec[i]._strParName.c_str());
				strStruct.append(t_ch);

				strStruct.append(WriteJsonStream_Array0ToObject(tablenum + 2, pStruct._ParVec[i]._strParName, pStruct._ParVec[i]._strParType, "v").c_str());

				strStruct.append(strTable_2);
				strStruct.append("}\r\n");
			}
			else if (IsBaseType(pStruct._ParVec[i]._strParType)) {
				strStruct.append(strTable_2);
				sprintf(t_ch, "tt_jns.WriteNewKey(\"%s\", 0, false);\r\n", pStruct._ParVec[i]._strParName.c_str());
				strStruct.append(t_ch);

				strStruct.append(strTable_2);
				sprintf(t_ch, "tt_jns<<v.%s;\r\n", pStruct._ParVec[i]._strParName.c_str());
				strStruct.append(t_ch);
			}
			else //新结构体
			{
				if (!tt_bCreatedJsonObject) {
					tt_bCreatedJsonObject = true;
					strStruct.append(strTable_2);
					strStruct.append("stJsonObject*   tt_pJsonObject = NULL;\r\n");
				}
				strStruct.append(strTable_2);
				sprintf(t_ch, "tt_pJsonObject = tt_jns.WriteNewKey(\"%s\", 2, true);\r\n", pStruct._ParVec[i]._strParName.c_str());
				strStruct.append(t_ch);

				strStruct.append(strTable_2);
				if (IsExType_enum(pStruct._ParVec[i]._strParType))
					sprintf(t_ch, "tt_jns<<(int)v.%s;\r\n", pStruct._ParVec[i]._strParName.c_str());
				else
					sprintf(t_ch, "tt_jns<<v.%s;\r\n", pStruct._ParVec[i]._strParName.c_str());

				strStruct.append(t_ch);

				strStruct.append(strTable_2);
				strStruct.append("tt_jns.EnterToChildEnd(tt_pJsonObject);\r\n");
			}
		}

		strStruct.append(strTable_2);
		strStruct.append("jns.AppendJsonStream(tt_jns);\r\n");

		strStruct.append(strTable_2);
		strStruct.append("return jns;\r\n");

		strStruct.append(strTable);
		strStruct.append("}\r\n");
		fwrite(strStruct.c_str(), strStruct.length(), 1, fp);
	}

}
void    ProtocolParse_Writer_CPlus::WriteStruct_CPlus(const string& rNameSpace,stStructStruct& pStruct, FILE* fp, int tablenum, int HCPPType)
{
	if (pStruct._name.compare(pStruct._oldname) == 0)
		return;
	char t_ch[512];
	string strTable = CreateTableString(tablenum);
	string strTable_2 = CreateTableString(tablenum + 1);
	string strTable_3 = CreateTableString(tablenum + 2);
	string strTable_4 = CreateTableString(tablenum + 3);
	string strTable_5 = CreateTableString(tablenum + 4);

	string strStruct;

	if (m_bAndroidPeer) {
		strStruct.append(strTable);
		strStruct.append("#pragma pack(4)\r\n");
	}
	else {
		strStruct.append(strTable);
		strStruct.append("#pragma pack(push)\r\n");
		strStruct.append(strTable);
		strStruct.append("#pragma pack(1)\r\n");
	}

	strStruct.append(WriteBlockCommentBegin_CPlus(strTable));
	strStruct.append(WriteBlockCommentBrief_CPlus(strTable, pStruct._comment));
	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		strStruct.append(WriteBlockCommentPar_CPlus(strTable, pStruct._ParVec[i]._comment, pStruct._ParVec[i]._strParName).c_str());
	}
	strStruct.append(WriteBlockCommentEnd_CPlus(strTable));


	strStruct.append(strTable);
	if (pStruct._inherited.length() > 0) {
		sprintf(t_ch, "struct %s : public %s\r\n", pStruct._name.c_str(), pStruct._inherited.c_str());
	}
	else {
		sprintf(t_ch, "struct %s\r\n", pStruct._name.c_str());
	}
	
	strStruct.append(t_ch);

	strStruct.append(strTable);
	strStruct.append("{\r\n");

	for (auto& r : pStruct._ParVec) {
		strStruct.append(strTable_2);
		sprintf(t_ch, "%s %s;\r\n", GetMemberTypeFullType_CPlus(r._strParType).c_str(), r._strParName.c_str());
		strStruct.append(t_ch);
	}

	if (pStruct._bWithConstruct) {
		strStruct.append(strTable_2);
		sprintf(t_ch, "%s()\r\n", pStruct._name.c_str());
		strStruct.append(t_ch);
		strStruct.append(strTable_2);
		strStruct.append("{\r\n");

		for (auto& r : pStruct._ParVec) {
			if (r._bWithDefault) {
				strStruct.append(strTable_3);
				sprintf(t_ch, "%s=%s;\r\n", r._strParName.c_str(), r._strDefaultValue.c_str());
				strStruct.append(t_ch);
			}
		}
		strStruct.append(strTable_2);
		strStruct.append("}\r\n");
	}

	if (g_bCreateCopy) {
		for (auto& r : pStruct._ParVec) {
			strStruct.append(strTable_2);
			if (IsBaseType(r._strParType)) {
				sprintf(t_ch, "//%s_A.%s=%s_B.%s;\r\n", pStruct._name.c_str(), r._strParName.c_str(), pStruct._name.c_str(), r._strParName.c_str());
			}
			else {
				sprintf(t_ch, "//Convert_%s(%s_A.%s,%s_B.%s);\r\n", r._strParName.c_str(), pStruct._name.c_str(), r._strParName.c_str(), pStruct._name.c_str(), r._strParName.c_str());
				if (g_bCreateCopy2) {
					if (IsExType_vector(r._strParType)) {
						string tt_strT = GetExTypeParT_vector(r._strParType);
						if (!IsBaseType(tt_strT)) {
							strStruct.append(t_ch);
							strStruct.append(strTable_2);
							sprintf(t_ch, "//void Convert_%s(%s& %s_A,const %s& %s_B);\r\n", r._strParType.c_str(), r._strParType.c_str(), r._strParType.c_str(), r._strParType.c_str(), r._strParType.c_str());
							strStruct.append(t_ch);
							sprintf(t_ch, "//void Convert_%s(%s& %s_A,const %s& %s_B);\r\n", tt_strT.c_str(), tt_strT.c_str(), tt_strT.c_str(), tt_strT.c_str(), tt_strT.c_str());

						}
					}
					else {
						strStruct.append(t_ch);
						strStruct.append(strTable_2);
						sprintf(t_ch, "//void Convert_%s(%s& %s_A,const %s& %s_B);\r\n", r._strParName.c_str(), r._strParType.c_str(), r._strParType.c_str(), r._strParType.c_str(), r._strParType.c_str());

					}
				}

			}

			strStruct.append(t_ch);
		}

	}


	strStruct.append(strTable);
	strStruct.append("};\r\n");

	if (m_bAndroidPeer) {
		strStruct.append(strTable);
		strStruct.append("#pragma  pack()\r\n");
	}
	else {
		strStruct.append("#if defined(_WINDOWS) || defined(WIN32) || defined(_X64)\r\n");
		strStruct.append(strTable);
		strStruct.append("#pragma  pack(pop)\r\n");
		strStruct.append("#endif\r\n");
	}



	if (HCPPType == 0 || HCPPType == 1) {
		fwrite(strStruct.c_str(), strStruct.length(), 1, fp);
		if (pStruct._oldname.length() > 0) {
			strStruct = "";
			strStruct.append(strTable);
			sprintf(t_ch, "typedef %s %s;\r\n", pStruct._name.c_str(), pStruct._oldname.c_str());
			strStruct.append(t_ch);
			fwrite(strStruct.c_str(), strStruct.length(), 1, fp);
		}
	}





	//输入流的实现
	strStruct = "";
	strStruct.append(strTable);

	if (HCPPType == 0) {
		sprintf(t_ch, "inline WJS_InByteStream& operator << (WJS_InByteStream& ins,const %s& v)\r\n", pStruct._name.c_str());
		strStruct.append(t_ch);
	}
	else if (HCPPType == 1) {
		sprintf(t_ch, "extern WJS_InByteStream& operator << (WJS_InByteStream& ins,const %s& v);\r\n", pStruct._name.c_str());
		strStruct.append(t_ch);
	}
	else if (HCPPType == 2) {
		sprintf(t_ch, "WJS_InByteStream& operator << (WJS_InByteStream& ins,const %s& v)\r\n", pStruct._name.c_str());
		strStruct.append(t_ch);
	}

	fwrite(strStruct.c_str(), strStruct.length(), 1, fp);
	strStruct = "";

	if (HCPPType == 0 || HCPPType == 2) {
		strStruct.append(strTable);
		strStruct.append("{\r\n");

		if (pStruct._inherited.length() > 0) {
			strStruct.append(strTable_2);
			sprintf(t_ch, "%s::operator <<(ins,(const %s)v);\r\n", rNameSpace.c_str(), pStruct._inherited.c_str());
			strStruct.append(t_ch);
		}

		//先写入结构体的大小
		strStruct.append(strTable_2);
		strStruct.append("short tt_len=0;\r\n");
		strStruct.append(strTable_2);
		strStruct.append("int  tt_logsize1 = ins.size();\r\n");
		strStruct.append(strTable_2);
		strStruct.append("ins<<tt_len;\r\n");


		//再写入版本
		strStruct.append(strTable_2);
		sprintf(t_ch, "short tt_version=%d;\r\n", pStruct._version);
		strStruct.append(t_ch);
		strStruct.append(strTable_2);
		strStruct.append("ins<<tt_version;\r\n");
		strStruct.append(strTable_2);
		strStruct.append("int  tt_logsize2 = ins.size();\r\n");



		if (pStruct.IsHasTransNeedValuePar()) {
			strStruct.append(strTable_2);
			strStruct.append("int tt_transNeedValue=0;\r\n");
			strStruct.append(strTable_2);
			strStruct.append("ins<<tt_transNeedValue;\r\n");
			//strStruct.append(strTable_2);
			//strStruct.append("int  tt_logsize4 = ins.size();\r\n");
		}

		for (auto& r : pStruct._ParVec) {
			if (r._transNeedValue != -1) {
				strStruct.append(strTable_2);
				sprintf(t_ch, "if(v.%s!=%s)\r\n", r._strParName.c_str(), r._strDefaultValue.c_str());
				strStruct.append(t_ch);
				strStruct.append(strTable_2);
				strStruct.append("{\r\n");
				strStruct.append(strTable_3);
				string tt_strNameSpace;
				sprintf(t_ch, "ins<<v.%s;\r\n", r._strParName.c_str());
				strStruct.append(t_ch);
				strStruct.append(strTable_3);
				sprintf(t_ch, "tt_transNeedValue = tt_transNeedValue | (1<<%d);\r\n", r._transNeedValue);
				strStruct.append(t_ch);
				strStruct.append(strTable_2);
				strStruct.append("}\r\n");
			}
			else {
				strStruct.append(strTable_2);
				string tt_strNameSpace;
				if (IsExType_baseVector(r._strParType, 0, tt_strNameSpace) && tt_strNameSpace.length() > 0)
					sprintf(t_ch, "%s::operator <<(ins,v.%s);\r\n", tt_strNameSpace.c_str(), r._strParName.c_str());
				else
					sprintf(t_ch, "ins<<v.%s;\r\n", r._strParName.c_str());
				strStruct.append(t_ch);
			}
		}

		if (pStruct.IsHasTransNeedValuePar()) {
			strStruct.append(strTable_2);
			strStruct.append("if(tt_transNeedValue!=0)\r\n");
			strStruct.append(strTable_2);
			strStruct.append("{\r\n");
			strStruct.append(strTable_3);
			strStruct.append("memcpy(&(ins[tt_logsize2]),&tt_transNeedValue,sizeof(int));\r\n");
			strStruct.append(strTable_2);
			strStruct.append("}\r\n");
		}


		strStruct.append(strTable_2);
		strStruct.append("short  tt_logsize3 = (short)(ins.size() - tt_logsize2);\r\n");
		strStruct.append(strTable_2);
		strStruct.append("memcpy(&(ins[tt_logsize1]),&tt_logsize3,sizeof(short));\r\n");
		strStruct.append(strTable_2);
		strStruct.append("return ins;\r\n");

		strStruct.append(strTable);
		strStruct.append("}\r\n");
		fwrite(strStruct.c_str(), strStruct.length(), 1, fp);

	}

	WriteStruct_OutStream_CPlus(rNameSpace,pStruct, fp, tablenum, false, HCPPType);
	WriteStruct_OutStream_CPlus(rNameSpace,pStruct, fp, tablenum, true, HCPPType);

	if (m_bOutJson)
		WriteStruct_JsonStream_CPlus(rNameSpace,pStruct, fp, tablenum, HCPPType);


}
void    ProtocolParse_Writer_CPlus::WriteStruct_OutStream_CPlus(const string& rNameSpace,stStructStruct& pStruct, FILE* fp, int tablenum, bool bOutStream2, int HCPPType)
{
	char t_ch[512];
	string strTable = CreateTableString(tablenum);
	string strTable_2 = CreateTableString(tablenum + 1);
	string strTable_3 = CreateTableString(tablenum + 2);
	string strTable_4 = CreateTableString(tablenum + 3);
	string strTable_5 = CreateTableString(tablenum + 4);

	string strStruct;


	//输出流
	strStruct = "";
	strStruct.append(strTable);
	if (HCPPType == 0) {
		if (bOutStream2) {
			sprintf(t_ch, "inline WJS_OutByteStream2& operator >> (WJS_OutByteStream2& out, %s& v)\r\n", pStruct._name.c_str());
			strStruct.append(t_ch);
		}
		else {
			sprintf(t_ch, "inline WJS_OutByteStream& operator >> (WJS_OutByteStream& out, %s& v)\r\n", pStruct._name.c_str());
			strStruct.append(t_ch);
		}

	}
	else if (HCPPType == 1) {
		if (bOutStream2) {
			sprintf(t_ch, "extern WJS_OutByteStream2& operator >> (WJS_OutByteStream2& out, %s& v);\r\n", pStruct._name.c_str());
			strStruct.append(t_ch);
		}
		else {
			sprintf(t_ch, "extern WJS_OutByteStream& operator >> (WJS_OutByteStream& out, %s& v);\r\n", pStruct._name.c_str());
			strStruct.append(t_ch);
		}

	}
	else if (HCPPType == 2) {
		if (bOutStream2) {
			sprintf(t_ch, "WJS_OutByteStream2& operator >> (WJS_OutByteStream2& out, %s& v)\r\n", pStruct._name.c_str());
			strStruct.append(t_ch);
		}
		else {
			sprintf(t_ch, "WJS_OutByteStream& operator >> (WJS_OutByteStream& out, %s& v)\r\n", pStruct._name.c_str());
			strStruct.append(t_ch);
		}
	}

	fwrite(strStruct.c_str(), strStruct.length(), 1, fp);
	strStruct = "";

	if (HCPPType == 0 || HCPPType == 2) {
		strStruct.append(strTable);
		strStruct.append("{\r\n");

		//先读出结构体的大小
		if (pStruct._inherited.length() > 0) {
			strStruct.append(strTable_2);
			sprintf(t_ch, "%s::operator >>(out,(%s&)v);\r\n", rNameSpace.c_str(), pStruct._inherited.c_str());
			strStruct.append(t_ch);
		}

		strStruct.append(strTable_2);
		strStruct.append("short tt_len=0;\r\n");

		strStruct.append(strTable_2);
		strStruct.append("out>>tt_len;\r\n");

		strStruct.append(strTable_2);
		strStruct.append("short tt_version=0;\r\n");

		strStruct.append(strTable_2);
		strStruct.append("out>>tt_version;\r\n");

		strStruct.append(strTable_2);
		strStruct.append("int tt_logsize1 = 0;\r\n");

		if (pStruct.IsHasTransNeedValuePar()) {
			strStruct.append(strTable_2);
			strStruct.append("tt_logsize1 = out.GetCurOffsetPos();\r\n");
		}

		if (pStruct.IsHasTransNeedValuePar()) {
			strStruct.append(strTable_2);
			strStruct.append("int tt_transNeedValue=0;\r\n");

			strStruct.append(strTable_2);
			strStruct.append("out>>tt_transNeedValue;\r\n");
		}

		strStruct.append(strTable_2);
		sprintf(t_ch, "if(tt_version >= %d || (tt_version==0)){\r\n", pStruct._version);
		strStruct.append(t_ch);


		if (!pStruct.IsHasTransNeedValuePar()) {
			strStruct.append(strTable_3);
			sprintf(t_ch, "if(tt_version > %d)\r\n", pStruct._version);
			strStruct.append(t_ch);
			strStruct.append(strTable_4);
			strStruct.append("tt_logsize1 = out.GetCurOffsetPos();\r\n");
		}



		for (auto& r : pStruct._ParVec) {
			if (pStruct.IsHasTransNeedValuePar() && (r._transNeedValue != -1)) {
				strStruct.append(strTable_3);
				sprintf(t_ch, "if((tt_transNeedValue & (1<<%d))!=0){\r\n", r._transNeedValue);
				strStruct.append(t_ch);

				strStruct.append(strTable_4);
				sprintf(t_ch, "out>>v.%s;\r\n", r._strParName.c_str());
				strStruct.append(t_ch);
				strStruct.append(strTable_3);
				strStruct.append("}\r\n");
				strStruct.append(strTable_3);
				strStruct.append("else{\r\n");

				strStruct.append(strTable_4);
				sprintf(t_ch, "v.%s=%s;\r\n", r._strParName.c_str(), r._strDefaultValue.c_str());
				strStruct.append(t_ch);
				strStruct.append(strTable_3);
				strStruct.append("}\r\n");
			}
			else {
				strStruct.append(strTable_3);
				string tt_strNameSpace;
				if (IsExType_baseVector(r._strParType, 0, tt_strNameSpace) && tt_strNameSpace.length() > 0)
					sprintf(t_ch, "%s::operator >>(out,v.%s);\r\n", tt_strNameSpace.c_str(), r._strParName.c_str());
				else
					sprintf(t_ch, "out>>v.%s;\r\n", r._strParName.c_str());
				strStruct.append(t_ch);
			}
		}


		strStruct.append(strTable_3);
		sprintf(t_ch, "if(tt_version > %d){\r\n", pStruct._version);
		strStruct.append(t_ch);
		strStruct.append(strTable_4);
		strStruct.append("int tt_logsize2 = out.GetCurOffsetPos();\r\n");

		strStruct.append(strTable_4);
		strStruct.append("int tt_offset = tt_len - (tt_logsize2 - tt_logsize1);\r\n");

		strStruct.append(strTable_4);
		strStruct.append("if(tt_offset>0)\r\n");

		strStruct.append(strTable_5);
		strStruct.append("out.CurOffset(tt_offset);\r\n");


		strStruct.append(strTable_3);
		strStruct.append("}\r\n");

		strStruct.append(strTable_2);
		strStruct.append("}\r\n");


		strStruct.append(strTable_2);
		strStruct.append("else{\r\n");


		int tt_lastVersion = -1;
		string tt_strDefault;

		if (pStruct.IsHasTransNeedValuePar()) {
			for (auto& r : pStruct._ParVec) {
				strStruct.append(strTable_3);
				sprintf(t_ch, "if(tt_version>=%d){\r\n", r._version);
				strStruct.append(t_ch);

				if (pStruct.IsHasTransNeedValuePar() && (r._transNeedValue != -1)) {
					strStruct.append(strTable_4);
					sprintf(t_ch, "if((tt_transNeedValue & (1<<%d))!=0){\r\n", r._transNeedValue);
					strStruct.append(t_ch);

					strStruct.append(strTable_5);
					sprintf(t_ch, "out>>v.%s;\r\n", r._strParName.c_str());
					strStruct.append(t_ch);
					strStruct.append(strTable_4);
					strStruct.append("}\r\n");
					strStruct.append(strTable_4);
					strStruct.append("else{\r\n");

					strStruct.append(strTable_5);
					sprintf(t_ch, "v.%s=%s;\r\n", r._strParName.c_str(), r._strDefaultValue.c_str());
					strStruct.append(t_ch);
					strStruct.append(strTable_4);
					strStruct.append("}\r\n");
				}
				else {
					strStruct.append(strTable_4);
					string tt_strNameSpace;
					if (IsExType_baseVector(r._strParType, 0, tt_strNameSpace) && tt_strNameSpace.length() > 0)
						sprintf(t_ch, "%s::operator >>(out,v.%s);\r\n", tt_strNameSpace.c_str(), r._strParName.c_str());
					else
						sprintf(t_ch, "out>>v.%s;\r\n", r._strParName.c_str());
					strStruct.append(t_ch);
				}

				strStruct.append(strTable_3);
				strStruct.append("}\r\n");

				string tt_default = GetDefaultValue(r._strParType);
				if (tt_default.length() > 0) {
					strStruct.append(strTable_3);
					strStruct.append("else\r\n");

					strStruct.append(strTable_4);
					sprintf(t_ch, "v.%s=%s;\r\n", r._strParName.c_str(), tt_default.c_str());
					strStruct.append(t_ch);

				}
			}
		}
		else {
			for (auto& r : pStruct._ParVec) {
				if (tt_lastVersion == -1 || tt_lastVersion != r._version) {

					if (tt_strDefault.length() > 0) {
						strStruct.append(strTable_3);
						strStruct.append("}\r\n");
						strStruct.append(strTable_3);
						strStruct.append("else{\r\n");

						strStruct.append(tt_strDefault.c_str());
						tt_strDefault = "";
						strStruct.append(strTable_3);
						strStruct.append("}\r\n");
					}
					else if (tt_lastVersion != -1) {
						strStruct.append(strTable_3);
						strStruct.append("}\r\n");
					}
					strStruct.append(strTable_3);
					sprintf(t_ch, "if(tt_version>=%d){\r\n", r._version);
					strStruct.append(t_ch);


					tt_lastVersion = r._version;
				}
				strStruct.append(strTable_4);
				string tt_strNameSpace;
				if (IsExType_baseVector(r._strParType, 0, tt_strNameSpace) && tt_strNameSpace.length() > 0)
					sprintf(t_ch, "%s::operator >>(out,v.%s);\r\n", tt_strNameSpace.c_str(), r._strParName.c_str());
				else
					sprintf(t_ch, "out>>v.%s;\r\n", r._strParName.c_str());
				strStruct.append(t_ch);

				string tt_default = GetDefaultValue(r._strParType);
				if (tt_default.length() > 0) {
					tt_strDefault.append(strTable_4);
					sprintf(t_ch, "v.%s=%s;\r\n", r._strParName.c_str(), tt_default.c_str());
					tt_strDefault.append(t_ch);
				}
			}

			if (tt_lastVersion != -1) {
				strStruct.append(strTable_3);
				strStruct.append("}\r\n");
				if (tt_strDefault.length() > 0) {
					strStruct.append(strTable_3);
					strStruct.append("else{\r\n");

					strStruct.append(tt_strDefault.c_str());
					tt_strDefault = "";
					strStruct.append(strTable_3);
					strStruct.append("}\r\n");
				}
			}

		}

		strStruct.append(strTable_2);
		strStruct.append("}\r\n");

		strStruct.append(strTable_2);
		strStruct.append("return out;\r\n");

		strStruct.append(strTable);
		strStruct.append("}\r\n");
		fwrite(strStruct.c_str(), strStruct.length(), 1, fp);
	}

}

void    ProtocolParse_Writer_CPlus::WriteVector_JsonStream_CPlus(stVector& pVector, FILE* fp, int tablenum, int HCPPType)
{
	char t_ch[512];
	string strTable = CreateTableString(tablenum);
	string strTable_2 = CreateTableString(tablenum + 1);
	string strTable_3 = CreateTableString(tablenum + 2);

	string strVector;

	//流入
	strVector = "";
	bool tt_bBaseType = IsBaseType(pVector._ParTName);
	if (HCPPType == 0 || (tt_bBaseType && HCPPType == 1)) {
		strVector.append(strTable);
		sprintf(t_ch, "inline WJS_JsonStream& operator << (WJS_JsonStream& jns,const %s& v)\r\n", pVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 1) {
		strVector.append(strTable);
		sprintf(t_ch, "extern WJS_JsonStream& operator << (WJS_JsonStream& jns,const %s& v);\r\n", pVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 2 && (!tt_bBaseType)) {
		strVector.append(strTable);
		sprintf(t_ch, "WJS_JsonStream& operator << (WJS_JsonStream& jns,const %s& v)\r\n", pVector._name.c_str());
		strVector.append(t_ch);
	}
	fwrite(strVector.c_str(), strVector.length(), 1, fp);
	strVector = "";

	if (HCPPType == 0 || (HCPPType == 2 && (!tt_bBaseType)) || (tt_bBaseType && HCPPType == 1)) {
		strVector.append(strTable);
		strVector.append("{\r\n");

		bool tt_bCreatedJsonObject = false;
		strVector.append(strTable_2);
		strVector.append("WJS_JsonStream  tt_jns;\r\n");


		//如果是基本类型，一次性处理
		if (IsCanFastProcessVectorType_CPlus(pVector._ParTName)) {
			strVector.append(WriteJsonStream_Array(tablenum + 1, "v", pVector._name, "", true).c_str());
		}
		else {
			strVector.append(WriteJsonStream_Array(tablenum + 1, "v", pVector._name, "", true).c_str());
			strVector.append(strTable_2);
			strVector.append("if(v.size()>0)\r\n");
			strVector.append(strTable_2);
			strVector.append("{\r\n");
			strVector.append(strTable_3);
			strVector.append("stJsonObject*   tt_pJsonObject = NULL;\r\n");
			strVector.append(WriteJsonStream_Array0ToObject(tablenum + 2, "v", pVector._name, "").c_str());

			strVector.append(strTable_3);
			strVector.append("jns.AppendJsonStream(tt_jns);\r\n");
			strVector.append(strTable_2);
			strVector.append("}\r\n");
		}

		strVector.append(strTable_2);
		strVector.append("return jns;\r\n");

		strVector.append(strTable);
		strVector.append("}\r\n");

		fwrite(strVector.c_str(), strVector.length(), 1, fp);
	}

}
void    ProtocolParse_Writer_CPlus::WriteWJSVector_JsonStream_CPlus(stWJSVector& pVector, FILE* fp, int tablenum, int HCPPType)
{
	char t_ch[512];
	string strTable = CreateTableString(tablenum);
	string strTable_2 = CreateTableString(tablenum + 1);
	string strTable_3 = CreateTableString(tablenum + 2);

	string strVector;

	//流入
	strVector = "";
	bool tt_bBaseType = IsBaseType(pVector._ParTName);
	if (HCPPType == 0 || (tt_bBaseType && HCPPType == 1)) {
		strVector.append(strTable);
		sprintf(t_ch, "inline WJS_JsonStream& operator << (WJS_JsonStream& jns,const %s& v)\r\n", pVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 1) {
		strVector.append(strTable);
		sprintf(t_ch, "extern WJS_JsonStream& operator << (WJS_JsonStream& jns,const %s& v);\r\n", pVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 2 && (!tt_bBaseType)) {
		strVector.append(strTable);
		sprintf(t_ch, "WJS_JsonStream& operator << (WJS_JsonStream& jns,const %s& v)\r\n", pVector._name.c_str());
		strVector.append(t_ch);
	}
	fwrite(strVector.c_str(), strVector.length(), 1, fp);
	strVector = "";

	if (HCPPType == 0 || (HCPPType == 2 && (!tt_bBaseType)) || (tt_bBaseType && HCPPType == 1)) {
		strVector.append(strTable);
		strVector.append("{\r\n");

		bool tt_bCreatedJsonObject = false;
		strVector.append(strTable_2);
		strVector.append("WJS_JsonStream  tt_jns;\r\n");

		//如果是基本类型，一次性处理
		if (IsCanFastProcessVectorType_CPlus(pVector._ParTName)) {
			strVector.append(WriteJsonStream_Array(tablenum + 1, "v", pVector._name, "", true).c_str());
		}
		else {
			strVector.append(WriteJsonStream_Array(tablenum + 1, "v", pVector._name, "", true).c_str());
			strVector.append(strTable_2);
			strVector.append("if(v.size()>0)\r\n");
			strVector.append(strTable_2);
			strVector.append("{\r\n");
			strVector.append(strTable_3);
			strVector.append("stJsonObject*   tt_pJsonObject = NULL;\r\n");
			strVector.append(WriteJsonStream_Array0ToObject(tablenum + 2, "v", pVector._name, "").c_str());
			strVector.append(strTable_3);
			strVector.append("jns.AppendJsonStream(tt_jns);\r\n");
			strVector.append(strTable_2);
			strVector.append("}\r\n");
		}

		strVector.append(strTable_2);
		strVector.append("return jns;\r\n");

		strVector.append(strTable);
		strVector.append("}\r\n");

		fwrite(strVector.c_str(), strVector.length(), 1, fp);
	}
}
void    ProtocolParse_Writer_CPlus::WriteVector_CPlus(stVector& pVector, FILE* fp, int tablenum, int HCPPType)
{
	if (pVector._name.compare(pVector._oldname) == 0)
		return;
	char t_ch[512];
	string strTable = CreateTableString(tablenum);
	string strTable_2 = CreateTableString(tablenum + 1);
	string strTable_3 = CreateTableString(tablenum + 2);

	string strVector;

	if (HCPPType == 0 || HCPPType == 1) {
		if (pVector._comment.length() > 0) {
			strVector.append(WriteBlockCommentBegin_CPlus(strTable));
			strVector.append(WriteBlockCommentBrief_CPlus(strTable, pVector._comment));
			strVector.append(WriteBlockCommentEnd_CPlus(strTable));
		}

		//strVector.append(WriteComment_CPlus(strTable, pVector._comment, "").c_str());
		strVector.append(strTable);
		sprintf(t_ch, "typedef std::vector<%s> %s;\r\n", GetMemberTypeFullType_CPlus(pVector._ParTName).c_str(), pVector._name.c_str());		
		strVector.append(t_ch);

		if (pVector._oldname.length() > 0) {
			strVector.append(strTable);
			sprintf(t_ch, "typedef std::vector<%s> %s;\r\n", GetMemberTypeFullType_CPlus(pVector._ParTName).c_str(), pVector._oldname.c_str());		
			strVector.append(t_ch);
		}


		fwrite(strVector.c_str(), strVector.length(), 1, fp);
	}


	//流入
	strVector = "";
	bool tt_bBaseType = IsBaseType(pVector._ParTName);
	if (HCPPType == 0 || (tt_bBaseType && HCPPType == 1)) {
		strVector.append(strTable);
		sprintf(t_ch, "inline WJS_InByteStream& operator << (WJS_InByteStream& ins,const %s& v)\r\n", pVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 1) {
		strVector.append(strTable);
		sprintf(t_ch, "extern WJS_InByteStream& operator << (WJS_InByteStream& ins,const %s& v);\r\n", pVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 2 && (!tt_bBaseType)) {
		strVector.append(strTable);
		sprintf(t_ch, "WJS_InByteStream& operator << (WJS_InByteStream& ins,const %s& v)\r\n", pVector._name.c_str());
		strVector.append(t_ch);
	}
	fwrite(strVector.c_str(), strVector.length(), 1, fp);
	strVector = "";

	if (HCPPType == 0 || (HCPPType == 2 && (!tt_bBaseType)) || (tt_bBaseType && HCPPType == 1)) {
		strVector.append(strTable);
		strVector.append("{\r\n");
		strVector.append(strTable_2);
		strVector.append("ins<<int(v.size());\r\n");

		//如果是基本类型，一次性处理
		if (IsCanFastProcessVectorType_CPlus(pVector._ParTName)) {
			strVector.append(strTable_2);
			strVector.append("if(v.size()>0)\r\n");
			strVector.append(strTable_2);
			strVector.append("{\r\n");
			strVector.append(strTable_3);
			sprintf(t_ch, "ins.write((void*)(&(v[0])),sizeof(%s)*(unsigned int)v.size());\r\n", GetMemberTypeFullType_CPlus(pVector._ParTName).c_str());
			
			strVector.append(t_ch);
			strVector.append(strTable_2);
			strVector.append("}\r\n");
		}
		else {
			if (_cplusStandardType >= eCPlusStandardType::CPlus11) {
				strVector.append(strTable_2);
				strVector.append("for(auto& r :v)\r\n");
				strVector.append(strTable_2);
				strVector.append("{\r\n");
				strVector.append(strTable_3);
				strVector.append("ins<<r;\r\n");
				strVector.append(strTable_2);
				strVector.append("}\r\n");
			}
			else {
				strVector.append(strTable_2);
				strVector.append("for(size_t i=0;i<v.size();i++)\r\n");
				strVector.append(strTable_2);
				strVector.append("{\r\n");
				strVector.append(strTable_3);
				strVector.append("ins<<v[i];\r\n");
				strVector.append(strTable_2);
				strVector.append("}\r\n");
			}

		}

		strVector.append(strTable_2);
		strVector.append("return ins;\r\n");

		strVector.append(strTable);
		strVector.append("}\r\n");

		fwrite(strVector.c_str(), strVector.length(), 1, fp);
	}


	//流出
	strVector = "";
	strVector.append(strTable);

	if (HCPPType == 0 || (tt_bBaseType && HCPPType == 1)) {
		sprintf(t_ch, "inline WJS_OutByteStream& operator >> (WJS_OutByteStream& out, %s& v)\r\n", pVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 1) {
		sprintf(t_ch, "extern WJS_OutByteStream& operator >> (WJS_OutByteStream& out, %s& v);\r\n", pVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 2 && (!tt_bBaseType)) {
		sprintf(t_ch, "WJS_OutByteStream& operator >> (WJS_OutByteStream& out, %s& v)\r\n", pVector._name.c_str());
		strVector.append(t_ch);
	}
	fwrite(strVector.c_str(), strVector.length(), 1, fp);
	strVector = "";


	if (HCPPType == 0 || (HCPPType == 2 && (!tt_bBaseType)) || (tt_bBaseType && HCPPType == 1)) {
		strVector.append(strTable);
		strVector.append("{\r\n");

		strVector.append(strTable_2);
		strVector.append("int count=0;\r\n");

		strVector.append(strTable_2);
		strVector.append("out >> count;\r\n");

		strVector.append(strTable_2);
		strVector.append("if(count<0)\r\n");
		strVector.append(strTable_2);
		strVector.append("{\r\n");
		strVector.append(strTable_3);
		strVector.append("throw(\"vecOverFlow\");\r\n");
		strVector.append(strTable_3);
		strVector.append("return out;\r\n");
		strVector.append(strTable_2);
		strVector.append("}\r\n");

		if (pVector._bBaseParT) {
			if (IsCanFastProcessVectorType_CPlus(pVector._ParTName)) {
				strVector.append(strTable_2);
				strVector.append("if(count>0)\r\n");
				strVector.append(strTable_2);
				strVector.append("{\r\n");
				strVector.append(strTable_3);
				strVector.append("v.resize(count);\r\n");
				strVector.append(strTable_3);
				sprintf(t_ch, "out.read(&(v[0]),sizeof(%s)*count);\r\n", GetMemberTypeFullType_CPlus(pVector._ParTName).c_str());
				
				strVector.append(t_ch);
				strVector.append(strTable_2);
				strVector.append("}\r\n");
				strVector.append(strTable_2);

			}
			else {
				strVector.append(strTable_2);
				sprintf(t_ch, "%s m;\r\n", GetMemberTypeFullType_CPlus(pVector._ParTName).c_str());
				
				strVector.append(t_ch);

				strVector.append(strTable_2);
				strVector.append("for(int i=0;i<count;i++)\r\n");
				strVector.append(strTable_2);
				strVector.append("{\r\n");

				strVector.append(strTable_3);
				strVector.append("out>>m;\r\n");

				strVector.append(strTable_3);
				strVector.append("v.push_back(m);\r\n");

				strVector.append(strTable_2);
				strVector.append("}\r\n");
			}

		}
		else {
			strVector.append(strTable_2);
			strVector.append("v.resize(count);\r\n");


			strVector.append(strTable_2);
			strVector.append("for(int i=0;i<count;i++)\r\n");
			strVector.append(strTable_2);
			strVector.append("{\r\n");


			strVector.append(strTable_3);
			strVector.append("out>>v[i];\r\n");

			strVector.append(strTable_2);
			strVector.append("}\r\n");
		}



		strVector.append(strTable_2);
		strVector.append("return out;\r\n");

		strVector.append(strTable);
		strVector.append("}\r\n");

		fwrite(strVector.c_str(), strVector.length(), 1, fp);
	}


	//流出2
	strVector = "";
	if (HCPPType == 0 || (tt_bBaseType && HCPPType == 1)) {
		strVector.append(strTable);
		sprintf(t_ch, "inline WJS_OutByteStream2& operator >> (WJS_OutByteStream2& out, %s& v)\r\n", pVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 1) {
		strVector.append(strTable);
		sprintf(t_ch, "extern WJS_OutByteStream2& operator >> (WJS_OutByteStream2& out, %s& v);\r\n", pVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 2 && (!tt_bBaseType)) {
		strVector.append(strTable);
		sprintf(t_ch, "WJS_OutByteStream2& operator >> (WJS_OutByteStream2& out, %s& v)\r\n", pVector._name.c_str());
		strVector.append(t_ch);
	}
	fwrite(strVector.c_str(), strVector.length(), 1, fp);
	strVector = "";



	if (HCPPType == 0 || (HCPPType == 2 && (!tt_bBaseType)) || (tt_bBaseType && HCPPType == 1)) {
		strVector.append(strTable);
		strVector.append("{\r\n");

		strVector.append(strTable_2);
		strVector.append("int count=0;\r\n");

		strVector.append(strTable_2);
		strVector.append("out >> count;\r\n");

		strVector.append(strTable_2);
		strVector.append("if(count<0)\r\n");
		strVector.append(strTable_2);
		strVector.append("{\r\n");
		strVector.append(strTable_3);
		strVector.append("throw(\"vecOverFlow\");\r\n");
		strVector.append(strTable_3);
		strVector.append("return out;\r\n");
		strVector.append(strTable_2);
		strVector.append("}\r\n");

		if (pVector._bBaseParT) {
			if (IsCanFastProcessVectorType_CPlus(pVector._ParTName)) {
				strVector.append(strTable_2);
				strVector.append("if(count>0)\r\n");
				strVector.append(strTable_2);
				strVector.append("{\r\n");
				strVector.append(strTable_3);
				strVector.append("v.resize(count);\r\n");
				strVector.append(strTable_3);
				sprintf(t_ch, "out.read(&(v[0]),sizeof(%s)*count);\r\n", GetMemberTypeFullType_CPlus(pVector._ParTName).c_str());
				
				strVector.append(t_ch);
				strVector.append(strTable_2);
				strVector.append("}\r\n");
			}
			else {
				strVector.append(strTable_2);
				sprintf(t_ch, "%s m;\r\n", GetMemberTypeFullType_CPlus(pVector._ParTName).c_str());
				
				strVector.append(t_ch);

				strVector.append(strTable_2);
				strVector.append("for(int i=0;i<count;i++)\r\n");
				strVector.append(strTable_2);
				strVector.append("{\r\n");

				strVector.append(strTable_3);
				strVector.append("out>>m;\r\n");

				strVector.append(strTable_3);
				strVector.append("v.push_back(m);\r\n");

				strVector.append(strTable_2);
				strVector.append("}\r\n");
			}

		}
		else {
			strVector.append(strTable_2);
			strVector.append("v.resize(count);\r\n");

			strVector.append(strTable_2);
			strVector.append("for(int i=0;i<count;i++)\r\n");
			strVector.append(strTable_2);
			strVector.append("{\r\n");


			strVector.append(strTable_3);
			strVector.append("out>>v[i];\r\n");

			strVector.append(strTable_2);
			strVector.append("}\r\n");
		}



		strVector.append(strTable_2);
		strVector.append("return out;\r\n");

		strVector.append(strTable);
		strVector.append("}\r\n");

		fwrite(strVector.c_str(), strVector.length(), 1, fp);
	}


	if (m_bOutJson)
		WriteVector_JsonStream_CPlus(pVector, fp, tablenum, HCPPType);

}

void    ProtocolParse_Writer_CPlus::WriteWJSVector_CPlus(stWJSVector& pWJSVector, FILE* fp, int tablenum, int HCPPType)
{
	char t_ch[512];
	string strTable = CreateTableString(tablenum);
	string strTable_2 = CreateTableString(tablenum + 1);
	string strTable_3 = CreateTableString(tablenum + 2);

	string strVector;
	if (HCPPType == 0 || HCPPType == 1) {
		strVector.append(strTable);
		sprintf(t_ch, "typedef WJS_ArrayVector<%s> %s;\r\n", GetMemberTypeFullType_CPlus(pWJSVector._ParTName).c_str(), pWJSVector._name.c_str());
		
		strVector.append(t_ch);
		fwrite(strVector.c_str(), strVector.length(), 1, fp);
	}


	//流入
	strVector = "";
	strVector.append(strTable);

	if (HCPPType == 0) {
		sprintf(t_ch, "inline WJS_InByteStream& operator << (WJS_InByteStream& ins, const %s& v)\r\n", pWJSVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 1) {
		sprintf(t_ch, "extern WJS_InByteStream& operator << (WJS_InByteStream& ins, const %s& v);\r\n", pWJSVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 2) {
		sprintf(t_ch, "WJS_InByteStream& operator << (WJS_InByteStream& ins, const %s& v)\r\n", pWJSVector._name.c_str());
		strVector.append(t_ch);
	}
	fwrite(strVector.c_str(), strVector.length(), 1, fp);
	strVector = "";



	if (HCPPType == 0 || HCPPType == 2) {
		strVector.append(strTable);
		strVector.append("{\r\n");

		strVector.append(strTable_2);
		strVector.append("ins<<int(v.size());\r\n");

		if (IsCanFastProcessVectorType_CPlus(pWJSVector._ParTName)) {
			strVector.append(strTable_2);
			strVector.append("if(v.size()>0)\r\n");
			strVector.append(strTable_2);
			strVector.append("{\r\n");
			strVector.append(strTable_3);
			sprintf(t_ch, "ins.write((void*)(&(v[0])),sizeof(%s)*v.size());\r\n", GetMemberTypeFullType_CPlus(pWJSVector._ParTName).c_str());
			
			strVector.append(t_ch);
			strVector.append(strTable_2);
			strVector.append("}\r\n");
		}
		else {
			//if (_cplusStandardType >= eCPlusStandardType::CPlus11) {
			//	strVector.append(strTable_2);
			//	strVector.append("for(auto& r: v)\r\n");
			//	strVector.append(strTable_2);
			//	strVector.append("{\r\n");
			//	strVector.append(strTable_3);
			//	strVector.append("ins<<r;\r\n");
			//	strVector.append(strTable_2);
			//	strVector.append("}\r\n");
			//}
			//else {
				strVector.append(strTable_2);
				strVector.append("for(size_t i=0;i<v.size();i++)\r\n");
				strVector.append(strTable_2);
				strVector.append("{\r\n");
				strVector.append(strTable_3);
				strVector.append("ins<<v[i];\r\n");
				strVector.append(strTable_2);
				strVector.append("}\r\n");
			//}

		}

		strVector.append(strTable_2);
		strVector.append("return ins;\r\n");

		strVector.append(strTable);
		strVector.append("}\r\n");

		fwrite(strVector.c_str(), strVector.length(), 1, fp);
	}


	//流出
	strVector = "";
	strVector.append(strTable);

	if (HCPPType == 0) {
		sprintf(t_ch, "inline WJS_OutByteStream& operator >> (WJS_OutByteStream& out, %s& v)\r\n", pWJSVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 1) {
		sprintf(t_ch, "extern WJS_OutByteStream& operator >> (WJS_OutByteStream& out, %s& v);\r\n", pWJSVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 2) {
		sprintf(t_ch, "WJS_OutByteStream& operator >> (WJS_OutByteStream& out, %s& v)\r\n", pWJSVector._name.c_str());
		strVector.append(t_ch);
	}
	fwrite(strVector.c_str(), strVector.length(), 1, fp);
	strVector = "";

	if (HCPPType == 0 || HCPPType == 2) {
		strVector.append(strTable);
		strVector.append("{\r\n");

		strVector.append(strTable_2);
		strVector.append("int count=0;\r\n");

		strVector.append(strTable_2);
		strVector.append("out >> count;\r\n");

		strVector.append(strTable_2);
		strVector.append("if(count<0)\r\n");
		strVector.append(strTable_2);
		strVector.append("{\r\n");
		strVector.append(strTable_3);
		strVector.append("throw(\"wvecOverFlow\");\r\n");
		strVector.append(strTable_3);
		strVector.append("return out;\r\n");
		strVector.append(strTable_2);
		strVector.append("}\r\n");


		if (pWJSVector._bBaseParT) {
			if (IsCanFastProcessVectorType_CPlus(pWJSVector._ParTName)) {
				strVector.append(strTable_2);
				strVector.append("if(count>0)\r\n");
				strVector.append(strTable_2);
				strVector.append("{\r\n");
				strVector.append(strTable_3);
				strVector.append("v.resize(count,true);\r\n");
				strVector.append(strTable_3);
				sprintf(t_ch, "out.read(&(v[0]),sizeof(%s)*count);\r\n", GetMemberTypeFullType_CPlus(pWJSVector._ParTName).c_str());				
				strVector.append(t_ch);
				strVector.append(strTable_2);
				strVector.append("}\r\n");
			}
			else {
				strVector.append(strTable_2);
				strVector.append("for(int i=0;i<count;i++)\r\n");
				strVector.append(strTable_2);
				strVector.append("{\r\n");
				strVector.append(strTable_3);
				sprintf(t_ch, "%s m;\r\n", GetMemberTypeFullType_CPlus(pWJSVector._ParTName).c_str());
				
				strVector.append(t_ch);
				strVector.append(strTable_3);
				strVector.append("out>>m;\r\n");
				strVector.append(strTable_3);
				strVector.append("v.push_back(m);\r\n");
				strVector.append(strTable_2);
				strVector.append("}\r\n");
			}

		}
		else {
			strVector.append(strTable_2);
			strVector.append("v.resize(count,true);\r\n");

			strVector.append(strTable_2);
			strVector.append("for(int i=0;i<count;i++)\r\n");
			strVector.append(strTable_2);
			strVector.append("{\r\n");

			strVector.append(strTable_3);
			strVector.append("out>>v[i];\r\n");

			strVector.append(strTable_2);
			strVector.append("}\r\n");
		}



		strVector.append(strTable_2);
		strVector.append("return out;\r\n");

		strVector.append(strTable);
		strVector.append("}\r\n");

		fwrite(strVector.c_str(), strVector.length(), 1, fp);

	}

	//流出2
	strVector = "";
	strVector.append(strTable);

	if (HCPPType == 0) {
		sprintf(t_ch, "inline WJS_OutByteStream2& operator >> (WJS_OutByteStream2& out, %s& v)\r\n", pWJSVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 1) {
		sprintf(t_ch, "extern WJS_OutByteStream2& operator >> (WJS_OutByteStream2& out, %s& v);\r\n", pWJSVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 2) {
		sprintf(t_ch, "WJS_OutByteStream2& operator >> (WJS_OutByteStream2& out, %s& v)\r\n", pWJSVector._name.c_str());
		strVector.append(t_ch);
	}

	fwrite(strVector.c_str(), strVector.length(), 1, fp);
	strVector = "";

	if (HCPPType == 0 || HCPPType == 2) {
		strVector.append(strTable);
		strVector.append("{\r\n");

		strVector.append(strTable_2);
		strVector.append("int count=0;\r\n");

		strVector.append(strTable_2);
		strVector.append("out >> count;\r\n");

		strVector.append(strTable_2);
		strVector.append("if(count<0)\r\n");
		strVector.append(strTable_2);
		strVector.append("{\r\n");
		strVector.append(strTable_3);
		strVector.append("throw(\"wvecOverFlow\");\r\n");
		strVector.append(strTable_3);
		strVector.append("return out;\r\n");
		strVector.append(strTable_2);
		strVector.append("}\r\n");


		if (pWJSVector._bBaseParT) {
			if (IsCanFastProcessVectorType_CPlus(pWJSVector._ParTName)) {
				strVector.append(strTable_2);
				strVector.append("if(count>0)\r\n");
				strVector.append(strTable_2);
				strVector.append("{\r\n");
				strVector.append(strTable_3);
				strVector.append("v.resize(count,true);\r\n");
				strVector.append(strTable_3);
				sprintf(t_ch, "out.read(&(v[0]),sizeof(%s)*count);\r\n", GetMemberTypeFullType_CPlus(pWJSVector._ParTName).c_str());
				
				strVector.append(t_ch);
				strVector.append(strTable_2);
				strVector.append("}\r\n");
			}
			else {
				strVector.append(strTable_2);
				strVector.append("for(int i=0;i<count;i++)\r\n");
				strVector.append(strTable_2);
				strVector.append("{\r\n");
				strVector.append(strTable_3);
				sprintf(t_ch, "%s m;\r\n", GetMemberTypeFullType_CPlus(pWJSVector._ParTName).c_str());
				
				strVector.append(t_ch);
				strVector.append(strTable_3);
				strVector.append("out>>m;\r\n");
				strVector.append(strTable_3);
				strVector.append("v.push_back(m);\r\n");
				strVector.append(strTable_2);
				strVector.append("}\r\n");
			}

		}
		else {
			strVector.append(strTable_2);
			strVector.append("v.resize(count,true);\r\n");

			strVector.append(strTable_2);
			strVector.append("for(int i=0;i<count;i++)\r\n");
			strVector.append(strTable_2);
			strVector.append("{\r\n");

			strVector.append(strTable_3);
			strVector.append("out>>v[i];\r\n");

			strVector.append(strTable_2);
			strVector.append("}\r\n");
		}


		strVector.append(strTable_2);
		strVector.append("return out;\r\n");

		strVector.append(strTable);
		strVector.append("}\r\n");

		fwrite(strVector.c_str(), strVector.length(), 1, fp);
	}

	if (m_bOutJson)
		WriteWJSVector_JsonStream_CPlus(pWJSVector, fp, tablenum, HCPPType);

}
void    ProtocolParse_Writer_CPlus::WriteClass_CPlus(stClass& pClass, FILE* fp, int tablenum, int filetype, const string& spacename, stNamespace& pNamespace, int HCPPType)
{
	switch (filetype) {
	case 0:
	{
		//对于客户端而言，request的为发送，callback的为回调
		if (pClass._strtype.compare("callback") == 0) {
			if (HCPPType == 0 || HCPPType == 1)
				WriteClass_IReceiver_CPlus(pClass, fp, tablenum, filetype, spacename);
			WriteClass_Receiver_CPlus(pClass, fp, tablenum, filetype, spacename, "", pNamespace, HCPPType);
		}
		else {
			if (HCPPType == 0 || HCPPType == 1)
				WriteClass_Send_CPlus(pClass, fp, tablenum, filetype, spacename, pNamespace);
		}
	}
	break;
	case 1:
	{
		//服务器而言，请求的是回调，callback是发送
		if (pClass._strtype.compare("request") == 0) //C2S
		{
			if (HCPPType == 0 || HCPPType == 1)
				WriteClass_IReceiver_CPlus(pClass, fp, tablenum, filetype, spacename);
			WriteClass_Receiver_CPlus(pClass, fp, tablenum, filetype, spacename, "", pNamespace, HCPPType);
		}
		else //S2C
		{
			if (HCPPType == 0 || HCPPType == 1)
				WriteClass_Send_CPlus(pClass, fp, tablenum, filetype, spacename, pNamespace);
		}
	}
	break;
	case 2:
	{
		//取一下callback的类名

		string strcallbackclassname;
		for (size_t i = 0; i < pNamespace._ClassVec.size(); i++) {
			if (pNamespace._ClassVec[i]._strtype.compare("callback") == 0) {
				strcallbackclassname = pNamespace._ClassVec[i]._name;
				break;
			}
		}

		//事实上两个里面是一样的
		if (pClass._strtype.compare("request") == 0) //C2S
		{
			if (HCPPType == 0 || HCPPType == 1)
				WriteClass_IReceiver_CPlus(pClass, fp, tablenum, filetype, spacename, true & (!m_bPureVirtual));
			WriteClass_Receiver_CPlus(pClass, fp, tablenum, filetype, spacename, strcallbackclassname, pNamespace, HCPPType);
			if (HCPPType == 0 || HCPPType == 1)
				WriteClass_Send_CPlus(pClass, fp, tablenum, filetype, spacename, pNamespace);
		}
		else //S2C
		{

		}

	}
	break;
	default:
		break;
	}
}
void    ProtocolParse_Writer_CPlus::WriteClass_IReceiver_CPlus(stClass& pClass, FILE* fp, int tablenum, int filetype, const string& spacename, bool bemptyimplement)//写接口类
{
	//////////没有接口
	char t_ch[512];
	string strTable = CreateTableString(tablenum);
	string strTable_2 = CreateTableString(tablenum + 1);

	string strClass;

	int tt_maxLayer = pClass.GetMaxLayer();
	for (int k = 1; k <= tt_maxLayer; k++) {
		strClass.append(strTable);
		if (k != 1) {
			sprintf(t_ch, "class IReceiver_%s_%d : public IReceiver_%s_%d\r\n", spacename.c_str(), k, spacename.c_str(), k - 1);
			strClass.append(t_ch);
		}
		else {
			sprintf(t_ch, "class IReceiver_%s_%d\r\n", spacename.c_str(), k);
			strClass.append(t_ch);
		}

		strClass.append(strTable);
		strClass.append("{\r\n");
		strClass.append(strTable);
		strClass.append("public:\r\n");
		fwrite(strClass.c_str(), strClass.length(), 1, fp);
		strClass = "";

		for (auto& r : pClass._FuncVec) {
			if (r._bDiscard || r._Layer != k)
				continue;
			if (filetype == 2 && ((m_bSCSpliteS && r._SCSpliteType == -1) || (m_bSCSpliteC && r._SCSpliteType == 1)))
				continue;
			WriteFunc_IReceiver_CPlus(r, fp, tablenum + 1, filetype, bemptyimplement);
		}

		strClass.append(strTable);
		strClass.append("};\r\n");

		fwrite(strClass.c_str(), strClass.length(), 1, fp);
		strClass = "";

	}

	strClass.append(strTable);
	if (tt_maxLayer > 0) {
		sprintf(t_ch, "class IReceiver_%s : public IReceiver_%s_%d\r\n", spacename.c_str(), spacename.c_str(), tt_maxLayer);
		strClass.append(t_ch);
	}
	else {
		sprintf(t_ch, "class IReceiver_%s\r\n", spacename.c_str());
		strClass.append(t_ch);
	}


	strClass.append(strTable);
	strClass.append("{\r\n");
	strClass.append(strTable);
	strClass.append("public:\r\n");
	fwrite(strClass.c_str(), strClass.length(), 1, fp);
	strClass = "";


	for (auto& r : pClass._FuncVec) {
		if (r._bDiscard || r._Layer != 0)
			continue;
		if (filetype == 2 && ((m_bSCSpliteS && r._SCSpliteType == -1) || (m_bSCSpliteC && r._SCSpliteType == 1)))
			continue;
		WriteFunc_IReceiver_CPlus(r, fp, tablenum + 1, filetype, bemptyimplement);
	}

	strClass.append(strTable);
	strClass.append("};\r\n");

	fwrite(strClass.c_str(), strClass.length(), 1, fp);
}

void    ProtocolParse_Writer_CPlus::WriteFunc_IReceiver_CPlus(stFunc& pFunc, FILE* fp, int tablenum, int filetype, bool bemptyimplement)
{
	char t_ch[512];
	string strTable = CreateTableString(tablenum);
	string strFunc;

	strFunc.append(WriteBlockCommentBegin_CPlus(strTable));
	strFunc.append(WriteBlockCommentBrief_CPlus(strTable, pFunc._comment));
	for (auto& r : pFunc._InParVec) {
		strFunc.append(WriteBlockCommentPar_CPlus(strTable, r._comment, r._strParName).c_str());
	}

	strFunc.append(WriteBlockCommentEnd_CPlus(strTable));


	//做为接收，函数名前+callback
	strFunc.append(strTable);

	//返回类型都改为void型
	sprintf(t_ch, "virtual  void IReceiver_%s(", pFunc._name.c_str());

	strFunc.append(t_ch);

	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		string strpar = CovertParToString_CPlus(pFunc._InParVec[i]);
		strFunc.append(strpar.c_str());
		if (i != pFunc._InParVec.size() - 1) {
			strFunc.append(",");
		}
	}

	switch (filetype) {
	case 0:
	{
		//客户端传入一个void* 
		if (pFunc._InParVec.size() > 0)
			strFunc.append(",void* pExData");
		else
			strFunc.append("void* pExData");
	}
	break;
	case 1:
	{
		//服务器端增另会话号
		if (pFunc._InParVec.size() > 0)
			strFunc.append(",unsigned int sessionId,void* pExData");
		else
			strFunc.append("unsigned int sessionId,void* pExData");
	}
	break;
	case 2:
	{
		//服务器端增另会话号
		if (pFunc._InParVec.size() > 0)
			strFunc.append(",unsigned int sessionId,void* pExData");
		else
			strFunc.append("unsigned int sessionId,void* pExData");
	}
	break;
	default:
		break;
	}

	if (bemptyimplement) {
		strFunc.append("){};\r\n");
	}
	else
		strFunc.append(")=0;\r\n");

	fwrite(strFunc.c_str(), strFunc.length(), 1, fp);
}

void    ProtocolParse_Writer_CPlus::WriteClass_Receiver_CPlus(stClass& pClass, FILE* fp, int tablenum, int filetype, const string& spacename, const string& strcallbackclassname, stNamespace& pNamespace, int HCPPType)
{
	char t_ch[512];
	string strTable = CreateTableString(tablenum);
	string strTable_2 = CreateTableString(tablenum + 1);

	string strClass;

	if (HCPPType == 0 || HCPPType == 1) {
		strClass.append(strTable);
		sprintf(t_ch, "class ReceiveCallback_%s\r\n", spacename.c_str());
		strClass.append(t_ch);

		strClass.append(strTable);
		strClass.append("{\r\n");

		if (m_bOutJson) {
			strClass.append(strTable);
			strClass.append("public:\r\n");

			strClass.append(strTable_2);
			sprintf(t_ch, "ReceiveCallback_%s() {_bOutJson=false;_pWriteJsonStream=NULL;}\r\n", spacename.c_str());
			strClass.append(t_ch);
		}

		strClass.append(strTable);
		strClass.append("protected:\r\n");
		fwrite(strClass.c_str(), strClass.length(), 1, fp);
		strClass = "";
	}

	for (auto& r : pClass._FuncVec) {
		if (r._bDiscard)
			continue;
		if (filetype == 2 && ((m_bSCSpliteS && r._SCSpliteType == -1) || (m_bSCSpliteC && r._SCSpliteType == 1)))
			continue;
		WriteFunc_Receiver_CPlus(r, fp, tablenum + 1, filetype, spacename, pNamespace, HCPPType);
	}

	if (HCPPType == 0 || HCPPType == 1) {
		strClass = "";
		strClass.append(strTable);
		strClass.append("public:\r\n");
		fwrite(strClass.c_str(), strClass.length(), 1, fp);
	}


	//函数
	WriteFunc_ReceiverParser_CPlus(pClass, fp, tablenum + 1, filetype, spacename, strcallbackclassname, pNamespace, HCPPType);

	if (HCPPType == 0 || HCPPType == 1) {
		strClass = "";

		if (m_bOutJson) {
			strClass.append(strTable_2);
			strClass.append("bool _bOutJson;\r\n");

			strClass.append(strTable_2);
			strClass.append("WJS_JsonStream _jsonStream;\r\n");

			strClass.append(strTable_2);
			strClass.append("IWrite_JsonStream* _pWriteJsonStream;\r\n");


		}

		strClass.append(strTable);
		strClass.append("};\r\n");
		fwrite(strClass.c_str(), strClass.length(), 1, fp);
	}

}
void    ProtocolParse_Writer_CPlus::WriteFunc_Receiver_CPlus(stFunc& pFunc, FILE* fp, int tablenum, int filetype, const string& spacename, stNamespace& pNamespace, int HCPPType)
{
	char t_ch[512];
	string strTable = CreateTableString(tablenum);
	string strTable_2 = CreateTableString(tablenum + 1);
	string strTable_3 = CreateTableString(tablenum + 2);
	string strTable_4 = CreateTableString(tablenum + 3);

	string strFunc;
	strFunc.append(strTable);

	if (pFunc._name.compare("OnUpdatePlayerRoomState") == 0) {
		int a;
		a = 10;

	}

	if (HCPPType == 0 || HCPPType == 1) {
		//返回类型void
		sprintf(t_ch, "void _parser_%s(", pFunc._name.c_str());
		strFunc.append(t_ch);
	}
	else {
		sprintf(t_ch, "void ReceiveCallback_%s::_parser_%s(", spacename.c_str(), pFunc._name.c_str());
		strFunc.append(t_ch);

	}


	switch (filetype) {
	case 0:
	{
		sprintf(t_ch, "WJS_OutByteStream2& out,IReceiver_%s* receiver,void* pExData,short proversionno)", spacename.c_str());
	}
	break;
	case 1:
	{
		sprintf(t_ch, "WJS_OutByteStream2& out,IReceiver_%s* receiver,unsigned int sessionId,void* pExData,short proversionno)", spacename.c_str());
	}
	break;
	case 2:
	{
		sprintf(t_ch, "WJS_OutByteStream2& out,IReceiver_%s* receiver,unsigned int sessionId,void* pExData,short proversionno)", spacename.c_str());
	}
	break;
	default:
		break;
	}
	strFunc.append(t_ch);

	if (HCPPType == 0) {
		strFunc.append("\r\n");
	}
	else if (HCPPType == 1) {
		strFunc.append(";\r\n");
	}
	else if (HCPPType == 2) {
		strFunc.append("\r\n");
	}

	fwrite(strFunc.c_str(), strFunc.length(), 1, fp);
	strFunc = "";

	if (HCPPType == 1)
		return;

	strFunc.append(strTable);
	strFunc.append("{\r\n");

	for (auto& r : pFunc._InParVec) {
		strFunc.append(strTable_2);
		sprintf(t_ch, "%s %s;\r\n", GetMemberTypeFullType_CPlus(r._strParType).c_str(), r._strParName.c_str());

		strFunc.append(t_ch);
	}

	for (auto& r : pFunc._InParVec) {
		if (r._version > 1)
			break;
		strFunc.append(strTable_2);
		string tt_strNameSpace;
		if (IsExType_baseVector(r._strParType, filetype, tt_strNameSpace) && tt_strNameSpace.length() > 0)
			sprintf(t_ch, "%s::operator >>(out,%s);\r\n", tt_strNameSpace.c_str(), r._strParName.c_str());
		else
			sprintf(t_ch, "out>>%s;\r\n", r._strParName.c_str());

		strFunc.append(t_ch);
	}

	string ttt_strFunc;
	bool   tt_bHasReadData = false;

	//作出判定相等
	ttt_strFunc.append(strTable_2);
	sprintf(t_ch, "if(proversionno==%d || (proversionno==0))\r\n", pFunc._version);
	ttt_strFunc.append(t_ch);

	ttt_strFunc.append(strTable_2);
	ttt_strFunc.append("{\r\n");

	for (auto& r : pFunc._InParVec) {
		if (r._version <= 1)
			continue;
		tt_bHasReadData = true;
		ttt_strFunc.append(strTable_3);
		string tt_strNameSpace;
		if (IsExType_baseVector(r._strParType, filetype, tt_strNameSpace) && tt_strNameSpace.length() > 0)
			sprintf(t_ch, "%s::operator >>(out,%s);\r\n", tt_strNameSpace.c_str(), r._strParName.c_str());
		else
			sprintf(t_ch, "out>>%s;\r\n", r._strParName.c_str());

		ttt_strFunc.append(t_ch);
	}

	ttt_strFunc.append(strTable_2);
	ttt_strFunc.append("}\r\n");

	//////////////低解高
	ttt_strFunc.append(strTable_2);
	sprintf(t_ch, "else if(proversionno>%d)\r\n", pFunc._version);
	ttt_strFunc.append(t_ch);

	ttt_strFunc.append(strTable_2);
	ttt_strFunc.append("{\r\n");

	for (auto& r : pFunc._InParVec) {
		if (r._version <= 1)
			continue;
		tt_bHasReadData = true;
		ttt_strFunc.append(strTable_3);
		string tt_strNameSpace;
		if (IsExType_baseVector(r._strParType, filetype, tt_strNameSpace) && tt_strNameSpace.length() > 0)
			sprintf(t_ch, "%s::operator >>(out,%s);\r\n", tt_strNameSpace.c_str(), r._strParName.c_str());
		else
			sprintf(t_ch, "out>>%s;\r\n", r._strParName.c_str());

		ttt_strFunc.append(t_ch);
	}

	ttt_strFunc.append(strTable_2);
	ttt_strFunc.append("}\r\n");

	///////////////高解低
	ttt_strFunc.append(strTable_2);
	ttt_strFunc.append("else\r\n");

	ttt_strFunc.append(strTable_2);
	ttt_strFunc.append("{\r\n");

	for (auto& r : pFunc._InParVec) {
		if (r._version <= 1)
			continue;
		tt_bHasReadData = true;
		ttt_strFunc.append(strTable_3);
		sprintf(t_ch, "if(proversionno>=%d)\r\n", r._version);
		ttt_strFunc.append(t_ch);

		ttt_strFunc.append(strTable_4);
		string tt_strNameSpace;
		if (IsExType_baseVector(r._strParType, filetype, tt_strNameSpace) && tt_strNameSpace.length() > 0) {
			sprintf(t_ch, "%s::operator >>(out,%s);\r\n", tt_strNameSpace.c_str(), r._strParName.c_str());
		}
		else {
			sprintf(t_ch, "out>>%s;\r\n", r._strParName.c_str());
		}
		ttt_strFunc.append(t_ch);

		string tt_default = GetDefaultValue(r._strParType);
		if (tt_default.length() > 0) {
			ttt_strFunc.append(strTable_3);
			ttt_strFunc.append("else\r\n");

			ttt_strFunc.append(strTable_4);
			sprintf(t_ch, "%s = %s;\r\n", r._strParName.c_str(), tt_default.c_str());
			ttt_strFunc.append(t_ch);
		}
	}

	ttt_strFunc.append(strTable_2);
	ttt_strFunc.append("}\r\n");

	if (tt_bHasReadData)
		strFunc.append(ttt_strFunc.c_str());

	strFunc.append(strTable_2);
	sprintf(t_ch, "receiver->IReceiver_%s(", pFunc._name.c_str());
	strFunc.append(t_ch);

	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		sprintf(t_ch, "%s", pFunc._InParVec[i]._strParName.c_str());
		strFunc.append(t_ch);

		if (i == pFunc._InParVec.size() - 1) {
			switch (filetype) {
			case 0:
			{
				strFunc.append(",pExData");
			}
			break;
			case 1:
			{
				strFunc.append(",sessionId,pExData");
			}
			break;
			case 2:
			{
				strFunc.append(",sessionId,pExData");
			}
			break;
			default:
				break;
			}
			strFunc.append(");\r\n");
		}
		else {
			strFunc.append(",");
		}

	}

	if (pFunc._InParVec.size() == 0) {
		switch (filetype) {
		case 0:
		{
			if (pFunc._InParVec.size() > 0)
				strFunc.append(",pExData");
			else
				strFunc.append("pExData");
		}
		break;
		case 1:
		{
			if (pFunc._InParVec.size() > 0)
				strFunc.append(",sessionId,pExData");
			else
				strFunc.append("sessionId,pExData");
		}
		break;
		case 2:
		{
			if (pFunc._InParVec.size() > 0)
				strFunc.append(",sessionId,pExData");
			else
				strFunc.append("sessionId,pExData");
		}
		break;
		default:
			break;
		}
		strFunc.append(");\r\n");
	}


	if (m_bOutJson) {
		if (HCPPType == 0 || HCPPType == 2) {
			strFunc.append(strTable_2);
			strFunc.append("if(_bOutJson){\r\n");

			strFunc.append(strTable_3);
			strFunc.append("stJsonObject*   tt_pJsonObject = NULL;\r\n");

			for (auto& r : pFunc._InParVec) {
				strFunc.append(strTable_3);
				string tt_strNameSpace;
				if (IsExType_baseVector(r._strParType, filetype, tt_strNameSpace)) {
					sprintf(t_ch, "%s::operator <<(_jsonStream,%s);\r\n", tt_strNameSpace.c_str(), r._strParName.c_str());
					strFunc.append(t_ch);
					//sprintf(t_ch, "_jsonStream<<%s;\r\n", r._strParName.c_str());					
				}
				else if (IsExType_vector(r._strParType)) {
					sprintf(t_ch, "_jsonStream<<%s;\r\n", r._strParName.c_str());
					strFunc.append(t_ch);
				}
				else if (IsBaseType(r._strParType)) {
					sprintf(t_ch, "_jsonStream.WriteNewKey(\"%s\",0,false);\r\n", r._strParName.c_str());
					strFunc.append(t_ch);
					strFunc.append(strTable_3);
					sprintf(t_ch, "_jsonStream<<%s;\r\n", r._strParName.c_str());
					strFunc.append(t_ch);
				}
				else if (IsExType_enum(r._strParType)) {
					sprintf(t_ch, "tt_pJsonObject=_jsonStream.WriteNewKey(\"%s\", 2, true);\r\n", r._strParName.c_str());
					strFunc.append(t_ch);
					strFunc.append(strTable_3);
					sprintf(t_ch, "_jsonStream<<(int)%s;\r\n", r._strParName.c_str());
					strFunc.append(t_ch);
					strFunc.append(strTable_3);
					strFunc.append("_jsonStream.EnterToChildEnd(tt_pJsonObject);\r\n");
				}
				else {
					sprintf(t_ch, "tt_pJsonObject=_jsonStream.WriteNewKey(\"%s\", 2, true);\r\n", r._strParName.c_str());
					strFunc.append(t_ch);
					strFunc.append(strTable_3);
					sprintf(t_ch, "_jsonStream<<%s;\r\n", r._strParName.c_str());
					strFunc.append(t_ch);
					strFunc.append(strTable_3);
					strFunc.append("_jsonStream.EnterToChildEnd(tt_pJsonObject);\r\n");
				}
			}

			strFunc.append(strTable_3);
			strFunc.append("if(_pWriteJsonStream)\r\n");

			strFunc.append(strTable_4);
			strFunc.append("_pWriteJsonStream->NotifyWriteJsonStream(_jsonStream);\r\n");

			strFunc.append(strTable_2);
			strFunc.append("}\r\n");
		}
	}



	strFunc.append(strTable);
	strFunc.append("}\r\n");

	fwrite(strFunc.c_str(), strFunc.length(), 1, fp);
}

void    ProtocolParse_Writer_CPlus::WriteFunc_ReceiverParser_CPlus(stClass& pClass, FILE* fp, int tablenum, int filetype, const string& spacename, const string& strcallbackclassname, stNamespace& pNamespace, int HCPPType)
{
	char t_ch[512];
	string strTable = CreateTableString(tablenum);
	string strTable_2 = CreateTableString(tablenum + 1);
	string strTable_3 = CreateTableString(tablenum + 2);
	string strTable_4 = CreateTableString(tablenum + 3);


	string strFunc;
	strFunc.append(strTable);
	strFunc.append("bool ");

	if (HCPPType == 2) {
		sprintf(t_ch, "ReceiveCallback_%s::", spacename.c_str());
		strFunc.append(t_ch);
	}

	switch (filetype) {
	case 0:
	{
		sprintf(t_ch, "Parser(const WJSByteSeq& byteStream,IReceiver_%s* receiver,void* pExData)", spacename.c_str());
		strFunc.append(t_ch);
	}
	break;
	case 1:
	{
		sprintf(t_ch, "Parser(const WJSByteSeq& byteStream,IReceiver_%s* receiver,unsigned int sessionId,void* pExData)", spacename.c_str());
		strFunc.append(t_ch);
	}
	break;
	case 2:
	{
		sprintf(t_ch, "Parser(const WJSByteSeq& byteStream,IReceiver_%s* receiver,unsigned int sessionId,void* pExData)", spacename.c_str());
		strFunc.append(t_ch);
	}
	break;
	default:
		break;
	}

	if (HCPPType == 1) {
		strFunc.append(";");
	}

	strFunc.append("\r\n");

	fwrite(strFunc.c_str(), strFunc.length(), 1, fp);
	strFunc = "";

	if (HCPPType == 1)
		return;



	strFunc.append(strTable);
	strFunc.append("{\r\n");

	strFunc.append(strTable_2);
	strFunc.append("int tt_prono=byteStream.readintonly();\r\n");
	strFunc.append(strTable_2);
	sprintf(t_ch, "if(tt_prono!=%s)\r\n", m_ProtocolNo._name.c_str());
	strFunc.append(t_ch);
	strFunc.append(strTable_2);
	strFunc.append("  return false;\r\n");

	strFunc.append(strTable_2);
	strFunc.append("WJS_OutByteStream2 out(byteStream);\r\n");
	//增加协议号
	strFunc.append(strTable_2);
	strFunc.append("int prono=0;\r\n");
	strFunc.append(strTable_2);
	strFunc.append("out>>prono;\r\n");
	strFunc.append(strTable_2);
	sprintf(t_ch, "if(prono!=%s)\r\n", m_ProtocolNo._name.c_str());
	strFunc.append(t_ch);
	strFunc.append(strTable_2);
	strFunc.append("   return false;\r\n");

	//////协议版本
	strFunc.append(strTable_2);
	strFunc.append("short proversionno=0;\r\n");
	strFunc.append(strTable_2);
	strFunc.append("out>>proversionno;\r\n");

	strFunc.append(strTable_2);
	strFunc.append("int methodid=0;\r\n");
	strFunc.append(strTable_2);
	strFunc.append("out>>methodid;\r\n");

	if (m_bOutJson) {
		strFunc.append(strTable_2);
		strFunc.append("if(_bOutJson){\r\n");

		strFunc.append(strTable_3);
		strFunc.append("_jsonStream.Free();\r\n");
		strFunc.append(strTable_3);
		strFunc.append("_jsonStream.CreateFirst();\r\n");

		strFunc.append(strTable_3);
		strFunc.append("switch(methodid)\r\n");

		strFunc.append(strTable_3);
		strFunc.append("{\r\n");

		for (auto& r : pClass._FuncVec) {
			if (r._bDiscard)
				continue;
			strFunc.append(strTable_3);
			string tt_funcIdName;
			string tt_funcIdFullName;

			strFunc.append("case ");
			switch (filetype) {
			case 0:
				sprintf(t_ch, "ID_%s_%s", pClass._name.c_str(), r._name.c_str());
				tt_funcIdName = t_ch;
				sprintf(t_ch, "%s::METHOD::%s", pNamespace._clientname.c_str(), tt_funcIdName.c_str());
				tt_funcIdFullName = t_ch;
				break;
			case 1:
				if (r._strSCClassName.length() > 0) {
					sprintf(t_ch, "ID_%s_%s", r._strSCClassName.c_str(), r._name.c_str());
					tt_funcIdName = t_ch;
					sprintf(t_ch, "%s::METHOD::%s", pNamespace._servername.c_str(), tt_funcIdName.c_str());
					tt_funcIdFullName = t_ch;
				}
				else {
					sprintf(t_ch, "ID_%s_%s", pClass._name.c_str(), r._name.c_str());
					tt_funcIdName = t_ch;
					sprintf(t_ch, "%s::METHOD::%s", pNamespace._servername.c_str(), tt_funcIdName.c_str());
					tt_funcIdFullName = t_ch;
				}

				break;
			case 2:
			{
				if (r._strSCClassName.length() > 0) {
					sprintf(t_ch, "ID_%s_%s", r._strSCClassName.c_str(), r._name.c_str());
					tt_funcIdName = t_ch;
					sprintf(t_ch, "%s::METHOD::%s", pNamespace._servername.c_str(), tt_funcIdName.c_str());
					tt_funcIdFullName = t_ch;
				}
				else {
					sprintf(t_ch, "ID_%s_%s", pClass._name.c_str(), r._name.c_str());
					tt_funcIdName = t_ch;
					sprintf(t_ch, "%s::METHOD::%s", pNamespace._servername.c_str(), tt_funcIdName.c_str());
					tt_funcIdFullName = t_ch;
				}


			}
			break;
			default:
				sprintf(t_ch, "ID_%s_%s", pClass._name.c_str(), r._name.c_str());
				tt_funcIdName = t_ch;
				sprintf(t_ch, "METHOD::ID_%s_%s", pClass._name.c_str(), r._name.c_str());
				tt_funcIdFullName = t_ch;
				break;
			}
			strFunc.append(t_ch);
			strFunc.append(" :\r\n");

			strFunc.append(strTable_4);
			sprintf(t_ch, "_jsonStream.WriteNewKey(\"%s\",0,false);\r\n", tt_funcIdName.c_str());
			strFunc.append(t_ch);
			strFunc.append(strTable_4);
			sprintf(t_ch, "_jsonStream<<%s;\r\n", tt_funcIdFullName.c_str());
			strFunc.append(t_ch);
			strFunc.append(strTable_4);
			strFunc.append("break;\r\n");

		}


		strFunc.append(strTable_3);
		strFunc.append("}\r\n");
		strFunc.append(strTable_2);
		strFunc.append("}\r\n");
	}

	strFunc.append(strTable_2);
	strFunc.append("switch(methodid)\r\n");

	strFunc.append(strTable_2);
	strFunc.append("{\r\n");

	for (auto& r : pClass._FuncVec) {
		if (r._bDiscard)
			continue;
		if (filetype == 2 && ((m_bSCSpliteS && r._SCSpliteType == -1) || (m_bSCSpliteC && r._SCSpliteType == 1)))
			continue;
		strFunc.append(strTable_2);
		switch (filetype) {
		case 0:
			sprintf(t_ch, "case %s::METHOD::ID_%s_%s :\r\n", pNamespace._clientname.c_str(), pClass._name.c_str(), r._name.c_str());
			break;
		case 1:
			if (r._strSCClassName.length() > 0) {
				sprintf(t_ch, "case %s::METHOD::ID_%s_%s :\r\n", pNamespace._servername.c_str(), r._strSCClassName.c_str(), r._name.c_str());

			}
			else
				sprintf(t_ch, "case %s::METHOD::ID_%s_%s :\r\n", pNamespace._servername.c_str(), pClass._name.c_str(), r._name.c_str());
			break;
		case 2:
		{
			if (r._strSCClassName.length() > 0) {
				sprintf(t_ch, "case %s::METHOD::ID_%s_%s :\r\n", pNamespace._servername.c_str(), r._strSCClassName.c_str(), r._name.c_str());
			}
			else
				sprintf(t_ch, "case %s::METHOD::ID_%s_%s :\r\n", pNamespace._servername.c_str(), pClass._name.c_str(), r._name.c_str());

		}

		break;
		default:
			sprintf(t_ch, "case METHOD::ID_%s_%s :\r\n", pClass._name.c_str(), r._name.c_str());
			break;
		}
		strFunc.append(t_ch);

		strFunc.append(strTable_3);
		switch (filetype) {
		case 0:
		{
			sprintf(t_ch, "_parser_%s(out,receiver,pExData,proversionno);\r\n", r._name.c_str());
		}
		break;
		case 1:
		{
			sprintf(t_ch, "_parser_%s(out,receiver,sessionId,pExData,proversionno);\r\n", r._name.c_str());
		}
		break;
		case 2:
		{
			sprintf(t_ch, "_parser_%s(out,receiver,sessionId,pExData,proversionno);\r\n", r._name.c_str());
		}
		break;
		default:
			break;
		}


		strFunc.append(t_ch);

		strFunc.append(strTable_3);
		strFunc.append("return true;\r\n");
	}

	strFunc.append(strTable_2);
	strFunc.append("}\r\n");

	strFunc.append(strTable_2);
	strFunc.append("return false;\r\n");

	strFunc.append(strTable);
	strFunc.append("}\r\n");

	fwrite(strFunc.c_str(), strFunc.length(), 1, fp);
}
void    ProtocolParse_Writer_CPlus::WriteClass_Send_CPlus(stClass& pClass, FILE* fp, int tablenum, int filetype, const string& spacename, stNamespace& pNamespace)
{
	char t_ch[512];
	string strTable = CreateTableString(tablenum);
	string strTable_2 = CreateTableString(tablenum + 1);

	string strClass;

	if (m_bFastModel && (filetype == 1 || filetype == 3)) {
		strClass.append(strTable);
		strClass.append("extern bool g_bUseFastModel;\r\n");
	}

	strClass.append(strTable);
	switch (filetype) {
	case 0:
	{
		sprintf(t_ch, "class SendData_%s\r\n", spacename.c_str());
	}
	break;
	case 1:
	{
		sprintf(t_ch, "class SendData_%s\r\n", spacename.c_str());
	}
	break;
	case 2:
	{
		sprintf(t_ch, "class SendData_%s\r\n", spacename.c_str());
	}
	break;
	default:
		break;
	}

	strClass.append(t_ch);

	strClass.append(strTable);
	strClass.append("{\r\n");
	strClass.append(strTable);
	strClass.append("public:\r\n");
	fwrite(strClass.c_str(), strClass.length(), 1, fp);
	strClass = "";

	for (auto& r : pClass._FuncVec) {
		if (r._bDiscard)
			continue;
		if (filetype == 2 && ((m_bSCSpliteS && r._SCSpliteType == 1) || (m_bSCSpliteC && r._SCSpliteType == -1)))
			continue;
		//写函数
		WriteFunc_SendByteSeqV2_CPlus(r, fp, tablenum + 1, filetype, pClass._name, pNamespace);
	}

	strClass.append(strTable);
	strClass.append("};\r\n");

	fwrite(strClass.c_str(), strClass.length(), 1, fp);
}

void    ProtocolParse_Writer_CPlus::WriteFunc_SendByteSeq_CPlus(stFunc& pFunc, FILE* fp, int tablenum, int filetype, string classname, stNamespace& pNamespace)
{
	char t_ch[512];
	string strTable = CreateTableString(tablenum);
	string strTable_2 = CreateTableString(tablenum + 1);

	string strFunc;

	strFunc.append(strTable);
	sprintf(t_ch, "static WJSByteSeq Send_%s(", pFunc._name.c_str());
	strFunc.append(t_ch);

	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		string strpar = CovertParToString_CPlus(pFunc._InParVec[i]);
		strFunc.append(strpar.c_str());

		//加,号
		if (i != pFunc._InParVec.size() - 1) {
			strFunc.append(",");
		}
	}

	strFunc.append(")\r\n");

	strFunc.append(strTable);
	strFunc.append("{\r\n");

	strFunc.append(strTable_2);
	sprintf(t_ch, "WJS_InByteStream ins;\r\n");
	strFunc.append(t_ch);

	strFunc.append(strTable_2);
	sprintf(t_ch, "ins<<%s;\r\n", m_ProtocolNo._name.c_str());
	strFunc.append(t_ch);

	strFunc.append(strTable_2);
	sprintf(t_ch, "ins<<(short)%d;\r\n", pFunc._version);
	strFunc.append(t_ch);

	strFunc.append(strTable_2);
	switch (filetype) {
	case 0:
		sprintf(t_ch, "ins<<%s::METHOD::ID_%s_%s;\r\n", pNamespace._clientname.c_str(), classname.c_str(), pFunc._name.c_str());
		break;
	case 1:
		sprintf(t_ch, "ins<<%s::METHOD::ID_%s_%s;\r\n", pNamespace._servername.c_str(), classname.c_str(), pFunc._name.c_str());
		break;
	case 2:
		sprintf(t_ch, "ins<<%s::METHOD::ID_%s_%s;\r\n", pNamespace._servername.c_str(), classname.c_str(), pFunc._name.c_str());
		break;
	default:
		sprintf(t_ch, "ins<<METHOD::ID_%s_%s;\r\n", classname.c_str(), pFunc._name.c_str());
		break;
	}


	strFunc.append(t_ch);

	for (auto& r : pFunc._InParVec) {
		strFunc.append(strTable_2);
		string tt_strNameSpace;
		if (IsExType_baseVector(r._strParType, filetype, tt_strNameSpace) && tt_strNameSpace.length() > 0) {
			sprintf(t_ch, "%s::operator <<(ins,%s);\r\n", tt_strNameSpace.c_str(), r._strParName.c_str());
		}
		else {
			sprintf(t_ch, "ins<<%s;\r\n", r._strParName.c_str());

		}

		strFunc.append(t_ch);
	}


	strFunc.append(strTable_2);
	strFunc.append("return ins;\r\n");

	strFunc.append(strTable);
	strFunc.append("}\r\n");

	fwrite(strFunc.c_str(), strFunc.length(), 1, fp);
}

void    ProtocolParse_Writer_CPlus::WriteFunc_SendByteSeqV2_CPlus(stFunc& pFunc, FILE* fp, int tablenum, int filetype, string classname, stNamespace& pNamespace)
{
	char t_ch[512];
	string strTable = CreateTableString(tablenum);
	string strTable_2 = CreateTableString(tablenum + 1);
	string strTable_3 = CreateTableString(tablenum + 3);

	string strFunc;

	strFunc.append(WriteBlockCommentBegin_CPlus(strTable));
	strFunc.append(WriteBlockCommentBrief_CPlus(strTable, pFunc._comment));

	for (auto& r : pFunc._InParVec) {
		strFunc.append(WriteBlockCommentPar_CPlus(strTable, r._comment, r._strParName).c_str());
	}
	strFunc.append(WriteBlockCommentEnd_CPlus(strTable));


	strFunc.append(strTable);
	sprintf(t_ch, "static void Send_%s(", pFunc._name.c_str());
	strFunc.append(t_ch);

	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		string strpar = CovertParToString_CPlus(pFunc._InParVec[i]);
		strFunc.append(strpar.c_str());

		//加,号
		if (i != pFunc._InParVec.size() - 1) {
			strFunc.append(",");
		}
	}

	if (pFunc._InParVec.size() > 0) {
		strFunc.append(",WJS_InByteStream& ins");
	}
	else {
		strFunc.append("WJS_InByteStream& ins");
	}

	strFunc.append(")\r\n");

	strFunc.append(strTable);
	strFunc.append("{\r\n");

	if (m_bFastModel) {
		strFunc.append(strTable_2);
		strFunc.append("if(g_bUseFastModel)\r\n");
		strFunc.append(strTable_3);
		strFunc.append("return;\r\n");
	}

	strFunc.append(strTable_2);
	sprintf(t_ch, "ins.OnlySetUseSize0();\r\n");
	strFunc.append(t_ch);

	strFunc.append(strTable_2);
	sprintf(t_ch, "ins<<%s;\r\n", m_ProtocolNo._name.c_str());
	strFunc.append(t_ch);

	strFunc.append(strTable_2);
	sprintf(t_ch, "ins<<(short)%d;\r\n", pFunc._version);
	strFunc.append(t_ch);

	strFunc.append(strTable_2);
	switch (filetype) {
	case 0:
		sprintf(t_ch, "ins<<%s::METHOD::ID_%s_%s;\r\n", pNamespace._clientname.c_str(), classname.c_str(), pFunc._name.c_str());
		break;
	case 1:
		if (pFunc._strSCClassName.length() > 0) {
			sprintf(t_ch, "ins<<%s::METHOD::ID_%s_%s;\r\n", pNamespace._servername.c_str(), pFunc._strSCClassName.c_str(), pFunc._name.c_str());

		}
		else
			sprintf(t_ch, "ins<<%s::METHOD::ID_%s_%s;\r\n", pNamespace._servername.c_str(), classname.c_str(), pFunc._name.c_str());
		break;
	case 2:
		if (pFunc._strSCClassName.length() > 0) {
			sprintf(t_ch, "ins<<%s::METHOD::ID_%s_%s;\r\n", pNamespace._servername.c_str(), pFunc._strSCClassName.c_str(), pFunc._name.c_str());

		}
		else
			sprintf(t_ch, "ins<<%s::METHOD::ID_%s_%s;\r\n", pNamespace._servername.c_str(), classname.c_str(), pFunc._name.c_str());
		break;
	default:
		sprintf(t_ch, "ins<<METHOD::ID_%s_%s;\r\n", classname.c_str(), pFunc._name.c_str());
		break;
	}
	strFunc.append(t_ch);

	for (auto& r : pFunc._InParVec) {
		strFunc.append(strTable_2);
		string tt_strNameSpace;
		if (IsExType_baseVector(r._strParType, filetype, tt_strNameSpace) && tt_strNameSpace.length() > 0) {
			sprintf(t_ch, "%s::operator <<(ins,%s);\r\n", tt_strNameSpace.c_str(), r._strParName.c_str());
		}
		else {
			sprintf(t_ch, "ins<<%s;\r\n", r._strParName.c_str());
		}

		strFunc.append(t_ch);
	}

	strFunc.append(strTable);
	strFunc.append("}\r\n");

	fwrite(strFunc.c_str(), strFunc.length(), 1, fp);
}




#pragma region 注释
string  ProtocolParse_Writer_CPlus::WriteComment_CPlus(const string& strTable, const string& strComment, const string& strPar)
{
	if (strComment.length() == 0)
		return "";
	string strLine;
	vector<string> tt_subList;
	ProtocolUtils::GetAllSubString(strComment, "split", tt_subList, false);

	for (size_t i = 0; i < tt_subList.size(); i++) {
		string tt_strReal;
		UTF8Decode(tt_subList[i].c_str(), tt_strReal);

		strLine.append(strTable);
		strLine.append("//");
		if (i == 0 && strPar.length() > 0) {
			strLine.append(strPar.c_str());
			strLine.append(" ");
		}
		strLine.append(tt_strReal.c_str());
		strLine.append("\r\n");
	}
	return strLine;
}
string  ProtocolParse_Writer_CPlus::WriteBlockCommentBegin_CPlus(const string& strTable)
{
	string strLine;
	strLine.append(strTable);
	strLine.append("/**\r\n");
	return strLine;
}
string  ProtocolParse_Writer_CPlus::WriteBlockCommentEnd_CPlus(const string& strTable)
{
	string strLine;
	strLine.append(strTable);
	strLine.append(" */\r\n");
	return strLine;
}
string  ProtocolParse_Writer_CPlus::WriteBlockCommentPar_CPlus(const string& strTable, const string& strComment, const string& strPar)
{
	if (strComment.length() == 0)
		return "";

	string strLine;

	vector<string> tt_subList;
	ProtocolUtils::GetAllSubString(strComment, "split", tt_subList, false);
	for (size_t i = 0; i < tt_subList.size(); i++) {
		string tt_strReal;
		UTF8Decode(tt_subList[i].c_str(), tt_strReal);
		if (i == 0 && strPar.length()>0) {
			strLine.append(strTable);
			strLine.append(" * @param ");
			strLine.append(strPar.c_str());
			strLine.append(" ");
			strLine.append(tt_strReal.c_str());
			strLine.append("\r\n");
		}
		else {
			strLine.append(strTable);
			strLine.append(" * ");
			strLine.append(tt_strReal.c_str());
			strLine.append("\r\n");
		}
	}

	return strLine;
}
string  ProtocolParse_Writer_CPlus::WriteBlockCommentBrief_CPlus(const string& strTable, const string& strComment)
{
	if (strComment.length() == 0)
		return "";

	string strLine;

	vector<string> tt_subList;
	ProtocolUtils::GetAllSubString(strComment, "split", tt_subList, false);
	for (size_t i = 0; i < tt_subList.size(); i++) {
		string tt_strReal;
		UTF8Decode(tt_subList[i].c_str(), tt_strReal);
		if (i == 0) {
			strLine.append(strTable);
			strLine.append(" * @brief ");
			strLine.append(" ");
			strLine.append(tt_strReal.c_str());
			strLine.append("\r\n");
		}
		else {
			strLine.append(strTable);
			strLine.append(" * ");
			strLine.append(tt_strReal.c_str());
			strLine.append("\r\n");
		}
	}

	return strLine;
}
#pragma endregion

string  ProtocolParse_Writer_CPlus::GetMemberTypeFullType_CPlus(const string& memberType)
{
	auto iter = _memberTypeFullTypeMap.find(memberType);
	if (iter != _memberTypeFullTypeMap.end()) {
		return iter->second;
	}
	return memberType;
}

bool ProtocolParse_Writer_CPlus::IsCanFastProcessVectorType_CPlus(const string& strtypename)
{
	auto iter = _canFastProcessVectorTypeSet.find(strtypename);
	if (iter != _canFastProcessVectorTypeSet.end())
		return true;

	return false;
}

string ProtocolParse_Writer_CPlus::CovertParToString_CPlus(stBasePar& pPar)
{
	char ch[256];
	ch[0] = '\0';

	if (pPar._IsConstRef) {
		sprintf(ch, "const %s& %s", GetMemberTypeFullType_CPlus(pPar._strParType).c_str(), pPar._strParName.c_str());
	}
	else {
		sprintf(ch, "%s %s", GetMemberTypeFullType_CPlus(pPar._strParType).c_str(), pPar._strParName.c_str());
	}


	return ch;

}