#include "ProtocolParse_Writer_Java.h"
#include "ProtocolParse.h"
#include <time.h>
#include <sys/timeb.h>

extern string g_strBaseType[];


/////////////
//                Java
////////////////
ProtocolParse_Writer_Java::ProtocolParse_Writer_Java(const string& strPathName)
	:ProtocolParse_Writer(strPathName, eProtocolWriterType::Java)
{

}
ProtocolParse_Writer_Java::~ProtocolParse_Writer_Java()
{

}
bool    ProtocolParse_Writer_Java::CreateFile_Path(string path, int filetype, bool brefprotocol)
{
	return CreateFile_Path_Java(path, filetype, brefprotocol);
}
ProtocolParse_Writer* ProtocolParse_Writer_Java::Create_RefProtocolParse_Writer(const string& strPathName)
{
	return new ProtocolParse_Writer_Java(strPathName);
}
bool    ProtocolParse_Writer_Java::CreateFile_Path_Java(string path, int filetype, bool brefprotocol)
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
		ToLower(tt_strAddPath);
		str1.append(tt_strAddPath);
		//string str2=path;
		//str2.append(m_SaveFileName._servername2);
		return CreateFile_Java(str1, filetype, brefprotocol);
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
		ToLower(tt_strAddPath);
		str1.append(tt_strAddPath);
		//string str2=path;
		//str2.append(m_SaveFileName._clientname2);
		return CreateFile_Java(str1, filetype, brefprotocol);
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
		ToLower(tt_strAddPath);
		str1.append(tt_strAddPath);
		//string str2=path;
		//str2.append(m_SaveFileName._servername2);
		return CreateFile_Java(str1, filetype, brefprotocol);
	}
	break;
	default:
		return false;
		break;
	}
	//return false;

}

bool    ProtocolParse_Writer_Java::CreateFile_Java(string foldername, int filetype, bool bRefProtocol)
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
		WriteOneNameSpace_Java(m_NamespaceVec[i], filetype, foldername);
	}


	return true;
}

bool    ProtocolParse_Writer_Java::WriteOneNameSpace_Java(stNamespace& pNamespace, int filetype, string foldername)
{
	m_tablenum = 0;

	//char ch[256];

	////写函数的id,和协议号
	CreateAndWriteFuncIdAndProtocolNo_Java(pNamespace, foldername);


	//先写枚举
	for (size_t i = 0; i < pNamespace._EnumVec.size(); i++) {
		WriteEnum_Java(pNamespace, pNamespace._EnumVec[i], foldername);
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
				WriteStruct_Java(pNamespace, pNamespace._StructVec[structindex], foldername);
				structindex++;
			}
			else if (pNamespace._VectorVec[vectorindex]._index < pNamespace._StructVec[structindex]._index
				&& (pNamespace._VectorVec[vectorindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index)) {
				//写vector
				WriteVector_Java(pNamespace, pNamespace._VectorVec[vectorindex], foldername);
				vectorindex++;
			}
			else {
				//写类vector
				//WriteWJSVector_Java(pNamespace,pNamespace._WJSVectorVec[wjsvectorindex],foldername);
				wjsvectorindex++;
			}
		}
		else if (!bvectorend && (!bstructend)) {
			if (pNamespace._StructVec[structindex]._index < pNamespace._VectorVec[vectorindex]._index) {
				//写结构体
				WriteStruct_Java(pNamespace, pNamespace._StructVec[structindex], foldername);
				structindex++;
			}
			else {
				//写vector
				WriteVector_Java(pNamespace, pNamespace._VectorVec[vectorindex], foldername);
				vectorindex++;
			}
		}
		else if (!bvectorend && (!bwjsvectorend)) {
			if (pNamespace._VectorVec[vectorindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index) {
				//写vector
				WriteVector_Java(pNamespace, pNamespace._VectorVec[vectorindex], foldername);
				vectorindex++;
			}
			else {
				//写类vector
				//WriteWJSVector_Java(pNamespace,pNamespace._WJSVectorVec[wjsvectorindex],foldername);
				wjsvectorindex++;
			}
		}
		else if ((!bstructend) && (!bwjsvectorend)) {
			if (pNamespace._StructVec[structindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index) {
				//写结构体
				WriteStruct_Java(pNamespace, pNamespace._StructVec[structindex], foldername);
				structindex++;
			}
			else {
				//写类vector
				//WriteWJSVector_Java(pNamespace,pNamespace._WJSVectorVec[wjsvectorindex],foldername);
				wjsvectorindex++;
			}
		}
		else if (!bvectorend) {
			//写vector
			WriteVector_Java(pNamespace, pNamespace._VectorVec[vectorindex], foldername);
			vectorindex++;

		}
		else if (!bstructend) {
			//写结构体
			WriteStruct_Java(pNamespace, pNamespace._StructVec[structindex], foldername);
			structindex++;
		}
		else if (!bwjsvectorend) {
			//写类vector
			//WriteWJSVector_Java(pNamespace,pNamespace._WJSVectorVec[wjsvectorindex],foldername);
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
		WriteClass_Java(pNamespace._ClassVec[i], filetype, pNamespace, foldername);
	}

	//写结束的中括号}
	//WriteEndZhongKuoKao(fp);
	return true;
}

void    ProtocolParse_Writer_Java::CreateAndWriteFuncIdAndProtocolNo_Java(stNamespace& pNamespace, string foldername)
{
	if (pNamespace._ClassVec.size() == 0)
		return;
	char t_ch[512];
	sprintf(t_ch, "%s/ID_%s.java", foldername.c_str(), pNamespace._protocolnamelower.c_str());

	//先创建文件
	FILE* fp = fopen(t_ch, "w");
	if (!fp)
		return;

	//写信息头,共用
	WriteInfoHead(fp);

	string strid;

	string strTable_1 = CreateTableString(1);

	sprintf(t_ch, "package %s.%s;\n", m_strJavaPackage.c_str(), pNamespace._protocolnamelower.c_str());
	strid.append(t_ch);


	sprintf(t_ch, "public class ID_%s\n", pNamespace._protocolnamelower.c_str());
	strid.append(t_ch);
	strid.append("{\n");

	//函数id
	for (size_t i = 0; i < pNamespace._ClassVec.size(); i++) {
		for (size_t j = 0; j < pNamespace._ClassVec[i]._FuncVec.size(); j++) {
			strid.append(strTable_1);
			strid.append("public static  final int ");
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
		strid.append("public static  final int ");
		sprintf(t_ch, "%s=%d;\n", m_ProtocolNo._name.c_str(), m_ProtocolNo._value);
		strid.append(t_ch);
	}
	strid.append("}\n");

	fwrite(strid.c_str(), strid.length(), 1, fp);

	fclose(fp);
}
void    ProtocolParse_Writer_Java::WriteEnum_Java(stNamespace& pNamespace, stEnum& pEnum, string foldername)
{
	char t_ch[512];
	sprintf(t_ch, "%s/%s.java", foldername.c_str(), pEnum._name.c_str());

	//先创建文件
	FILE* fp = fopen(t_ch, "w");
	if (!fp)
		return;

	//写信息头,共用
	WriteInfoHead(fp);


	//string strTable_1=CreateTableString(1);
	string strTable_2 = CreateTableString(1);
	string strTable_3 = CreateTableString(2);
	string strTable_4 = CreateTableString(3);
	string strTable_5 = CreateTableString(4);
	//string strTable_6=CreateTableString(5);

	string strEnum;

	sprintf(t_ch, "package %s.%s;\n", m_strJavaPackage.c_str(), pNamespace._protocolnamelower.c_str());
	strEnum.append(t_ch);

	sprintf(t_ch, "import %s.*;\n", m_strJavaPackage.c_str());
	strEnum.append(t_ch);

	strEnum.append(WriteComment_Java("", pEnum._comment, "").c_str());
	sprintf(t_ch, "public class %s\n", pEnum._name.c_str());
	strEnum.append(t_ch);
	strEnum.append("{\n");


	for (size_t i = 0; i < pEnum._ParNameList.size(); i++) {
		strEnum.append(WriteComment_Java(strTable_2, pEnum._commentList[i], "").c_str());
		strEnum.append(strTable_2);
		sprintf(t_ch, "public static final int %s = %d;\n", pEnum._ParNameList[i].c_str(), i/*CovertStringToInt(pEnum._ParNameList[i])*/);
		strEnum.append(t_ch);
	}

	//写读
	strEnum.append(strTable_2);
	strEnum.append("public static int read(JUIOutputStream outStream)\n");

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

	strEnum.append(strTable_3);
	strEnum.append("return tt_a;\n");

	strEnum.append(strTable_2);
	strEnum.append("}\n");


	strEnum.append(strTable_2);
	strEnum.append("public static void write(JUIInputStream inStream,int va)\n");

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
			sprintf(t_ch, "inStream.writeInt(%d);\n", i);
			strEnum.append(t_ch);

			strEnum.append(strTable_5);
			strEnum.append("break;\n");
		}
		else {
			strEnum.append(strTable_4);
			sprintf(t_ch, "case %s:\n", pEnum._ParNameList[i].c_str());
			strEnum.append(t_ch);

			strEnum.append(strTable_5);
			sprintf(t_ch, "inStream.writeInt(%d);\n", GetExistStringToIntValue(pEnum._ParNameList[i]));
			strEnum.append(t_ch);

			strEnum.append(strTable_5);
			strEnum.append("break;\n");
		}

	}

	strEnum.append(strTable_4);
	strEnum.append("defalut:\n");

	strEnum.append(strTable_5);
	strEnum.append("inStream.writeInt(0);\n");

	strEnum.append(strTable_5);
	strEnum.append("break;\n");

	strEnum.append(strTable_3);
	strEnum.append("}\n");

	strEnum.append(strTable_2);
	strEnum.append("}\n");

	strEnum.append("}\n");

	fwrite(strEnum.c_str(), strEnum.length(), 1, fp);

	fclose(fp);
}
void    ProtocolParse_Writer_Java::WriteStruct_Java(stNamespace& pNamespace, stStructStruct& pStruct, string foldername)
{
	if (pStruct._inherited.length() > 0)
		return;

	char t_ch[512];
	sprintf(t_ch, "%s/%s.java", foldername.c_str(), pStruct._name.c_str());

	//先创建文件
	FILE* fp = fopen(t_ch, "w");
	if (!fp)
		return;

	//写信息头,共用
	WriteInfoHead(fp);


	//string strTable_1=CreateTableString(1);
	string strTable_2 = CreateTableString(1);
	string strTable_3 = CreateTableString(2);
	string strTable_4 = CreateTableString(3);
	string strTable_5 = CreateTableString(4);
	string strTable_6 = CreateTableString(5);

	string strStruct;

	sprintf(t_ch, "package %s.%s;\n", m_strJavaPackage.c_str(), pNamespace._protocolnamelower.c_str());
	strStruct.append(t_ch);

	sprintf(t_ch, "import %s.*;\n", m_strJavaPackage.c_str());
	strStruct.append(t_ch);

	vector<string> tt_PacketNameList;
	GetUsePacketNamesByParType_Java(pStruct._name, tt_PacketNameList);
	RemovePacketNames(tt_PacketNameList, pNamespace._protocolnamelower);

	for (int i = 0; i < (int)tt_PacketNameList.size(); i++) {
		sprintf(t_ch, "import %s.%s.*;\n", m_strJavaPackage.c_str(), tt_PacketNameList[i].c_str());
		strStruct.append(t_ch);
	}


	strStruct.append(WriteComment_Java("", pStruct._comment, "").c_str());
	sprintf(t_ch, "public class %s\n", pStruct._name.c_str());
	strStruct.append(t_ch);
	strStruct.append("{\n");


	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		strStruct.append(WriteComment_Java(strTable_2, pStruct._ParVec[i]._comment, "").c_str());
		strStruct.append(strTable_2);
		if (IsExType_vector(pStruct._ParVec[i]._strParType)) {
			sprintf(t_ch, "public %s<%s> %s;\n", TypeConvertToJavaType(pStruct._ParVec[i]._strParType.c_str()).c_str(), GetExTypeParT_vector(pStruct._ParVec[i]._strParType).c_str(), pStruct._ParVec[i]._strParName.c_str());
		}
		else {
			sprintf(t_ch, "public %s %s;\n", TypeConvertToJavaType(pStruct._ParVec[i]._strParType.c_str()).c_str(), pStruct._ParVec[i]._strParName.c_str());
		}
		strStruct.append(t_ch);
	}

	//写读
	strStruct.append(strTable_2);
	sprintf(t_ch, "public static %s read(JUIOutputStream outStream)\n", pStruct._name.c_str());
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

	if (!pStruct.IsHasTransNeedValuePar()) {
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
			if (IsJavaBaseType(pStruct._ParVec[i]._strParType, false)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "o.%s = %s.read(outStream);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				else {
					sprintf(t_ch, "o.%s = outStream.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadJavaType(pStruct._ParVec[i]._strParType.c_str()).c_str());
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
			if (IsJavaBaseType(pStruct._ParVec[i]._strParType, false)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "o.%s = %s.read(outStream);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				else {
					sprintf(t_ch, "o.%s = outStream.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadJavaType(pStruct._ParVec[i]._strParType.c_str()).c_str());
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
			if (IsJavaBaseType(pStruct._ParVec[i]._strParType, false)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "o.%s = %s.read(outStream);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				else {
					sprintf(t_ch, "o.%s = outStream.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadJavaType(pStruct._ParVec[i]._strParType.c_str()).c_str());
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
			if (IsJavaBaseType(pStruct._ParVec[i]._strParType, false)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "o.%s = %s.read(outStream);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				else {
					sprintf(t_ch, "o.%s = outStream.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadJavaType(pStruct._ParVec[i]._strParType.c_str()).c_str());
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
			if (IsJavaBaseType(pStruct._ParVec[i]._strParType, false)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "o.%s = %s.read(outStream);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				else {
					sprintf(t_ch, "o.%s = outStream.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadJavaType(pStruct._ParVec[i]._strParType.c_str()).c_str());
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
			if (IsJavaBaseType(pStruct._ParVec[i]._strParType, false)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "o.%s = %s.read(outStream);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				else {
					sprintf(t_ch, "o.%s = outStream.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadJavaType(pStruct._ParVec[i]._strParType.c_str()).c_str());
				}
			}
			else {
				sprintf(t_ch, "o.%s = %s.read(outStream);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
			}
			strStruct.append(t_ch);
		}




		strStruct.append(strTable_4);
		strStruct.append("}\n");

		string tt_default = GetDefaultValue(pStruct._ParVec[i]._strParType, false, true);
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

	//写写
	strStruct.append(strTable_2);
	strStruct.append("public void write(JUIInputStream inStream)\n");
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
			sprintf(t_ch, "if(v.%s!=%s)\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strDefaultValue.c_str());
			strStruct.append(t_ch);
			strStruct.append(strTable_3);
			strStruct.append("{\n");


			strStruct.append(strTable_4);
			if (IsJavaBaseType(pStruct._ParVec[i]._strParType, true)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "%s.write(inStream,%s);\n", pStruct._ParVec[i]._strParType.c_str(), pStruct._ParVec[i]._strParName.c_str());
				}
				else
					sprintf(t_ch, "inStream.%s(%s);\n", TypeConvertToWriteJavaType(pStruct._ParVec[i]._strParType.c_str()).c_str(), pStruct._ParVec[i]._strParName.c_str());
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
			if (IsJavaBaseType(pStruct._ParVec[i]._strParType, true)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "%s.write(inStream,%s);\n", pStruct._ParVec[i]._strParType.c_str(), pStruct._ParVec[i]._strParName.c_str());
				}
				else
					sprintf(t_ch, "inStream.%s(%s);\n", TypeConvertToWriteJavaType(pStruct._ParVec[i]._strParType.c_str()).c_str(), pStruct._ParVec[i]._strParName.c_str());
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



	strStruct.append("}\n");

	fwrite(strStruct.c_str(), strStruct.length(), 1, fp);

	fclose(fp);
}

void    ProtocolParse_Writer_Java::WriteVector_Java(stNamespace& pNamespace, stVector& pVector, string foldername)
{
	char t_ch[512];
	sprintf(t_ch, "%s/%s.java", foldername.c_str(), pVector._name.c_str());

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

	string strVector;


	sprintf(t_ch, "package %s.%s;\n", m_strJavaPackage.c_str(), pNamespace._protocolnamelower.c_str());
	strVector.append(t_ch);

	sprintf(t_ch, "import %s.*;\n", m_strJavaPackage.c_str());
	strVector.append(t_ch);

	vector<string> tt_PacketNameList;
	GetUsePacketNamesByParType_Java(pVector._ParTName, tt_PacketNameList);
	RemovePacketNames(tt_PacketNameList, pNamespace._protocolnamelower);

	for (int i = 0; i < (int)tt_PacketNameList.size(); i++) {
		sprintf(t_ch, "import %s.%s.*;\n", m_strJavaPackage.c_str(), tt_PacketNameList[i].c_str());
		strVector.append(t_ch);
	}


	/////////////有两种版本，一种是非Flash基础类型，一种是
	bool  tt_bJavaBaseType = IsJavaBaseType2(pVector._ParTName);


	strVector.append(WriteComment_Java("", pVector._comment, "").c_str());
	sprintf(t_ch, "public  class %s<T> extends JUIVector<T>\n", pVector._name.c_str());
	strVector.append(t_ch);
	strVector.append("{\n");

	strVector.append(strTable_2);
	sprintf(t_ch, "private static final long serialVersionUID = %d;\n", CreateSerialVersionUID(pVector._name));
	strVector.append(t_ch);

	strVector.append(strTable_2);
	sprintf(t_ch, "public %s()\n", pVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	strVector.append("super();\n");

	strVector.append(strTable_2);
	strVector.append("}\n");


	strVector.append(strTable_2);
	sprintf(t_ch, "public static %s<%s> read(JUIOutputStream outStream)\n", pVector._name.c_str(), pVector._ParTName.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	sprintf(t_ch, "%s<%s> o = new %s<%s>();\n", pVector._name.c_str(), pVector._ParTName.c_str(), pVector._name.c_str(), pVector._ParTName.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_3);
	strVector.append("int count = outStream.ReadInt();\n");



	strVector.append(strTable_3);
	strVector.append("for(int i=0;i<count;i++)\n");

	strVector.append(strTable_3);
	strVector.append("{\n");

	if (!tt_bJavaBaseType) {
		strVector.append(strTable_4);
		sprintf(t_ch, "o.addElement(%s.read(outStream));\n", pVector._ParTName.c_str());
		strVector.append(t_ch);
	}
	else {
		strVector.append(strTable_4);
		sprintf(t_ch, "o.addElement(outStream.%s());\n", TypeConvertToReadJavaType(pVector._ParTName).c_str());
		strVector.append(t_ch);
	}

	strVector.append(strTable_3);
	strVector.append("}\n");

	strVector.append(strTable_3);
	strVector.append("return o;\n");

	strVector.append(strTable_2);
	strVector.append("}\n");

	strVector.append(strTable_2);
	strVector.append("public void write(JUIInputStream inStream)\n");

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	strVector.append("inStream.WriteInt(size());\n");

	strVector.append(strTable_3);
	strVector.append("for(int i=0;i<size();i++)\n");

	strVector.append(strTable_3);
	strVector.append("{\n");
	//
	if (!tt_bJavaBaseType) {
		strVector.append(strTable_4);
		sprintf(t_ch, "((%s)get(i)).write(inStream);\n", pVector._ParTName.c_str()/*,pVector._ParTName.c_str()*/);
		strVector.append(t_ch);
	}
	else {
		strVector.append(strTable_4);
		sprintf(t_ch, "inStream.%s((%s)get(i));\n", TypeConvertToWriteJavaType(pVector._ParTName).c_str(), TypeConvertToJavaType(pVector._ParTName).c_str());
		strVector.append(t_ch);
	}


	strVector.append(strTable_3);
	strVector.append("}\n");

	strVector.append(strTable_2);
	strVector.append("}\n");

	strVector.append("}\n");



	fwrite(strVector.c_str(), strVector.length(), 1, fp);

	fclose(fp);
}




void    ProtocolParse_Writer_Java::WriteClass_Java(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername)
{
	switch (filetype) {
	case 0:
	{
		////对于客户端而言，request的为发送，callback的为回调
		if (pClass._strtype.compare("callback") == 0) {
			WriteClass_IReceiver_Java(pClass, filetype, pNamespace, foldername, false);
			WriteClass_Receiver_Java(pClass, filetype, pNamespace, foldername);
		}
		else {
			WriteClass_Send_Java(pClass, filetype, pNamespace, foldername);
		}
	}
	break;
	case 1:
	{
		////服务器而言，请求的是回调，callback是发送
		if (pClass._strtype.compare("request") == 0) //C2S
		{
			WriteClass_IReceiver_Java(pClass, filetype, pNamespace, foldername, false);
			WriteClass_Receiver_Java(pClass, filetype, pNamespace, foldername);
		}
		else //S2C
		{
			WriteClass_Send_Java(pClass, filetype, pNamespace, foldername);
		}
	}
	break;
	case 2:
	{

		//事实上两个里面是一样的
		if (pClass._strtype.compare("request") == 0) //C2S
		{
			WriteClass_IReceiver_Java(pClass, filetype, pNamespace, foldername, true);
			WriteClass_Receiver_Java(pClass, filetype, pNamespace, foldername);
			WriteClass_Send_Java(pClass, filetype, pNamespace, foldername);
		}

	}
	break;
	default:
		break;
	}
}
void    ProtocolParse_Writer_Java::WriteClass_IReceiver_Java(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, bool bemptyimplement)//接收接口
{
	char t_ch[1024];
	sprintf(t_ch, "%s/IReceiver_%s.java", foldername.c_str(), pNamespace._protocolname.c_str());

	//先创建文件
	FILE* fp = fopen(t_ch, "w");
	if (!fp)
		return;

	//写信息头,共用
	WriteInfoHead(fp);

	string strclass;

	//string strTable_1=CreateTableString(1);
	string strTable_2 = CreateTableString(1);
	string strTable_3 = CreateTableString(2);

	sprintf(t_ch, "package %s.%s;\n", m_strJavaPackage.c_str(), pNamespace._protocolnamelower.c_str());
	strclass.append(t_ch);

	sprintf(t_ch, "import %s.*;\n", m_strJavaPackage.c_str());
	strclass.append(t_ch);

	vector<string> tt_usePacketNameList;
	GetUsePacketNamesByRefHead_Java(tt_usePacketNameList);

	for (int i = 0; i < (int)tt_usePacketNameList.size(); i++) {
		sprintf(t_ch, "import %s.%s.*;\n", m_strJavaPackage.c_str(), tt_usePacketNameList[i].c_str());
		strclass.append(t_ch);
	}

	sprintf(t_ch, "public interface IReceiver_%s\n", pNamespace._protocolname.c_str());
	strclass.append(t_ch);

	strclass.append("{\n");
	fwrite(strclass.c_str(), strclass.length(), 1, fp);


	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		if (filetype == 2 && ((m_bSCSpliteS && pClass._FuncVec[i]._SCSpliteType == -1) || (m_bSCSpliteC && pClass._FuncVec[i]._SCSpliteType == 1)))
			continue;
		//写函数
		WriteFunc_IReceiver_Java(pClass._FuncVec[i], filetype, 2, pClass, pNamespace, foldername, fp, bemptyimplement);
	}

	strclass = "";
	strclass.append("}\n");

	fwrite(strclass.c_str(), strclass.length(), 1, fp);
	fclose(fp);


}

void    ProtocolParse_Writer_Java::WriteFunc_IReceiver_Java(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp, bool bemptyimplement) //
{
	char t_ch[1024];

	string strTable_0 = CreateTableString(tablenum);
	string strTable_1 = CreateTableString(tablenum + 1);


	string strFunc;

	strFunc.append(WriteComment_Java(strTable_0, pFunc._comment, "").c_str());
	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		strFunc.append(WriteComment_Java(strTable_0, pFunc._InParVec[i]._comment, pFunc._InParVec[i]._strParName).c_str());
	}


	strFunc.append(strTable_0);
	sprintf(t_ch, "public void IReceiver_%s(", pFunc._name.c_str());
	strFunc.append(t_ch);

	if (filetype == 0) {
		if (pFunc._InParVec.size() > 0) {
			for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
				if (IsExType_vector(pFunc._InParVec[i]._strParType)) {
					sprintf(t_ch, "%s<%s> %s,", TypeConvertToJavaType(pFunc._InParVec[i]._strParType).c_str(), GetExTypeParT_vector(pFunc._InParVec[i]._strParType).c_str(), pFunc._InParVec[i]._strParName.c_str());
					strFunc.append(t_ch);
				}
				else {
					sprintf(t_ch, "%s %s,", TypeConvertToJavaType(pFunc._InParVec[i]._strParType).c_str(), pFunc._InParVec[i]._strParName.c_str());
					strFunc.append(t_ch);
				}

			}
		}

		strFunc.append("Object pExData);\n");
	}
	else {
		if (pFunc._InParVec.size() > 0) {
			for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
				if (IsExType_vector(pFunc._InParVec[i]._strParType)) {
					sprintf(t_ch, "%s<%s> %s,", TypeConvertToJavaType(pFunc._InParVec[i]._strParType).c_str(), GetExTypeParT_vector(pFunc._InParVec[i]._strParType).c_str(), pFunc._InParVec[i]._strParName.c_str());
					strFunc.append(t_ch);
				}
				else {
					sprintf(t_ch, "%s %s,", TypeConvertToJavaType(pFunc._InParVec[i]._strParType).c_str(), pFunc._InParVec[i]._strParName.c_str());
					strFunc.append(t_ch);
				}

			}
		}

		strFunc.append("int sessionId,Object pExData);\n");
	}


	fwrite(strFunc.c_str(), strFunc.length(), 1, fp);
}
void    ProtocolParse_Writer_Java::WriteClass_Receiver_Java(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername)
{
	char t_ch[1024];
	sprintf(t_ch, "%s/%s_Callback.java", foldername.c_str(), pNamespace._protocolname.c_str());

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


	sprintf(t_ch, "package %s.%s;\n", m_strJavaPackage.c_str(), pNamespace._protocolnamelower.c_str());
	strclass.append(t_ch);

	sprintf(t_ch, "import %s.*;\n", m_strJavaPackage.c_str());
	strclass.append(t_ch);

	vector<string> tt_usePacketNameList;
	GetUsePacketNamesByRefHead_Java(tt_usePacketNameList);

	for (int i = 0; i < (int)tt_usePacketNameList.size(); i++) {
		sprintf(t_ch, "import %s.%s.*;\n", m_strJavaPackage.c_str(), tt_usePacketNameList[i].c_str());
		strclass.append(t_ch);
	}


	sprintf(t_ch, "public class %s_Callback\n", pNamespace._protocolname.c_str());
	strclass.append(t_ch);

	strclass.append("{\n");
	fwrite(strclass.c_str(), strclass.length(), 1, fp);

	WriteFunc_ReceiverParser_Java(pClass, filetype, 2, pNamespace, foldername, fp);


	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		if (filetype == 2 && ((m_bSCSpliteS && pClass._FuncVec[i]._SCSpliteType == -1) || (m_bSCSpliteC && pClass._FuncVec[i]._SCSpliteType == 1)))
			continue;
		WriteFunc_Receiver_Java(pClass._FuncVec[i], filetype, 2, pClass, pNamespace, foldername, fp);
	}


	strclass = "";
	strclass.append("}\n");

	fwrite(strclass.c_str(), strclass.length(), 1, fp);
	fclose(fp);

}

void    ProtocolParse_Writer_Java::WriteFunc_ReceiverParser_Java(stClass& pClass, int filetype, int tablenum, stNamespace& pNamespace, string foldername, FILE* fp)
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
		sprintf(t_ch, "public static Boolean Parser(IReceiver_%s cls,JUIOutputStream outStream,Object pExData)\n", pNamespace._protocolname.c_str());
		strFunc.append(t_ch);
	}
	else {
		sprintf(t_ch, "public static Boolean Parser(IReceiver_%s cls,JUIOutputStream outStream,int sessionId,Object pExData)\n", pNamespace._protocolname.c_str());
		strFunc.append(t_ch);
	}

	strFunc.append(strTable_0);
	strFunc.append("{\n");

	strFunc.append(strTable_1);
	strFunc.append("int no = outStream.ReadInt();\n");

	strFunc.append(strTable_1);
	sprintf(t_ch, "if(no!=ID_%s.%s)\n", pNamespace._protocolnamelower.c_str(), m_ProtocolNo._name.c_str());
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
		sprintf(t_ch, "case ID_%s.ID_%s_%s:\n", pNamespace._protocolnamelower.c_str(), pClass._name.c_str(), pClass._FuncVec[i]._name.c_str());
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


	//strFunc.append(strTable_1);
	//strFunc.append("return false;\n");

	strFunc.append(strTable_0);
	strFunc.append("}\n");


	fwrite(strFunc.c_str(), strFunc.length(), 1, fp);
}

void    ProtocolParse_Writer_Java::WriteFunc_Receiver_Java(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp)
{
	char t_ch[1024];

	string strTable_0 = CreateTableString(tablenum);
	string strTable_1 = CreateTableString(tablenum + 1);
	string strTable_2 = CreateTableString(tablenum + 2);
	string strTable_3 = CreateTableString(tablenum + 3);


	string strFunc;

	if (filetype == 0) {
		strFunc.append(strTable_0);
		sprintf(t_ch, "private static void _parser_%s(IReceiver_%s cls,JUIOutputStream outStream,Object pExData,int proversionno)\n", pFunc._name.c_str(), pNamespace._protocolname.c_str());
		strFunc.append(t_ch);
	}
	else {
		strFunc.append(strTable_0);
		sprintf(t_ch, "private static void _parser_%s(IReceiver_%s cls,JUIOutputStream outStream,int sessionId,Object pExData,int proversionno)\n", pFunc._name.c_str(), pNamespace._protocolname.c_str());
		strFunc.append(t_ch);
	}

	strFunc.append(strTable_0);
	strFunc.append("{\n");

	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		strFunc.append(strTable_1);
		if (IsJavaBaseType(pFunc._InParVec[i]._strParType, false) || IsExType_enum(pFunc._InParVec[i]._strParType)) {
			sprintf(t_ch, "%s %s;\n", (TypeConvertToJavaType(pFunc._InParVec[i]._strParType)).c_str(), pFunc._InParVec[i]._strParName.c_str());

		}
		else if (IsExType_vector(pFunc._InParVec[i]._strParType)) {
			sprintf(t_ch, "%s<%s> %s;\n", (TypeConvertToJavaType(pFunc._InParVec[i]._strParType)).c_str(), GetExTypeParT_vector(pFunc._InParVec[i]._strParType).c_str(), pFunc._InParVec[i]._strParName.c_str());

		}
		else {
			sprintf(t_ch, "%s %s;\n", (TypeConvertToJavaType(pFunc._InParVec[i]._strParType)).c_str(), pFunc._InParVec[i]._strParName.c_str());
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
		if (IsJavaBaseType(pFunc._InParVec[i]._strParType, false)) {
			sprintf(t_ch, "%s = outStream.%s();\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToReadJavaType(pFunc._InParVec[i]._strParType)).c_str());

		}
		else if (IsExType_enum(pFunc._InParVec[i]._strParType)) {
			sprintf(t_ch, "%s = %s.read(outStream);\n", pFunc._InParVec[i]._strParName.c_str(), pFunc._InParVec[i]._strParType.c_str());
		}
		else {
			sprintf(t_ch, "%s = %s.read(outStream);\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToJavaType(pFunc._InParVec[i]._strParType)).c_str());
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
		if (IsJavaBaseType(pFunc._InParVec[i]._strParType, false)) {
			sprintf(t_ch, "%s = outStream.%s();\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToReadJavaType(pFunc._InParVec[i]._strParType)).c_str());
		}
		else if (IsExType_enum(pFunc._InParVec[i]._strParType)) {
			sprintf(t_ch, "%s = %s.read(outStream);\n", pFunc._InParVec[i]._strParName.c_str(), pFunc._InParVec[i]._strParType.c_str());
		}
		else {
			sprintf(t_ch, "%s = %s.read(outStream);\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToJavaType(pFunc._InParVec[i]._strParType)).c_str());
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
		if (IsJavaBaseType(pFunc._InParVec[i]._strParType, false)) {
			sprintf(t_ch, "%s = outStream.%s();\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToReadJavaType(pFunc._InParVec[i]._strParType)).c_str());

		}
		else if (IsExType_enum(pFunc._InParVec[i]._strParType)) {
			sprintf(t_ch, "%s = %s.read(ba);\n", pFunc._InParVec[i]._strParName.c_str(), pFunc._InParVec[i]._strParType.c_str());
		}
		else {
			sprintf(t_ch, "%s = %s.read(outStream);\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToJavaType(pFunc._InParVec[i]._strParType)).c_str());
		}
		strFunc.append(t_ch);

		string tt_default = GetDefaultValue(pFunc._InParVec[i]._strParType, false, true);
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
			sprintf(t_ch, "%s = new %s<%s>();\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToJavaType(pFunc._InParVec[i]._strParType)).c_str(), GetExTypeParT_vector(pFunc._InParVec[i]._strParType).c_str());
			strFunc.append(t_ch);
		}
		else if (IsExType_struct(pFunc._InParVec[i]._strParType)) {
			strFunc.append(strTable_2);
			strFunc.append("else\n");

			strFunc.append(strTable_3);
			sprintf(t_ch, "%s = new %s();\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToJavaType(pFunc._InParVec[i]._strParType)).c_str());
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

void    ProtocolParse_Writer_Java::WriteClass_Send_Java(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername)
{
	char t_ch[512];
	sprintf(t_ch, "%s/%s_Send.java", foldername.c_str(), pNamespace._protocolname.c_str());

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

	sprintf(t_ch, "package %s.%s;\n", m_strJavaPackage.c_str(), pNamespace._protocolnamelower.c_str());
	strclass.append(t_ch);

	sprintf(t_ch, "import %s.*;\n", m_strJavaPackage.c_str());
	strclass.append(t_ch);

	vector<string> tt_usePacketNameList;
	GetUsePacketNamesByRefHead_Java(tt_usePacketNameList);

	for (int i = 0; i < (int)tt_usePacketNameList.size(); i++) {
		sprintf(t_ch, "import %s.%s.*;\n", m_strJavaPackage.c_str(), tt_usePacketNameList[i].c_str());
		strclass.append(t_ch);
	}


	sprintf(t_ch, "public class %s_Send\n", pNamespace._protocolname.c_str());
	strclass.append(t_ch);

	strclass.append("{\n");


	fwrite(strclass.c_str(), strclass.length(), 1, fp);
	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		if (filetype == 2 && ((m_bSCSpliteS && pClass._FuncVec[i]._SCSpliteType == 1) || (m_bSCSpliteC && pClass._FuncVec[i]._SCSpliteType == -1)))
			continue;
		//写函数
		WriteFunc_SendByteSeq_Java(pClass._FuncVec[i], 2, pClass, pNamespace, foldername, fp);
	}


	strclass = "";
	strclass.append("}\n");




	fwrite(strclass.c_str(), strclass.length(), 1, fp);

	fclose(fp);


}

void    ProtocolParse_Writer_Java::WriteFunc_SendByteSeq_Java(stFunc& pFunc, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp)//发送流
{
	char t_ch[1024];


	string strTable_0 = CreateTableString(tablenum);
	string strTable_1 = CreateTableString(tablenum + 1);
	string strTable_2 = CreateTableString(tablenum + 2);

	string strfunc;

	strfunc.append(WriteComment_Java(strTable_0, pFunc._comment, "").c_str());
	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		strfunc.append(WriteComment_Java(strTable_0, pFunc._InParVec[i]._comment, pFunc._InParVec[i]._strParName).c_str());
	}

	strfunc.append(strTable_0);
	sprintf(t_ch, "public static JUIInputStream Send_%s(", pFunc._name.c_str());
	strfunc.append(t_ch);

	//有参
	if (pFunc._InParVec.size() > 0) {
		for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
			if (i == pFunc._InParVec.size() - 1) {
				sprintf(t_ch, "%s %s", TypeConvertToJavaType(pFunc._InParVec[i]._strParType).c_str(), pFunc._InParVec[i]._strParName.c_str());
				strfunc.append(t_ch);
			}
			else {
				sprintf(t_ch, "%s %s,", TypeConvertToJavaType(pFunc._InParVec[i]._strParType).c_str(), pFunc._InParVec[i]._strParName.c_str());
				strfunc.append(t_ch);
			}

		}

	}

	strfunc.append(")\n");
	strfunc.append(strTable_0);
	strfunc.append("{\n");

	strfunc.append(strTable_1);
	strfunc.append("JUIInputStream inStream = new JUIInputStream();\n");

	//先写协议号
	strfunc.append(strTable_1);
	sprintf(t_ch, "inStream.WriteInt(ID_%s.%s);\n", pNamespace._protocolnamelower.c_str(), m_ProtocolNo._name.c_str());
	strfunc.append(t_ch);

	//写版本号

	strfunc.append(strTable_1);
	sprintf(t_ch, "int tt_version = %d;\n", pFunc._version);
	strfunc.append(t_ch);
	strfunc.append(strTable_1);
	strfunc.append("inStream.WriteShort((short)tt_version);\n");

	//再写函数号
	strfunc.append(strTable_1);
	sprintf(t_ch, "inStream.WriteInt(ID_%s.ID_%s_%s);\n", pNamespace._protocolnamelower.c_str(), pClass._name.c_str(), pFunc._name.c_str());
	strfunc.append(t_ch);

	//
	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		strfunc.append(strTable_1);
		if (IsJavaBaseType(pFunc._InParVec[i]._strParType, true)) {
			if (IsExType_enum(pFunc._InParVec[i]._strParType)) {
				sprintf(t_ch, "%s.write(inStream,%s);\n", pFunc._InParVec[i]._strParType.c_str(), pFunc._InParVec[i]._strParName.c_str());
			}
			else
				sprintf(t_ch, "inStream.%s(%s);\n", TypeConvertToWriteJavaType(pFunc._InParVec[i]._strParType.c_str()).c_str(), pFunc._InParVec[i]._strParName.c_str());

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

void    ProtocolParse_Writer_Java::GetUsePacketNamesByRefHead_Java(vector<string>& packetNamesList)
{
	for (size_t i = 0; i < m_RefProtocolList.size(); i++) {
		if (m_RefProtocolList[i]._usingspacename.length() > 0) {
			AddPacketNames(packetNamesList, ToLowerV2(m_RefProtocolList[i]._usingspacename));
		}
	}

	for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++) {
		if (m_pRefProtocolParseList[i])
			m_pRefProtocolParseList[i]->GetUsePacketNamesByRefHead(packetNamesList, true);
	}
}

void    ProtocolParse_Writer_Java::GetUsePacketNamesByParType_Java(const string& strParType, vector<string>& packetNamesList)
{
	for (int i = 0; i < (int)m_NamespaceVec.size(); i++) {
		for (int j = 0; j < (int)m_NamespaceVec[i]._StructVec.size(); j++) {
			if (m_NamespaceVec[i]._StructVec[j]._name.compare(strParType.c_str()) == 0) {
				AddPacketNames(packetNamesList, m_NamespaceVec[i]._protocolnamelower);

				for (int k = 0; k < (int)m_NamespaceVec[i]._StructVec[j]._ParVec.size(); k++) {
					if (!IsJavaBaseType(m_NamespaceVec[i]._StructVec[j]._ParVec[k]._strParType, false)) {
						GetUsePacketNamesByParType_Java(m_NamespaceVec[i]._StructVec[j]._ParVec[k]._strParType, packetNamesList);
					}
				}

				return;
			}
		}

		for (int j = 0; j < (int)m_NamespaceVec[i]._VectorVec.size(); j++) {
			if (m_NamespaceVec[i]._VectorVec[j]._name.compare(strParType.c_str()) == 0) {
				AddPacketNames(packetNamesList, m_NamespaceVec[i]._protocolnamelower);

				if (!IsJavaBaseType(m_NamespaceVec[i]._VectorVec[j]._ParTName, false)) {
					GetUsePacketNamesByParType_Java(m_NamespaceVec[i]._VectorVec[j]._ParTName, packetNamesList);
				}

				return;
			}
		}

		for (int j = 0; j < (int)m_NamespaceVec[i]._WJSVectorVec.size(); j++) {
			if (m_NamespaceVec[i]._WJSVectorVec[j]._name.compare(strParType.c_str()) == 0) {
				AddPacketNames(packetNamesList, m_NamespaceVec[i]._protocolnamelower);

				if (!IsJavaBaseType(m_NamespaceVec[i]._WJSVectorVec[j]._ParTName, false)) {
					GetUsePacketNamesByParType_Java(m_NamespaceVec[i]._WJSVectorVec[j]._ParTName, packetNamesList);
				}

				return;
			}
		}

		for (int j = 0; j < (int)m_NamespaceVec[i]._EnumVec.size(); j++) {
			if (m_NamespaceVec[i]._EnumVec[j]._name.compare(strParType.c_str()) == 0) {
				AddPacketNames(packetNamesList, m_NamespaceVec[i]._protocolnamelower);

				return;
			}
		}
	}

	for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++) {
		if (m_pRefProtocolParseList[i])
			((ProtocolParse_Writer_Java*)m_pRefProtocolParseList[i])->GetUsePacketNamesByParType_Java(strParType, packetNamesList);
	}
}



void    ProtocolParse_Writer_Java::SetJavaPackName(const string& strJavaPackName)
{
	if (strJavaPackName.length() == 0)
		return;
	m_strJavaPackage = strJavaPackName;

	for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++) {
		if (m_pRefProtocolParseList[i]) {
			((ProtocolParse_Writer_Java*)m_pRefProtocolParseList[i])->SetJavaPackName(strJavaPackName);
		}
	}
}


string  ProtocolParse_Writer_Java::WriteComment_Java(const string& strTable, const string& strComment, const string& strPar)
{
	if (strComment.length() == 0)
		return "";
	char tt_ch[2048];

	string strLine;

	vector<string> tt_subList;
	ProtocolUtils::GetAllSubString(strComment, "split", tt_subList, false);

	for (unsigned int i = 0; i < tt_subList.size(); i++) {
		strLine.append(strTable);
		if (strPar.length() > 0 && i == 0) {
			sprintf(tt_ch, "//%s  %s\n", strPar.c_str(), tt_subList[i].c_str());

		}
		else {
			sprintf(tt_ch, "//%s\n", tt_subList[i].c_str());
		}

		strLine.append(tt_ch);

	}
	return strLine;
}
#pragma region 类型转换与判断
bool    ProtocolParse_Writer_Java::IsJavaBaseType(const string& strTypeName, bool bWrite)
{
	if (IsBaseType_WriterType(strTypeName, eProtocolWriterType::Java))
		return true;

	if (bWrite) {
		//枚举类型作为基本类型
		if (IsExType_enum(strTypeName))
			return true;
	}

	return false;
}

bool    ProtocolParse_Writer_Java::IsJavaBaseType2(string strTypeName)
{
	if (IsBaseType(strTypeName))
		return true;

	return false;
}
string  ProtocolParse_Writer_Java::TypeConvertToJavaType(const string& strTypeName)
{
	for (int i = 0; i < GetBaseTypeNum(); i++) {
		if (g_strBaseType[i].compare(strTypeName.c_str()) == 0) {
			switch (i) {
			case 0://void
				return strTypeName;
			case 1://int
				return "int";
			case 2://unsigned int
				return "int";
			case 3://short
				return "short";
			case 4://unsigned short
				return "short";
			case 5://char
				return "byte";


			case 11://float
				return "float";
			case 12://long
				return "int";
			case 13://unsigned long
				return "int";

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
				return "Boolean";
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
string  ProtocolParse_Writer_Java::TypeConvertToReadJavaType(const string& strTypeName)
{
	for (int i = 0; i < GetBaseTypeNum(); i++) {
		if (g_strBaseType[i].compare(strTypeName.c_str()) == 0) {
			switch (i) {
			case 0://void
				return "ReadInt";
			case 1://int
				return "ReadInt";
			case 2://unsigned int
				return "ReadInt";
			case 3://short
				return "ReadShort";
			case 4://unsigned short
				return "ReadShort";
			case 5://char
				return "ReadByte";


			case 11://float
				return "ReadFloat";
			case 12://long
				return "ReadInt";
			case 13://unsigned long
				return "ReadInt";



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
				return "ReadByte";
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

string  ProtocolParse_Writer_Java::TypeConvertToWriteJavaType(const string& strTypeName)
{
	for (int i = 0; i < GetBaseTypeNum(); i++) {
		if (g_strBaseType[i].compare(strTypeName.c_str()) == 0) {
			switch (i) {
			case 0://void
				return "WriteInt";
			case 1://int
				return "WriteInt";
			case 2://unsigned int
				return "WriteInt";
			case 3://short
				return "WriteShort";
			case 4://unsigned short
				return "WriteShort";
			case 5://char
				return "WriteByte";


			case 11://float
				return "WriteFloat";
			case 12://long
				return "WriteInt";
			case 13://unsigned long
				return "WriteInt";



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