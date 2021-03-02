#include "ProtocolParse_Writer_Flash.h"
#include "ProtocolParse.h"
#include <time.h>
#include <sys/timeb.h>

extern string g_strBaseType[];


/////////////
//                 Flash
////////////////
ProtocolParse_Writer_Flash::ProtocolParse_Writer_Flash(const string& strPathName)
	:ProtocolParse_Writer(strPathName, eProtocolWriterType::Flash)
{
	m_strFlashPackage = "com.lobby.proto";
	m_strFlashToolPackage = "com.lobby.tool";
}
ProtocolParse_Writer_Flash::~ProtocolParse_Writer_Flash()
{

}
bool    ProtocolParse_Writer_Flash::CreateFile_Path(string path, int filetype, bool brefprotocol)
{
	return CreateFile_Path_Flash(path,filetype,brefprotocol);
}
ProtocolParse_Writer* ProtocolParse_Writer_Flash::Create_RefProtocolParse_Writer(const string& strPathName)
{
	return new ProtocolParse_Writer_Flash(strPathName);
}
bool    ProtocolParse_Writer_Flash::CreateFile_Path_Flash(string path, int filetype, bool brefprotocol)
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
		return CreateFile_Flash(str1, filetype, brefprotocol);
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
		return CreateFile_Flash(str1, filetype, brefprotocol);
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
		return CreateFile_Flash(str1, filetype, brefprotocol);
	}
	break;
	default:
		return false;
		break;
	}

}

bool    ProtocolParse_Writer_Flash::CreateFile_Flash(string foldername, int filetype, bool bRefProtocol)
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
		WriteOneNameSpace_Flash(m_NamespaceVec[i], filetype, foldername);
	}


	return true;
}

bool    ProtocolParse_Writer_Flash::WriteOneNameSpace_Flash(stNamespace& pNamespace, int filetype, string foldername)
{
	m_tablenum = 0;

	//	char ch[256];

		////写函数的id,和协议号
	CreateAndWriteFuncIdAndProtocolNo_Flash(pNamespace, foldername);


	//先写枚举
	for (size_t i = 0; i < pNamespace._EnumVec.size(); i++) {
		WriteEnum_Flash(pNamespace, pNamespace._EnumVec[i], foldername);
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
				WriteStruct_Flash(pNamespace, pNamespace._StructVec[structindex], foldername);
				structindex++;
			}
			else if (pNamespace._VectorVec[vectorindex]._index < pNamespace._StructVec[structindex]._index
				&& (pNamespace._VectorVec[vectorindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index)) {
				//写vector
				WriteVector_Flash(pNamespace, pNamespace._VectorVec[vectorindex], foldername);
				vectorindex++;
			}
			else {
				//写类vector
				WriteWJSVector_Flash(pNamespace, pNamespace._WJSVectorVec[wjsvectorindex], foldername);
				wjsvectorindex++;
			}
		}
		else if (!bvectorend && (!bstructend)) {
			if (pNamespace._StructVec[structindex]._index < pNamespace._VectorVec[vectorindex]._index) {
				//写结构体
				WriteStruct_Flash(pNamespace, pNamespace._StructVec[structindex], foldername);
				structindex++;
			}
			else {
				//写vector
				WriteVector_Flash(pNamespace, pNamespace._VectorVec[vectorindex], foldername);
				vectorindex++;
			}
		}
		else if (!bvectorend && (!bwjsvectorend)) {
			if (pNamespace._VectorVec[vectorindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index) {
				//写vector
				WriteVector_Flash(pNamespace, pNamespace._VectorVec[vectorindex], foldername);
				vectorindex++;
			}
			else {
				//写类vector
				WriteWJSVector_Flash(pNamespace, pNamespace._WJSVectorVec[wjsvectorindex], foldername);
				wjsvectorindex++;
			}
		}
		else if ((!bstructend) && (!bwjsvectorend)) {
			if (pNamespace._StructVec[structindex]._index < pNamespace._WJSVectorVec[wjsvectorindex]._index) {
				//写结构体
				WriteStruct_Flash(pNamespace, pNamespace._StructVec[structindex], foldername);
				structindex++;
			}
			else {
				//写类vector
				WriteWJSVector_Flash(pNamespace, pNamespace._WJSVectorVec[wjsvectorindex], foldername);
				wjsvectorindex++;
			}
		}
		else if (!bvectorend) {
			//写vector
			WriteVector_Flash(pNamespace, pNamespace._VectorVec[vectorindex], foldername);
			vectorindex++;

		}
		else if (!bstructend) {
			//写结构体
			WriteStruct_Flash(pNamespace, pNamespace._StructVec[structindex], foldername);
			structindex++;
		}
		else if (!bwjsvectorend) {
			//写类vector
			WriteWJSVector_Flash(pNamespace, pNamespace._WJSVectorVec[wjsvectorindex], foldername);
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
		WriteClass_Flash(pNamespace._ClassVec[i], filetype, pNamespace, foldername);
	}

	//写结束的中括号}
	//WriteEndZhongKuoKao(fp);
	return true;
}

void    ProtocolParse_Writer_Flash::WriteEnum_Flash(stNamespace& pNamespace, stEnum& pEnum, string foldername)
{
	char t_ch[512];
	sprintf(t_ch, "%s/%s.as", foldername.c_str(), pEnum._name.c_str());

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

	string strEnum;
	sprintf(t_ch, "package %s.%s\n", m_strFlashPackage.c_str(), pNamespace._protocolnamelower.c_str());
	strEnum = t_ch;
	strEnum.append("{\n");
	strEnum.append(strTable_1);
	sprintf(t_ch, "import %s.ByteStream;\n", m_strFlashToolPackage.c_str());
	//strEnum.append("import com.net.ByteStream;\n");
	strEnum.append(t_ch);


	strEnum.append(WriteComment_Flash(strTable_1, pEnum._comment, false, "").c_str());
	strEnum.append(strTable_1);
	sprintf(t_ch, "public final class %s\n", pEnum._name.c_str());
	strEnum.append(t_ch);
	strEnum.append(strTable_1);
	strEnum.append("{\n");


	for (size_t i = 0; i < pEnum._ParNameList.size(); i++) {
		strEnum.append(WriteComment_Flash(strTable_2, pEnum._commentList[i], false, "").c_str());
		strEnum.append(strTable_2);
		sprintf(t_ch, "public static const %s:int = %d;\n", pEnum._ParNameList[i].c_str(), i/*CovertStringToInt(pEnum._ParNameList[i])*/);
		strEnum.append(t_ch);
	}

	//写读
	strEnum.append(strTable_2);
	//sprintf(t_ch,"public static function read(ba:ByteStream):%s\n",pEnum._name.c_str());
	strEnum.append("public static function read(ba:ByteStream):int\n");

	//
	strEnum.append(strTable_2);
	strEnum.append("{\n");

	strEnum.append(strTable_3);
	strEnum.append("var o:int = ba.readInt();\n");

	strEnum.append(strTable_3);
	strEnum.append("switch(o)\n");

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
	strEnum.append("return o;\n");

	strEnum.append(strTable_2);
	strEnum.append("}\n");


	strEnum.append(strTable_1);
	strEnum.append("}\n");

	strEnum.append("}\n");

	fwrite(strEnum.c_str(), strEnum.length(), 1, fp);

	fclose(fp);
}

void    ProtocolParse_Writer_Flash::WriteStruct_Flash(stNamespace& pNamespace, stStructStruct& pStruct, string foldername)
{
	if (pStruct._inherited.length() > 0)
		return;

	char t_ch[512];
	sprintf(t_ch, "%s/%s.as", foldername.c_str(), pStruct._name.c_str());

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


	sprintf(t_ch, "package %s.%s\n", m_strFlashPackage.c_str(), pNamespace._protocolnamelower.c_str());
	strStruct = t_ch;
	strStruct.append("{\n");
	strStruct.append(strTable_1);
	sprintf(t_ch, "import %s.ByteStream;\n", m_strFlashToolPackage.c_str());
	strStruct.append(t_ch);


	if (pStruct.hasInt64Type()) {
		strStruct.append(strTable_1);
		sprintf(t_ch, "import %s.Int64;\n", m_strFlashToolPackage.c_str());
		strStruct.append(t_ch);
	}

	vector<string> tt_PacketNameList;
	GetUsePacketNamesByParType(pStruct._name, tt_PacketNameList);
	RemovePacketNames(tt_PacketNameList, pNamespace._protocolnamelower);

	for (int i = 0; i < (int)tt_PacketNameList.size(); i++) {
		strStruct.append(strTable_1);
		sprintf(t_ch, "import %s.%s.*;\n", m_strFlashPackage.c_str(), tt_PacketNameList[i].c_str());
		strStruct.append(t_ch);
	}

	//strStruct.append("import %s.ByteStream;\n",m_strFlashToolPackage.c_str());
	//strStruct.append(strTable_1);
	//strStruct.append("[Bindable]\n");



	strStruct.append(WriteComment_Flash(strTable_1, pStruct._comment, false, "").c_str());
	strStruct.append(strTable_1);
	sprintf(t_ch, "public  class %s\n", pStruct._name.c_str());
	strStruct.append(t_ch);
	strStruct.append(strTable_1);
	strStruct.append("{\n");

	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		strStruct.append(WriteComment_Flash(strTable_2, pStruct._ParVec[i]._comment, false, "").c_str());
		strStruct.append(strTable_2);
		sprintf(t_ch, "public var %s:%s;\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToFlashType(pStruct._ParVec[i]._strParType.c_str()).c_str());
		strStruct.append(t_ch);
	}

	//写读
	strStruct.append(strTable_2);
	sprintf(t_ch, "public static function read(ba:ByteStream):%s\n", pStruct._name.c_str());
	strStruct.append(t_ch);
	strStruct.append(strTable_2);
	strStruct.append("{\n");

	strStruct.append(strTable_3);
	strStruct.append("var tt_len:int=0;\n");

	strStruct.append(strTable_3);
	strStruct.append("tt_len = ba.readShort();\n");

	strStruct.append(strTable_3);
	strStruct.append("var tt_version:int=0;\n");

	strStruct.append(strTable_3);
	strStruct.append("tt_version = ba.readShort();\n");

	if (pStruct.IsHasTransNeedValuePar()) {
		strStruct.append(strTable_3);
		strStruct.append("var tt_logsize1:int = ba.position;\n");
	}


	if (pStruct.IsHasTransNeedValuePar()) {
		strStruct.append(strTable_3);
		strStruct.append("var tt_transNeedValue:int=0;\n");

		strStruct.append(strTable_3);
		strStruct.append("tt_transNeedValue = ba.readInt();\n");
	}


	strStruct.append(strTable_3);
	sprintf(t_ch, "var o:%s = new %s();\n", pStruct._name.c_str(), pStruct._name.c_str());
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
			if (IsFlashBaseType(pStruct._ParVec[i]._strParType, false)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "o.%s = ba.readInt();\n", pStruct._ParVec[i]._strParName.c_str());
				}
				else {
					sprintf(t_ch, "o.%s = ba.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadFlashType(pStruct._ParVec[i]._strParType.c_str()).c_str());
				}
			}
			else {
				sprintf(t_ch, "o.%s = %s.read(ba);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
			}
			strStruct.append(t_ch);


			strStruct.append(strTable_4);
			strStruct.append("}\n");
			strStruct.append(strTable_4);
			strStruct.append("else\n");
			strStruct.append(strTable_4);
			strStruct.append("{\n");

			string tt_default = GetDefaultValue(pStruct._ParVec[i]._strParType, true);
			if (tt_default.length() > 0) {
				strStruct.append(strTable_5);
				sprintf(t_ch, "o.%s=%s;\n", pStruct._ParVec[i]._strParName.c_str(), tt_default.c_str());
				strStruct.append(t_ch);
			}
			else {
				strStruct.append(strTable_5);
				sprintf(t_ch, "o.%s=%s;\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strDefaultValue.c_str());
				strStruct.append(t_ch);
			}

			strStruct.append(strTable_4);
			strStruct.append("}\n");
		}
		else {
			strStruct.append(strTable_4);
			if (IsFlashBaseType(pStruct._ParVec[i]._strParType, false)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "o.%s = ba.readInt();\n", pStruct._ParVec[i]._strParName.c_str());

				}
				else {
					sprintf(t_ch, "o.%s = ba.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadFlashType(pStruct._ParVec[i]._strParType.c_str()).c_str());
				}
			}
			else {
				sprintf(t_ch, "o.%s = %s.read(ba);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
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
		strStruct.append("var tt_logsize1:int = ba.position;\n");
	}


	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		if (pStruct.IsHasTransNeedValuePar() && (pStruct._ParVec[i]._transNeedValue != -1)) {
			strStruct.append(strTable_4);
			sprintf(t_ch, "if((tt_transNeedValue & (1<<%d))!=0)\n", pStruct._ParVec[i]._transNeedValue);
			strStruct.append(t_ch);
			strStruct.append(strTable_4);
			strStruct.append("{\n");

			strStruct.append(strTable_5);
			if (IsFlashBaseType(pStruct._ParVec[i]._strParType, false)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "o.%s = ba.readInt();\n", pStruct._ParVec[i]._strParName.c_str());
				}
				else {
					sprintf(t_ch, "o.%s = ba.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadFlashType(pStruct._ParVec[i]._strParType.c_str()).c_str());
				}
			}
			else {
				sprintf(t_ch, "o.%s = %s.read(ba);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
			}
			strStruct.append(t_ch);


			strStruct.append(strTable_4);
			strStruct.append("}\n");
			strStruct.append(strTable_4);
			strStruct.append("else\n");
			strStruct.append(strTable_4);
			strStruct.append("{\n");

			string tt_default = GetDefaultValue(pStruct._ParVec[i]._strParType, true);
			if (tt_default.length() > 0) {
				strStruct.append(strTable_5);
				sprintf(t_ch, "o.%s=%s;\n", pStruct._ParVec[i]._strParName.c_str(), tt_default.c_str());
				strStruct.append(t_ch);
			}
			else {
				strStruct.append(strTable_5);
				sprintf(t_ch, "o.%s=%s;\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strDefaultValue.c_str());
				strStruct.append(t_ch);
			}

			strStruct.append(strTable_4);
			strStruct.append("}\n");
		}
		else {
			strStruct.append(strTable_4);
			if (IsFlashBaseType(pStruct._ParVec[i]._strParType, false)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "o.%s = ba.readInt();\n", pStruct._ParVec[i]._strParName.c_str());

				}
				else {
					sprintf(t_ch, "o.%s = ba.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadFlashType(pStruct._ParVec[i]._strParType.c_str()).c_str());
				}
			}
			else {
				sprintf(t_ch, "o.%s = %s.read(ba);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());
			}
			strStruct.append(t_ch);
		}
	}


	strStruct.append(strTable_4);
	strStruct.append("var tt_logsize2:int = ba.position;\n");


	strStruct.append(strTable_4);
	strStruct.append("var tt_offset:int = tt_len - (tt_logsize2 - tt_logsize1);\n");

	strStruct.append(strTable_4);
	strStruct.append("if(tt_offset>0)\n");

	strStruct.append(strTable_4);
	strStruct.append("{\n");

	strStruct.append(strTable_5);
	strStruct.append("ba.position = ba.position + tt_offset;\n");

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
			if (IsFlashBaseType(pStruct._ParVec[i]._strParType, false)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "o.%s = ba.readInt();\n", pStruct._ParVec[i]._strParName.c_str());

				}
				else {
					sprintf(t_ch, "o.%s = ba.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadFlashType(pStruct._ParVec[i]._strParType.c_str()).c_str());
				}
			}
			else {
				sprintf(t_ch, "o.%s = %s.read(ba);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());

			}
			strStruct.append(t_ch);


			strStruct.append(strTable_5);
			strStruct.append("}\n");
			strStruct.append(strTable_5);
			strStruct.append("else\n");
			strStruct.append(strTable_5);
			strStruct.append("{\n");

			string tt_default = GetDefaultValue(pStruct._ParVec[i]._strParType, true);
			if (tt_default.length() > 0) {
				strStruct.append(strTable_6);
				sprintf(t_ch, "o.%s=%s;\n", pStruct._ParVec[i]._strParName.c_str(), tt_default.c_str());
				strStruct.append(t_ch);
			}
			else {
				strStruct.append(strTable_6);
				sprintf(t_ch, "o.%s=%s;\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strDefaultValue.c_str());
				strStruct.append(t_ch);
			}

			strStruct.append(strTable_5);
			strStruct.append("}\n");
		}
		else {
			strStruct.append(strTable_5);
			if (IsFlashBaseType(pStruct._ParVec[i]._strParType, false)) {
				//是否是枚举
				if (IsExType_enum(pStruct._ParVec[i]._strParType)) {
					sprintf(t_ch, "o.%s = ba.readInt();\n", pStruct._ParVec[i]._strParName.c_str());

				}
				else {
					sprintf(t_ch, "o.%s = ba.%s();\n", pStruct._ParVec[i]._strParName.c_str(), TypeConvertToReadFlashType(pStruct._ParVec[i]._strParType.c_str()).c_str());
				}
			}
			else {
				sprintf(t_ch, "o.%s = %s.read(ba);\n", pStruct._ParVec[i]._strParName.c_str(), pStruct._ParVec[i]._strParType.c_str());

			}
			strStruct.append(t_ch);
		}



		strStruct.append(strTable_4);
		strStruct.append("}\n");

		string tt_default = GetDefaultValue(pStruct._ParVec[i]._strParType, true);
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
	strStruct.append("public function write(ba:ByteStream):void\n");
	strStruct.append(strTable_2);
	strStruct.append("{\n");

	//先写入结构体的大小
	strStruct.append(strTable_3);
	strStruct.append("var tt_len:int =0;\n");
	strStruct.append(strTable_3);
	strStruct.append("var tt_logsize1:int = ba.position;\n");
	strStruct.append(strTable_3);
	strStruct.append("ba.writeShort(tt_len);\n");


	//再写入版本
	strStruct.append(strTable_3);
	sprintf(t_ch, "var  tt_version:int = %d;\n", pStruct._version);
	strStruct.append(t_ch);
	strStruct.append(strTable_3);
	strStruct.append("ba.writeShort(tt_version);\n");

	strStruct.append(strTable_3);
	strStruct.append("var tt_logsize2:int = ba.position;\n");

	if (pStruct.IsHasTransNeedValuePar()) {
		strStruct.append(strTable_3);
		strStruct.append("var tt_transNeedValue:int=0;\n");
		strStruct.append(strTable_3);
		strStruct.append("ba.writeInt(tt_transNeedValue);\n");
	}


	for (size_t i = 0; i < pStruct._ParVec.size(); i++) {
		if (pStruct._ParVec[i]._transNeedValue != -1) {
			strStruct.append(strTable_3);

			if (pStruct._ParVec[i]._strParType.compare("__int64") == 0) {
				strStruct.append("if(!%s.isZero())\n");
			}
			else {
				strStruct.append("if(%s!=%s)\n");
			}


			strStruct.append(strTable_3);
			strStruct.append("{\n");

			strStruct.append(strTable_4);
			if (IsFlashBaseType(pStruct._ParVec[i]._strParType, true)) {
				sprintf(t_ch, "ba.%s(%s);\n", TypeConvertToWriteFlashType(pStruct._ParVec[i]._strParType.c_str()).c_str(), pStruct._ParVec[i]._strParName.c_str());
			}
			else {
				sprintf(t_ch, "%s.write(ba);\n", pStruct._ParVec[i]._strParName.c_str());
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
			if (IsFlashBaseType(pStruct._ParVec[i]._strParType, true)) {
				sprintf(t_ch, "ba.%s(%s);\n", TypeConvertToWriteFlashType(pStruct._ParVec[i]._strParType.c_str()).c_str(), pStruct._ParVec[i]._strParName.c_str());
			}
			else {
				sprintf(t_ch, "%s.write(ba);\n", pStruct._ParVec[i]._strParName.c_str());
			}
			strStruct.append(t_ch);
		}



	}
	strStruct.append(strTable_3);
	strStruct.append("var tt_logsize4:int = ba.position;\n");
	strStruct.append(strTable_3);
	strStruct.append("var tt_logsize3:int = tt_logsize4 - tt_logsize2;\n");
	strStruct.append(strTable_3);
	strStruct.append("ba.position = tt_logsize1;\n");
	strStruct.append(strTable_3);
	strStruct.append("ba.writeShort(tt_logsize3);\n");

	if (pStruct.IsHasTransNeedValuePar()) {
		strStruct.append(strTable_3);
		strStruct.append("if(tt_transNeedValue!=0)\n");
		strStruct.append(strTable_3);
		strStruct.append("{\n");
		strStruct.append(strTable_4);
		strStruct.append("ba.position = tt_logsize2;\n");
		strStruct.append(strTable_4);
		strStruct.append("ba.writeInt(tt_transNeedValue);\n");
		strStruct.append(strTable_3);
		strStruct.append("}\n");
	}



	strStruct.append(strTable_3);
	strStruct.append("ba.position = tt_logsize4;\n");

	strStruct.append(strTable_2);
	strStruct.append("}\n");



	strStruct.append(strTable_1);
	strStruct.append("}\n");

	strStruct.append("}\n");

	fwrite(strStruct.c_str(), strStruct.length(), 1, fp);

	fclose(fp);
}

void    ProtocolParse_Writer_Flash::WriteVector_Flash(stNamespace& pNamespace, stVector& pVector, string foldername)
{
	if (pVector._ParTName == "char" || pVector._ParTName == "byte" || pVector._ParTName == "BYTE") {
		return WriteVector2_Flash(pNamespace, pVector, foldername);
	}
	char t_ch[512];
	sprintf(t_ch, "%s/%s.as", foldername.c_str(), pVector._name.c_str());

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

	/////////////有两种版本，一种是非Flash基础类型，一种是
	bool  tt_bFlashBaseType = IsFlashBaseType2(pVector._ParTName);

	sprintf(t_ch, "package %s.%s\n", m_strFlashPackage.c_str(), pNamespace._protocolnamelower.c_str());
	strVector = t_ch;
	strVector.append("{\n");
	if (!tt_bFlashBaseType) {
		strVector.append(strTable_1);
		sprintf(t_ch, "import %s.%s.%s;\n", m_strFlashPackage.c_str(), pNamespace._protocolnamelower.c_str(), pVector._ParTName.c_str());
		strVector.append(t_ch);
	}

	strVector.append(strTable_1);
	sprintf(t_ch, "import %s.ByteStream;\n", m_strFlashToolPackage.c_str());
	//strVector.append("import com.net.ByteStream;\n");
	strVector.append(t_ch);

	if (pVector.isInt64Type()) {
		strVector.append(strTable_1);
		sprintf(t_ch, "import %s.Int64;\n", m_strFlashToolPackage.c_str());
		strVector.append(t_ch);
	}

	vector<string> tt_PacketNameList;
	GetUsePacketNamesByParType(pVector._ParTName, tt_PacketNameList);
	RemovePacketNames(tt_PacketNameList, pNamespace._protocolnamelower);

	for (int i = 0; i < (int)tt_PacketNameList.size(); i++) {
		strVector.append(strTable_1);
		sprintf(t_ch, "import %s.%s.*;\n", m_strFlashPackage.c_str(), tt_PacketNameList[i].c_str());
		strVector.append(t_ch);
	}


	/*strVector.append(strTable_1);
	strVector.append("import mx.collections.ArrayList;\n");
	strVector.append(strTable_1);
	strVector.append("import mx.collections.ArrayCollection;\n");
	*/

	strVector.append(WriteComment_Flash(strTable_1, pVector._comment, false, "").c_str());
	strVector.append(strTable_1);

	sprintf(t_ch, "public dynamic class %s extends Array\n", pVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_1);
	strVector.append("{\n");

	//if(tt_bFlashBaseType)
	//{
	//	strVector.append(strTable_2);
	//	sprintf(t_ch,"private var list:%s;\n",pVector._name.c_str());
	//	strVector.append(t_ch);	
	//}

	strVector.append(strTable_2);
	sprintf(t_ch, "public function %s()\n", pVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	strVector.append("super();\n");

	strVector.append(strTable_2);
	strVector.append("}\n");

	strVector.append(strTable_2);
	sprintf(t_ch, "public static function read(ba:ByteStream):%s\n", pVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	sprintf(t_ch, "var o:%s = new %s();\n", pVector._name.c_str(), pVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_3);
	strVector.append("var count:int = ba.readInt();\n");

	strVector.append(strTable_3);
	strVector.append("if(count<0)\n");

	strVector.append(strTable_3);
	strVector.append("{\n");

	strVector.append(strTable_4);
	strVector.append("throw new Error(\"vecOverFlow\");\n");

	strVector.append(strTable_3);
	strVector.append("}\n");



	strVector.append(strTable_3);
	strVector.append("for(var i:int;i<count;i++)\n");

	strVector.append(strTable_3);
	strVector.append("{\n");

	if (!tt_bFlashBaseType) {
		strVector.append(strTable_4);
		sprintf(t_ch, "o.push(%s.read(ba));\n", pVector._ParTName.c_str());
		strVector.append(t_ch);
	}
	else {
		strVector.append(strTable_4);
		sprintf(t_ch, "o.push(ba.%s());\n", TypeConvertToReadFlashType(pVector._ParTName).c_str());
		strVector.append(t_ch);
	}

	strVector.append(strTable_3);
	strVector.append("}\n");

	strVector.append(strTable_3);
	strVector.append("return o;\n");

	strVector.append(strTable_2);
	strVector.append("}\n");

	strVector.append(strTable_2);
	strVector.append("public function write(ba:ByteStream):void\n");

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	strVector.append("ba.writeInt(length);\n");

	strVector.append(strTable_3);
	strVector.append("for(var i:int;i<length;i++)\n");

	strVector.append(strTable_3);
	strVector.append("{\n");

	if (!tt_bFlashBaseType) {
		strVector.append(strTable_4);
		sprintf(t_ch, "(this[i] as %s).write(ba);\n", pVector._ParTName.c_str());
		strVector.append(t_ch);
	}
	else {
		strVector.append(strTable_4);
		sprintf(t_ch, "ba.%s(this[i] as %s);\n", TypeConvertToWriteFlashType(pVector._ParTName).c_str(), TypeConvertToFlashType(pVector._ParTName).c_str());
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

void    ProtocolParse_Writer_Flash::WriteVector2_Flash(stNamespace& pNamespace, stVector& pVector, string foldername)
{
	char t_ch[512];
	sprintf(t_ch, "%s/%s.as", foldername.c_str(), pVector._name.c_str());

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

	/////////////有两种版本，一种是非Flash基础类型，一种是
	bool  tt_bFlashBaseType = IsFlashBaseType2(pVector._ParTName);

	sprintf(t_ch, "package %s.%s\n", m_strFlashPackage.c_str(), pNamespace._protocolnamelower.c_str());
	strVector = t_ch;
	strVector.append("{\n");
	if (!tt_bFlashBaseType) {
		strVector.append(strTable_1);
		sprintf(t_ch, "import %s.%s.%s;\n", m_strFlashPackage.c_str(), pNamespace._protocolnamelower.c_str(), pVector._ParTName.c_str());
		strVector.append(t_ch);
	}

	strVector.append(strTable_1);
	sprintf(t_ch, "import %s.ByteStream;\n", m_strFlashToolPackage.c_str());
	//strVector.append("import com.net.ByteStream;\n");
	strVector.append(t_ch);

	if (pVector.isInt64Type()) {
		strVector.append(strTable_1);
		sprintf(t_ch, "import %s.Int64;\n", m_strFlashToolPackage.c_str());
		strVector.append(t_ch);
	}

	vector<string> tt_PacketNameList;
	GetUsePacketNamesByParType(pVector._ParTName, tt_PacketNameList);
	RemovePacketNames(tt_PacketNameList, pNamespace._protocolnamelower);

	for (int i = 0; i < (int)tt_PacketNameList.size(); i++) {
		strVector.append(strTable_1);
		sprintf(t_ch, "import %s.%s.*;\n", m_strFlashPackage.c_str(), tt_PacketNameList[i].c_str());
		strVector.append(t_ch);
	}


	/*strVector.append(strTable_1);
	strVector.append("import mx.collections.ArrayList;\n");
	strVector.append(strTable_1);
	strVector.append("import mx.collections.ArrayCollection;\n");
	*/

	strVector.append(strTable_1);

	sprintf(t_ch, "public dynamic class %s extends ByteStream\n", pVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_1);
	strVector.append("{\n");

	//if(tt_bFlashBaseType)
	//{
	//	strVector.append(strTable_2);
	//	sprintf(t_ch,"private var list:%s;\n",pVector._name.c_str());
	//	strVector.append(t_ch);	
	//}

	strVector.append(strTable_2);
	sprintf(t_ch, "public function %s()\n", pVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	strVector.append("super();\n");

	strVector.append(strTable_2);
	strVector.append("}\n");

	strVector.append(strTable_2);
	sprintf(t_ch, "public static function read(ba:ByteStream):%s\n", pVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	sprintf(t_ch, "var o:%s = new %s();\n", pVector._name.c_str(), pVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_3);
	strVector.append("var count:int = ba.readInt();\n");

	strVector.append(strTable_3);
	strVector.append("if(count<0)\n");

	strVector.append(strTable_3);
	strVector.append("{\n");

	strVector.append(strTable_4);
	strVector.append("throw new Error(\"vecOverFlow\");\n");

	strVector.append(strTable_3);
	strVector.append("}\n");

	strVector.append(strTable_3);
	strVector.append("if(count>0)\n");

	strVector.append(strTable_4);
	strVector.append("ba.readBytes(o,0,count);\n");

	strVector.append(strTable_3);
	strVector.append("return o;\n");

	strVector.append(strTable_2);
	strVector.append("}\n");

	strVector.append(strTable_2);
	strVector.append("public function write(ba:ByteStream):void\n");

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	strVector.append("ba.writeInt(length);\n");

	strVector.append(strTable_3);
	strVector.append("if(length>0)\n");

	strVector.append(strTable_4);
	strVector.append("ba.writeBytes(this,0,length);\n");

	strVector.append(strTable_2);
	strVector.append("}\n");

	strVector.append(strTable_1);
	strVector.append("}\n");

	strVector.append("}\n");

	fwrite(strVector.c_str(), strVector.length(), 1, fp);

	fclose(fp);
}
void    ProtocolParse_Writer_Flash::WriteWJSVector_Flash(stNamespace& pNamespace, stWJSVector& pWJSVector, string foldername)
{
	if (pWJSVector._ParTName == "char" || pWJSVector._ParTName == "byte" || pWJSVector._ParTName == "BYTE") {
		return WriteWJSVector2_Flash(pNamespace, pWJSVector, foldername);
	}
	char t_ch[512];
	sprintf(t_ch, "%s/%s.as", foldername.c_str(), pWJSVector._name.c_str());

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

	/////////////有两种版本，一种是非Flash基础类型，一种是
	bool  tt_bFlashBaseType = IsFlashBaseType2(pWJSVector._ParTName);

	sprintf(t_ch, "package %s.%s\n", m_strFlashPackage.c_str(), pNamespace._protocolnamelower.c_str());
	strVector = t_ch;
	strVector.append("{\n");
	if (!tt_bFlashBaseType) {
		strVector.append(strTable_1);
		sprintf(t_ch, "import %s.%s.%s;\n", m_strFlashPackage.c_str(), pNamespace._protocolnamelower.c_str(), pWJSVector._ParTName.c_str());
		strVector.append(t_ch);
	}

	strVector.append(strTable_1);
	sprintf(t_ch, "import %s.ByteStream;\n", m_strFlashToolPackage.c_str());
	strVector.append(t_ch);

	vector<string> tt_PacketNameList;
	GetUsePacketNamesByParType(pWJSVector._ParTName, tt_PacketNameList);
	RemovePacketNames(tt_PacketNameList, pNamespace._protocolnamelower);

	for (int i = 0; i < (int)tt_PacketNameList.size(); i++) {
		strVector.append(strTable_1);
		sprintf(t_ch, "import %s.%s.*;\n", m_strFlashPackage.c_str(), tt_PacketNameList[i].c_str());
		strVector.append(t_ch);
	}

	//strVector.append(strTable_1);
	//strVector.append("import mx.collections.ArrayList;\n");
	//strVector.append(strTable_1);
	//strVector.append("import mx.collections.ArrayCollection;\n");

	strVector.append(strTable_1);

	sprintf(t_ch, "public dynamic class %s extends Array\n", pWJSVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_1);
	strVector.append("{\n");

	//if(tt_bFlashBaseType)
	//{
	//	strVector.append(strTable_2);
	//	sprintf(t_ch,"private var list:%s;\n",pWJSVector._name.c_str());
	//	strVector.append(t_ch);	
	//}

	strVector.append(strTable_2);
	sprintf(t_ch, "public function %s()\n", pWJSVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	strVector.append("super();\n");

	strVector.append(strTable_2);
	strVector.append("}\n");

	strVector.append(strTable_2);
	sprintf(t_ch, "public static function read(ba:ByteStream):%s\n", pWJSVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	sprintf(t_ch, "var o:%s = new %s();\n", pWJSVector._name.c_str(), pWJSVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_3);
	strVector.append("var count:int = ba.readInt();\n");

	strVector.append(strTable_3);
	strVector.append("for(var i:int;i<count;i++)\n");

	strVector.append(strTable_3);
	strVector.append("{\n");

	if (!tt_bFlashBaseType) {
		strVector.append(strTable_4);
		sprintf(t_ch, "o.push(%s.read(ba));\n", pWJSVector._ParTName.c_str());
		strVector.append(t_ch);
	}
	else {
		strVector.append(strTable_4);
		sprintf(t_ch, "o.push(ba.%s());\n", TypeConvertToReadFlashType(pWJSVector._ParTName).c_str());
		strVector.append(t_ch);
	}

	strVector.append(strTable_3);
	strVector.append("}\n");

	strVector.append(strTable_3);
	strVector.append("return o;\n");

	strVector.append(strTable_2);
	strVector.append("}\n");

	strVector.append(strTable_2);
	strVector.append("public function write(ba:ByteStream):void\n");

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	strVector.append("ba.writeInt(length);\n");

	strVector.append(strTable_3);
	strVector.append("for(var i:int;i<length;i++)\n");

	strVector.append(strTable_3);
	strVector.append("{\n");

	if (!tt_bFlashBaseType) {
		strVector.append(strTable_4);
		sprintf(t_ch, "(this[i] as %s).write(ba);\n", pWJSVector._ParTName.c_str());
		strVector.append(t_ch);
	}
	else {
		strVector.append(strTable_4);
		sprintf(t_ch, "ba.%s(this[i] as %s);\n", TypeConvertToWriteFlashType(pWJSVector._ParTName).c_str(), TypeConvertToFlashType(pWJSVector._ParTName).c_str());
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
void    ProtocolParse_Writer_Flash::WriteWJSVector2_Flash(stNamespace& pNamespace, stWJSVector& pWJSVector, string foldername)
{
	char t_ch[512];
	sprintf(t_ch, "%s/%s.as", foldername.c_str(), pWJSVector._name.c_str());

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

	/////////////有两种版本，一种是非Flash基础类型，一种是
	bool  tt_bFlashBaseType = IsFlashBaseType2(pWJSVector._ParTName);

	sprintf(t_ch, "package %s.%s\n", m_strFlashPackage.c_str(), pNamespace._protocolnamelower.c_str());
	strVector = t_ch;
	strVector.append("{\n");
	if (!tt_bFlashBaseType) {
		strVector.append(strTable_1);
		sprintf(t_ch, "import %s.%s.%s;\n", m_strFlashPackage.c_str(), pNamespace._protocolnamelower.c_str(), pWJSVector._ParTName.c_str());
		strVector.append(t_ch);
	}

	strVector.append(strTable_1);
	sprintf(t_ch, "import %s.ByteStream;\n", m_strFlashToolPackage.c_str());
	strVector.append(t_ch);

	vector<string> tt_PacketNameList;
	GetUsePacketNamesByParType(pWJSVector._ParTName, tt_PacketNameList);
	RemovePacketNames(tt_PacketNameList, pNamespace._protocolnamelower);

	for (int i = 0; i < (int)tt_PacketNameList.size(); i++) {
		strVector.append(strTable_1);
		sprintf(t_ch, "import %s.%s.*;\n", m_strFlashPackage.c_str(), tt_PacketNameList[i].c_str());
		strVector.append(t_ch);
	}



	strVector.append(strTable_1);

	sprintf(t_ch, "public dynamic class %s extends ByteStream\n", pWJSVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_1);
	strVector.append("{\n");


	strVector.append(strTable_2);
	sprintf(t_ch, "public function %s()\n", pWJSVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	strVector.append("super();\n");

	strVector.append(strTable_2);
	strVector.append("}\n");

	strVector.append(strTable_2);
	sprintf(t_ch, "public static function read(ba:ByteStream):%s\n", pWJSVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	sprintf(t_ch, "var o:%s = new %s();\n", pWJSVector._name.c_str(), pWJSVector._name.c_str());
	strVector.append(t_ch);

	strVector.append(strTable_3);
	strVector.append("var count:int = ba.readInt();\n");

	strVector.append(strTable_3);
	strVector.append("if(count>0)\n");

	strVector.append(strTable_4);
	strVector.append("ba.readBytes(o,0,count);\n");

	strVector.append(strTable_3);
	strVector.append("return o;\n");

	strVector.append(strTable_2);
	strVector.append("}\n");

	strVector.append(strTable_2);
	strVector.append("public function write(ba:ByteStream):void\n");

	strVector.append(strTable_2);
	strVector.append("{\n");

	strVector.append(strTable_3);
	strVector.append("ba.writeInt(length);\n");

	strVector.append(strTable_3);
	strVector.append("if(length>0)\n");

	strVector.append(strTable_4);
	strVector.append("ba.writeBytes(this,0,length);\n");

	strVector.append(strTable_2);
	strVector.append("}\n");

	strVector.append(strTable_1);
	strVector.append("}\n");

	strVector.append("}\n");



	fwrite(strVector.c_str(), strVector.length(), 1, fp);

	fclose(fp);
}




void    ProtocolParse_Writer_Flash::CreateAndWriteFuncIdAndProtocolNo_Flash(stNamespace& pNamespace, string foldername)
{
	if (pNamespace._ClassVec.size() == 0)
		return;
	char t_ch[512];
	sprintf(t_ch, "%s/ID.as", foldername.c_str());

	//先创建文件
	FILE* fp = fopen(t_ch, "w");
	if (!fp)
		return;

	//写信息头,共用
	WriteInfoHead(fp);

	string strid;

	string strTable_1 = CreateTableString(1);
	string strTable_2 = CreateTableString(2);
	//string strTable_3=CreateTableString(3);

	sprintf(t_ch, "package %s.%s\n", m_strFlashPackage.c_str(), pNamespace._protocolnamelower.c_str());
	strid.append(t_ch);
	strid.append("{\n");

	strid.append(strTable_1);
	strid.append("internal final class ID\n");

	strid.append(strTable_1);
	strid.append("{\n");

	//函数id
	for (size_t i = 0; i < pNamespace._ClassVec.size(); i++) {
		for (size_t j = 0; j < pNamespace._ClassVec[i]._FuncVec.size(); j++) {
			strid.append(strTable_2);
			strid.append("public static  const ");
			sprintf(t_ch, "ID_%s_%s", pNamespace._ClassVec[i]._name.c_str(), pNamespace._ClassVec[i]._FuncVec[j]._name.c_str());
			strid.append(t_ch);
			strid.append(":int");

			int tempId;  //将字符串转成id
			if (pNamespace._ClassVec[i]._FuncVec[j]._userFuncId != 0)
				tempId = pNamespace._ClassVec[i]._FuncVec[j]._userFuncId;
			else
				tempId = CovertStringToInt(t_ch, pNamespace._ClassVec[i]._FuncVec[j]._bNew);
			//int tempId=CovertStringToInt(t_ch,pNamespace._ClassVec[i]._FuncVec[j]._bNew);  //将字符串转成id
			sprintf(t_ch, "=%d;\n", tempId);
			strid.append(t_ch);
		}
	}

	//协议号
	if (m_ProtocolNo._value != 1) {
		//协议头
		strid.append(strTable_2);
		strid.append("public static  const ");
		sprintf(t_ch, "%s:int=%d;\n", m_ProtocolNo._name.c_str(), m_ProtocolNo._value);
		strid.append(t_ch);
	}
	strid.append(strTable_1);
	strid.append("}\n");

	strid.append("}\n");

	fwrite(strid.c_str(), strid.length(), 1, fp);

	fclose(fp);
}

void    ProtocolParse_Writer_Flash::WriteClass_Flash(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername)
{
	switch (filetype) {
	case 0:
	{
		////对于客户端而言，request的为发送，callback的为回调
		if (pClass._strtype.compare("callback") == 0) {
			WriteClass_IReceiver_Flash(pClass, filetype, pNamespace, foldername, false);
			WriteClass_Receiver_Flash(pClass, filetype, pNamespace, foldername);
		}
		else {
			WriteClass_Send_Flash(pClass, filetype, pNamespace, foldername);
		}
	}
	break;
	case 1:
	{
		//服务器而言，请求的是回调，callback是发送
		if (pClass._strtype.compare("request") == 0) //C2S
		{
			WriteClass_IReceiver_Flash(pClass, filetype, pNamespace, foldername, false);
			WriteClass_Receiver_Flash(pClass, filetype, pNamespace, foldername);
		}
		else //S2C
		{
			WriteClass_Send_Flash(pClass, filetype, pNamespace, foldername);
		}
	}
	break;
	case 2:
	{
		//取一下callback的类名

		//string strcallbackclassname;
		//for(int i=0;i<pNamespace._ClassVec.size();i++)
		//{
		//	if(pNamespace._ClassVec[i]._strtype.compare("callback")==0)
		//	{
		//		strcallbackclassname = pNamespace._ClassVec[i]._name;
		//		break;
		//	}
		//}

		//事实上两个里面是一样的
		if (pClass._strtype.compare("request") == 0) //C2S
		{
			WriteClass_IReceiver_Flash(pClass, filetype, pNamespace, foldername, true);
			WriteClass_Receiver_Flash(pClass, filetype, pNamespace, foldername);

			WriteClass_Send_Flash(pClass, filetype, pNamespace, foldername);
		}
		//else //S2C
		//{
		//}

	}
	break;
	default:
		break;
	}
}

void    ProtocolParse_Writer_Flash::WriteClass_Send_Flash(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername)
{
	char t_ch[512];
	sprintf(t_ch, "%s/%s_Send.as", foldername.c_str(), pNamespace._protocolname.c_str());

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

	sprintf(t_ch, "package %s.%s\n", m_strFlashPackage.c_str(), pNamespace._protocolnamelower.c_str());
	strclass.append(t_ch);

	strclass.append("{\n");

	strclass.append(strTable_1);
	sprintf(t_ch, "import %s.%s.ID;\n", m_strFlashPackage.c_str(), pNamespace._protocolnamelower.c_str());
	strclass.append(t_ch);

	vector<string> tt_usePacketNameList;
	GetUsePacketNamesByRefHead(tt_usePacketNameList, true);

	for (int i = 0; i < (int)tt_usePacketNameList.size(); i++) {
		strclass.append(strTable_1);
		sprintf(t_ch, "import %s.%s.*;\n", m_strFlashPackage.c_str(), tt_usePacketNameList[i].c_str());
		strclass.append(t_ch);
	}

	strclass.append(strTable_1);
	sprintf(t_ch, "import %s.ByteStream;\n", m_strFlashToolPackage.c_str());
	strclass.append(t_ch);

	if (pClass.hasInt64Type()) {
		strclass.append(strTable_1);
		sprintf(t_ch, "import %s.Int64;\n", m_strFlashToolPackage.c_str());
		strclass.append(t_ch);
	}


	strclass.append(strTable_1);
	sprintf(t_ch, "public class %s_Send\n", pNamespace._protocolname.c_str());
	strclass.append(t_ch);

	strclass.append(strTable_1);
	strclass.append("{\n");


	fwrite(strclass.c_str(), strclass.length(), 1, fp);
	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		//写函数
		WriteFunc_SendByteSeq_Flash(pClass._FuncVec[i], 2, pClass, pNamespace, foldername, fp);
	}


	strclass = "";
	strclass.append(strTable_1);
	strclass.append("}\n");

	strclass.append("}\n");


	fwrite(strclass.c_str(), strclass.length(), 1, fp);

	fclose(fp);





}

void    ProtocolParse_Writer_Flash::WriteFunc_SendByteSeq_Flash(stFunc& pFunc, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp)//发送流
{
	char t_ch[1024];


	string strTable_0 = CreateTableString(tablenum);
	string strTable_1 = CreateTableString(tablenum + 1);
	string strTable_2 = CreateTableString(tablenum + 2);

	string strfunc;

	strfunc.append(strTable_0);
	strfunc.append("/**\n");

	strfunc.append(WriteComment_Flash(strTable_0, pFunc._comment, false, "", false).c_str());
	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		strfunc.append(WriteComment_Flash(strTable_0, pFunc._InParVec[i]._comment, true, pFunc._InParVec[i]._strParName, false).c_str());
	}

	strfunc.append(strTable_0);
	strfunc.append(" */\n");


	strfunc.append(strTable_0);
	sprintf(t_ch, "public static function Send_%s(", pFunc._name.c_str());
	strfunc.append(t_ch);

	//有参
	if (pFunc._InParVec.size() > 0) {
		for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
			if (i == pFunc._InParVec.size() - 1) {
				sprintf(t_ch, "%s:%s", pFunc._InParVec[i]._strParName.c_str(), TypeConvertToFlashType(pFunc._InParVec[i]._strParType).c_str());
				strfunc.append(t_ch);
			}
			else {
				sprintf(t_ch, "%s:%s,", pFunc._InParVec[i]._strParName.c_str(), TypeConvertToFlashType(pFunc._InParVec[i]._strParType).c_str());
				strfunc.append(t_ch);
			}

		}

	}

	strfunc.append("):ByteStream\n");
	strfunc.append(strTable_0);
	strfunc.append("{\n");

	strfunc.append(strTable_1);
	strfunc.append("var ba:ByteStream = new ByteStream();\n");

	//先写协议号
	strfunc.append(strTable_1);
	sprintf(t_ch, "ba.writeInt(ID.%s);\n", m_ProtocolNo._name.c_str());
	strfunc.append(t_ch);

	//写版本号

	strfunc.append(strTable_1);
	sprintf(t_ch, "var tt_version:int = %d;\n", pFunc._version);
	strfunc.append(t_ch);
	strfunc.append(strTable_1);
	strfunc.append("ba.writeShort(tt_version);\n");

	//再写函数号
	strfunc.append(strTable_1);
	sprintf(t_ch, "ba.writeInt(ID.ID_%s_%s);\n", pClass._name.c_str(), pFunc._name.c_str());
	strfunc.append(t_ch);

	//
	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		strfunc.append(strTable_1);
		if (IsFlashBaseType(pFunc._InParVec[i]._strParType, true)) {
			sprintf(t_ch, "ba.%s(%s);\n", TypeConvertToWriteFlashType(pFunc._InParVec[i]._strParType.c_str()).c_str(), pFunc._InParVec[i]._strParName.c_str());

		}
		else {
			sprintf(t_ch, "%s.write(ba);\n", pFunc._InParVec[i]._strParName.c_str());

		}
		strfunc.append(t_ch);
	}

	strfunc.append(strTable_1);
	strfunc.append("return ba;\n");
	strfunc.append(strTable_0);
	strfunc.append("}\n");

	fwrite(strfunc.c_str(), strfunc.length(), 1, fp);
}

void    ProtocolParse_Writer_Flash::WriteClass_IReceiver_Flash(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername, bool bemptyimplement)//接收接口
{
	char t_ch[1024];
	sprintf(t_ch, "%s/IReceiver_%s.as", foldername.c_str(), pNamespace._protocolname.c_str());

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

	sprintf(t_ch, "package %s.%s\n", m_strFlashPackage.c_str(), pNamespace._protocolnamelower.c_str());
	strclass.append(t_ch);

	strclass.append("{\n");

	vector<string> tt_usePacketNameList;
	GetUsePacketNamesByRefHead(tt_usePacketNameList, true);

	for (int i = 0; i < (int)tt_usePacketNameList.size(); i++) {
		strclass.append(strTable_1);
		sprintf(t_ch, "import %s.%s.*;\n", m_strFlashPackage.c_str(), tt_usePacketNameList[i].c_str());
		strclass.append(t_ch);
	}

	//strclass.append(strTable_1);
	//sprintf(t_ch,"import %s.%s.*;\n",m_strFlashPackage.c_str(),pNamespace._protocolnamelower.c_str());	
	//strclass.append(t_ch);

	if (pClass.hasInt64Type()) {
		strclass.append(strTable_1);
		sprintf(t_ch, "import %s.Int64;\n", m_strFlashToolPackage.c_str());
		strclass.append(t_ch);
	}

	strclass.append(strTable_1);
	sprintf(t_ch, "public interface IReceiver_%s\n", pNamespace._protocolname.c_str());
	strclass.append(t_ch);

	strclass.append(strTable_1);
	strclass.append("{\n");

	fwrite(strclass.c_str(), strclass.length(), 1, fp);


	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		//写函数
		WriteFunc_IReceiver_Flash(pClass._FuncVec[i], filetype, 2, pClass, pNamespace, foldername, fp, bemptyimplement);
	}

	strclass = "";
	strclass.append(strTable_1);
	strclass.append("}\n");

	strclass.append("}\n");

	fwrite(strclass.c_str(), strclass.length(), 1, fp);

	fclose(fp);



	///////////////
	//
	///////////////
	sprintf(t_ch, "%s/IReceiver_%s.aas", foldername.c_str(), pNamespace._protocolname.c_str());

	//先创建文件
	fp = fopen(t_ch, "w");
	if (!fp)
		return;

	strclass = "";

	//写信息头,共用
	WriteInfoHead(fp);

	strclass.append(strTable_1);
	sprintf(t_ch, "public interface IReceiver_%s\n", pNamespace._protocolname.c_str());
	strclass.append(t_ch);

	strclass.append(strTable_1);
	strclass.append("{\n");

	fwrite(strclass.c_str(), strclass.length(), 1, fp);


	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		//写函数
		WriteFunc_IReceiver_Flash2(pClass._FuncVec[i], filetype, 2, pClass, pNamespace, foldername, fp, bemptyimplement);
	}

	strclass = "";
	strclass.append(strTable_1);
	strclass.append("}\n");

	strclass.append("}\n");

	fwrite(strclass.c_str(), strclass.length(), 1, fp);

	fclose(fp);
}

void    ProtocolParse_Writer_Flash::WriteFunc_IReceiver_Flash(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp, bool bemptyimplement) //
{
	char t_ch[1024];

	string strTable_0 = CreateTableString(tablenum);
	string strTable_1 = CreateTableString(tablenum + 1);


	string strFunc;

	strFunc.append(strTable_0);
	strFunc.append("/**\n");

	strFunc.append(WriteComment_Flash(strTable_0, pFunc._comment, false, "", false).c_str());
	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		strFunc.append(WriteComment_Flash(strTable_0, pFunc._InParVec[i]._comment, true, pFunc._InParVec[i]._strParName, false).c_str());
	}
	strFunc.append(strTable_0);
	strFunc.append(" */\n");


	strFunc.append(strTable_0);
	sprintf(t_ch, "function IReceiver_%s(", pFunc._name.c_str());
	strFunc.append(t_ch);

	if (filetype == 0) {
		if (pFunc._InParVec.size() > 0) {
			for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
				sprintf(t_ch, "%s:%s,", pFunc._InParVec[i]._strParName.c_str(), TypeConvertToFlashType(pFunc._InParVec[i]._strParType).c_str());
				strFunc.append(t_ch);
			}
		}

		strFunc.append("pExData:Object):void;\n");
	}
	else {
		if (pFunc._InParVec.size() > 0) {
			for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
				sprintf(t_ch, "%s:%s,", pFunc._InParVec[i]._strParName.c_str(), TypeConvertToFlashType(pFunc._InParVec[i]._strParType).c_str());
				strFunc.append(t_ch);
			}
		}

		strFunc.append("sessionId:uint,pExData:Object):void;\n");
	}


	fwrite(strFunc.c_str(), strFunc.length(), 1, fp);
}
void    ProtocolParse_Writer_Flash::WriteFunc_IReceiver_Flash2(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp, bool bemptyimplement) //
{
	char t_ch[1024];

	string strTable_0 = CreateTableString(tablenum);
	string strTable_1 = CreateTableString(tablenum + 1);


	string strFunc;
	strFunc.append(strTable_0);
	sprintf(t_ch, "public function IReceiver_%s(", pFunc._name.c_str());
	strFunc.append(t_ch);

	if (filetype == 0) {
		if (pFunc._InParVec.size() > 0) {
			for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
				sprintf(t_ch, "%s:%s,", pFunc._InParVec[i]._strParName.c_str(), TypeConvertToFlashType(pFunc._InParVec[i]._strParType).c_str());
				strFunc.append(t_ch);
			}
		}

		strFunc.append("pExData:Object):void{};\n");
	}
	else {
		if (pFunc._InParVec.size() > 0) {
			for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
				sprintf(t_ch, "%s:%s,", pFunc._InParVec[i]._strParName.c_str(), TypeConvertToFlashType(pFunc._InParVec[i]._strParType).c_str());
				strFunc.append(t_ch);
			}
		}

		strFunc.append("sessionId:uint,pExData:Object):void{};\n");
	}


	fwrite(strFunc.c_str(), strFunc.length(), 1, fp);
}


void    ProtocolParse_Writer_Flash::WriteClass_Receiver_Flash(stClass& pClass, int filetype, stNamespace& pNamespace, string foldername)
{
	char t_ch[1024];
	sprintf(t_ch, "%s/%s_Callback.as", foldername.c_str(), pNamespace._protocolname.c_str());

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

	sprintf(t_ch, "package %s.%s\n", m_strFlashPackage.c_str(), pNamespace._protocolnamelower.c_str());
	strclass.append(t_ch);

	strclass.append("{\n");

	strclass.append(strTable_1);
	sprintf(t_ch, "import %s.ByteStream;\n", m_strFlashToolPackage.c_str());
	strclass.append(t_ch);
	//strclass.append("import %s.ByteStream;\n",m_strFlashToolPackage.c_str());

	vector<string> tt_usePacketNameList;
	GetUsePacketNamesByRefHead(tt_usePacketNameList, true);

	for (int i = 0; i < (int)tt_usePacketNameList.size(); i++) {
		strclass.append(strTable_1);
		sprintf(t_ch, "import %s.%s.*;\n", m_strFlashPackage.c_str(), tt_usePacketNameList[i].c_str());
		strclass.append(t_ch);
	}

	//导id
	strclass.append(strTable_1);
	sprintf(t_ch, "import %s.%s.ID;\n", m_strFlashPackage.c_str(), pNamespace._protocolnamelower.c_str());
	strclass.append(t_ch);

	//导接口
	strclass.append(strTable_1);
	sprintf(t_ch, "import %s.%s.IReceiver_%s;\n", m_strFlashPackage.c_str(), pNamespace._protocolnamelower.c_str(), pNamespace._protocolname.c_str());
	strclass.append(t_ch);

	if (pClass.hasInt64Type()) {
		strclass.append(strTable_1);
		sprintf(t_ch, "import %s.Int64;\n", m_strFlashToolPackage.c_str());
		strclass.append(t_ch);
	}


	strclass.append(strTable_1);
	sprintf(t_ch, "public class %s_Callback\n", pNamespace._protocolname.c_str());
	strclass.append(t_ch);

	strclass.append(strTable_1);
	strclass.append("{\n");
	fwrite(strclass.c_str(), strclass.length(), 1, fp);

	WriteFunc_ReceiverParser_Flash(pClass, filetype, 2, pNamespace, foldername, fp);


	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		WriteFunc_Receiver_Flash(pClass._FuncVec[i], filetype, 2, pClass, pNamespace, foldername, fp);
	}


	strclass = "";
	strclass.append(strTable_1);
	strclass.append("}\n");

	strclass.append("}\n");
	fwrite(strclass.c_str(), strclass.length(), 1, fp);

	fclose(fp);

}

void    ProtocolParse_Writer_Flash::WriteFunc_Receiver_Flash(stFunc& pFunc, int filetype, int tablenum, stClass& pClass, stNamespace& pNamespace, string foldername, FILE* fp)
{
	char t_ch[1024];

	string strTable_0 = CreateTableString(tablenum);
	string strTable_1 = CreateTableString(tablenum + 1);
	string strTable_2 = CreateTableString(tablenum + 2);
	string strTable_3 = CreateTableString(tablenum + 3);


	string strFunc;

	if (filetype == 0) {
		strFunc.append(strTable_0);
		sprintf(t_ch, "private static function _parser_%s(cls:IReceiver_%s,ba:ByteStream,pExData:Object,proversionno:int):void\n", pFunc._name.c_str(), pNamespace._protocolname.c_str());
		strFunc.append(t_ch);
	}
	else {
		strFunc.append(strTable_0);
		sprintf(t_ch, "private static function _parser_%s(cls:IReceiver_%s,ba:ByteStream,sessionId:uint,pExData:Object,proversionno:int):void\n", pFunc._name.c_str(), pNamespace._protocolname.c_str());
		strFunc.append(t_ch);
	}

	strFunc.append(strTable_0);
	strFunc.append("{\n");

	for (size_t i = 0; i < pFunc._InParVec.size(); i++) {
		strFunc.append(strTable_1);
		if (IsFlashBaseType(pFunc._InParVec[i]._strParType, false) || IsExType_enum(pFunc._InParVec[i]._strParType)) {
			sprintf(t_ch, "var %s:%s;\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToFlashType(pFunc._InParVec[i]._strParType)).c_str());

		}
		else {
			sprintf(t_ch, "var %s:%s;\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToFlashType(pFunc._InParVec[i]._strParType)).c_str());
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
		if (IsFlashBaseType(pFunc._InParVec[i]._strParType, false)) {
			sprintf(t_ch, "%s = ba.%s();\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToReadFlashType(pFunc._InParVec[i]._strParType)).c_str());

		}
		else if (IsExType_enum(pFunc._InParVec[i]._strParType)) {
			sprintf(t_ch, "%s = %s.read(ba);\n", pFunc._InParVec[i]._strParName.c_str(), pFunc._InParVec[i]._strParType.c_str());
		}
		else {
			sprintf(t_ch, "%s = %s.read(ba);\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToFlashType(pFunc._InParVec[i]._strParType)).c_str());
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
		if (IsFlashBaseType(pFunc._InParVec[i]._strParType, false)) {
			sprintf(t_ch, "%s = ba.%s();\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToReadFlashType(pFunc._InParVec[i]._strParType)).c_str());
		}
		else if (IsExType_enum(pFunc._InParVec[i]._strParType)) {
			sprintf(t_ch, "%s = %s.read(ba);\n", pFunc._InParVec[i]._strParName.c_str(), pFunc._InParVec[i]._strParType.c_str());
		}
		else {
			sprintf(t_ch, "%s = %s.read(ba);\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToFlashType(pFunc._InParVec[i]._strParType)).c_str());
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
		if (IsFlashBaseType(pFunc._InParVec[i]._strParType, false)) {
			sprintf(t_ch, "%s = ba.%s();\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToReadFlashType(pFunc._InParVec[i]._strParType)).c_str());

		}
		else if (IsExType_enum(pFunc._InParVec[i]._strParType)) {
			sprintf(t_ch, "%s = %s.read(ba);\n", pFunc._InParVec[i]._strParName.c_str(), pFunc._InParVec[i]._strParType.c_str());
		}
		else {
			sprintf(t_ch, "%s = %s.read(ba);\n", pFunc._InParVec[i]._strParName.c_str(), (TypeConvertToFlashType(pFunc._InParVec[i]._strParType)).c_str());
		}
		strFunc.append(t_ch);

		string tt_default = GetDefaultValue(pFunc._InParVec[i]._strParType, true);
		if (tt_default.length() > 0) {
			strFunc.append(strTable_2);
			strFunc.append("else\n");

			strFunc.append(strTable_3);
			sprintf(t_ch, "%s = %s;\n", pFunc._InParVec[i]._strParName.c_str(), tt_default.c_str());
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

void    ProtocolParse_Writer_Flash::WriteFunc_ReceiverParser_Flash(stClass& pClass, int filetype, int tablenum, stNamespace& pNamespace, string foldername, FILE* fp)
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
		sprintf(t_ch, "public static function Parser(cls:IReceiver_%s,ba:ByteStream,pExData:Object=null):Boolean\n", pNamespace._protocolname.c_str());
		strFunc.append(t_ch);
	}
	else {
		sprintf(t_ch, "public static function Parser(cls:IReceiver_%s,ba:ByteStream,sessionId:uint=0,pExData:Object=null):Boolean\n", pNamespace._protocolname.c_str());
		strFunc.append(t_ch);
	}

	strFunc.append(strTable_0);
	strFunc.append("{\n");

	strFunc.append(strTable_1);
	strFunc.append("var no:int = ba.readInt();\n");

	strFunc.append(strTable_1);
	sprintf(t_ch, "if(no!=ID.%s)\n", m_ProtocolNo._name.c_str());
	strFunc.append(t_ch);

	strFunc.append(strTable_1);
	strFunc.append("{\n");

	strFunc.append(strTable_2);
	strFunc.append("ba.position = ba.position - 4;\n");

	strFunc.append(strTable_2);
	strFunc.append("return false;\n");

	strFunc.append(strTable_1);
	strFunc.append("}\n");

	strFunc.append(strTable_1);
	strFunc.append("var proversionno:int = ba.readShort();\n");


	strFunc.append(strTable_1);
	strFunc.append("var methodid:int =ba.readInt();\n");

	strFunc.append(strTable_1);
	strFunc.append("switch(methodid)\n");

	strFunc.append(strTable_1);
	strFunc.append("{\n");

	for (size_t i = 0; i < pClass._FuncVec.size(); i++) {
		if (pClass._FuncVec[i]._bDiscard)
			continue;
		strFunc.append(strTable_2);
		sprintf(t_ch, "case ID.ID_%s_%s:\n", pClass._name.c_str(), pClass._FuncVec[i]._name.c_str());
		strFunc.append(t_ch);


		strFunc.append(strTable_3);
		if (filetype == 0) {
			sprintf(t_ch, "_parser_%s(cls,ba,pExData,proversionno);\n", pClass._FuncVec[i]._name.c_str());
			strFunc.append(t_ch);
		}
		else {
			sprintf(t_ch, "_parser_%s(cls,ba,sessionId,pExData,proversionno);\n", pClass._FuncVec[i]._name.c_str());
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


	strFunc.append(strTable_1);
	strFunc.append("return false;\n");

	strFunc.append(strTable_0);
	strFunc.append("}\n");


	fwrite(strFunc.c_str(), strFunc.length(), 1, fp);
}




string  ProtocolParse_Writer_Flash::WriteComment_Flash(const string& strTable, const string& strComment, bool bAsParam, const string& strPar, bool bWithSummary)
{
	if (strComment.length() == 0)
		return "";
	char tt_ch[2048];

	string strLine;

	vector<string> tt_subList;
	ProtocolUtils::GetAllSubString(strComment, "split", tt_subList, false);

	if (tt_subList.size() > 0 && bWithSummary) {
		strLine.append(strTable);
		strLine.append("/**\n");
	}

	for (unsigned int i = 0; i < tt_subList.size(); i++) {
		strLine.append(strTable);
		if (strPar.length() > 0 && i == 0) {
			if (bAsParam)
				sprintf(tt_ch, " *@param %s  %s\n", strPar.c_str(), tt_subList[i].c_str());
			else
				sprintf(tt_ch, " *%s  %s\n", strPar.c_str(), tt_subList[i].c_str());
		}
		else {
			sprintf(tt_ch, " *%s\n", tt_subList[i].c_str());
		}

		strLine.append(tt_ch);

	}

	if (tt_subList.size() > 0 && bWithSummary) {
		strLine.append(strTable);
		strLine.append(" */\n");
	}
	return strLine;

}

#pragma region 类型转换与判断
bool    ProtocolParse_Writer_Flash::IsFlashBaseType2(string strTypeName)
{
	if (IsBaseType(strTypeName))
		return true;

	return false;

}

string  ProtocolParse_Writer_Flash::TypeConvertToFlashType(const string& strTypeName)
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
				return "int";
			case 4://unsigned short
				return "int";
			case 5://char
				return "int";

			case 11://float
				return "Number";
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
				return "int";
			case 9://BOOL
				return "int";
			case 8://bool
				return "Boolean";
			case 10://DWORD
				return "uint";
			case 14://string
			case 24://wjs_utf8
				return "String";
			case 15://__int64
			case 22://long long
				return "Int64";
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

string  ProtocolParse_Writer_Flash::TypeConvertToReadFlashType(const string& strTypeName)
{
	for (int i = 0; i < GetBaseTypeNum(); i++) {
		if (g_strBaseType[i].compare(strTypeName.c_str()) == 0) {
			switch (i) {
			case 0://void
				return "readInt";
			case 1://int
				return "readInt";
			case 2://unsigned int
				return "readUnsignedInt";
			case 3://short
				return "readShort";
			case 4://unsigned short
				return "readUnsignedShort";
			case 5://char
				return "readByte";


			case 11://float
				return "readFloat";
			case 12://long
				return "readInt";
			case 13://unsigned long
				return "readUnsignedInt";



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
				return "readByte";
			case 9://BOOL
				return "readInt";
			case 8://bool
				return "readBoolean";
			case 10://DWORD
				return "readUnsignedInt";
			case 14://string
				return "readString";
			case 24://wjs_utf8
				return "readUtf8";
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
		return "readInt";
	}

	return "";
}

string  ProtocolParse_Writer_Flash::TypeConvertToWriteFlashType(const string& strTypeName)
{
	for (int i = 0; i < GetBaseTypeNum(); i++) {
		if (g_strBaseType[i].compare(strTypeName.c_str()) == 0) {
			switch (i) {
			case 0://void
				return "writeInt";
			case 1://int
				return "writeInt";
			case 2://unsigned int
				return "writeUnsignedInt";
			case 3://short
				return "writeShort";
			case 4://unsigned short
				return "writeShort";
			case 5://char
				return "writeByte";

			case 11://float
				return "writeFloat";
			case 12://long
				return "writeInt";
			case 13://unsigned long
				return "writeUnsignedInt";



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
				return "writeByte";
			case 9://BOOL
				return "writeInt";
			case 8://bool
				return "writeBoolean";
			case 10://DWORD
				return "writeUnsigendInt";
			case 14://string
				return "writeString";
			case 24://wjs_utf8
				return "writeUtf8";
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
		return "writeInt";
	}

	return "";
}
#pragma endregion