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
	//���ȴ����ļ���
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
	case 1://������
	{
		for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++) {
			//��ҪΪ��ʹ��id�������ظ�
			if (m_pRefProtocolParseList[i]) {
				m_pRefProtocolParseList[i]->m_bStdafx = m_bStdafx;
				m_pRefProtocolParseList[i]->CreateFile_Path(path, filetype, true);
				m_pRefProtocolParseList[i]->GetRefProtocolMapPar(m_StringIntLogMap, m_IntStringMap, m_StringIntSearchMap);
			}
		}

		string str = path;
		str.append(m_SaveFileName._servername);
		return CreateFile_CPlus(str, filetype, brefprotocol);
	}
	break;
	case 0://�ͻ���
	{
		for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++) {
			if (m_pRefProtocolParseList[i]) {
				m_pRefProtocolParseList[i]->m_bStdafx = m_bStdafx;
				m_pRefProtocolParseList[i]->CreateFile_Path(path, filetype, true);
				m_pRefProtocolParseList[i]->GetRefProtocolMapPar(m_StringIntLogMap, m_IntStringMap, m_StringIntSearchMap);
			}
		}


		string str = path;
		str.append(m_SaveFileName._clientname);
		return CreateFile_CPlus(str, filetype, brefprotocol);
	}
	break;
	case 2: //�ϳ�
	{
		//��ҪΪ��ʹ��id�������ظ�
		for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++) {
			if (m_pRefProtocolParseList[i]) {
				m_pRefProtocolParseList[i]->m_bStdafx = m_bStdafx;
				m_pRefProtocolParseList[i]->CreateFile_Path(path, filetype, true);
				m_pRefProtocolParseList[i]->GetRefProtocolMapPar(m_StringIntLogMap, m_IntStringMap, m_StringIntSearchMap);
			}
		}


		//�Ȱ�����������
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
	//�ȶ�ȡ�ļ����ж�MD5�Ƿ���ͬ����ͬ����������
	if (ProtocolUtils::TestFileMd5(filename,m_strXMLMd5)) {
		if (bRefProtocol) {
			tt_bNotSaveFile = true;
		}
		else
			return true;
	}

	//setlocale(LC_ALL,"chs");//��Ч

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

	//д��Ϣͷ
	WriteInfoHead(fp);
	//д�ļ�ͷ
	WriteFileHead_CPlus(fp, filetype);

	if (m_bHCPP) {

		//д�����ռ�
		for (size_t m = 0; m < m_NamespaceVec.size(); m++) {
			WriteOneNameSpace_CPlus(m_NamespaceVec[m], fp, filetype, 1);
		}
		string tt_strline;
		if (m_bStdafx) {
			tt_strline.append("#include \"stdafx.h\"\n");
		}
		char tt_ch[512];
		size_t tt_pos2 = filename.rfind("\\");
		if (tt_pos2 != string::npos) {
			sprintf(tt_ch, "#include \"%s\"\n", filename.substr(tt_pos2 + 1).c_str());

		}
		else
			sprintf(tt_ch, "#include \"%s\"\n", filename.c_str());
		tt_strline.append(tt_ch);
		fwrite(tt_strline.c_str(), tt_strline.size(), 1, fp_CPP);
		for (size_t m = 0; m < m_NamespaceVec.size(); m++) {
			WriteOneNameSpace_CPlus(m_NamespaceVec[m], fp_CPP, filetype, 2);
		}
	}
	else {
		//д�����ռ�
		for (size_t m = 0; m < m_NamespaceVec.size(); m++) {
			WriteOneNameSpace_CPlus(m_NamespaceVec[m], fp, filetype, 0);
		}
	}

	///////

	if (m_strConflict.length() > 6) {
		fwrite(m_strConflict.c_str(), m_strConflict.length(), 1, fp);
	}
	string tt_tempstr = "\r\n";
	fwrite(tt_tempstr.c_str(), tt_tempstr.length(), 1, fp);


	if (m_bAndroidPeer) {
		string tt_str = "\n#endif\n";
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
				sprintf(ch, "#ifndef __%s__\n#define __%s__\n", m_NamespaceVec[0]._clientname.c_str(), m_NamespaceVec[0]._clientname.c_str());
			}
			else
				sprintf(ch, "#ifndef __%s__\n#define __%s__\n", m_RefProtocolList[0]._usingspacename.c_str(), m_RefProtocolList[0]._usingspacename.c_str());
			str.append(ch);
			sprintf(ch, "#include <string>\n#include <vector>\n#include \"WJS_IByteStream.h\"\n#include \"WJS_ArrayVector.h\"\n");
			str.append(ch);
		}
		else {
			if (m_bOutJson) {
				sprintf(ch, "#pragma once\n#include <string>\n#include <vector>\n#include \"stream/WJS_IByteStream.h\"\n#include \"std/WJS_ArrayVector.h\"\n#include \"WJS_JsonStream.h\"\n");
				str.append(ch);
			}
			else {
				sprintf(ch, "#pragma once\n#include <string>\n#include <vector>\n#include \"stream/WJS_IByteStream.h\"\n#include \"std/WJS_ArrayVector.h\"\n");
				str.append(ch);
			}



		}


		for (size_t i = 0; i < m_RefProtocolList.size(); i++) {
			switch (filetype) {
			case 0:
			{
				sprintf(ch, "#include \"%s\"\n", m_RefProtocolList[i]._includeheadnamec.c_str());

			}
			break;
			case 1:
			{
				sprintf(ch, "#include \"%s\"\n", m_RefProtocolList[i]._includeheadnames.c_str());
			}
			break;
			case 2:
			{
				sprintf(ch, "#include \"%s\"\n", m_RefProtocolList[i]._includeheadnames.c_str());
			}
			break;
			default:
				break;
			}

			str.append(ch);
		}


		str.append("using namespace std;\n");

		for (size_t i = 0; i < m_RefProtocolList.size(); i++) {
			sprintf(ch, "using namespace %s;\n", m_RefProtocolList[i]._usingspacename.c_str());
			str.append(ch);
		}


		if (m_bAndroidPeer) {
			str.append("#ifndef _WINDOWS\n");
			str.append("#define GNUC_PACKED1 __attribute((aligned (1)))\n");
			str.append("#else \n");
			str.append("#define GNUC_PACKED1 \n");
			str.append("#endif\n");
		}



	}
	else {
		if (m_bAndroidPeer) {
			char ch[256];
			if (m_NamespaceVec.size() > 0) {
				switch (filetype) {
				case 0:
					sprintf(ch, "#ifndef __%s__\n#define __%s__\n", m_NamespaceVec[0]._clientname.c_str(), m_NamespaceVec[0]._clientname.c_str());
					break;
				case 1:
					sprintf(ch, "#ifndef __%s__\n#define __%s__\n", m_NamespaceVec[0]._servername.c_str(), m_NamespaceVec[0]._servername.c_str());
					break;
				case 2:
					sprintf(ch, "#ifndef __%s__\n#define __%s__\n", m_NamespaceVec[0]._servername.c_str(), m_NamespaceVec[0]._servername.c_str());
					break;
				default:
					sprintf(ch, "#ifndef __%s__\n#define __%s__\n", m_NamespaceVec[0]._clientname.c_str(), m_NamespaceVec[0]._clientname.c_str());
					break;
				}
				str.append(ch);
			}
			else {
				sprintf(ch, "#ifndef __%s__\n#define __%s__\n", m_ProtocolNo._name.c_str(), m_ProtocolNo._name.c_str());
				str.append(ch);
			}



			str.append("#include <string>\n#include <vector>\n#include \"WJS_IByteStream.h\"\n#include \"WJS_ArrayVector.h\" \nusing namespace std;\n");

			str.append("#ifndef _WINDOWS\n");
			str.append("#define GNUC_PACKED1 __attribute((aligned (1)))\n");
			str.append("#else \n");
			str.append("#define GNUC_PACKED1 \n");
			str.append("#endif\n");
		}
		else {
			if (m_bOutJson)
				str = "#pragma once\n#include <string>\n#include <vector>\n#include \"stream/WJS_IByteStream.h\"\n#include \"std/WJS_ArrayVector.h\" \n#include \"WJS_JsonStream.h\"\nusing namespace std;\n";
			else
				str = "#pragma once\n#include <string>\n#include <vector>\n#include \"stream/WJS_IByteStream.h\"\n#include \"std/WJS_ArrayVector.h\" \nusing namespace std;\n";

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
		sprintf(ch, "namespace %s\n", pNamespace._clientname.c_str());
		t_namespace = pNamespace._clientname;
		break;
	case 1:
		sprintf(ch, "namespace %s\n", pNamespace._servername.c_str());
		break;
	case 2:
		sprintf(ch, "namespace %s\n", pNamespace._servername.c_str());
		break;
	default:
		break;
	}

	//sprintf(ch,"namespace %s\n",pNamespace._name.c_str());
	size_t len = strlen(ch);
	fwrite(ch, len, 1, fp);

	//д��ʼ��������{
	WriteStartZhongKuoKao(fp);

	//д������id
	if (HCPPType == 0 || HCPPType == 1)
		CreateAndWriteFuncId_CPlus(pNamespace, fp, m_tablenum + 1);

	//дЭ���
	if (HCPPType == 0 || HCPPType == 1)
		WriteProtocolNo_CPlus(pNamespace, fp, m_tablenum + 1);

	//��дö��
	if (HCPPType == 0 || HCPPType == 1) {
		for (size_t i = 0; i < pNamespace._EnumVec.size(); i++) {
			WriteEnum_CPlus(pNamespace._EnumVec[i], fp, m_tablenum + 1);
		}
	}


	////////��Ҫ����˳��,��˳�����д
	//�ṹ���vector�ʹ�С
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
				//д�ṹ��
				WriteStruct_CPlus(t_namespace,pNamespace._StructVec[structindex], fp, m_tablenum + 1, HCPPType);
				structindex++;
			}
			else if (pNamespace._VectorVec[vectorindex]._index < pNamespace._StructVec[structindex]._index
				&& (pNamespace._VectorVec[vectorindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index)) {
				//дvector
				WriteVector_CPlus(pNamespace._VectorVec[vectorindex], fp, m_tablenum + 1, HCPPType);
				vectorindex++;
			}
			else {
				//д��vector
				WriteWJSVector_CPlus(pNamespace._WJSVectorVec[wjsvectorindex], fp, m_tablenum + 1, HCPPType);
				wjsvectorindex++;
			}
		}
		else if (!bvectorend && (!bstructend)) {
			if (pNamespace._StructVec[structindex]._index < pNamespace._VectorVec[vectorindex]._index) {
				//д�ṹ��
				WriteStruct_CPlus(t_namespace,pNamespace._StructVec[structindex], fp, m_tablenum + 1, HCPPType);
				structindex++;
			}
			else {
				//дvector
				WriteVector_CPlus(pNamespace._VectorVec[vectorindex], fp, m_tablenum + 1, HCPPType);
				vectorindex++;
			}
		}
		else if (!bvectorend && (!bwjsvectorend)) {
			if (pNamespace._VectorVec[vectorindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index) {
				//дvector
				WriteVector_CPlus(pNamespace._VectorVec[vectorindex], fp, m_tablenum + 1, HCPPType);
				vectorindex++;
			}
			else {
				//д��vector
				WriteWJSVector_CPlus(pNamespace._WJSVectorVec[wjsvectorindex], fp, m_tablenum + 1, HCPPType);
				wjsvectorindex++;
			}
		}
		else if ((!bstructend) && (!bwjsvectorend)) {
			if (pNamespace._StructVec[structindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index) {
				//д�ṹ��
				WriteStruct_CPlus(t_namespace,pNamespace._StructVec[structindex], fp, m_tablenum + 1, HCPPType);
				structindex++;
			}
			else {
				//д��vector
				WriteWJSVector_CPlus(pNamespace._WJSVectorVec[wjsvectorindex], fp, m_tablenum + 1, HCPPType);
				wjsvectorindex++;
			}
		}
		else if (!bvectorend) {
			//дvector
			WriteVector_CPlus(pNamespace._VectorVec[vectorindex], fp, m_tablenum + 1, HCPPType);
			vectorindex++;

		}
		else if (!bstructend) {
			//д�ṹ��
			WriteStruct_CPlus(t_namespace,pNamespace._StructVec[structindex], fp, m_tablenum + 1, HCPPType);
			structindex++;
		}
		else if (!bwjsvectorend) {
			//д��vector
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
			for (size_t j = 0; j < pNamespace._ClassVec[i]._FuncVec.size(); j++) {
				pNamespace._ClassVec[i]._FuncVec[j]._strSCClassName = pNamespace._ClassVec[i]._name;
				pNamespace._ClassVec[0]._FuncVec.push_back(pNamespace._ClassVec[i]._FuncVec[j]);
			}
		}
		while (pNamespace._ClassVec.size() > 1) {
			pNamespace._ClassVec.pop_back();
		}


		for (size_t i = 0; i < pNamespace._ClassVec[0]._FuncVec.size(); i++) {
			if (!pNamespace._ClassVec[0]._FuncVec[i]._bSCFlag) {
				pNamespace._ClassVec[0]._FuncVec[i]._bDiscard = true;
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
			for (size_t j = 0; j < pNamespace._ClassVec[i]._FuncVec.size(); j++) {
				if (pNamespace._ClassVec[i]._FuncVec[j]._bSCFlag) {
					pNamespace._ClassVec[0]._FuncVec.push_back(pNamespace._ClassVec[i]._FuncVec[j]);
					pNamespace._ClassVec[0]._FuncVec[pNamespace._ClassVec[0]._FuncVec.size() - 1]._strSCClassName = pNamespace._ClassVec[i]._name;
				}
			}
		}

		for (int m = 0; m < tt_fucsize; m++) {
			if (!pNamespace._ClassVec[0]._FuncVec[m]._bSCFlag)
				continue;
			for (size_t i = 1; i < pNamespace._ClassVec.size(); i++) {
				pNamespace._ClassVec[i]._FuncVec.push_back(pNamespace._ClassVec[0]._FuncVec[m]);
				pNamespace._ClassVec[i]._FuncVec[pNamespace._ClassVec[i]._FuncVec.size() - 1]._strSCClassName = pNamespace._ClassVec[0]._name;

			}
		}



		//д��
		for (size_t i = 0; i < pNamespace._ClassVec.size(); i++) {
			WriteClass_CPlus(pNamespace._ClassVec[i], fp, m_tablenum + 1, filetype, pNamespace._protocolname, pNamespace, HCPPType);
		}

		pNamespace._ClassVec.clear();
		pNamespace._ClassVec = tt_bakClassList;
	}
	else {
		//д��
		for (size_t i = 0; i < pNamespace._ClassVec.size(); i++) {
			WriteClass_CPlus(pNamespace._ClassVec[i], fp, m_tablenum + 1, filetype, pNamespace._protocolname, pNamespace, HCPPType);
		}
	}


	//д������������}
	WriteEndZhongKuoKao(fp);
	return true;
}
void    ProtocolParse_Writer_CPlus::CreateAndWriteFuncId_CPlus(stNamespace& pNamespace, FILE* fp, int tablenum)
{
	//û�����ǲ���д������
	if (pNamespace._ClassVec.size() == 0)
		return;

	char t_ch[512];
	string strTable = CreateTableString(tablenum);  //д�����
	string strTable_2 = CreateTableString(tablenum + 1);

	string strFuncId;

	strFuncId.append(strTable);
	strFuncId.append("namespace METHOD\n");
	strFuncId.append(strTable);
	strFuncId.append("{\n");



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

			int tempId;  //���ַ���ת��id
			if (pNamespace._ClassVec[i]._FuncVec[j]._userFuncId != 0)
				tempId = pNamespace._ClassVec[i]._FuncVec[j]._userFuncId;
			else
				tempId = CovertStringToInt(t_ch, pNamespace._ClassVec[i]._FuncVec[j]._bNew);
			sprintf(t_ch, "=%d;\n", tempId);
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
			sprintf(t_ch, "=%d;\n", r._funcId);
			strFuncId.append(t_ch);
		}
	}

	strFuncId.append(strTable);
	strFuncId.append("}\n");

	fwrite(strFuncId.c_str(), strFuncId.length(), 1, fp);
}
void    ProtocolParse_Writer_CPlus::WriteProtocolNo_CPlus(stNamespace& pNamespace, FILE* fp, int tablenum)
{
	char t_ch[512];
	string strTable = CreateTableString(tablenum);

	string strProtocolNo;
	strProtocolNo.append(strTable);

	if (m_ProtocolNo._value != 1) {
		sprintf(t_ch, "const int %s=%d;\n", m_ProtocolNo._name.c_str(), m_ProtocolNo._value);
		strProtocolNo.append(t_ch);
	}
	else {
		strProtocolNo.append("\n");
	}

	///////////////��дЭ���
	//strProtocolNo.append(strTable);
	//sprintf(t_ch,"const int %s_versionno = %d;\n",pNamespace._protocolname.c_str(),m_VersionManger._protocolVersion);
	//strProtocolNo.append(t_ch);


	fwrite(strProtocolNo.c_str(), strProtocolNo.length(), 1, fp);


}
void    ProtocolParse_Writer_CPlus::WriteEnum_CPlus(stEnum& pEnum, FILE* fp, int tablenum)//tablenum�����Ʊ�����
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
		sprintf(t_ch, "enum class %s\n", pEnum._name.c_str());
		strEnum.append(t_ch);
	}
	else {
		sprintf(t_ch, "enum %s\n", pEnum._name.c_str());
		strEnum.append(t_ch);
	}


	strEnum.append(strTable);
	strEnum.append("{\n");

	for (size_t i = 0; i < pEnum._ParNameList.size(); i++) {
		strEnum.append(strTable_2);
		if (i == 0) {
			sprintf(t_ch, "%s = 0,\n", pEnum._ParNameList[i].c_str());
		}
		else if (i != pEnum._ParNameList.size() - 1) {
			sprintf(t_ch, "%s,\n", pEnum._ParNameList[i].c_str());
		}
		else {
			sprintf(t_ch, "%s\n", pEnum._ParNameList[i].c_str());
		}
		strEnum.append(t_ch);
	}
	strEnum.append(strTable);
	strEnum.append("};\n");

	fwrite(strEnum.c_str(), strEnum.length(), 1, fp);


	strEnum = "";

	//��������ʵ��
	strEnum.append(strTable);

	sprintf(t_ch, "inline WJS_InByteStream& operator << (WJS_InByteStream& ins,const %s& v)\n", pEnum._name.c_str());
	strEnum.append(t_ch);

	strEnum.append(strTable);
	strEnum.append("{\n");

	strEnum.append(strTable_2);
	sprintf(t_ch, "switch(v)\n");
	strEnum.append(t_ch);

	strEnum.append(strTable_2);
	strEnum.append("{\n");

	int tt_0 = 0;
	for (size_t i = 0; i < pEnum._ParNameList.size(); i++) {
		if (pEnum._bNewVersion && (!pEnum._keepOldList[i])) {
			strEnum.append(strTable_2);
			sprintf(t_ch, "case %s::%s:\n", pEnum._name.c_str(), pEnum._ParNameList[i].c_str());
			strEnum.append(t_ch);

			strEnum.append(strTable_3);
			sprintf(t_ch, "ins<<%d;\n", i);
			strEnum.append(t_ch);

			strEnum.append(strTable_3);
			strEnum.append("break;\n");
		}
		else {
			if (pEnum._bNewVersion) {
				strEnum.append(strTable_2);
				sprintf(t_ch, "case %s::%s:\n", pEnum._name.c_str(), pEnum._ParNameList[i].c_str());
				strEnum.append(t_ch);
			}
			else {
				strEnum.append(strTable_2);
				sprintf(t_ch, "case %s:\n", pEnum._ParNameList[i].c_str());
				strEnum.append(t_ch);
			}


			strEnum.append(strTable_3);
			if (i == 0) {
				tt_0 = CovertStringToInt(pEnum._ParNameList[i]);
				sprintf(t_ch, "ins<<%d;\n", tt_0);
			}
			else {
				sprintf(t_ch, "ins<<%d;\n", CovertStringToInt(pEnum._ParNameList[i]));
			}

			strEnum.append(t_ch);

			strEnum.append(strTable_3);
			strEnum.append("break;\n");
		}

	}

	if (pEnum._ParNameList.size() > 0) {
		strEnum.append(strTable_2);
		strEnum.append("default:\n");

		strEnum.append(strTable_3);
		sprintf(t_ch, "ins<<%d;\n", tt_0);
		strEnum.append(t_ch);

		strEnum.append(strTable_3);
		strEnum.append("break;\n");
	}

	strEnum.append(strTable_2);
	strEnum.append("}\n");

	strEnum.append(strTable_2);
	strEnum.append("return ins;\n");


	strEnum.append(strTable);
	strEnum.append("}\n");

	fwrite(strEnum.c_str(), strEnum.length(), 1, fp);


	//�����
	strEnum = "";
	strEnum.append(strTable);

	sprintf(t_ch, "inline WJS_OutByteStream& operator >> (WJS_OutByteStream& out, %s& v)\n", pEnum._name.c_str());
	strEnum.append(t_ch);

	strEnum.append(strTable);
	strEnum.append("{\n");

	strEnum.append(strTable_2);
	strEnum.append("int m=0;\n");
	strEnum.append(strTable_2);
	strEnum.append("out >> m;\n");

	strEnum.append(strTable_2);
	sprintf(t_ch, "switch(m)\n");
	strEnum.append(t_ch);

	strEnum.append(strTable_2);
	strEnum.append("{\n");


	for (size_t i = 0; i < pEnum._ParNameList.size(); i++) {
		if (pEnum._bNewVersion && (!pEnum._keepOldList[i])) {
			strEnum.append(strTable_2);
			sprintf(t_ch, "case %d:\n", i);
			strEnum.append(t_ch);

			strEnum.append(strTable_3);
			sprintf(t_ch, "v=%s::%s;\n", pEnum._name.c_str(), pEnum._ParNameList[i].c_str());
			strEnum.append(t_ch);

			strEnum.append(strTable_3);
			strEnum.append("break;\n");
		}
		else {
			strEnum.append(strTable_2);
			sprintf(t_ch, "case %d:\n", GetExistStringToIntValue(pEnum._ParNameList[i]));
			strEnum.append(t_ch);

			if (pEnum._bNewVersion) {
				strEnum.append(strTable_3);
				sprintf(t_ch, "v=%s::%s;\n", pEnum._name.c_str(), pEnum._ParNameList[i].c_str());
				strEnum.append(t_ch);
			}
			else {
				strEnum.append(strTable_3);
				sprintf(t_ch, "v=%s;\n", pEnum._ParNameList[i].c_str());
				strEnum.append(t_ch);
			}


			strEnum.append(strTable_3);
			strEnum.append("break;\n");
		}

	}



	if (pEnum._ParNameList.size() > 0) {
		strEnum.append(strTable_2);
		strEnum.append("default:\n");

		if (pEnum._bNewVersion) {
			strEnum.append(strTable_3);
			sprintf(t_ch, "v=%s::%s;\n", pEnum._name.c_str(), pEnum._ParNameList[0].c_str());
			strEnum.append(t_ch);
		}
		else {
			strEnum.append(strTable_3);
			sprintf(t_ch, "v=%s;\n", pEnum._ParNameList[0].c_str());
			strEnum.append(t_ch);
		}


		strEnum.append(strTable_3);
		strEnum.append("break;\n");
	}

	strEnum.append(strTable_2);
	strEnum.append("}\n");

	strEnum.append(strTable_2);
	strEnum.append("return out;\n");


	strEnum.append(strTable);
	strEnum.append("}\n");

	fwrite(strEnum.c_str(), strEnum.length(), 1, fp);


	//�����
	strEnum = "";
	strEnum.append(strTable);

	sprintf(t_ch, "inline WJS_OutByteStream2& operator >> (WJS_OutByteStream2& out, %s& v)\n", pEnum._name.c_str());
	strEnum.append(t_ch);

	strEnum.append(strTable);
	strEnum.append("{\n");

	strEnum.append(strTable_2);
	strEnum.append("int m=0;\n");
	strEnum.append(strTable_2);
	strEnum.append("out >> m;\n");

	strEnum.append(strTable_2);
	sprintf(t_ch, "switch(m)\n");
	strEnum.append(t_ch);

	strEnum.append(strTable_2);
	strEnum.append("{\n");

	for (size_t i = 0; i < pEnum._ParNameList.size(); i++) {
		if (pEnum._bNewVersion && (!pEnum._keepOldList[i])) {
			strEnum.append(strTable_2);
			sprintf(t_ch, "case %d:\n", i);
			strEnum.append(t_ch);

			strEnum.append(strTable_3);
			sprintf(t_ch, "v=%s::%s;\n", pEnum._name.c_str(), pEnum._ParNameList[i].c_str());
			strEnum.append(t_ch);

			strEnum.append(strTable_3);
			strEnum.append("break;\n");
		}
		else {
			strEnum.append(strTable_2);
			sprintf(t_ch, "case %d:\n", GetExistStringToIntValue(pEnum._ParNameList[i]));
			strEnum.append(t_ch);

			if (pEnum._bNewVersion) {
				strEnum.append(strTable_3);
				sprintf(t_ch, "v=%s::%s;\n", pEnum._name.c_str(), pEnum._ParNameList[i].c_str());
				strEnum.append(t_ch);
			}
			else {
				strEnum.append(strTable_3);
				sprintf(t_ch, "v=%s;\n", pEnum._ParNameList[i].c_str());
				strEnum.append(t_ch);
			}


			strEnum.append(strTable_3);
			strEnum.append("break;\n");
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
		strEnum.append("default:\n");

		if (pEnum._bNewVersion) {
			strEnum.append(strTable_3);
			sprintf(t_ch, "v=%s::%s;\n", pEnum._name.c_str(), pEnum._ParNameList[0].c_str());
			strEnum.append(t_ch);
		}
		else {
			strEnum.append(strTable_3);
			sprintf(t_ch, "v=%s;\n", pEnum._ParNameList[0].c_str());
			strEnum.append(t_ch);
		}


		strEnum.append(strTable_3);
		strEnum.append("break;\n");
	}

	strEnum.append(strTable_2);
	strEnum.append("}\n");

	strEnum.append(strTable_2);
	strEnum.append("return out;\n");


	strEnum.append(strTable);
	strEnum.append("}\n");

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
	sprintf(t_ch, "tt_jns.WriteNewKey(\"%s\", 1, false);\n", strParName.c_str());
	strStruct.append(t_ch);
	strStruct.append(strTable_2);
	if (bCreateArrayPointer)
		strStruct.append("stJsonArray*  tt_pArray = tt_jns.CreateJsonArray(2);{\n");
	else
		strStruct.append("tt_pArray = tt_jns.CreateJsonArray(2);{\n");

	strStruct.append(strTable_3);
	strStruct.append("if (tt_pArray){\n");
	strStruct.append(strTable_4);
	sprintf(t_ch, "tt_pArray->_pv = tt_jns.CreateJsonValue_String(\"%s\");\n", strParType.c_str());
	strStruct.append(t_ch);
	strStruct.append(strTable_4);
	if (strMember.length() > 0)
		sprintf(t_ch, "(tt_pArray + 1)->_pv = tt_jns.CreateJsonValue_String(tt_jns.ToString(%s.%s.size()));\n", strMember.c_str(), strParName.c_str());
	else
		sprintf(t_ch, "(tt_pArray + 1)->_pv = tt_jns.CreateJsonValue_String(tt_jns.ToString(%s.size()));\n", strParName.c_str());

	strStruct.append(t_ch);
	strStruct.append(strTable_4);
	strStruct.append("tt_jns.WriteArrayValue(tt_pArray);\n");
	strStruct.append(strTable_3);
	strStruct.append("}\n");
	strStruct.append(strTable_2);
	strStruct.append("}\n");


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
	sprintf(t_ch, "tt_pJsonObject = tt_jns.WriteNewKey(\"%s_d0\", 2, true);\n", strParName.c_str());
	strStruct.append(t_ch);

	strStruct.append(strTable_3);
	if (strMember.length() > 0) {
		if (IsExType_enum(strParType))
			sprintf(t_ch, "tt_jns<<(int)%s.%s[0];\n", strMember.c_str(), strParName.c_str());
		else
			sprintf(t_ch, "tt_jns<<%s.%s[0];\n", strMember.c_str(), strParName.c_str());
	}

	else
		sprintf(t_ch, "tt_jns<<%s[0];\n", strParName.c_str());

	strStruct.append(t_ch);

	strStruct.append(strTable_3);
	strStruct.append("tt_jns.EnterToChildEnd(tt_pJsonObject);\n");

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
		sprintf(t_ch, "inline WJS_JsonStream& operator << (WJS_JsonStream& jns,const %s& v)\n", pStruct._name.c_str());
		strStruct.append(t_ch);
	}
	else if (HCPPType == 1) {
		sprintf(t_ch, "extern WJS_JsonStream& operator << (WJS_JsonStream& jns,const %s& v);\n", pStruct._name.c_str());
		strStruct.append(t_ch);
	}
	else if (HCPPType == 2) {
		sprintf(t_ch, "WJS_JsonStream& operator << (WJS_JsonStream& jns,const %s& v)\n", pStruct._name.c_str());
		strStruct.append(t_ch);
	}

	fwrite(strStruct.c_str(), strStruct.length(), 1, fp);
	strStruct = "";

	if (HCPPType == 0 || HCPPType == 2) {
		strStruct.append(strTable);
		strStruct.append("{\n");

		bool tt_bCreatedJsonObject = false;
		strStruct.append(strTable_2);
		strStruct.append("WJS_JsonStream  tt_jns;\n");

		if (pStruct._inherited.length() > 0) {
			strStruct.append(strTable_2);
			sprintf(t_ch, "%s::operator <<(tt_jns,(const %s&)v);\n", rNameSpace.c_str(), pStruct._inherited.c_str());
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
					strStruct.append("stJsonObject*   tt_pJsonObject = NULL;\n");
				}

				strStruct.append(strTable_2);
				sprintf(t_ch, "if(v.%s.size()>0){\n", pStruct._ParVec[i]._strParName.c_str());
				strStruct.append(t_ch);

				strStruct.append(WriteJsonStream_Array0ToObject(tablenum + 2, pStruct._ParVec[i]._strParName, pStruct._ParVec[i]._strParType, "v").c_str());

				strStruct.append(strTable_2);
				strStruct.append("}\n");
			}
			else if (IsBaseType(pStruct._ParVec[i]._strParType)) {
				strStruct.append(strTable_2);
				sprintf(t_ch, "tt_jns.WriteNewKey(\"%s\", 0, false);\n", pStruct._ParVec[i]._strParName.c_str());
				strStruct.append(t_ch);

				strStruct.append(strTable_2);
				sprintf(t_ch, "tt_jns<<v.%s;\n", pStruct._ParVec[i]._strParName.c_str());
				strStruct.append(t_ch);
			}
			else //�½ṹ��
			{
				if (!tt_bCreatedJsonObject) {
					tt_bCreatedJsonObject = true;
					strStruct.append(strTable_2);
					strStruct.append("stJsonObject*   tt_pJsonObject = NULL;\n");
				}
				strStruct.append(strTable_2);
				sprintf(t_ch, "tt_pJsonObject = tt_jns.WriteNewKey(\"%s\", 2, true);\n", pStruct._ParVec[i]._strParName.c_str());
				strStruct.append(t_ch);

				strStruct.append(strTable_2);
				if (IsExType_enum(pStruct._ParVec[i]._strParType))
					sprintf(t_ch, "tt_jns<<(int)v.%s;\n", pStruct._ParVec[i]._strParName.c_str());
				else
					sprintf(t_ch, "tt_jns<<v.%s;\n", pStruct._ParVec[i]._strParName.c_str());

				strStruct.append(t_ch);

				strStruct.append(strTable_2);
				strStruct.append("tt_jns.EnterToChildEnd(tt_pJsonObject);\n");
			}
		}

		strStruct.append(strTable_2);
		strStruct.append("jns.AppendJsonStream(tt_jns);\n");

		strStruct.append(strTable_2);
		strStruct.append("return jns;\n");

		strStruct.append(strTable);
		strStruct.append("}\n");
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
		strStruct.append("#pragma pack(4)\n");
	}
	else {
		strStruct.append(strTable);
		strStruct.append("#pragma pack(push)\n");
		strStruct.append(strTable);
		strStruct.append("#pragma pack(1)\n");
	}

	strStruct.append(WriteBlockCommentBegin_CPlus(strTable));
	strStruct.append(WriteBlockCommentBrief_CPlus(strTable, pStruct._comment));
	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		strStruct.append(WriteBlockCommentPar_CPlus(strTable, pStruct._ParVec[i]._comment, pStruct._ParVec[i]._strParName).c_str());
	}
	strStruct.append(WriteBlockCommentEnd_CPlus(strTable));


	strStruct.append(strTable);
	if (pStruct._inherited.length() > 0) {
		sprintf(t_ch, "struct %s : public %s\n", pStruct._name.c_str(), pStruct._inherited.c_str());
	}
	else {
		sprintf(t_ch, "struct %s\n", pStruct._name.c_str());
	}
	
	strStruct.append(t_ch);

	strStruct.append(strTable);
	strStruct.append("{\n");

	for (auto& r : pStruct._ParVec) {
		strStruct.append(strTable_2);
		sprintf(t_ch, "%s %s;\n", GetMemberTypeFullType_CPlus(r._strParType).c_str(), r._strParName.c_str());
		strStruct.append(t_ch);
	}

	if (pStruct._bWithConstruct) {
		strStruct.append(strTable_2);
		sprintf(t_ch, "%s()\n", pStruct._name.c_str());
		strStruct.append(t_ch);
		strStruct.append(strTable_2);
		strStruct.append("{\n");

		for (auto& r : pStruct._ParVec) {
			if (r._bWithDefault) {
				strStruct.append(strTable_3);
				sprintf(t_ch, "%s=%s;\n", r._strParName.c_str(), r._strDefaultValue.c_str());
				strStruct.append(t_ch);
			}
		}
		strStruct.append(strTable_2);
		strStruct.append("}\n");
	}

	if (g_bCreateCopy) {
		for (auto& r : pStruct._ParVec) {
			strStruct.append(strTable_2);
			if (IsBaseType(r._strParType)) {
				sprintf(t_ch, "//%s_A.%s=%s_B.%s;\n", pStruct._name.c_str(), r._strParName.c_str(), pStruct._name.c_str(), r._strParName.c_str());
			}
			else {
				sprintf(t_ch, "//Convert_%s(%s_A.%s,%s_B.%s);\n", r._strParName.c_str(), pStruct._name.c_str(), r._strParName.c_str(), pStruct._name.c_str(), r._strParName.c_str());
				if (g_bCreateCopy2) {
					if (IsExType_vector(r._strParType)) {
						string tt_strT = GetExTypeParT_vector(r._strParType);
						if (!IsBaseType(tt_strT)) {
							strStruct.append(t_ch);
							strStruct.append(strTable_2);
							sprintf(t_ch, "//void Convert_%s(%s& %s_A,const %s& %s_B);\n", r._strParType.c_str(), r._strParType.c_str(), r._strParType.c_str(), r._strParType.c_str(), r._strParType.c_str());
							strStruct.append(t_ch);
							sprintf(t_ch, "//void Convert_%s(%s& %s_A,const %s& %s_B);\n", tt_strT.c_str(), tt_strT.c_str(), tt_strT.c_str(), tt_strT.c_str(), tt_strT.c_str());

						}
					}
					else {
						strStruct.append(t_ch);
						strStruct.append(strTable_2);
						sprintf(t_ch, "//void Convert_%s(%s& %s_A,const %s& %s_B);\n", r._strParName.c_str(), r._strParType.c_str(), r._strParType.c_str(), r._strParType.c_str(), r._strParType.c_str());

					}
				}

			}

			strStruct.append(t_ch);
		}

	}


	strStruct.append(strTable);
	strStruct.append("};\n");

	if (m_bAndroidPeer) {
		strStruct.append(strTable);
		strStruct.append("#pragma  pack()\n");
	}
	else {
		strStruct.append("#if defined(_WINDOWS) || defined(WIN32) || defined(_X64)\n");
		strStruct.append(strTable);
		strStruct.append("#pragma  pack(pop)\n");
		strStruct.append("#endif\n");
	}



	if (HCPPType == 0 || HCPPType == 1) {
		fwrite(strStruct.c_str(), strStruct.length(), 1, fp);
		if (pStruct._oldname.length() > 0) {
			strStruct = "";
			strStruct.append(strTable);
			sprintf(t_ch, "typedef %s %s;\n", pStruct._name.c_str(), pStruct._oldname.c_str());
			strStruct.append(t_ch);
			fwrite(strStruct.c_str(), strStruct.length(), 1, fp);
		}
	}





	//��������ʵ��
	strStruct = "";
	strStruct.append(strTable);

	if (HCPPType == 0) {
		sprintf(t_ch, "inline WJS_InByteStream& operator << (WJS_InByteStream& ins,const %s& v)\n", pStruct._name.c_str());
		strStruct.append(t_ch);
	}
	else if (HCPPType == 1) {
		sprintf(t_ch, "extern WJS_InByteStream& operator << (WJS_InByteStream& ins,const %s& v);\n", pStruct._name.c_str());
		strStruct.append(t_ch);
	}
	else if (HCPPType == 2) {
		sprintf(t_ch, "WJS_InByteStream& operator << (WJS_InByteStream& ins,const %s& v)\n", pStruct._name.c_str());
		strStruct.append(t_ch);
	}

	fwrite(strStruct.c_str(), strStruct.length(), 1, fp);
	strStruct = "";

	if (HCPPType == 0 || HCPPType == 2) {
		strStruct.append(strTable);
		strStruct.append("{\n");

		if (pStruct._inherited.length() > 0) {
			strStruct.append(strTable_2);
			sprintf(t_ch, "%s::operator <<(ins,(const %s)v);\n", rNameSpace.c_str(), pStruct._inherited.c_str());
			strStruct.append(t_ch);
		}

		//��д��ṹ��Ĵ�С
		strStruct.append(strTable_2);
		strStruct.append("short tt_len=0;\n");
		strStruct.append(strTable_2);
		strStruct.append("int  tt_logsize1 = ins.size();\n");
		strStruct.append(strTable_2);
		strStruct.append("ins<<tt_len;\n");


		//��д��汾
		strStruct.append(strTable_2);
		sprintf(t_ch, "short tt_version=%d;\n", pStruct._version);
		strStruct.append(t_ch);
		strStruct.append(strTable_2);
		strStruct.append("ins<<tt_version;\n");
		strStruct.append(strTable_2);
		strStruct.append("int  tt_logsize2 = ins.size();\n");



		if (pStruct.IsHasTransNeedValuePar()) {
			strStruct.append(strTable_2);
			strStruct.append("int tt_transNeedValue=0;\n");
			strStruct.append(strTable_2);
			strStruct.append("ins<<tt_transNeedValue;\n");
			//strStruct.append(strTable_2);
			//strStruct.append("int  tt_logsize4 = ins.size();\n");
		}

		for (auto& r : pStruct._ParVec) {
			if (r._transNeedValue != -1) {
				strStruct.append(strTable_2);
				sprintf(t_ch, "if(v.%s!=%s)\n", r._strParName.c_str(), r._strDefaultValue.c_str());
				strStruct.append(t_ch);
				strStruct.append(strTable_2);
				strStruct.append("{\n");
				strStruct.append(strTable_3);
				string tt_strNameSpace;
				sprintf(t_ch, "ins<<v.%s;\n", r._strParName.c_str());
				strStruct.append(t_ch);
				strStruct.append(strTable_3);
				sprintf(t_ch, "tt_transNeedValue = tt_transNeedValue | (1<<%d);\n", r._transNeedValue);
				strStruct.append(t_ch);
				strStruct.append(strTable_2);
				strStruct.append("}\n");
			}
			else {
				strStruct.append(strTable_2);
				string tt_strNameSpace;
				if (IsExType_baseVector(r._strParType, 0, tt_strNameSpace) && tt_strNameSpace.length() > 0)
					sprintf(t_ch, "%s::operator <<(ins,v.%s);\n", tt_strNameSpace.c_str(), r._strParName.c_str());
				else
					sprintf(t_ch, "ins<<v.%s;\n", r._strParName.c_str());
				strStruct.append(t_ch);
			}
		}

		if (pStruct.IsHasTransNeedValuePar()) {
			strStruct.append(strTable_2);
			strStruct.append("if(tt_transNeedValue!=0)\n");
			strStruct.append(strTable_2);
			strStruct.append("{\n");
			strStruct.append(strTable_3);
			strStruct.append("memcpy(&(ins[tt_logsize2]),&tt_transNeedValue,sizeof(int));\n");
			strStruct.append(strTable_2);
			strStruct.append("}\n");
		}


		strStruct.append(strTable_2);
		strStruct.append("short  tt_logsize3 = (short)(ins.size() - tt_logsize2);\n");
		strStruct.append(strTable_2);
		strStruct.append("memcpy(&(ins[tt_logsize1]),&tt_logsize3,sizeof(short));\n");
		strStruct.append(strTable_2);
		strStruct.append("return ins;\n");

		strStruct.append(strTable);
		strStruct.append("}\n");
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


	//�����
	strStruct = "";
	strStruct.append(strTable);
	if (HCPPType == 0) {
		if (bOutStream2) {
			sprintf(t_ch, "inline WJS_OutByteStream2& operator >> (WJS_OutByteStream2& out, %s& v)\n", pStruct._name.c_str());
			strStruct.append(t_ch);
		}
		else {
			sprintf(t_ch, "inline WJS_OutByteStream& operator >> (WJS_OutByteStream& out, %s& v)\n", pStruct._name.c_str());
			strStruct.append(t_ch);
		}

	}
	else if (HCPPType == 1) {
		if (bOutStream2) {
			sprintf(t_ch, "extern WJS_OutByteStream2& operator >> (WJS_OutByteStream2& out, %s& v);\n", pStruct._name.c_str());
			strStruct.append(t_ch);
		}
		else {
			sprintf(t_ch, "extern WJS_OutByteStream& operator >> (WJS_OutByteStream& out, %s& v);\n", pStruct._name.c_str());
			strStruct.append(t_ch);
		}

	}
	else if (HCPPType == 2) {
		if (bOutStream2) {
			sprintf(t_ch, "WJS_OutByteStream2& operator >> (WJS_OutByteStream2& out, %s& v)\n", pStruct._name.c_str());
			strStruct.append(t_ch);
		}
		else {
			sprintf(t_ch, "WJS_OutByteStream& operator >> (WJS_OutByteStream& out, %s& v)\n", pStruct._name.c_str());
			strStruct.append(t_ch);
		}
	}

	fwrite(strStruct.c_str(), strStruct.length(), 1, fp);
	strStruct = "";

	if (HCPPType == 0 || HCPPType == 2) {
		strStruct.append(strTable);
		strStruct.append("{\n");

		//�ȶ����ṹ��Ĵ�С
		if (pStruct._inherited.length() > 0) {
			strStruct.append(strTable_2);
			sprintf(t_ch, "%s::operator >>(out,(%s&)v);\n", rNameSpace.c_str(), pStruct._inherited.c_str());
			strStruct.append(t_ch);
		}

		strStruct.append(strTable_2);
		strStruct.append("short tt_len=0;\n");

		strStruct.append(strTable_2);
		strStruct.append("out>>tt_len;\n");

		strStruct.append(strTable_2);
		strStruct.append("short tt_version=0;\n");

		strStruct.append(strTable_2);
		strStruct.append("out>>tt_version;\n");

		strStruct.append(strTable_2);
		strStruct.append("int tt_logsize1 = 0;\n");

		if (pStruct.IsHasTransNeedValuePar()) {
			strStruct.append(strTable_2);
			strStruct.append("tt_logsize1 = out.GetCurOffsetPos();\n");
		}

		if (pStruct.IsHasTransNeedValuePar()) {
			strStruct.append(strTable_2);
			strStruct.append("int tt_transNeedValue=0;\n");

			strStruct.append(strTable_2);
			strStruct.append("out>>tt_transNeedValue;\n");
		}

		strStruct.append(strTable_2);
		sprintf(t_ch, "if(tt_version >= %d || (tt_version==0)){\n", pStruct._version);
		strStruct.append(t_ch);


		if (!pStruct.IsHasTransNeedValuePar()) {
			strStruct.append(strTable_3);
			sprintf(t_ch, "if(tt_version > %d)\n", pStruct._version);
			strStruct.append(t_ch);
			strStruct.append(strTable_4);
			strStruct.append("tt_logsize1 = out.GetCurOffsetPos();\n");
		}



		for (auto& r : pStruct._ParVec) {
			if (pStruct.IsHasTransNeedValuePar() && (r._transNeedValue != -1)) {
				strStruct.append(strTable_3);
				sprintf(t_ch, "if((tt_transNeedValue & (1<<%d))!=0){\n", r._transNeedValue);
				strStruct.append(t_ch);

				strStruct.append(strTable_4);
				sprintf(t_ch, "out>>v.%s;\n", r._strParName.c_str());
				strStruct.append(t_ch);
				strStruct.append(strTable_3);
				strStruct.append("}\n");
				strStruct.append(strTable_3);
				strStruct.append("else{\n");

				strStruct.append(strTable_4);
				sprintf(t_ch, "v.%s=%s;\n", r._strParName.c_str(), r._strDefaultValue.c_str());
				strStruct.append(t_ch);
				strStruct.append(strTable_3);
				strStruct.append("}\n");
			}
			else {
				strStruct.append(strTable_3);
				string tt_strNameSpace;
				if (IsExType_baseVector(r._strParType, 0, tt_strNameSpace) && tt_strNameSpace.length() > 0)
					sprintf(t_ch, "%s::operator >>(out,v.%s);\n", tt_strNameSpace.c_str(), r._strParName.c_str());
				else
					sprintf(t_ch, "out>>v.%s;\n", r._strParName.c_str());
				strStruct.append(t_ch);
			}
		}


		strStruct.append(strTable_3);
		sprintf(t_ch, "if(tt_version > %d){\n", pStruct._version);
		strStruct.append(t_ch);
		strStruct.append(strTable_4);
		strStruct.append("int tt_logsize2 = out.GetCurOffsetPos();\n");

		strStruct.append(strTable_4);
		strStruct.append("int tt_offset = tt_len - (tt_logsize2 - tt_logsize1);\n");

		strStruct.append(strTable_4);
		strStruct.append("if(tt_offset>0)\n");

		strStruct.append(strTable_5);
		strStruct.append("out.CurOffset(tt_offset);\n");


		strStruct.append(strTable_3);
		strStruct.append("}\n");

		strStruct.append(strTable_2);
		strStruct.append("}\n");


		strStruct.append(strTable_2);
		strStruct.append("else{\n");


		int tt_lastVersion = -1;
		string tt_strDefault;

		if (pStruct.IsHasTransNeedValuePar()) {
			for (auto& r : pStruct._ParVec) {
				strStruct.append(strTable_3);
				sprintf(t_ch, "if(tt_version>=%d){\n", r._version);
				strStruct.append(t_ch);

				if (pStruct.IsHasTransNeedValuePar() && (r._transNeedValue != -1)) {
					strStruct.append(strTable_4);
					sprintf(t_ch, "if((tt_transNeedValue & (1<<%d))!=0){\n", r._transNeedValue);
					strStruct.append(t_ch);

					strStruct.append(strTable_5);
					sprintf(t_ch, "out>>v.%s;\n", r._strParName.c_str());
					strStruct.append(t_ch);
					strStruct.append(strTable_4);
					strStruct.append("}\n");
					strStruct.append(strTable_4);
					strStruct.append("else{\n");

					strStruct.append(strTable_5);
					sprintf(t_ch, "v.%s=%s;\n", r._strParName.c_str(), r._strDefaultValue.c_str());
					strStruct.append(t_ch);
					strStruct.append(strTable_4);
					strStruct.append("}\n");
				}
				else {
					strStruct.append(strTable_4);
					string tt_strNameSpace;
					if (IsExType_baseVector(r._strParType, 0, tt_strNameSpace) && tt_strNameSpace.length() > 0)
						sprintf(t_ch, "%s::operator >>(out,v.%s);\n", tt_strNameSpace.c_str(), r._strParName.c_str());
					else
						sprintf(t_ch, "out>>v.%s;\n", r._strParName.c_str());
					strStruct.append(t_ch);
				}

				strStruct.append(strTable_3);
				strStruct.append("}\n");

				string tt_default = GetDefaultValue(r._strParType);
				if (tt_default.length() > 0) {
					strStruct.append(strTable_3);
					strStruct.append("else\n");

					strStruct.append(strTable_4);
					sprintf(t_ch, "v.%s=%s;\n", r._strParName.c_str(), tt_default.c_str());
					strStruct.append(t_ch);

				}
			}
		}
		else {
			for (auto& r : pStruct._ParVec) {
				if (tt_lastVersion == -1 || tt_lastVersion != r._version) {

					if (tt_strDefault.length() > 0) {
						strStruct.append(strTable_3);
						strStruct.append("}\n");
						strStruct.append(strTable_3);
						strStruct.append("else{\n");

						strStruct.append(tt_strDefault.c_str());
						tt_strDefault = "";
						strStruct.append(strTable_3);
						strStruct.append("}\n");
					}
					else if (tt_lastVersion != -1) {
						strStruct.append(strTable_3);
						strStruct.append("}\n");
					}
					strStruct.append(strTable_3);
					sprintf(t_ch, "if(tt_version>=%d){\n", r._version);
					strStruct.append(t_ch);


					tt_lastVersion = r._version;
				}
				strStruct.append(strTable_4);
				string tt_strNameSpace;
				if (IsExType_baseVector(r._strParType, 0, tt_strNameSpace) && tt_strNameSpace.length() > 0)
					sprintf(t_ch, "%s::operator >>(out,v.%s);\n", tt_strNameSpace.c_str(), r._strParName.c_str());
				else
					sprintf(t_ch, "out>>v.%s;\n", r._strParName.c_str());
				strStruct.append(t_ch);

				string tt_default = GetDefaultValue(r._strParType);
				if (tt_default.length() > 0) {
					tt_strDefault.append(strTable_4);
					sprintf(t_ch, "v.%s=%s;\n", r._strParName.c_str(), tt_default.c_str());
					tt_strDefault.append(t_ch);
				}
			}

			if (tt_lastVersion != -1) {
				strStruct.append(strTable_3);
				strStruct.append("}\n");
				if (tt_strDefault.length() > 0) {
					strStruct.append(strTable_3);
					strStruct.append("else{\n");

					strStruct.append(tt_strDefault.c_str());
					tt_strDefault = "";
					strStruct.append(strTable_3);
					strStruct.append("}\n");
				}
			}

		}

		strStruct.append(strTable_2);
		strStruct.append("}\n");

		strStruct.append(strTable_2);
		strStruct.append("return out;\n");

		strStruct.append(strTable);
		strStruct.append("}\n");
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

	//����
	strVector = "";
	bool tt_bBaseType = IsBaseType(pVector._ParTName);
	if (HCPPType == 0 || (tt_bBaseType && HCPPType == 1)) {
		strVector.append(strTable);
		sprintf(t_ch, "inline WJS_JsonStream& operator << (WJS_JsonStream& jns,const %s& v)\n", pVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 1) {
		strVector.append(strTable);
		sprintf(t_ch, "extern WJS_JsonStream& operator << (WJS_JsonStream& jns,const %s& v);\n", pVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 2 && (!tt_bBaseType)) {
		strVector.append(strTable);
		sprintf(t_ch, "WJS_JsonStream& operator << (WJS_JsonStream& jns,const %s& v)\n", pVector._name.c_str());
		strVector.append(t_ch);
	}
	fwrite(strVector.c_str(), strVector.length(), 1, fp);
	strVector = "";

	if (HCPPType == 0 || (HCPPType == 2 && (!tt_bBaseType)) || (tt_bBaseType && HCPPType == 1)) {
		strVector.append(strTable);
		strVector.append("{\n");

		bool tt_bCreatedJsonObject = false;
		strVector.append(strTable_2);
		strVector.append("WJS_JsonStream  tt_jns;\n");


		//����ǻ������ͣ�һ���Դ���
		if (IsCanFastProcessVectorType_CPlus(pVector._ParTName)) {
			strVector.append(WriteJsonStream_Array(tablenum + 1, "v", pVector._name, "", true).c_str());
		}
		else {
			strVector.append(WriteJsonStream_Array(tablenum + 1, "v", pVector._name, "", true).c_str());
			strVector.append(strTable_2);
			strVector.append("if(v.size()>0)\n");
			strVector.append(strTable_2);
			strVector.append("{\n");
			strVector.append(strTable_3);
			strVector.append("stJsonObject*   tt_pJsonObject = NULL;\n");
			strVector.append(WriteJsonStream_Array0ToObject(tablenum + 2, "v", pVector._name, "").c_str());

			strVector.append(strTable_3);
			strVector.append("jns.AppendJsonStream(tt_jns);\n");
			strVector.append(strTable_2);
			strVector.append("}\n");
		}

		strVector.append(strTable_2);
		strVector.append("return jns;\n");

		strVector.append(strTable);
		strVector.append("}\n");

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

	//����
	strVector = "";
	bool tt_bBaseType = IsBaseType(pVector._ParTName);
	if (HCPPType == 0 || (tt_bBaseType && HCPPType == 1)) {
		strVector.append(strTable);
		sprintf(t_ch, "inline WJS_JsonStream& operator << (WJS_JsonStream& jns,const %s& v)\n", pVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 1) {
		strVector.append(strTable);
		sprintf(t_ch, "extern WJS_JsonStream& operator << (WJS_JsonStream& jns,const %s& v);\n", pVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 2 && (!tt_bBaseType)) {
		strVector.append(strTable);
		sprintf(t_ch, "WJS_JsonStream& operator << (WJS_JsonStream& jns,const %s& v)\n", pVector._name.c_str());
		strVector.append(t_ch);
	}
	fwrite(strVector.c_str(), strVector.length(), 1, fp);
	strVector = "";

	if (HCPPType == 0 || (HCPPType == 2 && (!tt_bBaseType)) || (tt_bBaseType && HCPPType == 1)) {
		strVector.append(strTable);
		strVector.append("{\n");

		bool tt_bCreatedJsonObject = false;
		strVector.append(strTable_2);
		strVector.append("WJS_JsonStream  tt_jns;\n");

		//����ǻ������ͣ�һ���Դ���
		if (IsCanFastProcessVectorType_CPlus(pVector._ParTName)) {
			strVector.append(WriteJsonStream_Array(tablenum + 1, "v", pVector._name, "", true).c_str());
		}
		else {
			strVector.append(WriteJsonStream_Array(tablenum + 1, "v", pVector._name, "", true).c_str());
			strVector.append(strTable_2);
			strVector.append("if(v.size()>0)\n");
			strVector.append(strTable_2);
			strVector.append("{\n");
			strVector.append(strTable_3);
			strVector.append("stJsonObject*   tt_pJsonObject = NULL;\n");
			strVector.append(WriteJsonStream_Array0ToObject(tablenum + 2, "v", pVector._name, "").c_str());
			strVector.append(strTable_3);
			strVector.append("jns.AppendJsonStream(tt_jns);\n");
			strVector.append(strTable_2);
			strVector.append("}\n");
		}

		strVector.append(strTable_2);
		strVector.append("return jns;\n");

		strVector.append(strTable);
		strVector.append("}\n");

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
		sprintf(t_ch, "typedef std::vector<%s> %s;\n", GetMemberTypeFullType_CPlus(pVector._ParTName).c_str(), pVector._name.c_str());		
		strVector.append(t_ch);

		if (pVector._oldname.length() > 0) {
			strVector.append(strTable);
			sprintf(t_ch, "typedef std::vector<%s> %s;\n", GetMemberTypeFullType_CPlus(pVector._ParTName).c_str(), pVector._oldname.c_str());		
			strVector.append(t_ch);
		}


		fwrite(strVector.c_str(), strVector.length(), 1, fp);
	}


	//����
	strVector = "";
	bool tt_bBaseType = IsBaseType(pVector._ParTName);
	if (HCPPType == 0 || (tt_bBaseType && HCPPType == 1)) {
		strVector.append(strTable);
		sprintf(t_ch, "inline WJS_InByteStream& operator << (WJS_InByteStream& ins,const %s& v)\n", pVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 1) {
		strVector.append(strTable);
		sprintf(t_ch, "extern WJS_InByteStream& operator << (WJS_InByteStream& ins,const %s& v);\n", pVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 2 && (!tt_bBaseType)) {
		strVector.append(strTable);
		sprintf(t_ch, "WJS_InByteStream& operator << (WJS_InByteStream& ins,const %s& v)\n", pVector._name.c_str());
		strVector.append(t_ch);
	}
	fwrite(strVector.c_str(), strVector.length(), 1, fp);
	strVector = "";

	if (HCPPType == 0 || (HCPPType == 2 && (!tt_bBaseType)) || (tt_bBaseType && HCPPType == 1)) {
		strVector.append(strTable);
		strVector.append("{\n");
		strVector.append(strTable_2);
		strVector.append("ins<<int(v.size());\n");

		//����ǻ������ͣ�һ���Դ���
		if (IsCanFastProcessVectorType_CPlus(pVector._ParTName)) {
			strVector.append(strTable_2);
			strVector.append("if(v.size()>0)\n");
			strVector.append(strTable_2);
			strVector.append("{\n");
			strVector.append(strTable_3);
			sprintf(t_ch, "ins.write((void*)(&(v[0])),sizeof(%s)*(unsigned int)v.size());\n", GetMemberTypeFullType_CPlus(pVector._ParTName).c_str());
			
			strVector.append(t_ch);
			strVector.append(strTable_2);
			strVector.append("}\n");
		}
		else {
			if (_cplusStandardType >= eCPlusStandardType::CPlus11) {
				strVector.append(strTable_2);
				strVector.append("for(auto& r :v)\n");
				strVector.append(strTable_2);
				strVector.append("{\n");
				strVector.append(strTable_3);
				strVector.append("ins<<r;\n");
				strVector.append(strTable_2);
				strVector.append("}\n");
			}
			else {
				strVector.append(strTable_2);
				strVector.append("for(size_t i=0;i<v.size();i++)\n");
				strVector.append(strTable_2);
				strVector.append("{\n");
				strVector.append(strTable_3);
				strVector.append("ins<<v[i];\n");
				strVector.append(strTable_2);
				strVector.append("}\n");
			}

		}

		strVector.append(strTable_2);
		strVector.append("return ins;\n");

		strVector.append(strTable);
		strVector.append("}\n");

		fwrite(strVector.c_str(), strVector.length(), 1, fp);
	}


	//����
	strVector = "";
	strVector.append(strTable);

	if (HCPPType == 0 || (tt_bBaseType && HCPPType == 1)) {
		sprintf(t_ch, "inline WJS_OutByteStream& operator >> (WJS_OutByteStream& out, %s& v)\n", pVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 1) {
		sprintf(t_ch, "extern WJS_OutByteStream& operator >> (WJS_OutByteStream& out, %s& v);\n", pVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 2 && (!tt_bBaseType)) {
		sprintf(t_ch, "WJS_OutByteStream& operator >> (WJS_OutByteStream& out, %s& v)\n", pVector._name.c_str());
		strVector.append(t_ch);
	}
	fwrite(strVector.c_str(), strVector.length(), 1, fp);
	strVector = "";


	if (HCPPType == 0 || (HCPPType == 2 && (!tt_bBaseType)) || (tt_bBaseType && HCPPType == 1)) {
		strVector.append(strTable);
		strVector.append("{\n");

		strVector.append(strTable_2);
		strVector.append("int count=0;\n");

		strVector.append(strTable_2);
		strVector.append("out >> count;\n");

		strVector.append(strTable_2);
		strVector.append("if(count<0)\n");
		strVector.append(strTable_2);
		strVector.append("{\n");
		strVector.append(strTable_3);
		strVector.append("throw(\"vecOverFlow\");\n");
		strVector.append(strTable_3);
		strVector.append("return out;\n");
		strVector.append(strTable_2);
		strVector.append("}\n");

		if (pVector._bBaseParT) {
			if (IsCanFastProcessVectorType_CPlus(pVector._ParTName)) {
				strVector.append(strTable_2);
				strVector.append("if(count>0)\n");
				strVector.append(strTable_2);
				strVector.append("{\n");
				strVector.append(strTable_3);
				strVector.append("v.resize(count);\n");
				strVector.append(strTable_3);
				sprintf(t_ch, "out.read(&(v[0]),sizeof(%s)*count);\n", GetMemberTypeFullType_CPlus(pVector._ParTName).c_str());
				
				strVector.append(t_ch);
				strVector.append(strTable_2);
				strVector.append("}\n");
				strVector.append(strTable_2);

			}
			else {
				strVector.append(strTable_2);
				sprintf(t_ch, "%s m;\n", GetMemberTypeFullType_CPlus(pVector._ParTName).c_str());
				
				strVector.append(t_ch);

				strVector.append(strTable_2);
				strVector.append("for(int i=0;i<count;i++)\n");
				strVector.append(strTable_2);
				strVector.append("{\n");

				strVector.append(strTable_3);
				strVector.append("out>>m;\n");

				strVector.append(strTable_3);
				strVector.append("v.push_back(m);\n");

				strVector.append(strTable_2);
				strVector.append("}\n");
			}

		}
		else {
			strVector.append(strTable_2);
			strVector.append("v.resize(count);\n");


			strVector.append(strTable_2);
			strVector.append("for(int i=0;i<count;i++)\n");
			strVector.append(strTable_2);
			strVector.append("{\n");


			strVector.append(strTable_3);
			strVector.append("out>>v[i];\n");

			strVector.append(strTable_2);
			strVector.append("}\n");
		}



		strVector.append(strTable_2);
		strVector.append("return out;\n");

		strVector.append(strTable);
		strVector.append("}\n");

		fwrite(strVector.c_str(), strVector.length(), 1, fp);
	}


	//����2
	strVector = "";
	if (HCPPType == 0 || (tt_bBaseType && HCPPType == 1)) {
		strVector.append(strTable);
		sprintf(t_ch, "inline WJS_OutByteStream2& operator >> (WJS_OutByteStream2& out, %s& v)\n", pVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 1) {
		strVector.append(strTable);
		sprintf(t_ch, "extern WJS_OutByteStream2& operator >> (WJS_OutByteStream2& out, %s& v);\n", pVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 2 && (!tt_bBaseType)) {
		strVector.append(strTable);
		sprintf(t_ch, "WJS_OutByteStream2& operator >> (WJS_OutByteStream2& out, %s& v)\n", pVector._name.c_str());
		strVector.append(t_ch);
	}
	fwrite(strVector.c_str(), strVector.length(), 1, fp);
	strVector = "";



	if (HCPPType == 0 || (HCPPType == 2 && (!tt_bBaseType)) || (tt_bBaseType && HCPPType == 1)) {
		strVector.append(strTable);
		strVector.append("{\n");

		strVector.append(strTable_2);
		strVector.append("int count=0;\n");

		strVector.append(strTable_2);
		strVector.append("out >> count;\n");

		strVector.append(strTable_2);
		strVector.append("if(count<0)\n");
		strVector.append(strTable_2);
		strVector.append("{\n");
		strVector.append(strTable_3);
		strVector.append("throw(\"vecOverFlow\");\n");
		strVector.append(strTable_3);
		strVector.append("return out;\n");
		strVector.append(strTable_2);
		strVector.append("}\n");

		if (pVector._bBaseParT) {
			if (IsCanFastProcessVectorType_CPlus(pVector._ParTName)) {
				strVector.append(strTable_2);
				strVector.append("if(count>0)\n");
				strVector.append(strTable_2);
				strVector.append("{\n");
				strVector.append(strTable_3);
				strVector.append("v.resize(count);\n");
				strVector.append(strTable_3);
				sprintf(t_ch, "out.read(&(v[0]),sizeof(%s)*count);\n", GetMemberTypeFullType_CPlus(pVector._ParTName).c_str());
				
				strVector.append(t_ch);
				strVector.append(strTable_2);
				strVector.append("}\n");
			}
			else {
				strVector.append(strTable_2);
				sprintf(t_ch, "%s m;\n", GetMemberTypeFullType_CPlus(pVector._ParTName).c_str());
				
				strVector.append(t_ch);

				strVector.append(strTable_2);
				strVector.append("for(int i=0;i<count;i++)\n");
				strVector.append(strTable_2);
				strVector.append("{\n");

				strVector.append(strTable_3);
				strVector.append("out>>m;\n");

				strVector.append(strTable_3);
				strVector.append("v.push_back(m);\n");

				strVector.append(strTable_2);
				strVector.append("}\n");
			}

		}
		else {
			strVector.append(strTable_2);
			strVector.append("v.resize(count);\n");

			strVector.append(strTable_2);
			strVector.append("for(int i=0;i<count;i++)\n");
			strVector.append(strTable_2);
			strVector.append("{\n");


			strVector.append(strTable_3);
			strVector.append("out>>v[i];\n");

			strVector.append(strTable_2);
			strVector.append("}\n");
		}



		strVector.append(strTable_2);
		strVector.append("return out;\n");

		strVector.append(strTable);
		strVector.append("}\n");

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
		sprintf(t_ch, "typedef WJS_ArrayVector<%s> %s;\n", GetMemberTypeFullType_CPlus(pWJSVector._ParTName).c_str(), pWJSVector._name.c_str());
		
		strVector.append(t_ch);
		fwrite(strVector.c_str(), strVector.length(), 1, fp);
	}


	//����
	strVector = "";
	strVector.append(strTable);

	if (HCPPType == 0) {
		sprintf(t_ch, "inline WJS_InByteStream& operator << (WJS_InByteStream& ins, const %s& v)\n", pWJSVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 1) {
		sprintf(t_ch, "extern WJS_InByteStream& operator << (WJS_InByteStream& ins, const %s& v);\n", pWJSVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 2) {
		sprintf(t_ch, "WJS_InByteStream& operator << (WJS_InByteStream& ins, const %s& v)\n", pWJSVector._name.c_str());
		strVector.append(t_ch);
	}
	fwrite(strVector.c_str(), strVector.length(), 1, fp);
	strVector = "";



	if (HCPPType == 0 || HCPPType == 2) {
		strVector.append(strTable);
		strVector.append("{\n");

		strVector.append(strTable_2);
		strVector.append("ins<<int(v.size());\n");

		if (IsCanFastProcessVectorType_CPlus(pWJSVector._ParTName)) {
			strVector.append(strTable_2);
			strVector.append("if(v.size()>0)\n");
			strVector.append(strTable_2);
			strVector.append("{\n");
			strVector.append(strTable_3);
			sprintf(t_ch, "ins.write((void*)(&(v[0])),sizeof(%s)*v.size());\n", GetMemberTypeFullType_CPlus(pWJSVector._ParTName).c_str());
			
			strVector.append(t_ch);
			strVector.append(strTable_2);
			strVector.append("}\n");
		}
		else {
			//if (_cplusStandardType >= eCPlusStandardType::CPlus11) {
			//	strVector.append(strTable_2);
			//	strVector.append("for(auto& r: v)\n");
			//	strVector.append(strTable_2);
			//	strVector.append("{\n");
			//	strVector.append(strTable_3);
			//	strVector.append("ins<<r;\n");
			//	strVector.append(strTable_2);
			//	strVector.append("}\n");
			//}
			//else {
				strVector.append(strTable_2);
				strVector.append("for(size_t i=0;i<v.size();i++)\n");
				strVector.append(strTable_2);
				strVector.append("{\n");
				strVector.append(strTable_3);
				strVector.append("ins<<v[i];\n");
				strVector.append(strTable_2);
				strVector.append("}\n");
			//}

		}

		strVector.append(strTable_2);
		strVector.append("return ins;\n");

		strVector.append(strTable);
		strVector.append("}\n");

		fwrite(strVector.c_str(), strVector.length(), 1, fp);
	}


	//����
	strVector = "";
	strVector.append(strTable);

	if (HCPPType == 0) {
		sprintf(t_ch, "inline WJS_OutByteStream& operator >> (WJS_OutByteStream& out, %s& v)\n", pWJSVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 1) {
		sprintf(t_ch, "extern WJS_OutByteStream& operator >> (WJS_OutByteStream& out, %s& v);\n", pWJSVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 2) {
		sprintf(t_ch, "WJS_OutByteStream& operator >> (WJS_OutByteStream& out, %s& v)\n", pWJSVector._name.c_str());
		strVector.append(t_ch);
	}
	fwrite(strVector.c_str(), strVector.length(), 1, fp);
	strVector = "";

	if (HCPPType == 0 || HCPPType == 2) {
		strVector.append(strTable);
		strVector.append("{\n");

		strVector.append(strTable_2);
		strVector.append("int count=0;\n");

		strVector.append(strTable_2);
		strVector.append("out >> count;\n");

		strVector.append(strTable_2);
		strVector.append("if(count<0)\n");
		strVector.append(strTable_2);
		strVector.append("{\n");
		strVector.append(strTable_3);
		strVector.append("throw(\"wvecOverFlow\");\n");
		strVector.append(strTable_3);
		strVector.append("return out;\n");
		strVector.append(strTable_2);
		strVector.append("}\n");


		if (pWJSVector._bBaseParT) {
			if (IsCanFastProcessVectorType_CPlus(pWJSVector._ParTName)) {
				strVector.append(strTable_2);
				strVector.append("if(count>0)\n");
				strVector.append(strTable_2);
				strVector.append("{\n");
				strVector.append(strTable_3);
				strVector.append("v.resize(count,true);\n");
				strVector.append(strTable_3);
				sprintf(t_ch, "out.read(&(v[0]),sizeof(%s)*count);\n", GetMemberTypeFullType_CPlus(pWJSVector._ParTName).c_str());				
				strVector.append(t_ch);
				strVector.append(strTable_2);
				strVector.append("}\n");
			}
			else {
				strVector.append(strTable_2);
				strVector.append("for(int i=0;i<count;i++)\n");
				strVector.append(strTable_2);
				strVector.append("{\n");
				strVector.append(strTable_3);
				sprintf(t_ch, "%s m;\n", GetMemberTypeFullType_CPlus(pWJSVector._ParTName).c_str());
				
				strVector.append(t_ch);
				strVector.append(strTable_3);
				strVector.append("out>>m;\n");
				strVector.append(strTable_3);
				strVector.append("v.push_back(m);\n");
				strVector.append(strTable_2);
				strVector.append("}\n");
			}

		}
		else {
			strVector.append(strTable_2);
			strVector.append("v.resize(count,true);\n");

			strVector.append(strTable_2);
			strVector.append("for(int i=0;i<count;i++)\n");
			strVector.append(strTable_2);
			strVector.append("{\n");

			strVector.append(strTable_3);
			strVector.append("out>>v[i];\n");

			strVector.append(strTable_2);
			strVector.append("}\n");
		}



		strVector.append(strTable_2);
		strVector.append("return out;\n");

		strVector.append(strTable);
		strVector.append("}\n");

		fwrite(strVector.c_str(), strVector.length(), 1, fp);

	}

	//����2
	strVector = "";
	strVector.append(strTable);

	if (HCPPType == 0) {
		sprintf(t_ch, "inline WJS_OutByteStream2& operator >> (WJS_OutByteStream2& out, %s& v)\n", pWJSVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 1) {
		sprintf(t_ch, "extern WJS_OutByteStream2& operator >> (WJS_OutByteStream2& out, %s& v);\n", pWJSVector._name.c_str());
		strVector.append(t_ch);
	}
	else if (HCPPType == 2) {
		sprintf(t_ch, "WJS_OutByteStream2& operator >> (WJS_OutByteStream2& out, %s& v)\n", pWJSVector._name.c_str());
		strVector.append(t_ch);
	}

	fwrite(strVector.c_str(), strVector.length(), 1, fp);
	strVector = "";

	if (HCPPType == 0 || HCPPType == 2) {
		strVector.append(strTable);
		strVector.append("{\n");

		strVector.append(strTable_2);
		strVector.append("int count=0;\n");

		strVector.append(strTable_2);
		strVector.append("out >> count;\n");

		strVector.append(strTable_2);
		strVector.append("if(count<0)\n");
		strVector.append(strTable_2);
		strVector.append("{\n");
		strVector.append(strTable_3);
		strVector.append("throw(\"wvecOverFlow\");\n");
		strVector.append(strTable_3);
		strVector.append("return out;\n");
		strVector.append(strTable_2);
		strVector.append("}\n");


		if (pWJSVector._bBaseParT) {
			if (IsCanFastProcessVectorType_CPlus(pWJSVector._ParTName)) {
				strVector.append(strTable_2);
				strVector.append("if(count>0)\n");
				strVector.append(strTable_2);
				strVector.append("{\n");
				strVector.append(strTable_3);
				strVector.append("v.resize(count,true);\n");
				strVector.append(strTable_3);
				sprintf(t_ch, "out.read(&(v[0]),sizeof(%s)*count);\n", GetMemberTypeFullType_CPlus(pWJSVector._ParTName).c_str());
				
				strVector.append(t_ch);
				strVector.append(strTable_2);
				strVector.append("}\n");
			}
			else {
				strVector.append(strTable_2);
				strVector.append("for(int i=0;i<count;i++)\n");
				strVector.append(strTable_2);
				strVector.append("{\n");
				strVector.append(strTable_3);
				sprintf(t_ch, "%s m;\n", GetMemberTypeFullType_CPlus(pWJSVector._ParTName).c_str());
				
				strVector.append(t_ch);
				strVector.append(strTable_3);
				strVector.append("out>>m;\n");
				strVector.append(strTable_3);
				strVector.append("v.push_back(m);\n");
				strVector.append(strTable_2);
				strVector.append("}\n");
			}

		}
		else {
			strVector.append(strTable_2);
			strVector.append("v.resize(count,true);\n");

			strVector.append(strTable_2);
			strVector.append("for(int i=0;i<count;i++)\n");
			strVector.append(strTable_2);
			strVector.append("{\n");

			strVector.append(strTable_3);
			strVector.append("out>>v[i];\n");

			strVector.append(strTable_2);
			strVector.append("}\n");
		}


		strVector.append(strTable_2);
		strVector.append("return out;\n");

		strVector.append(strTable);
		strVector.append("}\n");

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
		//���ڿͻ��˶��ԣ�request��Ϊ���ͣ�callback��Ϊ�ص�
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
		//���������ԣ�������ǻص���callback�Ƿ���
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
		//ȡһ��callback������

		string strcallbackclassname;
		for (size_t i = 0; i < pNamespace._ClassVec.size(); i++) {
			if (pNamespace._ClassVec[i]._strtype.compare("callback") == 0) {
				strcallbackclassname = pNamespace._ClassVec[i]._name;
				break;
			}
		}

		//��ʵ������������һ����
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
void    ProtocolParse_Writer_CPlus::WriteClass_IReceiver_CPlus(stClass& pClass, FILE* fp, int tablenum, int filetype, const string& spacename, bool bemptyimplement)//д�ӿ���
{
	//////////û�нӿ�
	char t_ch[512];
	string strTable = CreateTableString(tablenum);
	string strTable_2 = CreateTableString(tablenum + 1);

	string strClass;

	int tt_maxLayer = pClass.GetMaxLayer();
	for (int k = 1; k <= tt_maxLayer; k++) {
		strClass.append(strTable);
		if (k != 1) {
			sprintf(t_ch, "class IReceiver_%s_%d : public IReceiver_%s_%d\n", spacename.c_str(), k, spacename.c_str(), k - 1);
			strClass.append(t_ch);
		}
		else {
			sprintf(t_ch, "class IReceiver_%s_%d\n", spacename.c_str(), k);
			strClass.append(t_ch);
		}

		strClass.append(strTable);
		strClass.append("{\n");
		strClass.append(strTable);
		strClass.append("public:\n");
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
		strClass.append("};\n");

		fwrite(strClass.c_str(), strClass.length(), 1, fp);
		strClass = "";

	}

	strClass.append(strTable);
	if (tt_maxLayer > 0) {
		sprintf(t_ch, "class IReceiver_%s : public IReceiver_%s_%d\n", spacename.c_str(), spacename.c_str(), tt_maxLayer);
		strClass.append(t_ch);
	}
	else {
		sprintf(t_ch, "class IReceiver_%s\n", spacename.c_str());
		strClass.append(t_ch);
	}


	strClass.append(strTable);
	strClass.append("{\n");
	strClass.append(strTable);
	strClass.append("public:\n");
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
	strClass.append("};\n");

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


	//��Ϊ���գ�������ǰ+callback
	strFunc.append(strTable);

	//�������Ͷ���Ϊvoid��
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
		//�ͻ��˴���һ��void* 
		if (pFunc._InParVec.size() > 0)
			strFunc.append(",void* pExData");
		else
			strFunc.append("void* pExData");
	}
	break;
	case 1:
	{
		//�������������Ự��
		if (pFunc._InParVec.size() > 0)
			strFunc.append(",unsigned int sessionId,void* pExData");
		else
			strFunc.append("unsigned int sessionId,void* pExData");
	}
	break;
	case 2:
	{
		//�������������Ự��
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
		strFunc.append("){};\n");
	}
	else
		strFunc.append(")=0;\n");

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
		sprintf(t_ch, "class ReceiveCallback_%s\n", spacename.c_str());
		strClass.append(t_ch);

		strClass.append(strTable);
		strClass.append("{\n");

		if (m_bOutJson) {
			strClass.append(strTable);
			strClass.append("public:\n");

			strClass.append(strTable_2);
			sprintf(t_ch, "ReceiveCallback_%s() {_bOutJson=false;_pWriteJsonStream=NULL;}\n", spacename.c_str());
			strClass.append(t_ch);
		}

		strClass.append(strTable);
		strClass.append("protected:\n");
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
		strClass.append("public:\n");
		fwrite(strClass.c_str(), strClass.length(), 1, fp);
	}


	//����
	WriteFunc_ReceiverParser_CPlus(pClass, fp, tablenum + 1, filetype, spacename, strcallbackclassname, pNamespace, HCPPType);

	if (HCPPType == 0 || HCPPType == 1) {
		strClass = "";

		if (m_bOutJson) {
			strClass.append(strTable_2);
			strClass.append("bool _bOutJson;\n");

			strClass.append(strTable_2);
			strClass.append("WJS_JsonStream _jsonStream;\n");

			strClass.append(strTable_2);
			strClass.append("IWrite_JsonStream* _pWriteJsonStream;\n");


		}

		strClass.append(strTable);
		strClass.append("};\n");
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
		//��������void
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
		strFunc.append("\n");
	}
	else if (HCPPType == 1) {
		strFunc.append(";\n");
	}
	else if (HCPPType == 2) {
		strFunc.append("\n");
	}

	fwrite(strFunc.c_str(), strFunc.length(), 1, fp);
	strFunc = "";

	if (HCPPType == 1)
		return;

	strFunc.append(strTable);
	strFunc.append("{\n");

	for (auto& r : pFunc._InParVec) {
		strFunc.append(strTable_2);
		sprintf(t_ch, "%s %s;\n", GetMemberTypeFullType_CPlus(r._strParType).c_str(), r._strParName.c_str());

		strFunc.append(t_ch);
	}

	for (auto& r : pFunc._InParVec) {
		if (r._version > 1)
			break;
		strFunc.append(strTable_2);
		string tt_strNameSpace;
		if (IsExType_baseVector(r._strParType, filetype, tt_strNameSpace) && tt_strNameSpace.length() > 0)
			sprintf(t_ch, "%s::operator >>(out,%s);\n", tt_strNameSpace.c_str(), r._strParName.c_str());
		else
			sprintf(t_ch, "out>>%s;\n", r._strParName.c_str());

		strFunc.append(t_ch);
	}

	string ttt_strFunc;
	bool   tt_bHasReadData = false;

	//�����ж����
	ttt_strFunc.append(strTable_2);
	sprintf(t_ch, "if(proversionno==%d || (proversionno==0))\n", pFunc._version);
	ttt_strFunc.append(t_ch);

	ttt_strFunc.append(strTable_2);
	ttt_strFunc.append("{\n");

	for (auto& r : pFunc._InParVec) {
		if (r._version <= 1)
			continue;
		tt_bHasReadData = true;
		ttt_strFunc.append(strTable_3);
		string tt_strNameSpace;
		if (IsExType_baseVector(r._strParType, filetype, tt_strNameSpace) && tt_strNameSpace.length() > 0)
			sprintf(t_ch, "%s::operator >>(out,%s);\n", tt_strNameSpace.c_str(), r._strParName.c_str());
		else
			sprintf(t_ch, "out>>%s;\n", r._strParName.c_str());

		ttt_strFunc.append(t_ch);
	}

	ttt_strFunc.append(strTable_2);
	ttt_strFunc.append("}\n");

	//////////////�ͽ��
	ttt_strFunc.append(strTable_2);
	sprintf(t_ch, "else if(proversionno>%d)\n", pFunc._version);
	ttt_strFunc.append(t_ch);

	ttt_strFunc.append(strTable_2);
	ttt_strFunc.append("{\n");

	for (auto& r : pFunc._InParVec) {
		if (r._version <= 1)
			continue;
		tt_bHasReadData = true;
		ttt_strFunc.append(strTable_3);
		string tt_strNameSpace;
		if (IsExType_baseVector(r._strParType, filetype, tt_strNameSpace) && tt_strNameSpace.length() > 0)
			sprintf(t_ch, "%s::operator >>(out,%s);\n", tt_strNameSpace.c_str(), r._strParName.c_str());
		else
			sprintf(t_ch, "out>>%s;\n", r._strParName.c_str());

		ttt_strFunc.append(t_ch);
	}

	ttt_strFunc.append(strTable_2);
	ttt_strFunc.append("}\n");

	///////////////�߽��
	ttt_strFunc.append(strTable_2);
	ttt_strFunc.append("else\n");

	ttt_strFunc.append(strTable_2);
	ttt_strFunc.append("{\n");

	for (auto& r : pFunc._InParVec) {
		if (r._version <= 1)
			continue;
		tt_bHasReadData = true;
		ttt_strFunc.append(strTable_3);
		sprintf(t_ch, "if(proversionno>=%d)\n", r._version);
		ttt_strFunc.append(t_ch);

		ttt_strFunc.append(strTable_4);
		string tt_strNameSpace;
		if (IsExType_baseVector(r._strParType, filetype, tt_strNameSpace) && tt_strNameSpace.length() > 0) {
			sprintf(t_ch, "%s::operator >>(out,%s);\n", tt_strNameSpace.c_str(), r._strParName.c_str());
		}
		else {
			sprintf(t_ch, "out>>%s;\n", r._strParName.c_str());
		}
		ttt_strFunc.append(t_ch);

		string tt_default = GetDefaultValue(r._strParType);
		if (tt_default.length() > 0) {
			ttt_strFunc.append(strTable_3);
			ttt_strFunc.append("else\n");

			ttt_strFunc.append(strTable_4);
			sprintf(t_ch, "%s = %s;\n", r._strParName.c_str(), tt_default.c_str());
			ttt_strFunc.append(t_ch);
		}
	}

	ttt_strFunc.append(strTable_2);
	ttt_strFunc.append("}\n");

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
			strFunc.append(");\n");
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
		strFunc.append(");\n");
	}


	if (m_bOutJson) {
		if (HCPPType == 0 || HCPPType == 2) {
			strFunc.append(strTable_2);
			strFunc.append("if(_bOutJson){\n");

			strFunc.append(strTable_3);
			strFunc.append("stJsonObject*   tt_pJsonObject = NULL;\n");

			for (auto& r : pFunc._InParVec) {
				strFunc.append(strTable_3);
				string tt_strNameSpace;
				if (IsExType_baseVector(r._strParType, filetype, tt_strNameSpace)) {
					sprintf(t_ch, "%s::operator <<(_jsonStream,%s);\n", tt_strNameSpace.c_str(), r._strParName.c_str());
					strFunc.append(t_ch);
					//sprintf(t_ch, "_jsonStream<<%s;\n", r._strParName.c_str());					
				}
				else if (IsExType_vector(r._strParType)) {
					sprintf(t_ch, "_jsonStream<<%s;\n", r._strParName.c_str());
					strFunc.append(t_ch);
				}
				else if (IsBaseType(r._strParType)) {
					sprintf(t_ch, "_jsonStream.WriteNewKey(\"%s\",0,false);\n", r._strParName.c_str());
					strFunc.append(t_ch);
					strFunc.append(strTable_3);
					sprintf(t_ch, "_jsonStream<<%s;\n", r._strParName.c_str());
					strFunc.append(t_ch);
				}
				else if (IsExType_enum(r._strParType)) {
					sprintf(t_ch, "tt_pJsonObject=_jsonStream.WriteNewKey(\"%s\", 2, true);\n", r._strParName.c_str());
					strFunc.append(t_ch);
					strFunc.append(strTable_3);
					sprintf(t_ch, "_jsonStream<<(int)%s;\n", r._strParName.c_str());
					strFunc.append(t_ch);
					strFunc.append(strTable_3);
					strFunc.append("_jsonStream.EnterToChildEnd(tt_pJsonObject);\n");
				}
				else {
					sprintf(t_ch, "tt_pJsonObject=_jsonStream.WriteNewKey(\"%s\", 2, true);\n", r._strParName.c_str());
					strFunc.append(t_ch);
					strFunc.append(strTable_3);
					sprintf(t_ch, "_jsonStream<<%s;\n", r._strParName.c_str());
					strFunc.append(t_ch);
					strFunc.append(strTable_3);
					strFunc.append("_jsonStream.EnterToChildEnd(tt_pJsonObject);\n");
				}
			}

			strFunc.append(strTable_3);
			strFunc.append("if(_pWriteJsonStream)\n");

			strFunc.append(strTable_4);
			strFunc.append("_pWriteJsonStream->NotifyWriteJsonStream(_jsonStream);\n");

			strFunc.append(strTable_2);
			strFunc.append("}\n");
		}
	}



	strFunc.append(strTable);
	strFunc.append("}\n");

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

	strFunc.append("\n");

	fwrite(strFunc.c_str(), strFunc.length(), 1, fp);
	strFunc = "";

	if (HCPPType == 1)
		return;



	strFunc.append(strTable);
	strFunc.append("{\n");

	strFunc.append(strTable_2);
	strFunc.append("int tt_prono=byteStream.readintonly();\n");
	strFunc.append(strTable_2);
	sprintf(t_ch, "if(tt_prono!=%s)\n", m_ProtocolNo._name.c_str());
	strFunc.append(t_ch);
	strFunc.append(strTable_2);
	strFunc.append("  return false;\n");

	strFunc.append(strTable_2);
	strFunc.append("WJS_OutByteStream2 out(byteStream);\n");
	//����Э���
	strFunc.append(strTable_2);
	strFunc.append("int prono=0;\n");
	strFunc.append(strTable_2);
	strFunc.append("out>>prono;\n");
	strFunc.append(strTable_2);
	sprintf(t_ch, "if(prono!=%s)\n", m_ProtocolNo._name.c_str());
	strFunc.append(t_ch);
	strFunc.append(strTable_2);
	strFunc.append("   return false;\n");

	//////Э��汾
	strFunc.append(strTable_2);
	strFunc.append("short proversionno=0;\n");
	strFunc.append(strTable_2);
	strFunc.append("out>>proversionno;\n");

	strFunc.append(strTable_2);
	strFunc.append("int methodid=0;\n");
	strFunc.append(strTable_2);
	strFunc.append("out>>methodid;\n");

	if (m_bOutJson) {
		strFunc.append(strTable_2);
		strFunc.append("if(_bOutJson){\n");

		strFunc.append(strTable_3);
		strFunc.append("_jsonStream.Free();\n");
		strFunc.append(strTable_3);
		strFunc.append("_jsonStream.CreateFirst();\n");

		strFunc.append(strTable_3);
		strFunc.append("switch(methodid)\n");

		strFunc.append(strTable_3);
		strFunc.append("{\n");

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
			strFunc.append(" :\n");

			strFunc.append(strTable_4);
			sprintf(t_ch, "_jsonStream.WriteNewKey(\"%s\",0,false);\n", tt_funcIdName.c_str());
			strFunc.append(t_ch);
			strFunc.append(strTable_4);
			sprintf(t_ch, "_jsonStream<<%s;\n", tt_funcIdFullName.c_str());
			strFunc.append(t_ch);
			strFunc.append(strTable_4);
			strFunc.append("break;\n");

		}


		strFunc.append(strTable_3);
		strFunc.append("}\n");
		strFunc.append(strTable_2);
		strFunc.append("}\n");
	}

	strFunc.append(strTable_2);
	strFunc.append("switch(methodid)\n");

	strFunc.append(strTable_2);
	strFunc.append("{\n");

	for (auto& r : pClass._FuncVec) {
		if (r._bDiscard)
			continue;
		if (filetype == 2 && ((m_bSCSpliteS && r._SCSpliteType == -1) || (m_bSCSpliteC && r._SCSpliteType == 1)))
			continue;
		strFunc.append(strTable_2);
		switch (filetype) {
		case 0:
			sprintf(t_ch, "case %s::METHOD::ID_%s_%s :\n", pNamespace._clientname.c_str(), pClass._name.c_str(), r._name.c_str());
			break;
		case 1:
			if (r._strSCClassName.length() > 0) {
				sprintf(t_ch, "case %s::METHOD::ID_%s_%s :\n", pNamespace._servername.c_str(), r._strSCClassName.c_str(), r._name.c_str());

			}
			else
				sprintf(t_ch, "case %s::METHOD::ID_%s_%s :\n", pNamespace._servername.c_str(), pClass._name.c_str(), r._name.c_str());
			break;
		case 2:
		{
			if (r._strSCClassName.length() > 0) {
				sprintf(t_ch, "case %s::METHOD::ID_%s_%s :\n", pNamespace._servername.c_str(), r._strSCClassName.c_str(), r._name.c_str());
			}
			else
				sprintf(t_ch, "case %s::METHOD::ID_%s_%s :\n", pNamespace._servername.c_str(), pClass._name.c_str(), r._name.c_str());

		}

		break;
		default:
			sprintf(t_ch, "case METHOD::ID_%s_%s :\n", pClass._name.c_str(), r._name.c_str());
			break;
		}
		strFunc.append(t_ch);

		strFunc.append(strTable_3);
		switch (filetype) {
		case 0:
		{
			sprintf(t_ch, "_parser_%s(out,receiver,pExData,proversionno);\n", r._name.c_str());
		}
		break;
		case 1:
		{
			sprintf(t_ch, "_parser_%s(out,receiver,sessionId,pExData,proversionno);\n", r._name.c_str());
		}
		break;
		case 2:
		{
			sprintf(t_ch, "_parser_%s(out,receiver,sessionId,pExData,proversionno);\n", r._name.c_str());
		}
		break;
		default:
			break;
		}


		strFunc.append(t_ch);

		strFunc.append(strTable_3);
		strFunc.append("return true;\n");
	}

	strFunc.append(strTable_2);
	strFunc.append("}\n");

	strFunc.append(strTable_2);
	strFunc.append("return false;\n");

	strFunc.append(strTable);
	strFunc.append("}\n");

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
		strClass.append("extern bool g_bUseFastModel;\n");
	}

	strClass.append(strTable);
	switch (filetype) {
	case 0:
	{
		sprintf(t_ch, "class SendData_%s\n", spacename.c_str());
	}
	break;
	case 1:
	{
		sprintf(t_ch, "class SendData_%s\n", spacename.c_str());
	}
	break;
	case 2:
	{
		sprintf(t_ch, "class SendData_%s\n", spacename.c_str());
	}
	break;
	default:
		break;
	}

	strClass.append(t_ch);

	strClass.append(strTable);
	strClass.append("{\n");
	strClass.append(strTable);
	strClass.append("public:\n");
	fwrite(strClass.c_str(), strClass.length(), 1, fp);
	strClass = "";

	for (auto& r : pClass._FuncVec) {
		if (r._bDiscard)
			continue;
		if (filetype == 2 && ((m_bSCSpliteS && r._SCSpliteType == 1) || (m_bSCSpliteC && r._SCSpliteType == -1)))
			continue;
		//д����
		WriteFunc_SendByteSeqV2_CPlus(r, fp, tablenum + 1, filetype, pClass._name, pNamespace);
	}

	strClass.append(strTable);
	strClass.append("};\n");

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

		//��,��
		if (i != pFunc._InParVec.size() - 1) {
			strFunc.append(",");
		}
	}

	strFunc.append(")\n");

	strFunc.append(strTable);
	strFunc.append("{\n");

	strFunc.append(strTable_2);
	sprintf(t_ch, "WJS_InByteStream ins;\n");
	strFunc.append(t_ch);

	strFunc.append(strTable_2);
	sprintf(t_ch, "ins<<%s;\n", m_ProtocolNo._name.c_str());
	strFunc.append(t_ch);

	strFunc.append(strTable_2);
	sprintf(t_ch, "ins<<(short)%d;\n", pFunc._version);
	strFunc.append(t_ch);

	strFunc.append(strTable_2);
	switch (filetype) {
	case 0:
		sprintf(t_ch, "ins<<%s::METHOD::ID_%s_%s;\n", pNamespace._clientname.c_str(), classname.c_str(), pFunc._name.c_str());
		break;
	case 1:
		sprintf(t_ch, "ins<<%s::METHOD::ID_%s_%s;\n", pNamespace._servername.c_str(), classname.c_str(), pFunc._name.c_str());
		break;
	case 2:
		sprintf(t_ch, "ins<<%s::METHOD::ID_%s_%s;\n", pNamespace._servername.c_str(), classname.c_str(), pFunc._name.c_str());
		break;
	default:
		sprintf(t_ch, "ins<<METHOD::ID_%s_%s;\n", classname.c_str(), pFunc._name.c_str());
		break;
	}


	strFunc.append(t_ch);

	for (auto& r : pFunc._InParVec) {
		strFunc.append(strTable_2);
		string tt_strNameSpace;
		if (IsExType_baseVector(r._strParType, filetype, tt_strNameSpace) && tt_strNameSpace.length() > 0) {
			sprintf(t_ch, "%s::operator <<(ins,%s);\n", tt_strNameSpace.c_str(), r._strParName.c_str());
		}
		else {
			sprintf(t_ch, "ins<<%s;\n", r._strParName.c_str());

		}

		strFunc.append(t_ch);
	}


	strFunc.append(strTable_2);
	strFunc.append("return ins;\n");

	strFunc.append(strTable);
	strFunc.append("}\n");

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

		//��,��
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

	strFunc.append(")\n");

	strFunc.append(strTable);
	strFunc.append("{\n");

	if (m_bFastModel) {
		strFunc.append(strTable_2);
		strFunc.append("if(g_bUseFastModel)\n");
		strFunc.append(strTable_3);
		strFunc.append("return;\n");
	}

	strFunc.append(strTable_2);
	sprintf(t_ch, "ins.OnlySetUseSize0();\n");
	strFunc.append(t_ch);

	strFunc.append(strTable_2);
	sprintf(t_ch, "ins<<%s;\n", m_ProtocolNo._name.c_str());
	strFunc.append(t_ch);

	strFunc.append(strTable_2);
	sprintf(t_ch, "ins<<(short)%d;\n", pFunc._version);
	strFunc.append(t_ch);

	strFunc.append(strTable_2);
	switch (filetype) {
	case 0:
		sprintf(t_ch, "ins<<%s::METHOD::ID_%s_%s;\n", pNamespace._clientname.c_str(), classname.c_str(), pFunc._name.c_str());
		break;
	case 1:
		if (pFunc._strSCClassName.length() > 0) {
			sprintf(t_ch, "ins<<%s::METHOD::ID_%s_%s;\n", pNamespace._servername.c_str(), pFunc._strSCClassName.c_str(), pFunc._name.c_str());

		}
		else
			sprintf(t_ch, "ins<<%s::METHOD::ID_%s_%s;\n", pNamespace._servername.c_str(), classname.c_str(), pFunc._name.c_str());
		break;
	case 2:
		if (pFunc._strSCClassName.length() > 0) {
			sprintf(t_ch, "ins<<%s::METHOD::ID_%s_%s;\n", pNamespace._servername.c_str(), pFunc._strSCClassName.c_str(), pFunc._name.c_str());

		}
		else
			sprintf(t_ch, "ins<<%s::METHOD::ID_%s_%s;\n", pNamespace._servername.c_str(), classname.c_str(), pFunc._name.c_str());
		break;
	default:
		sprintf(t_ch, "ins<<METHOD::ID_%s_%s;\n", classname.c_str(), pFunc._name.c_str());
		break;
	}
	strFunc.append(t_ch);

	for (auto& r : pFunc._InParVec) {
		strFunc.append(strTable_2);
		string tt_strNameSpace;
		if (IsExType_baseVector(r._strParType, filetype, tt_strNameSpace) && tt_strNameSpace.length() > 0) {
			sprintf(t_ch, "%s::operator <<(ins,%s);\n", tt_strNameSpace.c_str(), r._strParName.c_str());
		}
		else {
			sprintf(t_ch, "ins<<%s;\n", r._strParName.c_str());
		}

		strFunc.append(t_ch);
	}

	strFunc.append(strTable);
	strFunc.append("}\n");

	fwrite(strFunc.c_str(), strFunc.length(), 1, fp);
}




#pragma region ע��
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
		strLine.append("\n");
	}
	return strLine;
}
string  ProtocolParse_Writer_CPlus::WriteBlockCommentBegin_CPlus(const string& strTable)
{
	string strLine;
	strLine.append(strTable);
	strLine.append("/**\n");
	return strLine;
}
string  ProtocolParse_Writer_CPlus::WriteBlockCommentEnd_CPlus(const string& strTable)
{
	string strLine;
	strLine.append(strTable);
	strLine.append(" */\n");
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
			strLine.append("\n");
		}
		else {
			strLine.append(strTable);
			strLine.append(" * ");
			strLine.append(tt_strReal.c_str());
			strLine.append("\n");
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
			strLine.append("\n");
		}
		else {
			strLine.append(strTable);
			strLine.append(" * ");
			strLine.append(tt_strReal.c_str());
			strLine.append("\n");
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