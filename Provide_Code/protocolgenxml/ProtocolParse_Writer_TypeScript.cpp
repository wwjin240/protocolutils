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


//void    ProtocolParse_Writer_TypeScript::CreateAndWriteFuncIdAndProtocolNo_TScript(stNamespace& pNamespace, string foldername, const string& strWriteNameSpace)
//{
//	if (m_bTScriptUseNameSpaceVersion)
//	{
//		CreateAndWriteFuncIdAndProtocolNo_TScript_NSV(pNamespace,foldername,strWriteNameSpace);
//		return;
//	}
//	if (pNamespace._ClassVec.size() == 0)
//		return;
//
//	char t_ch[512];
//	sprintf(t_ch, "%s/ID_%s.ts", foldername.c_str(), pNamespace._protocolname.c_str());
//
//	//先创建文件
//	FILE* fp = fopen(t_ch, "w");
//	if (!fp)
//		return;
//
//	//写信息头,共用
//	WriteInfoHead(fp);
//
//	string strid;
//
//
//	string strTable_0 = CreateTableString(0);
//	string strTable_1 = CreateTableString(1);
//
//	strid.append(strTable_0);
//	sprintf(t_ch, "export class ID_%s\n", pNamespace._protocolname.c_str());
//	strid.append(t_ch);
//
//	strid.append(strTable_0);
//	strid.append("{\n");
//
//	//函数id
//	for (int i = 0; i < pNamespace._ClassVec.size(); i++)
//	{
//		for (int j = 0; j < pNamespace._ClassVec[i]._FuncVec.size(); j++)
//		{
//			strid.append(strTable_1);
//			strid.append("static  ");
//			sprintf(t_ch, "ID_%s_%s:number", pNamespace._ClassVec[i]._name.c_str(), pNamespace._ClassVec[i]._FuncVec[j]._name.c_str());
//			strid.append(t_ch);
//
//			sprintf(t_ch, "ID_%s_%s", pNamespace._ClassVec[i]._name.c_str(), pNamespace._ClassVec[i]._FuncVec[j]._name.c_str());
//
//			//if (pNamespace._ClassVec[i]._FuncVec[j]._name.compare("PhoneRechargeHttp") == 0)
//			//{
//			//	int a;
//			//	a = 10;
//			//}
//
//			int tempId;  //将字符串转成id
//			if (pNamespace._ClassVec[i]._FuncVec[j]._userFuncId != 0)
//				tempId = pNamespace._ClassVec[i]._FuncVec[j]._userFuncId;
//			else
//				tempId = CovertStringToInt(t_ch, pNamespace._ClassVec[i]._FuncVec[j]._bNew);
//			sprintf(t_ch, "=%d;\n", tempId);
//			strid.append(t_ch);
//		}
//	}
//
//	//协议号
//	if (m_ProtocolNo._value != 1)
//	{
//		//协议头
//		strid.append(strTable_1);
//		strid.append("static ");
//		sprintf(t_ch, "%s:number=%d;\n", m_ProtocolNo._name.c_str(), m_ProtocolNo._value);
//		strid.append(t_ch);
//	}
//
//	strid.append(strTable_0);
//	strid.append("}\n");
//
//	//strid.append("}\n");
//
//	fwrite(strid.c_str(), strid.length(), 1, fp);
//
//	fclose(fp);
//}

//void    ProtocolParse_Writer_TypeScript::WriteEnum_TScript(stNamespace& pNamespace, stEnum& pEnum, string foldername, const string& strWriteNameSpace)
//{
//	if (m_bTScriptUseNameSpaceVersion)
//	{
//		WriteEnum_TScript_NSV(pNamespace,pEnum,foldername,strWriteNameSpace);
//		return;
//	}
//	char t_ch[512];
//	sprintf(t_ch, "%s/%s.ts", foldername.c_str(), pEnum._name.c_str());
//
//	//先创建文件
//	FILE* fp = fopen(t_ch, "w");
//	if (!fp)
//		return;
//
//	//写信息头,共用
//	WriteInfoHead(fp);
//
//
//	string strTable_1 = CreateTableString(0);
//	string strTable_2 = CreateTableString(1);
//	string strTable_3 = CreateTableString(2);
//	string strTable_4 = CreateTableString(3);
//	string strTable_5 = CreateTableString(4);
//	//string strTable_6=CreateTableString(5);
//
//	string strEnum;
//
//
//
//	//sprintf(t_ch, "namespace yy.wjsproto.%s\n", strWriteNameSpace.c_str());
//	//strEnum.append(t_ch);
//	//strEnum.append("{\n");
//
//	strEnum.append(WriteComment_TScript(strTable_1, pEnum._comment, "").c_str());
//
//	strEnum.append(strTable_1.c_str());
//	sprintf(t_ch, "export enum %s\n", pEnum._name.c_str());
//	strEnum.append(t_ch);
//	strEnum.append(strTable_1.c_str());
//	strEnum.append("{\n");
//
//
//	for (int i = 0; i < pEnum._ParNameList.size(); i++)
//	{
//		strEnum.append(WriteComment_TScript(strTable_2, pEnum._commentList[i], "").c_str());
//		strEnum.append(strTable_2.c_str());
//		sprintf(t_ch, "%s = %d,\n", pEnum._ParNameList[i].c_str(), i/*CovertStringToInt(pEnum._ParNameList[i])*/);
//		strEnum.append(t_ch);
//	}
//
//	////写读
//	//strEnum.append(strTable_2);
//	//strEnum.append("public static int read(OutStream outStream)\n");
//
//	////
//	//strEnum.append(strTable_2);
//	//strEnum.append("{\n");
//
//	//strEnum.append(strTable_3);
//	//strEnum.append("int tt_a = outStream.ReadInt();\n");
//
//	//strEnum.append(strTable_3);
//	//strEnum.append("switch(tt_a)\n");
//
//	//strEnum.append(strTable_3);
//	//strEnum.append("{\n");
//
//	//for (int i = 0; i < pEnum._ParNameList.size(); i++)
//	//{
//	//	strEnum.append(strTable_4);
//	//	sprintf(t_ch, "case %d:\n", CovertStringToInt(pEnum._ParNameList[i]));
//	//	strEnum.append(t_ch);
//
//	//	strEnum.append(strTable_5);
//	//	sprintf(t_ch, "return %s;\n", pEnum._ParNameList[i].c_str());
//	//	strEnum.append(t_ch);
//	//}
//
//	//if (pEnum._ParNameList.size() > 0)
//	//{
//	//	strEnum.append(strTable_4);
//	//	strEnum.append("default:\n");
//
//	//	strEnum.append(strTable_5);
//	//	sprintf(t_ch, "return %s;\n", pEnum._ParNameList[0].c_str());
//	//	strEnum.append(t_ch);
//	//}
//
//	//strEnum.append(strTable_3);
//	//strEnum.append("}\n");
//
//	////strEnum.append(strTable_3);
//	////strEnum.append("return tt_a;\n");
//
//	//strEnum.append(strTable_2);
//	//strEnum.append("}\n");
//
//
//	//strEnum.append(strTable_2);
//	//strEnum.append("public static void write(InStream inStream,int va)\n");
//
//	//strEnum.append(strTable_2);
//	//strEnum.append("{\n");
//
//	//strEnum.append(strTable_3);
//	//strEnum.append("switch(va)\n");
//
//	//strEnum.append(strTable_3);
//	//strEnum.append("{\n");
//
//
//	//for (int i = 0; i < pEnum._ParNameList.size(); i++)
//	//{
//	//	strEnum.append(strTable_4);
//	//	sprintf(t_ch, "case %s:\n", pEnum._ParNameList[i].c_str());
//	//	strEnum.append(t_ch);
//
//	//	strEnum.append(strTable_5);
//	//	WJS_BBSTreeNode<string, int>* pNode = m_StringIntSearchMap.Search(pEnum._ParNameList[i]);
//	//	int tempValue = 0;
//	//	if (pNode)
//	//	{
//	//		tempValue = pNode->Second();
//	//	}
//
//	//	sprintf(t_ch, "inStream.WriteInt(%d);\n", tempValue);
//	//	strEnum.append(t_ch);
//
//	//	strEnum.append(strTable_5);
//	//	strEnum.append("break;\n");
//	//}
//
//	//strEnum.append(strTable_4);
//	//strEnum.append("default:\n");
//
//	//strEnum.append(strTable_5);
//	//strEnum.append("inStream.WriteInt(0);\n");
//
//	//strEnum.append(strTable_5);
//	//strEnum.append("break;\n");
//
//	//strEnum.append(strTable_3);
//	//strEnum.append("}\n");
//
//	//strEnum.append(strTable_2);
//	//strEnum.append("}\n");
//
//	strEnum.append(strTable_1.c_str());
//	strEnum.append("}\n");
//
//	//strEnum.append("}\n");
//
//	fwrite(strEnum.c_str(), strEnum.length(), 1, fp);
//
//	fclose(fp);
//}
//void    ProtocolParse_Writer_TypeScript::WriteStruct_TScript(stNamespace& pNamespace, stStructStruct& pStruct, string foldername, const string& strWriteNameSpace)
//{
//	if(!pStruct._bBiaoZhuUsed)
//		return;
//	if (m_bTScriptUseNameSpaceVersion)
//	{
//		WriteStruct_TScript_NSV(pNamespace, pStruct, foldername, strWriteNameSpace);
//		return;
//	}
//	char t_ch[512];
//	sprintf(t_ch, "%s/%s.ts", foldername.c_str(), pStruct._name.c_str());
//
//	//先创建文件
//	FILE* fp = fopen(t_ch, "w");
//	if (!fp)
//		return;
//
//	//写信息头,共用
//	WriteInfoHead(fp);
//
//
//	string strTable_1 = CreateTableString(0);
//	string strTable_2 = CreateTableString(1);
//	string strTable_3 = CreateTableString(2);
//	string strTable_4 = CreateTableString(3);
//	string strTable_5 = CreateTableString(4);
//	string strTable_6 = CreateTableString(5);
//
//	string strStruct;
//
//
//	//sprintf(t_ch, "namespace yy.wjsproto.%s\n", strWriteNameSpace.c_str());
//	//strStruct.append(t_ch);
//	//strStruct.append("{\n");
//
//	vector<string> tt_PacketNameList;
//	tt_PacketNameList.push_back("import {InStream} from \"../../stream/InStream\"");
//	tt_PacketNameList.push_back("import {OutStream} from \"../../stream/OutStream\"");
//	tt_PacketNameList.push_back("import {Int64} from \"../../base/Int64\"");
//	for (int i = 0; i < pStruct._ParVec.size(); i++)
//	{
//		if (!IsTScriptBaseType(pStruct._ParVec[i]._strParType, false,true))
//		{
//			GetUsePacketNamesByParType_TScript(pStruct._ParVec[i]._strParType, tt_PacketNameList, GetSubFolderName(foldername));
//		}
//	}
//	string tt_strSelf;
//	sprintf(t_ch, "import {%s} from  \"./%s\"", pStruct._name.c_str(), pStruct._name.c_str());
//	tt_strSelf = t_ch;
//	RemovePacketNames(tt_PacketNameList, tt_strSelf);
//
//	for (int i = 0; i < tt_PacketNameList.size(); i++)
//	{
//		strStruct.append(strTable_1);
//		strStruct.append(tt_PacketNameList[i].c_str());
//		strStruct.append("\n");
//	}
//	if(tt_PacketNameList.size()>0)
//		strStruct.append("\n");
//
//
//
//	strStruct.append(WriteComment_TScript(strTable_1, pStruct._comment, "").c_str());
//	strStruct.append(strTable_1);
//	sprintf(t_ch, "export class %s\n", pStruct._name.c_str());
//	strStruct.append(t_ch);
//
//	strStruct.append(strTable_1);
//	strStruct.append("{\n");
//
//	if (pStruct._bWithConstruct)
//	{
//		strStruct.append(strTable_2);
//		strStruct.append("constructor()\n");
//		strStruct.append(strTable_2);
//		strStruct.append("{\n");
//
//		for (int i = 0; i < pStruct._ParVec.size(); i++)
//		{
//			if (pStruct._ParVec[i]._bWithDefault)
//			{
//				//if (pStruct._name.compare("PtChangedEvent") == 0)
//				//{
//				//	int a;
//				//	a = 10;
//				//}
//				strStruct.append(strTable_3);
//				sprintf(t_ch, "this.%s=%s;\n", pStruct._ParVec[i]._strParName.c_str(), ConvertDefaultValue(pStruct._ParVec[i]._strParType,pStruct._ParVec[i]._strDefaultValue).c_str());
//				strStruct.append(t_ch);
//			}
//		}
//
//		strStruct.append(strTable_2);
//		strStruct.append("}\n");
//	}
//
//
//	for (int i = 0; i < pStruct._ParVec.size(); i++)
//	{
//		strStruct.append(WriteComment_TScript(strTable_2, pStruct._ParVec[i]._comment, "").c_str());
//		strStruct.append(strTable_2);
//		if (IsExType_vector(pStruct._ParVec[i]._strParType))
//		{
//			sprintf(t_ch, "%s:%s;\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToTScriptType(pStruct._ParVec[i]._strParType.c_str()).c_str());
//		}
//		else
//		{
//			sprintf(t_ch, "%s:%s;\n", pStruct._ParVec[i]._strParName.c_str() , TypeConvertToTScriptType(pStruct._ParVec[i]._strParType.c_str()).c_str());
//		}
//		strStruct.append(t_ch);
//	}
//
//	//写读
//	strStruct.append(strTable_2);
//	sprintf(t_ch, "static  read_s(outs:OutStream):%s\n", pStruct._name.c_str());
//	strStruct.append(t_ch);
//	strStruct.append(strTable_2);
//	strStruct.append("{\n");
//
//	strStruct.append(strTable_3);
//	sprintf(t_ch, "let o:%s = new %s();\n", pStruct._name.c_str(), pStruct._name.c_str());
//	strStruct.append(t_ch);
//
//	strStruct.append(strTable_3);
//	sprintf(t_ch, "o.read(outs);\n");
//	strStruct.append(t_ch);
//
//	strStruct.append(strTable_3);
//	strStruct.append("return o;\n");
//
//	strStruct.append(strTable_2);
//	strStruct.append("}\n");
//
//	//////////////////
//	////
//	/////////////////
//
//	//写读
//	strStruct.append(strTable_2);
//	sprintf(t_ch, "public read(outs:OutStream)\n");
//	strStruct.append(t_ch);
//	strStruct.append(strTable_2);
//	strStruct.append("{\n");
//
//	strStruct.append(strTable_3);
//	strStruct.append("let tt_len:number =0;\n");
//
//	strStruct.append(strTable_3);
//	strStruct.append("tt_len = outs.readInt16();\n");
//
//	strStruct.append(strTable_3);
//	strStruct.append("let tt_version:number=0;\n");
//
//	strStruct.append(strTable_3);
//	strStruct.append("tt_version = outs.readInt16();\n");
//
//	if (pStruct.IsHasTransNeedValuePar())
//	{
//		strStruct.append(strTable_4);
//		strStruct.append("let tt_logsize1:number = outs.getPos();\n");
//	}
//
//	if (pStruct.IsHasTransNeedValuePar())
//	{
//		strStruct.append(strTable_3);
//		strStruct.append("let tt_transNeedValue:number=0;\n");
//
//		strStruct.append(strTable_3);
//		strStruct.append("tt_transNeedValue = outs.readInt32();\n");
//	}
//
//
//	strStruct.append(strTable_3);
//	sprintf(t_ch, "if(tt_version == %d || (tt_version==0))\n", pStruct._version);
//	strStruct.append(t_ch);
//
//	strStruct.append(strTable_3);
//	strStruct.append("{\n");
//
//
//
//	for (int i = 0; i < pStruct._ParVec.size(); i++)
//	{
//		if (pStruct.IsHasTransNeedValuePar() && (pStruct._ParVec[i]._transNeedValue != -1))
//		{
//			strStruct.append(strTable_4);
//			sprintf(t_ch, "if((tt_transNeedValue & (1<<%d))!=0)\n", pStruct._ParVec[i]._transNeedValue);
//			strStruct.append(t_ch);
//			strStruct.append(strTable_4);
//			strStruct.append("{\n");
//
//			strStruct.append(strTable_5);
//			if (IsTScriptBaseType(pStruct._ParVec[i]._strParType, true))
//			{
//				sprintf(t_ch, "this.%s = outs.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadTScriptType(pStruct._ParVec[i]._strParType.c_str()).c_str());
//			}
//			else
//			{
//				sprintf(t_ch, "this.%s = %s.read_s(outs);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
//
//			}
//			strStruct.append(t_ch);
//
//			strStruct.append(strTable_4);
//			strStruct.append("}\n");
//			strStruct.append(strTable_4);
//			strStruct.append("else\n");
//			strStruct.append(strTable_4);
//			strStruct.append("{\n");
//			strStruct.append(strTable_5);
//			sprintf(t_ch, "this.%s=%s;\n", pStruct._ParVec[i]._strParName.c_str(), ConvertDefaultValue(pStruct._ParVec[i]._strParType,pStruct._ParVec[i]._strDefaultValue).c_str());
//			strStruct.append(t_ch);
//			strStruct.append(strTable_4);
//			strStruct.append("}\n");
//		}
//		else
//		{
//			strStruct.append(strTable_4);
//			if (IsTScriptBaseType(pStruct._ParVec[i]._strParType, true))
//			{
//				sprintf(t_ch, "this.%s = outs.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadTScriptType(pStruct._ParVec[i]._strParType.c_str()).c_str());
//			}
//			else
//			{
//				sprintf(t_ch, "this.%s = %s.read_s(outs);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
//			}
//			strStruct.append(t_ch);
//		}
//
//
//	}
//	strStruct.append(strTable_3);
//	strStruct.append("}\n");
//
//	strStruct.append(strTable_3);
//	sprintf(t_ch, "else if(tt_version > %d)\n", pStruct._version);
//	strStruct.append(t_ch);
//
//	strStruct.append(strTable_3);
//	strStruct.append("{\n");
//
//	if (!pStruct.IsHasTransNeedValuePar())
//	{
//		strStruct.append(strTable_4);
//		strStruct.append("let tt_logsize1:number = outs.getPos();\n");
//	}
//
//
//	for (int i = 0; i < pStruct._ParVec.size(); i++)
//	{
//		if (pStruct.IsHasTransNeedValuePar() && (pStruct._ParVec[i]._transNeedValue != -1))
//		{
//			strStruct.append(strTable_4);
//			sprintf(t_ch, "if((tt_transNeedValue & (1<<%d))!=0)\n", pStruct._ParVec[i]._transNeedValue);
//			strStruct.append(t_ch);
//			strStruct.append(strTable_4);
//			strStruct.append("{\n");
//
//			strStruct.append(strTable_5);
//			if (IsTScriptBaseType(pStruct._ParVec[i]._strParType, true))
//			{
//
//				sprintf(t_ch, "this.%s = outs.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadTScriptType(pStruct._ParVec[i]._strParType.c_str()).c_str());
//			}
//			else
//			{
//				sprintf(t_ch, "this.%s = %s.read_s(outs);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
//
//			}
//			strStruct.append(t_ch);
//
//			strStruct.append(strTable_4);
//			strStruct.append("}\n");
//			strStruct.append(strTable_4);
//			strStruct.append("else\n");
//			strStruct.append(strTable_4);
//			strStruct.append("{\n");
//			strStruct.append(strTable_5);
//			sprintf(t_ch, "this.%s=%s;\n", pStruct._ParVec[i]._strParName.c_str(), ConvertDefaultValue(pStruct._ParVec[i]._strParType, pStruct._ParVec[i]._strDefaultValue).c_str());
//			strStruct.append(t_ch);
//			strStruct.append(strTable_4);
//			strStruct.append("}\n");
//		}
//		else
//		{
//			strStruct.append(strTable_4);
//			if (IsTScriptBaseType(pStruct._ParVec[i]._strParType, true))
//			{
//				sprintf(t_ch, "this.%s = outs.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadTScriptType(pStruct._ParVec[i]._strParType.c_str()).c_str());
//			}
//			else
//			{
//				sprintf(t_ch, "this.%s = %s.read_s(outs);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
//
//			}
//			strStruct.append(t_ch);
//		}
//	}
//
//	strStruct.append(strTable_4);
//	strStruct.append("let tt_logsize2:number = outs.getPos();\n");
//
//	strStruct.append(strTable_4);
//	strStruct.append("let tt_offset:number = tt_len - (tt_logsize2 - tt_logsize1);\n");
//
//	strStruct.append(strTable_4);
//	strStruct.append("if(tt_offset>0)\n");
//
//	strStruct.append(strTable_4);
//	strStruct.append("{\n");
//
//	strStruct.append(strTable_5);
//	strStruct.append("outs.addOffset(tt_offset);\n");
//
//	strStruct.append(strTable_4);
//	strStruct.append("}\n");
//
//
//	strStruct.append(strTable_3);
//	strStruct.append("}\n");
//
//	strStruct.append(strTable_3);
//	strStruct.append("else\n");
//
//	strStruct.append(strTable_3);
//	strStruct.append("{\n");
//
//	for (int i = 0; i < pStruct._ParVec.size(); i++)
//	{
//		string strTable_5_temp = strTable_5;
//		string strTable_6_temp = strTable_6;
//		bool tt_bNeedVersionCheck = true;
//		if (pStruct._ParVec[i]._version <= 1 || pStruct._ParVec[i]._version <= pStruct._startversion_ts)
//		{
//			tt_bNeedVersionCheck = false;
//			strTable_5_temp = strTable_4;
//			strTable_6_temp = strTable_5;
//		}
//
//		if (tt_bNeedVersionCheck)
//		{
//			strStruct.append(strTable_4);
//			sprintf(t_ch, "if(tt_version>=%d)\n", pStruct._ParVec[i]._version);
//			strStruct.append(t_ch);
//		}
//
//		if (pStruct.IsHasTransNeedValuePar() && (pStruct._ParVec[i]._transNeedValue != -1))
//		{
//			if (tt_bNeedVersionCheck)
//			{
//				strStruct.append(strTable_4);
//				strStruct.append("{\n");
//			}
//			strStruct.append(strTable_5_temp);
//			sprintf(t_ch, "if((tt_transNeedValue & (1<<%d))!=0)\n", pStruct._ParVec[i]._transNeedValue);
//			strStruct.append(t_ch);
//			strStruct.append(strTable_5_temp);
//			strStruct.append("{\n");
//
//			strStruct.append(strTable_6_temp);
//			if (IsTScriptBaseType(pStruct._ParVec[i]._strParType, true))
//			{
//					sprintf(t_ch, "this.%s = outs.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadTScriptType(pStruct._ParVec[i]._strParType.c_str()).c_str());
//			}
//			else
//			{
//				sprintf(t_ch, "this.%s = %s.read_s(outs);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
//			}
//			strStruct.append(t_ch);
//
//			strStruct.append(strTable_5_temp);
//			strStruct.append("}\n");
//			strStruct.append(strTable_5_temp);
//			strStruct.append("else\n");
//			strStruct.append(strTable_5_temp);
//			strStruct.append("{\n");
//			strStruct.append(strTable_6_temp);
//			sprintf(t_ch, "this.%s=%s;\n", pStruct._ParVec[i]._strParName.c_str(), ConvertDefaultValue(pStruct._ParVec[i]._strParType, pStruct._ParVec[i]._strDefaultValue).c_str());
//			strStruct.append(t_ch);
//			strStruct.append(strTable_5_temp);
//			strStruct.append("}\n");
//
//			if (tt_bNeedVersionCheck)
//			{
//				strStruct.append(strTable_4);
//				strStruct.append("}\n");
//			}
//
//		}
//		else
//		{
//			strStruct.append(strTable_5_temp);
//			if (IsTScriptBaseType(pStruct._ParVec[i]._strParType, true))
//			{
//				sprintf(t_ch, "this.%s = outs.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadTScriptType(pStruct._ParVec[i]._strParType.c_str()).c_str());
//			}
//			else
//			{
//				sprintf(t_ch, "this.%s = %s.read_s(outs);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
//			}
//			strStruct.append(t_ch);
//		}
//
//		if (tt_bNeedVersionCheck)
//		{
//			string tt_default = GetDefaultValue(pStruct._ParVec[i]._strParType, false, false, false, true);
//			if (tt_default.length() > 0)
//			{
//				strStruct.append(strTable_4);
//				strStruct.append("else\n");
//
//				strStruct.append(strTable_5);
//				sprintf(t_ch, "this.%s=%s;\n", pStruct._ParVec[i]._strParName.c_str(), tt_default.c_str());
//				strStruct.append(t_ch);
//			}
//		}
//
//	}
//
//	strStruct.append(strTable_3);
//	strStruct.append("}\n");
//
//	strStruct.append(strTable_2);
//	strStruct.append("}\n");
//
//	////////////////////
//	////
//	////////////////////
//
//	//写写
//	strStruct.append(strTable_2);
//	strStruct.append("public  write(ins:InStream)\n");
//	strStruct.append(strTable_2);
//	strStruct.append("{\n");
//
//	//先写入结构体的大小
//	strStruct.append(strTable_3);
//	strStruct.append("let tt_len:number  = 0;\n");
//	strStruct.append(strTable_3);
//	strStruct.append("let tt_logsize1:number = ins.getPos();\n");
//	strStruct.append(strTable_3);
//	strStruct.append("ins.writeInt16(tt_len);\n");
//
//
//	//再写入版本
//	strStruct.append(strTable_3);
//	sprintf(t_ch, "let  tt_version:number = %d;\n", pStruct._version);
//	strStruct.append(t_ch);
//	strStruct.append(strTable_3);
//	strStruct.append("ins.writeInt16(tt_version);\n");
//
//	strStruct.append(strTable_3);
//	strStruct.append("let tt_logsize2:number = ins.getPos();\n");
//
//	if (pStruct.IsHasTransNeedValuePar())
//	{
//		strStruct.append(strTable_3);
//		strStruct.append("let tt_transNeedValue:number=0;\n");
//		strStruct.append(strTable_3);
//		strStruct.append("ins.writeInt32(tt_transNeedValue);\n");
//	}
//
//
//
//	for (int i = 0; i < pStruct._ParVec.size(); i++)
//	{
//		if (pStruct._ParVec[i]._transNeedValue != -1)
//		{
//			strStruct.append(strTable_3);
//			if(IsInt64_TScript(pStruct._ParVec[i]._strParType))
//			   sprintf(t_ch, "if(this.%s.isNotEqual(%s))\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strDefaultValue.c_str());
//			else
//			   sprintf(t_ch, "if(this.%s!=%s)\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strDefaultValue.c_str());
//			strStruct.append(t_ch);
//			strStruct.append(strTable_3);
//			strStruct.append("{\n");
//
//
//			strStruct.append(strTable_4);
//			if (IsTScriptBaseType(pStruct._ParVec[i]._strParType, true))
//			{
//				sprintf(t_ch, "ins.%s(this.%s);\n", TypeConvertToWriteTScriptType(pStruct._ParVec[i]._strParType.c_str()).c_str(), pStruct._ParVec[i]._strParName.c_str());
//			}
//			else
//			{
//				sprintf(t_ch, "this.%s.write(ins);\n", pStruct._ParVec[i]._strParName.c_str());
//			}
//			strStruct.append(t_ch);
//
//
//			strStruct.append(strTable_4);
//			sprintf(t_ch, "tt_transNeedValue = tt_transNeedValue | (1<<%d);\n", pStruct._ParVec[i]._transNeedValue);
//			strStruct.append(t_ch);
//
//			strStruct.append(strTable_3);
//			strStruct.append("}\n");
//		}
//		else
//		{
//			strStruct.append(strTable_3);
//			if (IsTScriptBaseType(pStruct._ParVec[i]._strParType, true))
//			{
//				sprintf(t_ch, "ins.%s(this.%s);\n", TypeConvertToWriteTScriptType(pStruct._ParVec[i]._strParType.c_str()).c_str(), pStruct._ParVec[i]._strParName.c_str());
//			}
//			else
//			{
//				sprintf(t_ch, "this.%s.write(ins);\n", pStruct._ParVec[i]._strParName.c_str());
//			}
//			strStruct.append(t_ch);
//		}
//
//
//	}
//	strStruct.append(strTable_3);
//	strStruct.append("let tt_logsize4:number = ins.getPos();\n");
//	strStruct.append(strTable_3);
//	strStruct.append("let tt_logsize3:number = tt_logsize4 - tt_logsize2;\n");
//	strStruct.append(strTable_3);
//	strStruct.append("ins.setPos(tt_logsize1);\n");
//	strStruct.append(strTable_3);
//	strStruct.append("ins.writeInt16(tt_logsize3);\n");
//
//	if (pStruct.IsHasTransNeedValuePar())
//	{
//		strStruct.append(strTable_3);
//		strStruct.append("if(tt_transNeedValue!=0)\n");
//		strStruct.append(strTable_3);
//		strStruct.append("{\n");
//		strStruct.append(strTable_4);
//		strStruct.append("ins.setPos(tt_logsize2);\n");
//		strStruct.append(strTable_4);
//		strStruct.append("ins.writeInt32(tt_transNeedValue);\n");
//		strStruct.append(strTable_3);
//		strStruct.append("}\n");
//	}
//
//
//	strStruct.append(strTable_3);
//	strStruct.append("ins.setPos(tt_logsize4);\n");
//
//	strStruct.append(strTable_2);
//	strStruct.append("}\n");
//
//
//
//	strStruct.append(strTable_1);
//	strStruct.append("}\n");
//	//strStruct.append("}\n");
//
//	fwrite(strStruct.c_str(), strStruct.length(), 1, fp);
//
//	fclose(fp);
//}
//void    ProtocolParse_Writer_TypeScript::WriteVector_TScript(stNamespace& pNamespace, stVector& pVector, string foldername, const string& strWriteNameSpace)
//{
//	if (!pVector._bBiaoZhuUsed)
//		return;
//	if (m_bTScriptUseNameSpaceVersion)
//	{
//		WriteVector_TScript_NSV(pNamespace, pVector, foldername, strWriteNameSpace);
//		return;
//	}
//	if (pVector._ParTName == "byte" || pVector._ParTName == "BYTE" || pVector._ParTName == "char" || pVector._ParTName == "unsigned char")
//	{
//		WriteVector2_TScript(pNamespace,pVector,foldername,strWriteNameSpace);
//		return;
//	}
//	char t_ch[512];
//	sprintf(t_ch, "%s/%s.ts", foldername.c_str(), pVector._name.c_str());
//
//	//先创建文件
//	FILE* fp = fopen(t_ch, "w");
//	if (!fp)
//		return;
//
//	//写信息头,共用
//	WriteInfoHead(fp);
//
//
//
//	//string strTable_0=CreateTableString(1);
//	string strTable_1 = CreateTableString(0);
//	string strTable_2 = CreateTableString(1);
//	string strTable_3 = CreateTableString(2);
//	string strTable_4 = CreateTableString(3);
//
//	string strVector;
//
//	//sprintf(t_ch, "namespace yy.wjsproto.%s\n", strWriteNameSpace.c_str());
//	//strVector.append(t_ch);
//	//strVector.append("{\n");
//
//	//if (pVector._name.compare("PtCatchFishJoyRankList") == 0)
//	//{
//	//	int a;
//	//	a = 10;
//	//}
//
//	vector<string> tt_PacketNameList;
//	tt_PacketNameList.push_back("import {InStream} from \"../../stream/InStream\"");
//	tt_PacketNameList.push_back("import {OutStream} from \"../../stream/OutStream\"");
//	tt_PacketNameList.push_back("import {Int64} from \"../../base/Int64\"");
//	GetUsePacketNamesByParType_TScript(pVector._name, tt_PacketNameList, GetSubFolderName(foldername));
//	string tt_strSelf;
//	sprintf(t_ch, "import {%s} from  \"./%s\"", pVector._name.c_str(), pVector._name.c_str());
//	tt_strSelf = t_ch;
//	//sprintf(t_ch, "import {%s} from  \"../%s/%s\"", pVector._name.c_str(), pNamespace._protocolname.c_str(),pVector._name.c_str());
//	//tt_strSelf = t_ch;
//	RemovePacketNames(tt_PacketNameList, tt_strSelf);
//
//	for (int i = 0; i < tt_PacketNameList.size(); i++)
//	{
//		strVector.append(strTable_1);
//		strVector.append(tt_PacketNameList[i].c_str());
//		strVector.append("\n");
//	}
//	if (tt_PacketNameList.size() > 0)
//		strVector.append("\n");
//
//
//
//	/////////////有两种版本，一种是非Flash基础类型，一种是
//	bool  tt_bTScritpBaseType = IsTScriptBaseType2(pVector._ParTName);
//
//	strVector.append(WriteComment_TScript(strTable_1, pVector._comment, "").c_str());
//
//	strVector.append(strTable_1);
//	sprintf(t_ch, "export class %s extends Array<%s>\n", pVector._name.c_str(), TypeConvertToTScriptType(pVector._ParTName).c_str());
//	strVector.append(t_ch);
//	strVector.append(strTable_1);
//	strVector.append("{\n");
//
//	strVector.append(strTable_2);
//	sprintf(t_ch, "constructor()\n", pVector._name.c_str());
//	strVector.append(t_ch);
//
//	strVector.append(strTable_2);
//	strVector.append("{\n");
//
//	strVector.append(strTable_3);
//	strVector.append("super()\n");
//
//	strVector.append(strTable_2);
//	strVector.append("}\n");
//
//
//	strVector.append(strTable_2);
//	sprintf(t_ch, "static  read_s(outs:OutStream):%s\n", pVector._name.c_str());
//	strVector.append(t_ch);
//
//	strVector.append(strTable_2);
//	strVector.append("{\n");
//
//	strVector.append(strTable_3);
//	sprintf(t_ch, "var o:%s = new %s();\n", pVector._name.c_str(), pVector._name.c_str());
//	strVector.append(t_ch);
//
//	strVector.append(strTable_3);
//	strVector.append("let count:number = outs.readInt32();\n");
//
//
//	strVector.append(strTable_3);
//	strVector.append("for(let i=0;i<count;i++)\n");
//
//	strVector.append(strTable_3);
//	strVector.append("{\n");
//
//	if (!tt_bTScritpBaseType)
//	{
//		strVector.append(strTable_4);
//		sprintf(t_ch, "o.push(%s.read_s(outs));\n", ExTypeNameConvert_TScript(pVector._ParTName).c_str());
//		strVector.append(t_ch);
//	}
//	else
//	{
//		strVector.append(strTable_4);
//		sprintf(t_ch, "o.push(outs.%s());\n", TypeConvertToReadTScriptType(pVector._ParTName).c_str());
//		strVector.append(t_ch);
//	}
//
//	strVector.append(strTable_3);
//	strVector.append("}\n");
//
//	strVector.append(strTable_3);
//	strVector.append("return o;\n");
//
//	strVector.append(strTable_2);
//	strVector.append("}\n");
//
//	////////////////
//	//
//	////////////////
//
//	strVector.append(strTable_2);
//	sprintf(t_ch, "public  read(outs:OutStream)\n");
//	strVector.append(t_ch);
//
//	strVector.append(strTable_2);
//	strVector.append("{\n");
//
//	strVector.append(strTable_3);
//	strVector.append("this.length=0;\n");
//
//
//	strVector.append(strTable_3);
//	strVector.append("let count:number = outs.readInt32();\n");
//
//
//
//	strVector.append(strTable_3);
//	strVector.append("for(let i=0;i<count;i++)\n");
//
//	strVector.append(strTable_3);
//	strVector.append("{\n");
//
//	if (!tt_bTScritpBaseType)
//	{
//		strVector.append(strTable_4);
//		sprintf(t_ch, "this.push(%s.read_s(outs));\n", ExTypeNameConvert_TScript(pVector._ParTName).c_str());
//		strVector.append(t_ch);
//	}
//	else
//	{
//		strVector.append(strTable_4);
//		sprintf(t_ch, "this.push(outs.%s());\n", TypeConvertToReadTScriptType(pVector._ParTName).c_str());
//		strVector.append(t_ch);
//	}
//
//	strVector.append(strTable_3);
//	strVector.append("}\n");
//
//	strVector.append(strTable_2);
//	strVector.append("}\n");
//
//	/////////////
//	//
//	////////////
//	strVector.append(strTable_2);
//	strVector.append("public  write(ins:InStream)\n");
//
//	strVector.append(strTable_2);
//	strVector.append("{\n");
//
//	strVector.append(strTable_3);
//	strVector.append("ins.writeInt32(this.length);\n");
//
//	strVector.append(strTable_3);
//	strVector.append("for(let i=0;i<this.length;i++)\n");
//
//	strVector.append(strTable_3);
//	strVector.append("{\n");
//	//
//	if (!tt_bTScritpBaseType)
//	{
//		strVector.append(strTable_4);
//		sprintf(t_ch, "this[i].write(ins);\n", ExTypeNameConvert_TScript(pVector._ParTName).c_str());
//		strVector.append(t_ch);
//	}
//	else
//	{
//		strVector.append(strTable_4);
//		sprintf(t_ch, "ins.%s(this[i]);\n", TypeConvertToWriteTScriptType(pVector._ParTName).c_str());
//		strVector.append(t_ch);
//	}
//
//
//	strVector.append(strTable_3);
//	strVector.append("}\n");
//
//	strVector.append(strTable_2);
//	strVector.append("}\n");
//
//	strVector.append(strTable_1);
//	strVector.append("}\n");
//
//	//strVector.append("}\n");
//
//
//
//	fwrite(strVector.c_str(), strVector.length(), 1, fp);
//
//	fclose(fp);
//}
//void    ProtocolParse_Writer_TypeScript::WriteVector2_TScript(stNamespace& pNamespace, stVector& pVector, string foldername, const string& strWriteNameSpace)
//{
//	if (!pVector._bBiaoZhuUsed)
//		return;
//	char t_ch[512];
//	sprintf(t_ch, "%s/%s.ts", foldername.c_str(), pVector._name.c_str());
//
//	//先创建文件
//	FILE* fp = fopen(t_ch, "w");
//	if (!fp)
//		return;
//
//	//写信息头,共用
//	WriteInfoHead(fp);
//
//
//	//string strTable_0=CreateTableString(1);
//	string strTable_1 = CreateTableString(0);
//	string strTable_2 = CreateTableString(1);
//	string strTable_3 = CreateTableString(2);
//	string strTable_4 = CreateTableString(3);
//
//	string strVector;
//
//
//
//	vector<string> tt_PacketNameList;
//	tt_PacketNameList.push_back("import {InStream} from \"../../stream/InStream\"");
//	tt_PacketNameList.push_back("import {OutStream} from \"../../stream/OutStream\"");
//
//
//	for (int i = 0; i < tt_PacketNameList.size(); i++)
//	{
//		strVector.append(strTable_1);
//		strVector.append(tt_PacketNameList[i].c_str());
//		strVector.append("\n");
//	}
//	if (tt_PacketNameList.size() > 0)
//		strVector.append("\n");
//
//
//
//	strVector.append(strTable_1);
//	sprintf(t_ch, "export  class %s\n", pVector._name.c_str());
//	strVector.append(t_ch);
//	strVector.append(strTable_1);
//	strVector.append("{\n");
//
//	strVector.append(strTable_2);
//	strVector.append("public _buffer:Uint8Array;\n");
//
//	strVector.append(strTable_2);
//	sprintf(t_ch, "constructor()\n", pVector._name.c_str());
//	strVector.append(t_ch);
//
//	strVector.append(strTable_2);
//	strVector.append("{\n");
//
//	strVector.append(strTable_2);
//	strVector.append("}\n");
//
//
//	strVector.append(strTable_2);
//	sprintf(t_ch, "static  read_s(outs:OutStream):%s\n", pVector._name.c_str());
//	strVector.append(t_ch);
//
//	strVector.append(strTable_2);
//	strVector.append("{\n");
//
//	strVector.append(strTable_3);
//	sprintf(t_ch, "let o:%s = new %s();\n", pVector._name.c_str(), pVector._name.c_str());
//	strVector.append(t_ch);
//
//	strVector.append(strTable_3);
//	strVector.append("o.read(outs);\n");
//
//	strVector.append(strTable_3);
//	strVector.append("return o;\n");
//
//	strVector.append(strTable_2);
//	strVector.append("}\n");
//
//	////////////////
//	//
//	////////////////
//
//	strVector.append(strTable_2);
//	sprintf(t_ch, "public  read(outs:OutStream)\n");
//	strVector.append(t_ch);
//
//	strVector.append(strTable_2);
//	strVector.append("{\n");
//
//	strVector.append(strTable_3);
//	strVector.append("this._buffer=outs.readUint8Array(outs.readInt32());\n");
//
//	strVector.append(strTable_2);
//	strVector.append("}\n");
//
//	/////////////
//	//
//	////////////
//	strVector.append(strTable_2);
//	strVector.append("public  write(ins:InStream)\n");
//
//	strVector.append(strTable_2);
//	strVector.append("{\n");
//
//	strVector.append(strTable_3);
//	strVector.append("ins.writeUint8Array(this._buffer);\n");
//
//	strVector.append(strTable_2);
//	strVector.append("}\n");
//
//	strVector.append(strTable_1);
//	strVector.append("}\n");
//
//	//strVector.append("}\n");
//
//
//
//	fwrite(strVector.c_str(), strVector.length(), 1, fp);
//
//	fclose(fp);
//}
//void    ProtocolParse_Writer_TypeScript::WriteWJSVector_TScript(stNamespace& pNamespace, stWJSVector& pWJSVector, string foldername, const string& strWriteNameSpace)
//{
//	if (!pWJSVector._bBiaoZhuUsed)
//		return;
//	if (m_bTScriptUseNameSpaceVersion)
//	{
//		WriteWJSVector_TScript_NSV(pNamespace,pWJSVector,foldername,strWriteNameSpace);
//		return;
//	}
//	if (pWJSVector._ParTName == "byte" || pWJSVector._ParTName == "BYTE" || pWJSVector._ParTName == "char" || pWJSVector._ParTName == "unsigned char")
//	{
//		WriteWJSVector2_TScript(pNamespace, pWJSVector, foldername, strWriteNameSpace);
//		return;
//	}
//
//	char t_ch[512];
//	sprintf(t_ch, "%s/%s.ts", foldername.c_str(), pWJSVector._name.c_str());
//
//	//先创建文件
//	FILE* fp = fopen(t_ch, "w");
//	if (!fp)
//		return;
//
//	//写信息头,共用
//	WriteInfoHead(fp);
//
//
//	//string strTable_0=CreateTableString(1);
//	string strTable_1 = CreateTableString(0);
//	string strTable_2 = CreateTableString(1);
//	string strTable_3 = CreateTableString(2);
//	string strTable_4 = CreateTableString(3);
//
//	string strWJSVector;
//
//	//sprintf(t_ch, "namespace yy.wjsproto.%s\n", strWriteNameSpace.c_str());
//	//strWJSVector.append(t_ch);
//	//strWJSVector.append("{\n");
//
//	vector<string> tt_PacketNameList;
//	tt_PacketNameList.push_back("import {InStream} from \"../../stream/InStream\"");
//	tt_PacketNameList.push_back("import {OutStream} from \"../../stream/OutStream\"");
//	tt_PacketNameList.push_back("import {Int64} from \"../../base/Int64\"");
//	GetUsePacketNamesByParType_TScript(pWJSVector._name, tt_PacketNameList, GetSubFolderName(foldername));
//	string tt_strSelf;
//	sprintf(t_ch, "import {%s} from  \"./%s\"", pWJSVector._name.c_str(), pWJSVector._name.c_str());
//	tt_strSelf = t_ch;
//	RemovePacketNames(tt_PacketNameList, tt_strSelf);
//
//	for (int i = 0; i < tt_PacketNameList.size(); i++)
//	{
//		strWJSVector.append(strTable_1);
//		strWJSVector.append(tt_PacketNameList[i].c_str());
//		strWJSVector.append("\n");
//	}
//	if (tt_PacketNameList.size() > 0)
//		strWJSVector.append("\n");
//
//
//	/////////////有两种版本，一种是非Flash基础类型，一种是
//	bool  tt_bTScritpBaseType = IsTScriptBaseType2(pWJSVector._ParTName);
//
//
//	strWJSVector.append(strTable_1);
//	sprintf(t_ch, "export  class %s extends Array<%s>\n", pWJSVector._name.c_str(), TypeConvertToTScriptType(pWJSVector._ParTName).c_str());
//	strWJSVector.append(t_ch);
//	strWJSVector.append(strTable_1);
//	strWJSVector.append("{\n");
//
//	strWJSVector.append(strTable_2);
//	sprintf(t_ch, "constructor()\n");
//	strWJSVector.append(t_ch);
//
//	strWJSVector.append(strTable_2);
//	strWJSVector.append("{\n");
//
//	//strVector.append(strTable_3);
//	//strVector.append("super();\n");
//
//	strWJSVector.append(strTable_2);
//	strWJSVector.append("}\n");
//
//
//	strWJSVector.append(strTable_2);
//	sprintf(t_ch, "export function %s read_s(outs:OutStream)\n", pWJSVector._name.c_str());
//	strWJSVector.append(t_ch);
//
//	strWJSVector.append(strTable_2);
//	strWJSVector.append("{\n");
//
//	strWJSVector.append(strTable_3);
//	sprintf(t_ch, "%s o = new %s();\n", pWJSVector._name.c_str(), pWJSVector._name.c_str());
//	strWJSVector.append(t_ch);
//
//	strWJSVector.append(strTable_3);
//	strWJSVector.append("count:number = outs.readInt32();\n");
//
//	strWJSVector.append(strTable_3);
//	strWJSVector.append("for(let i=0;i<count;i++)\n");
//
//	strWJSVector.append(strTable_3);
//	strWJSVector.append("{\n");
//
//	if (!tt_bTScritpBaseType)
//	{
//		strWJSVector.append(strTable_4);
//		sprintf(t_ch, "o.push(%s.read_s(outs));\n", ExTypeNameConvert_TScript(pWJSVector._ParTName).c_str());
//		strWJSVector.append(t_ch);
//	}
//	else
//	{
//		strWJSVector.append(strTable_4);
//		sprintf(t_ch, "o.push(outs.%s());\n", TypeConvertToReadTScriptType(pWJSVector._ParTName).c_str());
//		strWJSVector.append(t_ch);
//	}
//
//	strWJSVector.append(strTable_3);
//	strWJSVector.append("}\n");
//
//	strWJSVector.append(strTable_3);
//	strWJSVector.append("return o;\n");
//
//	strWJSVector.append(strTable_2);
//	strWJSVector.append("}\n");
//
//	/////////////////////
//	//
//	////////////////////
//	strWJSVector.append(strTable_2);
//	sprintf(t_ch, "export function read(outs:OutStream)\n", pWJSVector._name.c_str());
//	strWJSVector.append(t_ch);
//
//	strWJSVector.append(strTable_2);
//	strWJSVector.append("{\n");
//
//	strWJSVector.append(strTable_3);
//	strWJSVector.append("this.length=0;\n");
//
//	strWJSVector.append(strTable_3);
//	strWJSVector.append("count:number = outs.readInt32();\n");
//
//
//
//	strWJSVector.append(strTable_3);
//	strWJSVector.append("for(let i=0;i<count;i++)\n");
//
//	strWJSVector.append(strTable_3);
//	strWJSVector.append("{\n");
//
//	if (!tt_bTScritpBaseType)
//	{
//		strWJSVector.append(strTable_4);
//		sprintf(t_ch, "this.push(%s.read_s(outs));\n", ExTypeNameConvert_TScript(pWJSVector._ParTName).c_str());
//		strWJSVector.append(t_ch);
//	}
//	else
//	{
//		strWJSVector.append(strTable_4);
//		sprintf(t_ch, "this.push(outStream.%s());\n", TypeConvertToReadTScriptType(pWJSVector._ParTName).c_str());
//		strWJSVector.append(t_ch);
//	}
//
//	strWJSVector.append(strTable_3);
//	strWJSVector.append("}\n");
//
//
//	strWJSVector.append(strTable_2);
//	strWJSVector.append("}\n");
//
//	////////////////
//	//
//	/////////////////
//
//	strWJSVector.append(strTable_2);
//	strWJSVector.append("export function write(ins:InStream)\n");
//
//	strWJSVector.append(strTable_2);
//	strWJSVector.append("{\n");
//
//	strWJSVector.append(strTable_3);
//	strWJSVector.append("ins.writeInt32(this.length);\n");
//
//	strWJSVector.append(strTable_3);
//	strWJSVector.append("for(let i=0;i<Count;i++)\n");
//
//	strWJSVector.append(strTable_3);
//	strWJSVector.append("{\n");
//	//
//	if (!tt_bTScritpBaseType)
//	{
//		strWJSVector.append(strTable_4);
//		sprintf(t_ch, "this[i].write(ins);\n", ExTypeNameConvert_TScript(pWJSVector._ParTName).c_str());
//		strWJSVector.append(t_ch);
//	}
//	else
//	{
//		strWJSVector.append(strTable_4);
//		sprintf(t_ch, "ins.%s(this[i]);\n", TypeConvertToWriteTScriptType(pWJSVector._ParTName).c_str());
//		strWJSVector.append(t_ch);
//	}
//
//
//	strWJSVector.append(strTable_3);
//	strWJSVector.append("}\n");
//
//	strWJSVector.append(strTable_2);
//	strWJSVector.append("}\n");
//
//	strWJSVector.append(strTable_1);
//	strWJSVector.append("}\n");
//
//	//strWJSVector.append("}\n");
//
//
//
//	fwrite(strWJSVector.c_str(), strWJSVector.length(), 1, fp);
//
//	fclose(fp);
//}
//void    ProtocolParse_Writer_TypeScript::WriteWJSVector2_TScript(stNamespace& pNamespace, stWJSVector& pWJSVector, string foldername, const string& strWriteNameSpace)
//{
//	if (!pWJSVector._bBiaoZhuUsed)
//		return;
//	char t_ch[512];
//	sprintf(t_ch, "%s/%s.ts", foldername.c_str(), pWJSVector._name.c_str());
//
//	//先创建文件
//	FILE* fp = fopen(t_ch, "w");
//	if (!fp)
//		return;
//
//	//写信息头,共用
//	WriteInfoHead(fp);
//
//
//	//string strTable_0=CreateTableString(1);
//	string strTable_1 = CreateTableString(0);
//	string strTable_2 = CreateTableString(1);
//	string strTable_3 = CreateTableString(2);
//	string strTable_4 = CreateTableString(3);
//
//	string strVector;
//
//
//
//	vector<string> tt_PacketNameList;
//	tt_PacketNameList.push_back("import {InStream} from \"../../stream/InStream\"");
//	tt_PacketNameList.push_back("import {OutStream} from \"../../stream/OutStream\"");
//
//
//
//	for (int i = 0; i < tt_PacketNameList.size(); i++)
//	{
//		strVector.append(strTable_1);
//		strVector.append(tt_PacketNameList[i].c_str());
//		strVector.append("\n");
//	}
//	if (tt_PacketNameList.size() > 0)
//		strVector.append("\n");
//
//
//
//	strVector.append(strTable_1);
//	sprintf(t_ch, "export  class %s\n", pWJSVector._name.c_str());
//	strVector.append(t_ch);
//	strVector.append(strTable_1);
//	strVector.append("{\n");
//
//	strVector.append(strTable_2);
//	strVector.append("public _buffer:Uint8Array;\n");
//
//	strVector.append(strTable_2);
//	sprintf(t_ch, "constructor()\n", pWJSVector._name.c_str());
//	strVector.append(t_ch);
//
//	strVector.append(strTable_2);
//	strVector.append("{\n");
//
//	strVector.append(strTable_2);
//	strVector.append("}\n");
//
//
//	strVector.append(strTable_2);
//	sprintf(t_ch, "static  read_s(outs:OutStream):%s\n", pWJSVector._name.c_str());
//	strVector.append(t_ch);
//
//	strVector.append(strTable_2);
//	strVector.append("{\n");
//
//	strVector.append(strTable_3);
//	sprintf(t_ch, "let o:%s = new %s();\n", pWJSVector._name.c_str(), pWJSVector._name.c_str());
//	strVector.append(t_ch);
//
//	strVector.append(strTable_3);
//	strVector.append("o.read(outs);\n");
//
//	strVector.append(strTable_3);
//	strVector.append("return o;\n");
//
//	strVector.append(strTable_2);
//	strVector.append("}\n");
//
//	////////////////
//	//
//	////////////////
//
//	strVector.append(strTable_2);
//	sprintf(t_ch, "public  read(outs:OutStream)\n");
//	strVector.append(t_ch);
//
//	strVector.append(strTable_2);
//	strVector.append("{\n");
//
//	strVector.append(strTable_3);
//	strVector.append("this._buffer=outs.readUint8Array(outs.readInt32());\n");
//
//	strVector.append(strTable_2);
//	strVector.append("}\n");
//
//	/////////////
//	//
//	////////////
//	strVector.append(strTable_2);
//	strVector.append("public  write(ins:InStream)\n");
//
//	strVector.append(strTable_2);
//	strVector.append("{\n");
//
//	strVector.append(strTable_3);
//	strVector.append("ins.writeUint8Array(this._buffer);\n");
//
//	strVector.append(strTable_2);
//	strVector.append("}\n");
//
//	strVector.append(strTable_1);
//	strVector.append("}\n");
//
//	//strVector.append("}\n");
//
//
//
//	fwrite(strVector.c_str(), strVector.length(), 1, fp);
//
//	fclose(fp);
//}

//void    ProtocolParse_Writer_TypeScript::WriteClass_IReceiver_TScript(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, bool bemptyimplement, const string& strWriteNameSpace)
//{
//	if (m_bTScriptUseNameSpaceVersion)
//	{
//		WriteClass_IReceiver_TScript_NSV(pClass,filetype,pNamespace,foldername,bemptyimplement,strWriteNameSpace);
//		return;
//	}
//	char t_ch[1024];
//	sprintf(t_ch, "%s/IReceiver_%s.ts", foldername.c_str(), pNamespace._protocolname.c_str());
//
//	//先创建文件
//	FILE* fp = fopen(t_ch, "w");
//	if (!fp)
//		return;
//
//	//写信息头,共用
//	WriteInfoHead(fp);
//
//	string strclass;
//
//	string strTable_1 = CreateTableString(0);
//	string strTable_2 = CreateTableString(1);
//	string strTable_3 = CreateTableString(2);
//
//
//	//sprintf(t_ch, "namespace yy.wjsproto.%s\n", strWriteNameSpace.c_str());
//	//strclass.append(t_ch);
//	//strclass.append("{\n");
//
//	vector<string> tt_PacketNameList;
//	tt_PacketNameList.push_back("import {Int64} from \"../../base/Int64\"");
//	for (int i = 0; i < pClass._FuncVec.size(); i++)
//	{
//		if (pClass._FuncVec[i]._bDiscard)
//			continue;
//
//		for (int j = 0; j < pClass._FuncVec[i]._InParVec.size(); j++)
//		{
//			if (!IsTScriptBaseType(pClass._FuncVec[i]._InParVec[j]._strParType, false, true))
//			{
//				GetUsePacketNamesByParType_TScript(pClass._FuncVec[i]._InParVec[j]._strParType, tt_PacketNameList, GetSubFolderName(foldername));
//			}
//		}
//	}
//
//	for (int i = 0; i < tt_PacketNameList.size(); i++)
//	{
//		strclass.append(strTable_1);
//		strclass.append(tt_PacketNameList[i].c_str());
//		strclass.append("\n");
//	}
//
//	if (tt_PacketNameList.size() > 0)
//		strclass.append("\n");
//
//
//	strclass.append(strTable_1);
//	sprintf(t_ch, "export interface IReceiver_%s\n", pNamespace._protocolname.c_str());
//	strclass.append(t_ch);
//
//	strclass.append(strTable_1);
//	strclass.append("{\n");
//	fwrite(strclass.c_str(), strclass.length(), 1, fp);
//
//
//	for (int i = 0; i < pClass._FuncVec.size(); i++)
//	{
//		if (pClass._FuncVec[i]._bDiscard)
//			continue;
//		//写函数
//		WriteFunc_IReceiver_TScript_NSV(pClass._FuncVec[i], filetype, 1, pClass, pNamespace, foldername, fp, bemptyimplement);
//	}
//
//	strclass = "";
//	strclass.append(strTable_1);
//	strclass.append("}\n");
//
//	//strclass.append("}\n");
//
//	fwrite(strclass.c_str(), strclass.length(), 1, fp);
//	fclose(fp);
//
//}

//void    ProtocolParse_Writer_TypeScript::WriteClass_Receiver_TScript(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace)
//{
//	if (m_bTScriptUseNameSpaceVersion)
//	{
//		WriteClass_Receiver_TScript_NSV(pClass,filetype,pNamespace,foldername,strWriteNameSpace);
//		return;
//	}
//	char t_ch[1024];
//	sprintf(t_ch, "%s/%s_Callback.ts", foldername.c_str(), pNamespace._protocolname.c_str());
//
//	//先创建文件
//	FILE* fp = fopen(t_ch, "w");
//	if (!fp)
//		return;
//
//	//写信息头,共用
//	WriteInfoHead(fp);
//
//	string strclass;
//
//	string strTable_0 = CreateTableString(0);
//	string strTable_1 = CreateTableString(1);
//	string strTable_2 = CreateTableString(2);
//	string strTable_3 = CreateTableString(3);
//
//
//
//
//
//	//sprintf(t_ch, "namespace yy.wjsproto.%s\n", strWriteNameSpace.c_str());
//	//strclass.append(t_ch);
//	//strclass.append("{\n");
//
//	vector<string> tt_PacketNameList;
//	tt_PacketNameList.push_back("import {InStream} from \"../../stream/InStream\"");
//	tt_PacketNameList.push_back("import {OutStream} from \"../../stream/OutStream\"");
//	tt_PacketNameList.push_back("import {Int64} from \"../../base/Int64\"");
//	sprintf(t_ch, "import {ID_%s} from \"./ID_%s\"", pNamespace._protocolname.c_str(), pNamespace._protocolname.c_str());
//	tt_PacketNameList.push_back(t_ch);
//	sprintf(t_ch, "import {IReceiver_%s} from \"./IReceiver_%s\"", pNamespace._protocolname.c_str(), pNamespace._protocolname.c_str());
//	tt_PacketNameList.push_back(t_ch);
//	for (int i = 0; i < pClass._FuncVec.size(); i++)
//	{
//		if (pClass._FuncVec[i]._bDiscard)
//			continue;
//
//		for (int j = 0; j < pClass._FuncVec[i]._InParVec.size(); j++)
//		{
//			if (!IsTScriptBaseType(pClass._FuncVec[i]._InParVec[j]._strParType, false,true))
//			{
//				GetUsePacketNamesByParType_TScript(pClass._FuncVec[i]._InParVec[j]._strParType, tt_PacketNameList, GetSubFolderName(foldername));
//			}
//		}
//	}
//
//	for (int i = 0; i < tt_PacketNameList.size(); i++)
//	{
//		strclass.append(strTable_0);
//		strclass.append(tt_PacketNameList[i].c_str());
//		strclass.append("\n");
//	}
//
//	if(tt_PacketNameList.size()>0)
//		strclass.append("\n");
//
//	strclass.append(strTable_0);
//	sprintf(t_ch, "export class %s_Callback\n", pNamespace._protocolname.c_str());
//	strclass.append(t_ch);
//
//	strclass.append(strTable_0);
//	strclass.append("{\n");
//	fwrite(strclass.c_str(), strclass.length(), 1, fp);
//
//	WriteFunc_ReceiverParser_TScript(pClass, filetype, 1, pNamespace, foldername, fp);
//
//
//	for (int i = 0; i < pClass._FuncVec.size(); i++)
//	{
//		if (pClass._FuncVec[i]._bDiscard)
//			continue;
//		WriteFunc_Receiver_TScript(pClass._FuncVec[i], filetype, 1, pClass, pNamespace, foldername, fp);
//	}
//
//
//	strclass = "";
//	strclass.append(strTable_0);
//	strclass.append("}\n");
//	//strclass.append("}\n");
//
//	fwrite(strclass.c_str(), strclass.length(), 1, fp);
//	fclose(fp);
//}
//void    ProtocolParse_Writer_TypeScript::WriteFunc_ReceiverParser_TScript(stClass& pClass, int filetype, int tablenum, stNamespace& pNamespace, string foldername, FILE* fp)
//{
//	char t_ch[1024];
//
//	string strTable_0 = CreateTableString(tablenum);
//	string strTable_1 = CreateTableString(tablenum + 1);
//	string strTable_2 = CreateTableString(tablenum + 2);
//	string strTable_3 = CreateTableString(tablenum + 3);
//	string strTable_4 = CreateTableString(tablenum + 4);
//
//
//	string strFunc;
//	strFunc.append(strTable_0);
//
//	if (filetype == 0)
//	{
//		sprintf(t_ch, "Parser(cls:IReceiver_%s,outs:OutStream,pExData):boolean\n", pNamespace._protocolname.c_str());
//		strFunc.append(t_ch);
//	}
//	else
//	{
//		sprintf(t_ch, "Parser(cls:IReceiver_%s,outs:OutStream,sessionId:number,pExData):boolean\n", pNamespace._protocolname.c_str());
//		strFunc.append(t_ch);
//	}
//
//	strFunc.append(strTable_0);
//	strFunc.append("{\n");
//
//	strFunc.append(strTable_1);
//	strFunc.append("let no:number = outs.readInt32();\n");
//
//	strFunc.append(strTable_1);
//	sprintf(t_ch, "if(no!=ID_%s.%s)\n", pNamespace._protocolname.c_str(),m_ProtocolNo._name.c_str());
//	strFunc.append(t_ch);
//
//	strFunc.append(strTable_1);
//	strFunc.append("{\n");
//
//	strFunc.append(strTable_2);
//	strFunc.append("outs.addOffset(- 4);\n");
//
//	strFunc.append(strTable_2);
//	strFunc.append("return false;\n");
//
//	strFunc.append(strTable_1);
//	strFunc.append("}\n");
//
//	strFunc.append(strTable_1);
//	strFunc.append("let proversionno:number = outs.readInt16();\n");
//
//
//	strFunc.append(strTable_1);
//	strFunc.append("let methodid:number =outs.readInt32();\n");
//
//	strFunc.append(strTable_1);
//	strFunc.append("switch(methodid)\n");
//
//	strFunc.append(strTable_1);
//	strFunc.append("{\n");
//
//	for (int i = 0; i < pClass._FuncVec.size(); i++)
//	{
//		if (pClass._FuncVec[i]._bDiscard)
//			continue;
//		strFunc.append(strTable_2);
//		sprintf(t_ch, "case ID_%s.ID_%s_%s:\n", pNamespace._protocolname.c_str(),pClass._name.c_str(), pClass._FuncVec[i]._name.c_str());
//		strFunc.append(t_ch);
//
//		strFunc.append(strTable_3);
//		if (filetype == 0)
//		{
//			sprintf(t_ch, "this._parser_%s(cls,outs,pExData,proversionno);\n", pClass._FuncVec[i]._name.c_str());
//			strFunc.append(t_ch);
//		}
//		else
//		{
//			sprintf(t_ch, "this._parser_%s(cls,outs,sessionId,pExData,proversionno);\n", pClass._FuncVec[i]._name.c_str());
//			strFunc.append(t_ch);
//		}
//
//
//		strFunc.append(strTable_3);
//		strFunc.append("return true;\n");
//	}
//
//
//	strFunc.append(strTable_2);
//	strFunc.append("default:\n");
//
//	strFunc.append(strTable_3);
//	strFunc.append("return false;\n");
//
//	strFunc.append(strTable_1);
//	strFunc.append("}\n");
//
//	strFunc.append(strTable_0);
//	strFunc.append("}\n");
//
//
//	fwrite(strFunc.c_str(), strFunc.length(), 1, fp);
//}

//void    ProtocolParse_Writer_TypeScript::WriteClass_Send_TScript(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace)
//{
//	if (m_bTScriptUseNameSpaceVersion)
//	{
//		WriteClass_Send_TScript_NSV(pClass,filetype,pNamespace,foldername,strWriteNameSpace);
//		return;
//	}
//	char t_ch[512];
//	sprintf(t_ch, "%s/%s_Send.ts", foldername.c_str(), pNamespace._protocolname.c_str());
//
//	//先创建文件
//	FILE* fp = fopen(t_ch, "w");
//	if (!fp)
//		return;
//
//	//写信息头,共用
//	WriteInfoHead(fp);
//
//	string strclass;
//
//	string strTable_1 = CreateTableString(0);
//	string strTable_2 = CreateTableString(1);
//	string strTable_3 = CreateTableString(2);
//
//
//	//sprintf(t_ch, "namespace yy.wjsproto.%s\n", strWriteNameSpace.c_str());
//	//strclass.append(t_ch);
//	//strclass.append("{\n");
//
//	vector<string> tt_PacketNameList;
//	tt_PacketNameList.push_back("import {InStream} from \"../../stream/InStream\"");
//	tt_PacketNameList.push_back("import {OutStream} from \"../../stream/OutStream\"");
//	tt_PacketNameList.push_back("import {Int64} from \"../../base/Int64\"");
//	sprintf(t_ch, "import {ID_%s} from \"./ID_%s\"", pNamespace._protocolname.c_str(), pNamespace._protocolname.c_str());
//	tt_PacketNameList.push_back(t_ch);
//	for (int i = 0; i < pClass._FuncVec.size(); i++)
//	{
//		if (pClass._FuncVec[i]._bDiscard)
//			continue;
//
//		for (int j = 0; j < pClass._FuncVec[i]._InParVec.size(); j++)
//		{
//			if (!IsTScriptBaseType(pClass._FuncVec[i]._InParVec[j]._strParType, false))
//			{
//				GetUsePacketNamesByParType_TScript(pClass._FuncVec[i]._InParVec[j]._strParType, tt_PacketNameList, GetSubFolderName(foldername));
//			}
//		}
//	}
//	for (int i = 0; i < tt_PacketNameList.size(); i++)
//	{
//		strclass.append(strTable_1);
//		strclass.append(tt_PacketNameList[i].c_str());
//		strclass.append("\n");
//	}
//	if (tt_PacketNameList.size() > 0)
//		strclass.append("\n");
//
//
//	strclass.append(strTable_1);
//	sprintf(t_ch, "export class %s_Send\n", pNamespace._protocolname.c_str());
//	strclass.append(t_ch);
//
//	strclass.append(strTable_1);
//	strclass.append("{\n");
//
//
//	fwrite(strclass.c_str(), strclass.length(), 1, fp);
//	for (int i = 0; i < pClass._FuncVec.size(); i++)
//	{
//		if (pClass._FuncVec[i]._bDiscard)
//			continue;
//		//写函数
//		WriteFunc_SendByteSeq_TScript(pClass._FuncVec[i], 1, pClass, pNamespace, foldername, fp);
//	}
//
//
//	strclass = "";
//	strclass.append(strTable_1);
//	strclass.append("}\n");
//
//	//strclass.append("}\n");
//
//	fwrite(strclass.c_str(), strclass.length(), 1, fp);
//
//	fclose(fp);
//
//}
//void    ProtocolParse_Writer_TypeScript::WriteFunc_SendByteSeq_TScript(stFunc& pFunc, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp)
//{
//	if (m_bTScriptUseNameSpaceVersion)
//	{
//		WriteFunc_SendByteSeq_TScript_NSV(pFunc, tablenum, pClass, pNamespace, foldername, fp);
//		return;
//	}
//	char t_ch[1024];
//
//
//	string strTable_0 = CreateTableString(tablenum);
//	string strTable_1 = CreateTableString(tablenum + 1);
//	string strTable_2 = CreateTableString(tablenum + 2);
//
//	string strfunc;
//
//	strfunc.append(strTable_0);
//	strfunc.append("/**\n");
//
//	strfunc.append(WriteComment_TScript(strTable_0, pFunc._comment, "", false).c_str());
//	for (int i = 0; i < pFunc._InParVec.size(); i++)
//	{
//		strfunc.append(WriteComment_TScript(strTable_0, pFunc._InParVec[i]._comment, pFunc._InParVec[i]._strParName, false).c_str());
//	}
//
//	strfunc.append(strTable_0);
//	strfunc.append("*/\n");
//
//	strfunc.append(strTable_0);
//	sprintf(t_ch, "static  Send_%s(", pFunc._name.c_str());
//	strfunc.append(t_ch);
//
//	//有参
//	if (pFunc._InParVec.size() > 0)
//	{
//		for (int i = 0; i < pFunc._InParVec.size(); i++)
//		{
//			//if (i == pFunc._InParVec.size() - 1)
//			//{
//			//	sprintf(t_ch, "%s:%s", pFunc._InParVec[i]._strParName.c_str(), TypeConvertToTScriptType(pFunc._InParVec[i]._strParType).c_str());
//			//	strfunc.append(t_ch);
//			//}
//			//else
//			//{
//				sprintf(t_ch, "%s:%s,", pFunc._InParVec[i]._strParName.c_str(), TypeConvertToTScriptType(pFunc._InParVec[i]._strParType).c_str());
//				strfunc.append(t_ch);
//			//}
//
//		}
//
//	}
//	strfunc.append("ins:InStream");
//
//	strfunc.append(")\n");
//	strfunc.append(strTable_0);
//	strfunc.append("{\n");
//
//	strfunc.append(strTable_1);
//	strfunc.append("ins.reset();\n");
//
//	//先写协议号
//	strfunc.append(strTable_1);
//	sprintf(t_ch, "ins.writeInt32(ID_%s.%s);\n", pNamespace._protocolname.c_str(), m_ProtocolNo._name.c_str());
//	strfunc.append(t_ch);
//
//	//写版本号
//
//	strfunc.append(strTable_1);
//	sprintf(t_ch, "let tt_version:number = %d;\n", pFunc._version);
//	strfunc.append(t_ch);
//	strfunc.append(strTable_1);
//	strfunc.append("ins.writeInt16(tt_version);\n");
//
//	//再写函数号
//	strfunc.append(strTable_1);
//	sprintf(t_ch, "ins.writeInt32(ID_%s.ID_%s_%s);\n", pNamespace._protocolname.c_str(),pClass._name.c_str(), pFunc._name.c_str());
//	strfunc.append(t_ch);
//
//	//
//	for (int i = 0; i < pFunc._InParVec.size(); i++)
//	{
//		strfunc.append(strTable_1);
//		if (IsTScriptBaseType(pFunc._InParVec[i]._strParType, true))
//		{
//			//if (IsExType_enum(pFunc._InParVec[i]._strParType))
//			//{
//			//	sprintf(t_ch, "%s.write(inStream,%s);\n", pFunc._InParVec[i]._strParType.c_str(), pFunc._InParVec[i]._strParName.c_str());
//			//}
//			//else
//				sprintf(t_ch, "ins.%s(%s);\n", TypeConvertToWriteTScriptType(pFunc._InParVec[i]._strParType.c_str()).c_str(), pFunc._InParVec[i]._strParName.c_str());
//
//		}
//		else
//		{
//			sprintf(t_ch, "%s.write(ins);\n", pFunc._InParVec[i]._strParName.c_str());
//
//		}
//		strfunc.append(t_ch);
//	}
//
//	strfunc.append(strTable_0);
//	strfunc.append("}\n");
//
//	fwrite(strfunc.c_str(), strfunc.length(), 1, fp);
//}
/////////////
//
///////////

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