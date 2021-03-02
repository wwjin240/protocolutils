#include "ProtocolParse.h"
#include <time.h>
#include <sys/timeb.h>
#include "CCreateComment.h"

#include "ProtocolParse_Writer_Java.h"

extern string g_ProgressId;
extern bool g_bCreateCopy;
extern bool g_bCreateCopy2;
extern string g_strBaseType[];
extern vector<string> g_AllTempFilesList;

ProtocolParse_Writer::ProtocolParse_Writer(const string& strPathName, eProtocolWriterType writerType)
	:m_strFileName(strPathName)
	,m_writerType(writerType)
	,m_bStdafx(false)
	, m_bNotSaveToFile(false)
	, m_bOutJson(false)
	, m_tablenum(0)
	, m_bWriteSortFuncId(false)
	, m_bSCSpliteS(false)
	, m_bSCSpliteC(false)
	, m_bFastModel(false)
	, m_bPureVirtual(false)
	, m_bTScriptUseNameSpaceVersion(true)
	
{
	m_strJavaPackage = "com.onyuan.DZPoker";

#ifdef _DEBUG
	_Logger.CreateLog("ProtocolLog", "protocol");
#endif
	CCreateComment tt_CreateComment(m_strFileName);
	tt_CreateComment.Parser();
	m_strFileName.append(".copy");
	m_strFileName.append(g_ProgressId.c_str());
	g_AllTempFilesList.push_back(m_strFileName);
}

ProtocolParse_Writer::~ProtocolParse_Writer()
{
	Free();
	for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++) {
		if (m_pRefProtocolParseList[i]) {
			delete m_pRefProtocolParseList[i];
		}
	}
	m_pRefProtocolParseList.clear();
}
void ProtocolParse_Writer::SetFileName(const string& strPathName)//设文件名
{
	m_strFileName = strPathName;
	m_strFileName.append(".copy");
	m_strFileName.append(g_ProgressId.c_str());
	g_AllTempFilesList.push_back(m_strFileName);
}
void ProtocolParse_Writer::Free()
{
	Free_();

	m_tablenum = 0;
	m_StringIntLogMap.Free();
	m_IntStringMap.Free();
	m_StringIntSearchMap.Free();
}
bool  ProtocolParse_Writer::Parser()//进行解析
{
	m_strXMLMd5 = ProtocolUtils::GetFileMd5(m_strFileName);
	m_strXMLMd5.append("BDM");

	//计算文件的MD5
	Free();
	return ParserXMLFile(m_strFileName);
}
void     ProtocolParse_Writer::FreeWriteLog()//清除写记录
{
	m_tablenum = 0;
	m_StringIntLogMap.Free();
	m_IntStringMap.Free();
	m_StringIntSearchMap.Free();

	for (auto& p : m_pRefProtocolParseList) {
		if (p) {
			p->FreeWriteLog();
		}
	}
}
string         ProtocolParse_Writer::GetShortName()
{
	if (m_NamespaceVec.size() > 0)
		return m_NamespaceVec[0]._shortname;
	return "";
}

ProtocolParse_Writer* ProtocolParse_Writer::Create_RefProtocolParse_Writer_Enter(const string& strPathName, bool bNotCreateFile)
{
	auto p = Create_RefProtocolParse_Writer(strPathName);
	if (!p)
		return p;

	p->m_bAndroidPeer = m_bAndroidPeer;
	p->m_bNotSaveToFile = bNotCreateFile;
	p->m_bUse4SpaceReplaceTable = m_bUse4SpaceReplaceTable;
	p->m_bOutJson = m_bOutJson;
	p->m_bTScriptUseNameSpaceVersion = m_bTScriptUseNameSpaceVersion;
	return p;

}

#pragma region 测试区
void   ProtocolParse_Writer::TestBaseParIsConstRef(stBasePar& pPar)
{
	//增加string类型，也使用constref
	if (pPar._strParType.compare("string") == 0 || pPar._strParType.compare("u16string")==0 || pPar._strParType.compare("wstring") == 0) {
		pPar._IsConstRef = true;
		return;
	}
	//不是基础类型，也不是枚举
	if (!IsBaseType(pPar._strParType) && (!IsExType_enum(pPar._strParType))) {
		if (m_pRefProtocolParseList.size() > 0) {
			for (auto& p : m_pRefProtocolParseList) {
				if (p)
					p->TestBaseParIsConstRef(pPar);
			}
		}
		else
			pPar._IsConstRef = true;
	}
}
void   ProtocolParse_Writer::RefProtocolProcess()
{
	if (m_RefProtocolList.size() > m_pRefProtocolParseList.size()) {
		int tt_index = m_RefProtocolList.size() - 1;
		bool tt_bUsePathFile = false;

		string tt_filePath;
		if (m_RefProtocolList[tt_index]._strPath.length() > 0) {
			tt_filePath = ProtocolUtils::ComputerPath(m_strFileName, m_RefProtocolList[tt_index]._strPath);
			tt_filePath.append("\\");
			tt_filePath.append(m_RefProtocolList[tt_index]._refProtocolName);

			//确认文件是否存在
			FILE* fp = fopen(tt_filePath.c_str(), "rb");
			if (fp) {
				tt_bUsePathFile = true;
				fclose(fp);
			}
		}

		if (!tt_bUsePathFile) {
			tt_filePath = m_strFileName;
			int npos1 = tt_filePath.find_last_of("\\");
			int npos2 = tt_filePath.find_last_of("/");
			int npos = npos1;
			if (npos < npos2)
				npos = npos2;
			if (npos == -1)
				return;
			else
				tt_filePath = m_strFileName.substr(0, npos);
			tt_filePath.append("\\");
			tt_filePath.append(m_RefProtocolList[tt_index]._refProtocolName);

		}


		ProtocolParse_Writer* tt_pRefProtocolWriter =  Create_RefProtocolParse_Writer_Enter(tt_filePath, m_RefProtocolList[tt_index]._bNotCreateFile);
		if (tt_pRefProtocolWriter) {
			if (!tt_pRefProtocolWriter->Parser()) {
				delete tt_pRefProtocolWriter;
				tt_pRefProtocolWriter = nullptr;
			}
			else {
				if (tt_pRefProtocolWriter->m_writerType == eProtocolWriterType::Java) {
					((ProtocolParse_Writer_Java*)tt_pRefProtocolWriter)->SetJavaPackName(m_strJavaPackage);
				}
				
				m_pRefProtocolParseList.push_back(tt_pRefProtocolWriter);
			}
		}

	}
}

#pragma endregion

#pragma region 类型判定
bool ProtocolParse_Writer::IsValidType(const string& strtypename)
{
	if (XMLParser::IsValidType(strtypename))
		return true;

	for (auto& p : m_pRefProtocolParseList) {
		if (p && p->IsValidType(strtypename)) {
			return true;
		}
	}

	m_strErrorReason = "invalid typename";
	return false;
}

bool ProtocolParse_Writer::IsExType_enum(const string& strtypename)
{
	if (XMLParser::IsExType_enum(strtypename))
		return true;

	for (auto& p : m_pRefProtocolParseList) {
		if (p && p->IsExType_enum(strtypename)) {
			return true;
		}
	}

	return false;
}

bool ProtocolParse_Writer::IsExType_struct(const string& strtypename)
{
	if (XMLParser::IsExType_struct(strtypename))
		return true;

	if (m_pRefProtocolParseList.size() > 0) {
		for (auto& p : m_pRefProtocolParseList) {
			if (p && p->IsExType_struct(strtypename))
				return true;
		}
	}
	return false;
}

bool ProtocolParse_Writer::IsExType_vector(const string& strtypename)
{
	if (XMLParser::IsExType_vector(strtypename))
		return true;

	if (m_pRefProtocolParseList.size() > 0) {
		for (auto& p : m_pRefProtocolParseList) {
			if (p && p->IsExType_vector(strtypename))
				return true;
		}
	}
	return false;
}
bool ProtocolParse_Writer::IsExType_WJSVector(const string& strtypename) //是否是另外版本的类vector型
{
	if (XMLParser::IsExType_WJSVector(strtypename))
		return true;
	if (m_pRefProtocolParseList.size() > 0) {
		for (auto& p : m_pRefProtocolParseList) {
			if (p && p->IsExType_WJSVector(strtypename))
				return true;
		}
	}

	return false;
}
#pragma endregion

#pragma region 类型判定扩展
string ProtocolParse_Writer::GetExTypeParT_vector(const string& strtypename)
{
	if (m_NamespaceVec.size() == 0)
		return "";
	vector<stVector> pVector = m_NamespaceVec[m_NamespaceVec.size() - 1]._VectorVec;

	for (size_t i = 0; i < pVector.size(); i++) {
		if (pVector[i]._name.compare(strtypename) == 0) {
			return pVector[i]._ParTName;
		}
	}

	if (m_pRefProtocolParseList.size() > 0) {
		for (auto& p : m_pRefProtocolParseList) {
			if (p) {
				string tt_str = p->GetExTypeParT_vector(strtypename);
				if (tt_str.length() > 0)
					return tt_str;
			}
		}
	}
	return "";

}
string ProtocolParse_Writer::GetExTypeParT_WJSVector(const string& strtypename)
{
	if (m_NamespaceVec.size() == 0)
		return "";
	const vector<stWJSVector>& pWJSVector = m_NamespaceVec[m_NamespaceVec.size() - 1]._WJSVectorVec;

	for (const auto& r : pWJSVector) {
		if (r._name.compare(strtypename) == 0) {
			return r._ParTName;
		}
	}

	if (m_pRefProtocolParseList.size() > 0) {
		for (auto& p : m_pRefProtocolParseList) {
			if (p) {
				string tt_str = p->GetExTypeParT_WJSVector(strtypename);
				if (tt_str.length() > 0)
					return tt_str;
			}
		}
	}
	return "";
}
int  ProtocolParse_Writer::IsExType_baseVector_DiGui(const string& strtypename, int filetype, string& strNameSpace)
{
	if (m_NamespaceVec.size() == 0)
		return 0;
	const vector<stVector>& pVector = m_NamespaceVec[m_NamespaceVec.size() - 1]._VectorVec;

	for (auto& r : pVector) {
		if (r._name.compare(strtypename) == 0) {
			if (r._bBaseParT) {
				switch (filetype) {
				case 0:
					strNameSpace = m_NamespaceVec.back()._clientname;
					break;
				case 1:
				case 2:
					strNameSpace = m_NamespaceVec.back()._servername;
					break;
				default:
					return -1;
				}

				return 1;
			}

			return -1;
		}
	}


	const vector<stWJSVector>& pWJSVector = m_NamespaceVec.back()._WJSVectorVec;
	for (auto& r : pWJSVector) {
		if (r._name.compare(strtypename) == 0) {
			if (r._bBaseParT) {
				switch (filetype) {
				case 0:
					strNameSpace = m_NamespaceVec.back()._clientname;
					break;
				case 1:
				case 2:
					strNameSpace = m_NamespaceVec.back()._servername;
					break;
				default:
					return -1;
				}

				return 1;
			}

			return -1;
		}
	}

	if (m_pRefProtocolParseList.size() > 0) {
		for (auto& p : m_pRefProtocolParseList) {
			if (p) {
				int tt_ret = p->IsExType_baseVector_DiGui(strtypename, filetype, strNameSpace);
				if (tt_ret != 0)
					return tt_ret;
			}
		}
	}

	return 0;
}
bool ProtocolParse_Writer::IsExType_baseVector(const string& strtypename, int filetype, string& strNameSpace)
{
	if (IsExType_baseVector_DiGui(strtypename, filetype, strNameSpace) == 1)
		return true;
	return false;
}

#pragma endregion

void    ProtocolParse_Writer::WriteInfoHead(FILE* fp, bool bLua, bool bPython)//写头
{
	//先写对应XML的MD5
	char tt_ch[512];
	if (bLua)
		sprintf(tt_ch, "--MD5=%s\n", m_strXMLMd5.c_str());
	else if (bPython)
		sprintf(tt_ch, "#MD5=%s\n", m_strXMLMd5.c_str());
	else
		sprintf(tt_ch, "//MD5=%s\n", m_strXMLMd5.c_str());


	string tt_strLine = tt_ch;
	fwrite(tt_strLine.c_str(), tt_strLine.length(), 1, fp);


	ProtocolHead head;
	if (bLua)
		head.SetLua();
	else if (bPython)
		head.SetPython();
	//得到生成时间
	struct tm *today;
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
		sprintf(strBuffer, "--protocolgen:[%d-%02d-%02d %02d:%02d:%02d]\n"
			, today->tm_year + 1900
			, today->tm_mon + 1
			, today->tm_mday
			, today->tm_hour
			, today->tm_min
			, today->tm_sec
		);
	}
	else if (bPython) {
		sprintf(strBuffer, "#protocolgen:[%d-%02d-%02d %02d:%02d:%02d]\n"
			, today->tm_year + 1900
			, today->tm_mon + 1
			, today->tm_mday
			, today->tm_hour
			, today->tm_min
			, today->tm_sec
		);
	}
	else {
		sprintf(strBuffer, "//protocolgen:[%d-%02d-%02d %02d:%02d:%02d]\n"
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
void    ProtocolParse_Writer::WriteStartZhongKuoKao(FILE* fp)
{
	char ch[10];
	sprintf(ch, "{\n");
	size_t len = strlen(ch);
	fwrite(ch, len, 1, fp);
}

void    ProtocolParse_Writer::WriteEndZhongKuoKao(FILE* fp)
{
	char ch[10];
	sprintf(ch, "}\n");
	size_t len = strlen(ch);
	fwrite(ch, len, 1, fp);
}

string  ProtocolParse_Writer::GetDefaultValue(const string& str, bool bFlash, bool bJava, bool bCSharp, bool bTScript, bool bPython)
{
	for (int i = 0; i < GetBaseTypeNum(); i++) {
		if (g_strBaseType[i].compare(str.c_str()) == 0) {
			switch (i) {
			case 0://void
				break;
			case 1://int	
			case 2://unsigned int
			case 3://short
			case 4://unsigned short
				return "0";

			case 5://char
			{
				if (bJava) {
					return "0";
				}
			}
			return "";
			//break;

			case 11://float
			case 12://long
			case 13://unsigned long
				return "0";


			case 16://wjs_char32
			case 17://wjs_char64
			case 18://wjs_char128
			case 19://wjs_char256
			case 20://wjs_char512
			case 21://wjs_char1024
				return "";
				//break;
			case 6://byte
			case 7://BYTE
			case 23:
				return "0";
			case 9://
			{
				if (bJava) {
					return "0";
				}
				if (bPython)
					return "0";
			}
			return "FALSE";
			case 8://bool
			{
				if (bPython)
					return "False";
			}
			return "false";
			case 10://DWORD
				return "0";
			case 14://string
			case 24://wjs_utf8
			case 25://u16string
			case 26://wstring
			{
				if (bJava) {
					return "new String()";
				}
				else if (bCSharp) {
					return "\"\"";
				}
				else if (bPython) {
					return "\"\"";
				}
			}
			return "";
			case 15://__int64
			case 22: //long long
			{
				if (bFlash)
					return "new Int64";
				if (bTScript)
					return "new Int64()";
				if (bPython)
					return "0";
				return "0";

			}

			}
			break;
		}
	}

	return "";
}



#pragma region 包名
bool    ProtocolParse_Writer::IsFlashBaseType(const string& strTypeName, bool bWrite)
{
	if (IsBaseType_WriterType(strTypeName, eProtocolWriterType::Flash))
		return true;
	

	if (bWrite) {
		//枚举类型作为基本类型
		if (IsExType_enum(strTypeName))
			return true;
	}


	return false;
}
void    ProtocolParse_Writer::GetUsePacketNamesByParType(const string& strParType, vector<string>& packetNamesList)
{
	for (auto& r : m_NamespaceVec) {
		for (auto& j : r._StructVec) {
			if (j._name.compare(strParType.c_str()) == 0) {
				AddPacketNames(packetNamesList, r._protocolnamelower);

				for (auto& k : j._ParVec) {
					if (!IsFlashBaseType(k._strParType, false)) {
						GetUsePacketNamesByParType(k._strParType, packetNamesList);
					}
				}

				return;
			}
		}

		for (auto& j : r._VectorVec) {
			if (j._name.compare(strParType.c_str()) == 0) {
				AddPacketNames(packetNamesList, r._protocolnamelower);

				if (!IsFlashBaseType(j._ParTName, false)) {
					GetUsePacketNamesByParType(j._ParTName, packetNamesList);
				}

				return;
			}
		}


		for (auto& j : r._WJSVectorVec) {
			if (j._name.compare(strParType.c_str()) == 0) {
				AddPacketNames(packetNamesList, r._protocolnamelower);

				if (!IsFlashBaseType(j._ParTName, false)) {
					GetUsePacketNamesByParType(j._ParTName, packetNamesList);
				}

				return;
			}
		}

		for (auto& j : r._EnumVec) {
			if (j._name.compare(strParType.c_str()) == 0) {
				AddPacketNames(packetNamesList, r._protocolnamelower);
				return;
			}
		}

	}


	for (auto& p : m_pRefProtocolParseList) {
		if (p)
			p->GetUsePacketNamesByParType(strParType, packetNamesList);
	}

}

void    ProtocolParse_Writer::AddPacketNames(vector<string>& packetNamesList, const string& strNewPacketName)
{
	for (auto& r : packetNamesList) {
		if (r.compare(strNewPacketName.c_str()) == 0)
			return;
	}

	packetNamesList.push_back(strNewPacketName);

}

void    ProtocolParse_Writer::RemovePacketNames(vector<string>& packetNamesList, const string& strRemovePacketName, bool bOnlyOne)
{
	for (int i = 0; i < (int)packetNamesList.size(); i++) {
		if (packetNamesList[i].compare(strRemovePacketName.c_str()) == 0) {
			packetNamesList.erase(packetNamesList.begin() + i);
			if (bOnlyOne)
				return;
			else
				i--;
		}
	}


}


void    ProtocolParse_Writer::GetUsePacketNamesByRefHead(vector<string>& packetNamesList, bool bXiaoXie)
{
	for (auto& r : m_RefProtocolList) {
		if (r._usingspacename.length() > 0) {
			if (bXiaoXie) {
				AddPacketNames(packetNamesList, ToLowerV2(r._usingspacename));

			}
			else {
				AddPacketNames(packetNamesList, r._usingspacename);
			}
		}
	}

	for (auto& p : m_pRefProtocolParseList) {
		if (p)
			p->GetUsePacketNamesByRefHead(packetNamesList, bXiaoXie);
	}
}
#pragma endregion

