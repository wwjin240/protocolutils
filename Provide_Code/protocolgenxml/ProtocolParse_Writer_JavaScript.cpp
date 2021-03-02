#include "ProtocolParse_Writer_JavaScript.h"
#include "ProtocolParse.h"
#include <time.h>
#include <sys/timeb.h>

extern string g_strBaseType[];

ProtocolParse_Writer_JavaScript::ProtocolParse_Writer_JavaScript(const string& strPathName)
	:ProtocolParse_Writer(strPathName, eProtocolWriterType::JavaScript)
{

}
ProtocolParse_Writer_JavaScript::~ProtocolParse_Writer_JavaScript()
{

}
bool    ProtocolParse_Writer_JavaScript::CreateFile_Path(string path, int filetype, bool brefprotocol)
{
	return CreateFile_Path_JavaScript(path,filetype,brefprotocol);
}
ProtocolParse_Writer* ProtocolParse_Writer_JavaScript::Create_RefProtocolParse_Writer(const string& strPathName)
{
	return new ProtocolParse_Writer_JavaScript(strPathName);
}
bool    ProtocolParse_Writer_JavaScript::CreateFile_Path_JavaScript(string path, int filetype, bool brefprotocol)
{
	if (filetype < 0 || filetype>2)
		return false;

	//按协议名来建文建夹
	if (m_NamespaceVec.size() > 0) {
		if (brefprotocol) {
			char tt_ch[256];
			m_SaveFileName._servername = m_NamespaceVec[0]._protocolname;
			sprintf(tt_ch, "%sProtocol.js", m_NamespaceVec[0]._protocolname.c_str());
			m_SaveFileName._servername2 = tt_ch;

			m_SaveFileName._clientname = m_NamespaceVec[0]._protocolname;
			sprintf(tt_ch, "%sProtocol.js", m_NamespaceVec[0]._protocolname.c_str());
			m_SaveFileName._clientname2 = tt_ch;
		}
		else {
			char tt_ch[256];

			m_SaveFileName._servername = m_NamespaceVec[0]._protocolname;
			sprintf(tt_ch, "%sProtocol_server.js", m_NamespaceVec[0]._protocolname.c_str());
			m_SaveFileName._servername2 = tt_ch;

			m_SaveFileName._clientname = m_NamespaceVec[0]._protocolname;
			sprintf(tt_ch, "%sProtocol_client.js", m_NamespaceVec[0]._protocolname.c_str());
			m_SaveFileName._clientname2 = tt_ch;
		}

	}

	switch (filetype) {
	case 1://服务器
	{
		StartBiaoZhuUsed();
		for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++)
			m_pRefProtocolParseList[i]->StartBiaoZhuUsed();

		for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++) {
			//主要为了使得id不出现重复
			if (m_pRefProtocolParseList[i]) {
				m_pRefProtocolParseList[i]->m_pCheckParUsed = this;
				m_pRefProtocolParseList[i]->CreateFile_Path(path, filetype, true);
				m_pRefProtocolParseList[i]->GetRefProtocolMapPar(m_StringIntLogMap, m_IntStringMap, m_StringIntSearchMap);
			}
		}

		string str2 = path;
		str2.append(m_SaveFileName._servername2);
		return CreateFile_JavaScript(str2, filetype, brefprotocol);

	}
	break;
	case 0://客户端
	{
		//StartBiaoZhuUsed();
		//if (!m_bTScriptUseNameSpaceVersion) {
		//	for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++)
		//		m_pRefProtocolParseList[i]->StartBiaoZhuUsed();
		//}
		//for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++) {
		//	if (m_pRefProtocolParseList[i]) {
		//		m_pRefProtocolParseList[i]->m_pCheckParUsed = this;
		//		m_pRefProtocolParseList[i]->CreateFile_Path(path, filetype, true);
		//		m_pRefProtocolParseList[i]->GetRefProtocolMapPar(m_StringIntLogMap, m_IntStringMap, m_StringIntSearchMap);
		//	}
		//}

		//if (m_bTScriptUseNameSpaceVersion) {
		//	string str1 = path;
		//	string tt_strAddPath = m_SaveFileName._clientname;
		//	//ToLower(tt_strAddPath);
		//	str1.append(tt_strAddPath);
		//	return CreateFile_TScript_NSV(str1, filetype, brefprotocol);
		//}
		//else {
		//	string str2 = path;
		//	str2.append(m_SaveFileName._clientname2);
		//	return CreateFile_TScript_Model(str2, filetype, brefprotocol);

		//}

	}
	break;
	case 2: //合成
	{
		//StartBiaoZhuUsed();
		//if (!m_bTScriptUseNameSpaceVersion) {
		//	for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++)
		//		m_pRefProtocolParseList[i]->StartBiaoZhuUsed();
		//}
		//for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++) {
		//	//主要为了使得id不出现重复
		//	if (m_pRefProtocolParseList[i]) {
		//		m_pRefProtocolParseList[i]->m_pCheckParUsed = this;
		//		m_pRefProtocolParseList[i]->CreateFile_Path(path, filetype, true);
		//		m_pRefProtocolParseList[i]->GetRefProtocolMapPar(m_StringIntLogMap, m_IntStringMap, m_StringIntSearchMap);
		//	}
		//}

		//if (m_bTScriptUseNameSpaceVersion) {
		//	//先按服务器名存
		//	string str1 = path;
		//	string tt_strAddPath = m_SaveFileName._servername;
		//	//ToLower(tt_strAddPath);
		//	str1.append(tt_strAddPath);

		//	return CreateFile_TScript_NSV(str1, filetype, brefprotocol);
		//}
		//else {
		//	string str2 = path;
		//	str2.append(m_SaveFileName._servername2);
		//	return CreateFile_TScript_Model(str2, filetype, brefprotocol);
		//}
	}
	break;
	default:
		return false;
		break;
	}
	return false;
}

bool    ProtocolParse_Writer_JavaScript::CreateFile_JavaScript(string fileName, int filetype, bool bRefProtocol)
{
	return false;
	//CheckDirectoryEixst(fileName.c_str());

	//FILE* fp = fopen(fileName.c_str(), "wb+");
	//if (!fp)
	//	return false;

	//string tt_strLine;
	//char tt_ch[512];
	//if (m_pRefProtocolParseList.size() == 0) {
	//	//写信息头,共用
	//	WriteInfoHead(fp);
	//	tt_strLine = "import InStream = InStream;\n";
	//	fwrite(tt_strLine.c_str(), tt_strLine.length(), 1, fp);
	//	tt_strLine = "import OutStream = OutStream;\n";
	//	fwrite(tt_strLine.c_str(), tt_strLine.length(), 1, fp);
	//	tt_strLine = "import PArray = PArray;\n";
	//	fwrite(tt_strLine.c_str(), tt_strLine.length(), 1, fp);
	//}

	//for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++) {
	//	string tt_strTempName;
	//	switch (filetype) {
	//	case 0:
	//	{
	//		tt_strTempName = m_pRefProtocolParseList[i]->m_SaveFileName._clientname2;
	//	}
	//	break;
	//	case 1:
	//	{
	//		tt_strTempName = m_pRefProtocolParseList[i]->m_SaveFileName._servername2;
	//	}
	//	break;
	//	case 2:
	//	{
	//		tt_strTempName = m_pRefProtocolParseList[i]->m_SaveFileName._servername2;
	//	}
	//	break;
	//	default:
	//		break;
	//	}

	//	size_t tt_pos = tt_strTempName.rfind(".");
	//	if (tt_pos != string::npos)
	//		tt_strTempName = tt_strTempName.substr(0, tt_pos);
	//	sprintf(tt_ch, "import * as %s from  \"./%s\";\n", m_pRefProtocolParseList[i]->GetShortName().c_str(), tt_strTempName.c_str());

	//	tt_strLine = tt_ch;
	//	fwrite(tt_strLine.c_str(), tt_strLine.length(), 1, fp);
	//}
	////写命名空间
	//for (size_t i = 0; i < m_NamespaceVec.size(); i++) {
	//	WriteOneNameSpace_JavaScript(m_NamespaceVec[i], filetype, "", fp);
	//}

	//if (m_pRefProtocolParseList.size() > 0) {
	//	unsigned int tt_len = ftell(fp);
	//	//fflush(fp);
	//	char* tt_pBuf = new char[tt_len + 1];
	//	tt_pBuf[tt_len] = 0;
	//	fseek(fp, 0, SEEK_SET);
	//	size_t tt = fread(tt_pBuf, tt_len, 1, fp);
	//	fseek(fp, 0, SEEK_SET);

	//	//写信息头,共用
	//	WriteInfoHead(fp);

	//	tt_strLine = "import InStream = yy.InStream;\n";
	//	fwrite(tt_strLine.c_str(), tt_strLine.length(), 1, fp);
	//	tt_strLine = "import OutStream = yy.OutStream;\n";
	//	fwrite(tt_strLine.c_str(), tt_strLine.length(), 1, fp);
	//	tt_strLine = "import PArray = yy.PArray;\n";
	//	fwrite(tt_strLine.c_str(), tt_strLine.length(), 1, fp);
	//	tt_strLine = "import Int64 = yy.Int64;\n\n";
	//	fwrite(tt_strLine.c_str(), tt_strLine.length(), 1, fp);



	//	for (int i = 0; i < (int)m_pRefProtocolParseList.size(); i++) {
	//		string tt_strTempName;
	//		switch (filetype) {
	//		case 0:
	//		{
	//			tt_strTempName = m_pRefProtocolParseList[i]->m_SaveFileName._clientname2;

	//		}
	//		break;
	//		case 1:
	//		{
	//			tt_strTempName = m_pRefProtocolParseList[i]->m_SaveFileName._servername2;
	//		}
	//		break;
	//		case 2:
	//		{
	//			tt_strTempName = m_pRefProtocolParseList[i]->m_SaveFileName._servername2;

	//		}
	//		break;
	//		default:
	//			break;
	//		}

	//		size_t tt_pos = tt_strTempName.rfind(".");
	//		if (tt_pos != string::npos)
	//			tt_strTempName = tt_strTempName.substr(0, tt_pos);
	//		sprintf(tt_ch, "import * as %s from  \"./%s\";\n", m_pRefProtocolParseList[i]->GetShortName().c_str(), tt_strTempName.c_str());

	//		tt_strLine = tt_ch;
	//		fwrite(tt_strLine.c_str(), tt_strLine.length(), 1, fp);
	//	}
	//	for (unsigned int i = 0; i < m_ImportPacketNameList_TS_Model.size(); i++) {
	//		for (unsigned int j = i + 1; j < m_ImportPacketNameList_TS_Model.size();) {
	//			if (m_ImportPacketNameList_TS_Model[i].compare(m_ImportPacketNameList_TS_Model[j].c_str()) == 0) {
	//				m_ImportPacketNameList_TS_Model.erase(m_ImportPacketNameList_TS_Model.begin() + j);
	//			}
	//			else
	//				j++;
	//		}
	//	}
	//	for (unsigned int i = 0; i < m_ImportPacketNameList_TS_Model.size(); i++) {
	//		tt_strLine = m_ImportPacketNameList_TS_Model[i];
	//		tt_strLine.append("\n");
	//		fwrite(tt_strLine.c_str(), tt_strLine.length(), 1, fp);
	//	}

	//	fwrite(tt_pBuf, tt_len, 1, fp);
	//	delete[] tt_pBuf;
	//}


	//fclose(fp);
	//return true;
}