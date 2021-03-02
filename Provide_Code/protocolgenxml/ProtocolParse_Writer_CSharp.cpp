#include "ProtocolParse_Writer_CSharp.h"
#include "ProtocolParse.h"
#include <time.h>
#include <sys/timeb.h>

extern string g_strBaseType[];

////////////////
//CSharp
///////////////
ProtocolParse_Writer_CSharp::ProtocolParse_Writer_CSharp(const string& strPathName)
	:ProtocolParse_Writer(strPathName, eProtocolWriterType::CSharp)
	,m_bWithLog(false)
{

}

ProtocolParse_Writer_CSharp::~ProtocolParse_Writer_CSharp()
{

}
bool    ProtocolParse_Writer_CSharp::CreateFile_Path(string path, int filetype, bool brefprotocol)
{
	return CreateFile_Path_CSharp(path, filetype, brefprotocol);
}

ProtocolParse_Writer* ProtocolParse_Writer_CSharp::Create_RefProtocolParse_Writer(const string& strPathName)
{
	return new ProtocolParse_Writer_CSharp(strPathName);
}
bool    ProtocolParse_Writer_CSharp::CreateFile_Path_CSharp(string path, int filetype, bool brefprotocol)
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
		return CreateFile_CSharp(str1, filetype, brefprotocol);
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
		return CreateFile_CSharp(str1, filetype, brefprotocol);
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
		return CreateFile_CSharp(str1, filetype, brefprotocol);
	}
	break;
	default:
		return false;
		break;
	}
	//return false;

}

bool    ProtocolParse_Writer_CSharp::CreateFile_CSharp(string foldername, int filetype, bool bRefProtocol)
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
		WriteOneNameSpace_CSharp(m_NamespaceVec[i], filetype, foldername);
	}


	return true;
}

bool    ProtocolParse_Writer_CSharp::WriteOneNameSpace_CSharp(stNamespace& pNamespace, int filetype, string foldername)
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
	CreateAndWriteFuncIdAndProtocolNo_CSharp(pNamespace, foldername, tt_strWriteNameSpace);


	//先写枚举
	for (size_t i = 0; i < pNamespace._EnumVec.size(); i++) {
		WriteEnum_CSharp(pNamespace, pNamespace._EnumVec[i], foldername, tt_strWriteNameSpace);
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
				WriteStruct_CSharp(pNamespace, pNamespace._StructVec[structindex], foldername, tt_strWriteNameSpace);
				structindex++;
			}
			else if (pNamespace._VectorVec[vectorindex]._index < pNamespace._StructVec[structindex]._index
				&& (pNamespace._VectorVec[vectorindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index)) {
				//写vector
				WriteVector_CSharp(pNamespace, pNamespace._VectorVec[vectorindex], foldername, tt_strWriteNameSpace);
				vectorindex++;
			}
			else {
				//写类vector
				WriteWJSVector_CSharp(pNamespace, pNamespace._WJSVectorVec[wjsvectorindex], foldername, tt_strWriteNameSpace);
				wjsvectorindex++;
			}
		}
		else if (!bvectorend && (!bstructend)) {
			if (pNamespace._StructVec[structindex]._index < pNamespace._VectorVec[vectorindex]._index) {
				//写结构体
				WriteStruct_CSharp(pNamespace, pNamespace._StructVec[structindex], foldername, tt_strWriteNameSpace);
				structindex++;
			}
			else {
				//写vector
				WriteVector_CSharp(pNamespace, pNamespace._VectorVec[vectorindex], foldername, tt_strWriteNameSpace);
				vectorindex++;
			}
		}
		else if (!bvectorend && (!bwjsvectorend)) {
			if (pNamespace._VectorVec[vectorindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index) {
				//写vector
				WriteVector_CSharp(pNamespace, pNamespace._VectorVec[vectorindex], foldername, tt_strWriteNameSpace);
				vectorindex++;
			}
			else {
				//写类vector
				WriteWJSVector_CSharp(pNamespace, pNamespace._WJSVectorVec[wjsvectorindex], foldername, tt_strWriteNameSpace);
				wjsvectorindex++;
			}
		}
		else if ((!bstructend) && (!bwjsvectorend)) {
			if (pNamespace._StructVec[structindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index) {
				//写结构体
				WriteStruct_CSharp(pNamespace, pNamespace._StructVec[structindex], foldername, tt_strWriteNameSpace);
				structindex++;
			}
			else {
				//写类vector
				WriteWJSVector_CSharp(pNamespace, pNamespace._WJSVectorVec[wjsvectorindex], foldername, tt_strWriteNameSpace);
				wjsvectorindex++;
			}
		}
		else if (!bvectorend) {
			//写vector
			WriteVector_CSharp(pNamespace, pNamespace._VectorVec[vectorindex], foldername, tt_strWriteNameSpace);
			vectorindex++;

		}
		else if (!bstructend) {
			//写结构体
			WriteStruct_CSharp(pNamespace, pNamespace._StructVec[structindex], foldername, tt_strWriteNameSpace);
			structindex++;
		}
		else if (!bwjsvectorend) {
			//写类vector
			WriteWJSVector_CSharp(pNamespace, pNamespace._WJSVectorVec[wjsvectorindex], foldername, tt_strWriteNameSpace);
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
		WriteClass_CSharp(pNamespace._ClassVec[i], filetype, pNamespace, foldername, tt_strWriteNameSpace);
	}

	//写结束的中括号}
	//WriteEndZhongKuoKao(fp);
	return true;
}
void    ProtocolParse_Writer_CSharp::CreateAndWriteFuncIdAndProtocolNo_CSharp(stNamespace& pNamespace, string foldername, const string& strWriteNameSpace)
{
	if (pNamespace._ClassVec.size() == 0)
		return;

	char t_ch[512];
	sprintf(t_ch, "%s/ID_%s.cs", foldername.c_str(), pNamespace._protocolname.c_str());

	//先创建文件
	FILE* fp = fopen(t_ch, "w");
	if (!fp)
		return;

	//写信息头,共用
	WriteInfoHead(fp);

	string strid;

	string strTable_0 = CreateTableString(1);
	string strTable_1 = CreateTableString(2);


	sprintf(t_ch, "namespace %s\n", strWriteNameSpace.c_str());
	strid.append(t_ch);
	strid.append("{\n");

	strid.append(strTable_0);
	sprintf(t_ch, "public class ID_%s\n", pNamespace._protocolname.c_str());
	strid.append(t_ch);

	strid.append(strTable_0);
	strid.append("{\n");

	//函数id
	for (size_t i = 0; i < pNamespace._ClassVec.size(); i++) {
		for (size_t j = 0; j < pNamespace._ClassVec[i]._FuncVec.size(); j++) {
			strid.append(strTable_1);
			strid.append("public  const  int ");
			sprintf(t_ch, "ID_%s_%s", pNamespace._ClassVec[i]._name.c_str(), pNamespace._ClassVec[i]._FuncVec[j]._name.c_str());
			strid.append(t_ch);


			int tempId;  //将字符串转成id
			if (pNamespace._ClassVec[i]._FuncVec[j]._userFuncId != 0)
				tempId = pNamespace._ClassVec[i]._FuncVec[j]._userFuncId;
			else
				tempId = CovertStringToInt(t_ch, pNamespace._ClassVec[i]._FuncVec[j]._bNew);
			sprintf(t_ch, "=%d;\n", tempId);
			strid.append(t_ch);
		}
	}

	//协议号
	if (m_ProtocolNo._value != 1) {
		//协议头
		strid.append(strTable_1);
		strid.append("public   const int ");
		sprintf(t_ch, "%s=%d;\n", m_ProtocolNo._name.c_str(), m_ProtocolNo._value);
		strid.append(t_ch);
	}
	strid.append(strTable_0);
	strid.append("}\n");

	strid.append("}\n");

	fwrite(strid.c_str(), strid.length(), 1, fp);

	fclose(fp);
}
void    ProtocolParse_Writer_CSharp::WriteEnum_CSharp(stNamespace& pNamespace, stEnum& pEnum, string foldername, const string& strWriteNameSpace)
{
	char t_ch[512];
	sprintf(t_ch, "%s/%s.cs", foldername.c_str(), pEnum._name.c_str());

	//先创建文件
	FILE* fp = fopen(t_ch, "w");
	if (!fp)
		return;

	//写信息头,共用
	WriteInfoHead(fp);


	string strTable_1 = CreateTableString(1);
	string strTable_2 = CreateTableString(2);
	string strTable_3 = CreateTableString(3);
	string strTable_4 = CreateTableString(4);
	string strTable_5 = CreateTableString(5);
	//string strTable_6=CreateTableString(5);

	string strEnum;


	strEnum.append("using System;\n");
	strEnum.append("using InOutStream;\n");
	sprintf(t_ch, "namespace %s\n", strWriteNameSpace.c_str());
	strEnum.append(t_ch);
	strEnum.append("{\n");

	strEnum.append(WriteComment_CSharp(strTable_1, pEnum._comment, "").c_str());

	strEnum.append(strTable_1);
	sprintf(t_ch, "public class %s\n", pEnum._name.c_str());
	strEnum.append(t_ch);
	strEnum.append(strTable_1);
	strEnum.append("{\n");


	for (size_t i = 0; i < pEnum._ParNameList.size(); i++) {
		strEnum.append(WriteComment_CSharp(strTable_2, pEnum._commentList[i], "").c_str());
		strEnum.append(strTable_2);
		sprintf(t_ch, "public  const int %s = %d;\n", pEnum._ParNameList[i].c_str(), i/*CovertStringToInt(pEnum._ParNameList[i])*/);
		strEnum.append(t_ch);
	}

	//写读
	strEnum.append(strTable_2);
	strEnum.append("public static int read(OutStream outStream)\n");

	//
	strEnum.append(strTable_2);
	strEnum.append("{\n");

	strEnum.append(strTable_3);
	strEnum.append("int tt_a = outStream.ReadInt();\n");

	strEnum.append(strTable_3);
	strEnum.append("switch(tt_a)\n");

	strEnum.append(strTable_3);
	strEnum.append("{\n");

	for (size_t i = 0; i < pEnum._ParNameList.size(); i++) {
		if (pEnum._bNewVersion && (!pEnum._keepOldList[i])) {
			strEnum.append(strTable_4);
			sprintf(t_ch, "case %d:\n", i);
			strEnum.append(t_ch);

			strEnum.append(strTable_5);
			sprintf(t_ch, "return %s;\n", pEnum._ParNameList[i].c_str());
			strEnum.append(t_ch);
		}
		else {
			strEnum.append(strTable_4);
			sprintf(t_ch, "case %d:\n", CovertStringToInt(pEnum._ParNameList[i]));
			strEnum.append(t_ch);

			strEnum.append(strTable_5);
			sprintf(t_ch, "return %s;\n", pEnum._ParNameList[i].c_str());
			strEnum.append(t_ch);
		}

	}

	if (pEnum._ParNameList.size() > 0) {
		strEnum.append(strTable_4);
		strEnum.append("default:\n");

		strEnum.append(strTable_5);
		sprintf(t_ch, "return %s;\n", pEnum._ParNameList[0].c_str());
		strEnum.append(t_ch);
	}

	strEnum.append(strTable_3);
	strEnum.append("}\n");

	//strEnum.append(strTable_3);
	//strEnum.append("return tt_a;\n");

	strEnum.append(strTable_2);
	strEnum.append("}\n");


	strEnum.append(strTable_2);
	strEnum.append("public static void write(InStream inStream,int va)\n");

	strEnum.append(strTable_2);
	strEnum.append("{\n");

	strEnum.append(strTable_3);
	strEnum.append("switch(va)\n");

	strEnum.append(strTable_3);
	strEnum.append("{\n");


	for (size_t i = 0; i < pEnum._ParNameList.size(); i++) {
		if (pEnum._bNewVersion && (!pEnum._keepOldList[i])) {
			strEnum.append(strTable_4);
			sprintf(t_ch, "case %s:\n", pEnum._ParNameList[i].c_str());
			strEnum.append(t_ch);

			strEnum.append(strTable_5);
			sprintf(t_ch, "inStream.WriteInt(%d);\n", i);
			strEnum.append(t_ch);

			strEnum.append(strTable_5);
			strEnum.append("break;\n");
		}
		else {
			strEnum.append(strTable_4);
			sprintf(t_ch, "case %s:\n", pEnum._ParNameList[i].c_str());
			strEnum.append(t_ch);

			strEnum.append(strTable_5);
			sprintf(t_ch, "inStream.WriteInt(%d);\n", GetExistStringToIntValue(pEnum._ParNameList[i]));
			strEnum.append(t_ch);

			strEnum.append(strTable_5);
			strEnum.append("break;\n");
		}

	}





	strEnum.append(strTable_4);
	strEnum.append("default:\n");

	strEnum.append(strTable_5);
	strEnum.append("inStream.WriteInt(0);\n");

	strEnum.append(strTable_5);
	strEnum.append("break;\n");

	strEnum.append(strTable_3);
	strEnum.append("}\n");

	strEnum.append(strTable_2);
	strEnum.append("}\n");

	strEnum.append(strTable_1);
	strEnum.append("}\n");

	strEnum.append("}\n");

	fwrite(strEnum.c_str(), strEnum.length(), 1, fp);

	fclose(fp);
}

void    ProtocolParse_Writer_CSharp::WriteStruct_CSharp(stNamespace& pNamespace, stStructStruct& pStruct, string foldername, const string& strWriteNameSpace)
{
	if (pStruct._inherited.length() > 0)
		return;

	char t_ch[512];
	sprintf(t_ch, "%s/%s.cs", foldername.c_str(), pStruct._name.c_str());

	//先创建文件
	FILE* fp = fopen(t_ch, "w");
	if (!fp)
		return;

	//写信息头,共用
	WriteInfoHead(fp);


	string strTable_1 = CreateTableString(1);
	string strTable_2 = CreateTableString(2);
	string strTable_3 = CreateTableString(3);
	string strTable_4 = CreateTableString(4);
	string strTable_5 = CreateTableString(5);
	string strTable_6 = CreateTableString(6);

	string strStruct;

	strStruct.append("using System;\n");
	strStruct.append("using InOutStream;\n");

	vector<string> tt_usePacketNameList;
	GetUsePacketNamesByRefHead_CSharp(tt_usePacketNameList);
	for (int i = 0; i < (int)tt_usePacketNameList.size(); i++) {
		sprintf(t_ch, "using %s;\n", tt_usePacketNameList[i].c_str());
		strStruct.append(t_ch);
	}

	sprintf(t_ch, "namespace %s\n", strWriteNameSpace.c_str());
	strStruct.append(t_ch);
	strStruct.append("{\n");



	strStruct.append(WriteComment_CSharp(strTable_1, pStruct._comment, "").c_str());
	strStruct.append(strTable_1);
	sprintf(t_ch, "public class %s\n", pStruct._name.c_str());
	strStruct.append(t_ch);

	strStruct.append(strTable_1);
	strStruct.append("{\n");


	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		strStruct.append(WriteComment_CSharp(strTable_2, pStruct._ParVec[i]._comment, "").c_str());
		strStruct.append(strTable_2);
		if (IsExType_vector(pStruct._ParVec[i]._strParType)) {
			sprintf(t_ch, "public %s %s;\n", TypeConvertToCSharpType(pStruct._ParVec[i]._strParType.c_str()).c_str(), pStruct._ParVec[i]._strParName.c_str());
		}
		else {
			sprintf(t_ch, "public %s %s;\n", TypeConvertToCSharpType(pStruct._ParVec[i]._strParType.c_str()).c_str(), pStruct._ParVec[i]._strParName.c_str());
		}
		strStruct.append(t_ch);
	}

	//写读
	strStruct.append(strTable_2);
	sprintf(t_ch, "public static %s read(OutStream outStream)\n", pStruct._name.c_str());
	strStruct.append(t_ch);
	strStruct.append(strTable_2);
	strStruct.append("{\n");

	strStruct.append(strTable_3);
	strStruct.append("short tt_len =0;\n");

	strStruct.append(strTable_3);
	strStruct.append("tt_len = outStream.ReadShort();\n");

	strStruct.append(strTable_3);
	strStruct.append("short tt_version=0;\n");

	strStruct.append(strTable_3);
	strStruct.append("tt_version = outStream.ReadShort();\n");

	if (pStruct.IsHasTransNeedValuePar()) {
		strStruct.append(strTable_3);
		strStruct.append("int tt_logsize1 = outStream.GetPos();\n");
	}


	if (pStruct.IsHasTransNeedValuePar()) {
		strStruct.append(strTable_3);
		strStruct.append("int tt_transNeedValue=0;\n");

		strStruct.append(strTable_3);
		strStruct.append("tt_transNeedValue = outStream.ReadInt();\n");
	}


	strStruct.append(strTable_3);
	sprintf(t_ch, "%s o = new %s();\n", pStruct._name.c_str(), pStruct._name.c_str());
	strStruct.append(t_ch);

	strStruct.append(strTable_3);
	sprintf(t_ch, "if(tt_version == %d || (tt_version==0))\n", pStruct._version);
	strStruct.append(t_ch);

	strStruct.append(strTable_3);
	strStruct.append("{\n");



	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		if (pStruct.IsHasTransNeedValuePar() && (pStruct._ParVec[i]._transNeedValue != -1)) {
			strStruct.append(strTable_4);
			sprintf(t_ch, "if((tt_transNeedValue & (1<<%d))!=0)\n", pStruct._ParVec[i]._transNeedValue);
			strStruct.append(t_ch);
			strStruct.append(strTable_4);
			strStruct.append("{\n");

			strStruct.append(strTable_5);
			if (IsCSharpBaseType(pStruct._ParVec[i]._strParType, false)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "o.%s = %s.read(outStream);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				else {
					sprintf(t_ch, "o.%s = outStream.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadCSharpType(pStruct._ParVec[i]._strParType.c_str()).c_str());
				}
			}
			else {
				sprintf(t_ch, "o.%s = %s.read(outStream);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
			}
			strStruct.append(t_ch);

			strStruct.append(strTable_4);
			strStruct.append("}\n");
			strStruct.append(strTable_4);
			strStruct.append("else\n");
			strStruct.append(strTable_4);
			strStruct.append("{\n");
			strStruct.append(strTable_5);
			sprintf(t_ch, "o.%s=%s;\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strDefaultValue.c_str());
			strStruct.append(t_ch);
			strStruct.append(strTable_4);
			strStruct.append("}\n");
		}
		else {
			strStruct.append(strTable_4);
			if (IsCSharpBaseType(pStruct._ParVec[i]._strParType, false)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "o.%s = %s.read(outStream);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				else {
					sprintf(t_ch, "o.%s = outStream.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadCSharpType(pStruct._ParVec[i]._strParType.c_str()).c_str());
				}
			}
			else {
				sprintf(t_ch, "o.%s = %s.read(outStream);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
			}
			strStruct.append(t_ch);
		}

	}
	strStruct.append(strTable_3);
	strStruct.append("}\n");

	strStruct.append(strTable_3);
	sprintf(t_ch, "else if(tt_version > %d)\n", pStruct._version);
	strStruct.append(t_ch);

	strStruct.append(strTable_3);
	strStruct.append("{\n");

	if (!pStruct.IsHasTransNeedValuePar()) {
		strStruct.append(strTable_4);
		strStruct.append("int tt_logsize1 = outStream.GetPos();\n");
	}


	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		if (pStruct.IsHasTransNeedValuePar() && (pStruct._ParVec[i]._transNeedValue != -1)) {
			strStruct.append(strTable_4);
			sprintf(t_ch, "if((tt_transNeedValue & (1<<%d))!=0)\n", pStruct._ParVec[i]._transNeedValue);
			strStruct.append(t_ch);
			strStruct.append(strTable_4);
			strStruct.append("{\n");

			strStruct.append(strTable_5);
			if (IsCSharpBaseType(pStruct._ParVec[i]._strParType, false)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "o.%s = %s.read(outStream);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				else {
					sprintf(t_ch, "o.%s = outStream.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadCSharpType(pStruct._ParVec[i]._strParType.c_str()).c_str());
				}
			}
			else {
				sprintf(t_ch, "o.%s = %s.read(outStream);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
			}
			strStruct.append(t_ch);

			strStruct.append(strTable_4);
			strStruct.append("}\n");
			strStruct.append(strTable_4);
			strStruct.append("else\n");
			strStruct.append(strTable_4);
			strStruct.append("{\n");
			strStruct.append(strTable_5);
			sprintf(t_ch, "o.%s=%s;\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strDefaultValue.c_str());
			strStruct.append(t_ch);
			strStruct.append(strTable_4);
			strStruct.append("}\n");
		}
		else {
			strStruct.append(strTable_4);
			if (IsCSharpBaseType(pStruct._ParVec[i]._strParType, false)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "o.%s = %s.read(outStream);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				else {
					sprintf(t_ch, "o.%s = outStream.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadCSharpType(pStruct._ParVec[i]._strParType.c_str()).c_str());
				}
			}
			else {
				sprintf(t_ch, "o.%s = %s.read(outStream);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
			}
			strStruct.append(t_ch);
		}


	}

	strStruct.append(strTable_4);
	strStruct.append("int tt_logsize2 = outStream.GetPos();\n");

	strStruct.append(strTable_4);
	strStruct.append("int tt_offset = tt_len - (tt_logsize2 - tt_logsize1);\n");

	strStruct.append(strTable_4);
	strStruct.append("if(tt_offset>0)\n");

	strStruct.append(strTable_4);
	strStruct.append("{\n");

	strStruct.append(strTable_5);
	strStruct.append("outStream.Offset(tt_offset);\n");

	strStruct.append(strTable_4);
	strStruct.append("}\n");


	strStruct.append(strTable_3);
	strStruct.append("}\n");

	strStruct.append(strTable_3);
	strStruct.append("else\n");

	strStruct.append(strTable_3);
	strStruct.append("{\n");

	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		strStruct.append(strTable_4);
		sprintf(t_ch, "if(tt_version>=%d)\n", pStruct._ParVec[i]._version);
		strStruct.append(t_ch);

		strStruct.append(strTable_4);
		strStruct.append("{\n");

		if (pStruct.IsHasTransNeedValuePar() && (pStruct._ParVec[i]._transNeedValue != -1)) {
			strStruct.append(strTable_5);
			sprintf(t_ch, "if((tt_transNeedValue & (1<<%d))!=0)\n", pStruct._ParVec[i]._transNeedValue);
			strStruct.append(t_ch);
			strStruct.append(strTable_5);
			strStruct.append("{\n");

			strStruct.append(strTable_6);
			if (IsCSharpBaseType(pStruct._ParVec[i]._strParType, false)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "o.%s = %s.read(outStream);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				else {
					sprintf(t_ch, "o.%s = outStream.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadCSharpType(pStruct._ParVec[i]._strParType.c_str()).c_str());
				}
			}
			else {
				sprintf(t_ch, "o.%s = %s.read(outStream);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
			}
			strStruct.append(t_ch);

			strStruct.append(strTable_5);
			strStruct.append("}\n");
			strStruct.append(strTable_5);
			strStruct.append("else\n");
			strStruct.append(strTable_5);
			strStruct.append("{\n");
			strStruct.append(strTable_6);
			sprintf(t_ch, "o.%s=%s;\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strDefaultValue.c_str());
			strStruct.append(t_ch);
			strStruct.append(strTable_5);
			strStruct.append("}\n");
		}
		else {
			strStruct.append(strTable_5);
			if (IsCSharpBaseType(pStruct._ParVec[i]._strParType, false)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "o.%s = %s.read(outStream);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				else {
					sprintf(t_ch, "o.%s = outStream.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadCSharpType(pStruct._ParVec[i]._strParType.c_str()).c_str());
				}
			}
			else {
				sprintf(t_ch, "o.%s = %s.read(outStream);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
			}
			strStruct.append(t_ch);
		}



		strStruct.append(strTable_4);
		strStruct.append("}\n");

		string tt_default = GetDefaultValue(pStruct._ParVec[i]._strParType, false, false, true);
		if (tt_default.length() > 0) {
			strStruct.append(strTable_4);
			strStruct.append("else\n");

			strStruct.append(strTable_5);
			sprintf(t_ch, "o.%s=%s;\n", pStruct._ParVec[i]._strParName.c_str(), tt_default.c_str());
			strStruct.append(t_ch);
		}
	}

	strStruct.append(strTable_3);
	strStruct.append("}\n");


	strStruct.append(strTable_3);
	strStruct.append("return o;\n");

	strStruct.append(strTable_2);
	strStruct.append("}\n");

	////////////////
	//
	///////////////
	//if(pStruct._name.compare("FrozenChoice")==0)
	//{
	//	int a;
	//	a=10;

	//}

	//写读
	strStruct.append(strTable_2);
	sprintf(t_ch, "public void readLocal(OutStream outStream)\n");
	strStruct.append(t_ch);
	strStruct.append(strTable_2);
	strStruct.append("{\n");

	strStruct.append(strTable_3);
	strStruct.append("short tt_len =0;\n");

	strStruct.append(strTable_3);
	strStruct.append("tt_len = outStream.ReadShort();\n");

	strStruct.append(strTable_3);
	strStruct.append("short tt_version=0;\n");

	strStruct.append(strTable_3);
	strStruct.append("tt_version = outStream.ReadShort();\n");

	if (pStruct.IsHasTransNeedValuePar()) {
		strStruct.append(strTable_4);
		strStruct.append("int tt_logsize1 = outStream.GetPos();\n");
	}

	if (pStruct.IsHasTransNeedValuePar()) {
		strStruct.append(strTable_3);
		strStruct.append("int tt_transNeedValue=0;\n");

		strStruct.append(strTable_3);
		strStruct.append("tt_transNeedValue = outStream.ReadInt();\n");
	}


	//strStruct.append(strTable_3);
	//sprintf(t_ch,"%s o = new %s();\n",pStruct._name.c_str(),pStruct._name.c_str());
	//strStruct.append(t_ch);

	strStruct.append(strTable_3);
	sprintf(t_ch, "if(tt_version == %d || (tt_version==0))\n", pStruct._version);
	strStruct.append(t_ch);

	strStruct.append(strTable_3);
	strStruct.append("{\n");



	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		if (pStruct.IsHasTransNeedValuePar() && (pStruct._ParVec[i]._transNeedValue != -1)) {
			strStruct.append(strTable_4);
			sprintf(t_ch, "if((tt_transNeedValue & (1<<%d))!=0)\n", pStruct._ParVec[i]._transNeedValue);
			strStruct.append(t_ch);
			strStruct.append(strTable_4);
			strStruct.append("{\n");

			strStruct.append(strTable_5);
			if (IsCSharpBaseType(pStruct._ParVec[i]._strParType, true)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "%s = %s.read(outStream);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				else {
					sprintf(t_ch, "%s = outStream.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadCSharpType(pStruct._ParVec[i]._strParType.c_str()).c_str());
				}
			}
			else {
				sprintf(t_ch, "%s.readLocal(outStream);\n", pStruct._ParVec[i]._strParName.c_str());

			}
			strStruct.append(t_ch);

			strStruct.append(strTable_4);
			strStruct.append("}\n");
			strStruct.append(strTable_4);
			strStruct.append("else\n");
			strStruct.append(strTable_4);
			strStruct.append("{\n");
			strStruct.append(strTable_5);
			sprintf(t_ch, "%s=%s;\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strDefaultValue.c_str());
			strStruct.append(t_ch);
			strStruct.append(strTable_4);
			strStruct.append("}\n");
		}
		else {
			strStruct.append(strTable_4);
			if (IsCSharpBaseType(pStruct._ParVec[i]._strParType, true)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "%s = %s.read(outStream);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				else {
					sprintf(t_ch, "%s = outStream.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadCSharpType(pStruct._ParVec[i]._strParType.c_str()).c_str());
				}
			}
			else {
				sprintf(t_ch, "%s.readLocal(outStream);\n", pStruct._ParVec[i]._strParName.c_str());

			}
			strStruct.append(t_ch);
		}


	}
	strStruct.append(strTable_3);
	strStruct.append("}\n");

	strStruct.append(strTable_3);
	sprintf(t_ch, "else if(tt_version > %d)\n", pStruct._version);
	strStruct.append(t_ch);

	strStruct.append(strTable_3);
	strStruct.append("{\n");

	if (!pStruct.IsHasTransNeedValuePar()) {
		strStruct.append(strTable_4);
		strStruct.append("int tt_logsize1 = outStream.GetPos();\n");
	}


	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		if (pStruct.IsHasTransNeedValuePar() && (pStruct._ParVec[i]._transNeedValue != -1)) {
			strStruct.append(strTable_4);
			sprintf(t_ch, "if((tt_transNeedValue & (1<<%d))!=0)\n", pStruct._ParVec[i]._transNeedValue);
			strStruct.append(t_ch);
			strStruct.append(strTable_4);
			strStruct.append("{\n");

			strStruct.append(strTable_5);
			if (IsCSharpBaseType(pStruct._ParVec[i]._strParType, true)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "%s = %s.read(outStream);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				else {
					sprintf(t_ch, "%s = outStream.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadCSharpType(pStruct._ParVec[i]._strParType.c_str()).c_str());
				}
			}
			else {
				sprintf(t_ch, "%s.readLocal(outStream);\n", pStruct._ParVec[i]._strParName.c_str());

			}
			strStruct.append(t_ch);

			strStruct.append(strTable_4);
			strStruct.append("}\n");
			strStruct.append(strTable_4);
			strStruct.append("else\n");
			strStruct.append(strTable_4);
			strStruct.append("{\n");
			strStruct.append(strTable_5);
			sprintf(t_ch, "%s=%s;\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strDefaultValue.c_str());
			strStruct.append(t_ch);
			strStruct.append(strTable_4);
			strStruct.append("}\n");
		}
		else {
			strStruct.append(strTable_4);
			if (IsCSharpBaseType(pStruct._ParVec[i]._strParType, true)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "%s = %s.read(outStream);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				else {
					sprintf(t_ch, "%s = outStream.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadCSharpType(pStruct._ParVec[i]._strParType.c_str()).c_str());
				}
			}
			else {
				sprintf(t_ch, "%s.readLocal(outStream);\n", pStruct._ParVec[i]._strParName.c_str());

			}
			strStruct.append(t_ch);
		}
	}

	strStruct.append(strTable_4);
	strStruct.append("int tt_logsize2 = outStream.GetPos();\n");

	strStruct.append(strTable_4);
	strStruct.append("int tt_offset = tt_len - (tt_logsize2 - tt_logsize1);\n");

	strStruct.append(strTable_4);
	strStruct.append("if(tt_offset>0)\n");

	strStruct.append(strTable_4);
	strStruct.append("{\n");

	strStruct.append(strTable_5);
	strStruct.append("outStream.Offset(tt_offset);\n");

	strStruct.append(strTable_4);
	strStruct.append("}\n");


	strStruct.append(strTable_3);
	strStruct.append("}\n");

	strStruct.append(strTable_3);
	strStruct.append("else\n");

	strStruct.append(strTable_3);
	strStruct.append("{\n");

	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		strStruct.append(strTable_4);
		sprintf(t_ch, "if(tt_version>=%d)\n", pStruct._ParVec[i]._version);
		strStruct.append(t_ch);

		strStruct.append(strTable_4);
		strStruct.append("{\n");

		if (pStruct.IsHasTransNeedValuePar() && (pStruct._ParVec[i]._transNeedValue != -1)) {
			strStruct.append(strTable_5);
			sprintf(t_ch, "if((tt_transNeedValue & (1<<%d))!=0)\n", pStruct._ParVec[i]._transNeedValue);
			strStruct.append(t_ch);
			strStruct.append(strTable_5);
			strStruct.append("{\n");

			strStruct.append(strTable_6);
			if (IsCSharpBaseType(pStruct._ParVec[i]._strParType, true)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "%s = %s.read(outStream);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				else {
					sprintf(t_ch, "%s = outStream.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadCSharpType(pStruct._ParVec[i]._strParType.c_str()).c_str());
				}
			}
			else {
				sprintf(t_ch, "%s.readLocal(outStream);\n", pStruct._ParVec[i]._strParName.c_str());
			}
			strStruct.append(t_ch);

			strStruct.append(strTable_5);
			strStruct.append("}\n");
			strStruct.append(strTable_5);
			strStruct.append("else\n");
			strStruct.append(strTable_5);
			strStruct.append("{\n");
			strStruct.append(strTable_6);
			sprintf(t_ch, "%s=%s;\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strDefaultValue.c_str());
			strStruct.append(t_ch);
			strStruct.append(strTable_5);
			strStruct.append("}\n");
		}
		else {
			strStruct.append(strTable_5);
			if (IsCSharpBaseType(pStruct._ParVec[i]._strParType, true)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "%s = %s.read(outStream);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				else {
					sprintf(t_ch, "%s = outStream.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadCSharpType(pStruct._ParVec[i]._strParType.c_str()).c_str());
				}
			}
			else {
				sprintf(t_ch, "%s.readLocal(outStream);\n", pStruct._ParVec[i]._strParName.c_str());
			}
			strStruct.append(t_ch);
		}



		strStruct.append(strTable_4);
		strStruct.append("}\n");

		string tt_default = GetDefaultValue(pStruct._ParVec[i]._strParType, false, false, true);
		if (tt_default.length() > 0) {
			strStruct.append(strTable_4);
			strStruct.append("else\n");

			strStruct.append(strTable_5);
			sprintf(t_ch, "%s=%s;\n", pStruct._ParVec[i]._strParName.c_str(), tt_default.c_str());
			strStruct.append(t_ch);
		}
	}

	strStruct.append(strTable_3);
	strStruct.append("}\n");


	//strStruct.append(strTable_3);
	//strStruct.append("return o;\n");

	strStruct.append(strTable_2);
	strStruct.append("}\n");

	//////////////////
	//
	//////////////////

	//写写
	strStruct.append(strTable_2);
	strStruct.append("public void write(InStream inStream)\n");
	strStruct.append(strTable_2);
	strStruct.append("{\n");

	//先写入结构体的大小
	strStruct.append(strTable_3);
	strStruct.append("short tt_len  = 0;\n");
	strStruct.append(strTable_3);
	strStruct.append("int tt_logsize1 = inStream.GetPos();\n");
	strStruct.append(strTable_3);
	strStruct.append("inStream.WriteShort(tt_len);\n");


	//再写入版本
	strStruct.append(strTable_3);
	sprintf(t_ch, "short  tt_version = %d;\n", pStruct._version);
	strStruct.append(t_ch);
	strStruct.append(strTable_3);
	strStruct.append("inStream.WriteShort(tt_version);\n");

	strStruct.append(strTable_3);
	strStruct.append("int tt_logsize2 = inStream.GetPos();\n");

	if (pStruct.IsHasTransNeedValuePar()) {
		strStruct.append(strTable_3);
		strStruct.append("int tt_transNeedValue=0;\n");
		strStruct.append(strTable_3);
		strStruct.append("inStream.WriteInt(tt_transNeedValue);\n");
	}



	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		if (pStruct._ParVec[i]._transNeedValue != -1) {
			strStruct.append(strTable_3);
			sprintf(t_ch, "if(%s!=%s)\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strDefaultValue.c_str());
			strStruct.append(t_ch);
			strStruct.append(strTable_3);
			strStruct.append("{\n");


			strStruct.append(strTable_4);
			if (IsCSharpBaseType(pStruct._ParVec[i]._strParType, true)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "%s.write(inStream,%s);\n", pStruct._ParVec[i]._strParType.c_str(), pStruct._ParVec[i]._strParName.c_str());
				}
				else
					sprintf(t_ch, "inStream.%s(%s);\n", TypeConvertToWriteCSharpType(pStruct._ParVec[i]._strParType.c_str()).c_str(), pStruct._ParVec[i]._strParName.c_str());
			}
			else {
				sprintf(t_ch, "%s.write(inStream);\n", pStruct._ParVec[i]._strParName.c_str());
			}
			strStruct.append(t_ch);


			strStruct.append(strTable_4);
			sprintf(t_ch, "tt_transNeedValue = tt_transNeedValue | (1<<%d);\n", pStruct._ParVec[i]._transNeedValue);
			strStruct.append(t_ch);

			strStruct.append(strTable_3);
			strStruct.append("}\n");
		}
		else {
			strStruct.append(strTable_3);
			if (IsCSharpBaseType(pStruct._ParVec[i]._strParType, true)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "%s.write(inStream,%s);\n", pStruct._ParVec[i]._strParType.c_str(), pStruct._ParVec[i]._strParName.c_str());
				}
				else
					sprintf(t_ch, "inStream.%s(%s);\n", TypeConvertToWriteCSharpType(pStruct._ParVec[i]._strParType.c_str()).c_str(), pStruct._ParVec[i]._strParName.c_str());
			}
			else {
				sprintf(t_ch, "%s.write(inStream);\n", pStruct._ParVec[i]._strParName.c_str());
			}
			strStruct.append(t_ch);
		}


	}
	strStruct.append(strTable_3);
	strStruct.append("int tt_logsize4 = inStream.GetPos();\n");
	strStruct.append(strTable_3);
	strStruct.append("int tt_logsize3 = tt_logsize4 - tt_logsize2;\n");
	strStruct.append(strTable_3);
	strStruct.append("inStream.SetPos(tt_logsize1);\n");
	strStruct.append(strTable_3);
	strStruct.append("inStream.WriteShort((short)tt_logsize3);\n");

	if (pStruct.IsHasTransNeedValuePar()) {
		strStruct.append(strTable_3);
		strStruct.append("if(tt_transNeedValue!=0)\n");
		strStruct.append(strTable_3);
		strStruct.append("{\n");
		strStruct.append(strTable_4);
		strStruct.append("inStream.SetPos(tt_logsize2);\n");
		strStruct.append(strTable_4);
		strStruct.append("inStream.WriteInt(tt_transNeedValue);\n");
		strStruct.append(strTable_3);
		strStruct.append("}\n");
	}


	strStruct.append(strTable_3);
	strStruct.append("inStream.SetPos(tt_logsize4);\n");

	strStruct.append(strTable_2);
	strStruct.append("}\n");



	strStruct.append(strTable_1);
	strStruct.append("}\n");
	strStruct.append("}\n");

	fwrite(strStruct.c_str(), strStruct.length(), 1, fp);

	fclose(fp);
}

void    ProtocolParse_Writer_CSharp::WriteVector_CSharp(stNamespace& pNamespace, stVector& pVector, string foldername, const string& strWriteNameSpace)
{
	if (pVector._ParTName == "byte" || pVector._ParTName == "BYTE") {
		WriteVector2_CSharp(pNamespace, pVector, foldername, strWriteNameSpace);
		return;
	}
	char t_ch[512];
	sprintf(t_ch, "%s/%s.cs", foldername.c_str(), pVector._name.c_str());

	//先创建文件
	FILE* fp = fopen(t_ch, "w");
	if (!fp)
		return;

	//写信息头,共用
	WriteInfoHead(fp);


	//string strTable_0=CreateTableString(1);
	string strTable_1 = CreateTableString(1);
	string strTable_2 = CreateTableString(2);
	string strTable_3 = CreateTableString(3);
	string strTable_4 = CreateTableString(4);

	string strVector;

	strVector.append("using System;\n");
	strVector.append("using System.Collections;\n");
	strVector.append("using System.Collections.Generic;\n");
	strVector.append("using InOutStream;\n");
	sprintf(t_ch, "namespace %s\n", strWriteNameSpace.c_str());
	strVector.append(t_ch);
	strVector.append("{\n");




	/////////////有两种版本，一种是非Flash基础类型，一种是
	bool  tt_bCSharpBaseType = IsCSharpBaseType2(pVector._ParTName);

	strVector.append(WriteComment_CSharp(strTable_1, pVector._comment, "").c_str());

	strVector.append(strTable_1);
	sprintf(t_ch, "public  class %s : List<%s>\n", pVector._name.c_str(), TypeConvertToCSharpType(pVector._ParTName).c_str());
	strVector.append(t_ch);
	strVector.append(strTable_1);
	strVector.append("{\n");

	strVector.append(strTable_2);
	sprintf(t_ch, "public %s()\n", pVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("{\n");

	//strVector.append(strTable_3);
	//strVector.append("super();\n");

	strVector.append(strTable_2);
	strVector.append("}\n");


	strVector.append(strTable_2);
	sprintf(t_ch, "public static %s read(OutStream outStream)\n", pVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	sprintf(t_ch, "%s o = new %s();\n", pVector._name.c_str(), pVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_3);
	strVector.append("int count = outStream.ReadInt();\n");



	strVector.append(strTable_3);
	strVector.append("for(int i=0;i<count;i++)\n");

	strVector.append(strTable_3);
	strVector.append("{\n");

	if (!tt_bCSharpBaseType) {
		strVector.append(strTable_4);
		sprintf(t_ch, "o.Add(%s.read(outStream));\n", pVector._ParTName.c_str());
		strVector.append(t_ch);
	}
	else {
		strVector.append(strTable_4);
		sprintf(t_ch, "o.Add(outStream.%s());\n", TypeConvertToReadCSharpType(pVector._ParTName).c_str());
		strVector.append(t_ch);
	}

	strVector.append(strTable_3);
	strVector.append("}\n");

	strVector.append(strTable_3);
	strVector.append("return o;\n");

	strVector.append(strTable_2);
	strVector.append("}\n");

	////////////////
	//
	////////////////

	strVector.append(strTable_2);
	sprintf(t_ch, "public void readLocal(OutStream outStream)\n");
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("{\n");


	strVector.append(strTable_3);
	strVector.append("this.Clear();\n");


	strVector.append(strTable_3);
	strVector.append("int count = outStream.ReadInt();\n");



	strVector.append(strTable_3);
	strVector.append("for(int i=0;i<count;i++)\n");

	strVector.append(strTable_3);
	strVector.append("{\n");

	if (!tt_bCSharpBaseType) {
		strVector.append(strTable_4);
		sprintf(t_ch, "this.Add(%s.read(outStream));\n", pVector._ParTName.c_str());
		strVector.append(t_ch);
	}
	else {
		strVector.append(strTable_4);
		sprintf(t_ch, "this.Add(outStream.%s());\n", TypeConvertToReadCSharpType(pVector._ParTName).c_str());
		strVector.append(t_ch);
	}

	strVector.append(strTable_3);
	strVector.append("}\n");

	strVector.append(strTable_2);
	strVector.append("}\n");

	/////////////
	//
	////////////
	strVector.append(strTable_2);
	strVector.append("public void write(InStream inStream)\n");

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	strVector.append("inStream.WriteInt(Count);\n");

	strVector.append(strTable_3);
	strVector.append("for(int i=0;i<Count;i++)\n");

	strVector.append(strTable_3);
	strVector.append("{\n");
	//
	if (!tt_bCSharpBaseType) {
		strVector.append(strTable_4);
		sprintf(t_ch, "((%s)(this)[i]).write(inStream);\n", pVector._ParTName.c_str());
		strVector.append(t_ch);
	}
	else {
		strVector.append(strTable_4);
		sprintf(t_ch, "inStream.%s((%s)(this)[i]);\n", TypeConvertToWriteCSharpType(pVector._ParTName).c_str(), TypeConvertToCSharpType(pVector._ParTName).c_str());
		strVector.append(t_ch);
	}


	strVector.append(strTable_3);
	strVector.append("}\n");

	strVector.append(strTable_2);
	strVector.append("}\n");

	strVector.append(strTable_1);
	strVector.append("}\n");

	strVector.append("}\n");



	fwrite(strVector.c_str(), strVector.length(), 1, fp);

	fclose(fp);
}

void    ProtocolParse_Writer_CSharp::WriteVector2_CSharp(stNamespace& pNamespace, stVector& pVector, string foldername, const string& strWriteNameSpace)
{
	char t_ch[512];
	sprintf(t_ch, "%s/%s.cs", foldername.c_str(), pVector._name.c_str());

	//先创建文件
	FILE* fp = fopen(t_ch, "w");
	if (!fp)
		return;

	//写信息头,共用
	WriteInfoHead(fp);


	//string strTable_0=CreateTableString(1);
	string strTable_1 = CreateTableString(1);
	string strTable_2 = CreateTableString(2);
	string strTable_3 = CreateTableString(3);
	string strTable_4 = CreateTableString(4);

	string strVector;

	strVector.append("using System;\n");
	strVector.append("using System.Collections;\n");
	strVector.append("using System.Collections.Generic;\n");
	strVector.append("using InOutStream;\n");
	sprintf(t_ch, "namespace %s\n", strWriteNameSpace.c_str());
	strVector.append(t_ch);
	strVector.append("{\n");




	/////////////有两种版本，一种是非Flash基础类型，一种是
	bool  tt_bCSharpBaseType = IsCSharpBaseType2(pVector._ParTName);

	strVector.append(WriteComment_CSharp(strTable_1, pVector._comment, "").c_str());

	strVector.append(strTable_1);
	sprintf(t_ch, "public  class %s\n", pVector._name.c_str());
	strVector.append(t_ch);
	strVector.append(strTable_1);
	strVector.append("{\n");

	strVector.append(strTable_2);
	strVector.append("public byte[] _bytes;\n");

	strVector.append(strTable_2);
	sprintf(t_ch, "public %s()\n", pVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("{\n");

	//strVector.append(strTable_3);
	//strVector.append("super();\n");

	strVector.append(strTable_2);
	strVector.append("}\n");


	strVector.append(strTable_2);
	sprintf(t_ch, "public static %s read(OutStream outStream)\n", pVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	sprintf(t_ch, "%s o = new %s();\n", pVector._name.c_str(), pVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_3);
	strVector.append("o._bytes = outStream.ReadByteArray();\n");

	strVector.append(strTable_3);
	strVector.append("return o;\n");

	strVector.append(strTable_2);
	strVector.append("}\n");

	strVector.append(strTable_2);
	sprintf(t_ch, "public void readLocal(OutStream outStream)\n");
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	strVector.append("this._bytes = outStream.ReadByteArray();\n");

	strVector.append(strTable_2);
	strVector.append("}\n");

	strVector.append(strTable_2);
	strVector.append("public void write(InStream inStream)\n");

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	strVector.append("if (_bytes == null)\n");

	strVector.append(strTable_3);
	strVector.append("{\n");

	strVector.append(strTable_4);
	strVector.append("inStream.WriteInt(0);\n");

	strVector.append(strTable_4);
	strVector.append("return;\n");

	strVector.append(strTable_3);
	strVector.append("}\n");


	strVector.append(strTable_3);
	strVector.append("inStream.WriteInt(_bytes.Length);\n");

	strVector.append(strTable_3);
	strVector.append("inStream.Write(_bytes, 0, _bytes.Length);\n");

	strVector.append(strTable_2);
	strVector.append("}\n");

	strVector.append(strTable_1);
	strVector.append("}\n");

	strVector.append("}\n");



	fwrite(strVector.c_str(), strVector.length(), 1, fp);

	fclose(fp);
}

void    ProtocolParse_Writer_CSharp::WriteWJSVector_CSharp(stNamespace& pNamespace, stWJSVector& pWJSVector, string foldername, const string& strWriteNameSpace)
{
	if (pWJSVector._ParTName == "byte" || pWJSVector._ParTName == "BYTE") {
		WriteWJSVector2_CSharp(pNamespace, pWJSVector, foldername, strWriteNameSpace);
		return;
	}

	char t_ch[512];
	sprintf(t_ch, "%s/%s.cs", foldername.c_str(), pWJSVector._name.c_str());

	//先创建文件
	FILE* fp = fopen(t_ch, "w");
	if (!fp)
		return;

	//写信息头,共用
	WriteInfoHead(fp);


	//string strTable_0=CreateTableString(1);
	string strTable_1 = CreateTableString(1);
	string strTable_2 = CreateTableString(2);
	string strTable_3 = CreateTableString(3);
	string strTable_4 = CreateTableString(4);

	string strWJSVector;

	strWJSVector.append("using System;\n");
	strWJSVector.append("using System.Collections;\n");
	strWJSVector.append("using System.Collections.Generic;\n");
	strWJSVector.append("using InOutStream;\n");
	sprintf(t_ch, "namespace %s\n", strWriteNameSpace.c_str());
	strWJSVector.append(t_ch);
	strWJSVector.append("{\n");




	/////////////有两种版本，一种是非Flash基础类型，一种是
	bool  tt_bCSharpBaseType = IsCSharpBaseType2(pWJSVector._ParTName);


	strWJSVector.append(strTable_1);
	sprintf(t_ch, "public  class %s : List<%s>\n", pWJSVector._name.c_str(), TypeConvertToCSharpType(pWJSVector._ParTName).c_str());
	strWJSVector.append(t_ch);
	strWJSVector.append(strTable_1);
	strWJSVector.append("{\n");

	strWJSVector.append(strTable_2);
	sprintf(t_ch, "public %s()\n", pWJSVector._name.c_str());
	strWJSVector.append(t_ch);

	strWJSVector.append(strTable_2);
	strWJSVector.append("{\n");

	//strVector.append(strTable_3);
	//strVector.append("super();\n");

	strWJSVector.append(strTable_2);
	strWJSVector.append("}\n");


	strWJSVector.append(strTable_2);
	sprintf(t_ch, "public static %s read(OutStream outStream)\n", pWJSVector._name.c_str());
	strWJSVector.append(t_ch);

	strWJSVector.append(strTable_2);
	strWJSVector.append("{\n");

	strWJSVector.append(strTable_3);
	sprintf(t_ch, "%s o = new %s();\n", pWJSVector._name.c_str(), pWJSVector._name.c_str());
	strWJSVector.append(t_ch);

	strWJSVector.append(strTable_3);
	strWJSVector.append("int count = outStream.ReadInt();\n");



	strWJSVector.append(strTable_3);
	strWJSVector.append("for(int i=0;i<count;i++)\n");

	strWJSVector.append(strTable_3);
	strWJSVector.append("{\n");

	if (!tt_bCSharpBaseType) {
		strWJSVector.append(strTable_4);
		sprintf(t_ch, "o.Add(%s.read(outStream));\n", pWJSVector._ParTName.c_str());
		strWJSVector.append(t_ch);
	}
	else {
		strWJSVector.append(strTable_4);
		sprintf(t_ch, "o.Add(outStream.%s());\n", TypeConvertToReadCSharpType(pWJSVector._ParTName).c_str());
		strWJSVector.append(t_ch);
	}

	strWJSVector.append(strTable_3);
	strWJSVector.append("}\n");

	strWJSVector.append(strTable_3);
	strWJSVector.append("return o;\n");

	strWJSVector.append(strTable_2);
	strWJSVector.append("}\n");

	/////////////////////
	//
	////////////////////
	strWJSVector.append(strTable_2);
	sprintf(t_ch, "public void readLocal(OutStream outStream)\n");
	strWJSVector.append(t_ch);

	strWJSVector.append(strTable_2);
	strWJSVector.append("{\n");


	strWJSVector.append(strTable_3);
	strWJSVector.append("int count = outStream.ReadInt();\n");



	strWJSVector.append(strTable_3);
	strWJSVector.append("for(int i=0;i<count;i++)\n");

	strWJSVector.append(strTable_3);
	strWJSVector.append("{\n");

	if (!tt_bCSharpBaseType) {
		strWJSVector.append(strTable_4);
		sprintf(t_ch, "this.Add(%s.read(outStream));\n", pWJSVector._ParTName.c_str());
		strWJSVector.append(t_ch);
	}
	else {
		strWJSVector.append(strTable_4);
		sprintf(t_ch, "this.Add(outStream.%s());\n", TypeConvertToReadCSharpType(pWJSVector._ParTName).c_str());
		strWJSVector.append(t_ch);
	}

	strWJSVector.append(strTable_3);
	strWJSVector.append("}\n");


	strWJSVector.append(strTable_2);
	strWJSVector.append("}\n");

	////////////////
	//
	/////////////////

	strWJSVector.append(strTable_2);
	strWJSVector.append("public void write(InStream inStream)\n");

	strWJSVector.append(strTable_2);
	strWJSVector.append("{\n");

	strWJSVector.append(strTable_3);
	strWJSVector.append("inStream.WriteInt(Count);\n");

	strWJSVector.append(strTable_3);
	strWJSVector.append("for(int i=0;i<Count;i++)\n");

	strWJSVector.append(strTable_3);
	strWJSVector.append("{\n");
	//
	if (!tt_bCSharpBaseType) {
		strWJSVector.append(strTable_4);
		sprintf(t_ch, "((%s)(this)[i]).write(inStream);\n", pWJSVector._ParTName.c_str());
		strWJSVector.append(t_ch);
	}
	else {
		strWJSVector.append(strTable_4);
		sprintf(t_ch, "inStream.%s((%s)(this)[i]);\n", TypeConvertToWriteCSharpType(pWJSVector._ParTName).c_str(), TypeConvertToCSharpType(pWJSVector._ParTName).c_str());
		strWJSVector.append(t_ch);
	}


	strWJSVector.append(strTable_3);
	strWJSVector.append("}\n");

	strWJSVector.append(strTable_2);
	strWJSVector.append("}\n");

	strWJSVector.append(strTable_1);
	strWJSVector.append("}\n");

	strWJSVector.append("}\n");



	fwrite(strWJSVector.c_str(), strWJSVector.length(), 1, fp);

	fclose(fp);
}

void    ProtocolParse_Writer_CSharp::WriteWJSVector2_CSharp(stNamespace& pNamespace, stWJSVector& pWJSVector, string foldername, const string& strWriteNameSpace)
{
	char t_ch[512];
	sprintf(t_ch, "%s/%s.cs", foldername.c_str(), pWJSVector._name.c_str());

	//先创建文件
	FILE* fp = fopen(t_ch, "w");
	if (!fp)
		return;

	//写信息头,共用
	WriteInfoHead(fp);


	//string strTable_0=CreateTableString(1);
	string strTable_1 = CreateTableString(1);
	string strTable_2 = CreateTableString(2);
	string strTable_3 = CreateTableString(3);
	string strTable_4 = CreateTableString(4);

	string strVector;

	strVector.append("using System;\n");
	strVector.append("using System.Collections;\n");
	strVector.append("using System.Collections.Generic;\n");
	strVector.append("using InOutStream;\n");
	sprintf(t_ch, "namespace %s\n", strWriteNameSpace.c_str());
	strVector.append(t_ch);
	strVector.append("{\n");




	/////////////有两种版本，一种是非Flash基础类型，一种是
	bool  tt_bCSharpBaseType = IsCSharpBaseType2(pWJSVector._ParTName);

	//strVector.append(WriteComment_CSharp(strTable_1,pWJSVector._comment,"").c_str());

	strVector.append(strTable_1);
	sprintf(t_ch, "public  class %s\n", pWJSVector._name.c_str());
	strVector.append(t_ch);
	strVector.append(strTable_1);
	strVector.append("{\n");

	strVector.append(strTable_2);
	strVector.append("public byte[] _bytes;\n");

	strVector.append(strTable_2);
	sprintf(t_ch, "public %s()\n", pWJSVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("{\n");

	//strVector.append(strTable_3);
	//strVector.append("super();\n");

	strVector.append(strTable_2);
	strVector.append("}\n");


	strVector.append(strTable_2);
	sprintf(t_ch, "public static %s read(OutStream outStream)\n", pWJSVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	sprintf(t_ch, "%s o = new %s();\n", pWJSVector._name.c_str(), pWJSVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_3);
	strVector.append("o._bytes = outStream.ReadByteArray();\n");

	strVector.append(strTable_3);
	strVector.append("return o;\n");

	strVector.append(strTable_2);
	strVector.append("}\n");


	strVector.append(strTable_2);
	sprintf(t_ch, "public void  readLocal(OutStream outStream)\n");
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("{\n");


	strVector.append(strTable_3);
	strVector.append("this._bytes = outStream.ReadByteArray();\n");

	strVector.append(strTable_2);
	strVector.append("}\n");


	strVector.append(strTable_2);
	strVector.append("public void write(InStream inStream)\n");

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	strVector.append("if (_bytes == null)\n");

	strVector.append(strTable_3);
	strVector.append("{\n");

	strVector.append(strTable_4);
	strVector.append("inStream.WriteInt(0);\n");

	strVector.append(strTable_4);
	strVector.append("return;\n");

	strVector.append(strTable_3);
	strVector.append("}\n");


	strVector.append(strTable_3);
	strVector.append("inStream.WriteInt(_bytes.Length);\n");

	strVector.append(strTable_3);
	strVector.append("inStream.Write(_bytes, 0, _bytes.Length);\n");

	strVector.append(strTable_2);
	strVector.append("}\n");

	strVector.append(strTable_1);
	strVector.append("}\n");

	strVector.append("}\n");



	fwrite(strVector.c_str(), strVector.length(), 1, fp);

	fclose(fp);
}
void    ProtocolParse_Writer_CSharp::WriteClass_CSharp(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace)
{
	switch (filetype) {
	case 0:
	{
		////对于客户端而言，request的为发送，callback的为回调
		if (pClass._strtype.compare("callback") == 0) {
			WriteClass_IReceiver_CSharp(pClass, filetype, pNamespace, foldername, false, strWriteNameSpace);
			WriteClass_Receiver_CSharp(pClass, filetype, pNamespace, foldername, strWriteNameSpace);
		}
		else {
			WriteClass_Send_CSharp(pClass, filetype, pNamespace, foldername, strWriteNameSpace);
		}
	}
	break;
	case 1:
	{
		////服务器而言，请求的是回调，callback是发送
		if (pClass._strtype.compare("request") == 0) //C2S
		{
			WriteClass_IReceiver_CSharp(pClass, filetype, pNamespace, foldername, false, strWriteNameSpace);
			WriteClass_Receiver_CSharp(pClass, filetype, pNamespace, foldername, strWriteNameSpace);
		}
		else //S2C
		{
			WriteClass_Send_CSharp(pClass, filetype, pNamespace, foldername, strWriteNameSpace);
		}
	}
	break;
	case 2:
	{

		//事实上两个里面是一样的
		if (pClass._strtype.compare("request") == 0) //C2S
		{
			WriteClass_IReceiver_CSharp(pClass, filetype, pNamespace, foldername, true, strWriteNameSpace);
			WriteClass_Receiver_CSharp(pClass, filetype, pNamespace, foldername, strWriteNameSpace);
			WriteClass_Send_CSharp(pClass, filetype, pNamespace, foldername, strWriteNameSpace);
		}

	}
	break;
	default:
		break;
	}
}

void    ProtocolParse_Writer_CSharp::WriteClass_IReceiver_CSharp(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, bool bemptyimplement, const string& strWriteNameSpace)
{
	char t_ch[1024];
	sprintf(t_ch, "%s/IReceiver_%s.cs", foldername.c_str(), pNamespace._protocolname.c_str());

	//先创建文件
	FILE* fp = fopen(t_ch, "w");
	if (!fp)
		return;

	//写信息头,共用
	WriteInfoHead(fp);

	string strclass;

	string strTable_1 = CreateTableString(1);
	string strTable_2 = CreateTableString(2);
	string strTable_3 = CreateTableString(3);

	strclass.append("using System;\n");
	strclass.append("using System.Collections;\n");
	strclass.append("using InOutStream;\n");

	vector<string> tt_usePacketNameList;
	GetUsePacketNamesByRefHead_CSharp(tt_usePacketNameList);
	for (int i = 0; i < (int)tt_usePacketNameList.size(); i++) {
		sprintf(t_ch, "using %s;\n", tt_usePacketNameList[i].c_str());
		strclass.append(t_ch);
	}

	sprintf(t_ch, "namespace %s\n", strWriteNameSpace.c_str());
	strclass.append(t_ch);
	strclass.append("{\n");

	int tt_maxLayer = pClass.GetMaxLayer();
	for (int k = 1; k <= tt_maxLayer; k++) {
		strclass.append(strTable_1);
		if (k != 1) {
			if (filetype == 2 && (!m_bPureVirtual)) {
				sprintf(t_ch, "public class IReceiver_%s_%d : IReceiver_%s_%d\n", pNamespace._protocolname.c_str(), k, pNamespace._protocolname.c_str(), k - 1);
			}
			else {
				sprintf(t_ch, "public interface IReceiver_%s_%d : IReceiver_%s_%d\n", pNamespace._protocolname.c_str(), k, pNamespace._protocolname.c_str(), k - 1);
			}

		}
		else {
			if (filetype == 2 && (!m_bPureVirtual)) {
				sprintf(t_ch, "public class IReceiver_%s_%d\n", pNamespace._protocolname.c_str(), k);
			}
			else {
				sprintf(t_ch, "public interface IReceiver_%s_%d\n", pNamespace._protocolname.c_str(), k);
			}
		}

		strclass.append(t_ch);

		strclass.append(strTable_1);
		strclass.append("{\n");
		fwrite(strclass.c_str(), strclass.length(), 1, fp);
		strclass = "";


		for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
			if (pClass._FuncVec[i]._bDiscard || pClass._FuncVec[i]._Layer != k)
				continue;
			if (filetype == 2 && ((m_bSCSpliteS && pClass._FuncVec[i]._SCSpliteType == -1) || (m_bSCSpliteC && pClass._FuncVec[i]._SCSpliteType == 1)))
				continue;
			//写函数
			WriteFunc_IReceiver_CSharp(pClass._FuncVec[i], filetype, 3, pClass, pNamespace, foldername, fp, bemptyimplement);
		}

		strclass.append(strTable_1);
		strclass.append("};\n");

		fwrite(strclass.c_str(), strclass.length(), 1, fp);
		strclass = "";

	}


	strclass.append(strTable_1);
	if (tt_maxLayer > 0) {
		if (filetype == 2 && (!m_bPureVirtual)) {
			sprintf(t_ch, "public class IReceiver_%s : IReceiver_%s_%d\n", pNamespace._protocolname.c_str(), pNamespace._protocolname.c_str(), tt_maxLayer);
		}
		else {
			sprintf(t_ch, "public interface IReceiver_%s : IReceiver_%s_%d\n", pNamespace._protocolname.c_str(), pNamespace._protocolname.c_str(), tt_maxLayer);
		}
	}
	else {
		if (filetype == 2 && (!m_bPureVirtual)) {
			sprintf(t_ch, "public class IReceiver_%s\n", pNamespace._protocolname.c_str());
		}
		else {
			sprintf(t_ch, "public interface IReceiver_%s\n", pNamespace._protocolname.c_str());
		}
	}


	strclass.append(t_ch);

	strclass.append(strTable_1);
	strclass.append("{\n");
	fwrite(strclass.c_str(), strclass.length(), 1, fp);


	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard || pClass._FuncVec[i]._Layer != 0)
			continue;
		if (filetype == 2 && ((m_bSCSpliteS && pClass._FuncVec[i]._SCSpliteType == -1) || (m_bSCSpliteC && pClass._FuncVec[i]._SCSpliteType == 1)))
			continue;
		//写函数
		WriteFunc_IReceiver_CSharp(pClass._FuncVec[i], filetype, 3, pClass, pNamespace, foldername, fp, bemptyimplement);
	}

	strclass = "";
	strclass.append(strTable_1);
	strclass.append("}\n");

	strclass.append("}\n");

	fwrite(strclass.c_str(), strclass.length(), 1, fp);
	fclose(fp);

}
void    ProtocolParse_Writer_CSharp::WriteFunc_IReceiver_CSharp(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp, bool bemptyimplement)
{
	char t_ch[1024];

	string strTable_0 = CreateTableString(tablenum);
	string strTable_1 = CreateTableString(tablenum + 1);


	string strFunc;

	strFunc.append(WriteComment_CSharp(strTable_0, pFunc._comment, "", false).c_str());
	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		strFunc.append(WriteComment_CSharp(strTable_0, pFunc._InParVec[i]._comment, pFunc._InParVec[i]._strParName, false).c_str());
	}

	strFunc.append(strTable_0);
	strFunc.append("///</summary>\n");


	strFunc.append(strTable_0);
	if (filetype == 2 && (!m_bPureVirtual)) {
		sprintf(t_ch, "public virtual void IReceiver_%s(", pFunc._name.c_str());
	}
	else
		sprintf(t_ch, "void IReceiver_%s(", pFunc._name.c_str());
	strFunc.append(t_ch);

	if (filetype == 0) {
		if (pFunc._InParVec.size() > 0) {
			for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
				if (IsExType_vector(pFunc._InParVec[i]._strParType)) {
					sprintf(t_ch, "%s %s,", TypeConvertToCSharpType(pFunc._InParVec[i]._strParType).c_str(), pFunc._InParVec[i]._strParName.c_str());
					strFunc.append(t_ch);
				}
				else {
					sprintf(t_ch, "%s %s,", TypeConvertToCSharpType(pFunc._InParVec[i]._strParType).c_str(), pFunc._InParVec[i]._strParName.c_str());
					strFunc.append(t_ch);
				}

			}
		}
		strFunc.append("System.Object pExData);\n");
	}
	else {
		if (pFunc._InParVec.size() > 0) {
			for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
				if (IsExType_vector(pFunc._InParVec[i]._strParType)) {
					sprintf(t_ch, "%s %s,", TypeConvertToCSharpType(pFunc._InParVec[i]._strParType).c_str(), pFunc._InParVec[i]._strParName.c_str());
					strFunc.append(t_ch);
				}
				else {
					sprintf(t_ch, "%s %s,", TypeConvertToCSharpType(pFunc._InParVec[i]._strParType).c_str(), pFunc._InParVec[i]._strParName.c_str());
					strFunc.append(t_ch);
				}

			}
		}

		if (filetype == 2 && (!m_bPureVirtual)) {
			strFunc.append("int sessionId,System.Object pExData){}\n");
		}
		else
			strFunc.append("int sessionId,System.Object pExData);\n");
	}


	fwrite(strFunc.c_str(), strFunc.length(), 1, fp);
}

void    ProtocolParse_Writer_CSharp::WriteClass_Receiver_CSharp(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace)
{
	char t_ch[1024];
	sprintf(t_ch, "%s/%s_Callback.cs", foldername.c_str(), pNamespace._protocolname.c_str());

	//先创建文件
	FILE* fp = fopen(t_ch, "w");
	if (!fp)
		return;

	//写信息头,共用
	WriteInfoHead(fp);

	string strclass;

	string strTable_0 = CreateTableString(1);
	string strTable_1 = CreateTableString(2);
	string strTable_2 = CreateTableString(3);
	string strTable_3 = CreateTableString(4);

	strclass.append("using System;\n");
	strclass.append("using System.Collections;\n");
	strclass.append("using InOutStream;\n");

	if (m_bWithLog) {
		strclass.append("using UnityEngine;\n");
	}

	vector<string> tt_usePacketNameList;
	GetUsePacketNamesByRefHead_CSharp(tt_usePacketNameList);
	for (int i = 0; i < (int)tt_usePacketNameList.size(); i++) {
		sprintf(t_ch, "using %s;\n", tt_usePacketNameList[i].c_str());
		strclass.append(t_ch);
	}

	sprintf(t_ch, "namespace %s\n", strWriteNameSpace.c_str());
	strclass.append(t_ch);
	strclass.append("{\n");



	strclass.append(strTable_0);
	sprintf(t_ch, "public class %s_Callback\n", pNamespace._protocolname.c_str());
	strclass.append(t_ch);

	strclass.append(strTable_0);
	strclass.append("{\n");
	fwrite(strclass.c_str(), strclass.length(), 1, fp);

	WriteFunc_ReceiverParser_CSharp(pClass, filetype, 3, pNamespace, foldername, fp);


	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		if (filetype == 2 && ((m_bSCSpliteS && pClass._FuncVec[i]._SCSpliteType == -1) || (m_bSCSpliteC && pClass._FuncVec[i]._SCSpliteType == 1)))
			continue;
		WriteFunc_Receiver_CSharp(pClass._FuncVec[i], filetype, 3, pClass, pNamespace, foldername, fp);
	}


	strclass = "";
	strclass.append(strTable_0);
	strclass.append("}\n");
	strclass.append("}\n");

	fwrite(strclass.c_str(), strclass.length(), 1, fp);
	fclose(fp);
}

void    ProtocolParse_Writer_CSharp::WriteFunc_ReceiverParser_CSharp(stClass& pClass, int filetype, int tablenum, stNamespace& pNamespace, string foldername, FILE* fp)
{
	char t_ch[1024];

	string strTable_0 = CreateTableString(tablenum);
	string strTable_1 = CreateTableString(tablenum + 1);
	string strTable_2 = CreateTableString(tablenum + 2);
	string strTable_3 = CreateTableString(tablenum + 3);
	string strTable_4 = CreateTableString(tablenum + 4);


	string strFunc;
	strFunc.append(strTable_0);

	if (filetype == 0) {
		sprintf(t_ch, "public static bool Parser(IReceiver_%s cls,OutStream outStream,System.Object pExData)\n", pNamespace._protocolname.c_str());
		strFunc.append(t_ch);
	}
	else {
		sprintf(t_ch, "public static bool Parser(IReceiver_%s cls,OutStream outStream,int sessionId,System.Object pExData)\n", pNamespace._protocolname.c_str());
		strFunc.append(t_ch);
	}

	strFunc.append(strTable_0);
	strFunc.append("{\n");

	strFunc.append(strTable_1);
	strFunc.append("int no = outStream.ReadInt();\n");

	strFunc.append(strTable_1);
	sprintf(t_ch, "if(no!=ID_%s.%s)\n", pNamespace._protocolname.c_str(), m_ProtocolNo._name.c_str());
	strFunc.append(t_ch);

	strFunc.append(strTable_1);
	strFunc.append("{\n");

	strFunc.append(strTable_2);
	strFunc.append("outStream.Offset(- 4);\n");

	strFunc.append(strTable_2);
	strFunc.append("return false;\n");

	strFunc.append(strTable_1);
	strFunc.append("}\n");

	strFunc.append(strTable_1);
	strFunc.append("int proversionno = (int)outStream.ReadShort();\n");


	strFunc.append(strTable_1);
	strFunc.append("int methodid =outStream.ReadInt();\n");

	strFunc.append(strTable_1);
	strFunc.append("switch(methodid)\n");

	strFunc.append(strTable_1);
	strFunc.append("{\n");

	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		strFunc.append(strTable_2);
		sprintf(t_ch, "case ID_%s.ID_%s_%s:\n", pNamespace._protocolname.c_str(), pClass._name.c_str(), pClass._FuncVec[i]._name.c_str());
		strFunc.append(t_ch);


		strFunc.append(strTable_3);
		if (filetype == 0) {
			sprintf(t_ch, "_parser_%s(cls,outStream,pExData,proversionno);\n", pClass._FuncVec[i]._name.c_str());
			strFunc.append(t_ch);
		}
		else {
			sprintf(t_ch, "_parser_%s(cls,outStream,sessionId,pExData,proversionno);\n", pClass._FuncVec[i]._name.c_str());
			strFunc.append(t_ch);
		}


		strFunc.append(strTable_3);
		strFunc.append("return true;\n");
	}


	strFunc.append(strTable_2);
	strFunc.append("default:\n");

	strFunc.append(strTable_3);
	strFunc.append("return false;\n");

	strFunc.append(strTable_1);
	strFunc.append("}\n");

	strFunc.append(strTable_0);
	strFunc.append("}\n");


	fwrite(strFunc.c_str(), strFunc.length(), 1, fp);
}
void    ProtocolParse_Writer_CSharp::WriteFunc_Receiver_CSharp(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp)
{
	char t_ch[1024];

	string strTable_0 = CreateTableString(tablenum);
	string strTable_1 = CreateTableString(tablenum + 1);
	string strTable_2 = CreateTableString(tablenum + 2);
	string strTable_3 = CreateTableString(tablenum + 3);


	string strFunc;

	if (filetype == 0) {
		strFunc.append(strTable_0);
		sprintf(t_ch, "private static void _parser_%s(IReceiver_%s cls,OutStream outStream,System.Object pExData,int proversionno)\n", pFunc._name.c_str(), pNamespace._protocolname.c_str());
		strFunc.append(t_ch);
	}
	else {
		strFunc.append(strTable_0);
		sprintf(t_ch, "private static void _parser_%s(IReceiver_%s cls,OutStream outStream,int sessionId,System.Object pExData,int proversionno)\n", pFunc._name.c_str(), pNamespace._protocolname.c_str());
		strFunc.append(t_ch);
	}

	strFunc.append(strTable_0);
	strFunc.append("{\n");

	if (m_bWithLog) {
		strFunc.append(strTable_1);
		sprintf(t_ch, "Debug.Log(\"[[[%s.%s]]]\");\n", pClass._name.c_str(), pFunc._name.c_str());
		strFunc.append(t_ch);
	}


	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		strFunc.append(strTable_1);
		if (IsCSharpBaseType(pFunc._InParVec[i]._strParType, false) || IsExType_enum(pFunc._InParVec[i]._strParType)) {
			sprintf(t_ch, "%s %s;\n", (TypeConvertToCSharpType(pFunc._InParVec[i]._strParType)).c_str(), pFunc._InParVec[i]._strParName.c_str());

		}
		else if (IsExType_vector(pFunc._InParVec[i]._strParType)) {
			sprintf(t_ch, "%s %s;\n", (TypeConvertToCSharpType(pFunc._InParVec[i]._strParType)).c_str(), pFunc._InParVec[i]._strParName.c_str());

		}
		else {
			sprintf(t_ch, "%s %s;\n", (TypeConvertToCSharpType(pFunc._InParVec[i]._strParType)).c_str(), pFunc._InParVec[i]._strParName.c_str());
		}
		strFunc.append(t_ch);
	}


	//作出判定相等
	strFunc.append(strTable_1);
	sprintf(t_ch, "if(proversionno==%d || (proversionno==0))\n", pFunc._version);
	strFunc.append(t_ch);

	strFunc.append(strTable_1);
	strFunc.append("{\n");

	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		strFunc.append(strTable_2);
		if (IsCSharpBaseType(pFunc._InParVec[i]._strParType, false)) {
			sprintf(t_ch, "%s = outStream.%s();\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToReadCSharpType(pFunc._InParVec[i]._strParType)).c_str());

		}
		else if (IsExType_enum(pFunc._InParVec[i]._strParType)) {
			sprintf(t_ch, "%s = %s.read(outStream);\n", pFunc._InParVec[i]._strParName.c_str(), pFunc._InParVec[i]._strParType.c_str());
		}
		else {
			sprintf(t_ch, "%s = %s.read(outStream);\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToCSharpType(pFunc._InParVec[i]._strParType)).c_str());
		}
		strFunc.append(t_ch);
	}

	strFunc.append(strTable_2);
	sprintf(t_ch, "cls.IReceiver_%s(", pFunc._name.c_str());
	strFunc.append(t_ch);


	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		sprintf(t_ch, "%s,", pFunc._InParVec[i]._strParName.c_str());
		strFunc.append(t_ch);
	}

	if (filetype == 0) {
		strFunc.append("pExData);\n");
	}
	else {
		strFunc.append("sessionId,pExData);\n");
	}

	strFunc.append(strTable_1);
	strFunc.append("}\n");


	//////////////低解高
	strFunc.append(strTable_1);
	sprintf(t_ch, "else if(proversionno>%d)\n", pFunc._version);
	strFunc.append(t_ch);

	strFunc.append(strTable_1);
	strFunc.append("{\n");

	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		strFunc.append(strTable_2);
		if (IsCSharpBaseType(pFunc._InParVec[i]._strParType, false)) {
			sprintf(t_ch, "%s = outStream.%s();\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToReadCSharpType(pFunc._InParVec[i]._strParType)).c_str());
		}
		else if (IsExType_enum(pFunc._InParVec[i]._strParType)) {
			sprintf(t_ch, "%s = %s.read(outStream);\n", pFunc._InParVec[i]._strParName.c_str(), pFunc._InParVec[i]._strParType.c_str());
		}
		else {
			sprintf(t_ch, "%s = %s.read(outStream);\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToCSharpType(pFunc._InParVec[i]._strParType)).c_str());
		}
		strFunc.append(t_ch);
	}

	strFunc.append(strTable_2);
	sprintf(t_ch, "cls.IReceiver_%s(", pFunc._name.c_str());
	strFunc.append(t_ch);


	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		sprintf(t_ch, "%s,", pFunc._InParVec[i]._strParName.c_str());
		strFunc.append(t_ch);
	}

	if (filetype == 0) {
		strFunc.append("pExData);\n");
	}
	else {
		strFunc.append("sessionId,pExData);\n");
	}

	strFunc.append(strTable_1);
	strFunc.append("}\n");

	///////////////高解低
	strFunc.append(strTable_1);
	strFunc.append("else\n");

	strFunc.append(strTable_1);
	strFunc.append("{\n");

	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		strFunc.append(strTable_2);
		sprintf(t_ch, "if(proversionno>=%d)\n", pFunc._InParVec[i]._version);
		strFunc.append(t_ch);

		strFunc.append(strTable_3);
		if (IsCSharpBaseType(pFunc._InParVec[i]._strParType, false)) {
			sprintf(t_ch, "%s = outStream.%s();\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToReadCSharpType(pFunc._InParVec[i]._strParType)).c_str());

		}
		else if (IsExType_enum(pFunc._InParVec[i]._strParType)) {
			sprintf(t_ch, "%s = %s.read(outStream);\n", pFunc._InParVec[i]._strParName.c_str(), pFunc._InParVec[i]._strParType.c_str());
		}
		else {
			sprintf(t_ch, "%s = %s.read(outStream);\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToCSharpType(pFunc._InParVec[i]._strParType)).c_str());
		}
		strFunc.append(t_ch);

		string tt_default = GetDefaultValue(pFunc._InParVec[i]._strParType, false, false, true);
		if (tt_default.length() > 0) {
			strFunc.append(strTable_2);
			strFunc.append("else\n");

			strFunc.append(strTable_3);
			sprintf(t_ch, "%s = %s;\n", pFunc._InParVec[i]._strParName.c_str(), tt_default.c_str());
			strFunc.append(t_ch);
		}
		else if (IsExType_vector(pFunc._InParVec[i]._strParType)) {
			strFunc.append(strTable_2);
			strFunc.append("else\n");

			strFunc.append(strTable_3);
			sprintf(t_ch, "%s = new %s();\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToCSharpType(pFunc._InParVec[i]._strParType)).c_str());
			strFunc.append(t_ch);
		}
		else if (IsExType_WJSVector(pFunc._InParVec[i]._strParType)) {
			strFunc.append(strTable_2);
			strFunc.append("else\n");

			strFunc.append(strTable_3);
			sprintf(t_ch, "%s = new %s();\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToCSharpType(pFunc._InParVec[i]._strParType)).c_str());
			strFunc.append(t_ch);
		}
		else if (IsExType_struct(pFunc._InParVec[i]._strParType)) {
			strFunc.append(strTable_2);
			strFunc.append("else\n");

			strFunc.append(strTable_3);
			sprintf(t_ch, "%s = new %s();\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToCSharpType(pFunc._InParVec[i]._strParType)).c_str());
			strFunc.append(t_ch);
		}
		else if (IsExType_enum(pFunc._InParVec[i]._strParType)) {
			strFunc.append(strTable_2);
			strFunc.append("else\n");

			strFunc.append(strTable_3);
			sprintf(t_ch, "%s = 0;\n", pFunc._InParVec[i]._strParName.c_str());
			strFunc.append(t_ch);
		}
	}

	strFunc.append(strTable_2);
	sprintf(t_ch, "cls.IReceiver_%s(", pFunc._name.c_str());
	strFunc.append(t_ch);


	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		sprintf(t_ch, "%s,", pFunc._InParVec[i]._strParName.c_str());
		strFunc.append(t_ch);
	}

	if (filetype == 0) {
		strFunc.append("pExData);\n");
	}
	else {
		strFunc.append("sessionId,pExData);\n");
	}

	strFunc.append(strTable_1);
	strFunc.append("}\n");


	/////////////////
	strFunc.append(strTable_0);
	strFunc.append("}\n");


	fwrite(strFunc.c_str(), strFunc.length(), 1, fp);

}

void    ProtocolParse_Writer_CSharp::WriteClass_Send_CSharp(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace)
{
	char t_ch[512];
	sprintf(t_ch, "%s/%s_Send.cs", foldername.c_str(), pNamespace._protocolname.c_str());

	//先创建文件
	FILE* fp = fopen(t_ch, "w");
	if (!fp)
		return;

	//写信息头,共用
	WriteInfoHead(fp);

	string strclass;

	string strTable_1 = CreateTableString(1);
	string strTable_2 = CreateTableString(2);
	string strTable_3 = CreateTableString(3);

	strclass.append("using System;\n");
	strclass.append("using System.Collections;\n");
	strclass.append("using InOutStream;\n");

	vector<string> tt_usePacketNameList;
	GetUsePacketNamesByRefHead_CSharp(tt_usePacketNameList);
	for (int i = 0; i < (int)tt_usePacketNameList.size(); i++) {
		sprintf(t_ch, "using %s;\n", tt_usePacketNameList[i].c_str());
		strclass.append(t_ch);
	}

	sprintf(t_ch, "namespace %s\n", strWriteNameSpace.c_str());
	strclass.append(t_ch);
	strclass.append("{\n");



	strclass.append(strTable_1);
	sprintf(t_ch, "public class %s_Send\n", pNamespace._protocolname.c_str());
	strclass.append(t_ch);

	strclass.append(strTable_1);
	strclass.append("{\n");


	fwrite(strclass.c_str(), strclass.length(), 1, fp);
	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		if (filetype == 2 && ((m_bSCSpliteS && pClass._FuncVec[i]._SCSpliteType == 1) || (m_bSCSpliteC && pClass._FuncVec[i]._SCSpliteType == -1)))
			continue;
		//写函数
		WriteFunc_SendByteSeq_CSharp(pClass._FuncVec[i], 2, pClass, pNamespace, foldername, fp);
	}


	strclass = "";
	strclass.append(strTable_1);
	strclass.append("}\n");

	strclass.append("}\n");

	fwrite(strclass.c_str(), strclass.length(), 1, fp);

	fclose(fp);

}

void    ProtocolParse_Writer_CSharp::WriteFunc_SendByteSeq_CSharp(stFunc& pFunc, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp)
{
	char t_ch[1024];


	string strTable_0 = CreateTableString(tablenum);
	string strTable_1 = CreateTableString(tablenum + 1);
	string strTable_2 = CreateTableString(tablenum + 2);

	string strfunc;

	strfunc.append(strTable_0);
	strfunc.append("///<summary>\n");

	strfunc.append(WriteComment_CSharp(strTable_0, pFunc._comment, "", false).c_str());
	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		strfunc.append(WriteComment_CSharp(strTable_0, pFunc._InParVec[i]._comment, pFunc._InParVec[i]._strParName, false).c_str());
	}

	strfunc.append(strTable_0);
	strfunc.append("///</summary>\n");

	strfunc.append(strTable_0);
	sprintf(t_ch, "public static InStream Send_%s(", pFunc._name.c_str());
	strfunc.append(t_ch);

	//有参
	if (pFunc._InParVec.size() > 0) {
		for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
			if (i == pFunc._InParVec.size() - 1) {
				sprintf(t_ch, "%s %s", TypeConvertToCSharpType(pFunc._InParVec[i]._strParType).c_str(), pFunc._InParVec[i]._strParName.c_str());
				strfunc.append(t_ch);
			}
			else {
				sprintf(t_ch, "%s %s,", TypeConvertToCSharpType(pFunc._InParVec[i]._strParType).c_str(), pFunc._InParVec[i]._strParName.c_str());
				strfunc.append(t_ch);
			}

		}

	}

	strfunc.append(")\n");
	strfunc.append(strTable_0);
	strfunc.append("{\n");

	strfunc.append(strTable_1);
	strfunc.append("InStream inStream = new InStream();\n");

	//先写协议号
	strfunc.append(strTable_1);
	sprintf(t_ch, "inStream.WriteInt(ID_%s.%s);\n", pNamespace._protocolname.c_str(), m_ProtocolNo._name.c_str());
	strfunc.append(t_ch);

	//写版本号

	strfunc.append(strTable_1);
	sprintf(t_ch, "int tt_version = %d;\n", pFunc._version);
	strfunc.append(t_ch);
	strfunc.append(strTable_1);
	strfunc.append("inStream.WriteShort((short)tt_version);\n");

	//再写函数号
	strfunc.append(strTable_1);
	sprintf(t_ch, "inStream.WriteInt(ID_%s.ID_%s_%s);\n", pNamespace._protocolname.c_str(), pClass._name.c_str(), pFunc._name.c_str());
	strfunc.append(t_ch);

	//
	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		strfunc.append(strTable_1);
		if (IsCSharpBaseType(pFunc._InParVec[i]._strParType, true)) {
			if (IsExType_enum(pFunc._InParVec[i]._strParType)) {
				sprintf(t_ch, "%s.write(inStream,%s);\n", pFunc._InParVec[i]._strParType.c_str(), pFunc._InParVec[i]._strParName.c_str());
			}
			else
				sprintf(t_ch, "inStream.%s(%s);\n", TypeConvertToWriteCSharpType(pFunc._InParVec[i]._strParType.c_str()).c_str(), pFunc._InParVec[i]._strParName.c_str());

		}
		else {
			sprintf(t_ch, "%s.write(inStream);\n", pFunc._InParVec[i]._strParName.c_str());

		}
		strfunc.append(t_ch);
	}

	strfunc.append(strTable_1);
	strfunc.append("return inStream;\n");
	strfunc.append(strTable_0);
	strfunc.append("}\n");

	fwrite(strfunc.c_str(), strfunc.length(), 1, fp);
}
//////////
//
//////////








void    ProtocolParse_Writer_CSharp::GetUsePacketNamesByRefHead_CSharp(vector<string>& packetNamesList)
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

string  ProtocolParse_Writer_CSharp::WriteComment_CSharp(const string& strTable, const string& strComment, const string& strPar, bool bWithSummary)
{
	if (strComment.length() == 0)
		return "";
	char tt_ch[2048];

	string strLine;

	vector<string> tt_subList;
	ProtocolUtils::GetAllSubString(strComment, "split", tt_subList, false);

	if (tt_subList.size() > 0 && bWithSummary) {
		strLine.append(strTable);
		strLine.append("///<summary>\n");
	}



	for (unsigned int i = 0; i < tt_subList.size(); i++) {
		string tt_strReal;
		UTF8Decode(tt_subList[i].c_str(), tt_strReal);
		strLine.append(strTable);
		if (strPar.length() > 0 && i == 0) {
			sprintf(tt_ch, "///%s  %s\n", strPar.c_str(), tt_strReal.c_str());

		}
		else {
			sprintf(tt_ch, "///%s\n", tt_strReal.c_str());
		}

		strLine.append(tt_ch);

	}

	if (tt_subList.size() > 0 && bWithSummary) {
		strLine.append(strTable);
		strLine.append("///</summary>\n");
	}
	return strLine;
}

#pragma region 类型转换与判断
bool    ProtocolParse_Writer_CSharp::IsCSharpBaseType(const string& strTypeName, bool bWrite)
{
	if (IsBaseType_WriterType(strTypeName, eProtocolWriterType::CSharp))
		return true;

	if (bWrite) {
		//枚举类型作为基本类型
		if (IsExType_enum(strTypeName))
			return true;
	}


	return false;
}
bool    ProtocolParse_Writer_CSharp::IsCSharpBaseType2(string strTypeName)
{
	if (IsBaseType(strTypeName))
		return true;

	return false;

}
string  ProtocolParse_Writer_CSharp::TypeConvertToCSharpType(const string& strTypeName)
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
				return "u16string";
			case 26://wstring
				return "wstring";

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
string  ProtocolParse_Writer_CSharp::TypeConvertToReadCSharpType(const string& strTypeName)
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

string  ProtocolParse_Writer_CSharp::TypeConvertToWriteCSharpType(const string& strTypeName)
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