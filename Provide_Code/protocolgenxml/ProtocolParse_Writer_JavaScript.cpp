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


	}
	break;
	case 2: //合成
	{
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

}