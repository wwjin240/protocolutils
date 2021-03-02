#include "ProtocolParse_Writer_Python.h"
#include "ProtocolParse.h"
#include <time.h>
#include <sys/timeb.h>

extern string g_strBaseType[];

ProtocolParse_Writer_Python::ProtocolParse_Writer_Python(const string& strPathName)
	:ProtocolParse_Writer(strPathName, eProtocolWriterType::Python)
{

}
ProtocolParse_Writer_Python::~ProtocolParse_Writer_Python()
{

}
bool    ProtocolParse_Writer_Python::CreateFile_Path(string path, int filetype, bool brefprotocol)
{
	return CreateFile_Path_Python(path, filetype, brefprotocol);
}
ProtocolParse_Writer* ProtocolParse_Writer_Python::Create_RefProtocolParse_Writer(const string& strPathName)
{
	return new ProtocolParse_Writer_Python(strPathName);
}
bool    ProtocolParse_Writer_Python::CreateFile_Path_Python(string path, int filetype, bool brefprotocol)
{
	if (filetype < 0 || filetype>2)
		return false;

	//按协议名来建文建夹
	if (m_NamespaceVec.size() > 0) {
		if (brefprotocol) {
			char tt_ch[256];
			m_SaveFileName._servername = m_NamespaceVec[0]._protocolname;
			sprintf(tt_ch, "%sProtocol.py", m_NamespaceVec[0]._protocolname.c_str());
			m_SaveFileName._servername2 = tt_ch;

			m_SaveFileName._clientname = m_NamespaceVec[0]._protocolname;
			sprintf(tt_ch, "%sProtocol.py", m_NamespaceVec[0]._protocolname.c_str());
			m_SaveFileName._clientname2 = tt_ch;


		}
		else {
			char tt_ch[256];

			m_SaveFileName._servername = m_NamespaceVec[0]._protocolname;
			sprintf(tt_ch, "%sProtocol_server.py", m_NamespaceVec[0]._protocolname.c_str());
			m_SaveFileName._servername2 = tt_ch;

			m_SaveFileName._clientname = m_NamespaceVec[0]._protocolname;
			sprintf(tt_ch, "%sProtocol_client.py", m_NamespaceVec[0]._protocolname.c_str());
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
		str1.append(tt_strAddPath);
		return CreateFile_Python(str1, filetype, brefprotocol);
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
		str1.append(tt_strAddPath);
		return CreateFile_Python(str1, filetype, brefprotocol);
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
		return CreateFile_Python(str1, filetype, brefprotocol);
	}
	break;
	default:
		return false;
		break;
	}

}

bool    ProtocolParse_Writer_Python::CreateFile_Python(string foldername, int filetype, bool bRefProtocol)
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
		WriteOneNameSpace_Python(m_NamespaceVec[i], filetype, foldername);
	}


	return true;
}
bool    ProtocolParse_Writer_Python::WriteOneNameSpace_Python(stNamespace& pNamespace, int filetype, string foldername)
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
	CreateAndWriteFuncIdAndProtocolNo_Python(pNamespace, foldername, tt_strWriteNameSpace);


	//先写枚举
	for (size_t i = 0; i < pNamespace._EnumVec.size(); i++) {
		WriteEnum_Python(pNamespace, pNamespace._EnumVec[i], foldername, tt_strWriteNameSpace);
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
				WriteStruct_Python(pNamespace, pNamespace._StructVec[structindex], foldername, tt_strWriteNameSpace);
				structindex++;
			}
			else if (pNamespace._VectorVec[vectorindex]._index < pNamespace._StructVec[structindex]._index
				&& (pNamespace._VectorVec[vectorindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index)) {
				//写vector
				WriteVector_Python(pNamespace, pNamespace._VectorVec[vectorindex], foldername, tt_strWriteNameSpace);
				vectorindex++;
			}
			else {
				//写类vector
				WriteWJSVector_Python(pNamespace, pNamespace._WJSVectorVec[wjsvectorindex], foldername, tt_strWriteNameSpace);
				wjsvectorindex++;
			}
		}
		else if (!bvectorend && (!bstructend)) {
			if (pNamespace._StructVec[structindex]._index < pNamespace._VectorVec[vectorindex]._index) {
				//写结构体
				WriteStruct_Python(pNamespace, pNamespace._StructVec[structindex], foldername, tt_strWriteNameSpace);
				structindex++;
			}
			else {
				//写vector
				WriteVector_Python(pNamespace, pNamespace._VectorVec[vectorindex], foldername, tt_strWriteNameSpace);
				vectorindex++;
			}
		}
		else if (!bvectorend && (!bwjsvectorend)) {
			if (pNamespace._VectorVec[vectorindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index) {
				//写vector
				WriteVector_Python(pNamespace, pNamespace._VectorVec[vectorindex], foldername, tt_strWriteNameSpace);
				vectorindex++;
			}
			else {
				//写类vector
				WriteWJSVector_Python(pNamespace, pNamespace._WJSVectorVec[wjsvectorindex], foldername, tt_strWriteNameSpace);
				wjsvectorindex++;
			}
		}
		else if ((!bstructend) && (!bwjsvectorend)) {
			if (pNamespace._StructVec[structindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index) {
				//写结构体
				WriteStruct_Python(pNamespace, pNamespace._StructVec[structindex], foldername, tt_strWriteNameSpace);
				structindex++;
			}
			else {
				//写类vector
				WriteWJSVector_Python(pNamespace, pNamespace._WJSVectorVec[wjsvectorindex], foldername, tt_strWriteNameSpace);
				wjsvectorindex++;
			}
		}
		else if (!bvectorend) {
			//写vector
			WriteVector_Python(pNamespace, pNamespace._VectorVec[vectorindex], foldername, tt_strWriteNameSpace);
			vectorindex++;

		}
		else if (!bstructend) {
			//写结构体
			WriteStruct_Python(pNamespace, pNamespace._StructVec[structindex], foldername, tt_strWriteNameSpace);
			structindex++;
		}
		else if (!bwjsvectorend) {
			//写类vector
			WriteWJSVector_Python(pNamespace, pNamespace._WJSVectorVec[wjsvectorindex], foldername, tt_strWriteNameSpace);
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
		WriteClass_Python(pNamespace._ClassVec[i], filetype, pNamespace, foldername, tt_strWriteNameSpace);
	}

	return true;
}

void    ProtocolParse_Writer_Python::CreateAndWriteFuncIdAndProtocolNo_Python(stNamespace& pNamespace, string foldername, const string& strWriteNameSpace)
{
	if (pNamespace._ClassVec.size() == 0)
		return;

	char t_ch[512];
	sprintf(t_ch, "%s/ID_%s.py", foldername.c_str(), pNamespace._protocolname.c_str());

	//先创建文件
	FILE* fp = fopen(t_ch, "w");
	if (!fp)
		return;

	//写信息头,共用
	WriteInfoHead(fp, false, true);

	string strid;

	string strTable_0 = CreateTableString(0);
	string strTable_1 = CreateTableString(1);




	strid.append(strTable_0);
	sprintf(t_ch, "class ID_%s():\n", pNamespace._protocolname.c_str());
	strid.append(t_ch);

	strid.append("\n");

	//函数id
	for (size_t i = 0; i < pNamespace._ClassVec.size(); i++) {
		for (size_t j = 0; j < pNamespace._ClassVec[i]._FuncVec.size(); j++) {
			strid.append(strTable_1);
			sprintf(t_ch, "ID_%s_%s", pNamespace._ClassVec[i]._name.c_str(), pNamespace._ClassVec[i]._FuncVec[j]._name.c_str());
			strid.append(t_ch);
			sprintf(t_ch, "ID_%s_%s", pNamespace._ClassVec[i]._name.c_str(), pNamespace._ClassVec[i]._FuncVec[j]._name.c_str());

			int tempId;  //将字符串转成id
			if (pNamespace._ClassVec[i]._FuncVec[j]._userFuncId != 0)
				tempId = pNamespace._ClassVec[i]._FuncVec[j]._userFuncId;
			else
				tempId = CovertStringToInt(t_ch, pNamespace._ClassVec[i]._FuncVec[j]._bNew);
			sprintf(t_ch, " = %d\n", tempId);
			strid.append(t_ch);
		}
	}

	//协议号
	if (m_ProtocolNo._value != 1) {
		//协议头
		strid.append(strTable_1);
		sprintf(t_ch, "%s = %d\n", m_ProtocolNo._name.c_str(), m_ProtocolNo._value);
		strid.append(t_ch);
	}

	fwrite(strid.c_str(), strid.length(), 1, fp);

	fclose(fp);
}

void    ProtocolParse_Writer_Python::WriteEnum_Python(stNamespace& pNamespace, stEnum& pEnum, string foldername, const string& strWriteNameSpace)
{
	char t_ch[512];
	sprintf(t_ch, "%s/%s.py", foldername.c_str(), pEnum._name.c_str());

	//先创建文件
	FILE* fp = fopen(t_ch, "w");
	if (!fp)
		return;

	//写信息头,共用
	WriteInfoHead(fp, false, true);


	string strTable_1 = CreateTableString(0);
	string strTable_2 = CreateTableString(1);
	string strTable_3 = CreateTableString(2);
	string strTable_4 = CreateTableString(3);
	string strTable_5 = CreateTableString(4);
	//string strTable_6=CreateTableString(5);

	string strEnum;
	strEnum.append(strTable_1);
	strEnum.append("import sys\n");
	//strEnum.append(strTable_1);
	//strEnum.append("sys.path.append('..')\n");
	//strEnum.append(strTable_1);
	//strEnum.append("sys.path.append('../..')\n");
	strEnum.append(strTable_1);
	strEnum.append("from InOutStream import InStream\n");
	strEnum.append(strTable_1);
	strEnum.append("from InOutStream import OutStream\n");
	//strEnum.append(strTable_1);
	//strEnum.append("from enum import Enum\n");	
	strEnum.append("\n");

	strEnum.append(WriteComment_Python(strTable_1, pEnum._comment, "").c_str());




	string tt_strMName;
	sprintf(t_ch, "%s", pEnum._name.c_str());
	tt_strMName = t_ch;


	strEnum.append(strTable_1);
	sprintf(t_ch, "class %s():\n", tt_strMName.c_str());
	strEnum.append(t_ch);

	for (size_t i = 0; i < pEnum._ParNameList.size(); i++) {
		strEnum.append(WriteComment_Python(strTable_2, pEnum._commentList[i], "").c_str());
		strEnum.append(strTable_2);
		sprintf(t_ch, "%s = %d\n", pEnum._ParNameList[i].c_str(), i/*CovertStringToInt(pEnum._ParNameList[i])*/);
		strEnum.append(t_ch);
	}
	strEnum.append("\n");


	//写读
	strEnum.append(strTable_2);
	strEnum.append("@staticmethod\n");
	strEnum.append(strTable_2);
	strEnum.append("def read(outStream):\n");

	strEnum.append(strTable_3);
	strEnum.append("tt_a = outStream.ReadInt()\n");


	for (size_t i = 0; i < pEnum._ParNameList.size(); i++) {
		if (pEnum._bNewVersion && (!pEnum._keepOldList[i])) {
			strEnum.append(strTable_3);
			if (i == 0) {
				sprintf(t_ch, "if tt_a == %d:\n", i);
				strEnum.append(t_ch);
			}
			else if (i == pEnum._ParNameList.size() - 1) {
				strEnum.append("else:\n");
			}
			else {
				sprintf(t_ch, "elif tt_a == %d:\n", i);
				strEnum.append(t_ch);
			}

			strEnum.append(strTable_4);
			sprintf(t_ch, "return %s.%s\n", tt_strMName.c_str(), pEnum._ParNameList[i].c_str());
			strEnum.append(t_ch);
		}
		else {
			strEnum.append(strTable_3);
			if (i == 0) {
				sprintf(t_ch, "if tt_a == %d:\n", CovertStringToInt(pEnum._ParNameList[i]));
				strEnum.append(t_ch);
			}
			else if (i == pEnum._ParNameList.size() - 1) {
				strEnum.append("else:\n");
			}
			else {
				sprintf(t_ch, "elif tt_a == %d:\n", CovertStringToInt(pEnum._ParNameList[i]));
				strEnum.append(t_ch);
			}

			strEnum.append(strTable_4);
			sprintf(t_ch, "return %s.%s\n", tt_strMName.c_str(), pEnum._ParNameList[i].c_str());
			strEnum.append(t_ch);
		}

	}


	strEnum.append("\n");


	//写写
	strEnum.append(strTable_2);
	//sprintf(t_ch, "def write(self,inStream,va):\n", tt_strMName.c_str());
	strEnum.append("def write(self,inStream,va):\n");

	for (size_t i = 0; i < pEnum._ParNameList.size(); i++) {
		if (pEnum._bNewVersion && (!pEnum._keepOldList[i])) {
			strEnum.append(strTable_3);
			if (i == 0) {
				sprintf(t_ch, "if va == %s.%s:\n", tt_strMName.c_str(), pEnum._ParNameList[i].c_str());
				strEnum.append(t_ch);
			}
			else if (i == pEnum._ParNameList.size() - 1) {
				strEnum.append("else:\n");
			}
			else {
				sprintf(t_ch, "elif va == %s.%s:\n", tt_strMName.c_str(), pEnum._ParNameList[i].c_str());
				strEnum.append(t_ch);
			}

			strEnum.append(strTable_4);
			sprintf(t_ch, "inStream.WriteInt(%d)\n", i);
			strEnum.append(t_ch);
		}
		else {
			strEnum.append(strTable_3);
			if (i == 0) {
				sprintf(t_ch, "if va == %s.%s:\n", tt_strMName.c_str(), pEnum._ParNameList[i].c_str());
				strEnum.append(t_ch);
			}
			else if (i == pEnum._ParNameList.size() - 1) {
				strEnum.append("else:\n");
			}
			else {
				sprintf(t_ch, "elif va == %s.%s:\n", tt_strMName.c_str(), pEnum._ParNameList[i].c_str());
				strEnum.append(t_ch);
			}

			strEnum.append(strTable_4);
			sprintf(t_ch, "inStream.WriteInt(%d)\n", GetExistStringToIntValue(pEnum._ParNameList[i]));
			strEnum.append(t_ch);
		}

	}


	strEnum.append("\n");
	fwrite(strEnum.c_str(), strEnum.length(), 1, fp);

	fclose(fp);
}
void    ProtocolParse_Writer_Python::WriteStruct_Python(stNamespace& pNamespace, stStructStruct& pStruct, string foldername, const string& strWriteNameSpace)
{
	if (pStruct._inherited.length() > 0)
		return;

	char t_ch[512];
	sprintf(t_ch, "%s/%s.py", foldername.c_str(), pStruct._name.c_str());

	//先创建文件
	FILE* fp = fopen(t_ch, "w");
	if (!fp)
		return;

	//写信息头,共用
	WriteInfoHead(fp, false, true);


	string strTable_1 = CreateTableString(0);
	string strTable_2 = CreateTableString(1);
	string strTable_3 = CreateTableString(2);
	string strTable_4 = CreateTableString(3);
	string strTable_5 = CreateTableString(4);
	string strTable_6 = CreateTableString(5);

	string strStruct;


	strStruct.append(WriteComment_Python(strTable_1, pStruct._comment, "").c_str());

	strStruct.append(strTable_1);
	strStruct.append("import sys\n");
	strStruct.append(strTable_1);
	strStruct.append("sys.path.append('..')\n");
	strStruct.append(strTable_1);
	strStruct.append("sys.path.append('../..')\n");
	strStruct.append(strTable_1);
	strStruct.append("from InOutStream import InStream\n");
	strStruct.append(strTable_1);
	strStruct.append("from InOutStream import OutStream\n");
	strStruct.append("\n");


	string tt_strMName;
	sprintf(t_ch, "%s", pStruct._name.c_str());
	tt_strMName = t_ch;

	vector<string> tt_PacketNameList;
	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		if (!IsPythonBaseType(pStruct._ParVec[i]._strParType, false)) {
			GetUsePacketNamesByParType_Python(pStruct._ParVec[i]._strParType, tt_PacketNameList);
		}
	}


	string tt_strSelf;
	sprintf(t_ch, "from %s.%s import %s", ProtocolUtils::PickEndFolderByPath(foldername).c_str(), pStruct._name.c_str(), pStruct._name.c_str());
	tt_strSelf = t_ch;
	RemovePacketNames(tt_PacketNameList, tt_strSelf, false);

	for (size_t i = 0; i < tt_PacketNameList.size(); i++) {
		strStruct.append(strTable_1);
		strStruct.append(tt_PacketNameList[i].c_str());
		strStruct.append("\n");
	}

	if (tt_PacketNameList.size() > 0)
		strStruct.append("\n");

	strStruct.append(strTable_1);
	sprintf(t_ch, "class %s():\n", tt_strMName.c_str());
	strStruct.append(t_ch);
	strStruct.append("\n");

	strStruct.append(strTable_2);
	strStruct.append("\"\"\"\n");
	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		strStruct.append(WriteComment_Python(strTable_2, pStruct._ParVec[i]._comment, "", true).c_str());
		strStruct.append(strTable_2);
		//strStruct.append("#");
		strStruct.append(pStruct._ParVec[i]._strParName.c_str());
		strStruct.append(": ");
		strStruct.append(pStruct._ParVec[i]._strParType.c_str());
		strStruct.append("\n");
	}
	strStruct.append(strTable_2);
	strStruct.append("\"\"\"\n");

	strStruct.append("\n");
	//写读
	strStruct.append(strTable_2);
	strStruct.append("@staticmethod\n");
	strStruct.append(strTable_2);
	strStruct.append("def read(outStream):\n");

	strStruct.append(strTable_3);
	sprintf(t_ch, "o = %s()\n", tt_strMName.c_str());
	strStruct.append(t_ch);

	strStruct.append(strTable_3);
	strStruct.append("o.readLocal(outStream)\n");

	strStruct.append(strTable_3);
	strStruct.append("return o\n");

	strStruct.append("\n");


	////////////////
	//
	///////////////
	//写本地读
	strStruct.append(strTable_2);
	strStruct.append("def readLocal(self, outStream) :\n");

	strStruct.append(strTable_3);
	strStruct.append("tt_len = outStream.ReadShort()\n");

	strStruct.append(strTable_3);
	strStruct.append("tt_version = outStream.ReadShort()\n");

	if (pStruct.IsHasTransNeedValuePar()) {
		strStruct.append(strTable_3);
		strStruct.append("tt_transNeedValue = outStream.ReadInt()\n");
	}

	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		if (pStruct._ParVec[i]._version > 1)
			break;
		if (pStruct.IsHasTransNeedValuePar() && (pStruct._ParVec[i]._transNeedValue != -1)) {
			strStruct.append(strTable_3);
			sprintf(t_ch, "if (tt_transNeedValue & (1<<%d))!=0:\n", pStruct._ParVec[i]._transNeedValue);
			strStruct.append(t_ch);

			strStruct.append(strTable_4);
			if (IsPythonBaseType(pStruct._ParVec[i]._strParType, false)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "self.%s = %s.read(outStream)\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				else {
					sprintf(t_ch, "self.%s = outStream.%s()\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadPythonType(pStruct._ParVec[i]._strParType.c_str()).c_str());
				}
			}
			else {
				sprintf(t_ch, "self.%s = %s.read(outStream)\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
			}
			strStruct.append(t_ch);

			strStruct.append(strTable_3);
			strStruct.append("else:\n");
			strStruct.append(strTable_4);
			sprintf(t_ch, "self.%s=%s\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strDefaultValue.c_str());
			strStruct.append(t_ch);

		}
		else {
			strStruct.append(strTable_3);
			if (IsPythonBaseType(pStruct._ParVec[i]._strParType, false)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "self.%s = %s.read(outStream)\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				else {
					sprintf(t_ch, "self.%s = outStream.%s()\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadPythonType(pStruct._ParVec[i]._strParType.c_str()).c_str());
				}
			}
			else {
				sprintf(t_ch, "self.%s = %s.read(outStream)\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
			}
			strStruct.append(t_ch);
		}

	}


	string ttt_strStruct;
	bool tt_bHasNewPar = false;

	if (pStruct._version > 1) {
		ttt_strStruct.append(strTable_3);
		sprintf(t_ch, "if tt_version == %d or tt_version==0:\n", pStruct._version);
		ttt_strStruct.append(t_ch);

		for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
			if (pStruct._ParVec[i]._version <= 1)
				continue;
			tt_bHasNewPar = true;
			if (pStruct.IsHasTransNeedValuePar() && (pStruct._ParVec[i]._transNeedValue != -1)) {
				ttt_strStruct.append(strTable_4);
				sprintf(t_ch, "if (tt_transNeedValue & (1<<%d))!=0:\n", pStruct._ParVec[i]._transNeedValue);
				ttt_strStruct.append(t_ch);

				ttt_strStruct.append(strTable_5);
				if (IsPythonBaseType(pStruct._ParVec[i]._strParType, false)) {
					//是否是枚举
					if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
						sprintf(t_ch, "self.%s = %s.read(outStream)\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
					}
					else {
						sprintf(t_ch, "self.%s = outStream.%s()\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadPythonType(pStruct._ParVec[i]._strParType.c_str()).c_str());
					}
				}
				else {
					sprintf(t_ch, "self.%s = %s.read(outStream)\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				ttt_strStruct.append(t_ch);

				ttt_strStruct.append(strTable_4);
				ttt_strStruct.append("else:\n");
				ttt_strStruct.append(strTable_5);
				sprintf(t_ch, "self.%s=%s\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strDefaultValue.c_str());
				ttt_strStruct.append(t_ch);

			}
			else {
				ttt_strStruct.append(strTable_4);
				if (IsPythonBaseType(pStruct._ParVec[i]._strParType, false)) {
					//是否是枚举
					if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
						sprintf(t_ch, "self.%s = %s.read(outStream)\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
					}
					else {
						sprintf(t_ch, "self.%s = outStream.%s()\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadPythonType(pStruct._ParVec[i]._strParType.c_str()).c_str());
					}
				}
				else {
					sprintf(t_ch, "self.%s = %s.read(outStream)\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				ttt_strStruct.append(t_ch);
			}

		}

		//if (pStruct._version <= 1)
		//{
		//	ttt_strStruct.append(strTable_4);
		//	ttt_strStruct.append("pass\n");
		//}
	}


	ttt_strStruct.append(strTable_3);
	if (pStruct._version > 1)
		sprintf(t_ch, "elif tt_version > %d :\n", pStruct._version);
	else
		sprintf(t_ch, "if tt_version > %d :\n", pStruct._version);

	ttt_strStruct.append(t_ch);

	ttt_strStruct.append(strTable_4);
	ttt_strStruct.append("tt_logsize1 = outStream.GetPos()\n");

	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		if (pStruct._ParVec[i]._version <= 1)
			continue;
		tt_bHasNewPar = true;
		if (pStruct.IsHasTransNeedValuePar() && (pStruct._ParVec[i]._transNeedValue != -1)) {
			ttt_strStruct.append(strTable_4);
			sprintf(t_ch, "if (tt_transNeedValue & (1<<%d))!=0:\n", pStruct._ParVec[i]._transNeedValue);
			ttt_strStruct.append(t_ch);

			ttt_strStruct.append(strTable_5);
			if (IsPythonBaseType(pStruct._ParVec[i]._strParType, false)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "self.%s = %s.read(outStream)\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				else {
					sprintf(t_ch, "self.%s = outStream.%s()\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadPythonType(pStruct._ParVec[i]._strParType.c_str()).c_str());
				}
			}
			else {
				sprintf(t_ch, "o.%s = %s.read(outStream)\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
			}
			ttt_strStruct.append(t_ch);

			ttt_strStruct.append(strTable_4);
			ttt_strStruct.append("else:\n");
			ttt_strStruct.append(strTable_5);
			sprintf(t_ch, "self.%s=%s\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strDefaultValue.c_str());
			ttt_strStruct.append(t_ch);
		}
		else {
			ttt_strStruct.append(strTable_4);
			if (IsPythonBaseType(pStruct._ParVec[i]._strParType, false)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "self.%s = %s.read(outStream)\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				else {
					sprintf(t_ch, "self.%s = outStream.%s()\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadPythonType(pStruct._ParVec[i]._strParType.c_str()).c_str());
				}
			}
			else {
				sprintf(t_ch, "self.%s = %s.read(outStream)\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
			}
			ttt_strStruct.append(t_ch);
		}


	}

	ttt_strStruct.append(strTable_4);
	ttt_strStruct.append("tt_logsize2 = outStream.GetPos()\n");

	ttt_strStruct.append(strTable_4);
	ttt_strStruct.append("tt_offset = tt_len - (tt_logsize2 - tt_logsize1)\n");

	ttt_strStruct.append(strTable_4);
	ttt_strStruct.append("if tt_offset>0:\n");


	ttt_strStruct.append(strTable_5);
	ttt_strStruct.append("outStream.MoveForward(tt_offset)\n");

	if (pStruct._version > 1) {
		ttt_strStruct.append(strTable_3);
		ttt_strStruct.append("else:\n");

		for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
			if (pStruct._ParVec[i]._version <= 1)
				continue;
			tt_bHasNewPar = true;
			ttt_strStruct.append(strTable_4);
			sprintf(t_ch, "if tt_version>=%d:\n", pStruct._ParVec[i]._version);
			ttt_strStruct.append(t_ch);


			if (pStruct.IsHasTransNeedValuePar() && (pStruct._ParVec[i]._transNeedValue != -1)) {
				ttt_strStruct.append(strTable_5);
				sprintf(t_ch, "if (tt_transNeedValue & (1<<%d))!=0:\n", pStruct._ParVec[i]._transNeedValue);
				ttt_strStruct.append(t_ch);

				ttt_strStruct.append(strTable_6);
				if (IsPythonBaseType(pStruct._ParVec[i]._strParType, false)) {
					//是否是枚举
					if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
						sprintf(t_ch, "self.%s = %s.read(outStream)\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
					}
					else {
						sprintf(t_ch, "self.%s = outStream.%s()\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadPythonType(pStruct._ParVec[i]._strParType.c_str()).c_str());
					}
				}
				else {
					sprintf(t_ch, "self.%s = %s.read(outStream)\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				ttt_strStruct.append(t_ch);

				ttt_strStruct.append(strTable_5);
				ttt_strStruct.append("else:\n");
				ttt_strStruct.append(strTable_6);
				sprintf(t_ch, "self.%s=%s\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strDefaultValue.c_str());
				ttt_strStruct.append(t_ch);
			}
			else {
				ttt_strStruct.append(strTable_5);
				if (IsPythonBaseType(pStruct._ParVec[i]._strParType, false)) {
					//是否是枚举
					if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
						sprintf(t_ch, "self.%s = %s.read(outStream)\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
					}
					else {
						sprintf(t_ch, "self.%s = outStream.%s()\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadPythonType(pStruct._ParVec[i]._strParType.c_str()).c_str());
					}
				}
				else {
					sprintf(t_ch, "self.%s = %s.read(outStream)\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
				}
				ttt_strStruct.append(t_ch);
			}


			string tt_default = GetDefaultValue(pStruct._ParVec[i]._strParType, false, false, false, false, true);
			if (tt_default.length() > 0) {
				ttt_strStruct.append(strTable_4);
				ttt_strStruct.append("else:\n");

				ttt_strStruct.append(strTable_5);
				sprintf(t_ch, "self.%s=%s\n", pStruct._ParVec[i]._strParName.c_str(), tt_default.c_str());
				ttt_strStruct.append(t_ch);
			}
			else {
				ttt_strStruct.append(strTable_4);
				ttt_strStruct.append("else:\n");
				ttt_strStruct.append(strTable_5);
				sprintf(t_ch, "self.%s=None\n", pStruct._ParVec[i]._strParName.c_str());
				ttt_strStruct.append(t_ch);
			}

		}
	}



	if (ttt_strStruct.length() > 0)
		strStruct.append(ttt_strStruct.c_str());
	strStruct.append("\n");




	////////////////////
	////
	////////////////////

	//写写
	strStruct.append(strTable_2);
	sprintf(t_ch, "def write(self,inStream):\n");
	strStruct.append(t_ch);


	//先写入结构体的大小
	strStruct.append(strTable_3);
	strStruct.append("tt_len  = 0\n");
	strStruct.append(strTable_3);
	strStruct.append("tt_logsize1 = inStream.GetPos()\n");
	strStruct.append(strTable_3);
	strStruct.append("inStream.WriteShort(tt_len)\n");


	//再写入版本
	strStruct.append(strTable_3);
	sprintf(t_ch, "tt_version = %d\n", pStruct._version);
	strStruct.append(t_ch);
	strStruct.append(strTable_3);
	strStruct.append("inStream.WriteShort(tt_version)\n");

	strStruct.append(strTable_3);
	strStruct.append("tt_logsize2 = inStream.GetPos()\n");

	if (pStruct.IsHasTransNeedValuePar()) {
		strStruct.append(strTable_3);
		strStruct.append("tt_transNeedValue=0\n");
		strStruct.append(strTable_3);
		strStruct.append("inStream.WriteInt(tt_transNeedValue)\n");
	}


	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		if (pStruct._ParVec[i]._transNeedValue != -1) {
			strStruct.append(strTable_3);
			sprintf(t_ch, "if self.%s !=%s:\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strDefaultValue.c_str());
			strStruct.append(t_ch);

			strStruct.append(strTable_4);
			if (IsPythonBaseType(pStruct._ParVec[i]._strParType, true)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "%s.write(inStream,self.%s)\n", pStruct._ParVec[i]._strParType.c_str(), pStruct._ParVec[i]._strParName.c_str());
				}
				else
					sprintf(t_ch, "inStream.%s(self.%s)\n", TypeConvertToWritePythonType(pStruct._ParVec[i]._strParType.c_str()).c_str(), pStruct._ParVec[i]._strParName.c_str());
			}
			else {
				sprintf(t_ch, "self.%s.write(inStream)\n", pStruct._ParVec[i]._strParName.c_str());
			}
			strStruct.append(t_ch);


			strStruct.append(strTable_4);
			sprintf(t_ch, "tt_transNeedValue = tt_transNeedValue | (1<<%d)\n", pStruct._ParVec[i]._transNeedValue);
			strStruct.append(t_ch);

		}
		else {
			strStruct.append(strTable_3);
			if (IsPythonBaseType(pStruct._ParVec[i]._strParType, true)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "%s.write(inStream,self.%s)\n", pStruct._ParVec[i]._strParType.c_str(), pStruct._ParVec[i]._strParName.c_str());
				}
				else
					sprintf(t_ch, "inStream.%s(self.%s)\n", TypeConvertToWritePythonType(pStruct._ParVec[i]._strParType.c_str()).c_str(), pStruct._ParVec[i]._strParName.c_str());
			}
			else {
				sprintf(t_ch, "self.%s.write(inStream)\n", pStruct._ParVec[i]._strParName.c_str());
			}
			strStruct.append(t_ch);
		}


	}
	strStruct.append(strTable_3);
	strStruct.append("tt_logsize4 = inStream.GetPos()\n");
	strStruct.append(strTable_3);
	strStruct.append("tt_logsize3 = tt_logsize4 - tt_logsize2\n");
	strStruct.append(strTable_3);
	strStruct.append("inStream.SetPos(tt_logsize1)\n");
	strStruct.append(strTable_3);
	strStruct.append("inStream.WriteShort(tt_logsize3)\n");

	if (pStruct.IsHasTransNeedValuePar()) {
		strStruct.append(strTable_3);
		strStruct.append("if tt_transNeedValue!=0:\n");
		strStruct.append(strTable_4);
		strStruct.append("inStream.SetPos(tt_logsize2)\n");
		strStruct.append(strTable_4);
		strStruct.append("inStream.WriteInt(tt_transNeedValue)\n");
	}


	strStruct.append(strTable_3);
	strStruct.append("inStream.SetPos(tt_logsize4)\n");

	fwrite(strStruct.c_str(), strStruct.length(), 1, fp);

	fclose(fp);
}

void    ProtocolParse_Writer_Python::WriteVector_Python(stNamespace& pNamespace, stVector& pVector, string foldername, const string& strWriteNameSpace)
{
	//if (pVector._name.compare("bytevecgw") == 0)
	//{
	//	int a;
	//	a = 10;
	//}
	if (pVector._ParTName == "byte" || pVector._ParTName == "BYTE" || pVector._ParTName == "char" || pVector._ParTName == "unsigned char") {
		WriteVector2_Python(pNamespace, pVector, foldername, strWriteNameSpace);
		return;
	}
	char t_ch[512];
	sprintf(t_ch, "%s/%s.py", foldername.c_str(), pVector._name.c_str());

	//先创建文件
	FILE* fp = fopen(t_ch, "w");
	if (!fp)
		return;

	//写信息头,共用
	WriteInfoHead(fp, false, true);



	string strTable_1 = CreateTableString(0);
	string strTable_2 = CreateTableString(1);
	string strTable_3 = CreateTableString(2);
	string strTable_4 = CreateTableString(3);

	string strVector;

	strVector.append(strTable_1);
	strVector.append("import sys\n");
	strVector.append(strTable_1);
	strVector.append("sys.path.append('..')\n");
	strVector.append(strTable_1);
	strVector.append("sys.path.append('../..')\n");
	strVector.append(strTable_1);
	strVector.append("from InOutStream import InStream\n");
	strVector.append(strTable_1);
	strVector.append("from InOutStream import OutStream\n");
	strVector.append("\n");

	string tt_strMName;
	sprintf(t_ch, "%s", pVector._name.c_str());
	tt_strMName = t_ch;

	//if (pVector._name.compare("PtLobbyNotice6List") == 0)
	//{
	//	int a;
	//	a = 10;
	//}

	vector<string> tt_PacketNameList;
	GetUsePacketNamesByParType_Python(pVector._name, tt_PacketNameList);
	string tt_strSelf;
	sprintf(t_ch, "from %s.%s import %s", ProtocolUtils::PickEndFolderByPath(foldername).c_str(), pVector._name.c_str(), pVector._name.c_str());
	tt_strSelf = t_ch;
	RemovePacketNames(tt_PacketNameList, tt_strSelf, false);

	for (size_t i = 0; i < tt_PacketNameList.size(); i++) {
		strVector.append(strTable_1);
		strVector.append(tt_PacketNameList[i].c_str());
		strVector.append("\n");
	}

	if (tt_PacketNameList.size() > 0) {
		strVector.append("\n");
	}


	strVector.append(WriteComment_Python(strTable_1, pVector._comment, "").c_str());


	/////////////有两种版本，一种是非Flash基础类型，一种是
	bool  tt_bCSharpBaseType = IsPythonBaseType2(pVector._ParTName);

	strVector.append(strTable_1);
	sprintf(t_ch, "class %s():\n", tt_strMName.c_str());
	strVector.append(t_ch);
	strVector.append("\n");

	strVector.append(strTable_2);
	strVector.append("def __init__(self):\n");
	strVector.append(strTable_3);
	strVector.append("self.objList=[]\n");


	strVector.append("\n");
	strVector.append(strTable_2);
	strVector.append("@staticmethod\n");
	strVector.append(strTable_2);
	strVector.append("def read(outStream):\n");

	strVector.append(strTable_3);
	sprintf(t_ch, "o=%s()\n", tt_strMName.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_3);
	strVector.append("o.readLocal(outStream)\n");

	strVector.append(strTable_3);
	strVector.append("return o\n");

	////////////////
	//
	////////////////

	strVector.append("\n");
	strVector.append(strTable_2);
	strVector.append("def readLocal(self,outStream):\n");

	strVector.append(strTable_3);
	strVector.append("self.objList=[]\n");

	strVector.append(strTable_3);
	strVector.append("count = outStream.ReadInt()\n");

	strVector.append(strTable_3);
	strVector.append("for i in range(count):\n");

	if (!tt_bCSharpBaseType) {
		strVector.append(strTable_4);
		sprintf(t_ch, "self.objList.append(%s.read(outStream))\n", pVector._ParTName.c_str());
		strVector.append(t_ch);
	}
	else {
		strVector.append(strTable_4);
		sprintf(t_ch, "self.objList.append(outStream.%s())\n", TypeConvertToReadPythonType(pVector._ParTName).c_str());
		strVector.append(t_ch);
	}


	/////////////
	//
	////////////
	strVector.append("\n");
	strVector.append(strTable_2);
	strVector.append("def write(self,inStream):\n");


	strVector.append(strTable_3);
	strVector.append("inStream.WriteInt(len(self.objList))\n");

	strVector.append(strTable_3);
	strVector.append("for i in range(len(self.objList)):\n");

	//
	if (!tt_bCSharpBaseType) {
		strVector.append(strTable_4);
		sprintf(t_ch, "self.objList[i].write(inStream)\n");
		strVector.append(t_ch);
	}
	else {
		strVector.append(strTable_4);
		sprintf(t_ch, "inStream.%s(self.objList[i])\n", TypeConvertToWritePythonType(pVector._ParTName).c_str());
		strVector.append(t_ch);
	}

	fwrite(strVector.c_str(), strVector.length(), 1, fp);
	fclose(fp);
}
void    ProtocolParse_Writer_Python::WriteVector2_Python(stNamespace& pNamespace, stVector& pVector, string foldername, const string& strWriteNameSpace)
{
	char t_ch[512];
	sprintf(t_ch, "%s/%s.py", foldername.c_str(), pVector._name.c_str());

	//先创建文件
	FILE* fp = fopen(t_ch, "w");
	if (!fp)
		return;

	//写信息头,共用
	WriteInfoHead(fp, false, true);



	string strTable_1 = CreateTableString(0);
	string strTable_2 = CreateTableString(1);
	string strTable_3 = CreateTableString(2);
	string strTable_4 = CreateTableString(3);

	string strVector;

	strVector.append(strTable_1);
	strVector.append("import sys\n");
	strVector.append(strTable_1);
	strVector.append("sys.path.append('..')\n");
	strVector.append(strTable_1);
	strVector.append("sys.path.append('../..')\n");
	strVector.append(strTable_1);
	strVector.append("from InOutStream import InStream\n");
	strVector.append(strTable_1);
	strVector.append("from InOutStream import OutStream\n");
	strVector.append(strTable_1);
	strVector.append("from PtByteArray import PtByteArray\n");
	strVector.append("\n");

	string tt_strMName;
	sprintf(t_ch, "%s", pVector._name.c_str());
	tt_strMName = t_ch;

	strVector.append(strTable_1);
	sprintf(t_ch, "class %s(PtByteArray):\n", tt_strMName.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("pass");
	strVector.append("\n");

	fwrite(strVector.c_str(), strVector.length(), 1, fp);
	fclose(fp);
}
void    ProtocolParse_Writer_Python::WriteWJSVector_Python(stNamespace& pNamespace, stWJSVector& pWJSVector, string foldername, const string& strWriteNameSpace)
{
	stVector tt_Vector;
	tt_Vector._name = pWJSVector._name;
	tt_Vector._ParTName = pWJSVector._ParTName;
	tt_Vector._index = pWJSVector._index;
	tt_Vector._version = pWJSVector._version;
	tt_Vector._bBaseParT = pWJSVector._bBaseParT;
	WriteVector_Python(pNamespace, tt_Vector, foldername, strWriteNameSpace);
}
void    ProtocolParse_Writer_Python::WriteClass_Python(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace)
{
	switch (filetype) {
	case 0:
	{
		////对于客户端而言，request的为发送，callback的为回调
		if (pClass._strtype.compare("callback") == 0) {
			WriteClass_IReceiver_Python(pClass, filetype, pNamespace, foldername, false, strWriteNameSpace);
			WriteClass_Receiver_Python(pClass, filetype, pNamespace, foldername, strWriteNameSpace);
		}
		else {
			WriteClass_Send_Python(pClass, filetype, pNamespace, foldername, strWriteNameSpace);
		}
	}
	break;
	case 1:
	{
		////服务器而言，请求的是回调，callback是发送
		if (pClass._strtype.compare("request") == 0) //C2S
		{
			WriteClass_IReceiver_Python(pClass, filetype, pNamespace, foldername, false, strWriteNameSpace);
			WriteClass_Receiver_Python(pClass, filetype, pNamespace, foldername, strWriteNameSpace);
		}
		else //S2C
		{
			WriteClass_Send_Python(pClass, filetype, pNamespace, foldername, strWriteNameSpace);
		}
	}
	break;
	case 2:
	{

		//事实上两个里面是一样的
		if (pClass._strtype.compare("request") == 0) //C2S
		{
			WriteClass_IReceiver_Python(pClass, filetype, pNamespace, foldername, true, strWriteNameSpace);
			WriteClass_Receiver_Python(pClass, filetype, pNamespace, foldername, strWriteNameSpace);
			WriteClass_Send_Python(pClass, filetype, pNamespace, foldername, strWriteNameSpace);
		}

	}
	break;
	default:
		break;
	}
}
void    ProtocolParse_Writer_Python::WriteClass_IReceiver_Python(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, bool bemptyimplement, const string& strWriteNameSpace)
{
	char t_ch[1024];
	sprintf(t_ch, "%s/IReceiver_%s.py", foldername.c_str(), pNamespace._protocolname.c_str());

	//先创建文件
	FILE* fp = fopen(t_ch, "w");
	if (!fp)
		return;

	//写信息头,共用
	WriteInfoHead(fp, false, true);

	string strclass;

	string strTable_1 = CreateTableString(0);
	string strTable_2 = CreateTableString(1);
	string strTable_3 = CreateTableString(2);

	string tt_strMName;
	sprintf(t_ch, "IReceiver_%s", pClass._name.c_str());
	tt_strMName = t_ch;

	strclass.append(strTable_1);
	sprintf(t_ch, "class IReceiver_%s():\n", pNamespace._protocolname.c_str());
	strclass.append(t_ch);

	strclass.append("\n");
	fwrite(strclass.c_str(), strclass.length(), 1, fp);



	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		if (filetype == 2 && ((m_bSCSpliteS && pClass._FuncVec[i]._SCSpliteType == -1) || (m_bSCSpliteC && pClass._FuncVec[i]._SCSpliteType == 1)))
			continue;
		//写函数
		WriteFunc_IReceiver_Python(pClass._FuncVec[i], filetype, 1, pClass, pNamespace, foldername, fp, bemptyimplement);
	}

	//strclass = "";
	//strclass.append("\n");
	//strclass.append(strTable_1);
	//sprintf(t_ch, "return %s\n", tt_strMName.c_str());
	//strclass.append(t_ch);
	//fwrite(strclass.c_str(), strclass.length(), 1, fp);
	//fclose(fp);
}
void    ProtocolParse_Writer_Python::WriteFunc_IReceiver_Python(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp, bool bemptyimplement)
{
	char t_ch[1024];

	string strTable_0 = CreateTableString(tablenum);
	string strTable_1 = CreateTableString(tablenum + 1);
	string strTable_2 = CreateTableString(tablenum + 2);


	string strFunc;

	string tt_strMName;
	sprintf(t_ch, "IReceiver_%s", pClass._name.c_str());
	tt_strMName = t_ch;


	strFunc.append(strTable_0);
	sprintf(t_ch, "def IReceiver_%s(self, ", pFunc._name.c_str());
	strFunc.append(t_ch);


	if (pFunc._InParVec.size() > 0) {
		for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
			if (IsExType_vector(pFunc._InParVec[i]._strParType)) {
				sprintf(t_ch, "%s, ", pFunc._InParVec[i]._strParName.c_str());
				strFunc.append(t_ch);
			}
			else {
				sprintf(t_ch, "%s, ", pFunc._InParVec[i]._strParName.c_str());
				strFunc.append(t_ch);
			}

		}
	}

	if (filetype == 2) {
		strFunc.append("sessionId, pExData):\n");
	}
	else
		strFunc.append("sessionId, pExData):\n");

	strFunc.append(strTable_1);
	strFunc.append("\"\"\"\n");
	strFunc.append(strTable_1);
	strFunc.append("NOTE:\n");

	strFunc.append(WriteComment_Python(strTable_2, pFunc._comment, "", true).c_str());
	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		strFunc.append(WriteComment_Python(strTable_2, pFunc._InParVec[i]._comment, pFunc._InParVec[i]._strParName, true).c_str());
	}

	strFunc.append("\n");
	strFunc.append(strTable_1);
	strFunc.append("Args:\n");
	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		strFunc.append(strTable_2);
		strFunc.append(pFunc._InParVec[i]._strParName.c_str());
		strFunc.append(": ");
		strFunc.append(pFunc._InParVec[i]._strParType.c_str());
		strFunc.append("\n");
	}
	strFunc.append(strTable_1);
	strFunc.append("\"\"\"\n");
	strFunc.append(strTable_1);
	strFunc.append("pass\n");
	strFunc.append("\n");
	fwrite(strFunc.c_str(), strFunc.length(), 1, fp);
}
void    ProtocolParse_Writer_Python::WriteClass_Receiver_Python(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace)
{
	char t_ch[1024];
	sprintf(t_ch, "%s/%s_Callback.py", foldername.c_str(), pNamespace._protocolname.c_str());

	//先创建文件
	FILE* fp = fopen(t_ch, "w");
	if (!fp)
		return;

	//写信息头,共用
	WriteInfoHead(fp, false, true);

	string strclass;

	string strTable_0 = CreateTableString(0);
	string strTable_1 = CreateTableString(1);
	string strTable_2 = CreateTableString(2);
	string strTable_3 = CreateTableString(3);



	string tt_strMName;
	sprintf(t_ch, "%s", pClass._name.c_str());
	tt_strMName = t_ch;

	strclass.append(strTable_0);
	strclass.append("import sys\n");
	strclass.append(strTable_0);
	strclass.append("sys.path.append('..')\n");
	strclass.append(strTable_0);
	strclass.append("sys.path.append('../..')\n");
	strclass.append(strTable_0);
	strclass.append("from InOutStream import InStream\n");
	strclass.append(strTable_0);
	strclass.append("from InOutStream import OutStream\n");
	strclass.append("\n\n");

	strclass.append(strTable_0);
	sprintf(t_ch, "from %s.ID_%s import ID_%s\n", ProtocolUtils::PickEndFolderByPath(foldername).c_str(), pNamespace._protocolname.c_str(), pNamespace._protocolname.c_str());
	strclass.append(t_ch);


	vector<string> tt_PacketNameList;
	//sprintf(t_ch, "import %s", pNamespace._protocolname.c_str());
	//tt_PacketNameList.push_back(t_ch);

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
			if (!IsPythonBaseType(pClass._FuncVec[i]._InParVec[j]._strParType, false)) {
				GetUsePacketNamesByParType_Python(pClass._FuncVec[i]._InParVec[j]._strParType, tt_PacketNameList);
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
	sprintf(t_ch, "class %s_Callback():\n", pNamespace._protocolname.c_str());
	strclass.append(t_ch);
	strclass.append("\n");

	fwrite(strclass.c_str(), strclass.length(), 1, fp);
	strclass = "";

	WriteFunc_ReceiverParser_Python(pClass, filetype, 1, pNamespace, foldername, fp);


	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		if (filetype == 2 && ((m_bSCSpliteS && pClass._FuncVec[i]._SCSpliteType == -1) || (m_bSCSpliteC && pClass._FuncVec[i]._SCSpliteType == 1)))
			continue;
		WriteFunc_Receiver_Python(pClass._FuncVec[i], filetype, 1, pClass, pNamespace, foldername, fp);
	}


	//strclass = "";
	//strclass.append("\n");
	//strclass.append(strTable_0);
	//sprintf(t_ch, "return %s", tt_strMName.c_str());
	//strclass.append(t_ch);

	//fwrite(strclass.c_str(), strclass.length(), 1, fp);
	fclose(fp);
}
void    ProtocolParse_Writer_Python::WriteFunc_ReceiverParser_Python(stClass& pClass, int filetype, int tablenum, stNamespace& pNamespace, string foldername, FILE* fp)
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

	////////////////新版本
	strFunc.append(strTable_0);
	strFunc.append("def __init__(self):\n");
	strFunc.append(strTable_1);
	strFunc.append("self.callbackMap = {\n");

	int tt_realCount_1 = 0;
	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		if (filetype == 2 && ((m_bSCSpliteS && pClass._FuncVec[i]._SCSpliteType == -1) || (m_bSCSpliteC && pClass._FuncVec[i]._SCSpliteType == 1)))
			continue;
		if (tt_realCount_1 != 0)
			strFunc.append(",\n");
		strFunc.append(strTable_1);
		sprintf(t_ch, "ID_%s.ID_%s_%s: self._parser_%s", pNamespace._protocolname.c_str(), pClass._name.c_str(), pClass._FuncVec[i]._name.c_str(),
			/*pNamespace._protocolname.c_str(),*/ pClass._FuncVec[i]._name.c_str());
		strFunc.append(t_ch);
		tt_realCount_1++;
	}
	strFunc.append("\n");
	strFunc.append(strTable_0);
	strFunc.append("}\n\n");

	//def __init__(self) :
	//	self.callbackMap = {
	//	ID_JoinUp.ID_JoinUpRequest_RegisterOrUpdataServer: JoinUp_Callback._parser_RegisterOrUpdataServer,
	//	ID_JoinUp.ID_JoinUpRequest_OnRegisterServer : JoinUp_Callback._parser_OnRegisterServer
	//}




	////////////////新片本

	strFunc.append(strTable_0);

	//if (filetype == 0)
	//{
	//	strFunc.append("def Parser(self, cls, outStream, pExData):\n");
	//}
	//else
	//{
	strFunc.append("def Parser(self, cls, outStream, sessionId, pExData):\n");
	//}


	strFunc.append(strTable_1);
	strFunc.append("no = outStream.ReadInt()\n");

	strFunc.append(strTable_1);
	sprintf(t_ch, "if no != ID_%s.%s:\n", pNamespace._protocolname.c_str(), m_ProtocolNo._name.c_str());
	strFunc.append(t_ch);


	strFunc.append(strTable_2);
	strFunc.append("outStream.MoveBack(4)\n");

	strFunc.append(strTable_2);
	strFunc.append("return False\n");


	strFunc.append(strTable_1);
	strFunc.append("proversionno = outStream.ReadShort()\n");


	strFunc.append(strTable_1);
	strFunc.append("methodid = outStream.ReadInt()\n");

	strFunc.append(strTable_1);
	strFunc.append("self.callbackfunc = self.callbackMap[methodid]\n");
	strFunc.append(strTable_1);
	strFunc.append("if self.callbackfunc != None:\n");
	strFunc.append(strTable_2);
	strFunc.append("self.callbackfunc(cls, outStream, sessionId, pExData, proversionno)\n");
	strFunc.append(strTable_2);
	strFunc.append("return True\n");
	strFunc.append(strTable_1);
	strFunc.append("return False\n\n");



	//self.callbackfunc = self.callbackMap[methodid]
	//if self.callbackfunc != None:
	//  self.callbackfunc(cls, outStream, sessionId, pExData, proversionno)
	//	return True
	//return False

	//strFunc.append("\n");
	//int tt_realCount = 0;
	//for (int i = 0; i < pClass._FuncVec.size(); i++)
	//{
	//	if (pClass._FuncVec[i]._bDiscard)
	//		continue;
	//	if (filetype == 2 && ((m_bSCSpliteS && pClass._FuncVec[i]._SCSpliteType == -1) || (m_bSCSpliteC && pClass._FuncVec[i]._SCSpliteType == 1)))
	//		continue;

	//	strFunc.append(strTable_1);
	//	if (tt_realCount == 0)
	//	{
	//		sprintf(t_ch, "if methodid == ID_%s.ID_%s_%s:\n", pNamespace._protocolname.c_str(), pClass._name.c_str(), pClass._FuncVec[i]._name.c_str());
	//		strFunc.append(t_ch);

	//	}
	//	else
	//	{
	//		sprintf(t_ch, "elif methodid == ID_%s.ID_%s_%s:\n", pNamespace._protocolname.c_str(), pClass._name.c_str(), pClass._FuncVec[i]._name.c_str());
	//		strFunc.append(t_ch);
	//	}
	//	tt_realCount++;

	//	strFunc.append(strTable_2);
	//	//if (filetype == 0)
	//	//{
	//	//	sprintf(t_ch, "%s._parser_%s(self, cls, outStream, pExData, proversionno)\n", tt_strMName.c_str(), pClass._FuncVec[i]._name.c_str());
	//	//	strFunc.append(t_ch);
	//	//}
	//	//else
	//	//{
	//		sprintf(t_ch, "%s_Callback._parser_%s(self, cls, outStream, sessionId, pExData, proversionno)\n", pNamespace._protocolname.c_str(), pClass._FuncVec[i]._name.c_str());
	//		strFunc.append(t_ch);
	//	//}

	//	strFunc.append(strTable_2);
	//	strFunc.append("return True\n");
	//}


	//strFunc.append(strTable_1);
	//strFunc.append("else:\n");

	//strFunc.append(strTable_2);
	//strFunc.append("return False\n");

	//strFunc.append("\n");


	fwrite(strFunc.c_str(), strFunc.length(), 1, fp);
}
void    ProtocolParse_Writer_Python::WriteFunc_Receiver_Python(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp)
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


	strFunc.append(strTable_0);
	sprintf(t_ch, "def _parser_%s(self, cls, outStream, sessionId, pExData, proversionno):\n", pFunc._name.c_str()/*, pNamespace._protocolname.c_str()*/);
	strFunc.append(t_ch);


	for (unsigned int i = 0; i < pFunc._InParVec.size(); i++) {
		if (pFunc._InParVec[i]._version > 1)
			break;
		strFunc.append(strTable_1);
		if (IsPythonBaseType(pFunc._InParVec[i]._strParType, false)) {
			sprintf(t_ch, "%s = outStream.%s()\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToReadPythonType(pFunc._InParVec[i]._strParType)).c_str());

		}
		else if (IsExType_enum(pFunc._InParVec[i]._strParType)) {
			sprintf(t_ch, "%s = %s.read(outStream)\n", pFunc._InParVec[i]._strParName.c_str(), pFunc._InParVec[i]._strParType.c_str());
		}
		else {
			sprintf(t_ch, "%s = %s.read(outStream)\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToPythonType(pFunc._InParVec[i]._strParType)).c_str());
		}
		strFunc.append(t_ch);
	}


	string ttt_strFunc;
	bool tt_bHasNewPar = false;

	//作出判定相等
	ttt_strFunc.append(strTable_1);
	sprintf(t_ch, "if proversionno == %d or proversionno == 0:\n", pFunc._version);
	ttt_strFunc.append(t_ch);


	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		if (pFunc._InParVec[i]._version <= 1)
			continue;
		tt_bHasNewPar = true;
		ttt_strFunc.append(strTable_2);
		if (IsPythonBaseType(pFunc._InParVec[i]._strParType, false)) {
			sprintf(t_ch, "%s = outStream.%s()\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToReadPythonType(pFunc._InParVec[i]._strParType)).c_str());

		}
		else if (IsExType_enum(pFunc._InParVec[i]._strParType)) {
			sprintf(t_ch, "%s = %s.read(outStream)\n", pFunc._InParVec[i]._strParName.c_str(), pFunc._InParVec[i]._strParType.c_str());
		}
		else {
			sprintf(t_ch, "%s = %s.read(outStream)\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToPythonType(pFunc._InParVec[i]._strParType)).c_str());
		}
		ttt_strFunc.append(t_ch);
	}

	//strFunc.append(strTable_2);
	//sprintf(t_ch, "cls.IReceiver_%s(", pFunc._name.c_str());
	//strFunc.append(t_ch);


	//for (int i = 0; i < pFunc._InParVec.size(); i++)
	//{
	//	sprintf(t_ch, "%s, ", pFunc._InParVec[i]._strParName.c_str());
	//	strFunc.append(t_ch);
	//}

	//strFunc.append("sessionId, pExData)\n");


	//////////////低解高
	ttt_strFunc.append(strTable_1);
	sprintf(t_ch, "elif proversionno > %d:\n", pFunc._version);
	ttt_strFunc.append(t_ch);



	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		if (pFunc._InParVec[i]._version <= 1)
			continue;
		tt_bHasNewPar = true;
		ttt_strFunc.append(strTable_2);
		if (IsPythonBaseType(pFunc._InParVec[i]._strParType, false)) {
			sprintf(t_ch, "%s = outStream.%s()\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToReadPythonType(pFunc._InParVec[i]._strParType)).c_str());
		}
		else if (IsExType_enum(pFunc._InParVec[i]._strParType)) {
			sprintf(t_ch, "%s = %s.read(outStream)\n", pFunc._InParVec[i]._strParName.c_str(), pFunc._InParVec[i]._strParType.c_str());
		}
		else {
			sprintf(t_ch, "%s = %s.read(outStream)\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToPythonType(pFunc._InParVec[i]._strParType)).c_str());
		}
		ttt_strFunc.append(t_ch);
	}

	//strFunc.append(strTable_2);
	//sprintf(t_ch, "cls.IReceiver_%s(", pFunc._name.c_str());
	//strFunc.append(t_ch);


	//for (int i = 0; i < pFunc._InParVec.size(); i++)
	//{
	//	sprintf(t_ch, "%s, ", pFunc._InParVec[i]._strParName.c_str());
	//	strFunc.append(t_ch);
	//}
	//strFunc.append("sessionId, pExData)\n");



	///////////////高解低
	ttt_strFunc.append(strTable_1);
	ttt_strFunc.append("else:\n");

	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		if (pFunc._InParVec[i]._version <= 1)
			continue;
		tt_bHasNewPar = true;
		ttt_strFunc.append(strTable_2);

		if (IsPythonBaseType(pFunc._InParVec[i]._strParType, false) || IsExType_enum(pFunc._InParVec[i]._strParType)) {
			sprintf(t_ch, "%s = None\n", pFunc._InParVec[i]._strParName.c_str());
		}
		else if (IsExType_vector(pFunc._InParVec[i]._strParType)) {
			sprintf(t_ch, "%s = None\n", pFunc._InParVec[i]._strParName.c_str());

		}
		else {
			sprintf(t_ch, "%s = None\n", pFunc._InParVec[i]._strParName.c_str());
		}
		ttt_strFunc.append(t_ch);
	}

	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		if (pFunc._InParVec[i]._version <= 1)
			continue;
		tt_bHasNewPar = true;
		ttt_strFunc.append(strTable_2);
		sprintf(t_ch, "if proversionno >= %d:\n", pFunc._InParVec[i]._version);
		ttt_strFunc.append(t_ch);

		ttt_strFunc.append(strTable_3);
		if (IsPythonBaseType(pFunc._InParVec[i]._strParType, false)) {
			sprintf(t_ch, "%s = outStream.%s()\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToReadPythonType(pFunc._InParVec[i]._strParType)).c_str());

		}
		else if (IsExType_enum(pFunc._InParVec[i]._strParType)) {
			sprintf(t_ch, "%s = %s.read(outStream)\n", pFunc._InParVec[i]._strParName.c_str(), pFunc._InParVec[i]._strParType.c_str());
		}
		else {
			sprintf(t_ch, "%s = %s.read(outStream)\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToPythonType(pFunc._InParVec[i]._strParType)).c_str());
		}
		ttt_strFunc.append(t_ch);

		string tt_default = GetDefaultValue(pFunc._InParVec[i]._strParType, false, false, false, false, true);
		if (tt_default.length() > 0) {
			ttt_strFunc.append(strTable_2);
			ttt_strFunc.append("else:\n");

			ttt_strFunc.append(strTable_3);
			sprintf(t_ch, "%s = %s\n", pFunc._InParVec[i]._strParName.c_str(), tt_default.c_str());
			ttt_strFunc.append(t_ch);
		}
		else if (IsExType_vector(pFunc._InParVec[i]._strParType)) {
			ttt_strFunc.append(strTable_2);
			ttt_strFunc.append("else:\n");

			ttt_strFunc.append(strTable_3);
			sprintf(t_ch, "%s = %s()\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToPythonType(pFunc._InParVec[i]._strParType)).c_str());
			ttt_strFunc.append(t_ch);
		}
		else if (IsExType_WJSVector(pFunc._InParVec[i]._strParType)) {
			ttt_strFunc.append(strTable_2);
			ttt_strFunc.append("else:\n");

			ttt_strFunc.append(strTable_3);
			sprintf(t_ch, "%s = %s()\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToPythonType(pFunc._InParVec[i]._strParType)).c_str());
			ttt_strFunc.append(t_ch);
		}
		else if (IsExType_struct(pFunc._InParVec[i]._strParType)) {
			ttt_strFunc.append(strTable_2);
			ttt_strFunc.append("else:\n");

			ttt_strFunc.append(strTable_3);
			sprintf(t_ch, "%s = %s()\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToPythonType(pFunc._InParVec[i]._strParType)).c_str());
			ttt_strFunc.append(t_ch);
		}
		else if (IsExType_enum(pFunc._InParVec[i]._strParType)) {
			ttt_strFunc.append(strTable_2);
			ttt_strFunc.append("else:\n");

			ttt_strFunc.append(strTable_3);
			sprintf(t_ch, "%s = 0\n", pFunc._InParVec[i]._strParName.c_str());
			ttt_strFunc.append(t_ch);
		}
	}

	if (tt_bHasNewPar)
		strFunc.append(ttt_strFunc.c_str());


	strFunc.append(strTable_1);
	sprintf(t_ch, "cls.IReceiver_%s(", pFunc._name.c_str());
	strFunc.append(t_ch);

	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		sprintf(t_ch, "%s, ", pFunc._InParVec[i]._strParName.c_str());
		strFunc.append(t_ch);
	}

	strFunc.append("sessionId, pExData)\n");

	/////////////////
	strFunc.append("\n");
	fwrite(strFunc.c_str(), strFunc.length(), 1, fp);
}
void    ProtocolParse_Writer_Python::WriteClass_Send_Python(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, const string& strWriteNameSpace)
{
	char t_ch[512];
	sprintf(t_ch, "%s/%s_Send.py", foldername.c_str(), pNamespace._protocolname.c_str());

	//先创建文件
	FILE* fp = fopen(t_ch, "w");
	if (!fp)
		return;

	//写信息头,共用
	WriteInfoHead(fp, false, true);

	string strclass;

	string strTable_1 = CreateTableString(0);
	string strTable_2 = CreateTableString(1);
	string strTable_3 = CreateTableString(2);

	string tt_strMName;
	sprintf(t_ch, "%s", pClass._name.c_str());
	tt_strMName = t_ch;


	strclass.append(strTable_1);
	strclass.append("import sys\n");
	strclass.append(strTable_1);
	strclass.append("sys.path.append('..')\n");
	strclass.append(strTable_1);
	strclass.append("sys.path.append('../..')\n");
	strclass.append(strTable_1);
	strclass.append("from InOutStream import InStream\n");
	strclass.append(strTable_1);
	strclass.append("from InOutStream import OutStream\n");
	strclass.append("\n");

	strclass.append("\n");

	strclass.append(strTable_1);
	sprintf(t_ch, "from %s.ID_%s import ID_%s\n", ProtocolUtils::PickEndFolderByPath(foldername).c_str(), pNamespace._protocolname.c_str(), pNamespace._protocolname.c_str());
	strclass.append(t_ch);

	vector<string> tt_PacketNameList;
	//sprintf(t_ch, "import %s", pNamespace._protocolname.c_str());
	//tt_PacketNameList.push_back(t_ch);


	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		if (filetype == 2 && ((m_bSCSpliteS && pClass._FuncVec[i]._SCSpliteType == 1) || (m_bSCSpliteC && pClass._FuncVec[i]._SCSpliteType == -1)))
			continue;

		for (size_t j = 0; j < pClass._FuncVec[i]._InParVec.size(); j++) {
			if (!IsPythonBaseType(pClass._FuncVec[i]._InParVec[j]._strParType, false)) {
				GetUsePacketNamesByParType_Python(pClass._FuncVec[i]._InParVec[j]._strParType, tt_PacketNameList);
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
	//if(filetype==2)
	sprintf(t_ch, "class %s_Send():\n", pNamespace._protocolname.c_str());
	//else
	//  sprintf(t_ch, "class %s():\n", tt_strMName.c_str());

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
		WriteFunc_SendByteSeq_Python(pClass._FuncVec[i], 1, pClass, pNamespace, foldername, fp);
	}


	//strclass = "";
	//strclass.append(strTable_1);
	//sprintf(t_ch, "return %s\n", tt_strMName.c_str());
	//strclass.append(t_ch);
	//fwrite(strclass.c_str(), strclass.length(), 1, fp);

	fclose(fp);

}
void    ProtocolParse_Writer_Python::WriteFunc_SendByteSeq_Python(stFunc& pFunc, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp)
{
	char t_ch[1024];


	string strTable_0 = CreateTableString(tablenum);
	string strTable_1 = CreateTableString(tablenum + 1);
	string strTable_2 = CreateTableString(tablenum + 2);

	string strfunc;

	string tt_strMName;
	sprintf(t_ch, "%s", pClass._name.c_str());
	tt_strMName = t_ch;

	strfunc.append(strTable_0);
	strfunc.append("@staticmethod\n");
	strfunc.append(strTable_0);
	//sprintf(t_ch, "def  Send_%s(self", pFunc._name.c_str());
	sprintf(t_ch, "def  Send_%s(", pFunc._name.c_str());
	strfunc.append(t_ch);

	//有参
	if (pFunc._InParVec.size() > 0) {
		//strfunc.append(", ");
		for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
			if (i == pFunc._InParVec.size() - 1) {
				sprintf(t_ch, "%s", pFunc._InParVec[i]._strParName.c_str());
				strfunc.append(t_ch);
			}
			else {
				sprintf(t_ch, "%s, ", pFunc._InParVec[i]._strParName.c_str());
				strfunc.append(t_ch);
			}

		}

	}

	strfunc.append("):\n");


	strfunc.append(strTable_1);
	strfunc.append("\"\"\"\n");
	strfunc.append(WriteComment_Python(strTable_1, pFunc._comment, "", true).c_str());
	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		strfunc.append(WriteComment_Python(strTable_1, pFunc._InParVec[i]._comment, pFunc._InParVec[i]._strParName, true).c_str());
	}
	strfunc.append(strTable_1);
	strfunc.append("\"\"\"\n");

	strfunc.append(strTable_1);
	strfunc.append("inStream = InStream()\n");

	//先写协议号
	strfunc.append(strTable_1);
	sprintf(t_ch, "inStream.WriteInt(ID_%s.%s)\n", pNamespace._protocolname.c_str(), m_ProtocolNo._name.c_str());
	strfunc.append(t_ch);

	//写版本号
	//strfunc.append(strTable_1);
	//sprintf(t_ch, "tt_version = %d\n", pFunc._version);
	//strfunc.append(t_ch);
	strfunc.append(strTable_1);
	sprintf(t_ch, "inStream.WriteShort(%d)\n", pFunc._version);
	strfunc.append(t_ch);
	//strfunc.append("inStream.WriteShort(tt_version)\n");

	//再写函数号
	strfunc.append(strTable_1);
	sprintf(t_ch, "inStream.WriteInt(ID_%s.ID_%s_%s)\n", pNamespace._protocolname.c_str(), pClass._name.c_str(), pFunc._name.c_str());
	strfunc.append(t_ch);

	//
	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		strfunc.append(strTable_1);
		if (IsPythonBaseType(pFunc._InParVec[i]._strParType, true)) {
			if (IsExType_enum(pFunc._InParVec[i]._strParType)) {
				sprintf(t_ch, "%s.write(inStream,%s)\n", pFunc._InParVec[i]._strParType.c_str(), pFunc._InParVec[i]._strParName.c_str());
			}
			else
				sprintf(t_ch, "inStream.%s(%s)\n", TypeConvertToWritePythonType(pFunc._InParVec[i]._strParType.c_str()).c_str(), pFunc._InParVec[i]._strParName.c_str());

		}
		else {
			sprintf(t_ch, "%s.write(inStream)\n", pFunc._InParVec[i]._strParName.c_str());

		}
		strfunc.append(t_ch);
	}

	strfunc.append(strTable_1);
	strfunc.append("return inStream\n");
	strfunc.append("\n");

	fwrite(strfunc.c_str(), strfunc.length(), 1, fp);
}
//////////////
//
/////////////



void    ProtocolParse_Writer_Python::GetUsePacketNamesByParType_Python(const string& strParType, vector<string>& packetNamesList)
{
	for (int i = 0; i < (int)m_NamespaceVec.size(); i++) {
		for (int j = 0; j < (int)m_NamespaceVec[i]._StructVec.size(); j++) {
			if (m_NamespaceVec[i]._StructVec[j]._name.compare(strParType.c_str()) == 0) {
				char tt_ch[256];
				sprintf(tt_ch, "from %s.%s import %s", m_NamespaceVec[i]._protocolname.c_str(), m_NamespaceVec[i]._StructVec[j]._name.c_str(), m_NamespaceVec[i]._StructVec[j]._name.c_str());
				AddPacketNames(packetNamesList, tt_ch);

				for (int k = 0; k < (int)m_NamespaceVec[i]._StructVec[j]._ParVec.size(); k++) {
					if (!IsPythonBaseType(m_NamespaceVec[i]._StructVec[j]._ParVec[k]._strParType, false)) {
						GetUsePacketNamesByParType_Python(m_NamespaceVec[i]._StructVec[j]._ParVec[k]._strParType, packetNamesList);
					}
				}

				return;
			}
		}

		for (int j = 0; j < (int)m_NamespaceVec[i]._VectorVec.size(); j++) {
			if (m_NamespaceVec[i]._VectorVec[j]._name.compare(strParType.c_str()) == 0) {
				char tt_ch[256];
				sprintf(tt_ch, "from %s.%s import %s", m_NamespaceVec[i]._protocolname.c_str(), m_NamespaceVec[i]._VectorVec[j]._name.c_str(), m_NamespaceVec[i]._VectorVec[j]._name.c_str());
				AddPacketNames(packetNamesList, tt_ch);

				if (!IsPythonBaseType(m_NamespaceVec[i]._VectorVec[j]._ParTName, false)) {
					GetUsePacketNamesByParType_Python(m_NamespaceVec[i]._VectorVec[j]._ParTName, packetNamesList);
				}

				return;
			}
		}

		for (int j = 0; j < (int)m_NamespaceVec[i]._WJSVectorVec.size(); j++) {
			if (m_NamespaceVec[i]._WJSVectorVec[j]._name.compare(strParType.c_str()) == 0) {
				char tt_ch[256];
				sprintf(tt_ch, "from %s.%s import %s", m_NamespaceVec[i]._protocolname.c_str(), m_NamespaceVec[i]._WJSVectorVec[j]._name.c_str(), m_NamespaceVec[i]._WJSVectorVec[j]._name.c_str());
				AddPacketNames(packetNamesList, tt_ch);

				if (!IsPythonBaseType(m_NamespaceVec[i]._WJSVectorVec[j]._ParTName, false)) {
					GetUsePacketNamesByParType_Python(m_NamespaceVec[i]._WJSVectorVec[j]._ParTName, packetNamesList);
				}

				return;
			}
		}

		for (int j = 0; j < (int)m_NamespaceVec[i]._EnumVec.size(); j++) {
			if (m_NamespaceVec[i]._EnumVec[j]._name.compare(strParType.c_str()) == 0) {
				char tt_ch[256];
				sprintf(tt_ch, "from %s.%s import %s", m_NamespaceVec[i]._protocolname.c_str(), m_NamespaceVec[i]._EnumVec[j]._name.c_str(), m_NamespaceVec[i]._EnumVec[j]._name.c_str());
				AddPacketNames(packetNamesList, tt_ch);
				return;
			}
		}
	}

	for (size_t i = 0; i < m_pRefProtocolParseList.size(); i++) {
		if (m_pRefProtocolParseList[i])
			((ProtocolParse_Writer_Python*)m_pRefProtocolParseList[i])->GetUsePacketNamesByParType_Python(strParType, packetNamesList);
	}

}


string  ProtocolParse_Writer_Python::WriteComment_Python(const string& strTable, const string& strComment, const string& strPar, bool bInBlockComment)
{
	if (strComment.length() == 0)
		return "";
	char tt_ch[2048];

	string strLine;

	vector<string> tt_subList;
	ProtocolUtils::GetAllSubString(strComment, "split", tt_subList, false);


	for (unsigned int i = 0; i < tt_subList.size(); i++) {
		string tt_strReal;
		//UTF8Decode(tt_subList[i].c_str(),tt_strReal);
		tt_strReal = tt_subList[i];
		strLine.append(strTable);
		if (strPar.length() > 0 && i == 0) {
			if (bInBlockComment)
				sprintf(tt_ch, "%s  %s\n", strPar.c_str(), tt_strReal.c_str());
			else
				sprintf(tt_ch, "#%s  %s\n", strPar.c_str(), tt_strReal.c_str());

		}
		else {
			if (bInBlockComment)
				sprintf(tt_ch, "%s\n", tt_strReal.c_str());
			else
				sprintf(tt_ch, "#%s\n", tt_strReal.c_str());

		}

		strLine.append(tt_ch);

	}

	return strLine;
}
#pragma region 类型转换与判断
bool    ProtocolParse_Writer_Python::IsPythonBaseType(const string& strTypeName, bool bWrite)
{
	if (IsBaseType_WriterType(strTypeName, eProtocolWriterType::Python))
		return true;


	if (bWrite) {
		//枚举类型作为基本类型
		if (IsExType_enum(strTypeName))
			return true;
	}


	return false;
}

bool    ProtocolParse_Writer_Python::IsPythonBaseType2(string strTypeName)
{
	if (IsBaseType(strTypeName))
		return true;

	return FALSE;

}
string  ProtocolParse_Writer_Python::TypeConvertToPythonType(const string& strTypeName)
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

string  ProtocolParse_Writer_Python::TypeConvertToReadPythonType(const string& strTypeName)
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
				return "ReadChar";


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
				return "ReadChar";
			case 9://BOOL
				return "ReadInt";
			case 8://bool
				return "ReadBool";
			case 10://DWORD
				return "ReadInt";
			case 14://string
				return "ReadString";
			case 24://wjs_utf8
				return "ReadUtf8";
			case 15://__int64
			case 22://long long
				return "ReadLongLong";
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
string  ProtocolParse_Writer_Python::TypeConvertToWritePythonType(const string& strTypeName)
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
				return "WriteChar";


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
				return "WriteUChar";
			case 9://BOOL
				return "WriteInt";
			case 8://bool
				return "WriteBool";
			case 10://DWORD
				return "WriteInt";
			case 14://string
				return "WriteString";
			case 24://wjs_utf8
				return "WriteUtf8";
			case 15://__int64
			case 22://long long
				return "WriteLongLong";
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