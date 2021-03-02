#include "ProtocolParse_Writer_Lua.h"
#include "ProtocolParse.h"
#include <time.h>
#include <sys/timeb.h>

extern string g_strBaseType[];

ProtocolParse_Writer_Lua::ProtocolParse_Writer_Lua(const string& strPathName)
	:ProtocolParse_Writer(strPathName, eProtocolWriterType::Lua)
	,m_bLuaRequireInFunc(true)
{
	m_strLuaPackage = "Net/";
}
ProtocolParse_Writer_Lua::~ProtocolParse_Writer_Lua()
{

}
bool    ProtocolParse_Writer_Lua::CreateFile_Path(string path, int filetype, bool brefprotocol)
{
	return CreateFile_Path_Lua(path,filetype,brefprotocol);
}
ProtocolParse_Writer* ProtocolParse_Writer_Lua::Create_RefProtocolParse_Writer(const string& strPathName)
{
	return new ProtocolParse_Writer_Lua(strPathName);
}
bool    ProtocolParse_Writer_Lua::CreateFile_Path_Lua(string path, int filetype, bool brefprotocol)
{
	if (filetype < 0 || filetype>2)
		return false;

	//按协议名来建文建夹
	if (m_NamespaceVec.size() > 0) {
		if (brefprotocol) {
			char tt_ch[256];
			m_SaveFileName._servername = m_NamespaceVec[0]._protocolname;
			sprintf(tt_ch, "%sProtocol.m", m_NamespaceVec[0]._protocolname.c_str());
			m_SaveFileName._servername2 = tt_ch;

			m_SaveFileName._clientname = m_NamespaceVec[0]._protocolname;
			sprintf(tt_ch, "%sProtocol.m", m_NamespaceVec[0]._protocolname.c_str());
			m_SaveFileName._clientname2 = tt_ch;


		}
		else {
			char tt_ch[256];

			m_SaveFileName._servername = m_NamespaceVec[0]._protocolname;
			sprintf(tt_ch, "%sProtocol_server.m", m_NamespaceVec[0]._protocolname.c_str());
			m_SaveFileName._servername2 = tt_ch;

			m_SaveFileName._clientname = m_NamespaceVec[0]._protocolname;
			sprintf(tt_ch, "%sProtocol_client.m", m_NamespaceVec[0]._protocolname.c_str());
			m_SaveFileName._clientname2 = tt_ch;
		}

	}


	switch (filetype) {
	case 1://服务器
	{
		for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++) {
			//主要为了使得id不出现重复
			if (m_pRefProtocolParseList[i]) {
				m_pRefProtocolParseList[i]->CreateFile_Path(path, filetype, true);
				m_pRefProtocolParseList[i]->GetRefProtocolMapPar(m_StringIntLogMap, m_IntStringMap, m_StringIntSearchMap);
			}
		}


		string str1 = path;
		string tt_strAddPath = m_SaveFileName._servername;
		//ToLower(tt_strAddPath);
		str1.append(tt_strAddPath);
		//string str2=path;
		//str2.append(m_SaveFileName._servername2);
		return CreateFile_Lua(str1, filetype, brefprotocol);
	}
	break;
	case 0://客户端
	{
		for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++) {
			if (m_pRefProtocolParseList[i]) {
				m_pRefProtocolParseList[i]->CreateFile_Path(path, filetype, true);
				m_pRefProtocolParseList[i]->GetRefProtocolMapPar(m_StringIntLogMap, m_IntStringMap, m_StringIntSearchMap);
			}
		}


		string str1 = path;
		string tt_strAddPath = m_SaveFileName._clientname;
		//ToLower(tt_strAddPath);
		str1.append(tt_strAddPath);
		//string str2=path;
		//str2.append(m_SaveFileName._clientname2);
		return CreateFile_Lua(str1, filetype, brefprotocol);
	}
	break;
	case 2: //合成
	{
		for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++) {
			//主要为了使得id不出现重复
			if (m_pRefProtocolParseList[i]) {
				m_pRefProtocolParseList[i]->CreateFile_Path(path, filetype, true);
				m_pRefProtocolParseList[i]->GetRefProtocolMapPar(m_StringIntLogMap, m_IntStringMap, m_StringIntSearchMap);
			}
		}


		//先按服务器名存
		string str1 = path;
		string tt_strAddPath = m_SaveFileName._servername;
		//ToLower(tt_strAddPath);
		str1.append(tt_strAddPath);
		//string str2=path;
		//str2.append(m_SaveFileName._servername2);
		return CreateFile_Lua(str1, filetype, brefprotocol);
	}
	break;
	default:
		return false;
		break;
	}

}

bool    ProtocolParse_Writer_Lua::CreateFile_Lua(string foldername, int filetype, bool bRefProtocol)
{
	//bool tt_bNotSaveFile=false;
	//先读取文件，判定MD5是否相同，相同则不用再生成
	//if(TestFileMd5(filename))
	//{
	//if(bRefProtocol)
	//{
	//	tt_bNotSaveFile=true;
	//}
	//	else
	//		return true;
	//}

	string tt_fullfoldername = foldername;
	tt_fullfoldername.append("\\");
	//首先创建文件夹
	CheckDirectoryEixst(tt_fullfoldername.c_str());


	//写命名空间
	for (size_t i = 0; i < m_NamespaceVec.size(); i++) {
		WriteOneNameSpace_Lua(m_NamespaceVec[i], filetype, foldername);
	}


	return true;
}

bool    ProtocolParse_Writer_Lua::WriteOneNameSpace_Lua(stNamespace& pNamespace, int filetype, string foldername)
{
	m_tablenum = 0;

	//char ch[256];

	string tt_strWriteNameSpace = "";
	switch (filetype) {
	case 0:
		tt_strWriteNameSpace = pNamespace._clientname.c_str();
		break;
	case 1:
		tt_strWriteNameSpace = pNamespace._servername.c_str();
		break;
	case 2:
		tt_strWriteNameSpace = pNamespace._servername.c_str();
		break;
	default:
		break;
	}

	////写函数的id,和协议号
	CreateAndWriteFuncIdAndProtocolNo_Lua(pNamespace, foldername, tt_strWriteNameSpace);


	//先写枚举
	for (size_t i = 0; i < pNamespace._EnumVec.size(); i++) {
		WriteEnum_Lua(pNamespace, pNamespace._EnumVec[i], foldername, tt_strWriteNameSpace);
	}

	//////////需要考虑顺序,按顺序插着写
	////结构体和vector型大小
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
				WriteStruct_Lua(pNamespace, pNamespace._StructVec[structindex], foldername, tt_strWriteNameSpace);
				structindex++;
			}
			else if (pNamespace._VectorVec[vectorindex]._index < pNamespace._StructVec[structindex]._index
				&& (pNamespace._VectorVec[vectorindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index)) {
				//写vector
				WriteVector_Lua(pNamespace, pNamespace._VectorVec[vectorindex], foldername, tt_strWriteNameSpace);
				vectorindex++;
			}
			else {
				//写类vector
				WriteWJSVector_Lua(pNamespace, pNamespace._WJSVectorVec[wjsvectorindex], foldername, tt_strWriteNameSpace);
				wjsvectorindex++;
			}
		}
		else if (!bvectorend && (!bstructend)) {
			if (pNamespace._StructVec[structindex]._index < pNamespace._VectorVec[vectorindex]._index) {
				//写结构体
				WriteStruct_Lua(pNamespace, pNamespace._StructVec[structindex], foldername, tt_strWriteNameSpace);
				structindex++;
			}
			else {
				//写vector
				WriteVector_Lua(pNamespace, pNamespace._VectorVec[vectorindex], foldername, tt_strWriteNameSpace);
				vectorindex++;
			}
		}
		else if (!bvectorend && (!bwjsvectorend)) {
			if (pNamespace._VectorVec[vectorindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index) {
				//写vector
				WriteVector_Lua(pNamespace, pNamespace._VectorVec[vectorindex], foldername, tt_strWriteNameSpace);
				vectorindex++;
			}
			else {
				//写类vector
				WriteWJSVector_Lua(pNamespace, pNamespace._WJSVectorVec[wjsvectorindex], foldername, tt_strWriteNameSpace);
				wjsvectorindex++;
			}
		}
		else if ((!bstructend) && (!bwjsvectorend)) {
			if (pNamespace._StructVec[structindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index) {
				//写结构体
				WriteStruct_Lua(pNamespace, pNamespace._StructVec[structindex], foldername, tt_strWriteNameSpace);
				structindex++;
			}
			else {
				//写类vector
				WriteWJSVector_Lua(pNamespace, pNamespace._WJSVectorVec[wjsvectorindex], foldername, tt_strWriteNameSpace);
				wjsvectorindex++;
			}
		}
		else if (!bvectorend) {
			//写vector
			WriteVector_Lua(pNamespace, pNamespace._VectorVec[vectorindex], foldername, tt_strWriteNameSpace);
			vectorindex++;

		}
		else if (!bstructend) {
			//写结构体
			WriteStruct_Lua(pNamespace, pNamespace._StructVec[structindex], foldername, tt_strWriteNameSpace);
			structindex++;
		}
		else if (!bwjsvectorend) {
			//写类vector
			WriteWJSVector_Lua(pNamespace, pNamespace._WJSVectorVec[wjsvectorindex], foldername, tt_strWriteNameSpace);
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


	//写类
	for (size_t i = 0; i < pNamespace._ClassVec.size(); i++) {
		WriteClass_Lua(pNamespace._ClassVec[i], filetype, pNamespace, foldername, tt_strWriteNameSpace);
	}

	return true;
}


void    ProtocolParse_Writer_Lua::CreateAndWriteFuncIdAndProtocolNo_Lua(stNamespace& pNamespace, string foldername, const string& strWriteNameSpace)
{
	if (pNamespace._ClassVec.size() == 0)
		return;

	char t_ch[512];
	sprintf(t_ch, "%s/ID_%s.lua", foldername.c_str(), pNamespace._protocolname.c_str());

	//先创建文件
	FILE* fp = fopen(t_ch, "w");
	if (!fp)
		return;

	//写信息头,共用
	WriteInfoHead(fp, true);

	string strid;

	string strTable_0 = CreateTableString(0);
	string strTable_1 = CreateTableString(1);




	strid.append(strTable_0);
	sprintf(t_ch, "local ID_%s = class()\n", pNamespace._protocolname.c_str());
	strid.append(t_ch);

	strid.append("\n");

	//函数id
	for (size_t i = 0; i < pNamespace._ClassVec.size(); i++) {
		for (size_t j = 0; j < pNamespace._ClassVec[i]._FuncVec.size(); j++) {
			strid.append(strTable_1);
			sprintf(t_ch, "ID_%s.ID_%s_%s", pNamespace._protocolname.c_str(), pNamespace._ClassVec[i]._name.c_str(), pNamespace._ClassVec[i]._FuncVec[j]._name.c_str());
			strid.append(t_ch);
			sprintf(t_ch, "ID_%s_%s", pNamespace._ClassVec[i]._name.c_str(), pNamespace._ClassVec[i]._FuncVec[j]._name.c_str());

			int tempId;  //将字符串转成id
			if (pNamespace._ClassVec[i]._FuncVec[j]._userFuncId != 0)
				tempId = pNamespace._ClassVec[i]._FuncVec[j]._userFuncId;
			else
				tempId = CovertStringToInt(t_ch, pNamespace._ClassVec[i]._FuncVec[j]._bNew);
			sprintf(t_ch, "=%d\n", tempId);
			strid.append(t_ch);
		}
	}

	//协议号
	if (m_ProtocolNo._value != 1) {
		//协议头
		strid.append(strTable_1);
		sprintf(t_ch, "ID_%s.%s=%d\n", pNamespace._protocolname.c_str(), m_ProtocolNo._name.c_str(), m_ProtocolNo._value);
		strid.append(t_ch);
	}
	strid.append("\n");
	strid.append(strTable_0);
	sprintf(t_ch, "return ID_%s\n", pNamespace._protocolname.c_str());
	strid.append(t_ch);
	strid.append("\n");
	fwrite(strid.c_str(), strid.length(), 1, fp);

	fclose(fp);
}
void    ProtocolParse_Writer_Lua::WriteEnum_Lua(stNamespace& pNamespace, stEnum& pEnum, string foldername, const string& strWriteNameSpace)
{
	char t_ch[512];
	sprintf(t_ch, "%s/%s.lua", foldername.c_str(), pEnum._name.c_str());

	//先创建文件
	FILE* fp = fopen(t_ch, "w");
	if (!fp)
		return;

	//写信息头,共用
	WriteInfoHead(fp, true);


	string strTable_1 = CreateTableString(0);
	string strTable_2 = CreateTableString(1);
	string strTable_3 = CreateTableString(2);
	string strTable_4 = CreateTableString(3);
	string strTable_5 = CreateTableString(4);
	//string strTable_6=CreateTableString(5);

	string strEnum;



	strEnum.append(WriteComment_Lua(strTable_1, pEnum._comment, "").c_str());

	strEnum.append(strTable_1);
	strEnum.append("local InStream = CS.InOutStream.InStream\n");
	strEnum.append(strTable_1);
	strEnum.append("local OutStream = CS.InOutStream.OutStream\n");
	strEnum.append("\n");

	string tt_strMName;
	sprintf(t_ch, "%s", pEnum._name.c_str());
	tt_strMName = t_ch;


	strEnum.append(strTable_1);
	sprintf(t_ch, "local %s =\n", tt_strMName.c_str());
	strEnum.append(t_ch);
	strEnum.append(strTable_1);
	strEnum.append("{\n");


	for (size_t i = 0; i < pEnum._ParNameList.size(); i++) {
		strEnum.append(WriteComment_Lua(strTable_2, pEnum._commentList[i], "").c_str());
		strEnum.append(strTable_2);
		if (i != pEnum._ParNameList.size() - 1) {
			sprintf(t_ch, "%s = %d,\n", pEnum._ParNameList[i].c_str(), i/*CovertStringToInt(pEnum._ParNameList[i])*/);

		}
		else {
			sprintf(t_ch, "%s = %d\n", pEnum._ParNameList[i].c_str(), i/*CovertStringToInt(pEnum._ParNameList[i])*/);

		}

		strEnum.append(t_ch);
	}

	strEnum.append(strTable_1);
	strEnum.append("}\n");
	strEnum.append("\n");


	//写读
	strEnum.append(strTable_1);
	sprintf(t_ch, "function %s.read(outStream)\n", tt_strMName.c_str());
	strEnum.append(t_ch);

	strEnum.append(strTable_2);
	strEnum.append("local tt_a = outStream:ReadInt()\n");


	for (size_t i = 0; i < pEnum._ParNameList.size(); i++) {
		if (pEnum._bNewVersion && (!pEnum._keepOldList[i])) {
			strEnum.append(strTable_2);
			if (i == 0) {
				strEnum.append("if");
				sprintf(t_ch, " tt_a == %d then\n", i);
				strEnum.append(t_ch);
			}
			else {
				strEnum.append("elseif");
				sprintf(t_ch, " tt_a == %d then\n", i);
				strEnum.append(t_ch);
			}

			strEnum.append(strTable_3);
			sprintf(t_ch, "return %s.%s\n", tt_strMName.c_str(), pEnum._ParNameList[i].c_str());
			strEnum.append(t_ch);
		}
		else {
			strEnum.append(strTable_2);
			if (i == 0) {
				strEnum.append("if");
				sprintf(t_ch, " tt_a == %d then\n", CovertStringToInt(pEnum._ParNameList[i]));
				strEnum.append(t_ch);
			}
			else {
				strEnum.append("elseif");
				sprintf(t_ch, " tt_a == %d then\n", CovertStringToInt(pEnum._ParNameList[i]));
				strEnum.append(t_ch);
			}

			strEnum.append(strTable_3);
			sprintf(t_ch, "return %s.%s\n", tt_strMName.c_str(), pEnum._ParNameList[i].c_str());
			strEnum.append(t_ch);
		}


	}
	if (pEnum._ParNameList.size() > 0) {
		strEnum.append(strTable_2);
		strEnum.append("else\n");
		strEnum.append(strTable_3);
		sprintf(t_ch, "return %s.%s\n", tt_strMName.c_str(), pEnum._ParNameList[0].c_str());
		strEnum.append(t_ch);
	}

	strEnum.append(strTable_2);
	strEnum.append("end\n");

	strEnum.append(strTable_1);
	strEnum.append("end\n");
	strEnum.append("\n");



	strEnum.append(strTable_1);
	sprintf(t_ch, "function %s.write(inStream,va)\n", tt_strMName.c_str());
	strEnum.append(t_ch);

	for (size_t i = 0; i < pEnum._ParNameList.size(); i++) {
		if (pEnum._bNewVersion && (!pEnum._keepOldList[i])) {
			strEnum.append(strTable_2);
			if (i == 0) {
				strEnum.append("if");
				sprintf(t_ch, " va == %s.%s then\n", tt_strMName.c_str(), pEnum._ParNameList[i].c_str());
				strEnum.append(t_ch);
			}
			else {
				strEnum.append("elseif");
				sprintf(t_ch, " va == %s.%s then\n", tt_strMName.c_str(), pEnum._ParNameList[i].c_str());
				strEnum.append(t_ch);
			}

			strEnum.append(strTable_3);
			sprintf(t_ch, "inStream:WriteInt(%d)\n", i);
			strEnum.append(t_ch);
		}
		else {
			strEnum.append(strTable_2);
			if (i == 0) {
				strEnum.append("if");
				sprintf(t_ch, " va == %s.%s then\n", tt_strMName.c_str(), pEnum._ParNameList[i].c_str());
				strEnum.append(t_ch);
			}
			else {
				strEnum.append("elseif");
				sprintf(t_ch, " va == %s.%s then\n", tt_strMName.c_str(), pEnum._ParNameList[i].c_str());
				strEnum.append(t_ch);
			}

			strEnum.append(strTable_3);
			sprintf(t_ch, "inStream:WriteInt(%d)\n", GetExistStringToIntValue(pEnum._ParNameList[i]));
			strEnum.append(t_ch);
		}

	}

	if (pEnum._bNewVersion) {

		strEnum.append(strTable_2);
		strEnum.append("else\n");
		strEnum.append(strTable_3);
		strEnum.append("inStream:WriteInt(0)\n");
	}
	else {
		if (pEnum._ParNameList.size() > 0) {
			strEnum.append(strTable_2);
			strEnum.append("else\n");
			strEnum.append(strTable_3);
			sprintf(t_ch, "inStream:WriteInt(%d)\n", GetExistStringToIntValue(pEnum._ParNameList[0]));
			strEnum.append(t_ch);
		}
	}


	strEnum.append(strTable_2);
	strEnum.append("end\n");

	strEnum.append(strTable_1);
	strEnum.append("end\n");

	strEnum.append("\n");
	strEnum.append(strTable_1);
	sprintf(t_ch, "return %s\n", tt_strMName.c_str());
	strEnum.append(t_ch);

	fwrite(strEnum.c_str(), strEnum.length(), 1, fp);

	fclose(fp);
}


void    ProtocolParse_Writer_Lua::WriteStruct_Lua(stNamespace& pNamespace, stStructStruct& pStruct, string foldername, const string& strWriteNameSpace)
{
	if (pStruct._inherited.length() > 0)
		return;

	char t_ch[512];
	sprintf(t_ch, "%s/%s.lua", foldername.c_str(), pStruct._name.c_str());

	//先创建文件
	FILE* fp = fopen(t_ch, "w");
	if (!fp)
		return;

	//写信息头,共用
	WriteInfoHead(fp, true);


	string strTable_1 = CreateTableString(0);
	string strTable_2 = CreateTableString(1);
	string strTable_3 = CreateTableString(2);
	string strTable_4 = CreateTableString(3);
	string strTable_5 = CreateTableString(4);
	string strTable_6 = CreateTableString(5);

	string strStruct;


	strStruct.append(WriteComment_Lua(strTable_1, pStruct._comment, "").c_str());

	strStruct.append(strTable_1);
	strStruct.append("local InStream = CS.InOutStream.InStream\n");
	strStruct.append(strTable_1);
	strStruct.append("local OutStream = CS.InOutStream.OutStream\n");
	strStruct.append("\n");

	if (pStruct._name.compare("PhonePlayerInfo20") == 0) {
		int a;
		a = 10;
	}


	string tt_strMName;
	sprintf(t_ch, "%s", pStruct._name.c_str());
	tt_strMName = t_ch;

	vector<string> tt_PacketNameList;
	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		if (!IsLuaBaseType(pStruct._ParVec[i]._strParType, false)) {
			GetUsePacketNamesByParType_Lua(pStruct._ParVec[i]._strParType, tt_PacketNameList);
		}
	}


	string tt_strSelf;
	sprintf(t_ch, "local %s = require \"%s%s/%s\"", pStruct._name.c_str(),
		m_strLuaPackage.c_str(), pNamespace._protocolname.c_str(), pStruct._name.c_str());
	tt_strSelf = t_ch;
	RemovePacketNames(tt_PacketNameList, tt_strSelf);

	for (size_t i = 0; i < tt_PacketNameList.size(); i++) {
		strStruct.append(strTable_1);
		strStruct.append(tt_PacketNameList[i].c_str());
		strStruct.append("\n");
	}

	if (tt_PacketNameList.size() > 0) {
		strStruct.append("\n");
	}

	strStruct.append(strTable_1);
	sprintf(t_ch, "local %s = class()\n", tt_strMName.c_str());
	strStruct.append(t_ch);
	strStruct.append("\n");

	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		strStruct.append(WriteComment_Lua(strTable_1, pStruct._ParVec[i]._comment, "").c_str());
		strStruct.append(strTable_1);
		strStruct.append("--");
		strStruct.append(pStruct._ParVec[i]._strParName.c_str());
		strStruct.append("\n");
	}

	strStruct.append("\n");
	//写读
	strStruct.append(strTable_1);
	sprintf(t_ch, "function %s.read(outStream)\n", tt_strMName.c_str());
	strStruct.append(t_ch);


	strStruct.append(strTable_2);
	sprintf(t_ch, "local o = %s.New()\n", tt_strMName.c_str());
	strStruct.append(t_ch);

	strStruct.append(strTable_2);
	strStruct.append("o:readLocal(outStream)\n");


	strStruct.append(strTable_2);
	strStruct.append("return o\n");

	strStruct.append(strTable_1);
	strStruct.append("end\n");

	strStruct.append("\n");


	////////////////
	//
	///////////////
	//继续写读
	strStruct.append(strTable_1);
	sprintf(t_ch, "function %s:readLocal(outStream)\n", tt_strMName.c_str());
	strStruct.append(t_ch);

	strStruct.append(strTable_2);
	strStruct.append("local tt_len =0\n");

	strStruct.append(strTable_2);
	strStruct.append("tt_len = outStream:ReadShort()\n");

	strStruct.append(strTable_2);
	strStruct.append("local tt_version=0\n");

	strStruct.append(strTable_2);
	strStruct.append("tt_version = outStream:ReadShort()\n");

	strStruct.append(strTable_2);
	strStruct.append("local tt_logsize1 = outStream:GetPos()\n");

	if (pStruct.IsHasTransNeedValuePar()) {
		strStruct.append(strTable_2);
		strStruct.append("local tt_transNeedValue=0\n");

		strStruct.append(strTable_2);
		strStruct.append("tt_transNeedValue = outStream:ReadInt()\n");
	}

	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		if (pStruct._ParVec[i]._version > 1)
			break;
		if (pStruct.IsHasTransNeedValuePar() && (pStruct._ParVec[i]._transNeedValue != -1)) {
			strStruct.append(strTable_2);
			sprintf(t_ch, "if (tt_transNeedValue & (1<<%d))~=0 then ", pStruct._ParVec[i]._transNeedValue);
			strStruct.append(t_ch);

			//strStruct.append(strTable_4);
			if (IsLuaBaseType(pStruct._ParVec[i]._strParType, false)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "self.%s = %s.read(outStream)", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				else {
					sprintf(t_ch, "self.%s = outStream:%s()", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadLuaType(pStruct._ParVec[i]._strParType.c_str()).c_str());
				}
			}
			else {
				sprintf(t_ch, "self.%s = %s.read(outStream)", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
			}
			strStruct.append(t_ch);

			//strStruct.append(strTable_3);
			strStruct.append(" else ");
			//strStruct.append(strTable_4);
			sprintf(t_ch, "self.%s=%s ", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strDefaultValue.c_str());
			strStruct.append(t_ch);
			//strStruct.append(strTable_3);
			strStruct.append(" end\n");

		}
		else {
			strStruct.append(strTable_2);
			if (IsLuaBaseType(pStruct._ParVec[i]._strParType, false)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "self.%s = %s.read(outStream)\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				else {
					sprintf(t_ch, "self.%s = outStream:%s()\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadLuaType(pStruct._ParVec[i]._strParType.c_str()).c_str());
				}
			}
			else {
				sprintf(t_ch, "self.%s = %s.read(outStream)\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
			}
			strStruct.append(t_ch);
		}

	}


	string ttt_strStruct;
	bool tt_bHasReadData = false;

	if (pStruct._version > 1) {
		ttt_strStruct.append(strTable_2);
		sprintf(t_ch, "if tt_version == %d or tt_version==0 then\n", pStruct._version);
		ttt_strStruct.append(t_ch);

		for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
			if (pStruct._ParVec[i]._version <= 1)
				continue;
			tt_bHasReadData = true;
			if (pStruct.IsHasTransNeedValuePar() && (pStruct._ParVec[i]._transNeedValue != -1)) {
				ttt_strStruct.append(strTable_3);
				sprintf(t_ch, "if (tt_transNeedValue & (1<<%d))~=0 then ", pStruct._ParVec[i]._transNeedValue);
				ttt_strStruct.append(t_ch);

				//strStruct.append(strTable_4);
				if (IsLuaBaseType(pStruct._ParVec[i]._strParType, false)) {
					//是否是枚举
					if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
						sprintf(t_ch, "self.%s = %s.read(outStream)", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
					}
					else {
						sprintf(t_ch, "self.%s = outStream:%s()", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadLuaType(pStruct._ParVec[i]._strParType.c_str()).c_str());
					}
				}
				else {
					sprintf(t_ch, "self.%s = %s.read(outStream)", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				ttt_strStruct.append(t_ch);

				//strStruct.append(strTable_3);
				ttt_strStruct.append(" else ");
				//strStruct.append(strTable_4);
				sprintf(t_ch, "self.%s=%s ", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strDefaultValue.c_str());
				ttt_strStruct.append(t_ch);
				//strStruct.append(strTable_3);
				ttt_strStruct.append(" end\n");

			}
			else {
				ttt_strStruct.append(strTable_3);
				if (IsLuaBaseType(pStruct._ParVec[i]._strParType, false)) {
					//是否是枚举
					if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
						sprintf(t_ch, "self.%s = %s.read(outStream)\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
					}
					else {
						sprintf(t_ch, "self.%s = outStream:%s()\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadLuaType(pStruct._ParVec[i]._strParType.c_str()).c_str());
					}
				}
				else {
					sprintf(t_ch, "self.%s = %s.read(outStream)\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				ttt_strStruct.append(t_ch);
			}

		}
	}



	ttt_strStruct.append(strTable_2);
	if (pStruct._version <= 1)
		sprintf(t_ch, "if tt_version > %d then\n", pStruct._version);
	else
		sprintf(t_ch, "elseif tt_version > %d then\n", pStruct._version);
	ttt_strStruct.append(t_ch);


	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		if (pStruct._ParVec[i]._version <= 1)
			continue;
		tt_bHasReadData = true;
		if (pStruct.IsHasTransNeedValuePar() && (pStruct._ParVec[i]._transNeedValue != -1)) {
			ttt_strStruct.append(strTable_3);
			sprintf(t_ch, "if (tt_transNeedValue & (1<<%d))~=0 then ", pStruct._ParVec[i]._transNeedValue);
			ttt_strStruct.append(t_ch);

			//strStruct.append(strTable_4);
			if (IsLuaBaseType(pStruct._ParVec[i]._strParType, false)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "self.%s = %s.read(outStream)", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				else {
					sprintf(t_ch, "self.%s = outStream:%s()", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadLuaType(pStruct._ParVec[i]._strParType.c_str()).c_str());
				}
			}
			else {
				sprintf(t_ch, "o.%s = %s.read(outStream)", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
			}
			ttt_strStruct.append(t_ch);

			//strStruct.append(strTable_3);
			ttt_strStruct.append(" else ");
			//strStruct.append(strTable_4);
			sprintf(t_ch, "self.%s=%s", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strDefaultValue.c_str());
			ttt_strStruct.append(t_ch);
			//strStruct.append(strTable_3);
			ttt_strStruct.append(" end\n");
		}
		else {
			ttt_strStruct.append(strTable_3);
			if (IsLuaBaseType(pStruct._ParVec[i]._strParType, false)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "self.%s = %s.read(outStream)\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				else {
					sprintf(t_ch, "self.%s = outStream:%s()\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadLuaType(pStruct._ParVec[i]._strParType.c_str()).c_str());
				}
			}
			else {
				sprintf(t_ch, "self.%s = %s.read(outStream)\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
			}
			ttt_strStruct.append(t_ch);
		}


	}

	ttt_strStruct.append(strTable_3);
	ttt_strStruct.append("local tt_logsize2 = outStream:GetPos()\n");

	ttt_strStruct.append(strTable_3);
	ttt_strStruct.append("local tt_offset = tt_len - (tt_logsize2 - tt_logsize1)\n");

	ttt_strStruct.append(strTable_3);
	ttt_strStruct.append("if tt_offset>0 then outStream:Offset(tt_offset) end\n");
	//ttt_strStruct.append(strTable_3);
	//ttt_strStruct.append("if tt_offset>0 then\n");


	//ttt_strStruct.append(strTable_4);
	//ttt_strStruct.append("outStream:Offset(tt_offset)\n");

	//ttt_strStruct.append(strTable_3);
	//ttt_strStruct.append("end\n");


	if (pStruct._version > 1) {
		ttt_strStruct.append(strTable_2);
		ttt_strStruct.append("else\n");

		for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
			if (pStruct._ParVec[i]._version <= 1)
				continue;
			tt_bHasReadData = true;
			ttt_strStruct.append(strTable_3);
			sprintf(t_ch, "if tt_version>=%d then", pStruct._ParVec[i]._version);
			ttt_strStruct.append(t_ch);


			if (pStruct.IsHasTransNeedValuePar() && (pStruct._ParVec[i]._transNeedValue != -1)) {
				ttt_strStruct.append("\n");
				ttt_strStruct.append(strTable_4);
				sprintf(t_ch, "if (tt_transNeedValue & (1<<%d))~=0 then ", pStruct._ParVec[i]._transNeedValue);
				ttt_strStruct.append(t_ch);

				//strStruct.append(strTable_5);
				if (IsLuaBaseType(pStruct._ParVec[i]._strParType, false)) {
					//是否是枚举
					if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
						sprintf(t_ch, "self.%s = %s.read(outStream)", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
					}
					else {
						sprintf(t_ch, "self.%s = outStream:%s()", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadLuaType(pStruct._ParVec[i]._strParType.c_str()).c_str());
					}
				}
				else {
					sprintf(t_ch, "self.%s = %s.read(outStream)", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				ttt_strStruct.append(t_ch);

				//strStruct.append(strTable_4);
				ttt_strStruct.append(" else ");
				//strStruct.append(strTable_5);
				sprintf(t_ch, "self.%s=%s", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strDefaultValue.c_str());
				ttt_strStruct.append(t_ch);
				//strStruct.append(strTable_4);
				ttt_strStruct.append(" end\n");
			}
			else {
				//ttt_strStruct.append(strTable_4);
				ttt_strStruct.append(" ");
				if (IsLuaBaseType(pStruct._ParVec[i]._strParType, false)) {
					//是否是枚举
					if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
						sprintf(t_ch, "self.%s = %s.read(outStream)", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
					}
					else {
						sprintf(t_ch, "self.%s = outStream:%s()", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadLuaType(pStruct._ParVec[i]._strParType.c_str()).c_str());
					}
				}
				else {
					sprintf(t_ch, "self.%s = %s.read(outStream)", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				ttt_strStruct.append(t_ch);
			}


			string tt_default = GetDefaultValue(pStruct._ParVec[i]._strParType, false, false, true);
			if (tt_default.length() > 0) {
				//ttt_strStruct.append(strTable_3);
				ttt_strStruct.append(" else ");

				//ttt_strStruct.append(strTable_4);
				sprintf(t_ch, "self.%s=%s", pStruct._ParVec[i]._strParName.c_str(), tt_default.c_str());
				ttt_strStruct.append(t_ch);
			}

			//ttt_strStruct.append(strTable_3);
			ttt_strStruct.append(" end\n");

		}
	}


	ttt_strStruct.append(strTable_2);
	ttt_strStruct.append("end\n");

	if (ttt_strStruct.length() > 0 /*&& tt_bHasReadData*/)
		strStruct.append(ttt_strStruct.c_str());

	strStruct.append(strTable_1);
	strStruct.append("end\n");

	strStruct.append("\n");




	////////////////////
	////
	////////////////////

	//写写
	strStruct.append(strTable_1);
	sprintf(t_ch, "function %s:write(inStream)\n", tt_strMName.c_str());
	strStruct.append(t_ch);


	//先写入结构体的大小
	strStruct.append(strTable_2);
	strStruct.append("local tt_len  = 0\n");
	strStruct.append(strTable_2);
	strStruct.append("local tt_logsize1 = inStream:GetPos()\n");
	strStruct.append(strTable_2);
	strStruct.append("inStream:WriteShort(tt_len)\n");


	//再写入版本
	strStruct.append(strTable_2);
	sprintf(t_ch, "local  tt_version = %d\n", pStruct._version);
	strStruct.append(t_ch);
	strStruct.append(strTable_2);
	strStruct.append("inStream:WriteShort(tt_version)\n");

	strStruct.append(strTable_2);
	strStruct.append("local tt_logsize2 = inStream:GetPos()\n");

	if (pStruct.IsHasTransNeedValuePar()) {
		strStruct.append(strTable_2);
		strStruct.append("local tt_transNeedValue=0\n");
		strStruct.append(strTable_2);
		strStruct.append("inStream:WriteInt(tt_transNeedValue)\n");
	}



	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		if (pStruct._ParVec[i]._transNeedValue != -1) {
			strStruct.append(strTable_2);
			sprintf(t_ch, "if self.%s~=%s then\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strDefaultValue.c_str());
			strStruct.append(t_ch);

			strStruct.append(strTable_3);
			if (IsLuaBaseType(pStruct._ParVec[i]._strParType, true)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "%s.write(inStream,self.%s)\n", pStruct._ParVec[i]._strParType.c_str(), pStruct._ParVec[i]._strParName.c_str());
				}
				else
					sprintf(t_ch, "inStream:%s(self.%s)\n", TypeConvertToWriteLuaType(pStruct._ParVec[i]._strParType.c_str()).c_str(), pStruct._ParVec[i]._strParName.c_str());
			}
			else {
				sprintf(t_ch, "self.%s:write(inStream)\n", pStruct._ParVec[i]._strParName.c_str());
			}
			strStruct.append(t_ch);


			strStruct.append(strTable_3);
			sprintf(t_ch, "tt_transNeedValue = tt_transNeedValue | (2^%d)\n", pStruct._ParVec[i]._transNeedValue);
			strStruct.append(t_ch);

			strStruct.append(strTable_2);
			strStruct.append("end\n");
		}
		else {
			strStruct.append(strTable_2);
			if (IsLuaBaseType(pStruct._ParVec[i]._strParType, true)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "%s.write(inStream,self.%s)\n", pStruct._ParVec[i]._strParType.c_str(), pStruct._ParVec[i]._strParName.c_str());
				}
				else
					sprintf(t_ch, "inStream:%s(self.%s)\n", TypeConvertToWriteLuaType(pStruct._ParVec[i]._strParType.c_str()).c_str(), pStruct._ParVec[i]._strParName.c_str());
			}
			else {
				sprintf(t_ch, "self.%s:write(inStream)\n", pStruct._ParVec[i]._strParName.c_str());
			}
			strStruct.append(t_ch);
		}


	}
	strStruct.append(strTable_2);
	strStruct.append("local tt_logsize4 = inStream:GetPos()\n");
	strStruct.append(strTable_2);
	strStruct.append("local tt_logsize3 = tt_logsize4 - tt_logsize2\n");
	strStruct.append(strTable_2);
	strStruct.append("inStream:SetPos(tt_logsize1)\n");
	strStruct.append(strTable_2);
	strStruct.append("inStream:WriteShort(tt_logsize3)\n");

	if (pStruct.IsHasTransNeedValuePar()) {
		strStruct.append(strTable_2);
		strStruct.append("if tt_transNeedValue~=0 then\n");
		strStruct.append(strTable_3);
		strStruct.append("inStream:SetPos(tt_logsize2)\n");
		strStruct.append(strTable_3);
		strStruct.append("inStream:WriteInt(tt_transNeedValue)\n");
		strStruct.append(strTable_2);
		strStruct.append("end\n");
	}


	strStruct.append(strTable_2);
	strStruct.append("inStream:SetPos(tt_logsize4)\n");

	strStruct.append(strTable_1);
	strStruct.append("end\n");
	strStruct.append("\n");


	strStruct.append(strTable_1);
	sprintf(t_ch, "return %s", tt_strMName.c_str());
	strStruct.append(t_ch);

	fwrite(strStruct.c_str(), strStruct.length(), 1, fp);

	fclose(fp);
}
void    ProtocolParse_Writer_Lua::WriteVector2_Lua(stNamespace& pNamespace, stVector& pVector, string foldername, const string& strWriteNameSpace)
{
	char t_ch[512];
	sprintf(t_ch, "%s/%s.lua", foldername.c_str(), pVector._name.c_str());

	//先创建文件
	FILE* fp = fopen(t_ch, "w");
	if (!fp)
		return;

	//写信息头,共用
	WriteInfoHead(fp, true);



	string strTable_1 = CreateTableString(0);
	string strTable_2 = CreateTableString(1);
	string strTable_3 = CreateTableString(2);
	string strTable_4 = CreateTableString(3);

	string strVector;

	strVector.append(strTable_1);
	strVector.append("local InStream = CS.InOutStream.InStream\n");
	strVector.append(strTable_1);
	strVector.append("local OutStream = CS.InOutStream.OutStream\n");
	strVector.append("\n");

	string tt_strMName;
	sprintf(t_ch, "%s", pVector._name.c_str());
	tt_strMName = t_ch;

	vector<string> tt_PacketNameList;
	GetUsePacketNamesByParType_Lua(pVector._name, tt_PacketNameList);
	string tt_strSelf;
	sprintf(t_ch, "local %s = require \"%s%s/%s\"", pVector._name.c_str(),
		m_strLuaPackage.c_str(), pNamespace._protocolname.c_str(), pVector._name.c_str());
	tt_strSelf = t_ch;
	RemovePacketNames(tt_PacketNameList, tt_strSelf);

	for (size_t i = 0; i < tt_PacketNameList.size(); i++) {
		strVector.append(strTable_1);
		strVector.append(tt_PacketNameList[i].c_str());
		strVector.append("\n");
	}

	if (tt_PacketNameList.size() > 0) {
		strVector.append("\n");
	}



	/////////////有两种版本，一种是非Flash基础类型，一种是
	bool  tt_bCSharpBaseType = IsLuaBaseType2(pVector._ParTName);
	//if(!tt_bCSharpBaseType)
	//{
	//	strVector.append(strTable_1);
	//	sprintf(t_ch,"local %s = require \"%s%s/%s\"\n",pVector._ParTName.c_str(),m_strLuaPackage.c_str(),pNamespace._protocolname.c_str(),pVector._ParTName.c_str());
	//	strVector.append(t_ch);
	//	strVector.append("\n");
	//}

	strVector.append(strTable_1);
	sprintf(t_ch, "local %s = class()\n", tt_strMName.c_str());
	strVector.append(t_ch);
	strVector.append("\n");


	strVector.append(WriteComment_Lua(strTable_1, pVector._comment, "").c_str());


	strVector.append(strTable_1);
	sprintf(t_ch, "function %s.read(outStream)\n", tt_strMName.c_str());
	strVector.append(t_ch);


	strVector.append(strTable_2);
	sprintf(t_ch, "local o = %s.New()\n", tt_strMName.c_str());
	strVector.append(t_ch);





	strVector.append(strTable_2);
	strVector.append("o._bytes = outStream:ReadByteArray()\n");
	strVector.append(strTable_2);
	strVector.append("return o\n");

	strVector.append(strTable_1);
	strVector.append("end\n");
	strVector.append("\n");

	////////////////
	//
	////////////////

	strVector.append(strTable_1);
	sprintf(t_ch, "function %s:readLocal(outStream)\n", tt_strMName.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("self._bytes = outStream:ReadByteArray()\n");

	strVector.append(strTable_1);
	strVector.append("end\n");
	strVector.append("\n");

	/////////////
	//
	////////////
	strVector.append(strTable_1);
	sprintf(t_ch, "function %s:write(inStream)\n", tt_strMName.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("if self._bytes == nil then\n");
	strVector.append(strTable_3);
	strVector.append("inStream:WriteInt(0)\n");
	strVector.append(strTable_3);
	strVector.append("return\n");
	strVector.append(strTable_2);
	strVector.append("end\n");

	strVector.append(strTable_2);
	strVector.append("inStream:WriteInt(#self._bytes)\n");
	strVector.append(strTable_2);
	strVector.append("inStream:Write(self._bytes,0,#self._bytes)\n");

	strVector.append(strTable_1);
	strVector.append("end\n");
	strVector.append("\n");

	sprintf(t_ch, "return %s", tt_strMName.c_str());
	strVector.append(t_ch);

	fwrite(strVector.c_str(), strVector.length(), 1, fp);
	fclose(fp);
}
void    ProtocolParse_Writer_Lua::WriteVector_Lua(stNamespace& pNamespace, stVector& pVector, string foldername, const string& strWriteNameSpace)
{
	//if (pVector._name.compare("bytevecgw") == 0)
	//{
	//	int a;
	//	a = 10;
	//}
	if (pVector._ParTName == "byte" || pVector._ParTName == "BYTE" || pVector._ParTName == "char" || pVector._ParTName == "unsigned char") {
		WriteVector2_Lua(pNamespace, pVector, foldername, strWriteNameSpace);
		return;
	}
	char t_ch[512];
	sprintf(t_ch, "%s/%s.lua", foldername.c_str(), pVector._name.c_str());

	//先创建文件
	FILE* fp = fopen(t_ch, "w");
	if (!fp)
		return;

	//写信息头,共用
	WriteInfoHead(fp, true);



	string strTable_1 = CreateTableString(0);
	string strTable_2 = CreateTableString(1);
	string strTable_3 = CreateTableString(2);
	string strTable_4 = CreateTableString(3);

	string strVector;

	strVector.append(strTable_1);
	strVector.append("local InStream = CS.InOutStream.InStream\n");
	strVector.append(strTable_1);
	strVector.append("local OutStream = CS.InOutStream.OutStream\n");
	strVector.append("\n");

	string tt_strMName;
	sprintf(t_ch, "%s", pVector._name.c_str());
	tt_strMName = t_ch;

	vector<string> tt_PacketNameList;
	GetUsePacketNamesByParType_Lua(pVector._name, tt_PacketNameList);
	string tt_strSelf;
	sprintf(t_ch, "local %s = require \"%s%s/%s\"", pVector._name.c_str(),
		m_strLuaPackage.c_str(), pNamespace._protocolname.c_str(), pVector._name.c_str());
	tt_strSelf = t_ch;
	RemovePacketNames(tt_PacketNameList, tt_strSelf);

	for (size_t i = 0; i < tt_PacketNameList.size(); i++) {
		strVector.append(strTable_1);
		strVector.append(tt_PacketNameList[i].c_str());
		strVector.append("\n");
	}

	if (tt_PacketNameList.size() > 0) {
		strVector.append("\n");
	}



	/////////////有两种版本，一种是非Flash基础类型，一种是
	bool  tt_bCSharpBaseType = IsLuaBaseType2(pVector._ParTName);
	//if(!tt_bCSharpBaseType)
	//{
	//	strVector.append(strTable_1);
	//	sprintf(t_ch,"local %s = require \"%s%s/%s\"\n",pVector._ParTName.c_str(),m_strLuaPackage.c_str(),pNamespace._protocolname.c_str(),pVector._ParTName.c_str());
	//	strVector.append(t_ch);
	//	strVector.append("\n");
	//}

	strVector.append(strTable_1);
	sprintf(t_ch, "local %s = class(List)\n", tt_strMName.c_str());
	strVector.append(t_ch);
	strVector.append("\n");


	strVector.append(WriteComment_Lua(strTable_1, pVector._comment, "").c_str());


	strVector.append(strTable_1);
	sprintf(t_ch, "function %s.read(outStream)\n", tt_strMName.c_str());
	strVector.append(t_ch);


	strVector.append(strTable_2);
	sprintf(t_ch, "local o = %s.New()\n", tt_strMName.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("local count = outStream:ReadInt()\n");



	strVector.append(strTable_2);
	strVector.append("for i=0,count-1 do\n");
	if (!tt_bCSharpBaseType) {
		strVector.append(strTable_3);
		sprintf(t_ch, "o:Add(%s.read(outStream))\n", pVector._ParTName.c_str());
		strVector.append(t_ch);
	}
	else {
		strVector.append(strTable_3);
		sprintf(t_ch, "o:Add(outStream:%s())\n", TypeConvertToReadLuaType(pVector._ParTName).c_str());
		strVector.append(t_ch);
	}

	strVector.append(strTable_2);
	strVector.append("end\n");

	strVector.append(strTable_2);
	strVector.append("return o\n");

	strVector.append(strTable_1);
	strVector.append("end\n");
	strVector.append("\n");

	////////////////
	//
	////////////////

	strVector.append(strTable_1);
	sprintf(t_ch, "function %s:readLocal(outStream)\n", tt_strMName.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("self:Clear()\n");


	strVector.append(strTable_2);
	strVector.append("local count = outStream:ReadInt()\n");



	strVector.append(strTable_2);
	strVector.append("for i=0,count-1 do\n");

	if (!tt_bCSharpBaseType) {
		strVector.append(strTable_3);
		sprintf(t_ch, "self:Add(%s.read(outStream))\n", pVector._ParTName.c_str());
		strVector.append(t_ch);
	}
	else {
		strVector.append(strTable_3);
		sprintf(t_ch, "self:Add(outStream:%s())\n", TypeConvertToReadLuaType(pVector._ParTName).c_str());
		strVector.append(t_ch);
	}

	strVector.append(strTable_2);
	strVector.append("end\n");

	strVector.append(strTable_1);
	strVector.append("end\n");
	strVector.append("\n");

	/////////////
	//
	////////////
	strVector.append(strTable_1);
	sprintf(t_ch, "function %s:write(inStream)\n", tt_strMName.c_str());
	strVector.append(t_ch);


	strVector.append(strTable_2);
	strVector.append("inStream:WriteInt(self.Count)\n");

	strVector.append(strTable_2);
	strVector.append("for  i=0,self.Count-1 do\n");

	//
	if (!tt_bCSharpBaseType) {
		strVector.append(strTable_3);
		sprintf(t_ch, "self[i]:write(inStream)\n");
		strVector.append(t_ch);
	}
	else {
		strVector.append(strTable_3);
		sprintf(t_ch, "inStream:%s(self[i])\n", TypeConvertToWriteLuaType(pVector._ParTName).c_str());
		strVector.append(t_ch);
	}


	strVector.append(strTable_2);
	strVector.append("end\n");

	strVector.append(strTable_1);
	strVector.append("end\n");
	strVector.append("\n");

	sprintf(t_ch, "return %s", tt_strMName.c_str());
	strVector.append(t_ch);

	fwrite(strVector.c_str(), strVector.length(), 1, fp);
	fclose(fp);
}

void    ProtocolParse_Writer_Lua::WriteWJSVector_Lua(stNamespace& pNamespace, stWJSVector& pWJSVector, string foldername, const string& strWriteNameSpace)
{
	stVector tt_Vector;
	tt_Vector._name = pWJSVector._name;
	tt_Vector._ParTName = pWJSVector._ParTName;
	tt_Vector._index = pWJSVector._index;
	tt_Vector._version = pWJSVector._version;
	tt_Vector._bBaseParT = pWJSVector._bBaseParT;
	WriteVector_Lua(pNamespace, tt_Vector, foldername, strWriteNameSpace);
}

void    ProtocolParse_Writer_Lua::WriteClass_Lua(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace)
{
	switch (filetype) {
	case 0:
	{
		////对于客户端而言，request的为发送，callback的为回调
		if (pClass._strtype.compare("callback") == 0) {
			WriteClass_IReceiver_Lua(pClass, filetype, pNamespace, foldername, false, strWriteNameSpace);
			WriteClass_Receiver_Lua(pClass, filetype, pNamespace, foldername, strWriteNameSpace);
		}
		else {
			WriteClass_Send_Lua(pClass, filetype, pNamespace, foldername, strWriteNameSpace);
		}
	}
	break;
	case 1:
	{
		////服务器而言，请求的是回调，callback是发送
		if (pClass._strtype.compare("request") == 0) //C2S
		{
			WriteClass_IReceiver_Lua(pClass, filetype, pNamespace, foldername, false, strWriteNameSpace);
			WriteClass_Receiver_Lua(pClass, filetype, pNamespace, foldername, strWriteNameSpace);
		}
		else //S2C
		{
			WriteClass_Send_Lua(pClass, filetype, pNamespace, foldername, strWriteNameSpace);
		}
	}
	break;
	case 2:
	{

		//事实上两个里面是一样的
		if (pClass._strtype.compare("request") == 0) //C2S
		{
			WriteClass_IReceiver_Lua(pClass, filetype, pNamespace, foldername, true, strWriteNameSpace);
			WriteClass_Receiver_Lua(pClass, filetype, pNamespace, foldername, strWriteNameSpace);
			WriteClass_Send_Lua(pClass, filetype, pNamespace, foldername, strWriteNameSpace);
		}

	}
	break;
	default:
		break;
	}
}
void    ProtocolParse_Writer_Lua::WriteClass_IReceiver_Lua(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, bool bemptyimplement, const string& strWriteNameSpace)
{
	char t_ch[1024];
	sprintf(t_ch, "%s/IReceiver_%s.lua", foldername.c_str(), pNamespace._protocolname.c_str());

	//先创建文件
	FILE* fp = fopen(t_ch, "w");
	if (!fp)
		return;

	//写信息头,共用
	WriteInfoHead(fp, true);

	string strclass;

	string strTable_1 = CreateTableString(0);
	string strTable_2 = CreateTableString(1);
	string strTable_3 = CreateTableString(2);

	string tt_strMName;
	sprintf(t_ch, "IReceiver_%s", pClass._name.c_str());
	tt_strMName = t_ch;

	strclass.append(strTable_1);
	sprintf(t_ch, "local %s = class()\n", tt_strMName.c_str());
	strclass.append(t_ch);

	strclass.append("\n");
	fwrite(strclass.c_str(), strclass.length(), 1, fp);



	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		if (filetype == 2 && ((m_bSCSpliteS && pClass._FuncVec[i]._SCSpliteType == -1) || (m_bSCSpliteC && pClass._FuncVec[i]._SCSpliteType == 1)))
			continue;
		//写函数
		WriteFunc_IReceiver_Lua(pClass._FuncVec[i], filetype, 0, pClass, pNamespace, foldername, fp, bemptyimplement);
	}

	strclass = "";
	strclass.append("\n");
	strclass.append(strTable_1);
	sprintf(t_ch, "return %s\n", tt_strMName.c_str());
	strclass.append(t_ch);
	fwrite(strclass.c_str(), strclass.length(), 1, fp);
	fclose(fp);

}

void    ProtocolParse_Writer_Lua::WriteFunc_IReceiver_Lua(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp, bool bemptyimplement)
{
	char t_ch[1024];

	string strTable_0 = CreateTableString(tablenum);
	string strTable_1 = CreateTableString(tablenum + 1);


	string strFunc;

	string tt_strMName;
	sprintf(t_ch, "IReceiver_%s", pClass._name.c_str());
	tt_strMName = t_ch;

	strFunc.append(WriteComment_Lua(strTable_0, pFunc._comment, "", false).c_str());
	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		strFunc.append(WriteComment_Lua(strTable_0, pFunc._InParVec[i]._comment, pFunc._InParVec[i]._strParName, false).c_str());
	}




	strFunc.append(strTable_0);
	sprintf(t_ch, "function %s:IReceiver_%s(", tt_strMName.c_str(), pFunc._name.c_str());
	strFunc.append(t_ch);

	if (filetype == 0) {
		if (pFunc._InParVec.size() > 0) {
			for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
				if (IsExType_vector(pFunc._InParVec[i]._strParType)) {
					sprintf(t_ch, "%s,", pFunc._InParVec[i]._strParName.c_str());
					strFunc.append(t_ch);
				}
				else {
					sprintf(t_ch, "%s,", pFunc._InParVec[i]._strParName.c_str());
					strFunc.append(t_ch);
				}

			}
		}
		strFunc.append("pExData)\n");
	}
	else {
		if (pFunc._InParVec.size() > 0) {
			for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
				if (IsExType_vector(pFunc._InParVec[i]._strParType)) {
					sprintf(t_ch, "%s,", pFunc._InParVec[i]._strParName.c_str());
					strFunc.append(t_ch);
				}
				else {
					sprintf(t_ch, "%s,", pFunc._InParVec[i]._strParName.c_str());
					strFunc.append(t_ch);
				}

			}
		}

		if (filetype == 2) {
			strFunc.append("sessionId,pExData)\n");
		}
		else
			strFunc.append("sessionId,pExData)\n");
	}

	strFunc.append(strTable_0);
	strFunc.append("end\n");
	strFunc.append("\n");
	fwrite(strFunc.c_str(), strFunc.length(), 1, fp);
}

void    ProtocolParse_Writer_Lua::WriteClass_Receiver_Lua(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace)
{
	char t_ch[1024];
	sprintf(t_ch, "%s/%s_Callback.lua", foldername.c_str(), pNamespace._protocolname.c_str());

	//先创建文件
	FILE* fp = fopen(t_ch, "w");
	if (!fp)
		return;

	//写信息头,共用
	WriteInfoHead(fp, true);

	string strclass;

	string strTable_0 = CreateTableString(0);
	string strTable_1 = CreateTableString(1);
	string strTable_2 = CreateTableString(2);
	string strTable_3 = CreateTableString(3);



	string tt_strMName;
	sprintf(t_ch, "%s", pClass._name.c_str());
	tt_strMName = t_ch;

	strclass.append(strTable_0);
	strclass.append("local InStream = CS.InOutStream.InStream\n");
	strclass.append(strTable_0);
	strclass.append("local OutStream = CS.InOutStream.OutStream\n");

	strclass.append("\n");



	vector<string> tt_PacketNameList;
	sprintf(t_ch, "local ID_%s = require \"%s%s/ID_%s\"", pNamespace._protocolname.c_str(), m_strLuaPackage.c_str()
		, pNamespace._protocolname.c_str(), pNamespace._protocolname.c_str());
	tt_PacketNameList.push_back(t_ch);

	if (!m_bLuaRequireInFunc) {
		for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
			if (pClass._FuncVec[i]._bDiscard)
				continue;
			if (filetype == 2 && ((m_bSCSpliteS && pClass._FuncVec[i]._SCSpliteType == -1) || (m_bSCSpliteC && pClass._FuncVec[i]._SCSpliteType == 1)))
				continue;
			//if (pClass._FuncVec[i]._name.compare("OnRecvGameMessage") == 0)
			//{
			//	int a;
			//	a = 10;
			//}

			for (size_t j = 0; j < pClass._FuncVec[i]._InParVec.size(); j++) {
				if (!IsLuaBaseType(pClass._FuncVec[i]._InParVec[j]._strParType, false)) {
					GetUsePacketNamesByParType_Lua(pClass._FuncVec[i]._InParVec[j]._strParType, tt_PacketNameList);
				}
			}
		}
	}


	for (size_t i = 0; i < tt_PacketNameList.size(); i++) {
		strclass.append(strTable_0);
		strclass.append(tt_PacketNameList[i].c_str());
		strclass.append("\n");
	}

	if (tt_PacketNameList.size() > 0) {
		strclass.append("\n");
	}



	strclass.append(strTable_0);
	sprintf(t_ch, "local %s = {}\n", tt_strMName.c_str());
	strclass.append(t_ch);
	strclass.append("\n");

	fwrite(strclass.c_str(), strclass.length(), 1, fp);
	strclass = "";

	WriteFunc_ReceiverParser_Lua(pClass, filetype, 0, pNamespace, foldername, fp);


	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		if (filetype == 2 && ((m_bSCSpliteS && pClass._FuncVec[i]._SCSpliteType == -1) || (m_bSCSpliteC && pClass._FuncVec[i]._SCSpliteType == 1)))
			continue;
		WriteFunc_Receiver_Lua(pClass._FuncVec[i], filetype, 0, pClass, pNamespace, foldername, fp);
	}


	strclass = "";
	strclass.append("\n");
	strclass.append(strTable_0);
	sprintf(t_ch, "return %s", tt_strMName.c_str());
	strclass.append(t_ch);

	fwrite(strclass.c_str(), strclass.length(), 1, fp);
	fclose(fp);
}

void    ProtocolParse_Writer_Lua::WriteFunc_ReceiverParser_Lua(stClass& pClass, int filetype, int tablenum, stNamespace& pNamespace, string foldername, FILE* fp)
{
	char t_ch[1024];

	string strTable_0 = CreateTableString(tablenum);
	string strTable_1 = CreateTableString(tablenum + 1);
	string strTable_2 = CreateTableString(tablenum + 2);
	string strTable_3 = CreateTableString(tablenum + 3);
	string strTable_4 = CreateTableString(tablenum + 4);

	string tt_strMName;
	sprintf(t_ch, "%s", pClass._name.c_str());
	tt_strMName = t_ch;


	string strFunc;
	strFunc.append(strTable_0);

	if (filetype == 0) {
		sprintf(t_ch, "function %s.Parser(cls,outStream,pExData)\n", tt_strMName.c_str());
		strFunc.append(t_ch);
	}
	else {
		sprintf(t_ch, "function %s.Parser(cls,outStream,sessionId,pExData)\n", tt_strMName.c_str());
		strFunc.append(t_ch);
	}


	strFunc.append(strTable_1);
	strFunc.append("local no = outStream:ReadInt()\n");

	strFunc.append(strTable_1);
	sprintf(t_ch, "if no ~=ID_%s.%s then\n", pNamespace._protocolname.c_str(), m_ProtocolNo._name.c_str());
	strFunc.append(t_ch);


	strFunc.append(strTable_2);
	strFunc.append("outStream:Offset(- 4)\n");

	strFunc.append(strTable_2);
	strFunc.append("return false\n");

	strFunc.append(strTable_1);
	strFunc.append("end\n");

	strFunc.append(strTable_1);
	strFunc.append("local proversionno = outStream:ReadShort()\n");


	strFunc.append(strTable_1);
	strFunc.append("local methodid =outStream:ReadInt()\n");


	strFunc.append("\n");
	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;

		strFunc.append(strTable_1);
		if (i == 0) {
			sprintf(t_ch, "if methodid == ID_%s.ID_%s_%s then\n", pNamespace._protocolname.c_str(), pClass._name.c_str(), pClass._FuncVec[i]._name.c_str());
			strFunc.append(t_ch);

		}
		else {
			sprintf(t_ch, "elseif methodid == ID_%s.ID_%s_%s then\n", pNamespace._protocolname.c_str(), pClass._name.c_str(), pClass._FuncVec[i]._name.c_str());
			strFunc.append(t_ch);
		}

		strFunc.append(strTable_2);
		if (filetype == 0) {
			sprintf(t_ch, "%s._parser_%s(cls,outStream,pExData,proversionno)\n", tt_strMName.c_str(), pClass._FuncVec[i]._name.c_str());
			strFunc.append(t_ch);
		}
		else {
			sprintf(t_ch, "%s._parser_%s(cls,outStream,sessionId,pExData,proversionno)\n", tt_strMName.c_str(), pClass._FuncVec[i]._name.c_str());
			strFunc.append(t_ch);
		}

		strFunc.append(strTable_2);
		strFunc.append("return true\n");
	}


	strFunc.append(strTable_1);
	strFunc.append("else\n");

	strFunc.append(strTable_2);
	strFunc.append("return false\n");

	strFunc.append(strTable_1);
	strFunc.append("end\n");

	strFunc.append(strTable_0);
	strFunc.append("end\n");

	strFunc.append("\n");


	fwrite(strFunc.c_str(), strFunc.length(), 1, fp);
}

void    ProtocolParse_Writer_Lua::WriteFunc_Receiver_Lua(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp)
{
	char t_ch[1024];

	string strTable_0 = CreateTableString(tablenum);
	string strTable_1 = CreateTableString(tablenum + 1);
	string strTable_2 = CreateTableString(tablenum + 2);
	string strTable_3 = CreateTableString(tablenum + 3);


	string tt_strMName;
	sprintf(t_ch, "%s", pClass._name.c_str());
	tt_strMName = t_ch;

	string strFunc;

	if (filetype == 0) {
		strFunc.append(strTable_0);
		sprintf(t_ch, "function  %s._parser_%s(cls,outStream,pExData,proversionno)\n", tt_strMName.c_str(), pFunc._name.c_str()/*,pNamespace._protocolname.c_str()*/);
		strFunc.append(t_ch);
	}
	else {
		strFunc.append(strTable_0);
		sprintf(t_ch, "function %s._parser_%s(cls,outStream,sessionId,pExData,proversionno)\n", tt_strMName.c_str(), pFunc._name.c_str()/*,pNamespace._protocolname.c_str()*/);
		strFunc.append(t_ch);
	}

	if (m_bLuaRequireInFunc) {
		vector<string> tt_PacketNameList;
		for (size_t j = 0; j < pFunc._InParVec.size(); j++) {
			if (!IsLuaBaseType(pFunc._InParVec[j]._strParType, false)) {
				GetUsePacketNamesByParType_Lua(pFunc._InParVec[j]._strParType, tt_PacketNameList, true);
			}
		}
		for (size_t i = 0; i < tt_PacketNameList.size(); i++) {
			strFunc.append(strTable_1);
			strFunc.append(tt_PacketNameList[i].c_str());
			strFunc.append("\n");
		}
	}


	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		if (i == 0)
			strFunc.append(strTable_1);

		if (IsLuaBaseType(pFunc._InParVec[i]._strParType, false) || IsExType_enum(pFunc._InParVec[i]._strParType)) {
			if (i == 0)
				sprintf(t_ch, "local %s", pFunc._InParVec[i]._strParName.c_str());
			else
				sprintf(t_ch, ",%s", pFunc._InParVec[i]._strParName.c_str());

		}
		else if (IsExType_vector(pFunc._InParVec[i]._strParType)) {
			if (i == 0)
				sprintf(t_ch, "local %s", pFunc._InParVec[i]._strParName.c_str());
			else
				sprintf(t_ch, ",%s", pFunc._InParVec[i]._strParName.c_str());
		}
		else {
			if (i == 0)
				sprintf(t_ch, "local %s", pFunc._InParVec[i]._strParName.c_str());
			else
				sprintf(t_ch, ",%s", pFunc._InParVec[i]._strParName.c_str());

		}
		strFunc.append(t_ch);
	}
	strFunc.append("\n");

	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		if (pFunc._InParVec[i]._version > 1)
			break;
		strFunc.append(strTable_1);
		if (IsLuaBaseType(pFunc._InParVec[i]._strParType, false)) {
			sprintf(t_ch, "%s = outStream:%s()\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToReadLuaType(pFunc._InParVec[i]._strParType)).c_str());

		}
		else if (IsExType_enum(pFunc._InParVec[i]._strParType)) {
			sprintf(t_ch, "%s = %s.read(outStream)\n", pFunc._InParVec[i]._strParName.c_str(), pFunc._InParVec[i]._strParType.c_str());
		}
		else {
			sprintf(t_ch, "%s = %s.read(outStream)\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToLuaType(pFunc._InParVec[i]._strParType)).c_str());
		}
		strFunc.append(t_ch);
	}

	string ttt_strFunc;
	bool tt_bHasReadData = false;

	//作出判定相等
	ttt_strFunc.append(strTable_1);
	sprintf(t_ch, "if proversionno==%d or proversionno==0 then\n", pFunc._version);
	ttt_strFunc.append(t_ch);


	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		if (pFunc._InParVec[i]._version <= 1)
			continue;
		tt_bHasReadData = true;
		ttt_strFunc.append(strTable_2);
		if (IsLuaBaseType(pFunc._InParVec[i]._strParType, false)) {
			sprintf(t_ch, "%s = outStream:%s()\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToReadLuaType(pFunc._InParVec[i]._strParType)).c_str());

		}
		else if (IsExType_enum(pFunc._InParVec[i]._strParType)) {
			sprintf(t_ch, "%s = %s.read(outStream)\n", pFunc._InParVec[i]._strParName.c_str(), pFunc._InParVec[i]._strParType.c_str());
		}
		else {
			sprintf(t_ch, "%s = %s.read(outStream)\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToLuaType(pFunc._InParVec[i]._strParType)).c_str());
		}
		ttt_strFunc.append(t_ch);
	}

	//strFunc.append(strTable_2);
	//sprintf(t_ch,"cls:IReceiver_%s(",pFunc._name.c_str());
	//strFunc.append(t_ch);


	//for(int i=0;i<pFunc._InParVec.size();i++)
	//{
	//	sprintf(t_ch,"%s,",pFunc._InParVec[i]._strParName.c_str());
	//	strFunc.append(t_ch);
	//}

	//if(filetype==0)
	//{
	//	strFunc.append("pExData)\n");
	//}
	//else
	//{
	//	strFunc.append("sessionId,pExData)\n");
	//}



	//////////////低解高
	ttt_strFunc.append(strTable_1);
	sprintf(t_ch, "elseif proversionno>%d then\n", pFunc._version);
	ttt_strFunc.append(t_ch);



	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		if (pFunc._InParVec[i]._version <= 1)
			continue;
		tt_bHasReadData = true;
		ttt_strFunc.append(strTable_2);
		if (IsLuaBaseType(pFunc._InParVec[i]._strParType, false)) {
			sprintf(t_ch, "%s = outStream:%s()\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToReadLuaType(pFunc._InParVec[i]._strParType)).c_str());
		}
		else if (IsExType_enum(pFunc._InParVec[i]._strParType)) {
			sprintf(t_ch, "%s = %s.read(outStream)\n", pFunc._InParVec[i]._strParName.c_str(), pFunc._InParVec[i]._strParType.c_str());
		}
		else {
			sprintf(t_ch, "%s = %s.read(outStream)\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToLuaType(pFunc._InParVec[i]._strParType)).c_str());
		}
		ttt_strFunc.append(t_ch);
	}

	//strFunc.append(strTable_2);
	//sprintf(t_ch,"cls:IReceiver_%s(",pFunc._name.c_str());
	//strFunc.append(t_ch);


	//for(int i=0;i<pFunc._InParVec.size();i++)
	//{
	//	sprintf(t_ch,"%s,",pFunc._InParVec[i]._strParName.c_str());
	//	strFunc.append(t_ch);
	//}

	//if(filetype==0)
	//{
	//	strFunc.append("pExData)\n");
	//}
	//else
	//{
	//	strFunc.append("sessionId,pExData)\n");
	//}


	///////////////高解低
	ttt_strFunc.append(strTable_1);
	ttt_strFunc.append("else\n");


	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		if (pFunc._InParVec[i]._version <= 1)
			continue;
		tt_bHasReadData = true;
		ttt_strFunc.append(strTable_2);
		sprintf(t_ch, "if proversionno>=%d then ", pFunc._InParVec[i]._version);
		ttt_strFunc.append(t_ch);

		//ttt_strFunc.append(strTable_3);
		if (IsLuaBaseType(pFunc._InParVec[i]._strParType, false)) {
			sprintf(t_ch, "%s = outStream:%s() ", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToReadLuaType(pFunc._InParVec[i]._strParType)).c_str());
		}
		else if (IsExType_enum(pFunc._InParVec[i]._strParType)) {
			sprintf(t_ch, "%s = %s.read(outStream) ", pFunc._InParVec[i]._strParName.c_str(), pFunc._InParVec[i]._strParType.c_str());
		}
		else {
			sprintf(t_ch, "%s = %s.read(outStream) ", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToLuaType(pFunc._InParVec[i]._strParType)).c_str());
		}
		ttt_strFunc.append(t_ch);

		string tt_default = GetDefaultValue(pFunc._InParVec[i]._strParType, false, false, true);
		if (tt_default.length() > 0) {
			//ttt_strFunc.append(strTable_2);
			ttt_strFunc.append(" else ");

			//ttt_strFunc.append(strTable_3);
			sprintf(t_ch, "%s = %s", pFunc._InParVec[i]._strParName.c_str(), tt_default.c_str());
			ttt_strFunc.append(t_ch);
		}
		else if (IsExType_vector(pFunc._InParVec[i]._strParType)) {
			//ttt_strFunc.append(strTable_2);
			ttt_strFunc.append(" else ");

			//ttt_strFunc.append(strTable_3);
			sprintf(t_ch, "%s = %s.New()", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToLuaType(pFunc._InParVec[i]._strParType)).c_str());
			ttt_strFunc.append(t_ch);
		}
		else if (IsExType_WJSVector(pFunc._InParVec[i]._strParType)) {
			//ttt_strFunc.append(strTable_2);
			ttt_strFunc.append(" else ");

			//ttt_strFunc.append(strTable_3);
			sprintf(t_ch, "%s =%s.New()", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToLuaType(pFunc._InParVec[i]._strParType)).c_str());
			ttt_strFunc.append(t_ch);
		}
		else if (IsExType_struct(pFunc._InParVec[i]._strParType)) {
			//ttt_strFunc.append(strTable_2);
			ttt_strFunc.append(" else ");

			//ttt_strFunc.append(strTable_3);
			sprintf(t_ch, "%s = %s.New() ", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToLuaType(pFunc._InParVec[i]._strParType)).c_str());
			ttt_strFunc.append(t_ch);
		}
		else if (IsExType_enum(pFunc._InParVec[i]._strParType)) {
			//ttt_strFunc.append(strTable_2);
			ttt_strFunc.append("else ");

			//ttt_strFunc.append(strTable_3);
			sprintf(t_ch, "%s = 0", pFunc._InParVec[i]._strParName.c_str());
			ttt_strFunc.append(t_ch);
		}
		//ttt_strFunc.append(strTable_2);
		ttt_strFunc.append(" end\n");
	}

	//strFunc.append(strTable_2);
	//sprintf(t_ch,"cls:IReceiver_%s(",pFunc._name.c_str());
	//strFunc.append(t_ch);


	//for(int i=0;i<pFunc._InParVec.size();i++)
	//{
	//	sprintf(t_ch,"%s,",pFunc._InParVec[i]._strParName.c_str());
	//	strFunc.append(t_ch);
	//}

	//if(filetype==0)
	//{
	//	strFunc.append("pExData);\n");
	//}
	//else
	//{
	//	strFunc.append("sessionId,pExData)\n");
	//}

	ttt_strFunc.append(strTable_1);
	ttt_strFunc.append("end\n");

	if (tt_bHasReadData)
		strFunc.append(ttt_strFunc.c_str());

	strFunc.append(strTable_1);
	sprintf(t_ch, "cls:IReceiver_%s(", pFunc._name.c_str());
	strFunc.append(t_ch);


	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		sprintf(t_ch, "%s,", pFunc._InParVec[i]._strParName.c_str());
		strFunc.append(t_ch);
	}

	if (filetype == 0) {
		strFunc.append("pExData);\n");
	}
	else {
		strFunc.append("sessionId,pExData)\n");
	}

	/////////////////
	strFunc.append(strTable_0);
	strFunc.append("end\n");
	strFunc.append("\n");


	fwrite(strFunc.c_str(), strFunc.length(), 1, fp);
}

void    ProtocolParse_Writer_Lua::WriteClass_Send_Lua(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace)
{
	char t_ch[512];
	sprintf(t_ch, "%s/%s_Send.lua", foldername.c_str(), pNamespace._protocolname.c_str());

	//先创建文件
	FILE* fp = fopen(t_ch, "w");
	if (!fp)
		return;

	//写信息头,共用
	WriteInfoHead(fp, true);

	string strclass;

	string strTable_1 = CreateTableString(0);
	string strTable_2 = CreateTableString(1);
	string strTable_3 = CreateTableString(2);

	string tt_strMName;
	sprintf(t_ch, "%s", pClass._name.c_str());
	tt_strMName = t_ch;

	strclass.append(strTable_1);
	strclass.append("local InStream = CS.InOutStream.InStream\n");
	strclass.append(strTable_1);
	strclass.append("local OutStream = CS.InOutStream.OutStream\n");

	strclass.append("\n");

	vector<string> tt_PacketNameList;
	sprintf(t_ch, "local ID_%s = require \"%s%s/ID_%s\"", pNamespace._protocolname.c_str(), m_strLuaPackage.c_str()
		, pNamespace._protocolname.c_str(), pNamespace._protocolname.c_str());
	tt_PacketNameList.push_back(t_ch);


	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		if (filetype == 2 && ((m_bSCSpliteS && pClass._FuncVec[i]._SCSpliteType == 1) || (m_bSCSpliteC && pClass._FuncVec[i]._SCSpliteType == -1)))
			continue;

		for (size_t j = 0; j < pClass._FuncVec[i]._InParVec.size(); j++) {
			if (!IsLuaBaseType(pClass._FuncVec[i]._InParVec[j]._strParType, false)) {
				GetUsePacketNamesByParType_Lua(pClass._FuncVec[i]._InParVec[j]._strParType, tt_PacketNameList);
			}
		}
	}

	for (size_t i = 0; i < tt_PacketNameList.size(); i++) {
		strclass.append(strTable_1);
		strclass.append(tt_PacketNameList[i].c_str());
		strclass.append("\n");
	}

	if (tt_PacketNameList.size() > 0) {
		strclass.append("\n");
	}



	strclass.append(strTable_1);
	sprintf(t_ch, "local %s = {}\n", tt_strMName.c_str());
	strclass.append(t_ch);
	strclass.append("\n");

	fwrite(strclass.c_str(), strclass.length(), 1, fp);
	strclass = "";





	fwrite(strclass.c_str(), strclass.length(), 1, fp);
	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		if (filetype == 2 && ((m_bSCSpliteS && pClass._FuncVec[i]._SCSpliteType == 1) || (m_bSCSpliteC && pClass._FuncVec[i]._SCSpliteType == -1)))
			continue;
		//写函数
		WriteFunc_SendByteSeq_Lua(pClass._FuncVec[i], 0, pClass, pNamespace, foldername, fp);
	}


	strclass = "";
	strclass.append(strTable_1);
	sprintf(t_ch, "return %s\n", tt_strMName.c_str());
	strclass.append(t_ch);
	fwrite(strclass.c_str(), strclass.length(), 1, fp);

	fclose(fp);

}
void    ProtocolParse_Writer_Lua::WriteFunc_SendByteSeq_Lua(stFunc& pFunc, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp)
{
	char t_ch[1024];


	string strTable_0 = CreateTableString(tablenum);
	string strTable_1 = CreateTableString(tablenum + 1);
	string strTable_2 = CreateTableString(tablenum + 2);

	string strfunc;

	string tt_strMName;
	sprintf(t_ch, "%s", pClass._name.c_str());
	tt_strMName = t_ch;


	strfunc.append(WriteComment_Lua(strTable_0, pFunc._comment, "", false).c_str());
	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		strfunc.append(WriteComment_Lua(strTable_0, pFunc._InParVec[i]._comment, pFunc._InParVec[i]._strParName, false).c_str());
	}


	strfunc.append(strTable_0);
	sprintf(t_ch, "function  %s.Send_%s(", tt_strMName.c_str(), pFunc._name.c_str());
	strfunc.append(t_ch);

	//有参
	if (pFunc._InParVec.size() > 0) {
		for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
			if (i == pFunc._InParVec.size() - 1) {
				sprintf(t_ch, "%s", pFunc._InParVec[i]._strParName.c_str());
				strfunc.append(t_ch);
			}
			else {
				sprintf(t_ch, "%s,", pFunc._InParVec[i]._strParName.c_str());
				strfunc.append(t_ch);
			}

		}

	}

	strfunc.append(")\n");


	strfunc.append(strTable_1);
	strfunc.append("local inStream = InStream.New()\n");

	//先写协议号
	strfunc.append(strTable_1);
	sprintf(t_ch, "inStream:WriteInt(ID_%s.%s)\n", pNamespace._protocolname.c_str(), m_ProtocolNo._name.c_str());
	strfunc.append(t_ch);

	//写版本号

	strfunc.append(strTable_1);
	sprintf(t_ch, "local tt_version = %d\n", pFunc._version);
	strfunc.append(t_ch);
	strfunc.append(strTable_1);
	strfunc.append("inStream:WriteShort(tt_version)\n");

	//再写函数号
	strfunc.append(strTable_1);
	sprintf(t_ch, "inStream:WriteInt(ID_%s.ID_%s_%s)\n", pNamespace._protocolname.c_str(), pClass._name.c_str(), pFunc._name.c_str());
	strfunc.append(t_ch);

	//
	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		strfunc.append(strTable_1);
		if (IsLuaBaseType(pFunc._InParVec[i]._strParType, true)) {
			if (IsExType_enum(pFunc._InParVec[i]._strParType)) {
				sprintf(t_ch, "%s.write(inStream,%s)\n", pFunc._InParVec[i]._strParType.c_str(), pFunc._InParVec[i]._strParName.c_str());
			}
			else
				sprintf(t_ch, "inStream:%s(%s)\n", TypeConvertToWriteLuaType(pFunc._InParVec[i]._strParType.c_str()).c_str(), pFunc._InParVec[i]._strParName.c_str());

		}
		else {
			sprintf(t_ch, "%s:write(inStream)\n", pFunc._InParVec[i]._strParName.c_str());

		}
		strfunc.append(t_ch);
	}

	strfunc.append(strTable_1);
	strfunc.append("return inStream\n");
	strfunc.append(strTable_0);
	strfunc.append("end\n");
	strfunc.append("\n");

	fwrite(strfunc.c_str(), strfunc.length(), 1, fp);
}
void    ProtocolParse_Writer_Lua::GetUsePacketNamesByParType_Lua(const string& strParType, vector<string>& packetNamesList, bool bOnlyOneLevel)
{
	for (int i = 0; i < (int)m_NamespaceVec.size(); i++) {
		for (int j = 0; j < (int)m_NamespaceVec[i]._StructVec.size(); j++) {
			if (m_NamespaceVec[i]._StructVec[j]._name.compare(strParType.c_str()) == 0) {
				char tt_ch[256];
				sprintf(tt_ch, "local %s = require \"%s%s/%s\"", m_NamespaceVec[i]._StructVec[j]._name.c_str(),
					m_strLuaPackage.c_str(), m_NamespaceVec[i]._protocolname.c_str(), m_NamespaceVec[i]._StructVec[j]._name.c_str());
				AddPacketNames(packetNamesList, tt_ch);
				if (bOnlyOneLevel)
					return;

				for (int k = 0; k < (int)m_NamespaceVec[i]._StructVec[j]._ParVec.size(); k++) {
					if (!IsLuaBaseType(m_NamespaceVec[i]._StructVec[j]._ParVec[k]._strParType, false)) {
						GetUsePacketNamesByParType_Lua(m_NamespaceVec[i]._StructVec[j]._ParVec[k]._strParType, packetNamesList, bOnlyOneLevel);
					}
				}

				return;
			}
		}

		for (int j = 0; j < (int)m_NamespaceVec[i]._VectorVec.size(); j++) {
			if (m_NamespaceVec[i]._VectorVec[j]._name.compare(strParType.c_str()) == 0) {
				char tt_ch[256];
				sprintf(tt_ch, "local %s = require \"%s%s/%s\"", m_NamespaceVec[i]._VectorVec[j]._name.c_str(),
					m_strLuaPackage.c_str(), m_NamespaceVec[i]._protocolname.c_str(), m_NamespaceVec[i]._VectorVec[j]._name.c_str());
				AddPacketNames(packetNamesList, tt_ch);
				if (bOnlyOneLevel)
					return;

				if (!IsLuaBaseType(m_NamespaceVec[i]._VectorVec[j]._ParTName, false)) {
					GetUsePacketNamesByParType_Lua(m_NamespaceVec[i]._VectorVec[j]._ParTName, packetNamesList, bOnlyOneLevel);
				}



				return;
			}
		}

		for (int j = 0; j < (int)m_NamespaceVec[i]._WJSVectorVec.size(); j++) {
			if (m_NamespaceVec[i]._WJSVectorVec[j]._name.compare(strParType.c_str()) == 0) {
				char tt_ch[256];
				sprintf(tt_ch, "local %s = require \"%s%s/%s\"", m_NamespaceVec[i]._WJSVectorVec[j]._name.c_str(),
					m_strLuaPackage.c_str(), m_NamespaceVec[i]._protocolname.c_str(), m_NamespaceVec[i]._WJSVectorVec[j]._name.c_str());
				AddPacketNames(packetNamesList, tt_ch);
				if (bOnlyOneLevel)
					return;

				if (!IsLuaBaseType(m_NamespaceVec[i]._WJSVectorVec[j]._ParTName, false)) {
					GetUsePacketNamesByParType_Lua(m_NamespaceVec[i]._WJSVectorVec[j]._ParTName, packetNamesList, bOnlyOneLevel);
				}

				return;
			}
		}

		for (int j = 0; j < (int)m_NamespaceVec[i]._EnumVec.size(); j++) {
			if (m_NamespaceVec[i]._EnumVec[j]._name.compare(strParType.c_str()) == 0) {
				char tt_ch[256];
				sprintf(tt_ch, "local %s = require \"%s%s/%s\"", m_NamespaceVec[i]._EnumVec[j]._name.c_str(),
					m_strLuaPackage.c_str(), m_NamespaceVec[i]._protocolname.c_str(), m_NamespaceVec[i]._EnumVec[j]._name.c_str());
				AddPacketNames(packetNamesList, tt_ch);
				return;
			}
		}
	}

	for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++) {
		if (m_pRefProtocolParseList[i])
			((ProtocolParse_Writer_Lua*)m_pRefProtocolParseList[i])->GetUsePacketNamesByParType_Lua(strParType, packetNamesList, bOnlyOneLevel);
	}

}

string  ProtocolParse_Writer_Lua::WriteComment_Lua(const string& strTable, const string& strComment, const string& strPar, bool bWithSummary)
{
	if (strComment.length() == 0)
		return "";
	char tt_ch[2048];

	string strLine;

	vector<string> tt_subList;
	ProtocolUtils::GetAllSubString(strComment, "split", tt_subList, false);

	//if(tt_subList.size()>0 && bWithSummary)
	//{
	//	strLine.append(strTable);
	//	strLine.append("///<summary>\n");
	//}



	for (unsigned int i = 0; i < tt_subList.size(); i++) {
		string tt_strReal;
		//UTF8Decode(tt_subList[i].c_str(),tt_strReal);
		tt_strReal = tt_subList[i];
		strLine.append(strTable);
		if (strPar.length() > 0 && i == 0) {
			sprintf(tt_ch, "--%s  %s\n", strPar.c_str(), tt_strReal.c_str());

		}
		else {
			sprintf(tt_ch, "--%s\n", tt_strReal.c_str());
		}

		strLine.append(tt_ch);

	}

	//if(tt_subList.size()>0 && bWithSummary)
	//{
	//	strLine.append(strTable);
	//	strLine.append("///</summary>\n");
	//}
	return strLine;
}
#pragma region 类型转换与判断
bool    ProtocolParse_Writer_Lua::IsLuaBaseType(const string& strTypeName, bool bWrite)
{
	if (IsBaseType_WriterType(strTypeName, eProtocolWriterType::Lua))
		return true;


	if (bWrite) {
		//枚举类型作为基本类型
		if (IsExType_enum(strTypeName))
			return true;
	}


	return false;
}
bool    ProtocolParse_Writer_Lua::IsLuaBaseType2(string strTypeName)
{
	if (IsBaseType(strTypeName))
		return true;

	return false;

}

string  ProtocolParse_Writer_Lua::TypeConvertToLuaType(const string& strTypeName)
{
	for (int i = 0; i < GetBaseTypeNum(); i++) {
		if (g_strBaseType[i].compare(strTypeName.c_str()) == 0) {
			switch (i) {
			case 0://void
				return strTypeName;
			case 1://int
				return "int";
			case 2://unsigned int
				return "uint";
			case 3://short
				return "short";
			case 4://unsigned short
				return "ushort";
			case 5://char
				return "byte";


			case 11://float
				return "float";
			case 12://long
				return "int";
			case 13://unsigned long
				return "uint";



			case 16://wjs_char32
			case 17://wjs_char64
			case 18://wjs_char128
			case 19://wjs_char256
			case 20://wjs_char512
			case 21://wjs_char1024
				return "String";
				//break;
			case 6://byte
			case 7://BYTE
			case 23:
				return "byte";
			case 9://BOOL
				return "int";
			case 8://bool
				return "bool";
			case 10://DWORD
				return "int";
			case 14://string
			case 24://wjs_utf8
				return "String";
			case 15://__int64
			case 22://long long
				return "long";
			case 25://u16string
				return "String";
			case 26://wstring
				return "String";
			}
			break;

		}
	}

	if (IsExType_enum(strTypeName)) {
		//string tt_temp;
		//tt_temp="enum ";
		//tt_temp.append(str);
		//return tt_temp;
		return "int";
	}

	return strTypeName;

}

string  ProtocolParse_Writer_Lua::TypeConvertToReadLuaType(const string& strTypeName)
{
	for (int i = 0; i < GetBaseTypeNum(); i++) {
		if (g_strBaseType[i].compare(strTypeName.c_str()) == 0) {
			switch (i) {
			case 0://void
				return "ReadInt";
			case 1://int
				return "ReadInt";
			case 2://unsigned int
				return "ReadUInt";
			case 3://short
				return "ReadShort";
			case 4://unsigned short
				return "ReadUShort";
			case 5://char
				return "ReadLocalByte";


			case 11://float
				return "ReadFloat";
			case 12://long
				return "ReadInt";
			case 13://unsigned long
				return "ReadUInt";



			case 16://wjs_char32
			case 17://wjs_char64
			case 18://wjs_char128
			case 19://wjs_char256
			case 20://wjs_char512
			case 21://wjs_char1024
				return "ReadString";
				//break;
			case 6://byte
			case 7://BYTE
			case 23:
				return "ReadLocalByte";
			case 9://BOOL
				return "ReadInt";
			case 8://bool
				return "ReadBoolean";
			case 10://DWORD
				return "ReadInt";
			case 14://string
				return "ReadString";
			case 24://wjs_utf8
				return "ReadUtf8";
			case 15://__int64
			case 22://long long
				return "ReadLong";
			case 25://u16string
				return "ReadUtf8";
			case 26://wstring
				return "ReadUtf8";
			}
			break;

		}
	}

	if (IsExType_enum(strTypeName)) {
		return "ReadInt";
	}

	return "";
}


string  ProtocolParse_Writer_Lua::TypeConvertToWriteLuaType(const string& strTypeName)
{
	for (int i = 0; i < GetBaseTypeNum(); i++) {
		if (g_strBaseType[i].compare(strTypeName.c_str()) == 0) {
			switch (i) {
			case 0://void
				return "WriteInt";
			case 1://int
				return "WriteInt";
			case 2://unsigned int
				return "WriteUInt";
			case 3://short
				return "WriteShort";
			case 4://unsigned short
				return "WriteUShort";
			case 5://char
				return "WriteByte";


			case 11://float
				return "WriteFloat";
			case 12://long
				return "WriteInt";
			case 13://unsigned long
				return "WriteUInt";



			case 16://wjs_char32
			case 17://wjs_char64
			case 18://wjs_char128
			case 19://wjs_char256
			case 20://wjs_char512
			case 21://wjs_char1024
				return "WriteString";
				//break;
			case 6://byte
			case 7://BYTE
			case 23:
				return "WriteByte";
			case 9://BOOL
				return "WriteInt";
			case 8://bool
				return "WriteBoolean";
			case 10://DWORD
				return "WriteInt";
			case 14://string
				return "WriteString";
			case 24://wjs_utf8
				return "WriteUtf8";
			case 15://__int64
			case 22://long long
				return "WriteLong";
			case 25://u16string
				return "WriteUtf8";
			case 26://wstring
				return "WriteUtf8";
			}
			break;

		}
	}

	if (IsExType_enum(strTypeName)) {
		return "WriteInt";
	}

	return "";
}
#pragma endregion




