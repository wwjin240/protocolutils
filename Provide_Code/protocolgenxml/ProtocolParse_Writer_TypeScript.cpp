#include "ProtocolParse_Writer_TypeScript.h"
#include "ProtocolParse.h"
#include <time.h>
#include <sys/timeb.h>

extern string g_strBaseType[];

ProtocolParse_Writer_TypeScript::ProtocolParse_Writer_TypeScript(const string& strPathName)
	:ProtocolParse_Writer(strPathName, eProtocolWriterType::TypeScript)
{

}

ProtocolParse_Writer_TypeScript::~ProtocolParse_Writer_TypeScript()
{

}

bool    ProtocolParse_Writer_TypeScript::CreateFile_Path(string path, int filetype, bool brefprotocol)
{
	return CreateFile_Path_TScript(path,filetype,brefprotocol);
}
ProtocolParse_Writer* ProtocolParse_Writer_TypeScript::Create_RefProtocolParse_Writer(const string& strPathName)
{
	return new ProtocolParse_Writer_TypeScript(strPathName);
}
bool    ProtocolParse_Writer_TypeScript::CreateFile_Path_TScript(string path, int filetype, bool brefprotocol, bool bSaveToFile)
{
	if (filetype < 0 || filetype>2)
		return false;


	//按协议名来建文建夹
	if (m_NamespaceVec.size() > 0) {
		if (brefprotocol) {
			char tt_ch[256];
			m_SaveFileName._servername = m_NamespaceVec[0]._protocolname;
			sprintf(tt_ch, "%sProtocol.ts", m_NamespaceVec[0]._protocolname.c_str());
			m_SaveFileName._servername2 = tt_ch;

			m_SaveFileName._clientname = m_NamespaceVec[0]._protocolname;
			sprintf(tt_ch, "%sProtocol.ts", m_NamespaceVec[0]._protocolname.c_str());
			m_SaveFileName._clientname2 = tt_ch;
		}
		else {
			char tt_ch[256];

			m_SaveFileName._servername = m_NamespaceVec[0]._protocolname;
			sprintf(tt_ch, "%sProtocol_server.ts", m_NamespaceVec[0]._protocolname.c_str());
			m_SaveFileName._servername2 = tt_ch;

			m_SaveFileName._clientname = m_NamespaceVec[0]._protocolname;
			sprintf(tt_ch, "%sProtocol_client.ts", m_NamespaceVec[0]._protocolname.c_str());
			m_SaveFileName._clientname2 = tt_ch;
		}

	}
	switch (filetype) {
	case 1://服务器
	{
		StartBiaoZhuUsed();
		if (!m_bTScriptUseNameSpaceVersion) {
			for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++)
				m_pRefProtocolParseList[i]->StartBiaoZhuUsed();
		}

		for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++) {
			//主要为了使得id不出现重复
			if (m_pRefProtocolParseList[i]) {
				m_pRefProtocolParseList[i]->m_pCheckParUsed = this;
				m_pRefProtocolParseList[i]->CreateFile_Path(path, filetype, true);
				m_pRefProtocolParseList[i]->GetRefProtocolMapPar(m_StringIntLogMap, m_IntStringMap, m_StringIntSearchMap);
			}
		}




		if (m_bTScriptUseNameSpaceVersion) {
			string str1 = path;
			string tt_strAddPath = m_SaveFileName._servername;
			//ToLower(tt_strAddPath);
			str1.append(tt_strAddPath);
			return CreateFile_TScript_NSV(str1, filetype, brefprotocol);
		}
		else {
			string str2 = path;
			str2.append(m_SaveFileName._servername2);
			return CreateFile_TScript_Model(str2, filetype, brefprotocol);
		}


	}
	break;
	case 0://客户端
	{
		StartBiaoZhuUsed();
		if (!m_bTScriptUseNameSpaceVersion) {
			for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++)
				m_pRefProtocolParseList[i]->StartBiaoZhuUsed();
		}
		for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++) {
			if (m_pRefProtocolParseList[i]) {
				m_pRefProtocolParseList[i]->m_pCheckParUsed = this;
				m_pRefProtocolParseList[i]->CreateFile_Path(path, filetype, true);
				m_pRefProtocolParseList[i]->GetRefProtocolMapPar(m_StringIntLogMap, m_IntStringMap, m_StringIntSearchMap);
			}
		}

		if (m_bTScriptUseNameSpaceVersion) {
			string str1 = path;
			string tt_strAddPath = m_SaveFileName._clientname;
			//ToLower(tt_strAddPath);
			str1.append(tt_strAddPath);
			return CreateFile_TScript_NSV(str1, filetype, brefprotocol);
		}
		else {
			string str2 = path;
			str2.append(m_SaveFileName._clientname2);
			return CreateFile_TScript_Model(str2, filetype, brefprotocol);

		}

	}
	break;
	case 2: //合成
	{
		StartBiaoZhuUsed();
		if (!m_bTScriptUseNameSpaceVersion) {
			for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++)
				m_pRefProtocolParseList[i]->StartBiaoZhuUsed();
		}
		for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++) {
			//主要为了使得id不出现重复
			if (m_pRefProtocolParseList[i]) {
				m_pRefProtocolParseList[i]->m_pCheckParUsed = this;
				m_pRefProtocolParseList[i]->CreateFile_Path(path, filetype, true);
				m_pRefProtocolParseList[i]->GetRefProtocolMapPar(m_StringIntLogMap, m_IntStringMap, m_StringIntSearchMap);
			}
		}

		if (m_bTScriptUseNameSpaceVersion) {
			//先按服务器名存
			string str1 = path;
			string tt_strAddPath = m_SaveFileName._servername;
			//ToLower(tt_strAddPath);
			str1.append(tt_strAddPath);

			return CreateFile_TScript_NSV(str1, filetype, brefprotocol);
		}
		else {
			string str2 = path;
			str2.append(m_SaveFileName._servername2);
			return CreateFile_TScript_Model(str2, filetype, brefprotocol);
		}
	}
	break;
	default:
		return false;
		break;
	}

}



void    ProtocolParse_Writer_TypeScript::GetUsePacketNamesByRefHead_TScript(vector<string>& packetNamesList)
{
	for (int i = 0; i < (int)m_RefProtocolList.size(); i++) {
		if (m_RefProtocolList[i]._usingspacename.length() > 0) {
			AddPacketNames(packetNamesList, m_RefProtocolList[i]._usingspacename);
		}
	}

	for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++) {
		if (m_pRefProtocolParseList[i])
			m_pRefProtocolParseList[i]->GetUsePacketNamesByRefHead(packetNamesList, false);
	}
}






string  ProtocolParse_Writer_TypeScript::ConvertDefaultValue(const string& strTypeName, const string& strDefaultValue)
{
	string tt_strDefaultValue = strDefaultValue;
	size_t tt_pos = tt_strDefaultValue.rfind("f");
	if (tt_pos != string::npos && tt_pos != 0) {
		tt_strDefaultValue = tt_strDefaultValue.substr(0, tt_pos);
	}
	if (strTypeName.compare("__int64") == 0 || strTypeName.compare("long long") == 0 || strTypeName.compare("Int64") == 0) {
		char tt_ch[128];
		sprintf(tt_ch, "new Int64(%s,0)", tt_strDefaultValue.c_str());
		return tt_ch;
	}

	return tt_strDefaultValue;
}

void    ProtocolParse_Writer_TypeScript::GetUsePacketNamesByParType_TScript(const string& strParType, vector<string>& packetNamesList, const string& selffoldername)
{

	for (int i = 0; i < (int)m_NamespaceVec.size(); i++) {
		for (int j = 0; j < (int)m_NamespaceVec[i]._StructVec.size(); j++) {
			if (m_NamespaceVec[i]._StructVec[j]._name.compare(strParType.c_str()) == 0) {
				char tt_ch[256];
				if (selffoldername.compare(m_NamespaceVec[i]._protocolname.c_str()) == 0)
					sprintf(tt_ch, "import {%s} from  \"./%s\"", m_NamespaceVec[i]._StructVec[j]._name.c_str(), m_NamespaceVec[i]._StructVec[j]._name.c_str());
				else
					sprintf(tt_ch, "import {%s} from  \"../%s/%s\"", m_NamespaceVec[i]._StructVec[j]._name.c_str(), m_NamespaceVec[i]._protocolname.c_str(), m_NamespaceVec[i]._StructVec[j]._name.c_str());
				AddPacketNames(packetNamesList, tt_ch);

				for (int k = 0; k < (int)m_NamespaceVec[i]._StructVec[j]._ParVec.size(); k++) {
					if (!IsTScriptBaseType(m_NamespaceVec[i]._StructVec[j]._ParVec[k]._strParType, false, true)) {
						GetUsePacketNamesByParType_TScript(m_NamespaceVec[i]._StructVec[j]._ParVec[k]._strParType, packetNamesList, selffoldername);
					}
				}

				return;
			}
		}

		for (int j = 0; j < (int)m_NamespaceVec[i]._VectorVec.size(); j++) {
			if (m_NamespaceVec[i]._VectorVec[j]._name.compare(strParType.c_str()) == 0) {
				char tt_ch[256];
				if (selffoldername.compare(m_NamespaceVec[i]._protocolname.c_str()) == 0)
					sprintf(tt_ch, "import {%s} from  \"./%s\"", m_NamespaceVec[i]._VectorVec[j]._name.c_str(), m_NamespaceVec[i]._VectorVec[j]._name.c_str());
				else
					sprintf(tt_ch, "import {%s} from  \"../%s/%s\"", m_NamespaceVec[i]._VectorVec[j]._name.c_str(), m_NamespaceVec[i]._protocolname.c_str(), m_NamespaceVec[i]._VectorVec[j]._name.c_str());

				AddPacketNames(packetNamesList, tt_ch);

				if (!IsTScriptBaseType(m_NamespaceVec[i]._VectorVec[j]._ParTName, false, true)) {
					//if (m_NamespaceVec[i]._VectorVec[j]._ParTName.compare("PtCatchFishJoyRank") == 0)
					//{
					//	int a;
					//	a = 10;
					//}
					GetUsePacketNamesByParType_TScript(m_NamespaceVec[i]._VectorVec[j]._ParTName, packetNamesList, selffoldername);
				}
				return;
			}
		}

		for (int j = 0; j < (int)m_NamespaceVec[i]._WJSVectorVec.size(); j++) {
			if (m_NamespaceVec[i]._WJSVectorVec[j]._name.compare(strParType.c_str()) == 0) {
				char tt_ch[256];
				if (selffoldername.compare(m_NamespaceVec[i]._protocolname.c_str()) == 0)
					sprintf(tt_ch, "import {%s} from  \"./%s\"", m_NamespaceVec[i]._WJSVectorVec[j]._name.c_str(), m_NamespaceVec[i]._WJSVectorVec[j]._name.c_str());
				else
					sprintf(tt_ch, "import {%s} from  \"../%s/%s\"", m_NamespaceVec[i]._WJSVectorVec[j]._name.c_str(), m_NamespaceVec[i]._protocolname.c_str(), m_NamespaceVec[i]._WJSVectorVec[j]._name.c_str());

				AddPacketNames(packetNamesList, tt_ch);
				if (!IsTScriptBaseType(m_NamespaceVec[i]._WJSVectorVec[j]._ParTName, false, true)) {
					GetUsePacketNamesByParType_TScript(m_NamespaceVec[i]._WJSVectorVec[j]._ParTName, packetNamesList, selffoldername);
				}

				return;
			}
		}

		for (int j = 0; j < (int)m_NamespaceVec[i]._EnumVec.size(); j++) {
			if (m_NamespaceVec[i]._EnumVec[j]._name.compare(strParType.c_str()) == 0) {
				char tt_ch[256];
				if (selffoldername.compare(m_NamespaceVec[i]._protocolname.c_str()) == 0)
					sprintf(tt_ch, "import {%s} from  \"./%s\"", m_NamespaceVec[i]._EnumVec[j]._name.c_str(), m_NamespaceVec[i]._EnumVec[j]._name.c_str());
				else
					sprintf(tt_ch, "import {%s} from  \"../%s/%s\"", m_NamespaceVec[i]._EnumVec[j]._name.c_str(), m_NamespaceVec[i]._protocolname.c_str(), m_NamespaceVec[i]._EnumVec[j]._name.c_str());

				AddPacketNames(packetNamesList, tt_ch);
				return;
			}
		}
	}

	for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++) {
		if (m_pRefProtocolParseList[i])
			((ProtocolParse_Writer_TypeScript*)m_pRefProtocolParseList[i])->GetUsePacketNamesByParType_TScript(strParType, packetNamesList, selffoldername);
	}

}

string  ProtocolParse_Writer_TypeScript::GetSubFolderName(const string& foldername)
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

string  ProtocolParse_Writer_TypeScript::WriteComment_TScript(const string& strTable, const string& strComment, const string& strPar, bool bWithBlock)
{
	if (strComment.length() == 0)
		return "";
	char tt_ch[2048];

	string strLine;

	vector<string> tt_subList;
	ProtocolUtils::GetAllSubString(strComment, "split", tt_subList, false);

	if (tt_subList.size() > 0 && bWithBlock) {
		strLine.append(strTable);
		strLine.append("/**\n");
	}



	for (unsigned int i = 0; i < tt_subList.size(); i++) {
		string tt_strReal;
		//UTF8Decode(tt_subList[i].c_str(), tt_strReal);
		tt_strReal = tt_subList[i];
		strLine.append(strTable);
		if (strPar.length() > 0 && i == 0) {
			sprintf(tt_ch, "@param %s  %s\n", strPar.c_str(), tt_strReal.c_str());

		}
		else {
			sprintf(tt_ch, "%s\n", tt_strReal.c_str());
		}

		strLine.append(tt_ch);

	}

	if (tt_subList.size() > 0 && bWithBlock) {
		strLine.append(strTable);
		strLine.append("*/\n");
	}
	return strLine;
}
#pragma region 类型转换与判断
bool    ProtocolParse_Writer_TypeScript::IsTScriptBaseType(const string& strTypeName, bool bWrite, bool bNotReadAndWrite)
{
	for (int i = 0; i < GetBaseTypeNum(); i++) {
		if (g_strBaseType[i].compare(strTypeName.c_str()) == 0) {
			switch (i) {
			case 15://__int64
			case 22://long long
			{
				if (bNotReadAndWrite)
					return false;
			}
			return true;
			default:
				return true;
			}
		}
	}

	if (bWrite) {
		//枚举类型作为基本类型
		if (IsExType_enum(strTypeName))
			return true;
	}


	return false;
}
bool    ProtocolParse_Writer_TypeScript::IsTScriptBaseType2(string strTypeName)
{
	for (int i = 0; i < GetBaseTypeNum(); i++) {
		if (g_strBaseType[i].compare(strTypeName.c_str()) == 0) {
			if (g_strBaseType[i].compare("__int64") == 0 || g_strBaseType[i].compare("long long") == 0)
				return false;

			return TRUE;
		}
	}

	return FALSE;

}
string  ProtocolParse_Writer_TypeScript::TypeConvertToTScriptType(const string& strTypeName)
{
	for (int i = 0; i < GetBaseTypeNum(); i++) {
		if (g_strBaseType[i].compare(strTypeName.c_str()) == 0) {
			switch (i) {
			case 0://void
				return strTypeName;
			case 1://int
				return "number";
			case 2://unsigned int
				return "number";
			case 3://short
				return "number";
			case 4://unsigned short
				return "number";
			case 5://char
				return "number";


			case 11://float
				return "number";
			case 12://long
				return "number";
			case 13://unsigned long
				return "number";



			case 16://wjs_char32
			case 17://wjs_char64
			case 18://wjs_char128
			case 19://wjs_char256
			case 20://wjs_char512
			case 21://wjs_char1024
				return "string";
				//break;
			case 6://byte
			case 7://BYTE
			case 23:
				return "number";
			case 9://BOOL
				return "number";
			case 8://bool
				return "boolean";
			case 10://DWORD
				return "number";
			case 14://string
			case 24://wjs_utf8
				return "string";
			case 15://__int64
			case 22://long long
				return "Int64";
			case 25://u16string
				return "string";
			case 26://wstring
				return "string";
			}
			break;

		}
	}

	if (IsExType_enum(strTypeName)) {
		//string tt_temp;
		//tt_temp="enum ";
		//tt_temp.append(str);
		//return tt_temp;
		return "number";
	}

	return strTypeName;
}
string  ProtocolParse_Writer_TypeScript::TypeConvertToReadTScriptType(const string& strTypeName)
{
	for (int i = 0; i < GetBaseTypeNum(); i++) {
		if (g_strBaseType[i].compare(strTypeName.c_str()) == 0) {
			switch (i) {
			case 0://void
				return "readInt32";
			case 1://int
				return "readInt32";
			case 2://unsigned int
				return "readUint32";
			case 3://short
				return "readInt16";
			case 4://unsigned short
				return "readUint16";
			case 5://char
				return "readInt8";


			case 11://float
				return "readFloat";
			case 12://long
				return "readInt32";
			case 13://unsigned long
				return "readUint32";



			case 16://wjs_char32
			case 17://wjs_char64
			case 18://wjs_char128
			case 19://wjs_char256
			case 20://wjs_char512
			case 21://wjs_char1024
				return "readString";
				//break;
			case 6://byte
			case 7://BYTE
			case 23:
				return "readUint8";
			case 9://BOOL
				return "readInt32";
			case 8://bool
				return "readBoolean";
			case 10://DWORD
				return "readInt32";
			case 14://string
				return "readString";
			case 24://wjs_utf8
				return "readString";
			case 15://__int64
			case 22://long long
				return "readInt64";
			case 25://u16string
				return "readUtf8";
			case 26://wstring
				return "readUtf8";
			}
			break;

		}
	}

	if (IsExType_enum(strTypeName)) {
		return "readInt32";
	}

	return "";
}
string  ProtocolParse_Writer_TypeScript::TypeConvertToWriteTScriptType(const string& strTypeName)
{
	for (int i = 0; i < GetBaseTypeNum(); i++) {
		if (g_strBaseType[i].compare(strTypeName.c_str()) == 0) {
			switch (i) {
			case 0://void
				return "writeInt32";
			case 1://int
				return "writeInt32";
			case 2://unsigned int
				return "writeUint32";
			case 3://short
				return "writeInt16";
			case 4://unsigned short
				return "writeUint16";
			case 5://char
				return "writeInt8";


			case 11://float
				return "writeFloat";
			case 12://long
				return "writeInt32";
			case 13://unsigned long
				return "writeUint32";



			case 16://wjs_char32
			case 17://wjs_char64
			case 18://wjs_char128
			case 19://wjs_char256
			case 20://wjs_char512
			case 21://wjs_char1024
				return "writeString";
				//break;
			case 6://byte
			case 7://BYTE
			case 23:
				return "writeUint8";
			case 9://BOOL
				return "writeInt32";
			case 8://bool
				return "writeBoolean";
			case 10://DWORD
				return "writeInt32";
			case 14://string
				return "writeString";
			case 24://wjs_utf8
				return "writeString";
			case 15://__int64
			case 22://long long
				return "writeInt64";
			case 25://u16string
				return "writeUtf8";
			case 26://wstring
				return "writeUtf8";
			}
			break;

		}
	}

	if (IsExType_enum(strTypeName)) {
		return "writeInt32";
	}

	return "";
}

bool    ProtocolParse_Writer_TypeScript::IsInt64_TScript(const string& strTypeName)
{
	if (strTypeName.compare("__int64") == 0 || strTypeName.compare("long long") == 0 || strTypeName.compare("Int64") == 0)
		return true;
	return false;

}
string  ProtocolParse_Writer_TypeScript::ExTypeNameConvert_TScript(const string& strTypeName)
{
	if (strTypeName.compare("__int64") == 0 || strTypeName.compare("long long") == 0)
		return "Int64";
	return strTypeName;

}
#pragma endregion